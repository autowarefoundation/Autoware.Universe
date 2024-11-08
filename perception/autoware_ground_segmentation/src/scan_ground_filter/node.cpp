// Copyright 2021 Tier IV, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "node.hpp"

#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/math/normalization.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>
#include <autoware_vehicle_info_utils/vehicle_info_utils.hpp>

#include <memory>
#include <string>
#include <vector>

namespace autoware::ground_segmentation
{
using autoware::pointcloud_preprocessor::get_param;
using autoware::universe_utils::calcDistance3d;
using autoware::universe_utils::deg2rad;
using autoware::universe_utils::normalizeDegree;
using autoware::universe_utils::normalizeRadian;
using autoware::universe_utils::ScopedTimeTrack;
using autoware::vehicle_info_utils::VehicleInfoUtils;

ScanGroundFilterComponent::ScanGroundFilterComponent(const rclcpp::NodeOptions & options)
: autoware::pointcloud_preprocessor::Filter("ScanGroundFilter", options)
{
  // set initial parameters
  {
    // modes
    elevation_grid_mode_ = declare_parameter<bool>("elevation_grid_mode");

    // common parameters
    radial_divider_angle_rad_ =
      static_cast<float>(deg2rad(declare_parameter<double>("radial_divider_angle_deg")));
    radial_dividers_num_ = std::ceil(2.0 * M_PI / radial_divider_angle_rad_);

    // common thresholds
    global_slope_max_angle_rad_ =
      static_cast<float>(deg2rad(declare_parameter<double>("global_slope_max_angle_deg")));
    local_slope_max_angle_rad_ =
      static_cast<float>(deg2rad(declare_parameter<double>("local_slope_max_angle_deg")));
    global_slope_max_ratio_ = std::tan(global_slope_max_angle_rad_);
    local_slope_max_ratio_ = std::tan(local_slope_max_angle_rad_);
    split_points_distance_tolerance_ =
      static_cast<float>(declare_parameter<double>("split_points_distance_tolerance"));
    split_points_distance_tolerance_square_ =
      split_points_distance_tolerance_ * split_points_distance_tolerance_;

    // vehicle info
    vehicle_info_ = VehicleInfoUtils(*this).getVehicleInfo();

    // non-grid parameters
    use_virtual_ground_point_ = declare_parameter<bool>("use_virtual_ground_point");
    split_height_distance_ = static_cast<float>(declare_parameter<double>("split_height_distance"));

    // grid mode parameters
    use_recheck_ground_cluster_ = declare_parameter<bool>("use_recheck_ground_cluster");
    use_lowest_point_ = declare_parameter<bool>("use_lowest_point");
    detection_range_z_max_ = static_cast<float>(declare_parameter<double>("detection_range_z_max"));
    low_priority_region_x_ = static_cast<float>(declare_parameter<double>("low_priority_region_x"));
    center_pcl_shift_ = static_cast<float>(declare_parameter<double>("center_pcl_shift"));
    non_ground_height_threshold_ =
      static_cast<float>(declare_parameter<double>("non_ground_height_threshold"));

    // grid parameters
    grid_size_m_ = static_cast<float>(declare_parameter<double>("grid_size_m"));
    grid_mode_switch_radius_ =
      static_cast<float>(declare_parameter<double>("grid_mode_switch_radius"));
    gnd_grid_buffer_size_ = declare_parameter<int>("gnd_grid_buffer_size");
    virtual_lidar_z_ = vehicle_info_.vehicle_height_m;

    // initialize grid pointer
    {
      const float point_origin_x = vehicle_info_.wheel_base_m / 2.0f + center_pcl_shift_;
      const float point_origin_y = 0.0f;
      const float point_origin_z = virtual_lidar_z_;
      grid_ptr_ = std::make_unique<Grid>(point_origin_x, point_origin_y, point_origin_z);
      grid_ptr_->initialize(grid_size_m_, radial_divider_angle_rad_, grid_mode_switch_radius_);
    }

    // data access
    data_offset_initialized_ = false;
  }

  using std::placeholders::_1;
  set_param_res_ = this->add_on_set_parameters_callback(
    std::bind(&ScanGroundFilterComponent::onParameter, this, _1));

  // initialize debug tool
  {
    using autoware::universe_utils::DebugPublisher;
    using autoware::universe_utils::StopWatch;
    stop_watch_ptr_ = std::make_unique<StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ptr_ = std::make_unique<DebugPublisher>(this, "scan_ground_filter");
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");

    bool use_time_keeper = declare_parameter<bool>("publish_processing_time_detail");
    if (use_time_keeper) {
      detailed_processing_time_publisher_ =
        this->create_publisher<autoware::universe_utils::ProcessingTimeDetail>(
          "~/debug/processing_time_detail_ms", 1);
      auto time_keeper = autoware::universe_utils::TimeKeeper(detailed_processing_time_publisher_);
      time_keeper_ = std::make_shared<autoware::universe_utils::TimeKeeper>(time_keeper);

      // set time keeper to grid
      grid_ptr_->setTimeKeeper(time_keeper_);
    }
  }
}

inline void ScanGroundFilterComponent::set_field_index_offsets(const PointCloud2ConstPtr & input)
{
  data_offset_x_ = input->fields[pcl::getFieldIndex(*input, "x")].offset;
  data_offset_y_ = input->fields[pcl::getFieldIndex(*input, "y")].offset;
  data_offset_z_ = input->fields[pcl::getFieldIndex(*input, "z")].offset;
  int intensity_index = pcl::getFieldIndex(*input, "intensity");
  if (intensity_index != -1) {
    data_offset_intensity_ = input->fields[intensity_index].offset;
    intensity_type_ = input->fields[intensity_index].datatype;
  } else {
    data_offset_intensity_ = -1;
  }
  data_offset_initialized_ = true;
}

inline void ScanGroundFilterComponent::get_point_from_data_index(
  const PointCloud2ConstPtr & input, const size_t data_index, pcl::PointXYZ & point) const
{
  point.x = *reinterpret_cast<const float *>(&input->data[data_index + data_offset_x_]);
  point.y = *reinterpret_cast<const float *>(&input->data[data_index + data_offset_y_]);
  point.z = *reinterpret_cast<const float *>(&input->data[data_index + data_offset_z_]);
}

void ScanGroundFilterComponent::convertPointcloudGridScan(
  const PointCloud2ConstPtr & in_cloud,
  std::vector<PointCloudVector> & out_radial_ordered_points) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  out_radial_ordered_points.resize(radial_dividers_num_);
  PointData current_point;

  const size_t in_cloud_data_size = in_cloud->data.size();
  const size_t in_cloud_point_step = in_cloud->point_step;

  {  // grouping pointcloud by its azimuth angle
    std::unique_ptr<ScopedTimeTrack> inner_st_ptr;
    if (time_keeper_)
      inner_st_ptr = std::make_unique<ScopedTimeTrack>("convert_grid", *time_keeper_);

    // reset grid cells
    grid_ptr_->resetCells();

    pcl::PointXYZ input_point;
    for (size_t data_index = 0; data_index + in_cloud_point_step <= in_cloud_data_size;
         data_index += in_cloud_point_step) {
      // Get Point
      get_point_from_data_index(in_cloud, data_index, input_point);

      // [new grid] store the point to the new grid
      grid_ptr_->addPoint(input_point.x, input_point.y, data_index);
    }
  }

  {
    std::unique_ptr<ScopedTimeTrack> inner_st_ptr;
    if (time_keeper_)
      inner_st_ptr = std::make_unique<ScopedTimeTrack>("connect_grid", *time_keeper_);
    // [new grid] set grid connections and statistics
    grid_ptr_->setGridConnections();
    // grid_ptr_->setGridStatistics();
  }
}

void ScanGroundFilterComponent::convertPointcloud(
  const PointCloud2ConstPtr & in_cloud,
  std::vector<PointCloudVector> & out_radial_ordered_points) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  out_radial_ordered_points.resize(radial_dividers_num_);
  PointData current_point;

  const auto inv_radial_divider_angle_rad = 1.0f / radial_divider_angle_rad_;

  const size_t in_cloud_data_size = in_cloud->data.size();
  const size_t in_cloud_point_step = in_cloud->point_step;

  {  // grouping pointcloud by its azimuth angle
    std::unique_ptr<ScopedTimeTrack> inner_st_ptr;
    if (time_keeper_)
      inner_st_ptr = std::make_unique<ScopedTimeTrack>("azimuth_angle_grouping", *time_keeper_);

    pcl::PointXYZ input_point;
    for (size_t data_index = 0; data_index + in_cloud_point_step <= in_cloud_data_size;
         data_index += in_cloud_point_step) {
      // Get Point
      get_point_from_data_index(in_cloud, data_index, input_point);

      // determine the azimuth angle group
      auto radius{static_cast<float>(std::hypot(input_point.x, input_point.y))};
      auto theta{normalizeRadian(std::atan2(input_point.x, input_point.y), 0.0)};
      auto radial_div{static_cast<size_t>(std::floor(theta * inv_radial_divider_angle_rad))};

      current_point.radius = radius;
      current_point.point_state = PointLabel::INIT;
      current_point.data_index = data_index;

      // store the point in the corresponding radial division
      out_radial_ordered_points[radial_div].emplace_back(current_point);
    }
  }

  {  // sorting pointcloud by distance, on each azimuth angle group
    std::unique_ptr<ScopedTimeTrack> inner_st_ptr;
    if (time_keeper_) inner_st_ptr = std::make_unique<ScopedTimeTrack>("sort", *time_keeper_);

    for (size_t i = 0; i < radial_dividers_num_; ++i) {
      std::sort(
        out_radial_ordered_points[i].begin(), out_radial_ordered_points[i].end(),
        [](const PointData & a, const PointData & b) { return a.radius < b.radius; });
    }
  }
}

void ScanGroundFilterComponent::calcVirtualGroundOrigin(pcl::PointXYZ & point) const
{
  point.x = vehicle_info_.wheel_base_m;
  point.y = 0;
  point.z = 0;
}

// [new grid]
bool ScanGroundFilterComponent::recursiveSearch(
  const int check_idx, const int search_cnt, std::vector<int> & idx) const
{
  // recursive search
  if (check_idx < 0) {
    return false;
  }
  if (search_cnt == 0) {
    return true;
  }
  const auto & check_cell = grid_ptr_->getCell(check_idx);
  if (!check_cell.has_ground_) {
    // if the cell does not have ground, search previous cell
    return recursiveSearch(check_cell.prev_grid_idx_, search_cnt, idx);
  }
  // the cell has ground, add the index to the list, and search previous cell
  idx.push_back(check_idx);
  return recursiveSearch(check_cell.prev_grid_idx_, search_cnt - 1, idx);
}
void ScanGroundFilterComponent::fitLineFromGndGrid(
  const std::vector<int> & idx, float & a, float & b) const
{
  // if the idx is empty, the line is not defined
  if (idx.empty()) {
    a = 0.0f;
    b = 0.0f;
    return;
  }
  // if the idx is length of 1, the line is horizontal
  if (idx.size() == 1) {
    a = 0.0f;
    b = grid_ptr_->getCell(idx.front()).avg_height_;
    return;
  }
  // calculate the line by least square method
  float sum_x = 0.0f;
  float sum_y = 0.0f;
  float sum_xy = 0.0f;
  float sum_x2 = 0.0f;
  for (const auto & i : idx) {
    const auto & cell = grid_ptr_->getCell(i);
    sum_x += cell.avg_radius_;
    sum_y += cell.avg_height_;
    sum_xy += cell.avg_radius_ * cell.avg_height_;
    sum_x2 += cell.avg_radius_ * cell.avg_radius_;
  }
  const float n = static_cast<float>(idx.size());
  a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
  b = (sum_y - a * sum_x) / n;
}

void ScanGroundFilterComponent::classifyPointCloudGridScan(
  const PointCloud2ConstPtr & in_cloud,
  const std::vector<PointCloudVector> & /*in_radial_ordered_clouds*/,
  pcl::PointIndices & out_no_ground_indices) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // [new grid] run ground segmentation
  out_no_ground_indices.indices.clear();
  const auto grid_size = grid_ptr_->getGridSize();
  // loop over grid cells
  for (size_t idx = 0; idx < grid_size; idx++) {
    auto & cell = grid_ptr_->getCell(idx);
    if (cell.is_processed_) {
      continue;
    }
    // if the cell is empty, skip
    if (cell.getPointNum() == 0) {
      continue;
    }

    // iterate over points in the grid cell
    const auto num_points = static_cast<size_t>(cell.getPointNum());

    bool is_previous_initialized = false;
    // set a cell pointer for the previous cell
    const Cell * prev_cell_ptr;
    // check prev grid only exist
    if (cell.prev_grid_idx_ >= 0) {
      prev_cell_ptr = &(grid_ptr_->getCell(cell.prev_grid_idx_));
      if (prev_cell_ptr->is_ground_initialized_) {
        is_previous_initialized = true;
      }
    }

    if (!is_previous_initialized) {
      // if the previous cell is not processed or do not have ground,
      // try initialize ground in this cell
      bool is_ground_found = false;
      PointsCentroid ground_bin;

      for (size_t j = 0; j < num_points; ++j) {
        const auto & pt_idx = cell.point_indices_[j];
        pcl::PointXYZ point;
        get_point_from_data_index(in_cloud, pt_idx, point);
        const float radius = std::hypot(point.x, point.y);
        const float global_slope_ratio = point.z / radius;
        if (
          global_slope_ratio >= global_slope_max_ratio_ && point.z < non_ground_height_threshold_) {
          // this point is obstacle
          out_no_ground_indices.indices.push_back(pt_idx);
        } else if (
          abs(global_slope_ratio) < global_slope_max_ratio_ &&
          abs(point.z) < non_ground_height_threshold_) {
          // this point is ground
          ground_bin.addPoint(radius, point.z, pt_idx);
          is_ground_found = true;
        }
      }
      cell.is_processed_ = true;
      cell.has_ground_ = is_ground_found;
      if (is_ground_found) {
        cell.is_ground_initialized_ = true;
        ground_bin.processAverage();
        cell.avg_height_ = ground_bin.getAverageHeight();
        cell.avg_radius_ = ground_bin.getAverageRadius();
        cell.max_height_ = ground_bin.getMaxHeight();
        cell.min_height_ = ground_bin.getMinHeight();
        cell.gradient_ = cell.avg_height_ / cell.avg_radius_;
        cell.intercept_ = 0.0f;
      }

      // go to the next cell
      continue;
    } else {
      // inherit the previous cell information
      cell.is_ground_initialized_ = true;
      // continue to the ground segmentation
    }

    // get current cell gradient and intercept
    std::vector<int> grid_idcs;
    {
      const int search_count = gnd_grid_buffer_size_;
      int check_cell_idx = cell.prev_grid_idx_;
      recursiveSearch(check_cell_idx, search_count, grid_idcs);
      if (grid_idcs.size() > 0) {
        // calculate the gradient and intercept by least square method
        float a, b;
        fitLineFromGndGrid(grid_idcs, a, b);
        cell.gradient_ = a;
        cell.intercept_ = b;
      } else {
        // initialization failed. which should not happen. print error message
        std::cerr << "Failed to initialize ground grid at cell index: " << cell.grid_idx_
                  << std::endl;
      }
    }

    // segment the ground and non-ground points
    bool is_continuous = true;
    bool is_discontinuous = false;
    bool is_break = false;
    {
      const int front_radial_id =
        grid_ptr_->getCell(grid_idcs.front()).radial_idx_ + grid_idcs.size();
      const float radial_diff_between_cells = cell.center_radius_ - prev_cell_ptr->center_radius_;

      if (radial_diff_between_cells < gnd_grid_continual_thresh_ * cell.radial_size_) {
        if (cell.grid_idx_ - front_radial_id < gnd_grid_continual_thresh_) {
          is_continuous = true;
          is_discontinuous = false;
          is_break = false;
        } else {
          is_continuous = false;
          is_discontinuous = true;
          is_break = false;
        }
      } else {
        is_continuous = false;
        is_discontinuous = false;
        is_break = true;
      }
    }

    {
      PointsCentroid ground_bin;
      for (size_t j = 0; j < num_points; ++j) {
        const auto & pt_idx = cell.point_indices_[j];
        pcl::PointXYZ point;
        get_point_from_data_index(in_cloud, pt_idx, point);

        // 1. height is out-of-range
        if (point.z - prev_cell_ptr->avg_height_ > detection_range_z_max_) {
          // this point is out-of-range
          continue;
        }

        // 2. the angle is exceed the global slope threshold
        const float radius = std::hypot(point.x, point.y);
        const float global_slope_ratio = point.z / radius;
        if (global_slope_ratio > global_slope_max_ratio_) {
          // this point is obstacle
          out_no_ground_indices.indices.push_back(pt_idx);
          // go to the next point
          continue;
        }

        // 3. the point is continuous with the previous grid
        if (is_continuous) {
          // 3-a. local slope
          const float delta_z = point.z - prev_cell_ptr->avg_height_;
          const float delta_radius = radius - prev_cell_ptr->avg_radius_;
          const float local_slope_ratio = delta_z / delta_radius;
          if (abs(local_slope_ratio) < local_slope_max_ratio_) {
            // this point is ground
            ground_bin.addPoint(radius, point.z, pt_idx);
            // go to the next point
            continue;
          }

          // 3-b. mean of grid buffer(filtering)
          const float gradient =
            std::clamp(cell.gradient_, -global_slope_max_ratio_, global_slope_max_ratio_);
          const float next_gnd_z = gradient * radius + cell.intercept_;
          const float gnd_z_local_thresh =
            std::tan(DEG2RAD(5.0)) * (radius - prev_cell_ptr->avg_radius_);
          if (abs(point.z - next_gnd_z) <= non_ground_height_threshold_ + gnd_z_local_thresh) {
            // this point is ground
            ground_bin.addPoint(radius, point.z, pt_idx);
            // go to the next point
            continue;
          }

          // 3-c. the point is non-ground
          if (point.z - next_gnd_z >= non_ground_height_threshold_ + gnd_z_local_thresh) {
            // this point is obstacle
            out_no_ground_indices.indices.push_back(pt_idx);
            // go to the next point
            continue;
          }
        }

        // 4. the point is discontinuous with the previous grid
        if (is_discontinuous) {
          const float delta_avg_z = point.z - prev_cell_ptr->avg_height_;
          if (abs(delta_avg_z) < non_ground_height_threshold_) {
            // this point is ground
            ground_bin.addPoint(radius, point.z, pt_idx);
            // go to the next point
            continue;
          }
          const float delta_max_z = point.z - prev_cell_ptr->max_height_;
          if (abs(delta_max_z) < non_ground_height_threshold_) {
            // this point is ground
            ground_bin.addPoint(radius, point.z, pt_idx);
            // go to the next point
            continue;
          }
          const float delta_radius = radius - prev_cell_ptr->avg_radius_;
          const float local_slope_ratio = delta_avg_z / delta_radius;
          if (abs(local_slope_ratio) < local_slope_max_ratio_) {
            // this point is ground
            ground_bin.addPoint(radius, point.z, pt_idx);
            // go to the next point
            continue;
          }
          if (local_slope_ratio >= local_slope_max_ratio_) {
            // this point is obstacle
            out_no_ground_indices.indices.push_back(pt_idx);
            // go to the next point
            continue;
          }
        }

        // 5. the point is break the previous grid
        if (is_break) {
          const float delta_avg_z = point.z - prev_cell_ptr->avg_height_;
          const float delta_radius = radius - prev_cell_ptr->avg_radius_;
          const float local_slope_ratio = delta_avg_z / delta_radius;
          if (abs(local_slope_ratio) < global_slope_max_ratio_) {
            // this point is ground
            ground_bin.addPoint(radius, point.z, pt_idx);
            // go to the next point
            continue;
          }
          if (local_slope_ratio >= global_slope_max_ratio_) {
            // this point is obstacle
            out_no_ground_indices.indices.push_back(pt_idx);
            // go to the next point
            continue;
          }
        }
      }

      // recheck ground bin
      if (ground_bin.getIndicesRef().size() > 0 && use_recheck_ground_cluster_) {
        ground_bin.processAverage();
        // recheck the ground cluster
        const float reference_height =
          use_lowest_point_ ? ground_bin.getMinHeight() : ground_bin.getAverageHeight();
        const std::vector<size_t> & gnd_indices = ground_bin.getIndicesRef();
        const std::vector<float> & height_list = ground_bin.getHeightListRef();
        for (size_t j = 0; j < height_list.size(); ++j) {
          if (height_list.at(j) >= reference_height + non_ground_height_threshold_) {
            // fill the non-ground indices
            out_no_ground_indices.indices.push_back(gnd_indices.at(j));
            // remove the point from the ground bin
            // ground_bin.removePoint(j);
          }
        }
      }

      // finalize current cell, update the cell ground information
      if (ground_bin.getIndicesRef().size() > 0) {
        ground_bin.processAverage();
        cell.avg_height_ = ground_bin.getAverageHeight();
        cell.avg_radius_ = ground_bin.getAverageRadius();
        cell.max_height_ = ground_bin.getMaxHeight();
        cell.min_height_ = ground_bin.getMinHeight();
        cell.has_ground_ = true;
      }
      cell.is_processed_ = true;
    }

    // // debug: cell info for all non-empty cells
    // std::cout << "cell index: " << cell.grid_idx_
    //            <<  " number of points: " << cell.getPointNum()
    //             << " has ground: " << cell.has_ground_
    //             << " avg height: " << cell.avg_height_ << " avg radius: " << cell.avg_radius_
    //             << " gradient: " << cell.gradient_ << " intercept: " << cell.intercept_
    //             << std::endl;
  }
}

void ScanGroundFilterComponent::classifyPointCloud(
  const PointCloud2ConstPtr & in_cloud,
  const std::vector<PointCloudVector> & in_radial_ordered_clouds,
  pcl::PointIndices & out_no_ground_indices) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  out_no_ground_indices.indices.clear();

  const pcl::PointXYZ init_ground_point(0, 0, 0);
  pcl::PointXYZ virtual_ground_point(0, 0, 0);
  calcVirtualGroundOrigin(virtual_ground_point);

  // run the classification algorithm for each ray (azimuth division)
  for (size_t i = 0; i < in_radial_ordered_clouds.size(); ++i) {
    float prev_gnd_radius = 0.0f;
    float prev_gnd_slope = 0.0f;
    PointsCentroid ground_cluster, non_ground_cluster;
    PointLabel point_label_curr = PointLabel::INIT;

    pcl::PointXYZ prev_gnd_point(0, 0, 0), point_curr, point_prev;

    // iterate over the points in the ray
    for (size_t j = 0; j < in_radial_ordered_clouds[i].size(); ++j) {
      float points_distance = 0.0f;
      const float local_slope_max_angle = local_slope_max_angle_rad_;

      // set the previous point
      point_prev = point_curr;
      PointLabel point_label_prev = point_label_curr;

      // set the current point
      const PointData & pd = in_radial_ordered_clouds[i][j];
      point_label_curr = pd.point_state;

      get_point_from_data_index(in_cloud, pd.data_index, point_curr);
      if (j == 0) {
        bool is_front_side = (point_curr.x > virtual_ground_point.x);
        if (use_virtual_ground_point_ && is_front_side) {
          prev_gnd_point = virtual_ground_point;
        } else {
          prev_gnd_point = init_ground_point;
        }
        prev_gnd_radius = std::hypot(prev_gnd_point.x, prev_gnd_point.y);
        prev_gnd_slope = 0.0f;
        ground_cluster.initialize();
        non_ground_cluster.initialize();
        points_distance = calcDistance3d(point_curr, prev_gnd_point);
      } else {
        points_distance = calcDistance3d(point_curr, point_prev);
      }

      float radius_distance_from_gnd = pd.radius - prev_gnd_radius;
      float height_from_gnd = point_curr.z - prev_gnd_point.z;
      float height_from_obj = point_curr.z - non_ground_cluster.getAverageHeight();
      bool calculate_slope = false;
      bool is_point_close_to_prev =
        (points_distance <
         (pd.radius * radial_divider_angle_rad_ + split_points_distance_tolerance_));

      float global_slope_ratio = point_curr.z / pd.radius;
      // check points which is far enough from previous point
      if (global_slope_ratio > global_slope_max_ratio_) {
        point_label_curr = PointLabel::NON_GROUND;
        calculate_slope = false;
      } else if (
        (point_label_prev == PointLabel::NON_GROUND) &&
        (std::abs(height_from_obj) >= split_height_distance_)) {
        calculate_slope = true;
      } else if (is_point_close_to_prev && std::abs(height_from_gnd) < split_height_distance_) {
        // close to the previous point, set point follow label
        point_label_curr = PointLabel::POINT_FOLLOW;
        calculate_slope = false;
      } else {
        calculate_slope = true;
      }
      if (is_point_close_to_prev) {
        height_from_gnd = point_curr.z - ground_cluster.getAverageHeight();
        radius_distance_from_gnd = pd.radius - ground_cluster.getAverageRadius();
      }
      if (calculate_slope) {
        // far from the previous point
        auto local_slope = std::atan2(height_from_gnd, radius_distance_from_gnd);
        if (local_slope - prev_gnd_slope > local_slope_max_angle) {
          // the point is outside of the local slope threshold
          point_label_curr = PointLabel::NON_GROUND;
        } else {
          point_label_curr = PointLabel::GROUND;
        }
      }

      if (point_label_curr == PointLabel::GROUND) {
        ground_cluster.initialize();
        non_ground_cluster.initialize();
      }
      if (point_label_curr == PointLabel::NON_GROUND) {
        out_no_ground_indices.indices.push_back(pd.data_index);
      } else if (  // NOLINT
        (point_label_prev == PointLabel::NON_GROUND) &&
        (point_label_curr == PointLabel::POINT_FOLLOW)) {
        point_label_curr = PointLabel::NON_GROUND;
        out_no_ground_indices.indices.push_back(pd.data_index);
      } else if (  // NOLINT
        (point_label_prev == PointLabel::GROUND) &&
        (point_label_curr == PointLabel::POINT_FOLLOW)) {
        point_label_curr = PointLabel::GROUND;
      } else {
      }

      // update the ground state
      if (point_label_curr == PointLabel::GROUND) {
        prev_gnd_radius = pd.radius;
        prev_gnd_point = pcl::PointXYZ(point_curr.x, point_curr.y, point_curr.z);
        ground_cluster.addPoint(pd.radius, point_curr.z);
        prev_gnd_slope = ground_cluster.getAverageSlope();
      }
      // update the non ground state
      if (point_label_curr == PointLabel::NON_GROUND) {
        non_ground_cluster.addPoint(pd.radius, point_curr.z);
      }
    }
  }
}

void ScanGroundFilterComponent::extractObjectPoints(
  const PointCloud2ConstPtr & in_cloud_ptr, const pcl::PointIndices & in_indices,
  PointCloud2 & out_object_cloud) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  size_t output_data_size = 0;

  for (const auto & idx : in_indices.indices) {
    std::memcpy(
      &out_object_cloud.data[output_data_size], &in_cloud_ptr->data[idx],
      in_cloud_ptr->point_step * sizeof(uint8_t));
    output_data_size += in_cloud_ptr->point_step;
  }
}

void ScanGroundFilterComponent::faster_filter(
  const PointCloud2ConstPtr & input, [[maybe_unused]] const IndicesPtr & indices,
  PointCloud2 & output,
  [[maybe_unused]] const autoware::pointcloud_preprocessor::TransformInfo & transform_info)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  std::scoped_lock lock(mutex_);
  stop_watch_ptr_->toc("processing_time", true);

  if (!data_offset_initialized_) {
    set_field_index_offsets(input);
  }

  pcl::PointIndices no_ground_indices;

  if (elevation_grid_mode_) {
    std::vector<PointCloudVector> radial_ordered_points;
    convertPointcloudGridScan(input, radial_ordered_points);
    classifyPointCloudGridScan(input, radial_ordered_points, no_ground_indices);
  } else {
    std::vector<PointCloudVector> radial_ordered_points;
    convertPointcloud(input, radial_ordered_points);
    classifyPointCloud(input, radial_ordered_points, no_ground_indices);
  }
  output.row_step = no_ground_indices.indices.size() * input->point_step;
  output.data.resize(output.row_step);
  output.width = no_ground_indices.indices.size();
  output.fields = input->fields;
  output.is_dense = true;
  output.height = input->height;
  output.is_bigendian = input->is_bigendian;
  output.point_step = input->point_step;
  output.header = input->header;

  extractObjectPoints(input, no_ground_indices, output);
  if (debug_publisher_ptr_ && stop_watch_ptr_) {
    const double cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    const double processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    debug_publisher_ptr_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    debug_publisher_ptr_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);
  }
}

// TODO(taisa1): Temporary Implementation: Delete this function definition when all the filter
// nodes conform to new API.
void ScanGroundFilterComponent::filter(
  const PointCloud2ConstPtr & input, [[maybe_unused]] const IndicesPtr & indices,
  PointCloud2 & output)
{
  (void)input;
  (void)indices;
  (void)output;
}

rcl_interfaces::msg::SetParametersResult ScanGroundFilterComponent::onParameter(
  const std::vector<rclcpp::Parameter> & param)
{
  if (get_param(param, "grid_size_m", grid_size_m_)) {
    grid_ptr_->initialize(grid_size_m_, radial_divider_angle_rad_, grid_mode_switch_radius_);
  }
  if (get_param(param, "grid_mode_switch_radius", grid_mode_switch_radius_)) {
    grid_ptr_->initialize(grid_size_m_, radial_divider_angle_rad_, grid_mode_switch_radius_);
  }
  double global_slope_max_angle_deg{get_parameter("global_slope_max_angle_deg").as_double()};
  if (get_param(param, "global_slope_max_angle_deg", global_slope_max_angle_deg)) {
    global_slope_max_angle_rad_ = deg2rad(global_slope_max_angle_deg);
    global_slope_max_ratio_ = std::tan(global_slope_max_angle_rad_);
    RCLCPP_DEBUG(
      get_logger(), "Setting global_slope_max_angle_rad to: %f.", global_slope_max_angle_rad_);
    RCLCPP_DEBUG(get_logger(), "Setting global_slope_max_ratio to: %f.", global_slope_max_ratio_);
  }
  double local_slope_max_angle_deg{get_parameter("local_slope_max_angle_deg").as_double()};
  if (get_param(param, "local_slope_max_angle_deg", local_slope_max_angle_deg)) {
    local_slope_max_angle_rad_ = deg2rad(local_slope_max_angle_deg);
    local_slope_max_ratio_ = std::tan(local_slope_max_angle_rad_);
    RCLCPP_DEBUG(
      get_logger(), "Setting local_slope_max_angle_rad to: %f.", local_slope_max_angle_rad_);
    RCLCPP_DEBUG(get_logger(), "Setting local_slope_max_ratio to: %f.", local_slope_max_ratio_);
  }
  double radial_divider_angle_deg{get_parameter("radial_divider_angle_deg").as_double()};
  if (get_param(param, "radial_divider_angle_deg", radial_divider_angle_deg)) {
    radial_divider_angle_rad_ = deg2rad(radial_divider_angle_deg);
    radial_dividers_num_ = std::ceil(2.0 * M_PI / radial_divider_angle_rad_);
    grid_ptr_->initialize(grid_size_m_, radial_divider_angle_rad_, grid_mode_switch_radius_);
    RCLCPP_DEBUG(
      get_logger(), "Setting radial_divider_angle_rad to: %f.", radial_divider_angle_rad_);
    RCLCPP_DEBUG(get_logger(), "Setting radial_dividers_num to: %zu.", radial_dividers_num_);
  }
  if (get_param(param, "split_points_distance_tolerance", split_points_distance_tolerance_)) {
    split_points_distance_tolerance_square_ =
      split_points_distance_tolerance_ * split_points_distance_tolerance_;
    RCLCPP_DEBUG(
      get_logger(), "Setting split_points_distance_tolerance to: %f.",
      split_points_distance_tolerance_);
    RCLCPP_DEBUG(
      get_logger(), "Setting split_points_distance_tolerance_square to: %f.",
      split_points_distance_tolerance_square_);
  }
  if (get_param(param, "split_height_distance", split_height_distance_)) {
    RCLCPP_DEBUG(get_logger(), "Setting split_height_distance to: %f.", split_height_distance_);
  }
  if (get_param(param, "use_virtual_ground_point", use_virtual_ground_point_)) {
    RCLCPP_DEBUG_STREAM(
      get_logger(),
      "Setting use_virtual_ground_point to: " << std::boolalpha << use_virtual_ground_point_);
  }
  if (get_param(param, "use_recheck_ground_cluster", use_recheck_ground_cluster_)) {
    RCLCPP_DEBUG_STREAM(
      get_logger(),
      "Setting use_recheck_ground_cluster to: " << std::boolalpha << use_recheck_ground_cluster_);
  }
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;
  result.reason = "success";

  return result;
}

}  // namespace autoware::ground_segmentation

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::ground_segmentation::ScanGroundFilterComponent)

// Copyright 2024 TIER IV, Inc.
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

#include "grid_ground_filter.hpp"

#include "data.hpp"

#include <pcl/PointIndices.h>

#include <memory>
#include <vector>

namespace autoware::ground_segmentation
{

// assign the pointcloud data to the grid
void GridGroundFilter::convert()
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  const size_t in_cloud_data_size = in_cloud_->data.size();
  const size_t in_cloud_point_step = in_cloud_->point_step;

  for (size_t data_index = 0; data_index + in_cloud_point_step <= in_cloud_data_size;
       data_index += in_cloud_point_step) {
    // Get Point
    pcl::PointXYZ input_point;
    data_accessor_.getPoint(in_cloud_, data_index, input_point);
    grid_ptr_->addPoint(input_point.x, input_point.y, input_point.z, data_index);
  }
}

// preprocess the grid data, set the grid connections
void GridGroundFilter::preprocess()
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // eliminate empty cells from connection for efficiency
  grid_ptr_->setGridConnections();
}

// recursive search for the ground grid cell close to the grid origin
bool GridGroundFilter::recursiveSearch(
  const int check_idx, const int search_cnt, std::vector<int> & idx) const
{
  // set the maximum search count
  constexpr size_t count_limit = 1023;
  return recursiveSearch(check_idx, search_cnt, idx, count_limit);
}

bool GridGroundFilter::recursiveSearch(
  const int check_idx, const int search_cnt, std::vector<int> & idx, size_t count) const
{
  if (count == 0) {
    return false;
  }
  count -= 1;
  // recursive search
  if (check_idx < 0) {
    return false;
  }
  if (search_cnt == 0) {
    return true;
  }
  const auto & check_cell = grid_ptr_->getCell(check_idx);
  if (check_cell.has_ground_) {
    // the cell has ground, add the index to the list, and search previous cell
    idx.push_back(check_idx);
    return recursiveSearch(check_cell.scan_grid_root_idx_, search_cnt - 1, idx, count);
  }
  // if the cell does not have ground, search previous cell
  return recursiveSearch(check_cell.scan_grid_root_idx_, search_cnt, idx, count);
}

// fit the line from the ground grid cells
void GridGroundFilter::fitLineFromGndGrid(const std::vector<int> & idx, float & a, float & b) const
{
  // if the idx is empty, the line is not defined
  if (idx.empty()) {
    a = 0.0f;
    b = 0.0f;
    return;
  }
  // if the idx is length of 1, the line is zero-crossing line
  if (idx.size() == 1) {
    const auto & cell = grid_ptr_->getCell(idx.front());
    a = cell.avg_height_ / cell.avg_radius_;
    b = 0.0f;
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
  const float denominator = n * sum_x2 - sum_x * sum_x;
  if (denominator != 0.0f) {
    a = (n * sum_xy - sum_x * sum_y) / denominator;
    a = std::clamp(a, -param_.global_slope_max_ratio, param_.global_slope_max_ratio);
    b = (sum_y - a * sum_x) / n;
  } else {
    const auto & cell = grid_ptr_->getCell(idx.front());
    a = cell.avg_height_ / cell.avg_radius_;
    b = 0.0f;
  }
}

// process the grid data to initialize the ground cells prior to the ground segmentation
void GridGroundFilter::initializeGround(pcl::PointIndices & out_no_ground_indices)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  const auto grid_size = grid_ptr_->getGridSize();
  // loop over grid cells
  for (size_t idx = 0; idx < grid_size; idx++) {
    auto & cell = grid_ptr_->getCell(idx);
    if (cell.is_ground_initialized_) continue;
    // if the cell is empty, skip
    if (cell.isEmpty()) continue;

    // check scan root grid
    if (cell.scan_grid_root_idx_ >= 0) {
      const Cell & prev_cell = grid_ptr_->getCell(cell.scan_grid_root_idx_);
      if (prev_cell.is_ground_initialized_) {
        cell.is_ground_initialized_ = true;
        continue;
      }
    }

    // initialize ground in this cell
    bool is_ground_found = false;
    PointsCentroid ground_bin;

    for (const auto & pt : cell.point_list_) {
      const size_t & pt_idx = pt.index;
      const float & radius = pt.distance;
      const float & height = pt.height;

      const float global_slope_threshold = param_.global_slope_max_ratio * radius;
      if (height >= global_slope_threshold && height > param_.non_ground_height_threshold) {
        // this point is obstacle
        out_no_ground_indices.indices.push_back(pt_idx);
      } else if (
        abs(height) < global_slope_threshold && abs(height) < param_.non_ground_height_threshold) {
        // this point is ground
        ground_bin.addPoint(radius, height, pt_idx);
        is_ground_found = true;
      }
      // else, this point is not classified, not ground nor obstacle
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
      cell.gradient_ = std::clamp(
        cell.avg_height_ / cell.avg_radius_, -param_.global_slope_max_ratio,
        param_.global_slope_max_ratio);
      cell.intercept_ = 0.0f;
    } else {
      cell.is_ground_initialized_ = false;
    }
  }
}

// segment the point in the cell, logic for the continuous cell
void GridGroundFilter::SegmentContinuousCell(
  const Cell & cell, PointsCentroid & ground_bin, pcl::PointIndices & out_no_ground_indices)
{
  const Cell & prev_cell = grid_ptr_->getCell(cell.scan_grid_root_idx_);
  const float local_thresh_angle_ratio = std::tan(DEG2RAD(5.0));

  // loop over points in the cell
  for (const auto & pt : cell.point_list_) {
    const size_t & pt_idx = pt.index;
    const float & radius = pt.distance;
    const float & height = pt.height;

    // 1. height is out-of-range
    const float delta_z = height - prev_cell.avg_height_;
    if (delta_z > param_.detection_range_z_max) {
      // this point is out-of-range
      continue;
    }

    // 2. the angle is exceed the global slope threshold
    if (height > param_.global_slope_max_ratio * radius) {
      // this point is obstacle
      out_no_ground_indices.indices.push_back(pt_idx);
      // go to the next point
      continue;
    }

    // 3. local slope
    const float delta_radius = radius - prev_cell.avg_radius_;
    if (abs(delta_z) < param_.global_slope_max_ratio * delta_radius) {
      // this point is ground
      ground_bin.addPoint(radius, height, pt_idx);
      // go to the next point
      continue;
    }

    // 3. height from the estimated ground
    const float next_gnd_z = cell.gradient_ * radius + cell.intercept_;
    const float gnd_z_local_thresh = local_thresh_angle_ratio * delta_radius;
    const float delta_gnd_z = height - next_gnd_z;
    const float gnd_z_threshold = param_.non_ground_height_threshold + gnd_z_local_thresh;
    if (delta_gnd_z > gnd_z_threshold) {
      // this point is obstacle
      out_no_ground_indices.indices.push_back(pt_idx);
      // go to the next point
      continue;
    }
    if (abs(delta_gnd_z) <= gnd_z_threshold) {
      // this point is ground
      ground_bin.addPoint(radius, height, pt_idx);
      // go to the next point
      continue;
    }
    // else, this point is not classified, not ground nor obstacle
  }
}

// segment the point in the cell, logic for the discontinuous cell
void GridGroundFilter::SegmentDiscontinuousCell(
  const Cell & cell, PointsCentroid & ground_bin, pcl::PointIndices & out_no_ground_indices)
{
  const Cell & prev_cell = grid_ptr_->getCell(cell.scan_grid_root_idx_);

  // loop over points in the cell
  for (const auto & pt : cell.point_list_) {
    const size_t & pt_idx = pt.index;
    const float & radius = pt.distance;
    const float & height = pt.height;

    // 1. height is out-of-range
    const float delta_avg_z = height - prev_cell.avg_height_;
    if (delta_avg_z > param_.detection_range_z_max) {
      // this point is out-of-range
      continue;
    }

    // 2. the angle is exceed the global slope threshold
    if (height > param_.global_slope_max_ratio * radius) {
      // this point is obstacle
      out_no_ground_indices.indices.push_back(pt_idx);
      // go to the next point
      continue;
    }
    // 3. local slope
    const float delta_radius = radius - prev_cell.avg_radius_;
    const float global_slope_threshold = param_.global_slope_max_ratio * delta_radius;
    if (abs(delta_avg_z) < global_slope_threshold) {
      // this point is ground
      ground_bin.addPoint(radius, height, pt_idx);
      // go to the next point
      continue;
    }
    // 4. height from the estimated ground
    if (abs(delta_avg_z) < param_.non_ground_height_threshold) {
      // this point is ground
      ground_bin.addPoint(radius, height, pt_idx);
      // go to the next point
      continue;
    }
    const float delta_max_z = height - prev_cell.max_height_;
    if (abs(delta_max_z) < param_.non_ground_height_threshold) {
      // this point is ground
      ground_bin.addPoint(radius, height, pt_idx);
      // go to the next point
      continue;
    }
    // 5. obstacle from local slope
    if (delta_avg_z >= global_slope_threshold) {
      // this point is obstacle
      out_no_ground_indices.indices.push_back(pt_idx);
      // go to the next point
      continue;
    }
    // else, this point is not classified, not ground nor obstacle
  }
}

// segment the point in the cell, logic for the break cell
void GridGroundFilter::SegmentBreakCell(
  const Cell & cell, PointsCentroid & ground_bin, pcl::PointIndices & out_no_ground_indices)
{
  const Cell & prev_cell = grid_ptr_->getCell(cell.scan_grid_root_idx_);

  // loop over points in the cell
  for (const auto & pt : cell.point_list_) {
    const size_t & pt_idx = pt.index;
    const float & radius = pt.distance;
    const float & height = pt.height;

    // 1. height is out-of-range
    const float delta_z = height - prev_cell.avg_height_;
    if (delta_z > param_.detection_range_z_max) {
      // this point is out-of-range
      continue;
    }

    // 2. the angle is exceed the global slope threshold
    if (height > param_.global_slope_max_ratio * radius) {
      // this point is obstacle
      out_no_ground_indices.indices.push_back(pt_idx);
      // go to the next point
      continue;
    }

    // 3. the point is over discontinuous grid
    const float delta_radius = radius - prev_cell.avg_radius_;
    const float global_slope_threshold = param_.global_slope_max_ratio * delta_radius;
    if (abs(delta_z) < global_slope_threshold) {
      // this point is ground
      ground_bin.addPoint(radius, height, pt_idx);
      // go to the next point
      continue;
    }
    if (delta_z >= global_slope_threshold) {
      // this point is obstacle
      out_no_ground_indices.indices.push_back(pt_idx);
      // go to the next point
      continue;
    }
    // else, this point is not classified, not ground nor obstacle
  }
}

// classify the point cloud into ground and non-ground points
void GridGroundFilter::classify(pcl::PointIndices & out_no_ground_indices)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // loop over grid cells
  const auto grid_size = grid_ptr_->getGridSize();
  for (size_t idx = 0; idx < grid_size; idx++) {
    auto & cell = grid_ptr_->getCell(idx);
    // if the cell is empty, skip
    if (cell.isEmpty()) continue;
    if (cell.is_processed_) continue;

    // set a cell pointer for the previous cell
    // check scan root grid
    if (cell.scan_grid_root_idx_ < 0) continue;
    const Cell & prev_cell = grid_ptr_->getCell(cell.scan_grid_root_idx_);
    if (!(prev_cell.is_ground_initialized_)) continue;

    // get current cell gradient and intercept
    std::vector<int> grid_idcs;
    {
      const int search_count = param_.gnd_grid_buffer_size;
      const int check_cell_idx = cell.scan_grid_root_idx_;
      recursiveSearch(check_cell_idx, search_count, grid_idcs);
    }

    // segment the ground and non-ground points
    enum SegmentationMode { NONE, CONTINUOUS, DISCONTINUOUS, BREAK };
    SegmentationMode mode = SegmentationMode::NONE;
    {
      const int front_radial_id =
        grid_ptr_->getCell(grid_idcs.back()).radial_idx_ + grid_idcs.size();
      const float radial_diff_between_cells = cell.center_radius_ - prev_cell.center_radius_;

      if (radial_diff_between_cells < param_.gnd_grid_continual_thresh * cell.radial_size_) {
        if (cell.radial_idx_ - front_radial_id < param_.gnd_grid_continual_thresh) {
          mode = SegmentationMode::CONTINUOUS;
        } else {
          mode = SegmentationMode::DISCONTINUOUS;
        }
      } else {
        mode = SegmentationMode::BREAK;
      }
    }

    {
      PointsCentroid ground_bin;
      if (mode == SegmentationMode::CONTINUOUS) {
        // calculate the gradient and intercept by least square method
        float a, b;
        fitLineFromGndGrid(grid_idcs, a, b);
        cell.gradient_ = a;
        cell.intercept_ = b;

        SegmentContinuousCell(cell, ground_bin, out_no_ground_indices);
      } else if (mode == SegmentationMode::DISCONTINUOUS) {
        SegmentDiscontinuousCell(cell, ground_bin, out_no_ground_indices);
      } else if (mode == SegmentationMode::BREAK) {
        SegmentBreakCell(cell, ground_bin, out_no_ground_indices);
      }

      // recheck ground bin
      if (
        param_.use_recheck_ground_cluster && cell.avg_radius_ > param_.grid_mode_switch_radius &&
        ground_bin.getGroundPointNum() > 0) {
        // recheck the ground cluster
        float reference_height = 0;
        if (param_.use_lowest_point) {
          reference_height = ground_bin.getMinHeightOnly();
        } else {
          ground_bin.processAverage();
          reference_height = ground_bin.getAverageHeight();
        }
        const float threshold = reference_height + param_.non_ground_height_threshold;
        const std::vector<size_t> & gnd_indices = ground_bin.getIndicesRef();
        const std::vector<float> & height_list = ground_bin.getHeightListRef();
        for (size_t j = 0; j < height_list.size(); ++j) {
          if (height_list.at(j) >= threshold) {
            // fill the non-ground indices
            out_no_ground_indices.indices.push_back(gnd_indices.at(j));
            // mark the point as non-ground
            ground_bin.is_ground_list.at(j) = false;
          }
        }
      }

      // finalize current cell, update the cell ground information
      if (ground_bin.getGroundPointNum() > 0) {
        ground_bin.processAverage();
        cell.avg_height_ = ground_bin.getAverageHeight();
        cell.avg_radius_ = ground_bin.getAverageRadius();
        cell.max_height_ = ground_bin.getMaxHeight();
        cell.min_height_ = ground_bin.getMinHeight();
        cell.has_ground_ = true;
      } else {
        // copy previous cell
        cell.avg_radius_ = prev_cell.avg_radius_;
        cell.avg_height_ = prev_cell.avg_height_;
        cell.max_height_ = prev_cell.max_height_;
        cell.min_height_ = prev_cell.min_height_;
        cell.has_ground_ = false;
      }

      cell.is_processed_ = true;
    }
  }
}

// process the point cloud to segment the ground points
void GridGroundFilter::process(
  const PointCloud2ConstPtr & in_cloud, pcl::PointIndices & out_no_ground_indices)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // set input cloud
  in_cloud_ = in_cloud;

  // clear the output indices
  out_no_ground_indices.indices.clear();

  // reset grid cells
  grid_ptr_->resetCells();

  // 1. assign points to grid cells
  convert();

  // 2. cell preprocess
  preprocess();

  // 3. initialize ground
  initializeGround(out_no_ground_indices);

  // 4. classify point cloud
  classify(out_no_ground_indices);
}

}  // namespace autoware::ground_segmentation

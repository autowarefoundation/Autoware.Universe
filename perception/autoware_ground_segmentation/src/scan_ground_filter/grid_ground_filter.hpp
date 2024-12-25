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

#ifndef SCAN_GROUND_FILTER__GRID_GROUND_FILTER_HPP_
#define SCAN_GROUND_FILTER__GRID_GROUND_FILTER_HPP_

#include "data.hpp"
#include "grid.hpp"

#include <autoware/universe_utils/system/time_keeper.hpp>
#include <pcl/impl/point_types.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl/PointIndices.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

namespace autoware::ground_segmentation
{
using PointCloud2ConstPtr = sensor_msgs::msg::PointCloud2::ConstSharedPtr;

struct PointsCentroid
{
  float radius_avg;
  float height_avg;
  float height_max;
  float height_min;
  uint16_t grid_id;
  std::vector<size_t> pcl_indices;
  std::vector<float> height_list;
  std::vector<float> radius_list;
  std::vector<bool> is_ground_list;

  PointsCentroid()
  : radius_avg(0.0f), height_avg(0.0f), height_max(-10.0f), height_min(10.0f), grid_id(0)
  {
  }

  void initialize()
  {
    radius_avg = 0.0f;
    height_avg = 0.0f;
    height_max = -10.0f;
    height_min = 10.0f;
    grid_id = 0;
    pcl_indices.clear();
    height_list.clear();
    radius_list.clear();
    is_ground_list.clear();
  }

  inline void addPoint(const float radius, const float height, const size_t index)
  {
    pcl_indices.push_back(index);
    height_list.push_back(height);
    radius_list.push_back(radius);
    is_ground_list.push_back(true);
  }

  int getGroundPointNum() const
  {
    return std::count(is_ground_list.begin(), is_ground_list.end(), true);
  }

  void processAverage()
  {
    // process only if is_ground_list is true
    const int ground_point_num = getGroundPointNum();
    if (ground_point_num == 0) {
      return;
    }

    float radius_sum = 0.0f;
    float height_sum = 0.0f;
    height_max = -10.0f;
    height_min = 10.0f;

    for (size_t i = 0; i < is_ground_list.size(); ++i) {
      if (!is_ground_list[i]) {
        continue;
      }
      radius_sum += radius_list[i];
      height_sum += height_list[i];
      height_max = std::max(height_max, height_list[i]);
      height_min = std::min(height_min, height_list[i]);
    }

    radius_avg = radius_sum / ground_point_num;
    height_avg = height_sum / ground_point_num;
  }

  float getMinHeightOnly() const
  {
    float min_height = 10.0f;
    for (size_t i = 0; i < is_ground_list.size(); ++i) {
      if (!is_ground_list[i]) {
        continue;
      }
      min_height = std::min(min_height, height_list[i]);
    }
    return min_height;
  }

  float getAverageSlope() const { return std::atan2(height_avg, radius_avg); }
  float getAverageHeight() const { return height_avg; }
  float getAverageRadius() const { return radius_avg; }
  float getMaxHeight() const { return height_max; }
  float getMinHeight() const { return height_min; }
  const std::vector<size_t> & getIndicesRef() const { return pcl_indices; }
  const std::vector<float> & getHeightListRef() const { return height_list; }
};

struct GridGroundFilterParameter
{
  // parameters
  float global_slope_max_angle_rad;
  float local_slope_max_angle_rad;
  float global_slope_max_ratio;
  float local_slope_max_ratio;
  float radial_divider_angle_rad;
  size_t radial_dividers_num;

  bool use_recheck_ground_cluster;
  bool use_lowest_point;
  float detection_range_z_max;
  float non_ground_height_threshold;
  const uint16_t gnd_grid_continual_thresh = 3;

  float grid_size_m;
  float grid_mode_switch_radius;
  int gnd_grid_buffer_size;
  float virtual_lidar_x;
  float virtual_lidar_y;
  float virtual_lidar_z;
};

class GridGroundFilter
{
public:
  explicit GridGroundFilter(GridGroundFilterParameter & param) : param_(param)
  {
    // calculate derived parameters
    param_.global_slope_max_ratio = std::tan(param_.global_slope_max_angle_rad);
    param_.local_slope_max_ratio = std::tan(param_.local_slope_max_angle_rad);
    param_.radial_dividers_num = std::ceil(2.0 * M_PI / param_.radial_divider_angle_rad);

    // initialize grid pointer
    grid_ptr_ = std::make_unique<Grid>(
      param_.virtual_lidar_x, param_.virtual_lidar_y, param_.virtual_lidar_z);
    grid_ptr_->initialize(
      param_.grid_size_m, param_.radial_divider_angle_rad, param_.grid_mode_switch_radius);
  }
  ~GridGroundFilter() = default;

  void setTimeKeeper(std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_ptr)
  {
    time_keeper_ = std::move(time_keeper_ptr);

    // set time keeper for grid
    grid_ptr_->setTimeKeeper(time_keeper_);
  }

  void setDataAccessor(const PointCloud2ConstPtr & in_cloud)
  {
    if (!data_accessor_.isInitialized()) {
      data_accessor_.setField(in_cloud);
    }
  }
  void process(const PointCloud2ConstPtr & in_cloud, pcl::PointIndices & out_no_ground_indices);

private:
  // parameters
  GridGroundFilterParameter param_;

  // data
  PointCloud2ConstPtr in_cloud_;
  PclDataAccessor data_accessor_;

  // grid data
  std::unique_ptr<Grid> grid_ptr_;

  // debug information
  std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_;

  bool recursiveSearch(const int check_idx, const int search_cnt, std::vector<int> & idx) const;
  bool recursiveSearch(
    const int check_idx, const int search_cnt, std::vector<int> & idx, size_t count) const;
  void fitLineFromGndGrid(const std::vector<int> & idx, float & a, float & b) const;

  void convert();
  void preprocess();
  void initializeGround(pcl::PointIndices & out_no_ground_indices);

  void SegmentContinuousCell(
    const Cell & cell, PointsCentroid & ground_bin, pcl::PointIndices & out_no_ground_indices);
  void SegmentDiscontinuousCell(
    const Cell & cell, PointsCentroid & ground_bin, pcl::PointIndices & out_no_ground_indices);
  void SegmentBreakCell(
    const Cell & cell, PointsCentroid & ground_bin, pcl::PointIndices & out_no_ground_indices);
  void classify(pcl::PointIndices & out_no_ground_indices);
};

}  // namespace autoware::ground_segmentation

#endif  // SCAN_GROUND_FILTER__GRID_GROUND_FILTER_HPP_

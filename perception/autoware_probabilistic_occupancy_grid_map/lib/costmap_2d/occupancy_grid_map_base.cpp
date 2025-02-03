// Copyright 2024 Tier IV, Inc.
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
/*********************************************************************
 *
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2008, 2013, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Eitan Marder-Eppstein
 *         David V. Lu!!
 *********************************************************************/

#include "autoware/probabilistic_occupancy_grid_map/costmap_2d/occupancy_grid_map_base.hpp"

#include "autoware/probabilistic_occupancy_grid_map/cost_value/cost_value.hpp"
#include "autoware/probabilistic_occupancy_grid_map/utils/utils.hpp"

#include <grid_map_costmap_2d/grid_map_costmap_2d.hpp>
#include <pcl_ros/transforms.hpp>

#include <sensor_msgs/point_cloud2_iterator.hpp>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#include <tf2_sensor_msgs/tf2_sensor_msgs.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>

#include <tf2_sensor_msgs/tf2_sensor_msgs.hpp>
#endif

#include <algorithm>

namespace autoware::occupancy_grid_map
{
namespace costmap_2d
{
using sensor_msgs::PointCloud2ConstIterator;

OccupancyGridMapInterface::OccupancyGridMapInterface(
  const bool use_cuda, const unsigned int cells_size_x, const unsigned int cells_size_y,
  const float resolution)
: Costmap2D(cells_size_x, cells_size_y, resolution, 0.f, 0.f, cost_value::NO_INFORMATION),
  use_cuda_(use_cuda)
{
#ifdef USE_CUDA
  if (use_cuda_) {
    min_height_ = -std::numeric_limits<double>::infinity();
    max_height_ = std::numeric_limits<double>::infinity();
    resolution_inv_ = 1.0 / resolution_;

    const auto num_cells_x = this->getSizeInCellsX();
    const auto num_cells_y = this->getSizeInCellsY();

    device_costmap_ = autoware::cuda_utils::make_unique<std::uint8_t[]>(num_cells_x * num_cells_y);
    device_costmap_aux_ =
      autoware::cuda_utils::make_unique<std::uint8_t[]>(num_cells_x * num_cells_y);

    device_rotation_map_ = autoware::cuda_utils::make_unique<Eigen::Matrix3f>();
    device_translation_map_ = autoware::cuda_utils::make_unique<Eigen::Vector3f>();
    device_rotation_scan_ = autoware::cuda_utils::make_unique<Eigen::Matrix3f>();
    device_translation_scan_ = autoware::cuda_utils::make_unique<Eigen::Vector3f>();
  }
#endif
}

void OccupancyGridMapInterface::updateOrigin(double new_origin_x, double new_origin_y)
{
  // project the new origin into the grid
  int cell_ox{static_cast<int>(std::floor((new_origin_x - origin_x_) / resolution_))};
  int cell_oy{static_cast<int>(std::floor((new_origin_y - origin_y_) / resolution_))};

  // compute the associated world coordinates for the origin cell
  // because we want to keep things grid-aligned
  double new_grid_ox{origin_x_ + cell_ox * resolution_};
  double new_grid_oy{origin_y_ + cell_oy * resolution_};

  // To save casting from unsigned int to int a bunch of times
  int size_x{static_cast<int>(size_x_)};
  int size_y{static_cast<int>(size_y_)};

  // we need to compute the overlap of the new and existing windows
  int lower_left_x{std::min(std::max(cell_ox, 0), size_x)};
  int lower_left_y{std::min(std::max(cell_oy, 0), size_y)};
  int upper_right_x{std::min(std::max(cell_ox + size_x, 0), size_x)};
  int upper_right_y{std::min(std::max(cell_oy + size_y, 0), size_y)};

  unsigned int cell_size_x = upper_right_x - lower_left_x;
  unsigned int cell_size_y = upper_right_y - lower_left_y;

  // we need a map to store the obstacles in the window temporarily
  unsigned char * local_map{nullptr};

#ifdef USE_CUDA
  using autoware::occupancy_grid_map::utils::copyMapRegionLaunch;
  if (use_cuda_) {
    copyMapRegionLaunch(
      device_costmap_.get(), lower_left_x, lower_left_y, size_x_, size_y_,
      device_costmap_aux_.get(), 0, 0, cell_size_x, cell_size_y, cell_size_x, cell_size_y, stream_);

    cudaMemsetAsync(
      device_costmap_.get(), cost_value::NO_INFORMATION, size_x_ * size_y_ * sizeof(std::uint8_t),
      stream_);
#else
  if (use_cuda_) {
    RCLCPP_ERROR(
      rclcpp::get_logger("pointcloud_based_occupancy_grid_map"),
      "The code was compiled without cuda.");
    return;
#endif
  } else {
    local_map = new unsigned char[cell_size_x * cell_size_y];

    // copy the local window in the costmap to the local map
    copyMapRegion(
      costmap_, lower_left_x, lower_left_y, size_x_, local_map, 0, 0, cell_size_x, cell_size_x,
      cell_size_y);

    // now we'll set the costmap to be completely unknown if we track unknown space
    nav2_costmap_2d::Costmap2D::resetMaps();
  }

  // update the origin with the appropriate world coordinates
  origin_x_ = new_grid_ox;
  origin_y_ = new_grid_oy;

  // compute the starting cell location for copying data back in
  int start_x{lower_left_x - cell_ox};
  int start_y{lower_left_y - cell_oy};

  // now we want to copy the overlapping information back into the map, but in its new location
#ifdef USE_CUDA
  if (use_cuda_) {
    if (
      start_x < 0 || start_y < 0 || start_x + cell_size_x > size_x_ ||
      start_y + cell_size_y > size_y_) {
      RCLCPP_ERROR(
        rclcpp::get_logger("pointcloud_based_occupancy_grid_map"),
        "update coordinates are negative or out of bounds: start.x=%d, start.y=%d, cell_size.x=%d, "
        "cell_size.y=%d size_x:%d, size_y=%d",
        start_x, start_y, cell_size_x, cell_size_y, size_x_, size_y_);
      return;
    }

    copyMapRegionLaunch(
      device_costmap_aux_.get(), 0, 0, cell_size_x, cell_size_y, device_costmap_.get(), start_x,
      start_y, size_x_, size_y_, cell_size_x, cell_size_y, stream_);
#else
  if (use_cuda_) {
    RCLCPP_ERROR(
      rclcpp::get_logger("pointcloud_based_occupancy_grid_map"),
      "The code was compiled without cuda.");
    return;
#endif
  } else {
    copyMapRegion(
      local_map, 0, 0, cell_size_x, costmap_, start_x, start_y, size_x_, cell_size_x, cell_size_y);

    // make sure to clean up
    if (local_map != nullptr) {
      delete[] local_map;
    }
  }
}

void OccupancyGridMapInterface::resetMaps()
{
#ifdef USE_CUDA
  if (use_cuda_) {
    cudaMemsetAsync(
      device_costmap_.get(), cost_value::NO_INFORMATION, getSizeInCellsX() * getSizeInCellsY(),
      stream_);
#else
  if (use_cuda_) {
    RCLCPP_ERROR(
      rclcpp::get_logger("pointcloud_based_occupancy_grid_map"),
      "The code was compiled without cuda.");
    return;
#endif
  } else {
    nav2_costmap_2d::Costmap2D::resetMaps();
  }
}

void OccupancyGridMapInterface::setHeightLimit(const double min_height, const double max_height)
{
  min_height_ = min_height;
  max_height_ = max_height;
}

bool OccupancyGridMapInterface::isCudaEnabled() const
{
  return use_cuda_;
}

#ifdef USE_CUDA
void OccupancyGridMapInterface::setCudaStream(const cudaStream_t & stream)
{
  if (isCudaEnabled()) {
    stream_ = stream;
  }
}

const autoware::cuda_utils::CudaUniquePtr<std::uint8_t[]> &
OccupancyGridMapInterface::getDeviceCostmap() const
{
  return device_costmap_;
}

void OccupancyGridMapInterface::copyDeviceCostmapToHost() const
{
  cudaMemcpyAsync(
    costmap_, device_costmap_.get(), getSizeInCellsX() * getSizeInCellsY() * sizeof(std::uint8_t),
    cudaMemcpyDeviceToHost, stream_);
  cudaStreamSynchronize(stream_);
}
#endif

}  // namespace costmap_2d
}  // namespace autoware::occupancy_grid_map

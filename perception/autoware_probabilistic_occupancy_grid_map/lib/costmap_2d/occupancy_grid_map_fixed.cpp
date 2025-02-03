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

#include "autoware/probabilistic_occupancy_grid_map/costmap_2d/occupancy_grid_map_fixed.hpp"

#include "autoware/probabilistic_occupancy_grid_map/cost_value/cost_value.hpp"
#include "autoware/probabilistic_occupancy_grid_map/costmap_2d/occupancy_grid_map_fixed_kernel.hpp"
#include "autoware/probabilistic_occupancy_grid_map/utils/utils.hpp"
#include "autoware/probabilistic_occupancy_grid_map/utils/utils_kernel.hpp"

#include <autoware/cuda_utils/cuda_unique_ptr.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>
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

OccupancyGridMapFixedBlindSpot::OccupancyGridMapFixedBlindSpot(
  const bool use_cuda, const unsigned int cells_size_x, const unsigned int cells_size_y,
  const float resolution)
: OccupancyGridMapInterface(use_cuda, cells_size_x, cells_size_y, resolution)
{
  if (use_cuda_) {
    const size_t angle_bin_size =
      ((max_angle_ - min_angle_) * angle_increment_inv_) + size_t(1 /*margin*/);

    const auto num_cells_x = this->getSizeInCellsX();
    const auto num_cells_y = this->getSizeInCellsY();
    const std::size_t range_bin_size =
      static_cast<std::size_t>(std::sqrt(2) * std::max(num_cells_x, num_cells_y) / 2.0) + 1;

    raw_points_tensor_ =
      autoware::cuda_utils::make_unique<std::uint64_t[]>(2 * angle_bin_size * range_bin_size);
    obstacle_points_tensor_ =
      autoware::cuda_utils::make_unique<std::uint64_t[]>(2 * angle_bin_size * range_bin_size);
  }
}

/**
 * @brief update Gridmap with PointCloud
 *
 * @param raw_pointcloud raw point cloud on a certain frame (usually base_link)
 * @param obstacle_pointcloud raw point cloud on a certain frame (usually base_link)
 * @param robot_pose frame of the input point cloud (usually base_link)
 * @param scan_origin manually chosen grid map origin frame
 */
void OccupancyGridMapFixedBlindSpot::updateWithPointCloud(
  const CudaPointCloud2 & raw_pointcloud, const CudaPointCloud2 & obstacle_pointcloud,
  const Pose & robot_pose, const Pose & scan_origin)
{
  const size_t angle_bin_size =
    ((max_angle_ - min_angle_) * angle_increment_inv_) + size_t(1 /*margin*/);

  // Transform Matrix from base_link to map frame
  mat_map_ = utils::getTransformMatrix(robot_pose);

  const auto scan2map_pose = utils::getInversePose(scan_origin);  // scan -> map transform pose

  // Transform Matrix from map frame to scan frame
  mat_scan_ = utils::getTransformMatrix(scan2map_pose);

  const auto map_res = this->getResolution();
  const auto num_cells_x = this->getSizeInCellsX();
  const auto num_cells_y = this->getSizeInCellsY();
  const std::size_t range_bin_size =
    static_cast<std::size_t>(std::sqrt(2) * std::max(num_cells_x, num_cells_y) / 2.0) + 1;

  cudaMemsetAsync(
    raw_points_tensor_.get(), 0xFF, 2 * angle_bin_size * range_bin_size * sizeof(std::int64_t),
    stream_);
  cudaMemsetAsync(
    obstacle_points_tensor_.get(), 0xFF, 2 * angle_bin_size * range_bin_size * sizeof(std::int64_t),
    stream_);
  cudaMemsetAsync(
    device_costmap_.get(), cost_value::NO_INFORMATION,
    num_cells_x * num_cells_y * sizeof(std::uint8_t), stream_);

  Eigen::Matrix3f rotation_map = mat_map_.block<3, 3>(0, 0);
  Eigen::Vector3f translation_map = mat_map_.block<3, 1>(0, 3);

  Eigen::Matrix3f rotation_scan = mat_scan_.block<3, 3>(0, 0);
  Eigen::Vector3f translation_scan = mat_scan_.block<3, 1>(0, 3);

  cudaMemcpyAsync(
    device_rotation_map_.get(), &rotation_map, sizeof(Eigen::Matrix3f), cudaMemcpyHostToDevice,
    stream_);
  cudaMemcpyAsync(
    device_translation_map_.get(), &translation_map, sizeof(Eigen::Vector3f),
    cudaMemcpyHostToDevice, stream_);
  cudaMemcpyAsync(
    device_rotation_scan_.get(), &rotation_scan, sizeof(Eigen::Matrix3f), cudaMemcpyHostToDevice,
    stream_);
  cudaMemcpyAsync(
    device_translation_scan_.get(), &translation_scan, sizeof(Eigen::Vector3f),
    cudaMemcpyHostToDevice, stream_);

  const std::size_t num_raw_points = raw_pointcloud.width * raw_pointcloud.height;
  float range_resolution_inv = 1.0 / map_res;

  map_fixed::prepareTensorLaunch(
    reinterpret_cast<const float *>(raw_pointcloud.data.get()), num_raw_points,
    raw_pointcloud.point_step / sizeof(float), angle_bin_size, range_bin_size, min_height_,
    max_height_, min_angle_, angle_increment_inv_, range_resolution_inv, device_rotation_map_.get(),
    device_translation_map_.get(), device_rotation_scan_.get(), device_translation_scan_.get(),
    raw_points_tensor_.get(), stream_);

  const std::size_t num_obstacle_points = obstacle_pointcloud.width * obstacle_pointcloud.height;

  map_fixed::prepareTensorLaunch(
    reinterpret_cast<const float *>(obstacle_pointcloud.data.get()), num_obstacle_points,
    obstacle_pointcloud.point_step / sizeof(float), angle_bin_size, range_bin_size, min_height_,
    max_height_, min_angle_, angle_increment_inv_, range_resolution_inv, device_rotation_map_.get(),
    device_translation_map_.get(), device_rotation_scan_.get(), device_translation_scan_.get(),
    obstacle_points_tensor_.get(), stream_);

  map_fixed::fillEmptySpaceLaunch(
    raw_points_tensor_.get(), angle_bin_size, range_bin_size, range_resolution_inv,
    scan_origin.position.x, scan_origin.position.y, origin_x_, origin_y_, num_cells_x, num_cells_y,
    cost_value::FREE_SPACE, device_costmap_.get(), stream_);

  map_fixed::fillUnknownSpaceLaunch(
    raw_points_tensor_.get(), obstacle_points_tensor_.get(), distance_margin_, angle_bin_size,
    range_bin_size, range_resolution_inv, scan_origin.position.x, scan_origin.position.y, origin_x_,
    origin_y_, num_cells_x, num_cells_y, cost_value::FREE_SPACE, cost_value::NO_INFORMATION,
    device_costmap_.get(), stream_);

  map_fixed::fillObstaclesLaunch(
    obstacle_points_tensor_.get(), distance_margin_, angle_bin_size, range_bin_size,
    range_resolution_inv, origin_x_, origin_y_, num_cells_x, num_cells_y,
    cost_value::LETHAL_OBSTACLE, device_costmap_.get(), stream_);
}

void OccupancyGridMapFixedBlindSpot::initRosParam(rclcpp::Node & node)
{
  distance_margin_ =
    node.declare_parameter<double>("OccupancyGridMapFixedBlindSpot.distance_margin");
}

}  // namespace costmap_2d
}  // namespace autoware::occupancy_grid_map

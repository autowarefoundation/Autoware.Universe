// Copyright 2021 Tier IV, Inc.
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

#include "pointcloud_based_occupancy_grid_map/occupancy_grid_map.hpp"

#include "cost_value.hpp"
#include "utils/utils.hpp"

#include <grid_map_costmap_2d/grid_map_costmap_2d.hpp>
#include <pcl_ros/transforms.hpp>
#include <tier4_autoware_utils/tier4_autoware_utils.hpp>

#include <sensor_msgs/point_cloud2_iterator.hpp>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#include <tf2_sensor_msgs/tf2_sensor_msgs.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>

#include <tf2_sensor_msgs/tf2_sensor_msgs.hpp>
#endif

#include <algorithm>
namespace costmap_2d
{
using sensor_msgs::PointCloud2ConstIterator;

OccupancyGridMap::OccupancyGridMap(
  const unsigned int cells_size_x, const unsigned int cells_size_y, const float resolution)
: Costmap2D(cells_size_x, cells_size_y, resolution, 0.f, 0.f, occupancy_cost_value::NO_INFORMATION)
{
}

bool OccupancyGridMap::worldToMap(double wx, double wy, unsigned int & mx, unsigned int & my) const
{
  if (wx < origin_x_ || wy < origin_y_) {
    return false;
  }

  mx = static_cast<int>(std::floor((wx - origin_x_) / resolution_));
  my = static_cast<int>(std::floor((wy - origin_y_) / resolution_));

  if (mx < size_x_ && my < size_y_) {
    return true;
  }

  return false;
}

void OccupancyGridMap::updateOrigin(double new_origin_x, double new_origin_y)
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
  unsigned char * local_map = new unsigned char[cell_size_x * cell_size_y];

  // copy the local window in the costmap to the local map
  copyMapRegion(
    costmap_, lower_left_x, lower_left_y, size_x_, local_map, 0, 0, cell_size_x, cell_size_x,
    cell_size_y);

  // now we'll set the costmap to be completely unknown if we track unknown space
  resetMaps();

  // update the origin with the appropriate world coordinates
  origin_x_ = new_grid_ox;
  origin_y_ = new_grid_oy;

  // compute the starting cell location for copying data back in
  int start_x{lower_left_x - cell_ox};
  int start_y{lower_left_y - cell_oy};

  // now we want to copy the overlapping information back into the map, but in its new location
  copyMapRegion(
    local_map, 0, 0, cell_size_x, costmap_, start_x, start_y, size_x_, cell_size_x, cell_size_y);

  // make sure to clean up
  delete[] local_map;
}

/**
 * @brief update Gridmap with PointCloud
 *
 * @param raw_pointcloud raw point cloud on a certain frame (usually base_link)
 * @param obstacle_pointcloud raw point cloud on a certain frame (usually base_link)
 * @param robot_pose frame of the input point cloud (usually base_link)
 * @param scan_origin manually chosen grid map origin frame
 */
void OccupancyGridMap::updateWithPointCloudSimple(
  const PointCloud2 & raw_pointcloud, const PointCloud2 & obstacle_pointcloud,
  const Pose & robot_pose, const Pose & scan_origin)
{
  constexpr double min_angle = tier4_autoware_utils::deg2rad(-180.0);
  constexpr double max_angle = tier4_autoware_utils::deg2rad(180.0);
  constexpr double angle_increment = tier4_autoware_utils::deg2rad(0.1);
  const size_t angle_bin_size = ((max_angle - min_angle) / angle_increment) + size_t(1 /*margin*/);

  // Transform from base_link to map frame
  PointCloud2 map_raw_pointcloud, map_obstacle_pointcloud;  // point cloud in map frame
  utils::transformPointcloud(raw_pointcloud, robot_pose, map_raw_pointcloud);
  utils::transformPointcloud(obstacle_pointcloud, robot_pose, map_obstacle_pointcloud);

  // Transform from map frame to scan frame
  PointCloud2 scan_raw_pointcloud, scan_obstacle_pointcloud;      // point cloud in scan frame
  const auto scan2map_pose = utils::getInversePose(scan_origin);  // scan -> map transform pose
  utils::transformPointcloud(map_raw_pointcloud, scan2map_pose, scan_raw_pointcloud);
  utils::transformPointcloud(map_obstacle_pointcloud, scan2map_pose, scan_obstacle_pointcloud);

  // Create angle bins
  struct BinInfo
  {
    BinInfo() = default;
    BinInfo(const double _range, const double _wx, const double _wy)
    : range(_range), wx(_wx), wy(_wy)
    {
    }
    double range;
    double wx;
    double wy;
  };
  std::vector</*angle bin*/ std::vector<BinInfo>> obstacle_pointcloud_angle_bins;
  std::vector</*angle bin*/ std::vector<BinInfo>> raw_pointcloud_angle_bins;
  obstacle_pointcloud_angle_bins.resize(angle_bin_size);
  raw_pointcloud_angle_bins.resize(angle_bin_size);
  for (PointCloud2ConstIterator<float> iter_x(scan_raw_pointcloud, "x"),
       iter_y(scan_raw_pointcloud, "y"), iter_wx(map_raw_pointcloud, "x"),
       iter_wy(map_raw_pointcloud, "y");
       iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_wx, ++iter_wy) {
    const double angle = atan2(*iter_y, *iter_x);
    const int angle_bin_index = (angle - min_angle) / angle_increment;
    raw_pointcloud_angle_bins.at(angle_bin_index)
      .push_back(BinInfo(std::hypot(*iter_y, *iter_x), *iter_wx, *iter_wy));
  }
  for (PointCloud2ConstIterator<float> iter_x(scan_obstacle_pointcloud, "x"),
       iter_y(scan_obstacle_pointcloud, "y"), iter_wx(map_obstacle_pointcloud, "x"),
       iter_wy(map_obstacle_pointcloud, "y");
       iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_wx, ++iter_wy) {
    const double angle = atan2(*iter_y, *iter_x);
    int angle_bin_index = (angle - min_angle) / angle_increment;
    obstacle_pointcloud_angle_bins.at(angle_bin_index)
      .push_back(BinInfo(std::hypot(*iter_y, *iter_x), *iter_wx, *iter_wy));
  }

  // Sort by distance
  for (auto & obstacle_pointcloud_angle_bin : obstacle_pointcloud_angle_bins) {
    std::sort(
      obstacle_pointcloud_angle_bin.begin(), obstacle_pointcloud_angle_bin.end(),
      [](auto a, auto b) { return a.range < b.range; });
  }
  for (auto & raw_pointcloud_angle_bin : raw_pointcloud_angle_bins) {
    std::sort(raw_pointcloud_angle_bin.begin(), raw_pointcloud_angle_bin.end(), [](auto a, auto b) {
      return a.range < b.range;
    });
  }

  // First step: Initialize cells to the final point with freespace
  constexpr double distance_margin = 1.0;
  for (size_t bin_index = 0; bin_index < obstacle_pointcloud_angle_bins.size(); ++bin_index) {
    auto & obstacle_pointcloud_angle_bin = obstacle_pointcloud_angle_bins.at(bin_index);
    auto & raw_pointcloud_angle_bin = raw_pointcloud_angle_bins.at(bin_index);

    BinInfo end_distance;
    if (raw_pointcloud_angle_bin.empty() && obstacle_pointcloud_angle_bin.empty()) {
      continue;
    } else if (raw_pointcloud_angle_bin.empty()) {
      end_distance = obstacle_pointcloud_angle_bin.back();
    } else if (obstacle_pointcloud_angle_bin.empty()) {
      end_distance = raw_pointcloud_angle_bin.back();
    } else {
      end_distance = obstacle_pointcloud_angle_bin.back().range + distance_margin <
                         raw_pointcloud_angle_bin.back().range
                       ? raw_pointcloud_angle_bin.back()
                       : obstacle_pointcloud_angle_bin.back();
    }
    raytrace(
      scan_origin.position.x, scan_origin.position.y, end_distance.wx, end_distance.wy,
      occupancy_cost_value::FREE_SPACE);
  }

  // Second step: Add uknown cell
  for (size_t bin_index = 0; bin_index < obstacle_pointcloud_angle_bins.size(); ++bin_index) {
    auto & obstacle_pointcloud_angle_bin = obstacle_pointcloud_angle_bins.at(bin_index);
    auto & raw_pointcloud_angle_bin = raw_pointcloud_angle_bins.at(bin_index);
    auto raw_distance_iter = raw_pointcloud_angle_bin.begin();
    for (size_t dist_index = 0; dist_index < obstacle_pointcloud_angle_bin.size(); ++dist_index) {
      // Calculate next raw point from obstacle point
      while (raw_distance_iter != raw_pointcloud_angle_bin.end()) {
        if (
          raw_distance_iter->range <
          obstacle_pointcloud_angle_bin.at(dist_index).range + distance_margin)
          raw_distance_iter++;
        else
          break;
      }

      // There is no point far than the obstacle point.
      const bool no_freespace_point = (raw_distance_iter == raw_pointcloud_angle_bin.end());

      if (dist_index + 1 == obstacle_pointcloud_angle_bin.size()) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        if (!no_freespace_point) {
          const auto & target = *raw_distance_iter;
          raytrace(
            source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
          setCellValue(target.wx, target.wy, occupancy_cost_value::FREE_SPACE);
        }
        continue;
      }

      auto next_obstacle_point_distance = std::abs(
        obstacle_pointcloud_angle_bin.at(dist_index + 1).range -
        obstacle_pointcloud_angle_bin.at(dist_index).range);
      if (next_obstacle_point_distance <= distance_margin) {
        continue;
      } else if (no_freespace_point) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = obstacle_pointcloud_angle_bin.at(dist_index + 1);
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
        continue;
      }

      auto next_raw_distance =
        std::abs(obstacle_pointcloud_angle_bin.at(dist_index).range - raw_distance_iter->range);
      if (next_raw_distance < next_obstacle_point_distance) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = *raw_distance_iter;
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
        setCellValue(target.wx, target.wy, occupancy_cost_value::FREE_SPACE);
        continue;
      } else {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = obstacle_pointcloud_angle_bin.at(dist_index + 1);
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
        continue;
      }
    }
  }

  // Third step: Overwrite occupied cell
  for (size_t bin_index = 0; bin_index < obstacle_pointcloud_angle_bins.size(); ++bin_index) {
    auto & obstacle_pointcloud_angle_bin = obstacle_pointcloud_angle_bins.at(bin_index);
    for (size_t dist_index = 0; dist_index < obstacle_pointcloud_angle_bin.size(); ++dist_index) {
      const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
      setCellValue(source.wx, source.wy, occupancy_cost_value::LETHAL_OBSTACLE);

      if (dist_index + 1 == obstacle_pointcloud_angle_bin.size()) {
        continue;
      }

      auto next_obstacle_point_distance = std::abs(
        obstacle_pointcloud_angle_bin.at(dist_index + 1).range -
        obstacle_pointcloud_angle_bin.at(dist_index).range);
      if (next_obstacle_point_distance <= distance_margin) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = obstacle_pointcloud_angle_bin.at(dist_index + 1);
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::LETHAL_OBSTACLE);
        continue;
      }
    }
  }
}

/**
 * @brief update Gridmap with PointCloud in 3D manner
 *
 * @param raw_pointcloud raw point cloud on a certain frame (usually base_link)
 * @param obstacle_pointcloud raw point cloud on a certain frame (usually base_link)
 * @param robot_pose frame of the input point cloud (usually base_link)
 * @param scan_origin manually chosen grid map origin frame
 */
void OccupancyGridMap::updateWithPointCloud3D(
  const PointCloud2 & raw_pointcloud, const PointCloud2 & obstacle_pointcloud,
  const Pose & robot_pose, const Pose & scan_origin, const double projection_dz_threshold,
  const double obstacle_separation_threshold, grid_map::GridMap * debug_grid)
{
  constexpr double min_angle = tier4_autoware_utils::deg2rad(-180.0);
  constexpr double max_angle = tier4_autoware_utils::deg2rad(180.0);
  constexpr double angle_increment = tier4_autoware_utils::deg2rad(0.1);
  const size_t angle_bin_size = ((max_angle - min_angle) / angle_increment) + size_t(1 /*margin*/);

  // Transform from base_link to map frame
  PointCloud2 map_raw_pointcloud, map_obstacle_pointcloud;  // point cloud in map frame
  utils::transformPointcloud(raw_pointcloud, robot_pose, map_raw_pointcloud);
  utils::transformPointcloud(obstacle_pointcloud, robot_pose, map_obstacle_pointcloud);

  // Transform from map frame to scan frame
  PointCloud2 scan_raw_pointcloud, scan_obstacle_pointcloud;      // point cloud in scan frame
  const auto scan2map_pose = utils::getInversePose(scan_origin);  // scan -> map transform pose
  utils::transformPointcloud(map_raw_pointcloud, scan2map_pose, scan_raw_pointcloud);
  utils::transformPointcloud(map_obstacle_pointcloud, scan2map_pose, scan_obstacle_pointcloud);

  // Create angle bins
  struct BinInfo3D
  {
    BinInfo3D(
      const double _range = 0.0, const double _wx = 0.0, const double _wy = 0.0,
      const double _wz = 0.0, const double _projection_length = 0.0,
      const double _projected_wx = 0.0, const double _projected_wy = 0.0)
    : range(_range),
      wx(_wx),
      wy(_wy),
      wz(_wz),
      projection_length(_projection_length),
      projected_wx(_projected_wx),
      projected_wy(_projected_wy)
    {
    }
    double range;
    double wx;
    double wy;
    double wz;
    double projection_length;
    double projected_wx;
    double projected_wy;
  };

  std::vector</*angle bin*/ std::vector<BinInfo3D>> obstacle_pointcloud_angle_bins;
  std::vector</*angle bin*/ std::vector<BinInfo3D>> raw_pointcloud_angle_bins;
  obstacle_pointcloud_angle_bins.resize(angle_bin_size);
  raw_pointcloud_angle_bins.resize(angle_bin_size);
  for (PointCloud2ConstIterator<float> iter_x(scan_raw_pointcloud, "x"),
       iter_y(scan_raw_pointcloud, "y"), iter_wx(map_raw_pointcloud, "x"),
       iter_wy(map_raw_pointcloud, "y"), iter_wz(map_raw_pointcloud, "z");
       iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_wx, ++iter_wy, ++iter_wz) {
    const double angle = atan2(*iter_y, *iter_x);
    const int angle_bin_index = (angle - min_angle) / angle_increment;
    raw_pointcloud_angle_bins.at(angle_bin_index)
      .emplace_back(std::hypot(*iter_y, *iter_x), *iter_wx, *iter_wy, *iter_wz);
  }
  for (PointCloud2ConstIterator<float> iter_x(scan_obstacle_pointcloud, "x"),
       iter_y(scan_obstacle_pointcloud, "y"), iter_z(scan_obstacle_pointcloud, "z"),
       iter_wx(map_obstacle_pointcloud, "x"), iter_wy(map_obstacle_pointcloud, "y"),
       iter_wz(map_obstacle_pointcloud, "z");
       iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++iter_wx, ++iter_wy, ++iter_wz) {
    const double scan_z = scan_origin.position.z - robot_pose.position.z;
    const double obstacle_z = (*iter_wz) - robot_pose.position.z;
    const double dz = scan_z - obstacle_z;
    const double angle = atan2(*iter_y, *iter_x);
    const int angle_bin_index = (angle - min_angle) / angle_increment;
    const double range = std::hypot(*iter_x, *iter_y);
    if (dz > projection_dz_threshold) {
      const double ratio = obstacle_z / dz;
      const double projection_length = range * ratio;
      const double projected_wx = (*iter_wx) + ((*iter_wx) - scan_origin.position.x) * ratio;
      const double projected_wy = (*iter_wy) + ((*iter_wy) - scan_origin.position.y) * ratio;
      obstacle_pointcloud_angle_bins.at(angle_bin_index)
        .emplace_back(
          range, *iter_wx, *iter_wy, *iter_wz, projection_length, projected_wx, projected_wy);
    } else {
      obstacle_pointcloud_angle_bins.at(angle_bin_index)
        .emplace_back(
          range, *iter_wx, *iter_wy, *iter_wz, std::numeric_limits<double>::infinity(),
          std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    }
  }

  // Sort by distance
  for (auto & obstacle_pointcloud_angle_bin : obstacle_pointcloud_angle_bins) {
    std::sort(
      obstacle_pointcloud_angle_bin.begin(), obstacle_pointcloud_angle_bin.end(),
      [](auto a, auto b) { return a.range < b.range; });
  }
  for (auto & raw_pointcloud_angle_bin : raw_pointcloud_angle_bins) {
    std::sort(raw_pointcloud_angle_bin.begin(), raw_pointcloud_angle_bin.end(), [](auto a, auto b) {
      return a.range < b.range;
    });
  }

  grid_map::Costmap2DConverter<grid_map::GridMap> converter;
  if (debug_grid) {
    debug_grid->setFrameId("map");
    debug_grid->setGeometry(
      grid_map::Length(size_x_ * resolution_, size_y_ * resolution_), resolution_,
      grid_map::Position(
        origin_x_ + size_x_ * resolution_ / 2.0, origin_y_ + size_y_ * resolution_ / 2.0));
  }

  auto is_visible_beyond_obstacle = [&](const BinInfo3D & obstacle, const BinInfo3D & raw) -> bool {
    if (raw.range < obstacle.range) {
      return false;
    }

    if (std::isinf(obstacle.projection_length)) {
      return false;
    }

    // y = ax + b
    const double a = -(scan_origin.position.z - robot_pose.position.z) /
                     (obstacle.range + obstacle.projection_length);
    const double b = scan_origin.position.z;
    return raw.wz > (a * raw.range + b);
  };

  // First step: Initialize cells to the final point with freespace
  for (size_t bin_index = 0; bin_index < obstacle_pointcloud_angle_bins.size(); ++bin_index) {
    const auto & obstacle_pointcloud_angle_bin = obstacle_pointcloud_angle_bins.at(bin_index);
    const auto & raw_pointcloud_angle_bin = raw_pointcloud_angle_bins.at(bin_index);

    BinInfo3D ray_end;
    if (raw_pointcloud_angle_bin.empty() && obstacle_pointcloud_angle_bin.empty()) {
      continue;
    } else if (raw_pointcloud_angle_bin.empty()) {
      ray_end = obstacle_pointcloud_angle_bin.back();
    } else if (obstacle_pointcloud_angle_bin.empty()) {
      ray_end = raw_pointcloud_angle_bin.back();
    } else {
      const auto & farthest_obstacle_this_bin = obstacle_pointcloud_angle_bin.back();
      const auto & farthest_raw_this_bin = raw_pointcloud_angle_bin.back();
      ray_end = is_visible_beyond_obstacle(farthest_obstacle_this_bin, farthest_raw_this_bin)
                  ? farthest_raw_this_bin
                  : farthest_obstacle_this_bin;
    }
    raytrace(
      scan_origin.position.x, scan_origin.position.y, ray_end.wx, ray_end.wy,
      occupancy_cost_value::FREE_SPACE);
  }

  if (debug_grid) converter.addLayerFromCostmap2D(*this, "filled_free_to_farthest", *debug_grid);

  // Second step: Add uknown cell
  for (size_t bin_index = 0; bin_index < obstacle_pointcloud_angle_bins.size(); ++bin_index) {
    const auto & obstacle_pointcloud_angle_bin = obstacle_pointcloud_angle_bins.at(bin_index);
    const auto & raw_pointcloud_angle_bin = raw_pointcloud_angle_bins.at(bin_index);
    auto raw_distance_iter = raw_pointcloud_angle_bin.begin();
    for (size_t dist_index = 0; dist_index < obstacle_pointcloud_angle_bin.size(); ++dist_index) {
      // Calculate next raw point from obstacle point
      const auto & obstacle_bin = obstacle_pointcloud_angle_bin.at(dist_index);
      while (raw_distance_iter != raw_pointcloud_angle_bin.end()) {
        if (!is_visible_beyond_obstacle(obstacle_bin, *raw_distance_iter))
          raw_distance_iter++;
        else
          break;
      }

      // There is no point farther than the obstacle point.
      const bool no_visible_point_beyond = (raw_distance_iter == raw_pointcloud_angle_bin.end());
      if (no_visible_point_beyond) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        raytrace(
          source.wx, source.wy, source.projected_wx, source.projected_wy,
          occupancy_cost_value::NO_INFORMATION);
        break;
      }

      if (dist_index + 1 == obstacle_pointcloud_angle_bin.size()) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        if (!no_visible_point_beyond) {
          raytrace(
            source.wx, source.wy, source.projected_wx, source.projected_wy,
            occupancy_cost_value::NO_INFORMATION);
        }
        continue;
      }

      auto next_obstacle_point_distance = std::abs(
        obstacle_pointcloud_angle_bin.at(dist_index + 1).range -
        obstacle_pointcloud_angle_bin.at(dist_index).range);
      if (next_obstacle_point_distance <= obstacle_separation_threshold) {
        continue;
      } else if (no_visible_point_beyond) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = obstacle_pointcloud_angle_bin.at(dist_index + 1);
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
        continue;
      }

      auto next_raw_distance =
        std::abs(obstacle_pointcloud_angle_bin.at(dist_index).range - raw_distance_iter->range);
      if (next_raw_distance < next_obstacle_point_distance) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = *raw_distance_iter;
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
        setCellValue(target.wx, target.wy, occupancy_cost_value::FREE_SPACE);
        continue;
      } else {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = obstacle_pointcloud_angle_bin.at(dist_index + 1);
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::NO_INFORMATION);
        continue;
      }
    }
  }

  if (debug_grid) converter.addLayerFromCostmap2D(*this, "added_unknown", *debug_grid);

  // Third step: Overwrite occupied cell
  for (size_t bin_index = 0; bin_index < obstacle_pointcloud_angle_bins.size(); ++bin_index) {
    auto & obstacle_pointcloud_angle_bin = obstacle_pointcloud_angle_bins.at(bin_index);
    for (size_t dist_index = 0; dist_index < obstacle_pointcloud_angle_bin.size(); ++dist_index) {
      const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
      setCellValue(source.wx, source.wy, occupancy_cost_value::LETHAL_OBSTACLE);

      if (dist_index + 1 == obstacle_pointcloud_angle_bin.size()) {
        continue;
      }

      auto next_obstacle_point_distance = std::abs(
        obstacle_pointcloud_angle_bin.at(dist_index + 1).range -
        obstacle_pointcloud_angle_bin.at(dist_index).range);
      if (next_obstacle_point_distance <= obstacle_separation_threshold) {
        const auto & source = obstacle_pointcloud_angle_bin.at(dist_index);
        const auto & target = obstacle_pointcloud_angle_bin.at(dist_index + 1);
        raytrace(source.wx, source.wy, target.wx, target.wy, occupancy_cost_value::LETHAL_OBSTACLE);
        continue;
      }
    }
  }

  if (debug_grid) converter.addLayerFromCostmap2D(*this, "added_obstacle", *debug_grid);
}

void OccupancyGridMap::setCellValue(const double wx, const double wy, const unsigned char cost)
{
  MarkCell marker(costmap_, cost);
  unsigned int mx{};
  unsigned int my{};
  if (!worldToMap(wx, wy, mx, my)) {
    RCLCPP_DEBUG(logger_, "Computing map coords failed");
    return;
  }
  const unsigned int index = getIndex(mx, my);
  marker(index);
}

void OccupancyGridMap::raytrace(
  const double source_x, const double source_y, const double target_x, const double target_y,
  const unsigned char cost)
{
  unsigned int x0{};
  unsigned int y0{};
  const double ox{source_x};
  const double oy{source_y};
  if (!worldToMap(ox, oy, x0, y0)) {
    RCLCPP_DEBUG(
      logger_,
      "The origin for the sensor at (%.2f, %.2f) is out of map bounds. So, the costmap cannot "
      "raytrace for it.",
      ox, oy);
    return;
  }

  // we can pre-compute the endpoints of the map outside of the inner loop... we'll need these later
  const double origin_x = origin_x_, origin_y = origin_y_;
  const double map_end_x = origin_x + size_x_ * resolution_;
  const double map_end_y = origin_y + size_y_ * resolution_;

  double wx = target_x;
  double wy = target_y;

  // now we also need to make sure that the endpoint we're ray-tracing
  // to isn't off the costmap and scale if necessary
  const double a = wx - ox;
  const double b = wy - oy;

  // the minimum value to raytrace from is the origin
  if (wx < origin_x) {
    const double t = (origin_x - ox) / a;
    wx = origin_x;
    wy = oy + b * t;
  }
  if (wy < origin_y) {
    const double t = (origin_y - oy) / b;
    wx = ox + a * t;
    wy = origin_y;
  }

  // the maximum value to raytrace to is the end of the map
  if (wx > map_end_x) {
    const double t = (map_end_x - ox) / a;
    wx = map_end_x - .001;
    wy = oy + b * t;
  }
  if (wy > map_end_y) {
    const double t = (map_end_y - oy) / b;
    wx = ox + a * t;
    wy = map_end_y - .001;
  }

  // now that the vector is scaled correctly... we'll get the map coordinates of its endpoint
  unsigned int x1{};
  unsigned int y1{};

  // check for legality just in case
  if (!worldToMap(wx, wy, x1, y1)) {
    return;
  }

  constexpr unsigned int cell_raytrace_range = 10000;  // large number to ignore range threshold
  MarkCell marker(costmap_, cost);
  raytraceLine(marker, x0, y0, x1, y1, cell_raytrace_range);
}

}  // namespace costmap_2d

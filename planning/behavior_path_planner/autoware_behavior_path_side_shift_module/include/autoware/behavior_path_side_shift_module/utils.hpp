// Copyright 2021 TIER IV, Inc.
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

#ifndef AUTOWARE__BEHAVIOR_PATH_SIDE_SHIFT_MODULE__UTILS_HPP_
#define AUTOWARE__BEHAVIOR_PATH_SIDE_SHIFT_MODULE__UTILS_HPP_

#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>

namespace autoware::behavior_path_planner
{
using autoware_internal_planning_msgs::msg::PathWithLaneId;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;
using geometry_msgs::msg::TransformStamped;

/**
 * @brief Sets the orientation (yaw angle) for all points in the path.
 * @param [in,out] path Path with lane ID to set orientation.
 * @details For each point, calculates orientation based on:
 *          - Vector to next point if not last point
 *          - Vector from previous point if last point
 *          - Zero angle if single point
 */
void setOrientation(PathWithLaneId * path);

/**
 * @brief Gets the shift length at the closest path point to the ego position.
 * @param [in] shifted_path Path with shift length information.
 * @param [in] ego_point Current ego position.
 * @return Shift length at the closest path point. Returns 0.0 if path is empty.
 */
double getClosestShiftLength(
  const ShiftedPath & shifted_path, const geometry_msgs::msg::Point ego_point);

}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_SIDE_SHIFT_MODULE__UTILS_HPP_

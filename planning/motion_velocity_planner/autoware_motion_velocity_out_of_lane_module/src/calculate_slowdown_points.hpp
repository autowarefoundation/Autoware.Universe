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

#ifndef CALCULATE_SLOWDOWN_POINTS_HPP_
#define CALCULATE_SLOWDOWN_POINTS_HPP_

#include "types.hpp"

#include <autoware/motion_velocity_planner_common_universe/planner_data.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>

#include <geometry_msgs/msg/pose.hpp>

#include <optional>

namespace autoware::motion_velocity_planner::out_of_lane
{
/// @brief calculate the slowdown pose just ahead of a point to avoid
/// @param ego_data ego data (trajectory, velocity, etc)
/// @param point_to_avoid the point to avoid
/// @param footprint the ego footprint
/// @param params parameters
/// @return optional slowdown point to insert in the trajectory
std::optional<geometry_msgs::msg::Pose> calculate_pose_ahead_of_collision(
  const EgoData & ego_data, const OutOfLanePoint & point_to_avoid,
  const universe_utils::Polygon2d & footprint, const double precision);

/// @brief calculate the last pose staying inside the lane and before the given point
/// @details if no pose staying inside the lane can be found, return a pose before the given point
/// @param [in] ego_data ego data (trajectory)
/// @param [in] point_to_avoid the out of lane point to avoid
/// @param [in] params parameters (ego footprint offsets, precision)
/// @return optional slowdown pose
std::optional<geometry_msgs::msg::Pose> calculate_last_in_lane_pose(
  const EgoData & ego_data, const OutOfLanePoint & point_to_avoid, PlannerParam params);

/// @brief calculate the slowdown point to insert in the trajectory
/// @param ego_data ego data (trajectory, velocity, etc)
/// @param out_of_lane_data data about out of lane areas
/// @param params parameters
/// @return optional slowdown pose to insert in the trajectory
std::optional<geometry_msgs::msg::Pose> calculate_slowdown_pose(
  const EgoData & ego_data, const OutOfLaneData & out_of_lane_data, const PlannerParam & params);

/// @brief calculate the minimum stop and slowdown distances of ego
/// @param [inout] ego_data ego data where minimum stop and slowdown distances are set
/// @param [in] planner_data data with vehicle related information
/// @param [in] previous_slowdown_pose previous slowdown pose
void calculate_min_stop_and_slowdown_distances(
  out_of_lane::EgoData & ego_data, const PlannerData & planner_data,
  const std::optional<geometry_msgs::msg::Pose> & previous_slowdown_pose);
}  // namespace autoware::motion_velocity_planner::out_of_lane
#endif  // CALCULATE_SLOWDOWN_POINTS_HPP_

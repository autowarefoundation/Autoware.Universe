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

#ifndef AUTOWARE__FREESPACE_PLANNER__UTILS_HPP_
#define AUTOWARE__FREESPACE_PLANNER__UTILS_HPP_

#include "autoware/freespace_planning_algorithms/abstract_algorithm.hpp"

#include <rclcpp/rclcpp.hpp>

#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <autoware_planning_msgs/msg/trajectory_point.hpp>
#include <geometry_msgs/msg/pose_array.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tier4_planning_msgs/msg/scenario.hpp>

#include <deque>
#include <vector>

namespace autoware::freespace_planner::utils
{
using autoware::freespace_planning_algorithms::PlannerWaypoint;
using autoware::freespace_planning_algorithms::PlannerWaypoints;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;
using geometry_msgs::msg::Pose;
using geometry_msgs::msg::PoseArray;
using geometry_msgs::msg::PoseStamped;
using geometry_msgs::msg::TransformStamped;
using nav_msgs::msg::Odometry;
using tier4_planning_msgs::msg::Scenario;

PoseArray trajectory_to_pose_array(const Trajectory & trajectory);

double calc_distance_2d(const Trajectory & trajectory, const Pose & pose);

Pose transform_pose(const Pose & pose, const TransformStamped & transform);

bool is_active(const Scenario::ConstSharedPtr & scenario);

std::vector<size_t> get_reversing_indices(const Trajectory & trajectory);

size_t get_next_target_index(
  const size_t trajectory_size, const std::vector<size_t> & reversing_indices,
  const size_t current_target_index);

Trajectory get_partial_trajectory(
  const Trajectory & trajectory, const size_t start_index, const size_t end_index,
  const rclcpp::Clock::SharedPtr clock);

Trajectory create_trajectory(
  const PoseStamped & current_pose, const PlannerWaypoints & planner_waypoints,
  const double & velocity);

Trajectory create_stop_trajectory(
  const PoseStamped & current_pose, const rclcpp::Clock::SharedPtr clock);

Trajectory create_stop_trajectory(const Trajectory & trajectory);

bool is_stopped(
  const std::deque<Odometry::ConstSharedPtr> & odom_buffer, const double th_stopped_velocity_mps);

bool is_near_target(
  const Pose & target_pose, const Pose & current_pose, const double th_distance_m);
}  // namespace autoware::freespace_planner::utils

#endif  // AUTOWARE__FREESPACE_PLANNER__UTILS_HPP_

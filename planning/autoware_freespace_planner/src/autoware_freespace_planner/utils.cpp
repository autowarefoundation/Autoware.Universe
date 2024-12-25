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

#include "autoware/freespace_planner/utils.hpp"

#include <autoware/motion_utils/trajectory/trajectory.hpp>

#include <deque>
#include <vector>

namespace autoware::freespace_planner::utils
{

PoseArray trajectory_to_pose_array(const Trajectory & trajectory)
{
  PoseArray pose_array;
  pose_array.header = trajectory.header;

  for (const auto & point : trajectory.points) {
    pose_array.poses.push_back(point.pose);
  }

  return pose_array;
}

double calc_distance_2d(const Trajectory & trajectory, const Pose & pose)
{
  const auto idx = autoware::motion_utils::findNearestIndex(trajectory.points, pose.position);
  return autoware::universe_utils::calcDistance2d(trajectory.points.at(idx), pose);
}

Pose transform_pose(const Pose & pose, const TransformStamped & transform)
{
  PoseStamped transformed_pose;
  PoseStamped orig_pose;
  orig_pose.pose = pose;
  tf2::doTransform(orig_pose, transformed_pose, transform);

  return transformed_pose.pose;
}

bool is_active(const Scenario::ConstSharedPtr & scenario)
{
  if (!scenario) return false;

  const auto & s = scenario->activating_scenarios;
  return std::find(std::begin(s), std::end(s), Scenario::PARKING) != std::end(s);
}

std::vector<size_t> get_reversing_indices(const Trajectory & trajectory)
{
  std::vector<size_t> indices;

  for (size_t i = 0; i < trajectory.points.size() - 1; ++i) {
    if (
      trajectory.points.at(i).longitudinal_velocity_mps *
        trajectory.points.at(i + 1).longitudinal_velocity_mps <
      0) {
      indices.push_back(i);
    }
  }

  return indices;
}

size_t get_next_target_index(
  const size_t trajectory_size, const std::vector<size_t> & reversing_indices,
  const size_t current_target_index)
{
  if (!reversing_indices.empty()) {
    for (const auto reversing_index : reversing_indices) {
      if (reversing_index > current_target_index) {
        return reversing_index;
      }
    }
  }

  return trajectory_size - 1;
}

Trajectory get_partial_trajectory(
  const Trajectory & trajectory, const size_t start_index, const size_t end_index,
  const rclcpp::Clock::SharedPtr clock)
{
  Trajectory partial_trajectory;
  partial_trajectory.header = trajectory.header;
  partial_trajectory.header.stamp = clock->now();

  partial_trajectory.points.reserve(trajectory.points.size());
  for (size_t i = start_index; i <= end_index; ++i) {
    partial_trajectory.points.push_back(trajectory.points.at(i));
  }

  // Modify velocity at start/end point
  if (partial_trajectory.points.size() >= 2) {
    partial_trajectory.points.front().longitudinal_velocity_mps =
      partial_trajectory.points.at(1).longitudinal_velocity_mps;
  }
  if (!partial_trajectory.points.empty()) {
    partial_trajectory.points.back().longitudinal_velocity_mps = 0;
  }

  return partial_trajectory;
}

Trajectory create_trajectory(
  const PoseStamped & current_pose, const PlannerWaypoints & planner_waypoints,
  const double & velocity)
{
  Trajectory trajectory;
  trajectory.header = planner_waypoints.header;

  for (const auto & awp : planner_waypoints.waypoints) {
    TrajectoryPoint point;

    point.pose = awp.pose.pose;

    point.pose.position.z = current_pose.pose.position.z;  // height = const
    point.longitudinal_velocity_mps = velocity / 3.6;      // velocity = const

    // switch sign by forward/backward
    point.longitudinal_velocity_mps = (awp.is_back ? -1 : 1) * point.longitudinal_velocity_mps;

    trajectory.points.push_back(point);
  }

  return trajectory;
}

Trajectory create_stop_trajectory(
  const PoseStamped & current_pose, const rclcpp::Clock::SharedPtr clock)
{
  PlannerWaypoints waypoints;
  PlannerWaypoint waypoint;

  waypoints.header.stamp = clock->now();
  waypoints.header.frame_id = current_pose.header.frame_id;
  waypoint.pose.header = waypoints.header;
  waypoint.pose.pose = current_pose.pose;
  waypoint.is_back = false;
  waypoints.waypoints.push_back(waypoint);

  return create_trajectory(current_pose, waypoints, 0.0);
}

Trajectory create_stop_trajectory(const Trajectory & trajectory)
{
  Trajectory stop_trajectory = trajectory;
  for (size_t i = 0; i < trajectory.points.size(); ++i) {
    stop_trajectory.points.at(i).longitudinal_velocity_mps = 0.0;
  }
  return stop_trajectory;
}

bool is_stopped(
  const std::deque<Odometry::ConstSharedPtr> & odom_buffer, const double th_stopped_velocity_mps)
{
  for (const auto & odom : odom_buffer) {
    if (std::abs(odom->twist.twist.linear.x) > th_stopped_velocity_mps) {
      return false;
    }
  }
  return true;
}

bool is_near_target(const Pose & target_pose, const Pose & current_pose, const double th_distance_m)
{
  const auto pose_dev = autoware::universe_utils::calcPoseDeviation(target_pose, current_pose);
  return abs(pose_dev.yaw) < M_PI_2 && abs(pose_dev.longitudinal) < th_distance_m &&
         abs(pose_dev.lateral) < th_distance_m;
}
}  // namespace autoware::freespace_planner::utils

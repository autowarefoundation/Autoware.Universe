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

#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <vector>

using autoware::freespace_planner::utils::Odometry;
using autoware::freespace_planner::utils::Scenario;
using autoware::freespace_planner::utils::Trajectory;
using autoware::freespace_planner::utils::TrajectoryPoint;
using autoware::freespace_planning_algorithms::PlannerWaypoint;
using autoware::freespace_planning_algorithms::PlannerWaypoints;

Trajectory get_trajectory(const size_t n_switches)
{
  const auto get_traj_point = [](const double x, const double y, const float v) {
    TrajectoryPoint point;
    point.pose.position.x = x;
    point.pose.position.y = y;
    point.longitudinal_velocity_mps = v;
    return point;
  };

  Trajectory trajectory;
  constexpr double eps = 0.001;
  constexpr double res = 0.5;

  const auto add_points = [&](const double length, const bool forward) {
    auto x = trajectory.points.empty() ? 0.0 : trajectory.points.back().pose.position.x;

    if (forward) {
      const auto target = x + length;
      for (; x < target + eps; x += res) {
        trajectory.points.push_back(get_traj_point(x, 0.0, 1.0));
      }
    } else {
      const auto target = x - length;
      for (; x > target - eps; x -= res) {
        trajectory.points.push_back(get_traj_point(x, 0.0, -1.0));
      }
    }
  };

  bool forward = true;
  for (size_t i = 0; i <= n_switches; ++i) {
    const auto length = forward ? 5.0 : 3.0;
    add_points(length, forward);
    forward = !forward;
  }

  return trajectory;
}

PlannerWaypoints get_waypoints(const size_t n_switches)
{
  const auto get_waypoint = [](const double x, const double y, const bool is_back) {
    PlannerWaypoint point;
    point.pose.pose.position.x = x;
    point.pose.pose.position.y = y;
    point.is_back = is_back;
    return point;
  };

  PlannerWaypoints waypoints;
  constexpr double eps = 0.001;
  constexpr double res = 0.5;

  const auto add_points = [&](const double length, const bool forward) {
    auto x = waypoints.waypoints.empty() ? 0.0 : waypoints.waypoints.back().pose.pose.position.x;

    if (forward) {
      const auto target = x + length;
      for (; x < target + eps; x += res) {
        waypoints.waypoints.push_back(get_waypoint(x, 0.0, !forward));
      }
    } else {
      const auto target = x - length;
      for (; x > target - eps; x -= res) {
        waypoints.waypoints.push_back(get_waypoint(x, 0.0, !forward));
      }
    }
  };

  bool forward = true;
  for (size_t i = 0; i <= n_switches; ++i) {
    const auto length = forward ? 5.0 : 3.0;
    add_points(length, forward);
    forward = !forward;
  }

  return waypoints;
}

TEST(FreespacePlannerUtilsTest, testIsActive)
{
  Scenario::ConstSharedPtr scenario_ptr;

  EXPECT_FALSE(autoware::freespace_planner::utils::is_active(scenario_ptr));

  Scenario scenario;
  scenario.current_scenario = Scenario::EMPTY;
  scenario_ptr = std::make_shared<Scenario>(scenario);
  EXPECT_FALSE(autoware::freespace_planner::utils::is_active(scenario_ptr));

  scenario.current_scenario = Scenario::LANEDRIVING;
  scenario_ptr = std::make_shared<Scenario>(scenario);
  EXPECT_FALSE(autoware::freespace_planner::utils::is_active(scenario_ptr));

  scenario.current_scenario = Scenario::PARKING;
  scenario.activating_scenarios.push_back(Scenario::PARKING);
  scenario_ptr = std::make_shared<Scenario>(scenario);
  EXPECT_TRUE(autoware::freespace_planner::utils::is_active(scenario_ptr));
}

TEST(FreespacePlannerUtilsTest, testIsStopped)
{
  std::deque<Odometry::ConstSharedPtr> odometry_buffer;
  const double th_stopped_velocity_mps = 0.01;
  EXPECT_TRUE(
    autoware::freespace_planner::utils::is_stopped(odometry_buffer, th_stopped_velocity_mps));

  Odometry odometry;
  odometry.twist.twist.linear.x = 0.0;
  odometry_buffer.push_back(std::make_shared<Odometry>(odometry));
  EXPECT_TRUE(
    autoware::freespace_planner::utils::is_stopped(odometry_buffer, th_stopped_velocity_mps));

  odometry.twist.twist.linear.x = 1.0;
  odometry_buffer.push_back(std::make_shared<Odometry>(odometry));
  EXPECT_FALSE(
    autoware::freespace_planner::utils::is_stopped(odometry_buffer, th_stopped_velocity_mps));
}

TEST(FreespacePlannerUtilsTest, testIsNearTarget)
{
  const auto trajectory = get_trajectory(0ul);
  const auto target_pose = trajectory.points.back().pose;

  static constexpr double eps = 0.01;
  auto current_pose = target_pose;
  current_pose.position.x -= 1.0;
  current_pose.position.y += 1.0;
  current_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(M_PI_2 + eps);

  const double th_distance_m = 0.5;

  EXPECT_FALSE(
    autoware::freespace_planner::utils::is_near_target(target_pose, current_pose, th_distance_m));

  current_pose.position.x += 0.6;
  EXPECT_FALSE(
    autoware::freespace_planner::utils::is_near_target(target_pose, current_pose, th_distance_m));

  current_pose.position.y -= 0.6;
  EXPECT_FALSE(
    autoware::freespace_planner::utils::is_near_target(target_pose, current_pose, th_distance_m));

  current_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(M_PI_4);
  EXPECT_TRUE(
    autoware::freespace_planner::utils::is_near_target(target_pose, current_pose, th_distance_m));
}

TEST(FreespacePlannerUtilsTest, testGetReversingIndices)
{
  auto trajectory = get_trajectory(0ul);
  auto reversing_indices = autoware::freespace_planner::utils::get_reversing_indices(trajectory);
  EXPECT_EQ(reversing_indices.size(), 0ul);

  trajectory = get_trajectory(1ul);
  reversing_indices = autoware::freespace_planner::utils::get_reversing_indices(trajectory);
  EXPECT_EQ(reversing_indices.size(), 1ul);
  if (!reversing_indices.empty()) {
    EXPECT_EQ(reversing_indices.front(), 10ul);
  }

  trajectory = get_trajectory(2ul);
  reversing_indices = autoware::freespace_planner::utils::get_reversing_indices(trajectory);
  EXPECT_EQ(reversing_indices.size(), 2ul);
  if (!reversing_indices.empty()) {
    EXPECT_EQ(reversing_indices.front(), 10ul);
    EXPECT_EQ(reversing_indices.back(), 17ul);
  }
}

TEST(FreespacePlannerUtilsTest, testGetNextTargetIndex)
{
  auto trajectory = get_trajectory(0ul);
  auto reversing_indices = autoware::freespace_planner::utils::get_reversing_indices(trajectory);
  auto current_target_index = 0ul;
  auto next_target_index = autoware::freespace_planner::utils::get_next_target_index(
    trajectory.points.size(), reversing_indices, current_target_index);
  EXPECT_EQ(next_target_index, trajectory.points.size() - 1);

  trajectory = get_trajectory(2ul);
  reversing_indices = autoware::freespace_planner::utils::get_reversing_indices(trajectory);
  ASSERT_EQ(reversing_indices.size(), 2ul);

  next_target_index = autoware::freespace_planner::utils::get_next_target_index(
    trajectory.points.size(), reversing_indices, current_target_index);
  EXPECT_EQ(next_target_index, reversing_indices.front());

  current_target_index = reversing_indices.front();
  next_target_index = autoware::freespace_planner::utils::get_next_target_index(
    trajectory.points.size(), reversing_indices, current_target_index);
  EXPECT_EQ(next_target_index, reversing_indices.back());
}

TEST(FreespacePlannerUtilsTest, testGetPartialTrajectory)
{
  const auto trajectory = get_trajectory(2ul);
  const auto reversing_indices =
    autoware::freespace_planner::utils::get_reversing_indices(trajectory);

  ASSERT_EQ(reversing_indices.size(), 2ul);
  auto partial_traj = autoware::freespace_planner::utils::get_partial_trajectory(
    trajectory, 0ul, reversing_indices.front(), std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME));
  ASSERT_FALSE(partial_traj.points.empty());
  auto expected_size = reversing_indices.front() + 1ul;
  EXPECT_EQ(partial_traj.points.size(), expected_size);
  EXPECT_TRUE(partial_traj.points.front().longitudinal_velocity_mps > 0.0);
  EXPECT_FLOAT_EQ(partial_traj.points.back().longitudinal_velocity_mps, 0.0);

  partial_traj = autoware::freespace_planner::utils::get_partial_trajectory(
    trajectory, reversing_indices.front(), reversing_indices.back(),
    std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME));
  ASSERT_FALSE(partial_traj.points.empty());
  expected_size = reversing_indices.back() - reversing_indices.front() + 1ul;
  EXPECT_EQ(partial_traj.points.size(), expected_size);
  EXPECT_TRUE(partial_traj.points.front().longitudinal_velocity_mps < 0.0);
  EXPECT_FLOAT_EQ(partial_traj.points.back().longitudinal_velocity_mps, 0.0);
}

TEST(FreespacePlannerUtilsTest, testCreateTrajectory)
{
  const auto waypoints = get_waypoints(1ul);

  geometry_msgs::msg::PoseStamped current_pose;
  current_pose.pose.position.z = 1.0;
  const double velocity = 1.0;

  const auto trajectory =
    autoware::freespace_planner::utils::create_trajectory(current_pose, waypoints, velocity);

  ASSERT_FALSE(trajectory.points.empty());
  EXPECT_EQ(trajectory.points.size(), waypoints.waypoints.size());
  EXPECT_EQ(trajectory.points.front().pose.position.z, current_pose.pose.position.z);
  EXPECT_EQ(trajectory.points.back().pose.position.z, current_pose.pose.position.z);
  EXPECT_TRUE(trajectory.points.front().longitudinal_velocity_mps > 0.0);
  EXPECT_TRUE(trajectory.points.back().longitudinal_velocity_mps < 0.0);
}

TEST(FreespacePlannerUtilsTest, testCreateStopTrajectory)
{
  geometry_msgs::msg::PoseStamped current_pose;
  current_pose.pose.position.x = 1.0;

  auto stop_traj = autoware::freespace_planner::utils::create_stop_trajectory(
    current_pose, std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME));
  EXPECT_EQ(stop_traj.points.size(), 1ul);
  if (!stop_traj.points.empty()) {
    EXPECT_DOUBLE_EQ(stop_traj.points.front().pose.position.x, 1.0);
    EXPECT_DOUBLE_EQ(stop_traj.points.front().longitudinal_velocity_mps, 0.0);
  }

  const auto trajectory = get_trajectory(0ul);
  stop_traj = autoware::freespace_planner::utils::create_stop_trajectory(trajectory);
  EXPECT_EQ(stop_traj.points.size(), trajectory.points.size());
  if (!stop_traj.points.empty()) {
    EXPECT_DOUBLE_EQ(
      stop_traj.points.front().pose.position.x, trajectory.points.front().pose.position.x);
    EXPECT_DOUBLE_EQ(
      stop_traj.points.back().pose.position.x, trajectory.points.back().pose.position.x);
    EXPECT_DOUBLE_EQ(stop_traj.points.front().longitudinal_velocity_mps, 0.0);
    EXPECT_DOUBLE_EQ(stop_traj.points.back().longitudinal_velocity_mps, 0.0);
  }
}

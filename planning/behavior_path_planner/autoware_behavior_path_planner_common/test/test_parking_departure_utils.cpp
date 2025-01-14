// Copyright 2024 Tier IV, Inc. All rights reserved.
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

#include "autoware/behavior_path_planner_common/data_manager.hpp"
#include "autoware/behavior_path_planner_common/utils/parking_departure/utils.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/path_safety_checker_parameters.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

constexpr double epsilon = 1e-6;

using autoware::behavior_path_planner::PlannerData;
using autoware_planning_msgs::msg::Trajectory;
using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;

using autoware::test_utils::generateTrajectory;

PathWithLaneId trajectory_to_path_with_lane_id(const Trajectory & trajectory)
{
  PathWithLaneId path_with_lane_id;
  PathPointWithLaneId path_point_with_lane_id;
  for (const auto & point : trajectory.points) {
    path_point_with_lane_id.point.pose = point.pose;
    path_point_with_lane_id.point.lateral_velocity_mps = point.lateral_velocity_mps;
    path_point_with_lane_id.point.longitudinal_velocity_mps = point.longitudinal_velocity_mps;
    path_point_with_lane_id.point.heading_rate_rps = point.heading_rate_rps;
    path_with_lane_id.points.push_back(path_point_with_lane_id);
  }
  return path_with_lane_id;
}

TEST(BehaviorPathPlanningParkingDepartureUtil, calcFeasibleDecelDistance)
{
  using autoware::behavior_path_planner::utils::parking_departure::calcFeasibleDecelDistance;

  auto data = std::make_shared<PlannerData>();
  double velocity = 2.0;
  double acceleration = 1.0;
  double acceleration_limit = 2.0;
  double jerk_limit = 1.0;
  double target_velocity = 3.0;

  auto odometry = std::make_shared<nav_msgs::msg::Odometry>();
  odometry->pose.pose = autoware::test_utils::createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  odometry->twist.twist.linear.x = velocity;
  auto accel_with_covariance =
    std::make_shared<autoware::behavior_path_planner::AccelWithCovarianceStamped>();
  accel_with_covariance->accel.accel.linear.x = acceleration;
  data->self_odometry = odometry;
  data->self_acceleration = accel_with_covariance;
  auto planner_data = std::static_pointer_cast<const PlannerData>(data);

  // condition: current velocity is slower than target velocity
  auto distance =
    calcFeasibleDecelDistance(planner_data, acceleration_limit, jerk_limit, target_velocity);
  ASSERT_TRUE(distance.has_value());
  EXPECT_DOUBLE_EQ(distance.value(), 0.0);

  // condition: calculates deceleration distance
  velocity = 5.0;
  odometry->twist.twist.linear.x = velocity;
  data->self_odometry = odometry;
  planner_data = std::static_pointer_cast<const PlannerData>(data);
  distance =
    calcFeasibleDecelDistance(planner_data, acceleration_limit, jerk_limit, target_velocity);
  ASSERT_TRUE(distance.has_value());
  EXPECT_NEAR(distance.value(), 18.7730133, epsilon);

  // condition: not valid condition
  velocity = 0.3;
  target_velocity = 0.0;
  acceleration = -1.5;
  jerk_limit = 0.25;
  acceleration_limit = 2.0;
  odometry->twist.twist.linear.x = velocity;
  accel_with_covariance->accel.accel.linear.x = acceleration;
  data->self_odometry = odometry;
  data->self_acceleration = accel_with_covariance;
  planner_data = std::static_pointer_cast<const PlannerData>(data);
  distance =
    calcFeasibleDecelDistance(planner_data, acceleration_limit, jerk_limit, target_velocity);
  ASSERT_FALSE(distance.has_value());
}

TEST(BehaviorPathPlanningParkingDepartureUtil, modifyVelocityByDirection)
{
  using autoware::behavior_path_planner::utils::parking_departure::modifyVelocityByDirection;

  const double velocity = 3.0;
  const double target_velocity = 2.0;
  const double acceleration = 2.0;

  std::vector<PathWithLaneId> paths;
  auto short_path = trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(1, 1.0));
  auto long_path =
    trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(10, 1.0, -velocity));
  auto reverse_path =
    trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(10, -1.0, velocity, -M_PI));

  paths.push_back(short_path);
  paths.push_back(long_path);
  paths.push_back(reverse_path);

  std::vector<std::pair<double, double>> terminal_vel_acc_pairs;
  terminal_vel_acc_pairs.emplace_back(0.5, 0.5);
  terminal_vel_acc_pairs.emplace_back(1.0, 1.0);
  terminal_vel_acc_pairs.emplace_back(1.5, 1.5);

  modifyVelocityByDirection(paths, terminal_vel_acc_pairs, target_velocity, acceleration);

  // condition: number of point less than 2
  EXPECT_DOUBLE_EQ(terminal_vel_acc_pairs.at(0).first, 0.0);
  EXPECT_DOUBLE_EQ(terminal_vel_acc_pairs.at(0).second, 0.0);

  // condition: forward driving
  EXPECT_DOUBLE_EQ(terminal_vel_acc_pairs.at(1).first, target_velocity);
  EXPECT_DOUBLE_EQ(terminal_vel_acc_pairs.at(1).second, acceleration);
  for (const auto & point : paths.at(1).points) {
    if (point == paths.at(1).points.back())
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, 0.0);
    else
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, velocity);
  }

  // condition: reverse driving
  EXPECT_DOUBLE_EQ(terminal_vel_acc_pairs.at(2).first, -target_velocity);
  EXPECT_DOUBLE_EQ(terminal_vel_acc_pairs.at(2).second, -acceleration);
  for (const auto & point : paths.at(2).points) {
    if (point == paths.at(2).points.back())
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, 0.0);
    else
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, -velocity);
  }
}

TEST(BehaviorPathPlanningParkingDepartureUtil, updatePathProperty)
{
  using autoware::behavior_path_planner::utils::parking_departure::EgoPredictedPathParams;
  using autoware::behavior_path_planner::utils::parking_departure::updatePathProperty;

  auto params = std::make_shared<EgoPredictedPathParams>();
  params->min_acceleration = 1.0;
  params->acceleration = 1.5;
  params->max_velocity = 0.0;
  auto pair_terminal_velocity_and_accel = std::make_pair(3.0, 2.0);

  updatePathProperty(params, pair_terminal_velocity_and_accel);
  EXPECT_DOUBLE_EQ(params->max_velocity, 3.0);
  EXPECT_DOUBLE_EQ(params->acceleration, 2.0);
}

TEST(BehaviorPathPlanningParkingDepartureUtil, initializeCollisionCheckDebugMap)
{
  using autoware::behavior_path_planner::utils::parking_departure::initializeCollisionCheckDebugMap;

  autoware::behavior_path_planner::CollisionCheckDebugMap debug_map;
  auto uuid1 = autoware::universe_utils::toBoostUUID(autoware::universe_utils::generateUUID());
  autoware::behavior_path_planner::utils::path_safety_checker::CollisionCheckDebug debug_info;
  debug_map[uuid1] = debug_info;

  initializeCollisionCheckDebugMap(debug_map);
  ASSERT_TRUE(debug_map.empty());
}

TEST(BehaviorPathPlanningParkingDepartureUtil, getPairsTerminalVelocityAndAccel)
{
  using autoware::behavior_path_planner::utils::parking_departure::getPairsTerminalVelocityAndAccel;

  std::vector<std::pair<double, double>> pairs_terminal_velocity_and_accel;
  pairs_terminal_velocity_and_accel.emplace_back(2.0, 1.0);
  pairs_terminal_velocity_and_accel.emplace_back(0.05, -1.0);

  // condition: current path idx exceeds pairs size
  auto pair = getPairsTerminalVelocityAndAccel(pairs_terminal_velocity_and_accel, 2);
  EXPECT_DOUBLE_EQ(pair.first, 0.0);
  EXPECT_DOUBLE_EQ(pair.first, 0.0);

  // condition: get current idx pair
  pair = getPairsTerminalVelocityAndAccel(pairs_terminal_velocity_and_accel, 1);
  EXPECT_DOUBLE_EQ(pair.first, 0.05);
  EXPECT_DOUBLE_EQ(pair.second, -1.0);
}

TEST(BehaviorPathPlanningParkingDepartureUtil, generateFeasibleStopPath)
{
  using autoware::behavior_path_planner::utils::parking_departure::generateFeasibleStopPath;

  auto data = std::make_shared<PlannerData>();
  double velocity = 0.3;
  double acceleration = -1.5;
  double maximum_jerk = 0.25;
  double maximum_deceleration = 2.0;

  auto odometry = std::make_shared<nav_msgs::msg::Odometry>();
  odometry->pose.pose = autoware::test_utils::createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  odometry->twist.twist.linear.x = velocity;
  auto accel_with_covariance =
    std::make_shared<autoware::behavior_path_planner::AccelWithCovarianceStamped>();
  accel_with_covariance->accel.accel.linear.x = acceleration;
  data->self_odometry = odometry;
  data->self_acceleration = accel_with_covariance;
  auto planner_data = std::static_pointer_cast<const PlannerData>(data);

  autoware::behavior_path_planner::PoseWithDetailOpt stop_pose{std::nullopt};

  // condition: empty path
  PathWithLaneId path;
  auto stop_path =
    generateFeasibleStopPath(path, planner_data, stop_pose, maximum_deceleration, maximum_jerk);
  EXPECT_FALSE(stop_path.has_value());

  // condition: not valid condition for stop distance
  path = trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(10, 1.0, velocity));
  stop_path =
    generateFeasibleStopPath(path, planner_data, stop_pose, maximum_deceleration, maximum_jerk);
  EXPECT_FALSE(stop_path.has_value());

  // condition: not valid condition for stop index
  velocity = 5.0;
  acceleration = 1.0;
  odometry->twist.twist.linear.x = velocity;
  accel_with_covariance->accel.accel.linear.x = acceleration;
  data->self_odometry = odometry;
  data->self_acceleration = accel_with_covariance;
  planner_data = std::static_pointer_cast<const PlannerData>(data);
  path = trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(10, 1.0, velocity));
  stop_path =
    generateFeasibleStopPath(path, planner_data, stop_pose, maximum_deceleration, maximum_jerk);
  EXPECT_FALSE(stop_path.has_value());

  // condition: valid condition
  maximum_jerk = 5.0;
  maximum_deceleration = -3.0;
  stop_path =
    generateFeasibleStopPath(path, planner_data, stop_pose, maximum_deceleration, maximum_jerk);
  size_t i = 0;
  ASSERT_TRUE(stop_path.has_value());
  for (const auto & point : stop_path->points) {
    if (i < 7)
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, velocity);
    else
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, 0.0);
    i++;
  }
}

TEST(BehaviorPathPlanningParkingDepartureUtil, calcEndArcLength)
{
  using autoware::behavior_path_planner::utils::parking_departure::calcEndArcLength;

  lanelet::LineString3d left_bound;
  lanelet::LineString3d right_bound;

  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, -1, -1});
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 0, -1});
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, -1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, -1, 1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 0, 1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, 1});
  lanelet::Lanelet lanelet{lanelet::InvalId, left_bound, right_bound};

  lanelet::LineString3d centerline;
  centerline.push_back(lanelet::Point3d{lanelet::InvalId, -1, 0});
  centerline.push_back(lanelet::Point3d{lanelet::InvalId, 0, 0});
  centerline.push_back(lanelet::Point3d{lanelet::InvalId, 1, 0});
  lanelet.setCenterline(centerline);

  lanelet::ConstLanelets road_lanes = {lanelet};

  double s_start = 0.2;
  double forward_path_length = 0.1;
  auto goal_pose = autoware::test_utils::createPose(5.0, 5.0, 0.0, 0.0, 0.0, 0.0);

  // condition: goal pose not in lanelets
  auto end_arc = calcEndArcLength(s_start, forward_path_length, road_lanes, goal_pose);
  EXPECT_DOUBLE_EQ(end_arc.first, s_start + forward_path_length);
  EXPECT_FALSE(end_arc.second);

  // condition: goal pose behind start
  goal_pose.position.x = -0.9;
  goal_pose.position.y = 0.0;
  end_arc = calcEndArcLength(s_start, forward_path_length, road_lanes, goal_pose);
  EXPECT_DOUBLE_EQ(end_arc.first, s_start + forward_path_length);
  EXPECT_FALSE(end_arc.second);

  // condition: goal pose beyond start
  goal_pose.position.x = 0.0;
  end_arc = calcEndArcLength(s_start, forward_path_length, road_lanes, goal_pose);
  EXPECT_DOUBLE_EQ(end_arc.first, s_start + forward_path_length);
  EXPECT_FALSE(end_arc.second);

  // condition: path end is goal
  goal_pose.position.x = -0.75;
  end_arc = calcEndArcLength(s_start, forward_path_length, road_lanes, goal_pose);
  EXPECT_DOUBLE_EQ(end_arc.first, 0.25);
  EXPECT_TRUE(end_arc.second);
}

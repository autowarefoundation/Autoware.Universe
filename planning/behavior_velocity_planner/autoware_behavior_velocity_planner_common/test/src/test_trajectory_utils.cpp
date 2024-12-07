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

#include "autoware/behavior_velocity_planner_common/utilization/trajectory_utils.hpp"
#include "autoware/velocity_smoother/smoother/jerk_filtered_smoother.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <rclcpp/node.hpp>

#include <geometry_msgs/msg/accel_stamped.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>

#include <gtest/gtest.h>

#include <memory>

TEST(smoothPath, nominal)
{
  using autoware::behavior_velocity_planner::smoothPath;
  using tier4_planning_msgs::msg::PathPointWithLaneId;
  using tier4_planning_msgs::msg::PathWithLaneId;

  rclcpp::init(0, nullptr);
  rclcpp::NodeOptions options;
  options.arguments(
    {"--ros-args", "--params-file",
     ament_index_cpp::get_package_share_directory("autoware_behavior_velocity_planner_common") +
       "/config/behavior_velocity_planner_common.param.yaml",
     "--params-file",
     ament_index_cpp::get_package_share_directory("autoware_test_utils") +
       "/config/test_vehicle_info.param.yaml",
     "--params-file",
     ament_index_cpp::get_package_share_directory("autoware_velocity_smoother") +
       "/config/default_common.param.yaml",
     "--params-file",
     ament_index_cpp::get_package_share_directory("autoware_velocity_smoother") +
       "/config/default_velocity_smoother.param.yaml",
     "--params-file",
     ament_index_cpp::get_package_share_directory("autoware_velocity_smoother") +
       "/config/JerkFiltered.param.yaml"});
  auto node = std::make_shared<rclcpp::Node>("test_node", options);

  auto planner_data = std::make_shared<autoware::behavior_velocity_planner::PlannerData>(*node);
  planner_data->stop_line_extend_length = 5.0;
  planner_data->vehicle_info_.max_longitudinal_offset_m = 1.0;

  planner_data->current_odometry = std::make_shared<geometry_msgs::msg::PoseStamped>([]() {
    geometry_msgs::msg::PoseStamped pose;
    pose.pose.position.x = 2.0;
    pose.pose.position.y = 1.0;
    return pose;
  }());

  planner_data->current_velocity = std::make_shared<geometry_msgs::msg::TwistStamped>([]() {
    geometry_msgs::msg::TwistStamped twist;
    twist.twist.linear.x = 3.0;
    return twist;
  }());

  planner_data->current_acceleration =
    std::make_shared<geometry_msgs::msg::AccelWithCovarianceStamped>([]() {
      geometry_msgs::msg::AccelWithCovarianceStamped accel;
      accel.accel.accel.linear.x = 1.0;
      return accel;
    }());

  planner_data->velocity_smoother_ =
    std::make_shared<autoware::velocity_smoother::JerkFilteredSmoother>(
      *node, std::make_shared<autoware::universe_utils::TimeKeeper>());

  // Input path
  PathWithLaneId in_path;
  for (double i = 0; i <= 10.0; i += 1.0) {
    PathPointWithLaneId point;
    point.point.pose.position.x = i;
    point.point.pose.position.y = 0.0;
    point.point.longitudinal_velocity_mps = 5.0;  // Set constant velocity
    in_path.points.emplace_back(point);
  }

  // Output path
  PathWithLaneId out_path;

  // Execute smoothing
  auto result = smoothPath(in_path, out_path, planner_data);

  // Check results
  EXPECT_TRUE(result);

  // Check initial and last points
  EXPECT_DOUBLE_EQ(out_path.points.front().point.pose.position.x, 0.0);
  EXPECT_DOUBLE_EQ(out_path.points.front().point.pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(out_path.points.back().point.pose.position.x, 10.0);
  EXPECT_DOUBLE_EQ(out_path.points.back().point.pose.position.y, 0.0);

  for (auto & point : out_path.points) {
    // Check velocities
    EXPECT_LE(
      point.point.longitudinal_velocity_mps,
      5.0);  // Smoothed velocity must not exceed initial
  }
}

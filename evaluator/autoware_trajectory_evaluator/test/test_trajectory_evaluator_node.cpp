// Copyright 2025 TIER IV, Inc.
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

#include "autoware/trajectory_evaluator/trajectory_evaluator.hpp"

#include "autoware_planning_msgs/msg/trajectory.hpp"
#include "autoware_planning_msgs/msg/trajectory_point.hpp"
#include "tier4_metric_msgs/msg/metric_array.hpp"

#include <gtest/gtest.h>

#include <cmath>

using MetricArrayMsg = tier4_metric_msgs::msg::MetricArray;
using MetricMsg = tier4_metric_msgs::msg::Metric;

class TrajectoryEvaluatorTests : public ::testing::Test
{
protected:
  void simulate_ego_vehicle(
    const autoware_planning_msgs::msg::Trajectory & trajectory,
    std::vector<trajectory_evaluator::TrajectoryWithTimestamp> & trajectory_history,
    std::vector<trajectory_evaluator::TimeErrorData> & time_errors, MetricArrayMsg & metrics_msg)
  {
    nav_msgs::msg::Odometry odometry_msg;
    odometry_msg.header.frame_id = "map";

    double current_time = 0.0;

    for (size_t i = 1; i < trajectory.points.size(); ++i) {
      const auto & previous_point = trajectory.points[i - 1];
      const auto & current_point = trajectory.points[i];

      double dx = current_point.pose.position.x - previous_point.pose.position.x;
      double dy = current_point.pose.position.y - previous_point.pose.position.y;
      double distance = std::sqrt(dx * dx + dy * dy);

      double velocity = current_point.longitudinal_velocity_mps;

      double travel_time = distance / velocity;
      double interval_time = travel_time;

      int num_steps = 10;

      for (int step = 0; step < num_steps; ++step) {
        double t = step / num_steps;

        odometry_msg.pose.pose.position.x = previous_point.pose.position.x + t * dx;
        odometry_msg.pose.pose.position.y = previous_point.pose.position.y + t * dy;
        odometry_msg.pose.pose.position.z = previous_point.pose.position.z;
        odometry_msg.twist.twist.linear.x = velocity;

        odometry_msg.header.stamp.sec = static_cast<int>(current_time);
        odometry_msg.header.stamp.nanosec =
          static_cast<int>((current_time - std::floor(current_time)) * 1e9);

        on_kinematic_state(
          std::make_shared<nav_msgs::msg::Odometry>(odometry_msg), trajectory_history, time_errors,
          metrics_msg);

        current_time += interval_time;
      }
    }
  }

  autoware_planning_msgs::msg::Trajectory generate_linear_trajectory(
    double length, int num_points, double velocity, std::string case_type)
  {
    autoware_planning_msgs::msg::Trajectory trajectory;

    double distance_per_point = length / (num_points - 1);

    for (int i = 0; i < num_points; ++i) {
      autoware_planning_msgs::msg::TrajectoryPoint point;
      point.pose.position.x = i * distance_per_point;
      point.pose.position.y = 0.0;
      point.pose.position.z = 0.0;

      if (case_type == "stopped") {
        point.longitudinal_velocity_mps = 0.0;

      } else if (case_type == "constant") {
        point.longitudinal_velocity_mps = velocity;

      } else if (case_type == "accelerating") {
        double factor = static_cast<double>(i) / (num_points - 1);
        point.longitudinal_velocity_mps = velocity * factor;

      } else if (case_type == "decelerating") {
        double factor = 1.0 - (static_cast<double>(i) / (num_points - 1));
        point.longitudinal_velocity_mps = velocity * factor;
      }

      trajectory.points.push_back(point);
    }

    return trajectory;
  }
};

TEST_F(TrajectoryEvaluatorTests, TestLinearTrajectory)
{
  double length = 50.0;
  int num_points = 50;
  double velocity = 5.0;

  auto trajectory = generate_linear_trajectory(length, num_points, velocity, "constant");

  std::vector<trajectory_evaluator::TrajectoryWithTimestamp> trajectory_history;
  std::vector<trajectory_evaluator::TimeErrorData> time_errors;
  MetricArrayMsg metrics_msg;

  nav_msgs::msg::Odometry initial_odom;
  initial_odom.header.stamp.sec = 0;
  initial_odom.header.stamp.nanosec = 0;

  store_trajectory(
    std::make_shared<autoware_planning_msgs::msg::Trajectory>(trajectory),
    std::make_shared<nav_msgs::msg::Odometry>(initial_odom), trajectory_history, 10);

  simulate_ego_vehicle(trajectory, trajectory_history, time_errors, metrics_msg);

  ASSERT_FALSE(time_errors.empty());

  for (const auto & error : time_errors) {
    EXPECT_NEAR(std::fabs(error.time_error), 0.0, 1e-6) << "Time error exceeds tolerance.";
  }
}

TEST_F(TrajectoryEvaluatorTests, TestStop)
{
  double length = 50.0;
  int num_points = 50;
  double velocity = 5.0;

  auto trajectory = generate_linear_trajectory(length, num_points, velocity, "stopped");

  std::vector<trajectory_evaluator::TrajectoryWithTimestamp> trajectory_history;
  std::vector<trajectory_evaluator::TimeErrorData> time_errors;
  MetricArrayMsg metrics_msg;

  nav_msgs::msg::Odometry initial_odom;
  initial_odom.header.stamp.sec = 0;
  initial_odom.header.stamp.nanosec = 0;

  store_trajectory(
    std::make_shared<autoware_planning_msgs::msg::Trajectory>(trajectory),
    std::make_shared<nav_msgs::msg::Odometry>(initial_odom), trajectory_history, 10);

  simulate_ego_vehicle(trajectory, trajectory_history, time_errors, metrics_msg);

  ASSERT_TRUE(time_errors.empty());
}

TEST_F(TrajectoryEvaluatorTests, TestAccelerating)
{
  double length = 50.0;
  int num_points = 50;
  double velocity = 5.0;

  auto trajectory = generate_linear_trajectory(length, num_points, velocity, "accelerating");

  std::vector<trajectory_evaluator::TrajectoryWithTimestamp> trajectory_history;
  std::vector<trajectory_evaluator::TimeErrorData> time_errors;
  MetricArrayMsg metrics_msg;

  nav_msgs::msg::Odometry initial_odom;
  initial_odom.header.stamp.sec = 0;
  initial_odom.header.stamp.nanosec = 0;

  store_trajectory(
    std::make_shared<autoware_planning_msgs::msg::Trajectory>(trajectory),
    std::make_shared<nav_msgs::msg::Odometry>(initial_odom), trajectory_history, 10);

  simulate_ego_vehicle(trajectory, trajectory_history, time_errors, metrics_msg);

  ASSERT_FALSE(time_errors.empty());

  for (const auto & error : time_errors) {
    EXPECT_NEAR(std::fabs(error.time_error), 0.0, 1e-6) << "Time error exceeds tolerance.";
  }
}

TEST_F(TrajectoryEvaluatorTests, TestDecelerating)
{
  double length = 50.0;
  int num_points = 50;
  double velocity = 5.0;

  auto trajectory = generate_linear_trajectory(length, num_points, velocity, "decelerating");

  std::vector<trajectory_evaluator::TrajectoryWithTimestamp> trajectory_history;
  std::vector<trajectory_evaluator::TimeErrorData> time_errors;
  MetricArrayMsg metrics_msg;

  nav_msgs::msg::Odometry initial_odom;
  initial_odom.header.stamp.sec = 0;
  initial_odom.header.stamp.nanosec = 0;

  store_trajectory(
    std::make_shared<autoware_planning_msgs::msg::Trajectory>(trajectory),
    std::make_shared<nav_msgs::msg::Odometry>(initial_odom), trajectory_history, 10);

  simulate_ego_vehicle(trajectory, trajectory_history, time_errors, metrics_msg);

  ASSERT_FALSE(time_errors.empty());
  for (const auto & error : time_errors) {
    EXPECT_NEAR(std::fabs(error.time_error), 0.0, 1e-6) << "Time error exceeds tolerance.";
  }
}

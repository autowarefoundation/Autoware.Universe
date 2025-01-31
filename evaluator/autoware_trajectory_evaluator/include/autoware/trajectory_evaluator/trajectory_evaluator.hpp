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

#ifndef AUTOWARE__TRAJECTORY_EVALUATOR__TRAJECTORY_EVALUATOR_HPP_
#define AUTOWARE__TRAJECTORY_EVALUATOR__TRAJECTORY_EVALUATOR_HPP_

#include "autoware/motion_utils/trajectory/trajectory.hpp"
#include "autoware/universe_utils/geometry/geometry.hpp"

#include <rclcpp/duration.hpp>
#include <rclcpp/time.hpp>

#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tier4_metric_msgs/msg/metric.hpp>
#include <tier4_metric_msgs/msg/metric_array.hpp>

#include <chrono>
#include <cmath>
#include <limits>
#include <string>
#include <vector>

using MetricMsg = tier4_metric_msgs::msg::Metric;
using MetricArrayMsg = tier4_metric_msgs::msg::MetricArray;

namespace trajectory_evaluator
{
struct TrajectoryPointWithTime
{
  autoware_planning_msgs::msg::TrajectoryPoint trajectory_point;
  double time_from_start;
  double time_stamp;

  TrajectoryPointWithTime(
    const autoware_planning_msgs::msg::TrajectoryPoint & point, const double & duration,
    const double & time_stamp)
  : trajectory_point(point), time_from_start(duration), time_stamp(time_stamp)
  {
  }
};

struct TrajectoryWithTimestamp
{
  std::vector<TrajectoryPointWithTime> trajectory_points;
  double time_stamp;
};

struct TimeErrorData
{
  double stamp;
  size_t trajectory_index;
  geometry_msgs::msg::Point position;
  double expected_time;
  double actual_time;
  double time_error;
};

void store_trajectory(
  const autoware_planning_msgs::msg::Trajectory::ConstSharedPtr traj_msg,
  const nav_msgs::msg::Odometry::ConstSharedPtr odom_msg,
  std::vector<TrajectoryWithTimestamp> & trajectory_history, size_t traj_history_limit);

void calculate_time_from_start(
  TrajectoryWithTimestamp & trajectory, const geometry_msgs::msg::Point & current_ego_point,
  const float min_velocity);

void on_kinematic_state(
  const nav_msgs::msg::Odometry::ConstSharedPtr odom_msg,
  std::vector<TrajectoryWithTimestamp> & trajectory_history,
  std::vector<TimeErrorData> & time_errors, MetricArrayMsg & metrics_msg);

}  // namespace trajectory_evaluator

#endif  // AUTOWARE__TRAJECTORY_EVALUATOR__TRAJECTORY_EVALUATOR_HPP_

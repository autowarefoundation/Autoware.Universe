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

#ifndef AUTOWARE__TRAJECTORY_EVALUATOR__TRAJECTORY_EVALUATOR_NODE_HPP_
#define AUTOWARE__TRAJECTORY_EVALUATOR__TRAJECTORY_EVALUATOR_NODE_HPP_

#include "autoware/motion_utils/trajectory/trajectory.hpp"
#include "autoware/trajectory_evaluator/trajectory_evaluator.hpp"  // Header for the evaluator functions
#include "autoware/universe_utils/geometry/geometry.hpp"
#include "rclcpp/rclcpp.hpp"

#include <autoware/universe_utils/ros/polling_subscriber.hpp>

#include "autoware_planning_msgs/msg/trajectory.hpp"
#include "autoware_planning_msgs/msg/trajectory_point.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <tier4_metric_msgs/msg/metric.hpp>
#include <tier4_metric_msgs/msg/metric_array.hpp>

#include <chrono>
#include <cmath>
#include <deque>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using MetricMsg = tier4_metric_msgs::msg::Metric;
using MetricArrayMsg = tier4_metric_msgs::msg::MetricArray;

namespace trajectory_evaluator
{
namespace node
{
class TrajectoryEvaluatorNode : public rclcpp::Node
{
public:
  explicit TrajectoryEvaluatorNode(const rclcpp::NodeOptions & node_options);
  ~TrajectoryEvaluatorNode();

  void store_trajectory(
    const autoware_planning_msgs::msg::Trajectory::ConstSharedPtr traj_msg,
    const nav_msgs::msg::Odometry::ConstSharedPtr odom_msg);

  void on_kinematic_state(const nav_msgs::msg::Odometry::ConstSharedPtr odom_msg);

  bool is_pose_equal(
    const geometry_msgs::msg::Pose & pose1, const geometry_msgs::msg::Pose & pose2);

private:
  std::shared_ptr<rclcpp::TimerBase> traj_timer_;
  std::shared_ptr<rclcpp::TimerBase> odom_timer_;
  void on_timer();

  std::string output_file_str_;
  double traj_history_limit;

  autoware::universe_utils::InterProcessPollingSubscriber<autoware_planning_msgs::msg::Trajectory>
    traj_sub_{this, "/planning/scenario_planning/trajectory"};
  autoware::universe_utils::InterProcessPollingSubscriber<nav_msgs::msg::Odometry>
    kinematic_state_sub_{this, "/localization/kinematic_state"};

  std::vector<TrajectoryWithTimestamp> trajectory_history_;
  rclcpp::TimerBase::SharedPtr timer_;
  std::vector<TimeErrorData> time_errors_;
  std::vector<rclcpp::Time> stamps_;

  rclcpp::Publisher<MetricArrayMsg>::SharedPtr metrics_pub_;
  MetricArrayMsg metrics_msg_;
};
}  // namespace node
}  // namespace trajectory_evaluator

#endif  // AUTOWARE__TRAJECTORY_EVALUATOR__TRAJECTORY_EVALUATOR_NODE_HPP_

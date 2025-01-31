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

#include "autoware/trajectory_evaluator/trajectory_evaluator_node.hpp"

#include "autoware/trajectory_evaluator/trajectory_evaluator.hpp"

namespace trajectory_evaluator
{
namespace node
{
TrajectoryEvaluatorNode::TrajectoryEvaluatorNode(const rclcpp::NodeOptions & node_options)
: Node("trajectory_evaluator", node_options)
{
  output_file_str_ = declare_parameter<std::string>("output_file");

  traj_history_limit = 10;

  timer_ = rclcpp::create_timer(
    this, get_clock(), std::chrono::milliseconds(100),
    std::bind(&TrajectoryEvaluatorNode::on_timer, this));
  metrics_pub_ = create_publisher<MetricArrayMsg>("~/metrics", 1);

  traj_sub_ = autoware::universe_utils::InterProcessPollingSubscriber<
    autoware_planning_msgs::msg::Trajectory>(this, "/planning/scenario_planning/trajectory");
  kinematic_state_sub_ =
    autoware::universe_utils::InterProcessPollingSubscriber<nav_msgs::msg::Odometry>(
      this, "/localization/kinematic_state");
}

TrajectoryEvaluatorNode::~TrajectoryEvaluatorNode()
{
  if (!output_file_str_.empty()) {
    std::ofstream f(output_file_str_);
    f << std::fixed << std::left;
    f << "Stamp(ns),TrajectoryIndex,PositionX,PositionY,PositionZ,ExpectedTime,ActualTime,"
         "TimeError\n";

    for (const auto & entry : time_errors_) {
      f << entry.stamp << "," << entry.trajectory_index << "," << entry.position.x << ","
        << entry.position.y << "," << entry.position.z << "," << entry.expected_time << ","
        << entry.actual_time << "," << entry.time_error << "\n";
    }
    f.close();
  }
}

void TrajectoryEvaluatorNode::on_timer()
{
  auto current_time = now();
  const auto ego_state_ptr = kinematic_state_sub_.takeData();
  const auto traj_msg = traj_sub_.takeData();

  if (traj_msg && ego_state_ptr) {
    trajectory_evaluator::store_trajectory(
      traj_msg, ego_state_ptr, trajectory_history_, traj_history_limit);
    trajectory_evaluator::on_kinematic_state(
      ego_state_ptr, trajectory_history_, time_errors_, metrics_msg_);
  }
  metrics_msg_.stamp = now();
  metrics_pub_->publish(metrics_msg_);
  metrics_msg_ = MetricArrayMsg{};
}

}  // namespace node
}  // namespace trajectory_evaluator

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(trajectory_evaluator::node::TrajectoryEvaluatorNode)

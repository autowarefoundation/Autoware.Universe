// Copyright 2024 Tier IV, Inc.
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

#include "autoware/control_evaluator/control_evaluator_node.hpp"

#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>

#include <limits>
#include <string>
#include <vector>

namespace control_diagnostics
{
ControlEvaluatorNode::ControlEvaluatorNode(const rclcpp::NodeOptions & node_options)
: Node("control_evaluator", node_options)
{
  using std::placeholders::_1;

  // Publisher
  processing_time_pub_ =
    this->create_publisher<tier4_debug_msgs::msg::Float64Stamped>("~/debug/processing_time_ms", 1);
  metrics_pub_ = create_publisher<MetricArrayMsg>("~/metrics", 1);

  // Timer callback to publish evaluator diagnostics
  using namespace std::literals::chrono_literals;
  timer_ =
    rclcpp::create_timer(this, get_clock(), 100ms, std::bind(&ControlEvaluatorNode::onTimer, this));
}

void ControlEvaluatorNode::getRouteData()
{
  // route
  {
    const auto msg = route_subscriber_.takeData();
    if (msg) {
      if (msg->segments.empty()) {
        RCLCPP_ERROR(get_logger(), "input route is empty. ignored");
      } else {
        route_handler_.setRoute(*msg);
      }
    }
  }

  // map
  {
    const auto msg = vector_map_subscriber_.takeData();
    if (msg) {
      route_handler_.setMap(*msg);
    }
  }
}

void ControlEvaluatorNode::AddLaneletMetricMsg(const Pose & ego_pose)
{
  const auto current_lanelets = [&]() {
    lanelet::ConstLanelet closest_route_lanelet;
    route_handler_.getClosestLaneletWithinRoute(ego_pose, &closest_route_lanelet);
    const auto shoulder_lanelets = route_handler_.getShoulderLaneletsAtPose(ego_pose);
    lanelet::ConstLanelets closest_lanelets{closest_route_lanelet};
    closest_lanelets.insert(
      closest_lanelets.end(), shoulder_lanelets.begin(), shoulder_lanelets.end());
    return closest_lanelets;
  }();
  const auto arc_coordinates = lanelet::utils::getArcCoordinates(current_lanelets, ego_pose);
  lanelet::ConstLanelet current_lane;
  lanelet::utils::query::getClosestLanelet(current_lanelets, ego_pose, &current_lane);

  const std::string base_name = "ego_lane_info/";
  MetricMsg metric_msg;

  {
    metric_msg.name = base_name + "lane_id";
    metric_msg.value = std::to_string(current_lane.id());
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  {
    metric_msg.name = base_name + "s";
    metric_msg.value = std::to_string(arc_coordinates.length);
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  {
    metric_msg.name = base_name + "t";
    metric_msg.value = std::to_string(arc_coordinates.distance);
    metrics_msg_.metric_array.push_back(metric_msg);
  }
  return;
}

void ControlEvaluatorNode::AddKinematicStateMetricMsg(
  const Odometry & odom, const AccelWithCovarianceStamped & accel_stamped)
{
  const std::string base_name = "kinematic_state/";
  MetricMsg metric_msg;

  metric_msg.name = base_name + "vel";
  metric_msg.value = std::to_string(odom.twist.twist.linear.x);
  metrics_msg_.metric_array.push_back(metric_msg);

  metric_msg.name = base_name + "acc";
  const auto & acc = accel_stamped.accel.accel.linear.x;
  metric_msg.value = std::to_string(acc);
  metrics_msg_.metric_array.push_back(metric_msg);

  const auto jerk = [&]() {
    if (!prev_acc_stamped_.has_value()) {
      prev_acc_stamped_ = accel_stamped;
      return 0.0;
    }
    const auto t = static_cast<double>(accel_stamped.header.stamp.sec) +
                   static_cast<double>(accel_stamped.header.stamp.nanosec) * 1e-9;
    const auto prev_t = static_cast<double>(prev_acc_stamped_.value().header.stamp.sec) +
                        static_cast<double>(prev_acc_stamped_.value().header.stamp.nanosec) * 1e-9;
    const auto dt = t - prev_t;
    if (dt < std::numeric_limits<double>::epsilon()) return 0.0;

    const auto prev_acc = prev_acc_stamped_.value().accel.accel.linear.x;
    prev_acc_stamped_ = accel_stamped;
    return (acc - prev_acc) / dt;
  }();

  metric_msg.name = base_name + "jerk";
  metric_msg.value = std::to_string(jerk);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void ControlEvaluatorNode::AddLateralDeviationMetricMsg(
  const Trajectory & traj, const Point & ego_point)
{
  const double lateral_deviation = metrics::calcLateralDeviation(traj, ego_point);

  MetricMsg metric_msg;
  metric_msg.name = "lateral_deviation";
  metric_msg.value = std::to_string(lateral_deviation);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void ControlEvaluatorNode::AddYawDeviationMetricMsg(const Trajectory & traj, const Pose & ego_pose)
{
  const double yaw_deviation = metrics::calcYawDeviation(traj, ego_pose);

  MetricMsg metric_msg;
  metric_msg.name = "yaw_deviation";
  metric_msg.value = std::to_string(yaw_deviation);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void ControlEvaluatorNode::AddGoalLongitudinalDeviationMetricMsg(const Pose & ego_pose)
{
  const double longitudinal_deviation =
    metrics::calcLongitudinalDeviation(route_handler_.getGoalPose(), ego_pose.position);

  MetricMsg metric_msg;
  metric_msg.name = "goal_longitudinal_deviation";
  metric_msg.value = std::to_string(longitudinal_deviation);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void ControlEvaluatorNode::AddGoalLateralDeviationMetricMsg(const Pose & ego_pose)
{
  const double lateral_deviation =
    metrics::calcLateralDeviation(route_handler_.getGoalPose(), ego_pose.position);

  MetricMsg metric_msg;
  metric_msg.name = "goal_lateral_deviation";
  metric_msg.value = std::to_string(lateral_deviation);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void ControlEvaluatorNode::AddGoalYawDeviationMetricMsg(const Pose & ego_pose)
{
  const double yaw_deviation = metrics::calcYawDeviation(route_handler_.getGoalPose(), ego_pose);

  MetricMsg metric_msg;
  metric_msg.name = "goal_yaw_deviation";
  metric_msg.value = std::to_string(yaw_deviation);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void ControlEvaluatorNode::onTimer()
{
  autoware::universe_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  const auto traj = traj_sub_.takeData();
  const auto odom = odometry_sub_.takeData();
  const auto acc = accel_sub_.takeData();

  // calculate deviation metrics
  if (odom && traj && !traj->points.empty()) {
    const Pose ego_pose = odom->pose.pose;
    AddLateralDeviationMetricMsg(*traj, ego_pose.position);
    AddYawDeviationMetricMsg(*traj, ego_pose);
  }

  getRouteData();
  if (odom && route_handler_.isHandlerReady()) {
    const Pose ego_pose = odom->pose.pose;
    AddLaneletMetricMsg(ego_pose);

    AddGoalLongitudinalDeviationMetricMsg(ego_pose);
    AddGoalLateralDeviationMetricMsg(ego_pose);
    AddGoalYawDeviationMetricMsg(ego_pose);
  }

  if (odom && acc) {
    AddKinematicStateMetricMsg(*odom, *acc);
  }

  // Publish metrics
  metrics_msg_.stamp = now();
  metrics_pub_->publish(metrics_msg_);
  metrics_msg_ = MetricArrayMsg{};

  // Publish processing time
  tier4_debug_msgs::msg::Float64Stamped processing_time_msg;
  processing_time_msg.stamp = get_clock()->now();
  processing_time_msg.data = stop_watch.toc();
  processing_time_pub_->publish(processing_time_msg);
}
}  // namespace control_diagnostics

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(control_diagnostics::ControlEvaluatorNode)

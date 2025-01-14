// Copyright 2021 Tier IV, Inc.
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

#include "autoware/planning_evaluator/motion_evaluator_node.hpp"

#include <nlohmann/json.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace planning_diagnostics
{
MotionEvaluatorNode::MotionEvaluatorNode(const rclcpp::NodeOptions & node_options)
: Node("motion_evaluator", node_options)
{
  tf_buffer_ptr_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
  tf_listener_ptr_ = std::make_unique<tf2_ros::TransformListener>(*tf_buffer_ptr_);

  twist_sub_ = create_subscription<nav_msgs::msg::Odometry>(
    "~/input/twist", rclcpp::QoS{1},
    std::bind(&MotionEvaluatorNode::onOdom, this, std::placeholders::_1));

  output_metrics_ = declare_parameter<bool>("output_metrics");

  // List of metrics to calculate
  for (const std::string & selected_metric :
       declare_parameter<std::vector<std::string>>("selected_metrics")) {
    Metric metric = str_to_metric.at(selected_metric);
    metrics_.push_back(metric);
  }
}

MotionEvaluatorNode::~MotionEvaluatorNode()
{
  if (!output_metrics_) {
    return;
  }
  try {
    // generate json data
    using json = nlohmann::json;
    json j;
    for (Metric metric : metrics_) {
      const std::string base_name = metric_to_str.at(metric) + "/";
      const auto & stat = metrics_calculator_.calculate(metric, accumulated_trajectory_);
      if (stat) {
        j[base_name + "min"] = stat->min();
        j[base_name + "max"] = stat->max();
        j[base_name + "mean"] = stat->mean();
      }
    }

    // get output folder
    const std::string output_folder_str =
      rclcpp::get_logging_directory().string() + "/autoware_metrics";
    if (!std::filesystem::exists(output_folder_str)) {
      if (!std::filesystem::create_directories(output_folder_str)) {
        RCLCPP_ERROR(
          this->get_logger(), "Failed to create directories: %s", output_folder_str.c_str());
        return;
      }
    }

    // get time stamp
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm * local_time = std::localtime(&now_time_t);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d-%H-%M-%S");
    std::string cur_time_str = oss.str();

    // Write metrics .json to file
    const std::string output_file_str =
      output_folder_str + "/autoware_motion_evaluator-" + cur_time_str + ".json";
    std::ofstream f(output_file_str);
    if (f.is_open()) {
      f << j.dump(4);
      f.close();
    } else {
      RCLCPP_ERROR(this->get_logger(), "Failed to open file: %s", output_file_str.c_str());
    }
  } catch (const std::exception & e) {
    std::cerr << "Exception in MotionEvaluatorNode destructor: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception in MotionEvaluatorNode destructor" << std::endl;
  }
}

void MotionEvaluatorNode::onOdom(const nav_msgs::msg::Odometry::SharedPtr msg)
{
  // TODO(Maxime CLEMENT): set some desired minimum time/distance between two points
  TrajectoryPoint current_point;
  current_point.pose = getCurrentEgoPose();
  current_point.longitudinal_velocity_mps = msg->twist.twist.linear.x;
  const rclcpp::Time now = this->get_clock()->now();
  if (!accumulated_trajectory_.points.empty()) {
    current_point.acceleration_mps2 =
      (msg->twist.twist.linear.x -
       accumulated_trajectory_.points.back().longitudinal_velocity_mps) /
      (now - stamps_.back()).seconds();
  }
  accumulated_trajectory_.points.push_back(current_point);
  stamps_.push_back(now);
}

geometry_msgs::msg::Pose MotionEvaluatorNode::getCurrentEgoPose() const
{
  geometry_msgs::msg::TransformStamped tf_current_pose;

  geometry_msgs::msg::Pose p;
  try {
    tf_current_pose = tf_buffer_ptr_->lookupTransform(
      "map", "base_link", rclcpp::Time(0), rclcpp::Duration::from_seconds(1.0));
  } catch (tf2::TransformException & ex) {
    RCLCPP_ERROR(get_logger(), "%s", ex.what());
    return p;
  }

  p.orientation = tf_current_pose.transform.rotation;
  p.position.x = tf_current_pose.transform.translation.x;
  p.position.y = tf_current_pose.transform.translation.y;
  p.position.z = tf_current_pose.transform.translation.z;
  return p;
}

}  // namespace planning_diagnostics

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(planning_diagnostics::MotionEvaluatorNode)

// Copyright 2023 Tier IV, Inc.
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

#include "scenario_simulator_v2_adapter/converter_node.hpp"

#include <rclcpp/rclcpp.hpp>

#include "tier4_simulation_msgs/msg/user_defined_value.hpp"
#include "tier4_simulation_msgs/msg/user_defined_value_type.hpp"
#include <tier4_metric_msgs/msg/metric.hpp>
#include <tier4_metric_msgs/msg/metric_array.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using ConverterNode = scenario_simulator_v2_adapter::MetricConverter;
using tier4_metric_msgs::msg::Metric;
using tier4_metric_msgs::msg::MetricArray;
using tier4_simulation_msgs::msg::UserDefinedValue;

void waitForMsg(
  bool & flag, const rclcpp::Node::SharedPtr node1, const rclcpp::Node::SharedPtr node2)
{
  while (!flag) {
    rclcpp::spin_some(node1);
    rclcpp::spin_some(node2);
    rclcpp::sleep_for(std::chrono::milliseconds(100));
  }
  flag = false;
}

std::function<void(UserDefinedValue::ConstSharedPtr)> generateCallback(
  bool & flag, UserDefinedValue & msg)
{
  return [&](UserDefinedValue::ConstSharedPtr received_msg) {
    flag = true;
    msg = *received_msg;
  };
}

TEST(ConverterNode, ConvertMetrics)
{
  const std::vector<std::string> input_topics = {"/test1/metrics", "/test2/metrics"};

  rclcpp::init(0, nullptr);
  rclcpp::Node::SharedPtr dummy_node = std::make_shared<rclcpp::Node>("converter_test_node");

  rclcpp::NodeOptions options;
  options.append_parameter_override("metric_topic_list", rclcpp::ParameterValue(input_topics));
  auto node = std::make_shared<ConverterNode>(options);

  {  // Simple case with 1 resulting UserDefinedValue
    bool msg_received = false;
    UserDefinedValue param;
    // MetricArray publishers
    const auto metric_pub = dummy_node->create_publisher<MetricArray>(input_topics[0], 1);
    // UserDefinedValue subscribers
    const auto param_sub_a = dummy_node->create_subscription<UserDefinedValue>(
      input_topics[0] + "/a", 1, generateCallback(msg_received, param));
    MetricArray metrics;
    Metric metric;
    metric.name = "a";
    metric.value = "1";
    metrics.metric_array.push_back(metric);
    metric_pub->publish(metrics);
    waitForMsg(msg_received, node, dummy_node);
    EXPECT_EQ(param.value, "1");
  }
  {  // Case with multiple UserDefinedValue converted from one MetricArray
    bool msg_received_x = false;
    bool msg_received_y = false;
    UserDefinedValue param_x;
    UserDefinedValue param_y;
    // MetricArray publishers
    const auto metric_pub = dummy_node->create_publisher<MetricArray>(input_topics[1], 1);
    // UserDefinedValue subscribers
    const auto param_sub_x = dummy_node->create_subscription<UserDefinedValue>(
      input_topics[1] + "/x", 1, generateCallback(msg_received_x, param_x));
    const auto param_sub_y = dummy_node->create_subscription<UserDefinedValue>(
      input_topics[1] + "/y", 1, generateCallback(msg_received_y, param_y));
    MetricArray metrics;
    Metric metric;
    metric.name = "x";
    metric.value = "2";
    metrics.metric_array.push_back(metric);
    metric.name = "y";
    metric.value = "3.33333";
    metrics.metric_array.push_back(metric);
    metric_pub->publish(metrics);
    waitForMsg(msg_received_x, node, dummy_node);
    EXPECT_EQ(param_x.value, "2");
    waitForMsg(msg_received_y, node, dummy_node);
    EXPECT_EQ(param_y.value, "3.33333");
  }

  rclcpp::shutdown();
}

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

#ifndef SCENARIO_SIMULATOR_V2_ADAPTER__CONVERTER_NODE_HPP_
#define SCENARIO_SIMULATOR_V2_ADAPTER__CONVERTER_NODE_HPP_

#include <rclcpp/rclcpp.hpp>

#include "tier4_simulation_msgs/msg/user_defined_value.hpp"
#include "tier4_simulation_msgs/msg/user_defined_value_type.hpp"
#include <tier4_metric_msgs/msg/metric.hpp>
#include <tier4_metric_msgs/msg/metric_array.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace scenario_simulator_v2_adapter
{
using tier4_metric_msgs::msg::Metric;
using tier4_metric_msgs::msg::MetricArray;
using tier4_simulation_msgs::msg::UserDefinedValue;
using tier4_simulation_msgs::msg::UserDefinedValueType;

/**
 * @brief Node for converting Autoware's messages to UserDefinedValue
 */
class MetricConverter : public rclcpp::Node
{
public:
  explicit MetricConverter(const rclcpp::NodeOptions & node_options);

  /**
   * @brief callback for MetricArray msgs that publishes equivalent UserDefinedValue msgs
   * @param [in] metrics_msg received metrics message
   */
  void onMetrics(
    const MetricArray::ConstSharedPtr metrics_msg, const size_t topic_idx,
    const std::string & topic);

  UserDefinedValue createUserDefinedValue(const Metric & metric) const;

  rclcpp::Publisher<UserDefinedValue>::SharedPtr getPublisher(
    const std::string & topic, const size_t topic_idx);

private:
  // ROS
  std::vector<rclcpp::Subscription<MetricArray>::SharedPtr> metrics_sub_;
  std::vector<std::unordered_map<std::string, rclcpp::Publisher<UserDefinedValue>::SharedPtr>>
    params_pub_;
};
}  // namespace scenario_simulator_v2_adapter

#endif  // SCENARIO_SIMULATOR_V2_ADAPTER__CONVERTER_NODE_HPP_

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

#ifndef PROCESSING_TIME_CHECKER_HPP_
#define PROCESSING_TIME_CHECKER_HPP_

#include "autoware/universe_utils/math/accumulator.hpp"

#include <rclcpp/rclcpp.hpp>

#include <tier4_debug_msgs/msg/float64_stamped.hpp>
#include <tier4_metric_msgs/msg/metric.hpp>
#include <tier4_metric_msgs/msg/metric_array.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace autoware::processing_time_checker
{
using autoware::universe_utils::Accumulator;
using MetricMsg = tier4_metric_msgs::msg::Metric;
using MetricArrayMsg = tier4_metric_msgs::msg::MetricArray;
using tier4_debug_msgs::msg::Float64Stamped;

class ProcessingTimeChecker : public rclcpp::Node
{
public:
  explicit ProcessingTimeChecker(const rclcpp::NodeOptions & node_options);
  ~ProcessingTimeChecker() override;

private:
  void on_timer();

  rclcpp::TimerBase::SharedPtr timer_;

  rclcpp::Publisher<MetricArrayMsg>::SharedPtr metrics_pub_;
  std::vector<rclcpp::Subscription<Float64Stamped>::SharedPtr> processing_time_subscribers_;

  // parameters
  bool output_metrics_;

  // topic name - module name
  std::unordered_map<std::string, std::string> module_name_map_{};
  // module name - processing time
  std::unordered_map<std::string, double> processing_time_map_{};
  // module name - accumulator
  std::unordered_map<std::string, Accumulator<double>> processing_time_accumulator_map_{};
};
}  // namespace autoware::processing_time_checker

#endif  // PROCESSING_TIME_CHECKER_HPP_

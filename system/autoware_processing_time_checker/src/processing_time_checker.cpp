// Copyright 2023 TIER IV, Inc.
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

#include "processing_time_checker.hpp"

#include <rclcpp/rclcpp.hpp>

#include <chrono>
#include <string>
#include <vector>

namespace autoware::processing_time_checker
{

ProcessingTimeChecker::ProcessingTimeChecker(const rclcpp::NodeOptions & node_options)
: Node("processing_time_checker", node_options)
{
  const double update_rate = declare_parameter<double>("update_rate");
  const auto processing_time_topic_name_list =
    declare_parameter<std::vector<std::string>>("processing_time_topic_name_list");

  // extract module name from topic name
  for (const auto & processing_time_topic_name : processing_time_topic_name_list) {
    auto module_name = processing_time_topic_name;
    module_name = module_name.substr(0, module_name.find_last_of("/"));
    module_name = module_name.substr(module_name.find_last_of("/") + 1);
    std::cerr << module_name << std::endl;
    module_name_map_.insert_or_assign(processing_time_topic_name, module_name);
  }

  // create subscribers
  for (const auto & processing_time_topic_name : processing_time_topic_name_list) {
    const auto & module_name = module_name_map_.at(processing_time_topic_name);

    processing_time_subscribers_.push_back(create_subscription<Float64Stamped>(
      processing_time_topic_name, 1,
      [this, &module_name]([[maybe_unused]] const Float64Stamped & msg) {
        processing_time_map_.insert_or_assign(module_name, msg.data);
      }));
  }

  diag_pub_ = create_publisher<DiagnosticArray>("~/metrics", 1);

  const auto period_ns = rclcpp::Rate(update_rate).period();
  timer_ = rclcpp::create_timer(
    this, get_clock(), period_ns, std::bind(&ProcessingTimeChecker::onTimer, this));
}

void ProcessingTimeChecker::onTimer()
{
  // create diagnostic status
  DiagnosticStatus status;
  status.level = status.OK;
  status.name = "processing_time";
  for (const auto & processing_time_iterator : processing_time_map_) {
    const auto processing_time_topic_name = processing_time_iterator.first;
    const double processing_time = processing_time_iterator.second;

    // generate diagnostic status
    diagnostic_msgs::msg::KeyValue key_value;
    key_value.key = processing_time_topic_name;
    key_value.value = std::to_string(processing_time);
    status.values.push_back(key_value);
  }

  // create diagnostic array
  DiagnosticArray diag_msg;
  diag_msg.header.stamp = now();
  diag_msg.status.push_back(status);

  // publish
  diag_pub_->publish(diag_msg);
}
}  // namespace autoware::processing_time_checker

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::processing_time_checker::ProcessingTimeChecker)

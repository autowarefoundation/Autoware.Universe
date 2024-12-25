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

#include "processing_time_checker.hpp"

#include <nlohmann/json.hpp>
#include <rclcpp/rclcpp.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace autoware::processing_time_checker
{

namespace
{
std::string remove_last_name(const std::string & str)
{
  return str.substr(0, str.find_last_of("/"));
}

std::string get_last_name(const std::string & str)
{
  return str.substr(str.find_last_of("/") + 1);
}
}  // namespace

ProcessingTimeChecker::ProcessingTimeChecker(const rclcpp::NodeOptions & node_options)
: Node("processing_time_checker", node_options)
{
  output_metrics_ = declare_parameter<bool>("output_metrics");
  const double update_rate = declare_parameter<double>("update_rate");
  const auto processing_time_topic_name_list =
    declare_parameter<std::vector<std::string>>("processing_time_topic_name_list");

  for (const auto & processing_time_topic_name : processing_time_topic_name_list) {
    std::optional<std::string> module_name{std::nullopt};

    // extract module name from topic name
    auto tmp_topic_name = processing_time_topic_name;
    for (size_t i = 0; i < 4; ++i) {  // 4 is enough for the search depth
      tmp_topic_name = remove_last_name(tmp_topic_name);
      const auto module_name_candidate = get_last_name(tmp_topic_name);
      // clang-format off
      if (
        module_name_candidate != "processing_time_ms" && module_name_candidate != "debug" &&
        module_name_candidate != "total_time")
      {
        module_name = module_name_candidate;
        break;
      }
      // clang-format on
    }

    // register module name
    if (module_name) {
      module_name_map_.insert_or_assign(processing_time_topic_name, *module_name);
      processing_time_accumulator_map_.insert_or_assign(*module_name, Accumulator<double>());
    } else {
      throw std::invalid_argument("The format of the processing time topic name is not correct.");
    }
  }

  // create subscribers
  for (const auto & processing_time_topic_name : processing_time_topic_name_list) {
    const auto & module_name = module_name_map_.at(processing_time_topic_name);

    // clang-format off
    processing_time_subscribers_.push_back(
      create_subscription<Float64Stamped>(
        processing_time_topic_name, 1,
        [this, &module_name]([[maybe_unused]] const Float64Stamped & msg) {
          processing_time_map_.insert_or_assign(module_name, msg.data);
          processing_time_accumulator_map_.at(module_name).add(msg.data);
        }));
    // clang-format on
  }

  metrics_pub_ = create_publisher<MetricArrayMsg>("~/metrics", 1);

  const auto period_ns = rclcpp::Rate(update_rate).period();
  timer_ = rclcpp::create_timer(
    this, get_clock(), period_ns, std::bind(&ProcessingTimeChecker::on_timer, this));
}

ProcessingTimeChecker::~ProcessingTimeChecker()
{
  if (!output_metrics_) {
    return;
  }

  try {
    // generate json data
    nlohmann::json j;
    for (const auto & accumulator_iterator : processing_time_accumulator_map_) {
      const auto module_name = accumulator_iterator.first;
      const auto processing_time_accumulator = accumulator_iterator.second;
      j[module_name + "/min"] = processing_time_accumulator.min();
      j[module_name + "/max"] = processing_time_accumulator.max();
      j[module_name + "/mean"] = processing_time_accumulator.mean();
      j[module_name + "/count"] = processing_time_accumulator.count();
      j[module_name + "/description"] = "processing time of " + module_name + "[ms]";
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
      output_folder_str + "/autoware_processing_time_checker-" + cur_time_str + ".json";
    std::ofstream f(output_file_str);
    if (f.is_open()) {
      f << j.dump(4);
      f.close();
    } else {
      RCLCPP_ERROR(this->get_logger(), "Failed to open file: %s", output_file_str.c_str());
    }
  } catch (const std::exception & e) {
    std::cerr << "Exception in ProcessingTimeChecker: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception in ProcessingTimeChecker" << std::endl;
  }
}

void ProcessingTimeChecker::on_timer()
{
  // create MetricArrayMsg
  MetricArrayMsg metrics_msg;
  for (const auto & processing_time_iterator : processing_time_map_) {
    const auto processing_time_topic_name = processing_time_iterator.first;
    const double processing_time = processing_time_iterator.second;

    // generate MetricMsg
    MetricMsg metric;
    metric.name = "processing_time/" + processing_time_topic_name;
    metric.value = std::to_string(processing_time);
    metric.unit = "millisecond";
    metrics_msg.metric_array.push_back(metric);
  }

  // publish
  metrics_msg.stamp = now();
  metrics_pub_->publish(metrics_msg);
}
}  // namespace autoware::processing_time_checker

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::processing_time_checker::ProcessingTimeChecker)

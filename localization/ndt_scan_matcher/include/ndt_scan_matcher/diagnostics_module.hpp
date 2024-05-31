// Copyright 2023 Autoware Foundation
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

#ifndef NDT_SCAN_MATCHER__DIAGNOSTICS_MODULE_HPP_
#define NDT_SCAN_MATCHER__DIAGNOSTICS_MODULE_HPP_

#include <rclcpp/rclcpp.hpp>

#include <diagnostic_msgs/msg/diagnostic_array.hpp>

#include <string>
#include <vector>

class DiagnosticsModule
{
public:
  DiagnosticsModule(rclcpp::Node * node, const std::string & diagnostic_name);
  void clear();
  void addKeyValue(const diagnostic_msgs::msg::KeyValue & key_value_msg);
  template <typename T>
  void addKeyValue(const std::string & key, const T & value);
  void updateLevelAndMessage(const int8_t level, const std::string & message);
  void publish(const rclcpp::Time & publish_time_stamp);

private:
  diagnostic_msgs::msg::DiagnosticArray createDiagnosticsArray(const rclcpp::Time & publish_time_stamp) const;

  rclcpp::Clock::SharedPtr clock_;
  rclcpp::Publisher<diagnostic_msgs::msg::DiagnosticArray>::SharedPtr diagnostics_pub_;

  diagnostic_msgs::msg::DiagnosticStatus diagnostics_status_msg_;
};

template <typename T>
void DiagnosticsModule::addKeyValue(const std::string & key, const T & value)
{
  diagnostic_msgs::msg::KeyValue key_value;
  key_value.key = key;
  key_value.value = std::to_string(value);
  addKeyValue(key_value);
}

template <>
void DiagnosticsModule::addKeyValue(const std::string & key, const std::string & value);
template <>
void DiagnosticsModule::addKeyValue(const std::string & key, const bool & value);

#endif  // NDT_SCAN_MATCHER__DIAGNOSTICS_MODULE_HPP_

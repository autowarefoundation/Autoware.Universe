// Copyright 2020 Tier IV, Inc.
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

#ifndef DUMMY_DIAG_PUBLISHER__DUMMY_DIAG_PUBLISHER_CORE_HPP_
#define DUMMY_DIAG_PUBLISHER__DUMMY_DIAG_PUBLISHER_CORE_HPP_

#include <rclcpp/rclcpp.hpp>

#include <diagnostic_msgs/msg/diagnostic_array.hpp>

#include <optional>
#include <string>
#include <vector>

struct DiagConfig
{
  std::string hardware_id;
  std::string msg_ok;
  std::string msg_warn;
  std::string msg_error;
  std::string msg_stale;
};

class DummyDiagPublisher : public rclcpp::Node
{
public:
  explicit DummyDiagPublisher(const rclcpp::NodeOptions & options);

private:
  enum Status {
    OK,
    WARN,
    ERROR,
    STALE,
  };

  struct DummyDiagConfig
  {
    std::string name;
    bool is_active;
    Status status;
  };

  using RequiredDiags = std::vector<DummyDiagConfig>;

  // Parameter
  double update_rate_;
  DiagConfig diag_config_;
  DummyDiagConfig config_;

  RequiredDiags required_diags_;
  void loadRequiredDiags();

  std::optional<Status> convertStrToStatus(std::string & status_str);
  std::string convertStatusToStr(const Status & status);
  diagnostic_msgs::msg::DiagnosticStatus::_level_type convertStatusToLevel(const Status & status);

  // Timer
  void onTimer();
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<diagnostic_msgs::msg::DiagnosticArray>::SharedPtr pub_;
};

#endif  // DUMMY_DIAG_PUBLISHER__DUMMY_DIAG_PUBLISHER_CORE_HPP_

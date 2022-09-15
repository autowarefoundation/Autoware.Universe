// Copyright 2022 The Autoware Contributors
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

#ifndef POSE_INITIALIZER__LOCALIZATION_TRIGGER_MODULEHPP_
#define POSE_INITIALIZER__LOCALIZATION_TRIGGER_MODULEHPP_

#include <rclcpp/rclcpp.hpp>

#include <tier4_localization_msgs/srv/trigger_node.hpp>

class LocalizationTriggerModule
{
private:
  using RequestTriggerNode = tier4_localization_msgs::srv::TriggerNode;

public:
  explicit LocalizationTriggerModule(rclcpp::Node * node);
  void deactivate() const;
  void activate() const;

private:
  rclcpp::Logger logger_;
  rclcpp::Client<RequestTriggerNode>::SharedPtr client_ekf_trigger_;
  rclcpp::Client<RequestTriggerNode>::SharedPtr client_ndt_trigger_;
};

#endif  // POSE_INITIALIZER__LOCALIZATION_TRIGGER_MODULEHPP_

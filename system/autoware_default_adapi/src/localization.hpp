// Copyright 2022 TIER IV, Inc.
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

#ifndef LOCALIZATION_HPP_
#define LOCALIZATION_HPP_

#include <autoware/adapi_specs/localization.hpp>
#include <autoware/component_interface_specs/localization.hpp>
#include <rclcpp/rclcpp.hpp>

// This file should be included after messages.
#include "utils/types.hpp"

namespace autoware::default_adapi
{

class LocalizationNode : public rclcpp::Node
{
public:
  explicit LocalizationNode(const rclcpp::NodeOptions & options);

private:
  rclcpp::CallbackGroup::SharedPtr group_cli_;
  Srv<autoware::adapi_specs::localization::Initialize> srv_initialize_;
  Pub<autoware::adapi_specs::localization::InitializationState> pub_state_;
  Cli<autoware::component_interface_specs::localization::Initialize> cli_initialize_;
  Sub<autoware::component_interface_specs::localization::InitializationState> sub_state_;

  void on_initialize(
    const autoware::adapi_specs::localization::Initialize::Service::Request::SharedPtr req,
    const autoware::adapi_specs::localization::Initialize::Service::Response::SharedPtr res);
};

}  // namespace autoware::default_adapi

#endif  // LOCALIZATION_HPP_

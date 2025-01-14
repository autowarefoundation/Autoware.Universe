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

#ifndef VEHICLE_DOOR_HPP_
#define VEHICLE_DOOR_HPP_

#include <autoware/adapi_specs/vehicle.hpp>
#include <autoware/component_interface_specs/system.hpp>
#include <autoware/component_interface_specs/vehicle.hpp>
#include <autoware/component_interface_utils/status.hpp>
#include <rclcpp/rclcpp.hpp>

#include <optional>

// This file should be included after messages.
#include "utils/types.hpp"

namespace autoware::default_adapi
{

class VehicleDoorNode : public rclcpp::Node
{
public:
  explicit VehicleDoorNode(const rclcpp::NodeOptions & options);

private:
  using OperationModeState = autoware::component_interface_specs::system::OperationModeState;
  using InternalDoorStatus = autoware::component_interface_specs::vehicle::DoorStatus;
  using InternalDoorLayout = autoware::component_interface_specs::vehicle::DoorLayout;
  using InternalDoorCommand = autoware::component_interface_specs::vehicle::DoorCommand;
  using ExternalDoorStatus = autoware::adapi_specs::vehicle::DoorStatus;
  using ExternalDoorLayout = autoware::adapi_specs::vehicle::DoorLayout;
  using ExternalDoorCommand = autoware::adapi_specs::vehicle::DoorCommand;

  void on_operation_mode(const OperationModeState::Message::ConstSharedPtr msg);
  void on_status(InternalDoorStatus::Message::ConstSharedPtr msg);
  void on_command(
    const ExternalDoorCommand::Service::Request::SharedPtr req,
    const ExternalDoorCommand::Service::Response::SharedPtr res);

  rclcpp::CallbackGroup::SharedPtr group_cli_;
  Srv<ExternalDoorCommand> srv_command_;
  Srv<ExternalDoorLayout> srv_layout_;
  Pub<ExternalDoorStatus> pub_status_;
  Cli<InternalDoorCommand> cli_command_;
  Cli<InternalDoorLayout> cli_layout_;
  Sub<InternalDoorStatus> sub_status_;
  std::optional<InternalDoorStatus::Message> status_;

  bool check_autoware_control_;
  bool is_autoware_control_;
  bool is_stop_mode_;
  Sub<OperationModeState> sub_operation_mode_;
};

}  // namespace autoware::default_adapi

#endif  // VEHICLE_DOOR_HPP_

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

#ifndef VEHICLE_HPP_
#define VEHICLE_HPP_

#include <autoware/adapi_specs/vehicle.hpp>
#include <autoware/component_interface_specs/localization.hpp>
#include <autoware/component_interface_specs/map.hpp>
#include <autoware/component_interface_specs/vehicle.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_adapi_v1_msgs/msg/gear.hpp>
#include <autoware_adapi_v1_msgs/msg/hazard_lights.hpp>
#include <autoware_adapi_v1_msgs/msg/turn_indicators.hpp>

#include <unordered_map>

// This file should be included after messages.
#include "utils/types.hpp"

namespace autoware::default_adapi
{

class VehicleNode : public rclcpp::Node
{
public:
  explicit VehicleNode(const rclcpp::NodeOptions & options);

private:
  rclcpp::CallbackGroup::SharedPtr group_cli_;
  Pub<autoware::adapi_specs::vehicle::VehicleKinematics> pub_kinematics_;
  Pub<autoware::adapi_specs::vehicle::VehicleStatus> pub_status_;
  Sub<autoware::component_interface_specs::localization::KinematicState> sub_kinematic_state_;
  Sub<autoware::component_interface_specs::localization::Acceleration> sub_acceleration_;
  Sub<autoware::component_interface_specs::vehicle::SteeringStatus> sub_steering_;
  Sub<autoware::component_interface_specs::vehicle::GearStatus> sub_gear_state_;
  Sub<autoware::component_interface_specs::vehicle::TurnIndicatorStatus> sub_turn_indicator_;
  Sub<autoware::component_interface_specs::vehicle::HazardLightStatus> sub_hazard_light_;
  Sub<autoware::component_interface_specs::vehicle::EnergyStatus> sub_energy_level_;
  Sub<autoware::component_interface_specs::map::MapProjectorInfo> sub_map_projector_info_;
  rclcpp::TimerBase::SharedPtr timer_;

  autoware::component_interface_specs::localization::KinematicState::Message::ConstSharedPtr
    kinematic_state_msgs_;
  autoware::component_interface_specs::localization::Acceleration::Message::ConstSharedPtr
    acceleration_msgs_;
  autoware::component_interface_specs::vehicle::SteeringStatus::Message::ConstSharedPtr
    steering_status_msgs_;
  autoware::component_interface_specs::vehicle::GearStatus::Message::ConstSharedPtr
    gear_status_msgs_;
  autoware::component_interface_specs::vehicle::TurnIndicatorStatus::Message::ConstSharedPtr
    turn_indicator_status_msgs_;
  autoware::component_interface_specs::vehicle::HazardLightStatus::Message::ConstSharedPtr
    hazard_light_status_msgs_;
  autoware::component_interface_specs::vehicle::EnergyStatus::Message::ConstSharedPtr
    energy_status_msgs_;
  autoware::component_interface_specs::map::MapProjectorInfo::Message::ConstSharedPtr
    map_projector_info_;

  void kinematic_state(
    const autoware::component_interface_specs::localization::KinematicState::Message::ConstSharedPtr
      msg_ptr);
  void acceleration_status(
    const autoware::component_interface_specs::localization::Acceleration::Message::ConstSharedPtr
      msg_ptr);
  void steering_status(
    const autoware::component_interface_specs::vehicle::SteeringStatus::Message::ConstSharedPtr
      msg_ptr);
  void gear_status(
    const autoware::component_interface_specs::vehicle::GearStatus::Message::ConstSharedPtr
      msg_ptr);
  void turn_indicator_status(
    const autoware::component_interface_specs::vehicle::TurnIndicatorStatus::Message::ConstSharedPtr
      msg_ptr);
  void map_projector_info(
    const autoware::component_interface_specs::map::MapProjectorInfo::Message::ConstSharedPtr
      msg_ptr);
  void hazard_light_status(
    const autoware::component_interface_specs::vehicle::HazardLightStatus::Message::ConstSharedPtr
      msg_ptr);
  void energy_status(
    const autoware::component_interface_specs::vehicle::EnergyStatus::Message::ConstSharedPtr
      msg_ptr);
  uint8_t mapping(
    std::unordered_map<uint8_t, uint8_t> hash_map, uint8_t input, uint8_t default_value);
  void publish_kinematics();
  void publish_status();
  void on_timer();
};

}  // namespace autoware::default_adapi

#endif  // VEHICLE_HPP_

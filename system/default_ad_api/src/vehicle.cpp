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

#include "vehicle.hpp"

namespace default_ad_api
{

VehicleNode::VehicleNode(const rclcpp::NodeOptions & options) : Node("vehicle", options)
{
  const auto adaptor = component_interface_utils::NodeAdaptor(this);
  group_cli_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  adaptor.init_pub(pub_kinematic_);
  adaptor.init_pub(pub_state_);
  adaptor.init_pub(pub_door_);
  adaptor.init_sub(sub_kinematic_state_, this, &VehicleNode::kinematic_state);
  adaptor.init_sub(sub_acceleration_, this, &VehicleNode::acceleration_status);
  adaptor.init_sub(sub_steering_, this, &VehicleNode::steering_status);
  adaptor.init_sub(sub_gear_state_, this, &VehicleNode::gear_status);
  adaptor.init_sub(sub_turn_indicator_, this, &VehicleNode::turn_indicator_status);
  adaptor.init_sub(sub_mgrs_grid_, this, &VehicleNode::mgrs_grid_data);
  adaptor.init_sub(sub_hazard_light_, this, &VehicleNode::hazard_light_status);
  adaptor.init_sub(sub_energy_level_, this, &VehicleNode::energy_status);
  adaptor.init_sub(sub_door_status_, this, &VehicleNode::door_status);

  const auto rate = rclcpp::Rate(10);
  timer_ = rclcpp::create_timer(this, get_clock(), rate.period(), [this]() { on_timer(); });
}

uint8_t VehicleNode::mapping(
  std::unordered_map<uint8_t, uint8_t> hash_map, uint8_t input, uint8_t default_value)
{
  if (hash_map.find(input) == hash_map.end()) {
    return default_value;
  } else {
    return hash_map[input];
  }
}

void VehicleNode::kinematic_state(
  const vehicle_interface::KinematicState::Message::ConstSharedPtr msg_ptr)
{
  vehicle_kinematic_.pose.header = msg_ptr->header;
  vehicle_kinematic_.pose.pose = msg_ptr->pose;
  vehicle_kinematic_.twist.header = msg_ptr->header;
  vehicle_kinematic_.twist.header.frame_id = msg_ptr->child_frame_id;
  vehicle_kinematic_.twist.twist = msg_ptr->twist;
  if (!mgrs_grid_.empty()) {
    lanelet::GPSPoint projected_gps_point = lanelet::projection::MGRSProjector::reverse(
      toBasicPoint3dPt(msg_ptr->pose.pose.position), mgrs_grid_);
    vehicle_kinematic_.geographic_pose.header = msg_ptr->header;
    vehicle_kinematic_.geographic_pose.header.frame_id = "global";
    vehicle_kinematic_.geographic_pose.position.latitude = projected_gps_point.lat;
    vehicle_kinematic_.geographic_pose.position.longitude = projected_gps_point.lon;
    vehicle_kinematic_.geographic_pose.position.altitude = projected_gps_point.ele;
  }
}

Eigen::Vector3d VehicleNode::toBasicPoint3dPt(const geometry_msgs::msg::Point src)
{
  Eigen::Vector3d dst;
  dst.x() = src.x;
  dst.y() = src.y;
  dst.z() = src.z;
  return dst;
}

void VehicleNode::acceleration_status(
  const vehicle_interface::Acceleration::Message::ConstSharedPtr msg_ptr)
{
  vehicle_kinematic_.accel.header = msg_ptr->header;
  vehicle_kinematic_.accel.accel = msg_ptr->accel;
}

void VehicleNode::steering_status(
  const vehicle_interface::SteeringStatus::Message::ConstSharedPtr msg_ptr)
{
  vehicle_state_.steering_tire_angle = msg_ptr->steering_tire_angle;
}

void VehicleNode::gear_status(const GearReport::ConstSharedPtr msg_ptr)
{
  vehicle_state_.gear.status = mapping(gear_type_, msg_ptr->report, ApiGear::UNKNOWN);
}

void VehicleNode::turn_indicator_status(const TurnIndicatorsReport::ConstSharedPtr msg_ptr)
{
  vehicle_state_.turn_indicator.status =
    mapping(turn_indicator_type_, msg_ptr->report, ApiTurnIndicator::UNKNOWN);
}

void VehicleNode::hazard_light_status(const HazardLightsReport::ConstSharedPtr msg_ptr)
{
  vehicle_state_.hazard_light.status =
    mapping(hazard_light_type_, msg_ptr->report, ApiHazardLight::UNKNOWN);
}

void VehicleNode::mgrs_grid_data(const vehicle_interface::MGRSGrid::Message::ConstSharedPtr msg_ptr)
{
  mgrs_grid_ = msg_ptr->data;
}

void VehicleNode::energy_status(
  const vehicle_interface::EnergyStatus::Message::ConstSharedPtr msg_ptr)
{
  vehicle_state_.energy_level = msg_ptr->energy_level;
}

void VehicleNode::door_status(const VehicleDoorStatus::ConstSharedPtr msg_ptr)
{
  // Currently only support single door status
  ApiDoorStatus door_status;
  door_status.status = mapping(door_status_type_, msg_ptr->status, ApiDoorStatus::UNKNOWN);
  vehicle_door_.stamp = now();
  vehicle_door_.doors = {door_status};
}

void VehicleNode::on_timer()
{
  vehicle_state_.stamp = now();
  pub_kinematic_->publish(vehicle_kinematic_);
  pub_state_->publish(vehicle_state_);
  pub_door_->publish(vehicle_door_);
}

}  // namespace default_ad_api

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(default_ad_api::VehicleNode)

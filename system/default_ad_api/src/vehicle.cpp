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

#include "tier4_autoware_utils/ros/polling_subscriber.hpp"

#include <geography_utils/height.hpp>
#include <geography_utils/projection.hpp>

#include <geographic_msgs/msg/geo_point.hpp>

#include <limits>

namespace default_ad_api
{

using GearReport = vehicle_interface::GearStatus::Message;
using ApiGear = autoware_adapi_v1_msgs::msg::Gear;
using TurnIndicatorsReport = vehicle_interface::TurnIndicatorStatus::Message;
using ApiTurnIndicator = autoware_adapi_v1_msgs::msg::TurnIndicators;
using HazardLightsReport = vehicle_interface::HazardLightStatus::Message;
using ApiHazardLight = autoware_adapi_v1_msgs::msg::HazardLights;
using MapProjectorInfo = map_interface::MapProjectorInfo::Message;

std::unordered_map<uint8_t, uint8_t> gear_type_ = {
  {GearReport::NONE, ApiGear::UNKNOWN},    {GearReport::NEUTRAL, ApiGear::NEUTRAL},
  {GearReport::DRIVE, ApiGear::DRIVE},     {GearReport::DRIVE_2, ApiGear::DRIVE},
  {GearReport::DRIVE_3, ApiGear::DRIVE},   {GearReport::DRIVE_4, ApiGear::DRIVE},
  {GearReport::DRIVE_5, ApiGear::DRIVE},   {GearReport::DRIVE_6, ApiGear::DRIVE},
  {GearReport::DRIVE_7, ApiGear::DRIVE},   {GearReport::DRIVE_8, ApiGear::DRIVE},
  {GearReport::DRIVE_9, ApiGear::DRIVE},   {GearReport::DRIVE_10, ApiGear::DRIVE},
  {GearReport::DRIVE_11, ApiGear::DRIVE},  {GearReport::DRIVE_12, ApiGear::DRIVE},
  {GearReport::DRIVE_13, ApiGear::DRIVE},  {GearReport::DRIVE_14, ApiGear::DRIVE},
  {GearReport::DRIVE_15, ApiGear::DRIVE},  {GearReport::DRIVE_16, ApiGear::DRIVE},
  {GearReport::DRIVE_17, ApiGear::DRIVE},  {GearReport::DRIVE_18, ApiGear::DRIVE},
  {GearReport::REVERSE, ApiGear::REVERSE}, {GearReport::REVERSE_2, ApiGear::REVERSE},
  {GearReport::PARK, ApiGear::PARK},       {GearReport::LOW, ApiGear::LOW},
  {GearReport::LOW_2, ApiGear::LOW},
};

std::unordered_map<uint8_t, uint8_t> turn_indicator_type_ = {
  {TurnIndicatorsReport::DISABLE, ApiTurnIndicator::DISABLE},
  {TurnIndicatorsReport::ENABLE_LEFT, ApiTurnIndicator::LEFT},
  {TurnIndicatorsReport::ENABLE_RIGHT, ApiTurnIndicator::RIGHT},
};

std::unordered_map<uint8_t, uint8_t> hazard_light_type_ = {
  {HazardLightsReport::DISABLE, ApiHazardLight::DISABLE},
  {HazardLightsReport::ENABLE, ApiHazardLight::ENABLE},
};

VehicleNode::VehicleNode(const rclcpp::NodeOptions & options) : Node("vehicle", options)
{
  const auto adaptor = component_interface_utils::NodeAdaptor(this);

  adaptor.init_pub(pub_kinematics_);
  adaptor.init_pub(pub_status_);

  kinematic_state_sub_ = create_polling_subscriber<localization_interface::KinematicState>(this);
  acceleration_sub_ = create_polling_subscriber<localization_interface::Acceleration>(this);
  steering_sub_ = create_polling_subscriber<vehicle_interface::SteeringStatus>(this);
  gear_state_sub_ = create_polling_subscriber<vehicle_interface::GearStatus>(this);
  turn_indicator_info_sub_ =
    create_polling_subscriber<vehicle_interface::TurnIndicatorStatus>(this);
  map_projector_info_sub_ = create_polling_subscriber<map_interface::MapProjectorInfo>(this);
  hazard_light_sub_ = create_polling_subscriber<vehicle_interface::HazardLightStatus>(this);
  energy_level_sub_ = create_polling_subscriber<vehicle_interface::EnergyStatus>(this);

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

void VehicleNode::publish_kinematics()
{
  kinematic_state_msgs_ = kinematic_state_sub_->takeData();
  acceleration_msgs_ = acceleration_sub_->takeData();
  map_projector_info_ = map_projector_info_sub_->takeData();

  if (!kinematic_state_msgs_ || !acceleration_msgs_ || !map_projector_info_) return;

  autoware_ad_api::vehicle::VehicleKinematics::Message vehicle_kinematics;
  vehicle_kinematics.pose.header = kinematic_state_msgs_->header;
  vehicle_kinematics.pose.pose = kinematic_state_msgs_->pose;
  vehicle_kinematics.twist.header = kinematic_state_msgs_->header;
  vehicle_kinematics.twist.header.frame_id = kinematic_state_msgs_->child_frame_id;
  vehicle_kinematics.twist.twist = kinematic_state_msgs_->twist;
  if (map_projector_info_->projector_type != MapProjectorInfo::LOCAL) {
    const geographic_msgs::msg::GeoPoint projected_gps_point = geography_utils::project_reverse(
      kinematic_state_msgs_->pose.pose.position, *map_projector_info_);
    vehicle_kinematics.geographic_pose.header = kinematic_state_msgs_->header;
    vehicle_kinematics.geographic_pose.header.frame_id = "global";
    vehicle_kinematics.geographic_pose.position.latitude = projected_gps_point.latitude;
    vehicle_kinematics.geographic_pose.position.longitude = projected_gps_point.longitude;
    vehicle_kinematics.geographic_pose.position.altitude = geography_utils::convert_height(
      projected_gps_point.altitude, projected_gps_point.latitude, projected_gps_point.longitude,
      map_projector_info_->vertical_datum, MapProjectorInfo::WGS84);
  } else {
    vehicle_kinematics.geographic_pose.position.latitude = std::numeric_limits<double>::quiet_NaN();
    vehicle_kinematics.geographic_pose.position.longitude =
      std::numeric_limits<double>::quiet_NaN();
    vehicle_kinematics.geographic_pose.position.altitude = std::numeric_limits<double>::quiet_NaN();
  }
  vehicle_kinematics.accel.header = acceleration_msgs_->header;
  vehicle_kinematics.accel.accel = acceleration_msgs_->accel;
  pub_kinematics_->publish(vehicle_kinematics);
}

void VehicleNode::publish_status()
{
  steering_status_msgs_ = steering_sub_->takeData();
  gear_status_msgs_ = gear_state_sub_->takeData();
  turn_indicator_status_msgs_ = turn_indicator_info_sub_->takeData();
  hazard_light_status_msgs_ = hazard_light_sub_->takeData();
  energy_status_msgs_ = energy_level_sub_->takeData();

  if (
    !steering_status_msgs_ || !gear_status_msgs_ || !turn_indicator_status_msgs_ ||
    !hazard_light_status_msgs_ || !energy_status_msgs_)
    return;

  autoware_ad_api::vehicle::VehicleStatus::Message vehicle_status;
  vehicle_status.stamp = now();
  vehicle_status.steering_tire_angle = steering_status_msgs_->steering_tire_angle;
  vehicle_status.gear.status = mapping(gear_type_, gear_status_msgs_->report, ApiGear::UNKNOWN);
  vehicle_status.turn_indicators.status =
    mapping(turn_indicator_type_, turn_indicator_status_msgs_->report, ApiTurnIndicator::UNKNOWN);
  vehicle_status.hazard_lights.status =
    mapping(hazard_light_type_, hazard_light_status_msgs_->report, ApiHazardLight::UNKNOWN);
  vehicle_status.energy_percentage = energy_status_msgs_->energy_level;
  pub_status_->publish(vehicle_status);
}

void VehicleNode::on_timer()
{
  publish_kinematics();
  publish_status();
}

}  // namespace default_ad_api

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(default_ad_api::VehicleNode)

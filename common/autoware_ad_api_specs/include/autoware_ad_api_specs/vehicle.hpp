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

#ifndef AUTOWARE_AD_API_SPECS__VEHICLE_HPP_
#define AUTOWARE_AD_API_SPECS__VEHICLE_HPP_

#include <rclcpp/qos.hpp>

#include <autoware_adapi_v1_msgs/msg/door_status_array.hpp>
#include <autoware_adapi_v1_msgs/msg/vehicle_state.hpp>
#include <autoware_adapi_v1_msgs/msg/vehicle_status.hpp>

namespace autoware_ad_api::vehicle
{

struct VehicleStatus
{
  using Message = autoware_adapi_v1_msgs::msg::VehicleStatus;
  static constexpr char name[] = "/api/vehicle/status";
  static constexpr size_t depth = 1;
  static constexpr auto reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  static constexpr auto durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
};

struct VehicleState
{
  using Message = autoware_adapi_v1_msgs::msg::VehicleState;
  static constexpr char name[] = "/api/vehicle/state";
  static constexpr size_t depth = 1;
  static constexpr auto reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  static constexpr auto durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
};

struct DoorStatusArray
{
  using Message = autoware_adapi_v1_msgs::msg::DoorStatusArray;
  static constexpr char name[] = "/api/vehicle/door";
  static constexpr size_t depth = 1;
  static constexpr auto reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  static constexpr auto durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
};

}  // namespace autoware_ad_api::vehicle

#endif  // AUTOWARE_AD_API_SPECS__VEHICLE_HPP_

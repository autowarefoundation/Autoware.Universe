//  Copyright 2024 Tier IV, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef AUTOWARE_RAW_VEHICLE_CMD_CONVERTER__VEHICLE_ADAPTOR__VEHICLE_ADAPTOR_HPP_
#define AUTOWARE_RAW_VEHICLE_CMD_CONVERTER__VEHICLE_ADAPTOR__VEHICLE_ADAPTOR_HPP_

#include <autoware_adapi_v1_msgs/msg/operation_mode_state.hpp>
#include <autoware_control_msgs/msg/control.hpp>
#include <autoware_control_msgs/msg/control_horizon.hpp>
#include <autoware_vehicle_msgs/msg/steering_report.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

namespace autoware::raw_vehicle_cmd_converter
{

using autoware_adapi_v1_msgs::msg::OperationModeState;
using autoware_control_msgs::msg::Control;
using autoware_control_msgs::msg::ControlHorizon;
using autoware_vehicle_msgs::msg::SteeringReport;
using geometry_msgs::msg::AccelWithCovarianceStamped;
using nav_msgs::msg::Odometry;

class VehicleAdaptor
{
public:
  VehicleAdaptor() = default;
  Control compensate(
    const Control & input_control_cmd, [[maybe_unused]] const Odometry & odometry,
    [[maybe_unused]] const AccelWithCovarianceStamped & accel,
    [[maybe_unused]] const double steering,
    [[maybe_unused]] const OperationModeState & operation_mode,
    [[maybe_unused]] const ControlHorizon & control_horizon);

private:
};
}  // namespace autoware::raw_vehicle_cmd_converter

#endif  // AUTOWARE_RAW_VEHICLE_CMD_CONVERTER__VEHICLE_ADAPTOR__VEHICLE_ADAPTOR_HPP_

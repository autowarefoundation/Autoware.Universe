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

#include "autoware_raw_vehicle_cmd_converter/vehicle_adaptor/vehicle_adaptor.hpp"

#include <iostream>

namespace autoware::raw_vehicle_cmd_converter
{
Control VehicleAdaptor::compensate(
  const Control & input_control_cmd, [[maybe_unused]] const Odometry & odometry,
  [[maybe_unused]] const AccelWithCovarianceStamped & accel, [[maybe_unused]] const double steering,
  [[maybe_unused]] const OperationModeState & operation_mode,
  [[maybe_unused]] const ControlHorizon & control_horizon)
{
  // TODO(someone): implement the control command compensation
  Control output_control_cmd = input_control_cmd;
  std::cerr << "vehicle adaptor: compensate control command" << std::endl;
  return output_control_cmd;
}
}  // namespace autoware::raw_vehicle_cmd_converter

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

#include <cmath>
#include <iostream>

namespace autoware::raw_vehicle_cmd_converter
{
double get_current_yaw(const Odometry & odometry, double yaw_prev)
{
  double yaw = std::atan2(
    2.0 * (odometry.pose.pose.orientation.w * odometry.pose.pose.orientation.z),
    1.0 - 2.0 * (odometry.pose.pose.orientation.z * odometry.pose.pose.orientation.z));
  while (yaw - yaw_prev > M_PI) {
    yaw -= 2.0 * M_PI;
  }
  while (yaw - yaw_prev < -M_PI) {
    yaw += 2.0 * M_PI;
  }
  return yaw;
}
Control VehicleAdaptor::compensate(
  const Control & input_control_cmd, [[maybe_unused]] const Odometry & odometry,
  [[maybe_unused]] const AccelWithCovarianceStamped & accel, [[maybe_unused]] const double steering,
  [[maybe_unused]] const OperationModeState & operation_mode,
  [[maybe_unused]] const ControlHorizon & control_horizon)
{
  if (!proxima_vehicle_adaptor_.use_vehicle_adaptor_) {
    proxima_vehicle_adaptor_.send_initialized_flag();
    proxima_vehicle_adaptor_.set_params();
    return input_control_cmd;
  }
  if (!initialized_) {
    proxima_vehicle_adaptor_.clear_NN_params();
    proxima_vehicle_adaptor_.set_NN_params_from_csv("vehicle_models/vehicle_model_1");
    proxima_vehicle_adaptor_.set_NN_params_from_csv("vehicle_models/vehicle_model_2");
    proxima_vehicle_adaptor_.set_NN_params_from_csv("vehicle_models/vehicle_model_3");
    proxima_vehicle_adaptor_.set_NN_params_from_csv("vehicle_models/vehicle_model_4");
    proxima_vehicle_adaptor_.set_NN_params_from_csv("vehicle_models/vehicle_model_5");
    proxima_vehicle_adaptor_.send_initialized_flag();

    if (proxima_vehicle_adaptor_.use_offline_features_autoware_) {
      proxima_vehicle_adaptor_.set_offline_features_from_csv("vehicle_models/vehicle_model_1");
    }
    initialized_ = true;
  }
  Eigen::VectorXd states(6);
  const double yaw = get_current_yaw(odometry, yaw_prev_);
  states[0] = odometry.pose.pose.position.x;
  states[1] = odometry.pose.pose.position.y;
  states[2] = odometry.twist.twist.linear.x;
  states[3] = yaw;
  states[4] = accel.accel.accel.linear.x;
  states[5] = steering;
  double control_timestamp = input_control_cmd.stamp.sec + input_control_cmd.stamp.nanosec * 1e-9;
  bool is_applying_control = operation_mode.mode > 1 && operation_mode.is_autoware_control_enabled;
  if (!is_applying_control) {
    proxima_vehicle_adaptor_.send_initialized_flag();
    std::cerr << "vehicle adaptor on" << std::endl;
    return input_control_cmd;
  }
  if (proxima_vehicle_adaptor_.use_controller_steer_input_schedule_) {
    std::vector<double> steer_controller_input_schedule(control_horizon.controls.size());
    for (int i = 0; i < int(control_horizon.controls.size()); i++) {
      steer_controller_input_schedule[i] = control_horizon.controls[i].lateral.steering_tire_angle;
    }
    proxima_vehicle_adaptor_.set_controller_steer_input_schedule(
      control_timestamp, steer_controller_input_schedule);
  }
  Eigen::Vector2d vehicle_adaptor_control_cmd = proxima_vehicle_adaptor_.get_adjusted_inputs(
    control_timestamp, states, input_control_cmd.longitudinal.acceleration,
    input_control_cmd.lateral.steering_tire_angle);
  Control output_control_cmd = input_control_cmd;

  output_control_cmd.longitudinal.acceleration = vehicle_adaptor_control_cmd[0];
  output_control_cmd.lateral.steering_tire_angle = vehicle_adaptor_control_cmd[1];
  yaw_prev_ = yaw;
  return output_control_cmd;
}
}  // namespace autoware::raw_vehicle_cmd_converter

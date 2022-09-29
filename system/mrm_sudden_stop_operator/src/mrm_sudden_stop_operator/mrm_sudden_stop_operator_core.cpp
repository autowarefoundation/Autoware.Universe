// Copyright 2022 Tier IV, Inc.
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

#include "mrm_sudden_stop_operator/mrm_sudden_stop_operator_core.hpp"

namespace mrm_sudden_stop_operator
{

MRMSuddenStopOperator::MRMSuddenStopOperator(const rclcpp::NodeOptions & node_options)
: Node("mrm_sudden_stop_operator", node_options)
{
  // Parameter
  params_.update_rate = static_cast<int>(declare_parameter<int>("update_rate", 30));
  params_.target_acceleration = declare_parameter<double>("target_acceleration", -2.5);
  params_.target_jerk = declare_parameter<double>("target_jerk", -1.5);

  // Subscriber
  sub_control_cmd_ = create_subscription<AckermannControlCommand>(
    "~/input/control/control_cmd", 1,
    std::bind(&MRMSuddenStopOperator::onControlCommand, this, std::placeholders::_1));

  // Server
  service_operation_ = create_service<OperateMRM>(
    "~/input/mrm/sudden_stop/operate",
    std::bind(&MRMSuddenStopOperator::operateSuddenStop, this,
    std::placeholders::_1, std::placeholders::_2));

  // Publisher
  pub_status_ = create_publisher<MRMBehaviorStatus>(
    "~/output/mrm/sudden_stop/status", 1);
  pub_control_cmd_ = create_publisher<AckermannControlCommand>(
    "~/output/mrm/sudden_stop/control_cmd", 1);

  // Timer
  const auto update_period_ns = rclcpp::Rate(params_.update_rate).period();
  timer_ = rclcpp::create_timer(
    this, get_clock(), update_period_ns, std::bind(&MRMSuddenStopOperator::onTimer, this));

  // Initialize
  status_.is_available = true;
  status_.is_operating = false;
  is_prev_control_cmd_subscribed_ = false;
}


void MRMSuddenStopOperator::onControlCommand(
    AckermannControlCommand::ConstSharedPtr msg)
{
  if (status_.is_operating == false) {
    prev_control_cmd_ = *msg;
    is_prev_control_cmd_subscribed_ = true;
  }
}

void MRMSuddenStopOperator::operateSuddenStop(
    const OperateMRM::Request::SharedPtr request,
    const OperateMRM::Response::SharedPtr response)
{
  if (request->operate == true) {
    status_.is_operating = true;
    response->response.success = true;
  } else {
    status_.is_operating = false;
    response->response.success = true;
  }
}

void MRMSuddenStopOperator::publishStatus() const
{
  auto status = status_;
  status.stamp = this->now();
  pub_status_->publish(status);
}

void MRMSuddenStopOperator::publishControlCommand(const AckermannControlCommand & command) const
{
  pub_control_cmd_->publish(command);
}

void MRMSuddenStopOperator::onTimer()
{
  if (status_.is_operating) {
    auto control_cmd = calcTargetAcceleration(prev_control_cmd_);
    publishControlCommand(control_cmd);
    prev_control_cmd_ = control_cmd;
  } else {
    publishControlCommand(prev_control_cmd_);
  }
  publishStatus();
}

AckermannControlCommand MRMSuddenStopOperator::calcTargetAcceleration(
  const AckermannControlCommand & prev_control_cmd) const
{
  auto control_cmd = AckermannControlCommand();

  if (!is_prev_control_cmd_subscribed_) {
    control_cmd.stamp = this->now();
    control_cmd.longitudinal.stamp = this->now();
    control_cmd.longitudinal.speed = 0.0;
    control_cmd.longitudinal.acceleration = static_cast<float>(params_.target_acceleration);
    control_cmd.longitudinal.jerk = 0.0;
    control_cmd.lateral.stamp = this->now();
    control_cmd.lateral.steering_tire_angle = 0.0;
    control_cmd.lateral.steering_tire_rotation_rate = 0.0;
    return control_cmd;
  }

  control_cmd = prev_control_cmd;
  const auto dt = (this->now() - prev_control_cmd.stamp).seconds();

  control_cmd.stamp = this->now();
  control_cmd.longitudinal.stamp = this->now();
  control_cmd.longitudinal.speed = static_cast<float>(std::max(
    prev_control_cmd.longitudinal.speed + prev_control_cmd.longitudinal.acceleration * dt,
    0.0));
  control_cmd.longitudinal.acceleration = static_cast<float>(std::max(
    prev_control_cmd.longitudinal.acceleration + params_.target_jerk * dt,
    params_.target_acceleration));
  if (prev_control_cmd.longitudinal.acceleration == params_.target_acceleration) {
    control_cmd.longitudinal.jerk = 0.0;
  } else {
    control_cmd.longitudinal.jerk = static_cast<float>(params_.target_jerk);
  }

  return control_cmd;
}

}  // namespace mrm_sudden_stop_operator

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(mrm_sudden_stop_operator::MRMSuddenStopOperator)

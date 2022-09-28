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

#include "mrm_comfortable_stop_operator/mrm_comfortable_stop_operator_core.hpp"

namespace mrm_comfortable_stop_operator
{

MRMComfortableStopOperator::MRMComfortableStopOperator(const rclcpp::NodeOptions & node_options)
: Node("mrm_comfortable_stop_operator", node_options)
{
  // Parameter
  params_.update_rate = static_cast<int>(declare_parameter<int>("update_rate", 1));
  params_.min_acceleration = declare_parameter<double>("min_acceleration", -1.0);
  params_.max_jerk = declare_parameter<double>("max_jerk", 0.3);
  params_.min_jerk = declare_parameter<double>("min_jerk", 0.3);

  // Server
  service_operation_ = create_service<autoware_ad_api_msgs::srv::MRMOperation>(
    "~/input/mrm/comfortable_stop/operate",
    std::bind(&MRMComfortableStopOperator::operateComfortableStop, this,
    std::placeholders::_1, std::placeholders::_2));

  // Publisher
  pub_status_ = create_publisher<autoware_ad_api_msgs::msg::MRMStatus>(
    "~/output/mrm/comfortable_stop/status", 1);
  pub_velocity_limit_ = create_publisher<tier4_planning_msgs::msg::VelocityLimit>(
    "~/output/velocity_limit", 1);
  pub_velocity_limit_clear_command_ = create_publisher<tier4_planning_msgs::msg::VelocityLimitClearCommand>(
    "~/output/velocity_limit/clear", 1);

  // Timer
  const auto update_period_ns = rclcpp::Rate(params_.update_rate).period();
  timer_ = rclcpp::create_timer(
    this, get_clock(), update_period_ns, std::bind(&MRMComfortableStopOperator::onTimer, this));

  // Initialize
  is_available_ = true;
  is_operating_ = false;
}

void MRMComfortableStopOperator::operateComfortableStop(
    const autoware_ad_api_msgs::srv::MRMOperation::Request::SharedPtr request,
    const autoware_ad_api_msgs::srv::MRMOperation::Response::SharedPtr response)
{
  if(request->operate == true) {
    publishVelocityLimit();
    is_operating_ = true;
    response->response.success = true;
  } else {
    publishVelocityLimitClearCommand();
    is_operating_ = false;
    response->response.success = true;
  }
}

void MRMComfortableStopOperator::publishStatus() const
{
  auto status = autoware_ad_api_msgs::msg::MRMStatus();
  status.is_available = is_available_;
  status.is_operating = is_operating_;

  pub_status_->publish(status);
}

void MRMComfortableStopOperator::publishVelocityLimit() const
{
  auto velocity_limit = tier4_planning_msgs::msg::VelocityLimit();
  velocity_limit.stamp = this->now();
  velocity_limit.max_velocity = 0;
  velocity_limit.use_constraints = true;
  velocity_limit.constraints.min_acceleration = static_cast<float>(params_.min_acceleration);
  velocity_limit.constraints.max_jerk = static_cast<float>(params_.max_jerk);
  velocity_limit.constraints.min_jerk = static_cast<float>(params_.min_jerk);
  velocity_limit.sender = "mrm_comfortable_stop_operator";

  pub_velocity_limit_->publish(velocity_limit);
}

void MRMComfortableStopOperator::publishVelocityLimitClearCommand() const
{
  auto velocity_limit_clear_command = tier4_planning_msgs::msg::VelocityLimitClearCommand();
  velocity_limit_clear_command.stamp = this->now();
  velocity_limit_clear_command.command = true;
  velocity_limit_clear_command.sender = "mrm_comfortable_stop_operator";

  pub_velocity_limit_clear_command_->publish(velocity_limit_clear_command);
}

void MRMComfortableStopOperator::onTimer() const
{
  publishStatus();
}

}  // namespace mrm_comfortable_stop_operator

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(mrm_comfortable_stop_operator::MRMComfortableStopOperator)

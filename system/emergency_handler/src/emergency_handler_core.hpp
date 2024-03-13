// Copyright 2020 Tier IV, Inc.
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

#ifndef EMERGENCY_HANDLER_CORE_HPP_
#define EMERGENCY_HANDLER_CORE_HPP_

// Core
#include <memory>
#include <string>

// Autoware
#include <mrm_comfortable_stop_operator/mrm_comfortable_stop_operator.hpp>
#include <mrm_emergency_stop_operator/mrm_emergency_stop_operator.hpp>

#include <autoware_adapi_v1_msgs/msg/mrm_state.hpp>
#include <autoware_auto_system_msgs/msg/hazard_status_stamped.hpp>
#include <autoware_auto_vehicle_msgs/msg/control_mode_report.hpp>
#include <diagnostic_msgs/msg/diagnostic_array.hpp>
#include <nav_msgs/msg/odometry.hpp>

// ROS 2 core
#include <rclcpp/rclcpp.hpp>

namespace emergency_handler
{

struct Param
{
  int update_rate;
  double timeout_hazard_status;
  bool use_comfortable_stop;
};

class EmergencyHandler : public rclcpp::Node
{
public:
  EmergencyHandler();

private:
  // Subscribers
  rclcpp::Subscription<autoware_auto_system_msgs::msg::HazardStatusStamped>::SharedPtr
    sub_hazard_status_stamped_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_odom_;
  rclcpp::Subscription<autoware_auto_vehicle_msgs::msg::ControlModeReport>::SharedPtr
    sub_control_mode_;

  autoware_auto_system_msgs::msg::HazardStatusStamped::ConstSharedPtr hazard_status_stamped_;
  nav_msgs::msg::Odometry::ConstSharedPtr odom_;
  autoware_auto_vehicle_msgs::msg::ControlModeReport::ConstSharedPtr control_mode_;

  void onHazardStatusStamped(
    const autoware_auto_system_msgs::msg::HazardStatusStamped::ConstSharedPtr msg);
  void onOdometry(const nav_msgs::msg::Odometry::ConstSharedPtr msg);
  void onControlMode(const autoware_auto_vehicle_msgs::msg::ControlModeReport::ConstSharedPtr msg);

  // Publisher
  rclcpp::Publisher<autoware_adapi_v1_msgs::msg::MrmState>::SharedPtr pub_mrm_state_;
  autoware_adapi_v1_msgs::msg::MrmState mrm_state_;
  void publishMrmState();

  void callMrmBehavior(
    const autoware_adapi_v1_msgs::msg::MrmState::_behavior_type & mrm_behavior) const;
  void cancelMrmBehavior(
    const autoware_adapi_v1_msgs::msg::MrmState::_behavior_type & mrm_behavior) const;

  // Timer
  rclcpp::TimerBase::SharedPtr timer_;

  // Parameters
  Param param_;

  bool isDataReady();
  void onTimer();

  // Heartbeat
  rclcpp::Time stamp_hazard_status_;
  bool is_hazard_status_timeout_;
  void checkHazardStatusTimeout();

  // Operators
  std::unique_ptr<mrm_emergency_stop_operator::MrmEmergencyStopOperator>
    mrm_emergency_stop_operator_;
  std::unique_ptr<mrm_comfortable_stop_operator::MrmComfortableStopOperator>
    mrm_comfortable_stop_operator_;

  // Algorithm
  void transitionTo(const int new_state);
  void updateMrmState();
  void operateMrm();
  autoware_adapi_v1_msgs::msg::MrmState::_behavior_type getCurrentMrmBehavior();
  bool isStopped();
  bool isEmergency();
};

}  // namespace emergency_handler

#endif  // EMERGENCY_HANDLER_CORE_HPP_

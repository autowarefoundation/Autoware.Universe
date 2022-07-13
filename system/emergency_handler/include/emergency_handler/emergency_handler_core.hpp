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

#ifndef EMERGENCY_HANDLER__EMERGENCY_HANDLER_CORE_HPP_
#define EMERGENCY_HANDLER__EMERGENCY_HANDLER_CORE_HPP_

// Core
#include <memory>
#include <string>

// Autoware
#include <autoware_auto_control_msgs/msg/ackermann_control_command.hpp>
#include <autoware_auto_system_msgs/msg/emergency_state.hpp>
#include <autoware_auto_system_msgs/msg/hazard_status_stamped.hpp>
#include <autoware_ad_api_msgs/srv/mrm_operation.hpp>
#include <autoware_ad_api_msgs/msg/mrm_status.hpp>
#include <autoware_ad_api_msgs/msg/mrm_behavior_status.hpp>
#include <autoware_auto_vehicle_msgs/msg/control_mode_report.hpp>
#include <autoware_auto_vehicle_msgs/msg/gear_command.hpp>
#include <autoware_auto_vehicle_msgs/msg/hazard_lights_command.hpp>

// ROS2 core
#include <rclcpp/create_timer.hpp>
#include <rclcpp/rclcpp.hpp>
#include <tier4_autoware_utils/system/heartbeat_checker.hpp>

#include <diagnostic_msgs/msg/diagnostic_array.hpp>
#include <nav_msgs/msg/odometry.hpp>

struct HazardLampPolicy
{
  bool emergency;
};

struct Param
{
  int update_rate;
  double timeout_hazard_status;
  double timeout_takeover_request;
  bool use_takeover_request;
  bool use_parking_after_stopped;
  HazardLampPolicy turning_hazard_on{};
};

class EmergencyHandler : public rclcpp::Node
{
public:
  EmergencyHandler();

private:
  // Subscribers
  rclcpp::Subscription<autoware_auto_system_msgs::msg::HazardStatusStamped>::SharedPtr
    sub_hazard_status_stamped_;
  rclcpp::Subscription<autoware_auto_control_msgs::msg::AckermannControlCommand>::SharedPtr
    sub_prev_control_command_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_odom_;
  rclcpp::Subscription<autoware_auto_vehicle_msgs::msg::ControlModeReport>::SharedPtr
    sub_control_mode_;
  rclcpp::Subscription<autoware_ad_api_msgs::msg::MRMBehaviorStatus>::SharedPtr sub_mrm_comfortable_stop_status_;
  rclcpp::Subscription<autoware_ad_api_msgs::msg::MRMBehaviorStatus>::SharedPtr sub_mrm_sudden_stop_status_;

  autoware_auto_system_msgs::msg::HazardStatusStamped::ConstSharedPtr hazard_status_stamped_;
  autoware_auto_control_msgs::msg::AckermannControlCommand::ConstSharedPtr prev_control_command_;
  nav_msgs::msg::Odometry::ConstSharedPtr odom_;
  autoware_auto_vehicle_msgs::msg::ControlModeReport::ConstSharedPtr control_mode_;
  autoware_ad_api_msgs::msg::MRMBehaviorStatus::ConstSharedPtr mrm_comfortable_stop_status_;
  autoware_ad_api_msgs::msg::MRMBehaviorStatus::ConstSharedPtr mrm_sudden_stop_status_;

  void onHazardStatusStamped(
    const autoware_auto_system_msgs::msg::HazardStatusStamped::ConstSharedPtr msg);
  void onPrevControlCommand(
    const autoware_auto_control_msgs::msg::AckermannControlCommand::ConstSharedPtr msg);
  void onOdometry(const nav_msgs::msg::Odometry::ConstSharedPtr msg);
  void onControlMode(const autoware_auto_vehicle_msgs::msg::ControlModeReport::ConstSharedPtr msg);
  void onMRMComfortableStopStatus(const autoware_ad_api_msgs::msg::MRMBehaviorStatus::ConstSharedPtr msg);
  void onMRMSuddenStopStatus(const autoware_ad_api_msgs::msg::MRMBehaviorStatus::ConstSharedPtr msg);

  // Publisher
  rclcpp::Publisher<autoware_auto_control_msgs::msg::AckermannControlCommand>::SharedPtr
    pub_control_command_;

  // rclcpp::Publisher<tier4_vehicle_msgs::msg::ShiftStamped>::SharedPtr pub_shift_;
  // rclcpp::Publisher<tier4_vehicle_msgs::msg::TurnSignal>::SharedPtr pub_turn_signal_;
  rclcpp::Publisher<autoware_auto_vehicle_msgs::msg::HazardLightsCommand>::SharedPtr
    pub_hazard_cmd_;
  rclcpp::Publisher<autoware_auto_vehicle_msgs::msg::GearCommand>::SharedPtr pub_gear_cmd_;
  rclcpp::Publisher<autoware_auto_system_msgs::msg::EmergencyState>::SharedPtr pub_emergency_state_;

  autoware_auto_vehicle_msgs::msg::HazardLightsCommand createHazardCmdMsg();
  autoware_auto_vehicle_msgs::msg::GearCommand createGearCmdMsg();
  void publishControlCommands();

  // Clients
  rclcpp::CallbackGroup::SharedPtr client_mrm_comfortable_stop_group_;
  rclcpp::Client<autoware_ad_api_msgs::srv::MRMOperation>::SharedPtr client_mrm_comfortable_stop_;
  rclcpp::CallbackGroup::SharedPtr client_mrm_sudden_stop_group_;
  rclcpp::Client<autoware_ad_api_msgs::srv::MRMOperation>::SharedPtr client_mrm_sudden_stop_;

  void callMRMBehavior(const autoware_ad_api_msgs::msg::MRMStatus::_behavior_type & mrm_behavior) const;
  void cancelMRMBehavior(const autoware_ad_api_msgs::msg::MRMStatus::_behavior_type & mrm_behavior) const;
  void logMRMCallingResult(
    const autoware_ad_api_msgs::srv::MRMOperation::Response & result, const std::string & behavior,
    bool is_call) const;

  // Timer
  rclcpp::TimerBase::SharedPtr timer_;

  // Parameters
  Param param_;

  bool isDataReady();
  void onTimer();

  // Heartbeat
  std::shared_ptr<HeaderlessHeartbeatChecker<autoware_auto_system_msgs::msg::HazardStatusStamped>>
    heartbeat_hazard_status_;

  // Algorithm
  autoware_auto_system_msgs::msg::EmergencyState::_state_type emergency_state_{
    autoware_auto_system_msgs::msg::EmergencyState::NORMAL};
  rclcpp::Time takeover_requested_time_;
  autoware_ad_api_msgs::msg::MRMStatus::_behavior_type mrm_behavior_{
    autoware_ad_api_msgs::msg::MRMStatus::NONE};

  void transitionTo(const int new_state);
  void updateEmergencyState();
  void operateMRM();
  autoware_ad_api_msgs::msg::MRMStatus::_behavior_type updateMRMBehavior();
  bool isStopped();
  bool isEmergency(const autoware_auto_system_msgs::msg::HazardStatus & hazard_status);
  autoware_auto_control_msgs::msg::AckermannControlCommand selectAlternativeControlCommand();
};

#endif  // EMERGENCY_HANDLER__EMERGENCY_HANDLER_CORE_HPP_

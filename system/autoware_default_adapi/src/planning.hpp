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

#ifndef PLANNING_HPP_
#define PLANNING_HPP_

#include <autoware/component_interface_specs/localization.hpp>
#include <autoware/component_interface_specs/planning.hpp>
#include <autoware/motion_utils/vehicle/vehicle_state_checker.hpp>
#include <autoware_ad_api_specs/planning.hpp>
#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <vector>

// This file should be included after messages.
#include "utils/types.hpp"

namespace autoware::default_adapi
{

class PlanningNode : public rclcpp::Node
{
public:
  explicit PlanningNode(const rclcpp::NodeOptions & options);

private:
  using VelocityFactorArray = autoware_adapi_v1_msgs::msg::VelocityFactorArray;
  using SteeringFactorArray = autoware_adapi_v1_msgs::msg::SteeringFactorArray;
  Pub<autoware_ad_api::planning::VelocityFactors> pub_velocity_factors_;
  Pub<autoware_ad_api::planning::SteeringFactors> pub_steering_factors_;
  Sub<autoware::component_interface_specs::planning::Trajectory> sub_trajectory_;
  Sub<autoware::component_interface_specs::localization::KinematicState> sub_kinematic_state_;
  std::vector<rclcpp::Subscription<VelocityFactorArray>::SharedPtr> sub_velocity_factors_;
  std::vector<rclcpp::Subscription<SteeringFactorArray>::SharedPtr> sub_steering_factors_;
  std::vector<VelocityFactorArray::ConstSharedPtr> velocity_factors_;
  std::vector<SteeringFactorArray::ConstSharedPtr> steering_factors_;
  rclcpp::TimerBase::SharedPtr timer_;

  using VehicleStopChecker = autoware::motion_utils::VehicleStopCheckerBase;
  using Trajectory = autoware::component_interface_specs::planning::Trajectory::Message;
  using KinematicState = autoware::component_interface_specs::localization::KinematicState::Message;
  void on_trajectory(const Trajectory::ConstSharedPtr msg);
  void on_kinematic_state(const KinematicState::ConstSharedPtr msg);
  void on_timer();

  double stop_distance_;
  double stop_duration_;
  std::unique_ptr<VehicleStopChecker> stop_checker_;
  Trajectory::ConstSharedPtr trajectory_;
  KinematicState::ConstSharedPtr kinematic_state_;
};

}  // namespace autoware::default_adapi

#endif  // PLANNING_HPP_

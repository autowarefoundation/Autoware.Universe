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

#ifndef AUTOWARE__MOTION_UTILS__FACTOR__STEERING_FACTOR_INTERFACE_HPP_
#define AUTOWARE__MOTION_UTILS__FACTOR__STEERING_FACTOR_INTERFACE_HPP_

#include <autoware_adapi_v1_msgs/msg/planning_behavior.hpp>
#include <autoware_adapi_v1_msgs/msg/steering_factor.hpp>
#include <geometry_msgs/msg/pose.hpp>

#include <string>

namespace autoware::motion_utils
{

using autoware_adapi_v1_msgs::msg::PlanningBehavior;
using autoware_adapi_v1_msgs::msg::SteeringFactor;
using SteeringFactorBehavior = SteeringFactor::_behavior_type;
using SteeringFactorStatus = SteeringFactor::_status_type;
using geometry_msgs::msg::Pose;

class SteeringFactorInterface
{
public:
  [[nodiscard]] SteeringFactor get() const { return steering_factor_; }
  void init(const SteeringFactorBehavior & behavior) { behavior_ = behavior; }
  void reset() { steering_factor_.behavior = PlanningBehavior::UNKNOWN; }

  void set(
    const std::array<Pose, 2> & pose, const std::array<double, 2> distance,
    const uint16_t direction, const uint16_t status, const std::string & detail = "");

private:
  SteeringFactorBehavior behavior_{SteeringFactor::UNKNOWN};
  SteeringFactor steering_factor_{};
};

}  // namespace autoware::motion_utils

#endif  // AUTOWARE__MOTION_UTILS__FACTOR__STEERING_FACTOR_INTERFACE_HPP_

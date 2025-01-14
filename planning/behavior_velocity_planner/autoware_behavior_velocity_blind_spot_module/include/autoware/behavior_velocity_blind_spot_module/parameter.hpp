// Copyright 2024 TIER IV, Inc.
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

#ifndef AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__PARAMETER_HPP_
#define AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__PARAMETER_HPP_

#include <rclcpp/node.hpp>

#include <string>

namespace autoware::behavior_velocity_planner
{
struct PlannerParam
{
  static PlannerParam init(rclcpp::Node & node, const std::string & ns);
  bool use_pass_judge_line{};
  double stop_line_margin{};
  double backward_detection_length{};
  double ignore_width_from_center_line{};
  double adjacent_extend_width{};
  double opposite_adjacent_extend_width{};
  double max_future_movement_time{};
  double ttc_min{};
  double ttc_max{};
  double ttc_ego_minimal_velocity{};
};
}  // namespace autoware::behavior_velocity_planner

#endif  // AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__PARAMETER_HPP_

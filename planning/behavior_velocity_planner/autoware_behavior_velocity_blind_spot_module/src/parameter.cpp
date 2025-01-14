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

#include "autoware/behavior_velocity_blind_spot_module/parameter.hpp"

#include <autoware/universe_utils/ros/parameter.hpp>

#include <string>

namespace autoware::behavior_velocity_planner
{

PlannerParam PlannerParam::init(rclcpp::Node & node, const std::string & ns)
{
  using autoware::universe_utils::getOrDeclareParameter;
  PlannerParam param;
  param.use_pass_judge_line = getOrDeclareParameter<bool>(node, ns + ".use_pass_judge_line");
  param.stop_line_margin = getOrDeclareParameter<double>(node, ns + ".stop_line_margin");
  param.backward_detection_length =
    getOrDeclareParameter<double>(node, ns + ".backward_detection_length");
  param.ignore_width_from_center_line =
    getOrDeclareParameter<double>(node, ns + ".ignore_width_from_center_line");
  param.adjacent_extend_width = getOrDeclareParameter<double>(node, ns + ".adjacent_extend_width");
  param.opposite_adjacent_extend_width =
    getOrDeclareParameter<double>(node, ns + ".opposite_adjacent_extend_width");
  param.max_future_movement_time =
    getOrDeclareParameter<double>(node, ns + ".max_future_movement_time");
  param.ttc_min = getOrDeclareParameter<double>(node, ns + ".ttc_min");
  param.ttc_max = getOrDeclareParameter<double>(node, ns + ".ttc_max");
  param.ttc_ego_minimal_velocity =
    getOrDeclareParameter<double>(node, ns + ".ttc_ego_minimal_velocity");
  return param;
}
}  // namespace autoware::behavior_velocity_planner

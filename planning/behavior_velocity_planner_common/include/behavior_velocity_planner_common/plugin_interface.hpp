// Copyright 2023 The Autoware Contributors
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

#ifndef BEHAVIOR_VELOCITY_PLANNER_COMMON__PLUGIN_INTERFACE_HPP_
#define BEHAVIOR_VELOCITY_PLANNER_COMMON__PLUGIN_INTERFACE_HPP_

#include <rclcpp/rclcpp.hpp>

namespace behavior_velocity_planner
{

class PluginInterface
{
public:
  virtual ~PluginInterface() = default;
  virtual void init(rclcpp::Node & node) = 0;
};

}  // namespace behavior_velocity_planner

#endif  // BEHAVIOR_VELOCITY_PLANNER_COMMON__PLUGIN_INTERFACE_HPP_

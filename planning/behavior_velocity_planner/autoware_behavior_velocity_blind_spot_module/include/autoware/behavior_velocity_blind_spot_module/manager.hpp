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

#ifndef AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__MANAGER_HPP_
#define AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__MANAGER_HPP_

#include "autoware/behavior_velocity_blind_spot_module/parameter.hpp"
#include "autoware/behavior_velocity_blind_spot_module/scene.hpp"

#include <autoware/behavior_velocity_planner_common/plugin_interface.hpp>
#include <autoware/behavior_velocity_planner_common/plugin_wrapper.hpp>
#include <autoware/behavior_velocity_rtc_interface/scene_module_interface_with_rtc.hpp>
#include <rclcpp/rclcpp.hpp>

#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <functional>
#include <memory>

namespace autoware::behavior_velocity_planner
{

class BlindSpotModuleManager : public SceneModuleManagerInterfaceWithRTC
{
public:
  explicit BlindSpotModuleManager(rclcpp::Node & node);

  const char * getModuleName() override { return "blind_spot"; }

private:
  PlannerParam planner_param_;

  void launchNewModules(const tier4_planning_msgs::msg::PathWithLaneId & path) override;

  std::function<bool(const std::shared_ptr<SceneModuleInterfaceWithRTC> &)>
  getModuleExpiredFunction(const tier4_planning_msgs::msg::PathWithLaneId & path) override;
};

class BlindSpotModulePlugin : public PluginWrapper<BlindSpotModuleManager>
{
};

}  // namespace autoware::behavior_velocity_planner

#endif  // AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__MANAGER_HPP_

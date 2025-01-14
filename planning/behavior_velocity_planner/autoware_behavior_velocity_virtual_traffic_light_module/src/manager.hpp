// Copyright 2021 Tier IV, Inc.
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

#ifndef MANAGER_HPP_
#define MANAGER_HPP_

#include "scene.hpp"

#include <autoware/behavior_velocity_planner_common/plugin_interface.hpp>
#include <autoware/behavior_velocity_planner_common/plugin_wrapper.hpp>
#include <autoware/behavior_velocity_planner_common/scene_module_interface.hpp>
#include <autoware/universe_utils/ros/polling_subscriber.hpp>
#include <rclcpp/rclcpp.hpp>

#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>
#include <tier4_v2x_msgs/msg/infrastructure_command_array.hpp>
#include <tier4_v2x_msgs/msg/virtual_traffic_light_state_array.hpp>

#include <functional>
#include <memory>

namespace autoware::behavior_velocity_planner
{
class VirtualTrafficLightModuleManager
: public SceneModuleManagerInterface<VirtualTrafficLightModule>
{
public:
  explicit VirtualTrafficLightModuleManager(rclcpp::Node & node);

  const char * getModuleName() override { return "virtual_traffic_light"; }

private:
  VirtualTrafficLightModule::PlannerParam planner_param_;

  void modifyPathVelocity(tier4_planning_msgs::msg::PathWithLaneId * path) override;

  void launchNewModules(const tier4_planning_msgs::msg::PathWithLaneId & path) override;

  std::function<bool(const std::shared_ptr<VirtualTrafficLightModule> &)> getModuleExpiredFunction(
    const tier4_planning_msgs::msg::PathWithLaneId & path) override;

  autoware::universe_utils::InterProcessPollingSubscriber<
    tier4_v2x_msgs::msg::VirtualTrafficLightStateArray>::SharedPtr
    sub_virtual_traffic_light_states_;

  rclcpp::Publisher<tier4_v2x_msgs::msg::InfrastructureCommandArray>::SharedPtr
    pub_infrastructure_commands_;
};

class VirtualTrafficLightModulePlugin : public PluginWrapper<VirtualTrafficLightModuleManager>
{
};

}  // namespace autoware::behavior_velocity_planner

#endif  // MANAGER_HPP_

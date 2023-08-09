// Copyright 2023 TIER IV, Inc.
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

#ifndef BEHAVIOR_PATH_PLANNER__SCENE_MODULE__AVOIDANCE__MANAGER_HPP_
#define BEHAVIOR_PATH_PLANNER__SCENE_MODULE__AVOIDANCE__MANAGER_HPP_

#include "behavior_path_planner/scene_module/avoidance/avoidance_module.hpp"
#include "behavior_path_planner/scene_module/scene_module_manager_interface.hpp"
#include "behavior_path_planner/utils/avoidance/avoidance_module_data.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace behavior_path_planner
{

class AvoidanceModuleManager : public SceneModuleManagerInterface
{
public:
  AvoidanceModuleManager(
    rclcpp::Node * node, const std::string & name, const ModuleConfigParameters & config);

  std::vector<std::unique_ptr<SceneModuleInterface>> createNewSceneModuleInstance() override
  {
    std::vector<std::unique_ptr<SceneModuleInterface>> scene_module_ptrs;
    scene_module_ptrs.emplace_back(
      std::make_unique<AvoidanceModule>(name_, *node_, parameters_, rtc_interface_ptr_map_));
    return scene_module_ptrs;
  }

  void updateModuleParams(const std::vector<rclcpp::Parameter> & parameters) override;

private:
  std::shared_ptr<AvoidanceParameters> parameters_;
};

}  // namespace behavior_path_planner

#endif  // BEHAVIOR_PATH_PLANNER__SCENE_MODULE__AVOIDANCE__MANAGER_HPP_

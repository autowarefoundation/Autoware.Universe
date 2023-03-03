// Copyright 2023 Tier IV, Inc.
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

#include <scene_module/out_of_lane/manager.hpp>
#include <scene_module/out_of_lane/scene_out_of_lane.hpp>
#include <utilization/util.hpp>

#include <lanelet2_core/primitives/BasicRegulatoryElements.h>

#include <memory>
#include <string>
#include <vector>

namespace behavior_velocity_planner
{
OutOfLaneModuleManager::OutOfLaneModuleManager(rclcpp::Node & node)
: SceneModuleManagerInterface(node, getModuleName())
{
  const std::string ns(getModuleName());
  auto & pp = planner_param_;

  pp.dist_thr = node.declare_parameter<double>(ns + ".distance_threshold");
  pp.overlap_min_dist = node.declare_parameter<double>(ns + ".overlap_min_distance");
  pp.extra_front_offset = node.declare_parameter<double>(ns + ".ego.extra_front_offset");
  pp.extra_rear_offset = node.declare_parameter<double>(ns + ".ego.extra_rear_offset");
  pp.extra_left_offset = node.declare_parameter<double>(ns + ".ego.extra_left_offset");
  pp.extra_right_offset = node.declare_parameter<double>(ns + ".ego.extra_right_offset");
  const auto vehicle_info = vehicle_info_util::VehicleInfoUtil(node).getVehicleInfo();
  pp.front_offset = vehicle_info.max_longitudinal_offset_m;
  pp.rear_offset = vehicle_info.min_longitudinal_offset_m;
  pp.left_offset = vehicle_info.max_lateral_offset_m;
  pp.right_offset = vehicle_info.min_lateral_offset_m;
}

void OutOfLaneModuleManager::launchNewModules(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path)
{
  if (path.points.empty()) return;
  // general
  if (!isModuleRegistered(module_id_)) {
    registerModule(std::make_shared<OutOfLaneModule>(
      module_id_, planner_data_, planner_param_, logger_.get_child("out_of_lane_module"), clock_));
  }
}

std::function<bool(const std::shared_ptr<SceneModuleInterface> &)>
OutOfLaneModuleManager::getModuleExpiredFunction(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path)
{
  return [path]([[maybe_unused]] const std::shared_ptr<SceneModuleInterface> & scene_module) {
    return false;
  };
}
}  // namespace behavior_velocity_planner

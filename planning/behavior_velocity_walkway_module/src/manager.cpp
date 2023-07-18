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

#include "manager.hpp"

#include <behavior_velocity_planner_common/utilization/util.hpp>

#include <limits>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace behavior_velocity_planner
{

using lanelet::autoware::Crosswalk;

WalkwayModuleManager::WalkwayModuleManager(rclcpp::Node & node)
: SceneModuleManagerInterface(node, getModuleName())
{
  const std::string ns(getModuleName());

  // for walkway parameters
  auto & wp = walkway_planner_param_;
  wp.stop_line_distance = node.declare_parameter<double>(ns + ".stop_line_distance");
  wp.stop_duration_sec = node.declare_parameter<double>(ns + ".stop_duration_sec");
}

void WalkwayModuleManager::launchNewModules(const PathWithLaneId & path)
{
  const auto rh = planner_data_->route_handler_;
  if (!opt_use_regulatory_element_) {
    opt_use_regulatory_element_ = checkRegulatoryElementExistence(rh->getLaneletMapPtr());
  }

  const auto launch = [this, &path](const auto & lanelet) {
    const auto attribute =
      lanelet.attributeOr(lanelet::AttributeNamesString::Subtype, std::string(""));
    if (attribute != lanelet::AttributeValueString::Walkway) {
      return;
    }

    if (isModuleRegistered(lanelet.id())) {
      return;
    }

    const auto & p = walkway_planner_param_;
    const auto logger = logger_.get_child("walkway_module");
    const auto lanelet_map_ptr = planner_data_->route_handler_->getLaneletMapPtr();

    registerModule(std::make_shared<WalkwayModule>(
      lanelet.id(), lanelet_map_ptr, p, *opt_use_regulatory_element_, logger, clock_));
  };

  if (*opt_use_regulatory_element_) {
    const auto crosswalk_leg_elem_map = planning_utils::getRegElemMapOnPath<Crosswalk>(
      path, rh->getLaneletMapPtr(), planner_data_->current_odometry->pose);

    for (const auto & crosswalk : crosswalk_leg_elem_map) {
      launch(crosswalk.first->crosswalkLanelet());
    }
  } else {
    const auto crosswalk_lanelets = getCrosswalksOnPath(
      planner_data_->current_odometry->pose, path, rh->getLaneletMapPtr(),
      rh->getOverallGraphPtr());

    for (const auto & crosswalk : crosswalk_lanelets) {
      launch(crosswalk);
    }
  }
}

std::function<bool(const std::shared_ptr<SceneModuleInterface> &)>
WalkwayModuleManager::getModuleExpiredFunction(const PathWithLaneId & path)
{
  const auto rh = planner_data_->route_handler_;

  std::set<int64_t> walkway_id_set;

  if (*opt_use_regulatory_element_) {
    const auto crosswalk_leg_elem_map = planning_utils::getRegElemMapOnPath<Crosswalk>(
      path, rh->getLaneletMapPtr(), planner_data_->current_odometry->pose);

    for (const auto & crosswalk : crosswalk_leg_elem_map) {
      walkway_id_set.insert(crosswalk.first->id());
    }
  } else {
    walkway_id_set = getCrosswalkIdSetOnPath(
      planner_data_->current_odometry->pose, path, rh->getLaneletMapPtr(),
      rh->getOverallGraphPtr());
  }

  return [walkway_id_set](const std::shared_ptr<SceneModuleInterface> & scene_module) {
    return walkway_id_set.count(scene_module->getModuleId()) == 0;
  };
}
}  // namespace behavior_velocity_planner

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(
  behavior_velocity_planner::WalkwayModulePlugin, behavior_velocity_planner::PluginInterface)

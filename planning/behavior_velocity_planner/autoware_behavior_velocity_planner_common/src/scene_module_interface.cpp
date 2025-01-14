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

#include <autoware/behavior_velocity_planner_common/scene_module_interface.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>

#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

namespace autoware::behavior_velocity_planner
{

SceneModuleInterface::SceneModuleInterface(
  const int64_t module_id, rclcpp::Logger logger, rclcpp::Clock::SharedPtr clock)
: module_id_(module_id),
  logger_(logger),
  clock_(clock),
  time_keeper_(std::shared_ptr<universe_utils::TimeKeeper>())
{
}

size_t SceneModuleInterface::findEgoSegmentIndex(
  const std::vector<tier4_planning_msgs::msg::PathPointWithLaneId> & points) const
{
  const auto & p = planner_data_;
  return autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
    points, p->current_odometry->pose, p->ego_nearest_dist_threshold);
}

template SceneModuleManagerInterface<SceneModuleInterface>::SceneModuleManagerInterface(
  rclcpp::Node & node, [[maybe_unused]] const char * module_name);
template size_t SceneModuleManagerInterface<SceneModuleInterface>::findEgoSegmentIndex(
  const std::vector<tier4_planning_msgs::msg::PathPointWithLaneId> & points) const;
template void SceneModuleManagerInterface<SceneModuleInterface>::updateSceneModuleInstances(
  const std::shared_ptr<const PlannerData> & planner_data,
  const tier4_planning_msgs::msg::PathWithLaneId & path);
template void SceneModuleManagerInterface<SceneModuleInterface>::modifyPathVelocity(
  tier4_planning_msgs::msg::PathWithLaneId * path);
template void SceneModuleManagerInterface<SceneModuleInterface>::deleteExpiredModules(
  const tier4_planning_msgs::msg::PathWithLaneId & path);
template void SceneModuleManagerInterface<SceneModuleInterface>::registerModule(
  const std::shared_ptr<SceneModuleInterface> & scene_module);
}  // namespace autoware::behavior_velocity_planner

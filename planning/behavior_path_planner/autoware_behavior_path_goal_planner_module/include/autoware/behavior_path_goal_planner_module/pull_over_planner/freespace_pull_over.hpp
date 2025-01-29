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

#ifndef AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__PULL_OVER_PLANNER__FREESPACE_PULL_OVER_HPP_
#define AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__PULL_OVER_PLANNER__FREESPACE_PULL_OVER_HPP_

#include "autoware/behavior_path_goal_planner_module/pull_over_planner/pull_over_planner_base.hpp"

#include <autoware/freespace_planning_algorithms/abstract_algorithm.hpp>

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>

#include <memory>

namespace autoware::behavior_path_planner
{
using autoware::freespace_planning_algorithms::AbstractPlanningAlgorithm;

class FreespacePullOver : public PullOverPlannerBase
{
public:
  FreespacePullOver(rclcpp::Node & node, const GoalPlannerParameters & parameters);

  PullOverPlannerType getPlannerType() const override { return PullOverPlannerType::FREESPACE; }

  void setMap(const nav_msgs::msg::OccupancyGrid & costmap) { planner_->setMap(costmap); }

  std::optional<PullOverPath> plan(
    const GoalCandidate & modified_goal_pose, const size_t id,
    const std::shared_ptr<const PlannerData> planner_data,
    const BehaviorModuleOutput & upstream_module_output) override;

protected:
  const double velocity_;
  const bool left_side_parking_;
  const bool use_back_;
  std::unique_ptr<AbstractPlanningAlgorithm> planner_;
};
}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__PULL_OVER_PLANNER__FREESPACE_PULL_OVER_HPP_

// Copyright 2024 Tier IV, Inc.
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

#ifndef AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__THREAD_DATA_HPP_
#define AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__THREAD_DATA_HPP_

#include "autoware/behavior_path_goal_planner_module/decision_state.hpp"
#include "autoware/behavior_path_planner_common/interface/scene_module_interface.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace autoware::behavior_path_planner
{

class LaneParkingRequest
{
public:
  LaneParkingRequest(
    const autoware::universe_utils::LinearRing2d & vehicle_footprint,
    const GoalCandidates & goal_candidates, const BehaviorModuleOutput & upstream_module_output)
  : vehicle_footprint_(vehicle_footprint),
    goal_candidates_(goal_candidates),
    upstream_module_output_(upstream_module_output)
  {
  }

  void update(
    const PlannerData & planner_data, const ModuleStatus & current_status,
    const BehaviorModuleOutput & upstream_module_output,
    const std::optional<PullOverPath> & pull_over_path, const PathDecisionState & prev_data);

  const autoware::universe_utils::LinearRing2d vehicle_footprint_;
  const GoalCandidates goal_candidates_;

  const std::shared_ptr<PlannerData> & get_planner_data() const { return planner_data_; }
  const ModuleStatus & get_current_status() const { return current_status_; }
  const BehaviorModuleOutput & get_upstream_module_output() const
  {
    return upstream_module_output_;
  }
  const std::optional<PullOverPath> & get_pull_over_path() const { return pull_over_path_; }
  const PathDecisionState & get_prev_data() const { return prev_data_; }

private:
  std::shared_ptr<PlannerData> planner_data_;
  ModuleStatus current_status_;
  BehaviorModuleOutput upstream_module_output_;
  std::optional<PullOverPath> pull_over_path_;  //<! pull over path selected by main thread
  PathDecisionState prev_data_;
};

struct LaneParkingResponse
{
  std::vector<PullOverPath> pull_over_path_candidates;
  std::optional<Pose> closest_start_pose;
};

class FreespaceParkingRequest
{
public:
  FreespaceParkingRequest(
    const GoalPlannerParameters & parameters,
    const autoware::universe_utils::LinearRing2d & vehicle_footprint,
    const GoalCandidates & goal_candidates, const PlannerData & planner_data)
  : parameters_(parameters),
    vehicle_footprint_(vehicle_footprint),
    goal_candidates_(goal_candidates)
  {
    initializeOccupancyGridMap(planner_data, parameters_);
  };

  const ModuleStatus & getCurrentStatus() const { return current_status_; }
  void update(
    const PlannerData & planner_data, const ModuleStatus & current_status,
    const std::optional<PullOverPath> & pull_over_path,
    const std::optional<rclcpp::Time> & last_path_update_time, const bool is_stopped);

  const GoalPlannerParameters parameters_;
  const autoware::universe_utils::LinearRing2d vehicle_footprint_;
  const GoalCandidates goal_candidates_;

  const std::shared_ptr<PlannerData> & get_planner_data() const { return planner_data_; }
  const ModuleStatus & get_current_status() const { return current_status_; }
  std::shared_ptr<OccupancyGridBasedCollisionDetector> get_occupancy_grid_map() const
  {
    return occupancy_grid_map_;
  }
  const std::optional<PullOverPath> & get_pull_over_path() const { return pull_over_path_; }
  const std::optional<rclcpp::Time> & get_last_path_update_time() const
  {
    return last_path_update_time_;
  }
  bool is_stopped() const { return is_stopped_; }

private:
  std::shared_ptr<PlannerData> planner_data_;
  ModuleStatus current_status_;
  std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map_;
  std::optional<PullOverPath> pull_over_path_;
  std::optional<rclcpp::Time> last_path_update_time_;
  bool is_stopped_;

  void initializeOccupancyGridMap(
    const PlannerData & planner_data, const GoalPlannerParameters & parameters);
};

struct FreespaceParkingResponse
{
  std::optional<PullOverPath> freespace_pull_over_path;
};

}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__THREAD_DATA_HPP_

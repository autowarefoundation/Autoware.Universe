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

#include "autoware/behavior_path_goal_planner_module/util.hpp"

#include <autoware/behavior_path_goal_planner_module/decision_state.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>

#include <memory>
#include <vector>

namespace autoware::behavior_path_planner
{

using autoware::motion_utils::calcSignedArcLength;

void PathDecisionStateController::transit_state(
  const bool found_pull_over_path, const rclcpp::Time & now,
  const PredictedObjects & static_target_objects, const PredictedObjects & dynamic_target_objects,
  const std::optional<GoalCandidate> modified_goal_opt,
  const std::shared_ptr<const PlannerData> planner_data,
  const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map,
  const bool is_current_safe, const GoalPlannerParameters & parameters,
  const std::shared_ptr<GoalSearcherBase> goal_searcher, const bool is_activated,
  const std::optional<PullOverPath> & pull_over_path,
  std::vector<autoware::universe_utils::Polygon2d> & ego_polygons_expanded)
{
  const auto next_state = get_next_state(
    found_pull_over_path, now, static_target_objects, dynamic_target_objects, modified_goal_opt,
    planner_data, occupancy_grid_map, is_current_safe, parameters, goal_searcher, is_activated,
    pull_over_path, ego_polygons_expanded);
  current_state_ = next_state;
}

PathDecisionState PathDecisionStateController::get_next_state(
  const bool found_pull_over_path, const rclcpp::Time & now,
  const PredictedObjects & static_target_objects, const PredictedObjects & dynamic_target_objects,
  const std::optional<GoalCandidate> modified_goal_opt,
  const std::shared_ptr<const PlannerData> planner_data,
  const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map,
  const bool is_current_safe, const GoalPlannerParameters & parameters,
  const std::shared_ptr<GoalSearcherBase> goal_searcher, const bool is_activated,
  const std::optional<PullOverPath> & pull_over_path_opt,
  std::vector<autoware::universe_utils::Polygon2d> & ego_polygons_expanded) const
{
  auto next_state = current_state_;

  // update safety
  if (!parameters.safety_check_params.enable_safety_check) {
    next_state.is_stable_safe = true;
  } else {
    if (is_current_safe) {
      if (!next_state.safe_start_time) {
        next_state.safe_start_time = now;
        next_state.is_stable_safe = false;
      } else {
        next_state.is_stable_safe =
          ((now - next_state.safe_start_time.value()).seconds() >
           parameters.safety_check_params.keep_unsafe_time);
      }
    } else {
      next_state.safe_start_time = std::nullopt;
      next_state.is_stable_safe = false;
    }
  }

  // Once this function returns true, it will continue to return true thereafter
  if (next_state.state == PathDecisionState::DecisionKind::DECIDED) {
    return next_state;
  }

  // if path is not safe, not decided
  if (!found_pull_over_path || !pull_over_path_opt) {
    next_state.state = PathDecisionState::DecisionKind::NOT_DECIDED;
    return next_state;
  }

  const auto & pull_over_path = pull_over_path_opt.value();
  const bool enable_safety_check = parameters.safety_check_params.enable_safety_check;
  // If it is dangerous against dynamic objects before approval, do not determine the path.
  // This eliminates a unsafe path to be approved
  if (enable_safety_check && !next_state.is_stable_safe && !is_activated) {
    RCLCPP_DEBUG(
      logger_,
      "[DecidingPathStatus]: NOT_DECIDED. path is not safe against dynamic objects before "
      "approval");
    next_state.state = PathDecisionState::DecisionKind::NOT_DECIDED;
    return next_state;
  }

  const auto & current_path = pull_over_path.getCurrentPath();
  if (current_state_.state == PathDecisionState::DecisionKind::DECIDING) {
    const double hysteresis_factor = 0.9;

    // check goal pose collision
    if (
      modified_goal_opt && !goal_searcher->isSafeGoalWithMarginScaleFactor(
                             modified_goal_opt.value(), hysteresis_factor, occupancy_grid_map,
                             planner_data, static_target_objects)) {
      RCLCPP_DEBUG(logger_, "[DecidingPathStatus]: DECIDING->NOT_DECIDED. goal is not safe");
      next_state.state = PathDecisionState::DecisionKind::NOT_DECIDED;
      next_state.deciding_start_time = std::nullopt;
      return next_state;
    }

    // check current parking path collision
    const auto & parking_path = pull_over_path.parking_path();
    const auto & parking_path_curvatures = pull_over_path.parking_path_curvatures();
    const double margin =
      parameters.object_recognition_collision_check_hard_margins.back() * hysteresis_factor;
    if (goal_planner_utils::checkObjectsCollision(
          parking_path, parking_path_curvatures, static_target_objects, dynamic_target_objects,
          planner_data->parameters, margin,
          /*extract_static_objects=*/false, parameters.maximum_deceleration,
          parameters.object_recognition_collision_check_max_extra_stopping_margin,
          ego_polygons_expanded, true)) {
      RCLCPP_DEBUG(
        logger_, "[DecidingPathStatus]: DECIDING->NOT_DECIDED. path has collision with objects");
      next_state.state = PathDecisionState::DecisionKind::NOT_DECIDED;
      next_state.deciding_start_time = std::nullopt;
      return next_state;
    }

    if (enable_safety_check && !next_state.is_stable_safe) {
      RCLCPP_DEBUG(
        logger_,
        "[DecidingPathStatus]: DECIDING->NOT_DECIDED. path is not safe against dynamic objects");
      next_state.state = PathDecisionState::DecisionKind::NOT_DECIDED;
      return next_state;
    }

    // if enough time has passed since deciding status starts, transition to DECIDED
    constexpr double check_collision_duration = 1.0;
    const double elapsed_time_from_deciding =
      (now - current_state_.deciding_start_time.value()).seconds();
    if (elapsed_time_from_deciding > check_collision_duration) {
      RCLCPP_DEBUG(logger_, "[DecidingPathStatus]: DECIDING->DECIDED. has enough safe time passed");
      next_state.state = PathDecisionState::DecisionKind::DECIDED;
      next_state.deciding_start_time = std::nullopt;
      return next_state;
    }

    // if enough time has NOT passed since deciding status starts, keep DECIDING
    RCLCPP_DEBUG(
      logger_, "[DecidingPathStatus]: keep DECIDING. elapsed_time_from_deciding: %f",
      elapsed_time_from_deciding);
    return next_state;
  }

  // if ego is sufficiently close to the start of the nearest candidate path, the path is decided
  const auto & current_pose = planner_data->self_odometry->pose.pose;
  const size_t ego_segment_idx =
    autoware::motion_utils::findNearestSegmentIndex(current_path.points, current_pose.position);

  const size_t start_pose_segment_idx = autoware::motion_utils::findNearestSegmentIndex(
    current_path.points, pull_over_path.start_pose().position);
  const double dist_to_parking_start_pose = calcSignedArcLength(
    current_path.points, current_pose.position, ego_segment_idx,
    pull_over_path.start_pose().position, start_pose_segment_idx);
  if (dist_to_parking_start_pose > parameters.decide_path_distance) {
    next_state.state = PathDecisionState::DecisionKind::NOT_DECIDED;
    return next_state;
  }

  // if object recognition for path collision check is enabled, transition to DECIDING to check
  // collision for a certain period of time. Otherwise, transition to DECIDED directly.
  if (parameters.use_object_recognition) {
    RCLCPP_DEBUG(
      logger_,
      "[DecidingPathStatus]: NOT_DECIDED->DECIDING. start checking collision for certain "
      "period of time");
    next_state.state = PathDecisionState::DecisionKind::DECIDING;
    next_state.deciding_start_time = now;
    return next_state;
  }
  return {PathDecisionState::DecisionKind::DECIDED, std::nullopt};
}

}  // namespace autoware::behavior_path_planner

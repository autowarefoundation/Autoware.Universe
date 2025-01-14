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
#ifndef AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__PATH_HPP_
#define AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__PATH_HPP_

#include "autoware/behavior_path_lane_change_module/structs/data.hpp"
#include "autoware/behavior_path_lane_change_module/structs/path.hpp"

#include <autoware/behavior_path_planner_common/utils/utils.hpp>

#include <vector>

namespace autoware::behavior_path_planner::utils::lane_change
{
using behavior_path_planner::LaneChangePath;
using behavior_path_planner::lane_change::CommonDataPtr;
using behavior_path_planner::lane_change::TrajectoryGroup;

/**
 * @brief Generates a prepare segment for a lane change maneuver.
 *
 * This function generates the "prepare segment" of the path by trimming it to the specified length,
 * adjusting longitudinal velocity for acceleration or deceleration, and ensuring the starting point
 * meets necessary constraints for a lane change.
 *
 * @param common_data_ptr Shared pointer to CommonData containing current and target lane
 *                        information, vehicle parameters, and ego state.
 * @param prev_module_path The input path from the previous module, which will be used
 *                         as the base for the prepare segment.
 * @param prep_metric Preparation metrics containing desired length and velocity for the segment.
 * @param prepare_segment Output parameter where the resulting prepare segment path will be stored.
 *
 * @throws std::logic_error If the lane change start point is behind the target lanelet or
 *                          if lane information is invalid.
 *
 * @return true if the prepare segment is successfully generated, false otherwise.
 */
bool get_prepare_segment(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & prev_module_path,
  const double prep_length, PathWithLaneId & prepare_segment);

/**
 * @brief Generates the candidate path for a lane change maneuver.
 *
 * This function calculates the candidate lane change path based on the provided preparation
 * and lane change metrics. It resamples the target lane reference path, determines the start
 * and end poses for the lane change, and constructs the shift line required for the maneuver.
 * The function ensures that the resulting path satisfies length and distance constraints.
 *
 * @param common_data_ptr Shared pointer to CommonData containing route, lane, and transient data.
 * @param prep_metric Metrics for the preparation phase, including path length and velocity.
 * @param lc_metric Metrics for the lane change phase, including path length and velocity.
 * @param prep_segment The path segment prepared before initiating the lane change.
 * @param sorted_lane_ids A vector of sorted lane IDs used for updating lane information in the
 * path.
 * @param lc_start_pose The pose where the lane change begins.
 * @param shift_length The lateral distance to shift during the lane change maneuver.
 *
 * @throws std::logic_error If the target lane reference path is empty, candidate path generation
 * fails, or the resulting path length exceeds terminal constraints.
 *
 * @return LaneChangePath The constructed candidate lane change path.
 */
LaneChangePath get_candidate_path(
  const CommonDataPtr & common_data_ptr, const LaneChangePhaseMetrics & prep_metric,
  const LaneChangePhaseMetrics & lc_metric, const PathWithLaneId & prep_segment,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids, const double shift_length);

/**
 * @brief Constructs a candidate path for a lane change maneuver.
 *
 * This function generates a candidate lane change path by shifting the target lane's reference path
 * and combining it with the prepare segment. It verifies the path's validity by checking the yaw
 * difference, adjusting longitudinal velocity, and updating lane IDs based on the provided lane
 * sorting information.
 *
 * @param lane_change_info Information about the lane change, including shift line and target
 * velocity.
 * @param prepare_segment The path segment leading up to the lane change.
 * @param target_lane_reference_path The reference path of the target lane.
 * @param sorted_lane_ids A vector of sorted lane IDs used to update the candidate path's lane IDs.
 *
 * @return std::optional<LaneChangePath> The constructed candidate path if valid, or std::nullopt
 *                                       if the path fails any constraints.
 */
LaneChangePath construct_candidate_path(
  const LaneChangeInfo & lane_change_info, const PathWithLaneId & prepare_segment,
  const PathWithLaneId & target_lane_reference_path,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids);

/**
 * @brief Generates candidate trajectories in the Frenet frame for a lane change maneuver.
 *
 * This function computes a set of candidate trajectories for the ego vehicle in the Frenet frame,
 * based on the provided metrics, target lane reference path, and preparation segments. It ensures
 * that the generated trajectories adhere to specified constraints, such as lane boundaries and
 * velocity limits, while optimizing for smoothness and curvature.
 *
 * @param common_data_ptr Shared pointer to CommonData containing route, lane, and transient
 * information.
 * @param prev_module_path The previous module's path used as a base for preparation segments.
 * @param prep_metric Metrics for the preparation phase, including path length and velocity.
 *
 * @return std::vector<lane_change::TrajectoryGroup> A vector of trajectory groups representing
 * valid lane change candidates, each containing the prepare segment, target reference path, and
 * Frenet trajectory.
 */
std::vector<lane_change::TrajectoryGroup> generate_frenet_candidates(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & prev_module_path,
  const std::vector<LaneChangePhaseMetrics> & prep_metrics);

/**
 * @brief Constructs a lane change path candidate based on a Frenet trajectory group.
 *
 * This function generates a candidate lane change path by converting a Frenet trajectory group
 * into a shifted path and combining it with a prepare segment. It validates the path's feasibility
 * by ensuring yaw differences are within allowable thresholds and calculates lane change
 * information, such as acceleration, velocity, and duration.
 *
 * @param trajectory_group A Frenet trajectory group containing the prepared path and Frenet
 * trajectory data.
 * @param common_data_ptr Shared pointer to CommonData providing parameters and constraints for lane
 * changes.
 * @param sorted_lane_ids A vector of sorted lane IDs used to update the lane IDs in the path.
 *
 * @return std::optional<LaneChangePath> The constructed candidate lane change path if valid, or
 *         std::nullopt if the path is not feasible.
 *
 * @throws std::logic_error If the yaw difference exceeds the threshold, or other critical errors
 * occur.
 */
std::optional<LaneChangePath> get_candidate_path(
  const TrajectoryGroup & trajectory_group, const CommonDataPtr & common_data_ptr,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids);

/**
 * @brief Appends the target lane reference path to the candidate lane change path.
 *
 * This function extends the lane change candidate path by appending points from the
 * target lane reference path beyond the lane change end position. The appending process
 * is constrained by a curvature threshold to ensure smooth transitions and avoid sharp turns.
 *
 * @param frenet_candidate The candidate lane change path to which the target reference path is
 * appended. This includes the lane change path and associated Frenet trajectory data.
 * @param th_curvature_smoothing A threshold for the allowable curvature during the extension
 * process. Points with curvature exceeding this threshold are excluded.
 */
void append_target_ref_to_candidate(LaneChangePath & frenet_candidate, const double th_curvature);
}  // namespace autoware::behavior_path_planner::utils::lane_change

#endif  // AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__PATH_HPP_

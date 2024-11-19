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
#ifndef AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__SCENE_HPP_
#define AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__SCENE_HPP_

#include "autoware/behavior_path_lane_change_module/utils/base_class.hpp"
#include "autoware/behavior_path_lane_change_module/utils/data_structs.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace autoware::behavior_path_planner
{
using autoware::behavior_path_planner::utils::path_safety_checker::CollisionCheckDebug;
using autoware::behavior_path_planner::utils::path_safety_checker::CollisionCheckDebugMap;
using autoware::behavior_path_planner::utils::path_safety_checker::ExtendedPredictedObject;
using autoware::behavior_path_planner::utils::path_safety_checker::
  PoseWithVelocityAndPolygonStamped;
using autoware::behavior_path_planner::utils::path_safety_checker::PoseWithVelocityStamped;
using autoware::behavior_path_planner::utils::path_safety_checker::PredictedPathWithPolygon;
using autoware::route_handler::Direction;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;
using geometry_msgs::msg::Twist;
using lane_change::LanesPolygon;
using tier4_planning_msgs::msg::PathWithLaneId;
using utils::path_safety_checker::ExtendedPredictedObjects;
using utils::path_safety_checker::RSSparams;

class NormalLaneChange : public LaneChangeBase
{
public:
  NormalLaneChange(
    const std::shared_ptr<LaneChangeParameters> & parameters, LaneChangeModuleType type,
    Direction direction);

  NormalLaneChange(const NormalLaneChange &) = delete;
  NormalLaneChange(NormalLaneChange &&) = delete;
  NormalLaneChange & operator=(const NormalLaneChange &) = delete;
  NormalLaneChange & operator=(NormalLaneChange &&) = delete;
  ~NormalLaneChange() override = default;

  void update_lanes(const bool is_approved) final;

  void update_transient_data() final;

  void update_filtered_objects() final;

  void updateLaneChangeStatus() override;

  std::pair<bool, bool> getSafePath(LaneChangePath & safe_path) const override;

  LaneChangePath getLaneChangePath() const override;

  BehaviorModuleOutput getTerminalLaneChangePath() const override;

  BehaviorModuleOutput generateOutput() override;

  void extendOutputDrivableArea(BehaviorModuleOutput & output) const override;

  void insert_stop_point(const lanelet::ConstLanelets & lanelets, PathWithLaneId & path) override;

  void insert_stop_point_on_current_lanes(PathWithLaneId & path);

  PathWithLaneId getReferencePath() const override;

  std::optional<PathWithLaneId> extendPath() override;

  void resetParameters() override;

  TurnSignalInfo updateOutputTurnSignal() const override;

  bool calcAbortPath() override;

  PathSafetyStatus isApprovedPathSafe() const override;

  PathSafetyStatus evaluateApprovedPathWithUnsafeHysteresis(
    PathSafetyStatus approved_path_safety_status) override;

  bool isRequiredStop(const bool is_trailing_object) override;

  bool hasFinishedLaneChange() const override;

  bool isAbleToReturnCurrentLane() const override;

  bool is_near_terminal() const final;

  bool isEgoOnPreparePhase() const override;

  bool isAbleToStopSafely() const override;

  bool hasFinishedAbort() const override;

  bool isAbortState() const override;

  bool isLaneChangeRequired() override;

  bool isStoppedAtRedTrafficLight() const override;

  bool is_near_regulatory_element() const final;

  TurnSignalInfo get_current_turn_signal_info() const final;

protected:
  lanelet::ConstLanelets get_lane_change_lanes(const lanelet::ConstLanelets & current_lanes) const;

  int getNumToPreferredLane(const lanelet::ConstLanelet & lane) const override;

  TurnSignalInfo get_terminal_turn_signal_info() const final;

  lane_change::TargetObjects get_target_objects(
    const FilteredByLanesExtendedObjects & filtered_objects,
    const lanelet::ConstLanelets & current_lanes) const;

  FilteredByLanesExtendedObjects filterObjects() const;

  void filterOncomingObjects(PredictedObjects & objects) const;

  FilteredByLanesObjects filterObjectsByLanelets(
    const PredictedObjects & objects, const PathWithLaneId & current_lanes_ref_path) const;

  bool get_prepare_segment(
    PathWithLaneId & prepare_segment, const double prepare_length) const override;

  PathWithLaneId getTargetSegment(
    const lanelet::ConstLanelets & target_lanes, const Pose & lane_changing_start_pose,
    const double target_lane_length, const double lane_changing_length,
    const double lane_changing_velocity, const double buffer_for_next_lane_change) const;

  std::vector<LaneChangePhaseMetrics> get_prepare_metrics() const;
  std::vector<LaneChangePhaseMetrics> get_lane_changing_metrics(
    const PathWithLaneId & prep_segment, const LaneChangePhaseMetrics & prep_metrics,
    const double shift_length, const double dist_to_reg_element) const;

  bool get_lane_change_paths(LaneChangePaths & candidate_paths) const;

  LaneChangePath get_candidate_path(
    const LaneChangePhaseMetrics & prep_metrics, const LaneChangePhaseMetrics & lc_metrics,
    const PathWithLaneId & prep_segment, const std::vector<std::vector<int64_t>> & sorted_lane_ids,
    const Pose & lc_start_pose, const double shift_length) const;

  bool check_candidate_path_safety(
    const LaneChangePath & candidate_path, const lane_change::TargetObjects & target_objects) const;

  std::optional<LaneChangePath> calcTerminalLaneChangePath(
    const lanelet::ConstLanelets & current_lanes,
    const lanelet::ConstLanelets & target_lanes) const;

  bool isValidPath(const PathWithLaneId & path) const override;

  PathSafetyStatus isLaneChangePathSafe(
    const LaneChangePath & lane_change_path,
    const lane_change::TargetObjects & collision_check_objects,
    const utils::path_safety_checker::RSSparams & rss_params,
    const size_t deceleration_sampling_num, CollisionCheckDebugMap & debug_data) const;

  bool has_collision_with_decel_patterns(
    const LaneChangePath & lane_change_path, const ExtendedPredictedObjects & objects,
    const size_t deceleration_sampling_num, const RSSparams & rss_param,
    const bool check_prepare_phase, CollisionCheckDebugMap & debug_data) const;

  bool is_collided(
    const LaneChangePath & lane_change_path, const ExtendedPredictedObject & obj,
    const std::vector<PoseWithVelocityStamped> & ego_predicted_path,
    const RSSparams & selected_rss_param, const bool check_prepare_phase,
    CollisionCheckDebugMap & debug_data) const;

  double get_max_velocity_for_safety_check() const;

  bool is_ego_stuck() const;

  /**
   * @brief Checks if the given pose is a valid starting point for a lane change.
   *
   * This function determines whether the given pose (position) of the vehicle is within
   * the area of either the target neighbor lane or the target lane itself. It uses geometric
   * checks to see if the start point of the lane change is covered by the polygons representing
   * these lanes.
   *
   * @param common_data_ptr Shared pointer to a CommonData structure, which should include:
   *  - Non-null `lanes_polygon_ptr` that contains the polygon data for lanes.
   * @param pose The current pose of the vehicle
   *
   * @return `true` if the pose is within either the target neighbor lane or the target lane;
   * `false` otherwise.
   */
  bool is_valid_start_point(
    const lane_change::CommonDataPtr & common_data_ptr, const Pose & pose) const;

  bool check_prepare_phase() const;

  void set_stop_pose(const double arc_length_to_stop_pose, PathWithLaneId & path);

  void updateStopTime();

  double getStopTime() const { return stop_time_; }

  const lanelet::ConstLanelets & get_target_lanes() const
  {
    return common_data_ptr_->lanes_ptr->target;
  }

  void update_dist_from_intersection();

  std::vector<PathPointWithLaneId> path_after_intersection_;
  double stop_time_{0.0};
  static constexpr double floating_err_th{1e-3};
};
}  // namespace autoware::behavior_path_planner
#endif  // AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__SCENE_HPP_

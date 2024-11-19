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

#include "scene.hpp"

#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/utilization/arc_lane_util.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <tf2_eigen/tf2_eigen.hpp>

#include <lanelet2_core/geometry/Polygon.h>

#include <memory>
#include <utility>
#include <vector>

namespace autoware::behavior_velocity_planner
{
using autoware::motion_utils::calcLongitudinalOffsetPose;
using autoware::motion_utils::calcSignedArcLength;

DetectionAreaModule::DetectionAreaModule(
  const int64_t module_id, const int64_t lane_id,
  const lanelet::autoware::DetectionArea & detection_area_reg_elem,
  const PlannerParam & planner_param, const rclcpp::Logger & logger,
  const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  lane_id_(lane_id),
  detection_area_reg_elem_(detection_area_reg_elem),
  state_(State::GO),
  planner_param_(planner_param),
  debug_data_()
{
  velocity_factor_.init(PlanningBehavior::USER_DEFINED_DETECTION_AREA);
}

bool DetectionAreaModule::modifyPathVelocity(PathWithLaneId * path, StopReason * stop_reason)
{
  // Store original path
  const auto original_path = *path;

  // Reset data
  debug_data_ = DebugData();
  debug_data_.base_link2front = planner_data_->vehicle_info_.max_longitudinal_offset_m;
  *stop_reason = planning_utils::initializeStopReason(StopReason::DETECTION_AREA);

  // Find obstacles in detection area
  const auto obstacle_points = detection_area::get_obstacle_points(
    detection_area_reg_elem_.detectionAreas(), *planner_data_->no_ground_pointcloud);
  debug_data_.obstacle_points = obstacle_points;
  if (!obstacle_points.empty()) {
    last_obstacle_found_time_ = std::make_shared<const rclcpp::Time>(clock_->now());
  }

  // Get stop line geometry
  const auto stop_line = detection_area::get_stop_line_geometry2d(
    detection_area_reg_elem_, planner_data_->stop_line_extend_length);

  // Get self pose
  const auto & self_pose = planner_data_->current_odometry->pose;
  const size_t current_seg_idx = findEgoSegmentIndex(path->points);

  // Get stop point
  const auto stop_point = arc_lane_utils::createTargetPoint(
    original_path, stop_line, planner_param_.stop_margin,
    planner_data_->vehicle_info_.max_longitudinal_offset_m);
  if (!stop_point) {
    return true;
  }

  const auto & stop_point_idx = stop_point->first;
  const auto & stop_pose = stop_point->second;
  const size_t stop_line_seg_idx = planning_utils::calcSegmentIndexFromPointIndex(
    path->points, stop_pose.position, stop_point_idx);

  auto modified_stop_pose = stop_pose;
  size_t modified_stop_line_seg_idx = stop_line_seg_idx;

  const auto is_stopped = planner_data_->isVehicleStopped(0.0);
  const auto stop_dist = calcSignedArcLength(
    path->points, self_pose.position, current_seg_idx, stop_pose.position, stop_line_seg_idx);

  // Don't re-approach when the ego stops closer to the stop point than hold_stop_margin_distance
  if (is_stopped && stop_dist < planner_param_.hold_stop_margin_distance) {
    const auto ego_pos_on_path =
      calcLongitudinalOffsetPose(original_path.points, self_pose.position, 0.0);

    if (!ego_pos_on_path) {
      return false;
    }

    modified_stop_pose = ego_pos_on_path.value();
    modified_stop_line_seg_idx = current_seg_idx;
  }

  setDistance(stop_dist);

  // Check state
  setSafe(detection_area::can_clear_stop_state(
    last_obstacle_found_time_, clock_->now(), planner_param_.state_clear_time));
  if (isActivated()) {
    last_obstacle_found_time_ = {};
    if (!planner_param_.suppress_pass_judge_when_stopping || !is_stopped) {
      state_ = State::GO;
    }
    return true;
  }

  // Force ignore objects after dead_line
  if (planner_param_.use_dead_line) {
    // Use '-' for margin because it's the backward distance from stop line
    const auto dead_line_point = arc_lane_utils::createTargetPoint(
      original_path, stop_line, -planner_param_.dead_line_margin,
      planner_data_->vehicle_info_.max_longitudinal_offset_m);

    if (dead_line_point) {
      const size_t dead_line_point_idx = dead_line_point->first;
      const auto & dead_line_pose = dead_line_point->second;

      const size_t dead_line_seg_idx = planning_utils::calcSegmentIndexFromPointIndex(
        path->points, dead_line_pose.position, dead_line_point_idx);

      debug_data_.dead_line_poses.push_back(dead_line_pose);

      const double dist_from_ego_to_dead_line = calcSignedArcLength(
        original_path.points, self_pose.position, current_seg_idx, dead_line_pose.position,
        dead_line_seg_idx);
      if (dist_from_ego_to_dead_line < 0.0) {
        RCLCPP_WARN(logger_, "[detection_area] vehicle is over dead line");
        setSafe(true);
        return true;
      }
    }
  }

  // Ignore objects detected after stop_line if not in STOP state
  const double dist_from_ego_to_stop = calcSignedArcLength(
    original_path.points, self_pose.position, current_seg_idx, stop_pose.position,
    stop_line_seg_idx);
  if (
    state_ != State::STOP &&
    dist_from_ego_to_stop < -planner_param_.distance_to_judge_over_stop_line) {
    setSafe(true);
    return true;
  }

  // Ignore objects if braking distance is not enough
  if (planner_param_.use_pass_judge_line) {
    const auto current_velocity = planner_data_->current_velocity->twist.linear.x;
    const double pass_judge_line_distance = planning_utils::calcJudgeLineDistWithAccLimit(
      current_velocity, planner_data_->current_acceleration->accel.accel.linear.x,
      planner_data_->delay_response_time);
    if (
      state_ != State::STOP &&
      !detection_area::has_enough_braking_distance(
        self_pose, stop_point->second, pass_judge_line_distance, current_velocity)) {
      RCLCPP_WARN_THROTTLE(
        logger_, *clock_, std::chrono::milliseconds(1000).count(),
        "[detection_area] vehicle is over stop border");
      setSafe(true);
      return true;
    }
  }

  // Insert stop point
  state_ = State::STOP;
  planning_utils::insertStopPoint(modified_stop_pose.position, modified_stop_line_seg_idx, *path);

  // For virtual wall
  debug_data_.stop_poses.push_back(stop_point->second);

  // Create StopReason
  {
    StopFactor stop_factor{};
    stop_factor.stop_pose = stop_point->second;
    stop_factor.stop_factor_points = obstacle_points;
    planning_utils::appendStopReason(stop_factor, stop_reason);
    velocity_factor_.set(
      path->points, planner_data_->current_odometry->pose, stop_point->second,
      VelocityFactor::UNKNOWN);
  }

  // Create legacy StopReason
  {
    const double stop_path_point_distance = autoware::motion_utils::calcSignedArcLength(
      path->points, 0, stop_pose.position, stop_point->first);

    if (
      !first_stop_path_point_distance_ ||
      stop_path_point_distance < first_stop_path_point_distance_.value()) {
      debug_data_.first_stop_pose = stop_point->second;
      first_stop_path_point_distance_ = stop_path_point_distance;
    }
  }

  return true;
}
}  // namespace autoware::behavior_velocity_planner

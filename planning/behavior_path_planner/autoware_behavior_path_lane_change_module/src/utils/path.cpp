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

#include "autoware/behavior_path_lane_change_module/utils/path.hpp"

#include "autoware/behavior_path_lane_change_module/structs/data.hpp"
#include "autoware/behavior_path_lane_change_module/utils/utils.hpp"
#include "autoware/behavior_path_planner_common/utils/path_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"

#include <autoware/motion_utils/trajectory/path_shift.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view.hpp>

#include <algorithm>
#include <limits>
#include <vector>

namespace
{
using autoware::behavior_path_planner::LaneChangeInfo;
using autoware::behavior_path_planner::PathPointWithLaneId;
using autoware::behavior_path_planner::PathShifter;
using autoware::behavior_path_planner::PathWithLaneId;
using autoware::behavior_path_planner::ShiftedPath;
using autoware::behavior_path_planner::lane_change::CommonDataPtr;
using autoware::behavior_path_planner::lane_change::LCParamPtr;

using autoware::behavior_path_planner::LaneChangePhaseMetrics;
using autoware::behavior_path_planner::ShiftLine;
using geometry_msgs::msg::Pose;

double calc_resample_interval(
  const double lane_changing_length, const double lane_changing_velocity)
{
  constexpr auto min_resampling_points{30.0};
  constexpr auto resampling_dt{0.2};
  return std::max(
    lane_changing_length / min_resampling_points, lane_changing_velocity * resampling_dt);
}

PathWithLaneId get_reference_path_from_target_Lane(
  const CommonDataPtr & common_data_ptr, const Pose & lane_changing_start_pose,
  const double lane_changing_length, const double resample_interval)
{
  const auto & route_handler = *common_data_ptr->route_handler_ptr;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const auto target_lane_length = common_data_ptr->transient_data.target_lane_length;
  const auto is_goal_in_route = common_data_ptr->lanes_ptr->target_lane_in_goal_section;
  const auto next_lc_buffer = common_data_ptr->transient_data.next_dist_buffer.min;
  const auto forward_path_length = common_data_ptr->bpp_param_ptr->forward_path_length;

  const auto lane_change_start_arc_position =
    lanelet::utils::getArcCoordinates(target_lanes, lane_changing_start_pose);

  const double s_start = lane_change_start_arc_position.length;
  const double s_end = std::invoke([&]() {
    const auto dist_from_lc_start = s_start + lane_changing_length + forward_path_length;
    if (is_goal_in_route) {
      const double s_goal =
        lanelet::utils::getArcCoordinates(target_lanes, route_handler.getGoalPose()).length -
        next_lc_buffer;
      return std::min(dist_from_lc_start, s_goal);
    }
    return std::min(dist_from_lc_start, target_lane_length - next_lc_buffer);
  });

  constexpr double epsilon = 1e-4;
  if (s_end - s_start + epsilon < lane_changing_length) {
    return PathWithLaneId();
  }

  const auto lane_changing_reference_path =
    route_handler.getCenterLinePath(target_lanes, s_start, s_end);

  return autoware::behavior_path_planner::utils::resamplePathWithSpline(
    lane_changing_reference_path, resample_interval, true, {0.0, lane_changing_length});
}

ShiftLine get_lane_changing_shift_line(
  const Pose & lane_changing_start_pose, const Pose & lane_changing_end_pose,
  const PathWithLaneId & reference_path, const double shift_length)
{
  ShiftLine shift_line;
  shift_line.end_shift_length = shift_length;
  shift_line.start = lane_changing_start_pose;
  shift_line.end = lane_changing_end_pose;
  shift_line.start_idx = autoware::motion_utils::findNearestIndex(
    reference_path.points, lane_changing_start_pose.position);
  shift_line.end_idx = autoware::motion_utils::findNearestIndex(
    reference_path.points, lane_changing_end_pose.position);

  return shift_line;
}

ShiftedPath get_shifted_path(
  const PathWithLaneId & target_lane_reference_path, const LaneChangeInfo & lane_change_info)
{
  const auto longitudinal_acceleration = lane_change_info.longitudinal_acceleration;

  PathShifter path_shifter;
  path_shifter.setPath(target_lane_reference_path);
  path_shifter.addShiftLine(lane_change_info.shift_line);
  path_shifter.setLongitudinalAcceleration(longitudinal_acceleration.lane_changing);
  const auto initial_lane_changing_velocity = lane_change_info.velocity.lane_changing;
  path_shifter.setVelocity(initial_lane_changing_velocity);
  path_shifter.setLateralAccelerationLimit(std::abs(lane_change_info.lateral_acceleration));

  constexpr auto offset_back = false;
  ShiftedPath shifted_path;
  [[maybe_unused]] const auto success = path_shifter.generate(&shifted_path, offset_back);
  return shifted_path;
}

std::optional<double> exceed_yaw_threshold(
  const PathWithLaneId & prepare_segment, const PathWithLaneId & lane_changing_segment,
  const double yaw_th_rad)
{
  const auto & prepare = prepare_segment.points;
  const auto & lane_changing = lane_changing_segment.points;

  if (prepare.size() <= 1 || lane_changing.size() <= 1) {
    return std::nullopt;
  }

  const auto & p1 = std::prev(prepare.end() - 1)->point.pose;
  const auto & p2 = std::next(lane_changing.begin())->point.pose;
  const auto yaw_diff_rad = std::abs(autoware::universe_utils::normalizeRadian(
    tf2::getYaw(p1.orientation) - tf2::getYaw(p2.orientation)));
  if (yaw_diff_rad > yaw_th_rad) {
    return yaw_diff_rad;
  }
  return std::nullopt;
}
};  // namespace

namespace autoware::behavior_path_planner::utils::lane_change
{
using behavior_path_planner::lane_change::CommonDataPtr;

bool get_prepare_segment(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & prev_module_path,
  const LaneChangePhaseMetrics prep_metric, PathWithLaneId & prepare_segment)
{
  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const auto backward_path_length = common_data_ptr->bpp_param_ptr->backward_path_length;

  if (current_lanes.empty() || target_lanes.empty()) {
    throw std::logic_error("Empty lanes!");
  }

  prepare_segment = prev_module_path;
  const size_t current_seg_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      prepare_segment.points, common_data_ptr->get_ego_pose(), 3.0, 1.0);
  utils::clipPathLength(prepare_segment, current_seg_idx, prep_metric.length, backward_path_length);

  if (prepare_segment.points.empty()) return false;

  const auto & lc_start_pose = prepare_segment.points.back().point.pose;

  // TODO(Quda, Azu): Is it possible to remove these checks if we ensure prepare segment length is
  // larger than distance to target lane start
  if (!is_valid_start_point(common_data_ptr, lc_start_pose)) return false;

  // lane changing start is at the end of prepare segment
  const auto target_length_from_lane_change_start_pose =
    utils::getArcLengthToTargetLanelet(current_lanes, target_lanes.front(), lc_start_pose);

  // Check if the lane changing start point is not on the lanes next to target lanes,
  if (target_length_from_lane_change_start_pose > std::numeric_limits<double>::epsilon()) {
    throw std::logic_error("lane change start is behind target lanelet!");
  }

  return true;
}

LaneChangePath get_candidate_path(
  const CommonDataPtr & common_data_ptr, const LaneChangePhaseMetrics & prep_metric,
  const LaneChangePhaseMetrics & lc_metric, const PathWithLaneId & prep_segment,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids, const double shift_length)
{
  const auto & route_handler = *common_data_ptr->route_handler_ptr;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;

  const auto resample_interval = calc_resample_interval(lc_metric.length, prep_metric.velocity);

  if (prep_segment.points.empty()) {
    throw std::logic_error("Empty prepare segment!");
  }

  const auto & lc_start_pose = prep_segment.points.back().point.pose;
  const auto target_lane_reference_path = get_reference_path_from_target_Lane(
    common_data_ptr, lc_start_pose, lc_metric.length, resample_interval);

  if (target_lane_reference_path.points.empty()) {
    throw std::logic_error("Empty target reference!");
  }

  const auto lc_end_pose = std::invoke([&]() {
    const auto dist_to_lc_start =
      lanelet::utils::getArcCoordinates(target_lanes, lc_start_pose).length;
    const auto dist_to_lc_end = dist_to_lc_start + lc_metric.length;
    return route_handler.get_pose_from_2d_arc_length(target_lanes, dist_to_lc_end);
  });

  const auto shift_line = get_lane_changing_shift_line(
    lc_start_pose, lc_end_pose, target_lane_reference_path, shift_length);

  LaneChangeInfo lane_change_info{prep_metric, lc_metric, lc_start_pose, lc_end_pose, shift_line};

  if (
    lane_change_info.length.sum() + common_data_ptr->transient_data.next_dist_buffer.min >
    common_data_ptr->transient_data.dist_to_terminal_end) {
    throw std::logic_error("invalid candidate path length!");
  }

  return utils::lane_change::construct_candidate_path(
    lane_change_info, prep_segment, target_lane_reference_path, sorted_lane_ids);
}

LaneChangePath construct_candidate_path(
  const LaneChangeInfo & lane_change_info, const PathWithLaneId & prepare_segment,
  const PathWithLaneId & target_lane_reference_path,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids)
{
  const auto & shift_line = lane_change_info.shift_line;
  const auto terminal_lane_changing_velocity = lane_change_info.terminal_lane_changing_velocity;

  auto shifted_path = get_shifted_path(target_lane_reference_path, lane_change_info);

  if (shifted_path.path.points.empty()) {
    throw std::logic_error("Failed to generate shifted path.");
  }

  if (shifted_path.path.points.size() < shift_line.end_idx + 1) {
    throw std::logic_error("Path points are removed by PathShifter.");
  }

  const auto lc_end_idx_opt = autoware::motion_utils::findNearestIndex(
    shifted_path.path.points, lane_change_info.lane_changing_end);

  if (!lc_end_idx_opt) {
    throw std::logic_error("Lane change end idx not found on target path.");
  }

  for (size_t i = 0; i < shifted_path.path.points.size(); ++i) {
    auto & point = shifted_path.path.points.at(i);
    if (i < *lc_end_idx_opt) {
      point.lane_ids = replaceWithSortedIds(point.lane_ids, sorted_lane_ids);
      point.point.longitudinal_velocity_mps = std::min(
        point.point.longitudinal_velocity_mps, static_cast<float>(terminal_lane_changing_velocity));
      continue;
    }
    const auto nearest_idx =
      autoware::motion_utils::findNearestIndex(target_lane_reference_path.points, point.point.pose);
    point.lane_ids = target_lane_reference_path.points.at(*nearest_idx).lane_ids;
  }

  constexpr auto yaw_diff_th = autoware::universe_utils::deg2rad(5.0);
  if (
    const auto yaw_diff_opt =
      exceed_yaw_threshold(prepare_segment, shifted_path.path, yaw_diff_th)) {
    std::stringstream err_msg;
    err_msg << "Excessive yaw difference " << yaw_diff_opt.value() << " which exceeds the "
            << yaw_diff_th << " radian threshold.";
    throw std::logic_error(err_msg.str());
  }

  LaneChangePath candidate_path;
  candidate_path.path = utils::combinePath(prepare_segment, shifted_path.path);
  candidate_path.shifted_path = shifted_path;
  candidate_path.info = lane_change_info;

  return candidate_path;
}
}  // namespace autoware::behavior_path_planner::utils::lane_change

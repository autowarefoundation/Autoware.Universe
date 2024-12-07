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

#include "autoware/behavior_path_lane_change_module/scene.hpp"

#include "autoware/behavior_path_lane_change_module/utils/calculation.hpp"
#include "autoware/behavior_path_lane_change_module/utils/utils.hpp"
#include "autoware/behavior_path_planner_common/utils/drivable_area_expansion/static_drivable_area.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/objects_filtering.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/safety_check.hpp"
#include "autoware/behavior_path_planner_common/utils/path_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/traffic_light_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"

#include <autoware/motion_utils/trajectory/path_shift.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <range/v3/action/insert.hpp>
#include <range/v3/algorithm/any_of.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/sort.hpp>

#include <lanelet2_core/geometry/Point.h>
#include <lanelet2_core/geometry/Polygon.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::behavior_path_planner
{
using autoware::motion_utils::calcSignedArcLength;
using utils::lane_change::create_lanes_polygon;
namespace calculation = utils::lane_change::calculation;
using utils::path_safety_checker::filter::velocity_filter;

NormalLaneChange::NormalLaneChange(
  const std::shared_ptr<LaneChangeParameters> & parameters, LaneChangeModuleType type,
  Direction direction)
: LaneChangeBase(parameters, type, direction)
{
  stop_watch_.tic(getModuleTypeStr());
  stop_watch_.tic("stop_time");
}

void NormalLaneChange::update_lanes(const bool is_approved)
{
  if (is_approved) {
    return;
  }

  const auto current_lanes =
    utils::getCurrentLanesFromPath(prev_module_output_.path, planner_data_);

  if (current_lanes.empty()) {
    return;
  }

  const auto target_lanes = get_lane_change_lanes(current_lanes);
  if (target_lanes.empty()) {
    return;
  }

  lanelet::ConstLanelet current_lane;
  if (!common_data_ptr_->route_handler_ptr->getClosestLaneletWithinRoute(
        common_data_ptr_->get_ego_pose(), &current_lane)) {
    return;
  }

  common_data_ptr_->lanes_ptr->ego_lane = current_lane;
  const auto is_same_lanes_with_prev_iteration =
    utils::lane_change::is_same_lane_with_prev_iteration(
      common_data_ptr_, current_lanes, target_lanes);

  if (is_same_lanes_with_prev_iteration) {
    return;
  }

  common_data_ptr_->lanes_ptr->current = current_lanes;
  common_data_ptr_->lanes_ptr->target = target_lanes;

  const auto & route_handler_ptr = common_data_ptr_->route_handler_ptr;
  common_data_ptr_->current_lanes_path =
    route_handler_ptr->getCenterLinePath(current_lanes, 0.0, std::numeric_limits<double>::max());

  common_data_ptr_->lanes_ptr->target_neighbor = utils::lane_change::get_target_neighbor_lanes(
    *route_handler_ptr, current_lanes, common_data_ptr_->lc_type);

  common_data_ptr_->current_lanes_path =
    route_handler_ptr->getCenterLinePath(current_lanes, 0.0, std::numeric_limits<double>::max());
  common_data_ptr_->lanes_ptr->current_lane_in_goal_section =
    route_handler_ptr->isInGoalRouteSection(current_lanes.back());
  common_data_ptr_->lanes_ptr->target_lane_in_goal_section =
    route_handler_ptr->isInGoalRouteSection(target_lanes.back());

  common_data_ptr_->lanes_ptr->preceding_target = utils::getPrecedingLanelets(
    *route_handler_ptr, get_target_lanes(), common_data_ptr_->get_ego_pose(),
    common_data_ptr_->lc_param_ptr->backward_lane_length);

  lane_change_debug_.current_lanes = common_data_ptr_->lanes_ptr->current;
  lane_change_debug_.target_lanes = common_data_ptr_->lanes_ptr->target;

  lane_change_debug_.target_backward_lanes.clear();
  ranges::for_each(
    common_data_ptr_->lanes_ptr->preceding_target,
    [&](const lanelet::ConstLanelets & preceding_lanes) {
      ranges::insert(
        lane_change_debug_.target_backward_lanes, lane_change_debug_.target_backward_lanes.end(),
        preceding_lanes);
    });

  *common_data_ptr_->lanes_polygon_ptr = create_lanes_polygon(common_data_ptr_);
}

void NormalLaneChange::update_transient_data(const bool is_approved)
{
  if (
    !common_data_ptr_ || !common_data_ptr_->is_data_available() ||
    !common_data_ptr_->is_lanes_available()) {
    return;
  }

  auto & transient_data = common_data_ptr_->transient_data;

  const auto & p = *common_data_ptr_->bpp_param_ptr;
  const auto nearest_seg_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      prev_module_output_.path.points, common_data_ptr_->get_ego_pose(),
      p.ego_nearest_dist_threshold, p.ego_nearest_yaw_threshold);
  transient_data.current_path_velocity =
    prev_module_output_.path.points.at(nearest_seg_idx).point.longitudinal_velocity_mps;
  transient_data.current_path_seg_idx = nearest_seg_idx;

  const auto active_signal_duration =
    signal_activation_time_ ? (clock_.now() - signal_activation_time_.value()).seconds() : 0.0;
  transient_data.lane_change_prepare_duration =
    is_approved ? status_.lane_change_path.info.duration.prepare
                : calculation::calc_actual_prepare_duration(
                    common_data_ptr_, common_data_ptr_->get_ego_speed(), active_signal_duration);

  std::tie(transient_data.lane_changing_length, transient_data.current_dist_buffer) =
    calculation::calc_lc_length_and_dist_buffer(common_data_ptr_, get_current_lanes());

  transient_data.next_dist_buffer.min =
    transient_data.current_dist_buffer.min - transient_data.lane_changing_length.min -
    common_data_ptr_->lc_param_ptr->lane_change_finish_judge_buffer;

  transient_data.dist_to_terminal_end = calculation::calc_dist_from_pose_to_terminal_end(
    common_data_ptr_, common_data_ptr_->lanes_ptr->current, common_data_ptr_->get_ego_pose());
  transient_data.dist_to_terminal_start =
    transient_data.dist_to_terminal_end - transient_data.current_dist_buffer.min;

  transient_data.max_prepare_length = calculation::calc_maximum_prepare_length(common_data_ptr_);

  transient_data.target_lane_length =
    lanelet::utils::getLaneletLength2d(common_data_ptr_->lanes_ptr->target);

  transient_data.current_lanes_ego_arc = lanelet::utils::getArcCoordinates(
    common_data_ptr_->lanes_ptr->current, common_data_ptr_->get_ego_pose());

  transient_data.target_lanes_ego_arc = lanelet::utils::getArcCoordinates(
    common_data_ptr_->lanes_ptr->target, common_data_ptr_->get_ego_pose());

  transient_data.is_ego_near_current_terminal_start =
    transient_data.dist_to_terminal_start < transient_data.max_prepare_length;

  transient_data.current_footprint = utils::lane_change::get_ego_footprint(
    common_data_ptr_->get_ego_pose(), common_data_ptr_->bpp_param_ptr->vehicle_info);

  const auto & ego_lane = common_data_ptr_->lanes_ptr->ego_lane;
  const auto & route_handler_ptr = common_data_ptr_->route_handler_ptr;
  transient_data.in_intersection = utils::lane_change::is_within_intersection(
    route_handler_ptr, ego_lane, transient_data.current_footprint);
  transient_data.in_turn_direction_lane =
    utils::lane_change::is_within_turn_direction_lanes(ego_lane, transient_data.current_footprint);

  update_dist_from_intersection();

  updateStopTime();
  transient_data.is_ego_stuck = is_ego_stuck();

  RCLCPP_DEBUG(
    logger_, "lane_changing_length - min: %.4f, max: %.4f", transient_data.lane_changing_length.min,
    transient_data.lane_changing_length.max);
  RCLCPP_DEBUG(
    logger_, "current_dist_buffer - min: %.4f, max: %.4f", transient_data.current_dist_buffer.min,
    transient_data.current_dist_buffer.max);
  RCLCPP_DEBUG(
    logger_, "next_dist_buffer - min: %.4f, max: %.4f", transient_data.next_dist_buffer.min,
    transient_data.next_dist_buffer.max);
  RCLCPP_DEBUG(logger_, "dist_to_terminal_start: %.4f", transient_data.dist_to_terminal_start);
  RCLCPP_DEBUG(logger_, "dist_to_terminal_end: %.4f", transient_data.dist_to_terminal_end);
  RCLCPP_DEBUG(logger_, "max_prepare_length: %.4f", transient_data.max_prepare_length);
  RCLCPP_DEBUG(
    logger_, "is_ego_near_current_terminal_start: %s",
    (transient_data.is_ego_near_current_terminal_start ? "true" : "false"));
}

void NormalLaneChange::update_filtered_objects()
{
  filtered_objects_ = filter_objects();
}

void NormalLaneChange::updateLaneChangeStatus()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto [found_valid_path, found_safe_path] = getSafePath(status_.lane_change_path);

  // Update status
  status_.is_valid_path = found_valid_path;
  status_.is_safe = found_safe_path;

  status_.start_distance = common_data_ptr_->transient_data.target_lanes_ego_arc.length;
  status_.lane_change_path.path.header = getRouteHeader();
}

std::pair<bool, bool> NormalLaneChange::getSafePath(LaneChangePath & safe_path) const
{
  const auto & current_lanes = get_current_lanes();
  const auto & target_lanes = get_target_lanes();

  if (current_lanes.empty() || target_lanes.empty()) {
    return {false, false};
  }

  LaneChangePaths valid_paths{};
  bool found_safe_path = get_lane_change_paths(valid_paths);
  // if no safe path is found and ego is stuck, try to find a path with a small margin

  lane_change_debug_.valid_paths = valid_paths;

  if (valid_paths.empty()) {
    return {false, false};
  }

  if (found_safe_path) {
    safe_path = valid_paths.back();
  } else {
    // force candidate
    safe_path = valid_paths.front();
  }

  return {true, found_safe_path};
}

bool NormalLaneChange::isLaneChangeRequired()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (
    !common_data_ptr_ || !common_data_ptr_->is_data_available() ||
    !common_data_ptr_->is_lanes_available()) {
    return false;
  }

  const auto & current_lanes = common_data_ptr_->lanes_ptr->current;
  const auto & target_lanes = common_data_ptr_->lanes_ptr->target;

  const auto ego_dist_to_target_start =
    calculation::calc_ego_dist_to_lanes_start(common_data_ptr_, current_lanes, target_lanes);
  const auto max_prepare_length = calculation::calc_maximum_prepare_length(common_data_ptr_);

  if (ego_dist_to_target_start > max_prepare_length) {
    return false;
  }

  if (is_near_regulatory_element()) {
    RCLCPP_DEBUG(logger_, "Ego is close to regulatory element, don't run LC module");
    return false;
  }

  return true;
}

bool NormalLaneChange::is_near_regulatory_element() const
{
  if (!common_data_ptr_ || !common_data_ptr_->is_data_available()) return false;

  if (common_data_ptr_->transient_data.is_ego_near_current_terminal_start) return false;

  const bool only_tl = getStopTime() >= lane_change_parameters_->th_stop_time;

  if (only_tl) {
    RCLCPP_DEBUG(logger_, "Stop time is over threshold. Ignore crosswalk and intersection checks.");
  }

  return common_data_ptr_->transient_data.max_prepare_length >
         utils::lane_change::get_distance_to_next_regulatory_element(
           common_data_ptr_, only_tl, only_tl);
}

bool NormalLaneChange::isStoppedAtRedTrafficLight() const
{
  return utils::traffic_light::isStoppedAtRedTrafficLightWithinDistance(
    get_current_lanes(), status_.lane_change_path.path, planner_data_,
    status_.lane_change_path.info.length.sum());
}

TurnSignalInfo NormalLaneChange::get_current_turn_signal_info() const
{
  const auto original_turn_signal_info = prev_module_output_.turn_signal_info;

  if (getModuleType() != LaneChangeModuleType::NORMAL || get_current_lanes().empty()) {
    return original_turn_signal_info;
  }

  if (direction_ != Direction::LEFT && direction_ != Direction::RIGHT) {
    return original_turn_signal_info;
  }

  const auto & path = prev_module_output_.path;
  const auto & original_command = original_turn_signal_info.turn_signal.command;
  if (
    !path.points.empty() && original_command != TurnIndicatorsCommand::DISABLE &&
    original_command != TurnIndicatorsCommand::NO_COMMAND) {
    return get_terminal_turn_signal_info();
  }

  set_signal_activation_time();

  return get_turn_signal(getEgoPose(), getLaneChangePath().info.lane_changing_end);
}

TurnSignalInfo NormalLaneChange::get_terminal_turn_signal_info() const
{
  const auto & lane_change_param = getLaneChangeParam();
  const auto & common_param = getCommonParam();
  const auto & current_pose = getEgoPose();
  const auto & path = prev_module_output_.path;

  const auto original_turn_signal_info = prev_module_output_.turn_signal_info;

  const auto buffer = common_data_ptr_->transient_data.current_dist_buffer.min +
                      lane_change_param.min_length_for_turn_signal_activation +
                      common_param.base_link2front;
  const double path_length = autoware::motion_utils::calcArcLength(path.points);
  const auto start_pose = autoware::motion_utils::calcLongitudinalOffsetPose(
    path.points, 0, std::max(path_length - buffer, 0.0));

  if (!start_pose) return original_turn_signal_info;

  const auto terminal_turn_signal_info =
    get_turn_signal(*start_pose, getLaneChangePath().info.lane_changing_end);

  const auto nearest_dist_threshold = common_param.ego_nearest_dist_threshold;
  const auto nearest_yaw_threshold = common_param.ego_nearest_yaw_threshold;
  const auto current_nearest_seg_idx = common_data_ptr_->transient_data.current_path_seg_idx;

  const auto turn_signal_info = getTurnSignalDecider().overwrite_turn_signal(
    path, current_pose, current_nearest_seg_idx, original_turn_signal_info,
    terminal_turn_signal_info, nearest_dist_threshold, nearest_yaw_threshold);

  set_signal_activation_time(
    turn_signal_info.turn_signal.command != terminal_turn_signal_info.turn_signal.command);

  return turn_signal_info;
}

LaneChangePath NormalLaneChange::getLaneChangePath() const
{
  return status_.lane_change_path;
}

BehaviorModuleOutput NormalLaneChange::getTerminalLaneChangePath() const
{
  auto output = prev_module_output_;

  if (isAbortState() && abort_path_) {
    output.path = abort_path_->path;
    extendOutputDrivableArea(output);
    const auto current_seg_idx = planner_data_->findEgoSegmentIndex(output.path.points);
    output.turn_signal_info = planner_data_->turn_signal_decider.overwrite_turn_signal(
      output.path, getEgoPose(), current_seg_idx, prev_module_output_.turn_signal_info,
      output.turn_signal_info, planner_data_->parameters.ego_nearest_dist_threshold,
      planner_data_->parameters.ego_nearest_yaw_threshold);
    return output;
  }

  const auto & current_lanes = get_current_lanes();
  if (current_lanes.empty()) {
    RCLCPP_DEBUG(logger_, "Current lanes not found. Returning previous module's path as output.");
    return prev_module_output_;
  }

  const auto terminal_path =
    calcTerminalLaneChangePath(current_lanes, get_lane_change_lanes(current_lanes));
  if (!terminal_path) {
    RCLCPP_DEBUG(logger_, "Terminal path not found. Returning previous module's path as output.");
    return prev_module_output_;
  }

  output.path = terminal_path->path;
  output.turn_signal_info = updateOutputTurnSignal();

  extendOutputDrivableArea(output);

  const auto current_seg_idx = planner_data_->findEgoSegmentIndex(output.path.points);
  output.turn_signal_info = planner_data_->turn_signal_decider.overwrite_turn_signal(
    output.path, getEgoPose(), current_seg_idx, prev_module_output_.turn_signal_info,
    output.turn_signal_info, planner_data_->parameters.ego_nearest_dist_threshold,
    planner_data_->parameters.ego_nearest_yaw_threshold);

  return output;
}

BehaviorModuleOutput NormalLaneChange::generateOutput()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  if (!status_.is_valid_path) {
    RCLCPP_DEBUG(logger_, "No valid path found. Returning previous module's path as output.");
    insert_stop_point(get_current_lanes(), prev_module_output_.path);
    return prev_module_output_;
  }

  auto output = prev_module_output_;
  if (isAbortState() && abort_path_) {
    output.path = abort_path_->path;
    insert_stop_point(get_current_lanes(), output.path);
  } else {
    output.path = status_.lane_change_path.path;

    const auto found_extended_path = extendPath();
    if (found_extended_path) {
      output.path = utils::combinePath(output.path, *found_extended_path);
    }
    output.reference_path = getReferencePath();

    if (isStopState()) {
      const auto current_velocity = getEgoVelocity();
      const auto current_dist = calcSignedArcLength(
        output.path.points, output.path.points.front().point.pose.position, getEgoPosition());
      const auto stop_dist =
        -(current_velocity * current_velocity / (2.0 * planner_data_->parameters.min_acc));
      set_stop_pose(stop_dist + current_dist, output.path);
    } else {
      insert_stop_point(get_target_lanes(), output.path);
    }
  }

  extendOutputDrivableArea(output);

  const auto turn_signal_info =
    get_turn_signal(getEgoPose(), status_.lane_change_path.info.lane_changing_end);
  const auto current_seg_idx = planner_data_->findEgoSegmentIndex(output.path.points);
  output.turn_signal_info = planner_data_->turn_signal_decider.overwrite_turn_signal(
    output.path, getEgoPose(), current_seg_idx, prev_module_output_.turn_signal_info,
    turn_signal_info, planner_data_->parameters.ego_nearest_dist_threshold,
    planner_data_->parameters.ego_nearest_yaw_threshold);

  set_signal_activation_time(
    output.turn_signal_info.turn_signal.command != turn_signal_info.turn_signal.command);

  return output;
}

void NormalLaneChange::extendOutputDrivableArea(BehaviorModuleOutput & output) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto & dp = planner_data_->drivable_area_expansion_parameters;

  const auto drivable_lanes = utils::lane_change::generateDrivableLanes(
    *getRouteHandler(), get_current_lanes(), get_target_lanes());
  const auto shorten_lanes = utils::cutOverlappedLanes(output.path, drivable_lanes);
  const auto expanded_lanes = utils::expandLanelets(
    shorten_lanes, dp.drivable_area_left_bound_offset, dp.drivable_area_right_bound_offset,
    dp.drivable_area_types_to_skip);

  // for new architecture
  DrivableAreaInfo current_drivable_area_info;
  current_drivable_area_info.drivable_lanes = expanded_lanes;
  output.drivable_area_info = utils::combineDrivableAreaInfo(
    current_drivable_area_info, prev_module_output_.drivable_area_info);
}

void NormalLaneChange::insert_stop_point(
  const lanelet::ConstLanelets & lanelets, PathWithLaneId & path)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  if (lanelets.empty()) {
    return;
  }

  const auto & route_handler = getRouteHandler();

  if (route_handler->getNumLaneToPreferredLane(lanelets.back()) == 0) {
    return;
  }

  const auto & current_lanes = get_current_lanes();
  const auto is_current_lane = lanelets.front().id() == current_lanes.front().id() &&
                               lanelets.back().id() == current_lanes.back().id();

  // if input is not current lane, we can just insert the points at terminal.
  if (!is_current_lane) {
    const auto arc_length_to_stop_pose = motion_utils::calcArcLength(path.points) -
                                         common_data_ptr_->transient_data.next_dist_buffer.min;
    set_stop_pose(arc_length_to_stop_pose, path);
    return;
  }

  insert_stop_point_on_current_lanes(path);
}

void NormalLaneChange::insert_stop_point_on_current_lanes(PathWithLaneId & path)
{
  const auto & path_front_pose = path.points.front().point.pose;
  const auto & center_line = common_data_ptr_->current_lanes_path.points;
  const auto get_arc_length_along_lanelet = [&](const geometry_msgs::msg::Pose & target) {
    return motion_utils::calcSignedArcLength(
      center_line, path_front_pose.position, target.position);
  };

  const auto & transient_data = common_data_ptr_->transient_data;
  const auto & lanes_ptr = common_data_ptr_->lanes_ptr;
  const auto & lc_param_ptr = common_data_ptr_->lc_param_ptr;

  const auto dist_to_terminal = std::invoke([&]() -> double {
    const auto target_pose = (lanes_ptr->current_lane_in_goal_section)
                               ? common_data_ptr_->route_handler_ptr->getGoalPose()
                               : center_line.back().point.pose;
    return get_arc_length_along_lanelet(target_pose);
  });

  const auto & bpp_param_ptr = common_data_ptr_->bpp_param_ptr;
  const auto min_dist_buffer = transient_data.current_dist_buffer.min;
  const auto dist_to_terminal_start =
    dist_to_terminal - min_dist_buffer - calculation::calc_stopping_distance(lc_param_ptr);

  const auto distance_to_last_fit_width = std::invoke([&]() -> double {
    const auto & curr_lanes_poly = common_data_ptr_->lanes_polygon_ptr->current;
    if (utils::isEgoWithinOriginalLane(curr_lanes_poly, getEgoPose(), *bpp_param_ptr)) {
      return utils::lane_change::calculation::calc_dist_to_last_fit_width(
        lanes_ptr->current, path.points.front().point.pose, *bpp_param_ptr);
    }
    return std::numeric_limits<double>::max();
  });

  const auto dist_to_terminal_stop = std::min(dist_to_terminal_start, distance_to_last_fit_width);

  if (filtered_objects_.current_lane.empty()) {
    set_stop_pose(dist_to_terminal_stop, path);
    return;
  }

  const auto dist_to_target_lane_start = std::invoke([&]() -> double {
    const auto & front_lane = lanes_ptr->target_neighbor.front();
    const auto target_front =
      utils::to_geom_msg_pose(front_lane.centerline2d().front(), front_lane);
    return get_arc_length_along_lanelet(target_front);
  });

  const auto arc_length_to_current_obj = utils::lane_change::get_min_dist_to_current_lanes_obj(
    common_data_ptr_, filtered_objects_, dist_to_target_lane_start, path);

  // margin with leading vehicle
  // consider rss distance when the LC need to avoid obstacles
  const auto rss_dist = calcRssDistance(
    0.0, lc_param_ptr->trajectory.min_lane_changing_velocity,
    lc_param_ptr->safety.rss_params_for_parked);

  const auto stop_margin = transient_data.lane_changing_length.min +
                           lc_param_ptr->backward_length_buffer_for_blocking_object + rss_dist +
                           bpp_param_ptr->base_link2front;
  const auto stop_arc_length_behind_obj = arc_length_to_current_obj - stop_margin;

  if (stop_arc_length_behind_obj < dist_to_target_lane_start) {
    set_stop_pose(dist_to_target_lane_start, path);
    return;
  }

  if (stop_arc_length_behind_obj > dist_to_terminal_stop) {
    set_stop_pose(dist_to_terminal_stop, path);
    return;
  }

  //  If the target lane in the lane change section is blocked by a stationary obstacle, there
  //  is no reason for stopping with a lane change margin. Instead, stop right behind the
  //  obstacle.
  //  ----------------------------------------------------------
  //                            [obj]>
  //  ----------------------------------------------------------
  //    [ego]>          | <--- stop margin --->  [obj]>
  //  ----------------------------------------------------------
  const auto has_blocking_target_lane_obj = utils::lane_change::has_blocking_target_object(
    filtered_objects_.target_lane_leading, stop_arc_length_behind_obj, path);

  if (has_blocking_target_lane_obj || stop_arc_length_behind_obj <= 0.0) {
    set_stop_pose(dist_to_terminal_stop, path);
    return;
  }

  set_stop_pose(stop_arc_length_behind_obj, path);
}

PathWithLaneId NormalLaneChange::getReferencePath() const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  lanelet::ConstLanelet closest_lanelet;
  if (!lanelet::utils::query::getClosestLanelet(
        get_target_lanes(), getEgoPose(), &closest_lanelet)) {
    return prev_module_output_.reference_path;
  }
  auto reference_path = utils::getCenterLinePathFromLanelet(closest_lanelet, planner_data_);
  if (reference_path.points.empty()) {
    return prev_module_output_.reference_path;
  }
  return reference_path;
}

std::optional<PathWithLaneId> NormalLaneChange::extendPath()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto path = status_.lane_change_path.path;

  auto & target_lanes = common_data_ptr_->lanes_ptr->target;
  const auto & transient_data = common_data_ptr_->transient_data;

  const auto forward_path_length = getCommonParam().forward_path_length;

  if (
    (transient_data.target_lane_length - transient_data.target_lanes_ego_arc.length) >
    forward_path_length) {
    return std::nullopt;
  }
  const auto dist_to_end_of_path =
    lanelet::utils::getArcCoordinates(target_lanes, path.points.back().point.pose).length;

  if (common_data_ptr_->lanes_ptr->target_lane_in_goal_section) {
    const auto goal_pose = getRouteHandler()->getGoalPose();

    const auto dist_to_goal = lanelet::utils::getArcCoordinates(target_lanes, goal_pose).length;

    return getRouteHandler()->getCenterLinePath(target_lanes, dist_to_end_of_path, dist_to_goal);
  }

  lanelet::ConstLanelet next_lane;
  if (!getRouteHandler()->getNextLaneletWithinRoute(target_lanes.back(), &next_lane)) {
    return getRouteHandler()->getCenterLinePath(
      target_lanes, dist_to_end_of_path, transient_data.target_lane_length);
  }

  target_lanes.push_back(next_lane);

  const auto target_pose = std::invoke([&]() {
    const auto is_goal_in_next_lane = getRouteHandler()->isInGoalRouteSection(next_lane);
    if (is_goal_in_next_lane) {
      return getRouteHandler()->getGoalPose();
    }

    return utils::to_geom_msg_pose(next_lane.centerline2d().back(), next_lane);
  });

  const auto dist_to_target_pose =
    lanelet::utils::getArcCoordinates(target_lanes, target_pose).length;

  return getRouteHandler()->getCenterLinePath(
    target_lanes, dist_to_end_of_path, dist_to_target_pose);
}

void NormalLaneChange::resetParameters()
{
  is_abort_path_approved_ = false;
  is_abort_approval_requested_ = false;
  current_lane_change_state_ = LaneChangeStates::Normal;
  abort_path_ = nullptr;
  status_ = {};
  unsafe_hysteresis_count_ = 0;
  lane_change_debug_.reset();

  RCLCPP_DEBUG(logger_, "reset all flags and debug information.");
}

TurnSignalInfo NormalLaneChange::updateOutputTurnSignal() const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto & current_lanes = get_current_lanes();
  const auto & shift_line = status_.lane_change_path.info.shift_line;
  const auto & shift_path = status_.lane_change_path.shifted_path;
  const auto current_shift_length = common_data_ptr_->transient_data.current_lanes_ego_arc.distance;
  constexpr bool is_driving_forward = true;
  // The getBehaviorTurnSignalInfo method expects the shifted line to be generated off of the ego's
  // current lane, lane change is different, so we set this flag to false.
  constexpr bool egos_lane_is_shifted = false;
  constexpr bool is_pull_out = false;
  constexpr bool is_lane_change = true;

  const auto [new_signal, is_ignore] = planner_data_->getBehaviorTurnSignalInfo(
    shift_path, shift_line, current_lanes, current_shift_length, is_driving_forward,
    egos_lane_is_shifted, is_pull_out, is_lane_change);
  return new_signal;
}

lanelet::ConstLanelets NormalLaneChange::get_lane_change_lanes(
  const lanelet::ConstLanelets & current_lanes) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  if (current_lanes.empty()) {
    return {};
  }
  // Get lane change lanes
  const auto & route_handler = getRouteHandler();

  const auto lane_change_lane =
    utils::lane_change::get_lane_change_target_lane(common_data_ptr_, current_lanes);

  if (!lane_change_lane) {
    return {};
  }

  const auto forward_length = std::invoke([&]() {
    const auto front_pose =
      utils::to_geom_msg_pose(lane_change_lane->centerline().front(), *lane_change_lane);
    const auto signed_distance = utils::getSignedDistance(front_pose, getEgoPose(), current_lanes);
    const auto forward_path_length = planner_data_->parameters.forward_path_length;
    return forward_path_length + std::max(signed_distance, 0.0);
  });

  const auto backward_length = lane_change_parameters_->backward_lane_length;

  return route_handler->getLaneletSequence(
    lane_change_lane.value(), getEgoPose(), backward_length, forward_length);
}

bool NormalLaneChange::hasFinishedLaneChange() const
{
  const auto & current_pose = getEgoPose();
  const auto & lane_change_end = status_.lane_change_path.info.shift_line.end;
  const auto & target_lanes = get_target_lanes();
  const double dist_to_lane_change_end =
    utils::getSignedDistance(current_pose, lane_change_end, target_lanes);

  const auto finish_judge_buffer = std::invoke([&]() {
    const double ego_velocity = getEgoVelocity();
    // If ego velocity is low, relax finish judge buffer
    if (std::abs(ego_velocity) < 1.0) {
      return 0.0;
    }
    return lane_change_parameters_->lane_change_finish_judge_buffer;
  });

  const auto has_passed_end_pose = dist_to_lane_change_end + finish_judge_buffer < 0.0;

  lane_change_debug_.distance_to_lane_change_finished =
    dist_to_lane_change_end + finish_judge_buffer;

  if (has_passed_end_pose) {
    const auto & lanes_polygon = common_data_ptr_->lanes_polygon_ptr->target;
    return !boost::geometry::disjoint(
      lanes_polygon,
      lanelet::utils::to2D(lanelet::utils::conversion::toLaneletPoint(current_pose.position)));
  }

  const auto yaw_deviation_to_centerline =
    utils::lane_change::calc_angle_to_lanelet_segment(target_lanes, current_pose);

  if (yaw_deviation_to_centerline > lane_change_parameters_->th_finish_judge_yaw_diff) {
    return false;
  }

  const auto & arc_length = common_data_ptr_->transient_data.target_lanes_ego_arc;
  const auto reach_target_lane =
    std::abs(arc_length.distance) < lane_change_parameters_->th_finish_judge_lateral_diff;

  lane_change_debug_.distance_to_lane_change_finished = arc_length.distance;

  return reach_target_lane;
}

bool NormalLaneChange::isAbleToReturnCurrentLane() const
{
  if (status_.lane_change_path.path.points.size() < 2) {
    lane_change_debug_.is_able_to_return_to_current_lane = false;
    return false;
  }

  const auto & curr_lanes_poly = common_data_ptr_->lanes_polygon_ptr->current;
  if (!utils::isEgoWithinOriginalLane(
        curr_lanes_poly, getEgoPose(), planner_data_->parameters,
        lane_change_parameters_->cancel.overhang_tolerance)) {
    lane_change_debug_.is_able_to_return_to_current_lane = false;
    return false;
  }

  if (common_data_ptr_->transient_data.in_turn_direction_lane) {
    return true;
  }

  const auto nearest_idx = autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
    status_.lane_change_path.path.points, getEgoPose(),
    planner_data_->parameters.ego_nearest_dist_threshold,
    planner_data_->parameters.ego_nearest_yaw_threshold);

  const double ego_velocity =
    std::max(getEgoVelocity(), lane_change_parameters_->trajectory.min_lane_changing_velocity);
  const double estimated_travel_dist = ego_velocity * lane_change_parameters_->cancel.delta_time;

  double dist = 0.0;
  for (size_t idx = nearest_idx; idx < status_.lane_change_path.path.points.size() - 1; ++idx) {
    dist += calcSignedArcLength(status_.lane_change_path.path.points, idx, idx + 1);
    if (dist > estimated_travel_dist) {
      const auto & estimated_pose = status_.lane_change_path.path.points.at(idx + 1).point.pose;
      auto is_ego_within_original_lane = utils::isEgoWithinOriginalLane(
        curr_lanes_poly, estimated_pose, planner_data_->parameters,
        lane_change_parameters_->cancel.overhang_tolerance);
      lane_change_debug_.is_able_to_return_to_current_lane = is_ego_within_original_lane;
      return is_ego_within_original_lane;
    }
  }

  lane_change_debug_.is_able_to_return_to_current_lane = true;
  return true;
}

bool NormalLaneChange::is_near_terminal() const
{
  if (!common_data_ptr_ || !common_data_ptr_->is_data_available()) {
    return true;
  }

  // TODO(Azu) fully change to transient data
  const auto & lc_param_ptr = common_data_ptr_->lc_param_ptr;
  const auto backward_buffer = calculation::calc_stopping_distance(lc_param_ptr);

  const auto current_min_dist_buffer = common_data_ptr_->transient_data.current_dist_buffer.min;
  const auto min_lc_dist_with_buffer = backward_buffer + current_min_dist_buffer;

  return common_data_ptr_->transient_data.dist_to_terminal_end < min_lc_dist_with_buffer;
}

bool NormalLaneChange::isEgoOnPreparePhase() const
{
  const auto & start_position = status_.lane_change_path.info.shift_line.start.position;
  const auto & path_points = status_.lane_change_path.path.points;
  return calcSignedArcLength(path_points, start_position, getEgoPosition()) < 0.0;
}

bool NormalLaneChange::isAbleToStopSafely() const
{
  if (status_.lane_change_path.path.points.size() < 2) {
    return false;
  }

  const auto nearest_idx = autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
    status_.lane_change_path.path.points, getEgoPose(),
    planner_data_->parameters.ego_nearest_dist_threshold,
    planner_data_->parameters.ego_nearest_yaw_threshold);

  const auto current_velocity = getEgoVelocity();
  const auto stop_dist =
    -(current_velocity * current_velocity / (2.0 * planner_data_->parameters.min_acc));

  const auto & curr_lanes_poly = common_data_ptr_->lanes_polygon_ptr->current;
  double dist = 0.0;
  for (size_t idx = nearest_idx; idx < status_.lane_change_path.path.points.size() - 1; ++idx) {
    dist += calcSignedArcLength(status_.lane_change_path.path.points, idx, idx + 1);
    if (dist > stop_dist) {
      const auto & estimated_pose = status_.lane_change_path.path.points.at(idx + 1).point.pose;
      return utils::isEgoWithinOriginalLane(
        curr_lanes_poly, estimated_pose, planner_data_->parameters);
    }
  }
  return true;
}

bool NormalLaneChange::hasFinishedAbort() const
{
  if (!abort_path_) {
    lane_change_debug_.is_abort = true;
    return true;
  }

  const auto distance_to_finish = calcSignedArcLength(
    abort_path_->path.points, getEgoPosition(), abort_path_->info.shift_line.end.position);
  lane_change_debug_.distance_to_abort_finished = distance_to_finish;

  const auto has_finished_abort = distance_to_finish < 0.0;
  lane_change_debug_.is_abort = has_finished_abort;

  return has_finished_abort;
}

bool NormalLaneChange::isAbortState() const
{
  if (!lane_change_parameters_->cancel.enable_on_lane_changing_phase) {
    return false;
  }

  if (current_lane_change_state_ != LaneChangeStates::Abort) {
    return false;
  }

  if (!abort_path_) {
    return false;
  }

  lane_change_debug_.is_abort = true;
  return true;
}

int NormalLaneChange::getNumToPreferredLane(const lanelet::ConstLanelet & lane) const
{
  const auto get_opposite_direction =
    (direction_ == Direction::RIGHT) ? Direction::LEFT : Direction::RIGHT;
  return std::abs(getRouteHandler()->getNumLaneToPreferredLane(lane, get_opposite_direction));
}

bool NormalLaneChange::get_prepare_segment(
  PathWithLaneId & prepare_segment, const double prepare_length) const
{
  const auto & current_lanes = common_data_ptr_->lanes_ptr->current;
  const auto & target_lanes = common_data_ptr_->lanes_ptr->target;
  const auto backward_path_length = common_data_ptr_->bpp_param_ptr->backward_path_length;

  if (current_lanes.empty() || target_lanes.empty()) {
    return false;
  }

  prepare_segment = prev_module_output_.path;
  const size_t current_seg_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      prepare_segment.points, getEgoPose(), 3.0, 1.0);
  utils::clipPathLength(prepare_segment, current_seg_idx, prepare_length, backward_path_length);

  if (prepare_segment.points.empty()) return false;

  const auto & lc_start_pose = prepare_segment.points.back().point.pose;

  // TODO(Quda, Azu): Is it possible to remove these checks if we ensure prepare segment length is
  // larger than distance to target lane start
  if (!is_valid_start_point(common_data_ptr_, lc_start_pose)) return false;

  // lane changing start is at the end of prepare segment
  const auto target_length_from_lane_change_start_pose =
    utils::getArcLengthToTargetLanelet(current_lanes, target_lanes.front(), lc_start_pose);

  // Check if the lane changing start point is not on the lanes next to target lanes,
  if (target_length_from_lane_change_start_pose > std::numeric_limits<double>::epsilon()) {
    throw std::logic_error("lane change start is behind target lanelet!");
  }

  return true;
}

lane_change::TargetObjects NormalLaneChange::get_target_objects(
  const FilteredLanesObjects & filtered_objects,
  [[maybe_unused]] const lanelet::ConstLanelets & current_lanes) const
{
  ExtendedPredictedObjects leading_objects = filtered_objects.target_lane_leading.moving;
  auto insert_leading_objects = [&](const auto & objects) {
    ranges::actions::insert(leading_objects, leading_objects.end(), objects);
  };

  insert_leading_objects(filtered_objects.target_lane_leading.stopped);
  insert_leading_objects(filtered_objects.target_lane_leading.stopped_at_bound);
  const auto chk_obj_in_curr_lanes =
    lane_change_parameters_->safety.collision_check.check_current_lane;
  if (chk_obj_in_curr_lanes || common_data_ptr_->transient_data.is_ego_stuck) {
    insert_leading_objects(filtered_objects.current_lane);
  }

  const auto chk_obj_in_other_lanes =
    lane_change_parameters_->safety.collision_check.check_other_lanes;
  if (chk_obj_in_other_lanes) {
    insert_leading_objects(filtered_objects.others);
  }

  return {leading_objects, filtered_objects.target_lane_trailing};
}

FilteredLanesObjects NormalLaneChange::filter_objects() const
{
  auto objects = *planner_data_->dynamic_object;
  utils::path_safety_checker::filterObjectsByClass(
    objects, lane_change_parameters_->safety.target_object_types);

  if (objects.objects.empty()) {
    return {};
  }

  filterOncomingObjects(objects);

  if (objects.objects.empty()) {
    return {};
  }

  if (!common_data_ptr_->is_lanes_available()) {
    return {};
  }

  const auto & current_pose = common_data_ptr_->get_ego_pose();
  const auto & current_lanes = common_data_ptr_->lanes_ptr->current;

  const auto & current_lanes_ref_path = common_data_ptr_->current_lanes_path;

  const auto & lanes_polygon = *common_data_ptr_->lanes_polygon_ptr;
  const auto dist_ego_to_current_lanes_center =
    lanelet::utils::getLateralDistanceToClosestLanelet(current_lanes, current_pose);

  FilteredLanesObjects filtered_objects;
  const auto reserve_size = objects.objects.size();
  filtered_objects.current_lane.reserve(reserve_size);
  auto & target_lane_leading = filtered_objects.target_lane_leading;
  target_lane_leading.stopped.reserve(reserve_size);
  target_lane_leading.moving.reserve(reserve_size);
  target_lane_leading.stopped_at_bound.reserve(reserve_size);
  filtered_objects.target_lane_trailing.reserve(reserve_size);
  filtered_objects.others.reserve(reserve_size);

  const auto stopped_obj_vel_th = lane_change_parameters_->safety.th_stopped_object_velocity;

  for (const auto & object : objects.objects) {
    auto ext_object = utils::lane_change::transform(object, *lane_change_parameters_);
    const auto & ext_obj_pose = ext_object.initial_pose;
    ext_object.dist_from_ego = autoware::motion_utils::calcSignedArcLength(
      current_lanes_ref_path.points, current_pose.position, ext_obj_pose.position);

    const auto is_before_terminal =
      utils::lane_change::is_before_terminal(common_data_ptr_, current_lanes_ref_path, ext_object);

    const auto ahead_of_ego =
      utils::lane_change::is_ahead_of_ego(common_data_ptr_, current_lanes_ref_path, ext_object);

    if (utils::lane_change::filter_target_lane_objects(
          common_data_ptr_, ext_object, dist_ego_to_current_lanes_center, ahead_of_ego,
          is_before_terminal, target_lane_leading, filtered_objects.target_lane_trailing)) {
      continue;
    }

    // TODO(Azu): We have to think about how to remove is_within_vel_th without breaking AW behavior
    const auto is_moving = velocity_filter(
      ext_object.initial_twist, stopped_obj_vel_th, std::numeric_limits<double>::max());

    if (
      ahead_of_ego && is_moving && is_before_terminal &&
      !boost::geometry::disjoint(ext_object.initial_polygon, lanes_polygon.current)) {
      // check only the objects that are in front of the ego vehicle
      filtered_objects.current_lane.push_back(ext_object);
      continue;
    }

    filtered_objects.others.push_back(ext_object);
  }

  const auto dist_comparator = [](const auto & obj1, const auto & obj2) {
    return obj1.dist_from_ego < obj2.dist_from_ego;
  };

  // There are no use cases for other lane objects yet, so to save some computation time, we dont
  // have to sort them.
  ranges::sort(filtered_objects.current_lane, dist_comparator);
  ranges::sort(target_lane_leading.stopped_at_bound, dist_comparator);
  ranges::sort(target_lane_leading.stopped, dist_comparator);
  ranges::sort(target_lane_leading.moving, dist_comparator);
  ranges::sort(filtered_objects.target_lane_trailing, [&](const auto & obj1, const auto & obj2) {
    return !dist_comparator(obj1, obj2);
  });

  lane_change_debug_.filtered_objects = filtered_objects;

  return filtered_objects;
}

void NormalLaneChange::filterOncomingObjects(PredictedObjects & objects) const
{
  const auto & current_pose = getEgoPose();

  const auto is_same_direction = [&](const PredictedObject & object) {
    const auto & object_pose = object.kinematics.initial_pose_with_covariance.pose;
    return !utils::path_safety_checker::isTargetObjectOncoming(current_pose, object_pose);
  };

  //  Perception noise could make stationary objects seem opposite the ego vehicle; check the
  //  velocity to prevent this.
  const auto is_stopped_object = [](const auto & object) -> bool {
    constexpr double min_vel_th = -0.5;
    constexpr double max_vel_th = 0.5;
    return velocity_filter(
      object.kinematics.initial_twist_with_covariance.twist, min_vel_th, max_vel_th);
  };

  utils::path_safety_checker::filterObjects(objects, [&](const PredictedObject & object) {
    const auto same_direction = is_same_direction(object);
    if (same_direction) {
      return true;
    }

    return is_stopped_object(object);
  });
}

PathWithLaneId NormalLaneChange::getTargetSegment(
  const lanelet::ConstLanelets & target_lanes, const Pose & lane_changing_start_pose,
  const double target_lane_length, const double lane_changing_length,
  const double lane_changing_velocity, const double buffer_for_next_lane_change) const
{
  const auto & route_handler = *getRouteHandler();
  const auto forward_path_length = planner_data_->parameters.forward_path_length;

  const double s_start = std::invoke([&lane_changing_start_pose, &target_lanes,
                                      &lane_changing_length, &target_lane_length,
                                      &buffer_for_next_lane_change]() {
    const auto arc_to_start_pose =
      lanelet::utils::getArcCoordinates(target_lanes, lane_changing_start_pose);
    const double dist_from_front_target_lanelet = arc_to_start_pose.length + lane_changing_length;
    const double end_of_lane_dist_without_buffer = target_lane_length - buffer_for_next_lane_change;
    return std::min(dist_from_front_target_lanelet, end_of_lane_dist_without_buffer);
  });

  const double s_end = std::invoke(
    [&s_start, &forward_path_length, &target_lane_length, &buffer_for_next_lane_change]() {
      const double dist_from_start = s_start + forward_path_length;
      const double dist_from_end = target_lane_length - buffer_for_next_lane_change;
      return std::max(
        std::min(dist_from_start, dist_from_end), s_start + std::numeric_limits<double>::epsilon());
    });

  PathWithLaneId target_segment = route_handler.getCenterLinePath(target_lanes, s_start, s_end);
  for (auto & point : target_segment.points) {
    point.point.longitudinal_velocity_mps =
      std::min(point.point.longitudinal_velocity_mps, static_cast<float>(lane_changing_velocity));
  }

  return target_segment;
}

std::vector<LaneChangePhaseMetrics> NormalLaneChange::get_prepare_metrics() const
{
  const auto & current_lanes = common_data_ptr_->lanes_ptr->current;
  const auto & target_lanes = common_data_ptr_->lanes_ptr->target;
  const auto current_velocity = getEgoVelocity();
  // set speed limit to be current path velocity;
  const auto max_path_velocity = common_data_ptr_->transient_data.current_path_velocity;

  const auto dist_to_target_start =
    calculation::calc_ego_dist_to_lanes_start(common_data_ptr_, current_lanes, target_lanes);
  return calculation::calc_prepare_phase_metrics(
    common_data_ptr_, current_velocity, max_path_velocity, dist_to_target_start,
    common_data_ptr_->transient_data.dist_to_terminal_start);
}

std::vector<LaneChangePhaseMetrics> NormalLaneChange::get_lane_changing_metrics(
  const PathWithLaneId & prep_segment, const LaneChangePhaseMetrics & prep_metric,
  const double shift_length, const double dist_to_reg_element) const
{
  const auto & transient_data = common_data_ptr_->transient_data;
  const auto dist_lc_start_to_end_of_lanes = calculation::calc_dist_from_pose_to_terminal_end(
    common_data_ptr_, common_data_ptr_->lanes_ptr->target_neighbor,
    prep_segment.points.back().point.pose);

  const auto max_lane_changing_length = std::invoke([&]() {
    double max_length =
      transient_data.is_ego_near_current_terminal_start
        ? transient_data.dist_to_terminal_end - prep_metric.length
        : std::min(transient_data.dist_to_terminal_end, dist_to_reg_element) - prep_metric.length;
    max_length =
      std::min(max_length, dist_lc_start_to_end_of_lanes - transient_data.next_dist_buffer.min);
    return max_length;
  });

  const auto max_path_velocity = prep_segment.points.back().point.longitudinal_velocity_mps;
  return calculation::calc_shift_phase_metrics(
    common_data_ptr_, shift_length, prep_metric.velocity, max_path_velocity,
    prep_metric.sampled_lon_accel, max_lane_changing_length);
}

bool NormalLaneChange::get_lane_change_paths(LaneChangePaths & candidate_paths) const
{
  lane_change_debug_.collision_check_objects.clear();

  if (!common_data_ptr_->is_lanes_available()) {
    RCLCPP_WARN(logger_, "lanes are not available. Not expected.");
    return false;
  }

  if (common_data_ptr_->lanes_polygon_ptr->target_neighbor.empty()) {
    RCLCPP_WARN(logger_, "target_lane_neighbors_polygon_2d is empty. Not expected.");
    return false;
  }

  const auto & current_lanes = get_current_lanes();
  const auto & target_lanes = get_target_lanes();

  const auto current_velocity = getEgoVelocity();
  const auto sorted_lane_ids = utils::lane_change::get_sorted_lane_ids(common_data_ptr_);
  const auto target_objects = get_target_objects(filtered_objects_, current_lanes);

  const auto prepare_phase_metrics = get_prepare_metrics();

  candidate_paths.reserve(
    prepare_phase_metrics.size() * lane_change_parameters_->trajectory.lat_acc_sampling_num);

  const bool only_tl = getStopTime() >= lane_change_parameters_->th_stop_time;
  const auto dist_to_next_regulatory_element =
    utils::lane_change::get_distance_to_next_regulatory_element(common_data_ptr_, only_tl, only_tl);

  auto check_length_diff =
    [&](const double prep_length, const double lc_length, const bool check_lc) {
      if (candidate_paths.empty()) return true;

      const auto prep_diff = std::abs(candidate_paths.back().info.length.prepare - prep_length);
      if (prep_diff > lane_change_parameters_->trajectory.th_prepare_length_diff) return true;

      if (!check_lc) return false;

      const auto lc_diff = std::abs(candidate_paths.back().info.length.lane_changing - lc_length);
      return lc_diff > lane_change_parameters_->trajectory.th_lane_changing_length_diff;
    };

  for (const auto & prep_metric : prepare_phase_metrics) {
    const auto debug_print = [&](const std::string & s) {
      RCLCPP_DEBUG(
        logger_, "%s | prep_time: %.5f | lon_acc: %.5f | prep_len: %.5f", s.c_str(),
        prep_metric.duration, prep_metric.actual_lon_accel, prep_metric.length);
    };

    if (!check_length_diff(prep_metric.length, 0.0, false)) {
      RCLCPP_DEBUG(logger_, "Skip: Change in prepare length is less than threshold.");
      continue;
    }

    PathWithLaneId prepare_segment;
    try {
      if (!get_prepare_segment(prepare_segment, prep_metric.length)) {
        debug_print("Reject: failed to get valid prepare segment!");
        continue;
      }
    } catch (const std::exception & e) {
      debug_print(e.what());
      break;
    }

    debug_print("Prepare path satisfy constraints");

    const auto & lane_changing_start_pose = prepare_segment.points.back().point.pose;

    const auto shift_length =
      lanelet::utils::getLateralDistanceToClosestLanelet(target_lanes, lane_changing_start_pose);

    const auto lane_changing_metrics = get_lane_changing_metrics(
      prepare_segment, prep_metric, shift_length, dist_to_next_regulatory_element);

    utils::lane_change::setPrepareVelocity(prepare_segment, current_velocity, prep_metric.velocity);

    for (const auto & lc_metric : lane_changing_metrics) {
      const auto debug_print_lat = [&](const std::string & s) {
        RCLCPP_DEBUG(
          logger_, "%s | lc_time: %.5f | lon_acc: %.5f | lat_acc: %.5f | lc_len: %.5f", s.c_str(),
          lc_metric.duration, lc_metric.actual_lon_accel, lc_metric.lat_accel, lc_metric.length);
      };

      if (!check_length_diff(prep_metric.length, lc_metric.length, true)) {
        RCLCPP_DEBUG(logger_, "Skip: Change in lane changing length is less than threshold.");
        continue;
      }

      LaneChangePath candidate_path;
      try {
        candidate_path = get_candidate_path(
          prep_metric, lc_metric, prepare_segment, sorted_lane_ids, lane_changing_start_pose,
          shift_length);
      } catch (const std::exception & e) {
        debug_print_lat(std::string("Reject: ") + e.what());
        continue;
      }

      candidate_paths.push_back(candidate_path);

      try {
        if (check_candidate_path_safety(candidate_path, target_objects)) {
          debug_print_lat("ACCEPT!!!: it is valid and safe!");
          return true;
        }
      } catch (const std::exception & e) {
        debug_print_lat(std::string("Reject: ") + e.what());
        return false;
      }

      debug_print_lat("Reject: sampled path is not safe.");
    }
  }

  RCLCPP_DEBUG(logger_, "No safety path found.");
  return false;
}

LaneChangePath NormalLaneChange::get_candidate_path(
  const LaneChangePhaseMetrics & prep_metrics, const LaneChangePhaseMetrics & lc_metrics,
  const PathWithLaneId & prep_segment, const std::vector<std::vector<int64_t>> & sorted_lane_ids,
  const Pose & lc_start_pose, const double shift_length) const
{
  const auto & route_handler = *getRouteHandler();
  const auto & target_lanes = common_data_ptr_->lanes_ptr->target;

  const auto resample_interval =
    utils::lane_change::calcLaneChangeResampleInterval(lc_metrics.length, prep_metrics.velocity);
  const auto target_lane_reference_path = utils::lane_change::get_reference_path_from_target_Lane(
    common_data_ptr_, lc_start_pose, lc_metrics.length, resample_interval);

  if (target_lane_reference_path.points.empty()) {
    throw std::logic_error("target_lane_reference_path is empty!");
  }

  const auto lc_end_pose = std::invoke([&]() {
    const auto dist_to_lc_start =
      lanelet::utils::getArcCoordinates(target_lanes, lc_start_pose).length;
    const auto dist_to_lc_end = dist_to_lc_start + lc_metrics.length;
    return route_handler.get_pose_from_2d_arc_length(target_lanes, dist_to_lc_end);
  });

  const auto shift_line = utils::lane_change::get_lane_changing_shift_line(
    lc_start_pose, lc_end_pose, target_lane_reference_path, shift_length);

  LaneChangeInfo lane_change_info{prep_metrics, lc_metrics, lc_start_pose, lc_end_pose, shift_line};

  const auto candidate_path = utils::lane_change::construct_candidate_path(
    common_data_ptr_, lane_change_info, prep_segment, target_lane_reference_path, sorted_lane_ids);

  if (!candidate_path) {
    throw std::logic_error("failed to generate candidate path!");
  }

  if (
    candidate_path.value().info.length.sum() +
      common_data_ptr_->transient_data.next_dist_buffer.min >
    common_data_ptr_->transient_data.dist_to_terminal_end) {
    throw std::logic_error("invalid candidate path length!");
  }

  return *candidate_path;
}

bool NormalLaneChange::check_candidate_path_safety(
  const LaneChangePath & candidate_path, const lane_change::TargetObjects & target_objects) const
{
  const auto is_stuck = common_data_ptr_->transient_data.is_ego_stuck;
  if (utils::lane_change::has_overtaking_turn_lane_object(
        common_data_ptr_, filtered_objects_.target_lane_trailing)) {
    throw std::logic_error("Ego is nearby intersection, and there might be overtaking vehicle.");
  }

  if (
    !is_stuck && utils::lane_change::is_delay_lane_change(
                   common_data_ptr_, candidate_path, filtered_objects_.target_lane_leading.stopped,
                   lane_change_debug_.collision_check_objects)) {
    throw std::logic_error(
      "Ego is not stuck and parked vehicle exists in the target lane. Skip lane change.");
  }

  const auto lc_start_velocity = candidate_path.info.velocity.prepare;
  const auto min_lc_velocity = lane_change_parameters_->trajectory.min_lane_changing_velocity;
  constexpr double margin = 0.1;
  // path is unsafe if it exceeds target lane boundary with a high velocity
  if (
    lane_change_parameters_->safety.enable_target_lane_bound_check &&
    lc_start_velocity > min_lc_velocity + margin &&
    utils::lane_change::path_footprint_exceeds_target_lane_bound(
      common_data_ptr_, candidate_path.shifted_path.path, planner_data_->parameters.vehicle_info)) {
    throw std::logic_error("Path footprint exceeds target lane boundary. Skip lane change.");
  }

  constexpr size_t decel_sampling_num = 1;
  const auto safety_check_with_normal_rss = isLaneChangePathSafe(
    candidate_path, target_objects, common_data_ptr_->lc_param_ptr->safety.rss_params,
    decel_sampling_num, lane_change_debug_.collision_check_objects);

  if (!safety_check_with_normal_rss.is_safe && is_stuck) {
    const auto safety_check_with_stuck_rss = isLaneChangePathSafe(
      candidate_path, target_objects, common_data_ptr_->lc_param_ptr->safety.rss_params_for_stuck,
      decel_sampling_num, lane_change_debug_.collision_check_objects);
    return safety_check_with_stuck_rss.is_safe;
  }

  return safety_check_with_normal_rss.is_safe;
}

std::optional<LaneChangePath> NormalLaneChange::calcTerminalLaneChangePath(
  const lanelet::ConstLanelets & current_lanes, const lanelet::ConstLanelets & target_lanes) const
{
  const auto is_empty = [&](const auto & data, const auto & s) {
    if (!data.empty()) return false;
    RCLCPP_WARN(logger_, "%s is empty. Not expected.", s);
    return true;
  };

  const auto target_lane_neighbors_polygon_2d =
    common_data_ptr_->lanes_polygon_ptr->target_neighbor;
  if (
    is_empty(current_lanes, "current_lanes") || is_empty(target_lanes, "target_lanes") ||
    is_empty(target_lane_neighbors_polygon_2d, "target_lane_neighbors_polygon_2d")) {
    return {};
  }

  const auto & route_handler = *getRouteHandler();

  const auto minimum_lane_changing_velocity =
    lane_change_parameters_->trajectory.min_lane_changing_velocity;

  const auto is_goal_in_route = route_handler.isInGoalRouteSection(target_lanes.back());

  const auto current_min_dist_buffer = common_data_ptr_->transient_data.current_dist_buffer.min;
  const auto next_min_dist_buffer = common_data_ptr_->transient_data.next_dist_buffer.min;

  const auto sorted_lane_ids = utils::lane_change::get_sorted_lane_ids(common_data_ptr_);

  // lane changing start getEgoPose() is at the end of prepare segment
  const auto current_lane_terminal_point =
    lanelet::utils::conversion::toGeomMsgPt(current_lanes.back().centerline3d().back());

  double distance_to_terminal_from_goal = 0;
  if (is_goal_in_route) {
    distance_to_terminal_from_goal =
      utils::getDistanceToEndOfLane(route_handler.getGoalPose(), current_lanes);
  }

  const auto lane_changing_start_pose = autoware::motion_utils::calcLongitudinalOffsetPose(
    prev_module_output_.path.points, current_lane_terminal_point,
    -(current_min_dist_buffer + next_min_dist_buffer + distance_to_terminal_from_goal));

  if (!lane_changing_start_pose) {
    RCLCPP_DEBUG(logger_, "Reject: lane changing start pose not found!!!");
    return {};
  }

  const auto target_length_from_lane_change_start_pose = utils::getArcLengthToTargetLanelet(
    current_lanes, target_lanes.front(), lane_changing_start_pose.value());

  // Check if the lane changing start point is not on the lanes next to target lanes,
  if (target_length_from_lane_change_start_pose > 0.0) {
    RCLCPP_DEBUG(logger_, "lane change start getEgoPose() is behind target lanelet!");
    return {};
  }

  const auto shift_length = lanelet::utils::getLateralDistanceToClosestLanelet(
    target_lanes, lane_changing_start_pose.value());

  const auto [min_lateral_acc, max_lateral_acc] =
    lane_change_parameters_->trajectory.lat_acc_map.find(minimum_lane_changing_velocity);

  const auto lane_changing_time = autoware::motion_utils::calc_shift_time_from_jerk(
    shift_length, lane_change_parameters_->trajectory.lateral_jerk, max_lateral_acc);

  const auto target_lane_length = common_data_ptr_->transient_data.target_lane_length;
  const auto target_segment = getTargetSegment(
    target_lanes, lane_changing_start_pose.value(), target_lane_length, current_min_dist_buffer,
    minimum_lane_changing_velocity, next_min_dist_buffer);

  if (target_segment.points.empty()) {
    RCLCPP_DEBUG(logger_, "Reject: target segment is empty!! something wrong...");
    return {};
  }

  LaneChangeInfo lane_change_info;
  lane_change_info.longitudinal_acceleration = LaneChangePhaseInfo{0.0, 0.0};
  lane_change_info.duration = LaneChangePhaseInfo{0.0, lane_changing_time};
  lane_change_info.velocity =
    LaneChangePhaseInfo{minimum_lane_changing_velocity, minimum_lane_changing_velocity};
  lane_change_info.length = LaneChangePhaseInfo{0.0, current_min_dist_buffer};
  lane_change_info.lane_changing_start = lane_changing_start_pose.value();
  lane_change_info.lane_changing_end = target_segment.points.front().point.pose;
  lane_change_info.lateral_acceleration = max_lateral_acc;
  lane_change_info.terminal_lane_changing_velocity = minimum_lane_changing_velocity;

  if (!is_valid_start_point(common_data_ptr_, lane_changing_start_pose.value())) {
    RCLCPP_DEBUG(
      logger_,
      "Reject: lane changing points are not inside of the target preferred lanes or its "
      "neighbors");
    return {};
  }

  const auto resample_interval = utils::lane_change::calcLaneChangeResampleInterval(
    current_min_dist_buffer, minimum_lane_changing_velocity);
  const auto target_lane_reference_path = utils::lane_change::get_reference_path_from_target_Lane(
    common_data_ptr_, lane_changing_start_pose.value(), current_min_dist_buffer, resample_interval);

  if (target_lane_reference_path.points.empty()) {
    RCLCPP_DEBUG(logger_, "Reject: target_lane_reference_path is empty!!");
    return {};
  }

  lane_change_info.shift_line = utils::lane_change::get_lane_changing_shift_line(
    lane_changing_start_pose.value(), target_segment.points.front().point.pose,
    target_lane_reference_path, shift_length);

  auto reference_segment = prev_module_output_.path;
  const double length_to_lane_changing_start = autoware::motion_utils::calcSignedArcLength(
    reference_segment.points, reference_segment.points.front().point.pose.position,
    lane_changing_start_pose->position);
  utils::clipPathLength(reference_segment, 0, length_to_lane_changing_start, 0.0);
  // remove terminal points because utils::clipPathLength() calculates extra long path
  reference_segment.points.pop_back();
  reference_segment.points.back().point.longitudinal_velocity_mps =
    static_cast<float>(minimum_lane_changing_velocity);

  const auto terminal_lane_change_path = utils::lane_change::construct_candidate_path(
    common_data_ptr_, lane_change_info, reference_segment, target_lane_reference_path,
    sorted_lane_ids);

  return terminal_lane_change_path;
}

PathSafetyStatus NormalLaneChange::isApprovedPathSafe() const
{
  const auto & path = status_.lane_change_path;
  const auto & current_lanes = get_current_lanes();
  const auto & target_lanes = get_target_lanes();

  if (current_lanes.empty() || target_lanes.empty()) {
    return {true, true};
  }

  const auto target_objects = get_target_objects(filtered_objects_, current_lanes);

  CollisionCheckDebugMap debug_data;

  const auto has_overtaking_object = utils::lane_change::has_overtaking_turn_lane_object(
    common_data_ptr_, filtered_objects_.target_lane_trailing);

  if (has_overtaking_object) {
    return {false, true};
  }

  if (utils::lane_change::is_delay_lane_change(
        common_data_ptr_, path, filtered_objects_.target_lane_leading.stopped, debug_data)) {
    RCLCPP_DEBUG(logger_, "Lane change has been delayed.");
    return {false, false};
  }

  const auto safety_status = isLaneChangePathSafe(
    path, target_objects, lane_change_parameters_->safety.rss_params_for_abort,
    static_cast<size_t>(lane_change_parameters_->cancel.deceleration_sampling_num), debug_data);
  {
    // only for debug purpose
    lane_change_debug_.collision_check_objects.clear();
    lane_change_debug_.collision_check_object_debug_lifetime +=
      (stop_watch_.toc(getModuleTypeStr()) / 1000);
    if (lane_change_debug_.collision_check_object_debug_lifetime > 2.0) {
      stop_watch_.toc(getModuleTypeStr(), true);
      lane_change_debug_.collision_check_object_debug_lifetime = 0.0;
      lane_change_debug_.collision_check_objects_after_approval.clear();
    }

    if (!safety_status.is_safe) {
      lane_change_debug_.collision_check_objects_after_approval = debug_data;
    }
  }

  return safety_status;
}

PathSafetyStatus NormalLaneChange::evaluateApprovedPathWithUnsafeHysteresis(
  PathSafetyStatus approved_path_safety_status)
{
  if (!approved_path_safety_status.is_safe) {
    ++unsafe_hysteresis_count_;
    RCLCPP_DEBUG(
      logger_, "%s: Increasing hysteresis count to %d.", __func__, unsafe_hysteresis_count_);
  } else {
    if (unsafe_hysteresis_count_ > 0) {
      RCLCPP_DEBUG(logger_, "%s: Lane change is now SAFE. Resetting hysteresis count.", __func__);
    }
    unsafe_hysteresis_count_ = 0;
  }
  if (unsafe_hysteresis_count_ > lane_change_parameters_->cancel.th_unsafe_hysteresis) {
    RCLCPP_DEBUG(
      logger_, "%s: hysteresis count exceed threshold. lane change is now %s", __func__,
      (approved_path_safety_status.is_safe ? "safe" : "UNSAFE"));
    return approved_path_safety_status;
  }
  return {};
}

bool NormalLaneChange::isValidPath(const PathWithLaneId & path) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto & route_handler = planner_data_->route_handler;
  const auto & dp = planner_data_->drivable_area_expansion_parameters;

  // check lane departure
  const auto drivable_lanes = utils::lane_change::generateDrivableLanes(
    *route_handler, utils::extendLanes(route_handler, get_current_lanes()),
    utils::extendLanes(route_handler, get_target_lanes()));
  const auto expanded_lanes = utils::expandLanelets(
    drivable_lanes, dp.drivable_area_left_bound_offset, dp.drivable_area_right_bound_offset,
    dp.drivable_area_types_to_skip);

  const auto lanelets = utils::transformToLanelets(expanded_lanes);

  // check path points are in any lanelets
  for (const auto & point : path.points) {
    bool is_in_lanelet = false;
    for (const auto & lanelet : lanelets) {
      if (lanelet::utils::isInLanelet(point.point.pose, lanelet)) {
        is_in_lanelet = true;
        break;
      }
    }
    if (!is_in_lanelet) {
      return false;
    }
  }

  // check relative angle
  return utils::checkPathRelativeAngle(path, M_PI);
}

bool NormalLaneChange::isRequiredStop(const bool is_trailing_object)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  if (
    common_data_ptr_->transient_data.is_ego_near_current_terminal_start && isAbleToStopSafely() &&
    is_trailing_object) {
    current_lane_change_state_ = LaneChangeStates::Stop;
    return true;
  }
  current_lane_change_state_ = LaneChangeStates::Normal;
  return false;
}

bool NormalLaneChange::calcAbortPath()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto & route_handler = getRouteHandler();
  const auto & common_param = getCommonParam();
  const auto current_velocity =
    std::max(lane_change_parameters_->trajectory.min_lane_changing_velocity, getEgoVelocity());
  const auto current_pose = getEgoPose();
  const auto & selected_path = status_.lane_change_path;

  const auto ego_nearest_dist_threshold = common_param.ego_nearest_dist_threshold;
  const auto ego_nearest_yaw_threshold = common_param.ego_nearest_yaw_threshold;

  const auto current_min_dist_buffer = common_data_ptr_->transient_data.current_dist_buffer.min;

  const auto & lane_changing_path = selected_path.path;
  const auto & reference_lanelets = get_current_lanes();
  const auto lane_changing_end_pose_idx = std::invoke([&]() {
    constexpr double s_start = 0.0;
    const double s_end = std::max(
      lanelet::utils::getLaneletLength2d(reference_lanelets) - current_min_dist_buffer, 0.0);

    const auto ref = route_handler->getCenterLinePath(reference_lanelets, s_start, s_end);
    return autoware::motion_utils::findFirstNearestIndexWithSoftConstraints(
      lane_changing_path.points, ref.points.back().point.pose, ego_nearest_dist_threshold,
      ego_nearest_yaw_threshold);
  });

  const auto ego_pose_idx = autoware::motion_utils::findFirstNearestIndexWithSoftConstraints(
    lane_changing_path.points, current_pose, ego_nearest_dist_threshold, ego_nearest_yaw_threshold);

  const auto get_abort_idx_and_distance = [&](const double param_time) {
    if (ego_pose_idx > lane_changing_end_pose_idx) {
      return std::make_pair(ego_pose_idx, 0.0);
    }

    const auto desired_distance = current_velocity * param_time;
    const auto & points = lane_changing_path.points;

    for (size_t idx = ego_pose_idx; idx < lane_changing_end_pose_idx; ++idx) {
      const double distance =
        utils::getSignedDistance(current_pose, points.at(idx).point.pose, reference_lanelets);
      if (distance > desired_distance) {
        return std::make_pair(idx, distance);
      }
    }

    return std::make_pair(ego_pose_idx, 0.0);
  };

  const auto [abort_start_idx, abort_start_dist] =
    get_abort_idx_and_distance(lane_change_parameters_->cancel.delta_time);
  const auto [abort_return_idx, abort_return_dist] = get_abort_idx_and_distance(
    lane_change_parameters_->cancel.delta_time + lane_change_parameters_->cancel.duration);

  if (abort_start_idx >= abort_return_idx) {
    RCLCPP_ERROR(logger_, "abort start idx and return idx is equal. can't compute abort path.");
    return false;
  }

  const auto enough_abort_dist =
    abort_start_dist + abort_return_dist +
      calculation::calc_stopping_distance(common_data_ptr_->lc_param_ptr) <=
    common_data_ptr_->transient_data.dist_to_terminal_start;

  if (!enough_abort_dist) {
    RCLCPP_ERROR(logger_, "insufficient distance to abort.");
    return false;
  }

  const auto abort_start_pose = lane_changing_path.points.at(abort_start_idx).point.pose;
  const auto abort_return_pose = lane_changing_path.points.at(abort_return_idx).point.pose;
  const auto shift_length =
    lanelet::utils::getArcCoordinates(reference_lanelets, abort_return_pose).distance;

  ShiftLine shift_line;
  shift_line.start = abort_start_pose;
  shift_line.end = abort_return_pose;
  shift_line.end_shift_length = -shift_length;
  shift_line.start_idx = abort_start_idx;
  shift_line.end_idx = abort_return_idx;

  PathShifter path_shifter;
  path_shifter.setPath(lane_changing_path);
  path_shifter.addShiftLine(shift_line);
  const auto lateral_jerk = autoware::motion_utils::calc_jerk_from_lat_lon_distance(
    shift_line.end_shift_length, abort_start_dist, current_velocity);
  path_shifter.setVelocity(current_velocity);
  const auto lateral_acc_range =
    lane_change_parameters_->trajectory.lat_acc_map.find(current_velocity);
  const double & max_lateral_acc = lateral_acc_range.second;
  path_shifter.setLateralAccelerationLimit(max_lateral_acc);

  if (lateral_jerk > lane_change_parameters_->cancel.max_lateral_jerk) {
    RCLCPP_ERROR(logger_, "Aborting jerk is too strong. lateral_jerk = %f", lateral_jerk);
    return false;
  }

  ShiftedPath shifted_path;
  // offset front side
  if (!path_shifter.generate(&shifted_path)) {
    RCLCPP_ERROR(logger_, "failed to generate abort shifted path.");
  }

  auto reference_lane_segment = prev_module_output_.path;
  {
    // const auto terminal_path =
    //   calcTerminalLaneChangePath(reference_lanelets, get_target_lanes());
    // if (terminal_path) {
    //   reference_lane_segment = terminal_path->path;
    // }
    const auto return_pose = shifted_path.path.points.at(abort_return_idx).point.pose;
    const auto seg_idx = autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      reference_lane_segment.points, return_pose, common_param.ego_nearest_dist_threshold,
      common_param.ego_nearest_yaw_threshold);
    reference_lane_segment.points = autoware::motion_utils::cropPoints(
      reference_lane_segment.points, return_pose.position, seg_idx,
      common_param.forward_path_length, 0.0);
  }

  auto abort_path = selected_path;
  abort_path.shifted_path = shifted_path;
  abort_path.info.shift_line = shift_line;

  {
    PathWithLaneId aborting_path;
    aborting_path.points.insert(
      aborting_path.points.begin(), shifted_path.path.points.begin(),
      shifted_path.path.points.begin() + static_cast<int>(abort_return_idx));

    if (!reference_lane_segment.points.empty()) {
      abort_path.path = utils::combinePath(aborting_path, reference_lane_segment);
    } else {
      abort_path.path = aborting_path;
    }
  }

  abort_path_ = std::make_shared<LaneChangePath>(abort_path);
  return true;
}

PathSafetyStatus NormalLaneChange::isLaneChangePathSafe(
  const LaneChangePath & lane_change_path,
  const lane_change::TargetObjects & collision_check_objects,
  const utils::path_safety_checker::RSSparams & rss_params, const size_t deceleration_sampling_num,
  CollisionCheckDebugMap & debug_data) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  constexpr auto is_safe = true;
  constexpr auto is_object_behind_ego = true;

  if (collision_check_objects.leading.empty() && collision_check_objects.trailing.empty()) {
    RCLCPP_DEBUG(logger_, "There is nothing to check.");
    return {is_safe, !is_object_behind_ego};
  }

  const auto is_check_prepare_phase = check_prepare_phase();

  const auto all_decel_pattern_has_collision =
    [&](const utils::path_safety_checker::ExtendedPredictedObjects & objects) -> bool {
    return has_collision_with_decel_patterns(
      lane_change_path, objects, deceleration_sampling_num, rss_params, is_check_prepare_phase,
      debug_data);
  };

  if (all_decel_pattern_has_collision(collision_check_objects.trailing)) {
    return {!is_safe, is_object_behind_ego};
  }

  if (all_decel_pattern_has_collision(collision_check_objects.leading)) {
    return {!is_safe, !is_object_behind_ego};
  }

  return {is_safe, !is_object_behind_ego};
}

bool NormalLaneChange::has_collision_with_decel_patterns(
  const LaneChangePath & lane_change_path, const ExtendedPredictedObjects & objects,
  const size_t deceleration_sampling_num, const RSSparams & rss_param,
  const bool check_prepare_phase, CollisionCheckDebugMap & debug_data) const
{
  if (objects.empty()) {
    return false;
  }

  const auto & path = lane_change_path.path;

  if (path.points.empty()) {
    return false;
  }

  const auto bpp_param = *common_data_ptr_->bpp_param_ptr;
  const auto global_min_acc = bpp_param.min_acc;
  const auto lane_changing_acc = lane_change_path.info.longitudinal_acceleration.lane_changing;

  const auto min_acc = std::min(lane_changing_acc, global_min_acc);
  const auto sampling_num =
    std::abs(min_acc - lane_changing_acc) > floating_err_th ? deceleration_sampling_num : 1;
  const auto acc_resolution = (min_acc - lane_changing_acc) / static_cast<double>(sampling_num);

  std::vector<double> acceleration_values(sampling_num);
  std::iota(acceleration_values.begin(), acceleration_values.end(), 0);

  std::transform(
    acceleration_values.begin(), acceleration_values.end(), acceleration_values.begin(),
    [&](double n) { return lane_changing_acc + n * acc_resolution; });

  const auto stopped_obj_vel_th = lane_change_parameters_->safety.th_stopped_object_velocity;
  const auto all_collided = std::all_of(
    acceleration_values.begin(), acceleration_values.end(), [&](const auto acceleration) {
      const auto ego_predicted_path = utils::lane_change::convert_to_predicted_path(
        common_data_ptr_, lane_change_path, acceleration);

      return std::any_of(objects.begin(), objects.end(), [&](const auto & obj) {
        const auto selected_rss_param = (obj.initial_twist.linear.x <= stopped_obj_vel_th)
                                          ? lane_change_parameters_->safety.rss_params_for_parked
                                          : rss_param;
        return is_collided(
          lane_change_path, obj, ego_predicted_path, selected_rss_param, check_prepare_phase,
          debug_data);
      });
    });

  return all_collided;
}

bool NormalLaneChange::is_collided(
  const LaneChangePath & lane_change_path, const ExtendedPredictedObject & obj,
  const std::vector<PoseWithVelocityStamped> & ego_predicted_path,
  const RSSparams & selected_rss_param, const bool check_prepare_phase,
  CollisionCheckDebugMap & debug_data) const
{
  constexpr auto is_collided{true};

  if (lane_change_path.path.points.empty()) {
    return !is_collided;
  }

  if (ego_predicted_path.empty()) {
    return !is_collided;
  }

  const auto & lanes_polygon_ptr = common_data_ptr_->lanes_polygon_ptr;
  const auto & current_polygon = lanes_polygon_ptr->current;
  const auto & expanded_target_polygon = lanes_polygon_ptr->target;

  if (current_polygon.empty() || expanded_target_polygon.empty()) {
    return !is_collided;
  }

  constexpr auto is_safe{true};
  auto current_debug_data = utils::path_safety_checker::createObjectDebug(obj);
  constexpr auto collision_check_yaw_diff_threshold{M_PI};
  constexpr auto hysteresis_factor{1.0};
  const auto obj_predicted_paths = utils::path_safety_checker::getPredictedPathFromObj(
    obj, lane_change_parameters_->safety.collision_check.use_all_predicted_paths);
  const auto safety_check_max_vel = get_max_velocity_for_safety_check();
  const auto & bpp_param = *common_data_ptr_->bpp_param_ptr;

  const double velocity_threshold = lane_change_parameters_->safety.th_stopped_object_velocity;
  const double prepare_duration = lane_change_path.info.duration.prepare;
  const double start_time = check_prepare_phase ? 0.0 : prepare_duration;

  for (const auto & obj_path : obj_predicted_paths) {
    utils::path_safety_checker::PredictedPathWithPolygon predicted_obj_path;
    predicted_obj_path.confidence = obj_path.confidence;
    std::copy_if(
      obj_path.path.begin(), obj_path.path.end(), std::back_inserter(predicted_obj_path.path),
      [&](const auto & entry) {
        return !(
          entry.time < start_time ||
          (entry.time < prepare_duration && entry.velocity < velocity_threshold));
      });

    const auto collided_polygons = utils::path_safety_checker::get_collided_polygons(
      lane_change_path.path, ego_predicted_path, obj, predicted_obj_path, bpp_param.vehicle_info,
      selected_rss_param, hysteresis_factor, safety_check_max_vel,
      collision_check_yaw_diff_threshold, current_debug_data.second);

    if (collided_polygons.empty()) {
      utils::path_safety_checker::updateCollisionCheckDebugMap(
        debug_data, current_debug_data, is_safe);
      continue;
    }

    const auto collision_in_current_lanes =
      utils::lane_change::is_collided_polygons_in_lanelet(collided_polygons, current_polygon);
    const auto collision_in_target_lanes = utils::lane_change::is_collided_polygons_in_lanelet(
      collided_polygons, expanded_target_polygon);

    if (!collision_in_current_lanes && !collision_in_target_lanes) {
      utils::path_safety_checker::updateCollisionCheckDebugMap(
        debug_data, current_debug_data, is_safe);
      continue;
    }

    utils::path_safety_checker::updateCollisionCheckDebugMap(
      debug_data, current_debug_data, !is_safe);
    return is_collided;
  }
  utils::path_safety_checker::updateCollisionCheckDebugMap(debug_data, current_debug_data, is_safe);
  return !is_collided;
}

double NormalLaneChange::get_max_velocity_for_safety_check() const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto external_velocity_limit_ptr = planner_data_->external_limit_max_velocity;
  if (external_velocity_limit_ptr) {
    return std::min(
      static_cast<double>(external_velocity_limit_ptr->max_velocity), getCommonParam().max_vel);
  }

  return getCommonParam().max_vel;
}

bool NormalLaneChange::is_ego_stuck() const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto & lc_param_ptr = common_data_ptr_->lc_param_ptr;

  if (std::abs(common_data_ptr_->get_ego_speed()) > lc_param_ptr->th_stop_velocity) {
    RCLCPP_DEBUG(logger_, "Ego is still moving, not in stuck");
    return false;
  }

  // Ego is just stopped, not sure it is in stuck yet.
  if (getStopTime() < lc_param_ptr->th_stop_time) {
    RCLCPP_DEBUG(logger_, "Ego is just stopped, counting for stuck judge... (%f)", getStopTime());
    return false;
  }

  // Check if any stationary object exist in obstacle_check_distance
  const auto & current_lanes_path = common_data_ptr_->current_lanes_path;
  const auto & ego_pose = common_data_ptr_->get_ego_pose();
  const auto rss_dist = calcRssDistance(
    0.0, lc_param_ptr->trajectory.min_lane_changing_velocity, lc_param_ptr->safety.rss_params);

  // It is difficult to define the detection range. If it is too short, the stuck will not be
  // determined, even though you are stuck by an obstacle. If it is too long,
  // the ego will be judged to be stuck by a distant vehicle, even though the ego is only
  // stopped at a traffic light. Essentially, the calculation should be based on the information of
  // the stop reason, but this is outside the scope of one module. I keep it as a TODO.
  constexpr auto detection_distance_margin = 10.0;
  const auto obstacle_check_distance = common_data_ptr_->transient_data.lane_changing_length.max +
                                       rss_dist + common_data_ptr_->bpp_param_ptr->base_link2front +
                                       detection_distance_margin;
  const auto has_object_blocking = std::any_of(
    filtered_objects_.current_lane.begin(), filtered_objects_.current_lane.end(),
    [&](const auto & object) {
      // Note: it needs chattering prevention.
      if (
        std::abs(object.initial_twist.linear.x) >
        lc_param_ptr->safety.th_stopped_object_velocity) {  // check if stationary
        return false;
      }

      const auto ego_to_obj_dist =
        calcSignedArcLength(
          current_lanes_path.points, ego_pose.position, object.initial_pose.position) -
        obstacle_check_distance;
      return ego_to_obj_dist < 0.0;
    });

  lane_change_debug_.is_stuck = has_object_blocking;
  return has_object_blocking;
}

bool NormalLaneChange::is_valid_start_point(
  const lane_change::CommonDataPtr & common_data_ptr, const Pose & pose) const
{
  const lanelet::BasicPoint2d lc_start_point(pose.position.x, pose.position.y);

  const auto & target_neighbor_poly = common_data_ptr->lanes_polygon_ptr->target_neighbor;
  const auto & target_lane_poly = common_data_ptr_->lanes_polygon_ptr->target;

  return boost::geometry::covered_by(lc_start_point, target_neighbor_poly) ||
         boost::geometry::covered_by(lc_start_point, target_lane_poly);
}

void NormalLaneChange::set_stop_pose(const double arc_length_to_stop_pose, PathWithLaneId & path)
{
  const auto stop_point = utils::insertStopPoint(arc_length_to_stop_pose, path);
  lane_change_stop_pose_ = stop_point.point.pose;
}

void NormalLaneChange::updateStopTime()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);
  const auto current_vel = getEgoVelocity();

  if (std::abs(current_vel) > lane_change_parameters_->th_stop_velocity) {
    stop_time_ = 0.0;
  } else {
    const double duration = stop_watch_.toc("stop_time");
    // clip stop time
    if (stop_time_ + duration * 0.001 > lane_change_parameters_->th_stop_time) {
      constexpr double eps = 0.1;
      stop_time_ = lane_change_parameters_->th_stop_time + eps;
    } else {
      stop_time_ += duration * 0.001;
    }
  }

  stop_watch_.tic("stop_time");
}

bool NormalLaneChange::check_prepare_phase() const
{
  const auto & route_handler = getRouteHandler();

  const auto check_in_general_lanes =
    lane_change_parameters_->safety.collision_check.enable_for_prepare_phase_in_general_lanes;

  lanelet::ConstLanelet current_lane;
  if (!route_handler->getClosestLaneletWithinRoute(getEgoPose(), &current_lane)) {
    RCLCPP_DEBUG(
      logger_, "Unable to get current lane. Default to %s.",
      (check_in_general_lanes ? "true" : "false"));
    return check_in_general_lanes;
  }

  const auto check_in_intersection =
    lane_change_parameters_->safety.collision_check.enable_for_prepare_phase_in_intersection &&
    common_data_ptr_->transient_data.in_intersection;

  const auto check_in_turns =
    lane_change_parameters_->safety.collision_check.enable_for_prepare_phase_in_turns &&
    common_data_ptr_->transient_data.in_turn_direction_lane;

  return check_in_intersection || check_in_turns || check_in_general_lanes;
}

void NormalLaneChange::update_dist_from_intersection()
{
  auto & transient_data = common_data_ptr_->transient_data;
  const auto & route_handler_ptr = common_data_ptr_->route_handler_ptr;

  if (
    transient_data.in_intersection && transient_data.in_turn_direction_lane &&
    path_after_intersection_.empty()) {
    auto path_after_intersection = route_handler_ptr->getCenterLinePath(
      common_data_ptr_->lanes_ptr->target_neighbor, 0.0, std::numeric_limits<double>::max());
    path_after_intersection_ = std::move(path_after_intersection.points);
    transient_data.dist_from_prev_intersection = 0.0;
    return;
  }

  if (
    transient_data.in_intersection || transient_data.in_turn_direction_lane ||
    path_after_intersection_.empty()) {
    return;
  }

  const auto & path_points = path_after_intersection_;
  const auto & front_point = path_points.front().point.pose.position;
  const auto & ego_position = common_data_ptr_->get_ego_pose().position;
  transient_data.dist_from_prev_intersection =
    calcSignedArcLength(path_points, front_point, ego_position);

  if (
    transient_data.dist_from_prev_intersection >= 0.0 &&
    transient_data.dist_from_prev_intersection <=
      common_data_ptr_->lc_param_ptr->backward_length_from_intersection) {
    return;
  }

  path_after_intersection_.clear();
  transient_data.dist_from_prev_intersection = std::numeric_limits<double>::max();
}
}  // namespace autoware::behavior_path_planner

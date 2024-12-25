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

#include "autoware/behavior_velocity_blind_spot_module/scene.hpp"

#include "autoware/behavior_velocity_blind_spot_module/util.hpp"

#include <autoware/behavior_velocity_planner_common/utilization/boost_geometry_helper.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>

#include <boost/geometry/algorithms/intersects.hpp>

#include <lanelet2_core/geometry/Polygon.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace autoware::behavior_velocity_planner
{
namespace bg = boost::geometry;

BlindSpotModule::BlindSpotModule(
  const int64_t module_id, const int64_t lane_id, const TurnDirection turn_direction,
  const std::shared_ptr<const PlannerData> planner_data, const PlannerParam & planner_param,
  const rclcpp::Logger logger, const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  lane_id_(lane_id),
  planner_param_{planner_param},
  turn_direction_(turn_direction),
  is_over_pass_judge_line_(false)
{
  velocity_factor_.init(PlanningBehavior::REAR_CHECK);
  sibling_straight_lanelet_ = getSiblingStraightLanelet(
    planner_data->route_handler_->getLaneletMapPtr()->laneletLayer.get(lane_id_),
    planner_data->route_handler_->getRoutingGraphPtr());
}

void BlindSpotModule::initializeRTCStatus()
{
  setSafe(true);
  setDistance(std::numeric_limits<double>::lowest());
}

BlindSpotDecision BlindSpotModule::modifyPathVelocityDetail(PathWithLaneId * path)
{
  if (planner_param_.use_pass_judge_line && is_over_pass_judge_line_) {
    return OverPassJudge{"already over the pass judge line. no plan needed."};
  }
  const auto & input_path = *path;

  /* set stop-line and stop-judgement-line for base_link */
  const auto interpolated_path_info_opt =
    generateInterpolatedPathInfo(lane_id_, input_path, logger_);
  if (!interpolated_path_info_opt) {
    return InternalError{"Failed to interpolate path"};
  }
  const auto & interpolated_path_info = interpolated_path_info_opt.value();

  const auto stoplines_idx_opt = generateStopLine(interpolated_path_info, path);
  if (!stoplines_idx_opt) {
    return InternalError{"generateStopLine fail"};
  }

  const auto [default_stopline_idx, critical_stopline_idx] = stoplines_idx_opt.value();
  if (default_stopline_idx == 0) {
    return InternalError{"stop line or pass judge line is at path[0], ignore planning."};
  }

  /* calc closest index */
  const auto & current_pose = planner_data_->current_odometry->pose;
  const auto closest_idx = autoware::motion_utils::findFirstNearestIndexWithSoftConstraints(
    input_path.points, current_pose, planner_data_->ego_nearest_dist_threshold,
    planner_data_->ego_nearest_yaw_threshold);
  const auto stop_line_idx =
    closest_idx > default_stopline_idx ? critical_stopline_idx : default_stopline_idx;
  const auto stop_line_pose = planning_utils::getAheadPose(
    stop_line_idx, planner_data_->vehicle_info_.max_longitudinal_offset_m, input_path);

  const auto is_over_pass_judge = isOverPassJudge(input_path, stop_line_pose);
  if (is_over_pass_judge) {
    is_over_pass_judge_line_ = true;
    return is_over_pass_judge.value();
  }

  if (!blind_spot_lanelets_) {
    const auto blind_spot_lanelets = generateBlindSpotLanelets(
      planner_data_->route_handler_, turn_direction_, lane_id_, input_path,
      planner_param_.ignore_width_from_center_line, planner_param_.adjacent_extend_width,
      planner_param_.opposite_adjacent_extend_width);
    if (!blind_spot_lanelets.empty()) {
      blind_spot_lanelets_ = blind_spot_lanelets;
    }
  }
  if (!blind_spot_lanelets_) {
    return InternalError{"There are not blind_spot_lanelets"};
  }
  const auto & blind_spot_lanelets = blind_spot_lanelets_.value();

  const auto detection_area_opt = generateBlindSpotPolygons(
    input_path, closest_idx, blind_spot_lanelets, path->points.at(critical_stopline_idx).point.pose,
    planner_param_.backward_detection_length);
  if (!detection_area_opt) {
    return InternalError{"Failed to generate BlindSpotPolygons"};
  }
  const auto & detection_area = detection_area_opt.value();
  debug_data_.detection_area = detection_area;

  const auto ego_time_to_reach_stop_line = computeTimeToPassStopLine(
    blind_spot_lanelets, path->points.at(critical_stopline_idx).point.pose);
  /* calculate dynamic collision around detection area */
  const auto collision_obstacle = isCollisionDetected(
    blind_spot_lanelets, path->points.at(critical_stopline_idx).point.pose, detection_area,
    ego_time_to_reach_stop_line);
  state_machine_.setStateWithMarginTime(
    collision_obstacle.has_value() ? StateMachine::State::STOP : StateMachine::State::GO,
    logger_.get_child("state_machine"), *clock_);

  if (state_machine_.getState() == StateMachine::State::STOP) {
    return Unsafe{stop_line_idx, collision_obstacle};
  }

  return Safe{stop_line_idx};
}

// template-specification based visitor pattern
// https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct VisitorSwitch : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
VisitorSwitch(Ts...) -> VisitorSwitch<Ts...>;

void BlindSpotModule::setRTCStatus(
  const BlindSpotDecision & decision, const tier4_planning_msgs::msg::PathWithLaneId & path)
{
  std::visit(
    VisitorSwitch{[&](const auto & sub_decision) { setRTCStatusByDecision(sub_decision, path); }},
    decision);
}

void BlindSpotModule::reactRTCApproval(const BlindSpotDecision & decision, PathWithLaneId * path)
{
  std::visit(
    VisitorSwitch{
      [&](const auto & sub_decision) { reactRTCApprovalByDecision(sub_decision, path); }},
    decision);
}

bool BlindSpotModule::modifyPathVelocity(PathWithLaneId * path)
{
  debug_data_ = DebugData();

  initializeRTCStatus();
  const auto decision = modifyPathVelocityDetail(path);
  const auto & input_path = *path;
  setRTCStatus(decision, input_path);
  reactRTCApproval(decision, path);

  return true;
}

static std::optional<size_t> getFirstPointIntersectsLineByFootprint(
  const lanelet::ConstLineString2d & line, const InterpolatedPathInfo & interpolated_path_info,
  const autoware::universe_utils::LinearRing2d & footprint, const double vehicle_length)
{
  const auto & path_ip = interpolated_path_info.path;
  const auto [lane_start, lane_end] = interpolated_path_info.lane_id_interval.value();
  const size_t vehicle_length_idx = static_cast<size_t>(vehicle_length / interpolated_path_info.ds);
  const size_t start =
    static_cast<size_t>(std::max<int>(0, static_cast<int>(lane_start) - vehicle_length_idx));
  const auto line2d = line.basicLineString();
  for (auto i = start; i <= lane_end; ++i) {
    const auto & base_pose = path_ip.points.at(i).point.pose;
    const auto path_footprint = autoware::universe_utils::transformVector(
      footprint, autoware::universe_utils::pose2transform(base_pose));
    if (bg::intersects(path_footprint, line2d)) {
      return std::make_optional<size_t>(i);
    }
  }
  return std::nullopt;
}

static std::optional<size_t> getDuplicatedPointIdx(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const geometry_msgs::msg::Point & point)
{
  for (size_t i = 0; i < path.points.size(); i++) {
    const auto & p = path.points.at(i).point.pose.position;

    constexpr double min_dist = 0.001;
    if (autoware::universe_utils::calcDistance2d(p, point) < min_dist) {
      return i;
    }
  }

  return std::nullopt;
}

static std::optional<size_t> insertPointIndex(
  const geometry_msgs::msg::Pose & in_pose, tier4_planning_msgs::msg::PathWithLaneId * inout_path,
  const double ego_nearest_dist_threshold, const double ego_nearest_yaw_threshold)
{
  const auto duplicate_idx_opt = getDuplicatedPointIdx(*inout_path, in_pose.position);
  if (duplicate_idx_opt) {
    return duplicate_idx_opt.value();
  }

  const size_t closest_idx = autoware::motion_utils::findFirstNearestIndexWithSoftConstraints(
    inout_path->points, in_pose, ego_nearest_dist_threshold, ego_nearest_yaw_threshold);
  // vector.insert(i) inserts element on the left side of v[i]
  // the velocity need to be zero order hold(from prior point)
  size_t insert_idx = closest_idx;
  tier4_planning_msgs::msg::PathPointWithLaneId inserted_point = inout_path->points.at(closest_idx);
  if (planning_utils::isAheadOf(in_pose, inout_path->points.at(closest_idx).point.pose)) {
    ++insert_idx;
  } else {
    // copy with velocity from prior point
    const size_t prior_ind = closest_idx > 0 ? closest_idx - 1 : 0;
    inserted_point.point.longitudinal_velocity_mps =
      inout_path->points.at(prior_ind).point.longitudinal_velocity_mps;
  }
  inserted_point.point.pose = in_pose;

  auto it = inout_path->points.begin() + insert_idx;
  inout_path->points.insert(it, inserted_point);

  return insert_idx;
}

std::optional<std::pair<size_t, size_t>> BlindSpotModule::generateStopLine(
  const InterpolatedPathInfo & interpolated_path_info,
  tier4_planning_msgs::msg::PathWithLaneId * path) const
{
  // NOTE: this is optionally int for later subtraction
  const int margin_idx_dist =
    std::ceil(planner_param_.stop_line_margin / interpolated_path_info.ds);

  const auto & path_ip = interpolated_path_info.path;

  size_t stop_idx_default_ip = 0;
  size_t stop_idx_critical_ip = 0;
  if (sibling_straight_lanelet_) {
    const auto sibling_straight_lanelet = sibling_straight_lanelet_.value();
    const auto turn_boundary_line = turn_direction_ == TurnDirection::LEFT
                                      ? sibling_straight_lanelet.leftBound()
                                      : sibling_straight_lanelet.rightBound();
    const auto first_conflict_idx_ip_opt = getFirstPointIntersectsLineByFootprint(
      lanelet::utils::to2D(turn_boundary_line), interpolated_path_info,
      planner_data_->vehicle_info_.createFootprint(0.0, 0.0),
      planner_data_->vehicle_info_.max_longitudinal_offset_m);
    if (!first_conflict_idx_ip_opt) {
      return std::nullopt;
    }

    // NOTE: this is optionally int for later subtraction
    const auto first_conflict_idx_ip = static_cast<int>(first_conflict_idx_ip_opt.value());

    stop_idx_default_ip = static_cast<size_t>(std::max(first_conflict_idx_ip - margin_idx_dist, 0));
    stop_idx_critical_ip = static_cast<size_t>(first_conflict_idx_ip);
  } else {
    // the entry point of the assigned lane
    const auto & assigned_lanelet =
      planner_data_->route_handler_->getLaneletMapPtr()->laneletLayer.get(lane_id_);
    const auto left_pt = assigned_lanelet.leftBound().front().basicPoint();
    const auto right_pt = assigned_lanelet.rightBound().front().basicPoint();
    const auto mid_pt = (left_pt + right_pt) / 2.0;
    const geometry_msgs::msg::Point mid_point =
      geometry_msgs::build<geometry_msgs::msg::Point>().x(mid_pt.x()).y(mid_pt.y()).z(mid_pt.z());
    stop_idx_default_ip = stop_idx_critical_ip =
      autoware::motion_utils::findNearestSegmentIndex(path_ip.points, mid_point);
    /*
    // NOTE: it is not ambiguous when the curve starts on the left/right lanelet, so this module
    inserts stopline at the beginning of the lanelet for baselink
    stop_idx_default_ip = stop_idx_critical_ip = static_cast<size_t>(std::max<int>(0,
    static_cast<int>(autoware::motion_utils::findNearestSegmentIndex(path_ip.points, mid_point)) -
    baselink2front_dist));
    */
  }

  /* insert stop_point to use interpolated path*/
  const auto stopline_idx_default_opt = insertPointIndex(
    path_ip.points.at(stop_idx_default_ip).point.pose, path,
    planner_data_->ego_nearest_dist_threshold, planner_data_->ego_nearest_yaw_threshold);
  const auto stopline_idx_critical_opt = insertPointIndex(
    path_ip.points.at(stop_idx_critical_ip).point.pose, path,
    planner_data_->ego_nearest_dist_threshold, planner_data_->ego_nearest_yaw_threshold);

  if (!stopline_idx_default_opt || !stopline_idx_critical_opt) {
    return std::nullopt;
  }
  return std::make_pair(stopline_idx_default_opt.value(), stopline_idx_critical_opt.value());
}

autoware_perception_msgs::msg::PredictedObject BlindSpotModule::cutPredictPathWithDuration(
  const std_msgs::msg::Header & header,
  const autoware_perception_msgs::msg::PredictedObject & object_original,
  const double time_thr) const
{
  auto object = object_original;
  const rclcpp::Time current_time = clock_->now();

  for (auto & predicted_path : object.kinematics.predicted_paths) {  // each predicted paths
    const auto origin_path = predicted_path;
    predicted_path.path.clear();

    for (size_t k = 0; k < origin_path.path.size(); ++k) {  // each path points
      const auto & predicted_pose = origin_path.path.at(k);
      const auto predicted_time = rclcpp::Time(header.stamp) +
                                  rclcpp::Duration(origin_path.time_step) * static_cast<double>(k);
      if ((predicted_time - current_time).seconds() < time_thr) {
        predicted_path.path.push_back(predicted_pose);
      }
    }
  }
  return object;
}

std::optional<OverPassJudge> BlindSpotModule::isOverPassJudge(
  const tier4_planning_msgs::msg::PathWithLaneId & input_path,
  const geometry_msgs::msg::Pose & stop_point_pose) const
{
  const auto & current_pose = planner_data_->current_odometry->pose;

  /* set judge line dist */
  const double pass_judge_line_dist = planning_utils::calcJudgeLineDistWithAccLimit(
    planner_data_->current_velocity->twist.linear.x, planner_data_->max_stop_acceleration_threshold,
    planner_data_->delay_response_time);
  const auto ego_segment_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      input_path.points, current_pose, planner_data_->ego_nearest_dist_threshold,
      planner_data_->ego_nearest_yaw_threshold);
  const size_t stop_point_segment_idx =
    autoware::motion_utils::findNearestSegmentIndex(input_path.points, stop_point_pose.position);
  const auto distance_until_stop = autoware::motion_utils::calcSignedArcLength(
    input_path.points, current_pose.position, ego_segment_idx, stop_point_pose.position,
    stop_point_segment_idx);

  /* if current_state = GO, and current_pose is over judge_line, ignore planning. */
  if (planner_param_.use_pass_judge_line) {
    const double eps = 1e-1;  // to prevent hunting
    if (const auto current_state = state_machine_.getState();
        current_state == StateMachine::State::GO &&
        distance_until_stop + eps < pass_judge_line_dist) {
      return OverPassJudge{"over the pass judge line. no plan needed."};
    }
  }
  return std::nullopt;
}

double BlindSpotModule::computeTimeToPassStopLine(
  const lanelet::ConstLanelets & blind_spot_lanelets,
  const geometry_msgs::msg::Pose & stop_line_pose) const
{
  // if ego is completely stopped, using ego velocity yields "no-collision"
  const auto & current_pose = planner_data_->current_odometry->pose;
  const auto current_arc_ego =
    lanelet::utils::getArcCoordinates(blind_spot_lanelets, current_pose).length;
  const auto stopline_arc_ego =
    lanelet::utils::getArcCoordinates(blind_spot_lanelets, stop_line_pose).length;
  const auto remaining_distance = stopline_arc_ego - current_arc_ego;
  return remaining_distance / std::max<double>(
                                planner_param_.ttc_ego_minimal_velocity,
                                planner_data_->current_velocity->twist.linear.x);
}

std::optional<autoware_perception_msgs::msg::PredictedObject> BlindSpotModule::isCollisionDetected(
  const lanelet::ConstLanelets & blind_spot_lanelets,
  const geometry_msgs::msg::Pose & stop_line_pose, const lanelet::CompoundPolygon3d & area,
  const double ego_time_to_reach_stop_line)
{
  // check objects in blind spot areas
  const auto stop_line_arc_ego =
    lanelet::utils::getArcCoordinates(blind_spot_lanelets, stop_line_pose).length;
  for (const auto & original_object : planner_data_->predicted_objects->objects) {
    if (!isTargetObjectType(original_object)) {
      continue;
    }
    const auto object = cutPredictPathWithDuration(
      planner_data_->predicted_objects->header, original_object,
      planner_param_.max_future_movement_time);
    // right direction
    const bool exist_in_detection_area = bg::within(
      to_bg2d(object.kinematics.initial_pose_with_covariance.pose.position),
      lanelet::utils::to2D(area));
    if (!exist_in_detection_area) {
      continue;
    }
    const auto object_arc_length =
      lanelet::utils::getArcCoordinates(
        blind_spot_lanelets, object.kinematics.initial_pose_with_covariance.pose)
        .length;
    const auto object_time_to_reach_stop_line =
      (object_arc_length - stop_line_arc_ego) /
      (object.kinematics.initial_twist_with_covariance.twist.linear.x);
    const auto ttc = ego_time_to_reach_stop_line - object_time_to_reach_stop_line;
    RCLCPP_DEBUG(logger_, "object ttc is %f", ttc);
    if (planner_param_.ttc_min < ttc && ttc < planner_param_.ttc_max) {
      return object;
    }
  }
  return std::nullopt;
}

bool BlindSpotModule::isTargetObjectType(
  const autoware_perception_msgs::msg::PredictedObject & object) const
{
  if (
    object.classification.at(0).label ==
      autoware_perception_msgs::msg::ObjectClassification::BICYCLE ||
    object.classification.at(0).label ==
      autoware_perception_msgs::msg::ObjectClassification::PEDESTRIAN ||
    object.classification.at(0).label ==
      autoware_perception_msgs::msg::ObjectClassification::MOTORCYCLE) {
    return true;
  }
  return false;
}

}  // namespace autoware::behavior_velocity_planner

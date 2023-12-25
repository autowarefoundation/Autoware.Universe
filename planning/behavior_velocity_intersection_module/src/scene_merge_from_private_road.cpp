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

#include "scene_merge_from_private_road.hpp"

#include "util.hpp"

#include <behavior_velocity_planner_common/utilization/path_utilization.hpp>
#include <behavior_velocity_planner_common/utilization/util.hpp>
#include <lanelet2_extension/regulatory_elements/road_marking.hpp>
#include <lanelet2_extension/utility/utilities.hpp>
#include <motion_utils/trajectory/trajectory.hpp>

#include <lanelet2_core/geometry/Polygon.h>
#include <lanelet2_core/primitives/BasicRegulatoryElements.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace behavior_velocity_planner
{
namespace bg = boost::geometry;

MergeFromPrivateRoadModule::MergeFromPrivateRoadModule(
  const int64_t module_id, const int64_t lane_id,
  [[maybe_unused]] std::shared_ptr<const PlannerData> planner_data,
  const PlannerParam & planner_param, const std::set<lanelet::Id> & associative_ids,
  const rclcpp::Logger logger, const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  lane_id_(lane_id),
  associative_ids_(associative_ids)
{
  velocity_factor_.init(PlanningBehavior::MERGE);
  planner_param_ = planner_param;
  state_machine_.setState(StateMachine::State::STOP);
}

static std::optional<lanelet::ConstLanelet> getFirstConflictingLanelet(
  const lanelet::ConstLanelets & conflicting_lanelets,
  const util::InterpolatedPathInfo & interpolated_path_info,
  const tier4_autoware_utils::LinearRing2d & footprint, const double vehicle_length)
{
  const auto & path_ip = interpolated_path_info.path;
  const auto [lane_start, end] = interpolated_path_info.lane_id_interval.value();
  const size_t vehicle_length_idx = static_cast<size_t>(vehicle_length / interpolated_path_info.ds);
  const size_t start =
    static_cast<size_t>(std::max<int>(0, static_cast<int>(lane_start) - vehicle_length_idx));

  for (size_t i = start; i <= end; ++i) {
    const auto & pose = path_ip.points.at(i).point.pose;
    const auto path_footprint =
      tier4_autoware_utils::transformVector(footprint, tier4_autoware_utils::pose2transform(pose));
    for (const auto & conflicting_lanelet : conflicting_lanelets) {
      const auto polygon_2d = conflicting_lanelet.polygon2d().basicPolygon();
      const bool intersects = bg::intersects(polygon_2d, path_footprint);
      if (intersects) {
        return std::make_optional(conflicting_lanelet);
      }
    }
  }
  return std::nullopt;
}

static std::optional<size_t> getFirstPointInsidePolygonByFootprint(
  const lanelet::CompoundPolygon3d & polygon,
  const util::InterpolatedPathInfo & interpolated_path_info,
  const tier4_autoware_utils::LinearRing2d & footprint, const double vehicle_length)
{
  const auto & path_ip = interpolated_path_info.path;
  const auto [lane_start, lane_end] = interpolated_path_info.lane_id_interval.value();
  const size_t vehicle_length_idx = static_cast<size_t>(vehicle_length / interpolated_path_info.ds);
  const size_t start =
    static_cast<size_t>(std::max<int>(0, static_cast<int>(lane_start) - vehicle_length_idx));
  const auto area_2d = lanelet::utils::to2D(polygon).basicPolygon();
  for (auto i = start; i <= lane_end; ++i) {
    const auto & base_pose = path_ip.points.at(i).point.pose;
    const auto path_footprint = tier4_autoware_utils::transformVector(
      footprint, tier4_autoware_utils::pose2transform(base_pose));
    if (bg::intersects(path_footprint, area_2d)) {
      return std::make_optional<size_t>(i);
    }
  }
  return std::nullopt;
}

static std::optional<size_t> getDuplicatedPointIdx(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path,
  const geometry_msgs::msg::Point & point)
{
  for (size_t i = 0; i < path.points.size(); i++) {
    const auto & p = path.points.at(i).point.pose.position;

    constexpr double min_dist = 0.001;
    if (tier4_autoware_utils::calcDistance2d(p, point) < min_dist) {
      return i;
    }
  }

  return std::nullopt;
}

static std::optional<size_t> insertPointIndex(
  const geometry_msgs::msg::Pose & in_pose,
  autoware_auto_planning_msgs::msg::PathWithLaneId * inout_path,
  const double ego_nearest_dist_threshold, const double ego_nearest_yaw_threshold)
{
  const auto duplicate_idx_opt = getDuplicatedPointIdx(*inout_path, in_pose.position);
  if (duplicate_idx_opt) {
    return duplicate_idx_opt.value();
  }

  const size_t closest_idx = motion_utils::findFirstNearestIndexWithSoftConstraints(
    inout_path->points, in_pose, ego_nearest_dist_threshold, ego_nearest_yaw_threshold);
  // vector.insert(i) inserts element on the left side of v[i]
  // the velocity need to be zero order hold(from prior point)
  int insert_idx = closest_idx;
  autoware_auto_planning_msgs::msg::PathPointWithLaneId inserted_point =
    inout_path->points.at(closest_idx);
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

bool MergeFromPrivateRoadModule::modifyPathVelocity(PathWithLaneId * path, StopReason * stop_reason)
{
  debug_data_ = DebugData();
  *stop_reason = planning_utils::initializeStopReason(StopReason::MERGE_FROM_PRIVATE_ROAD);

  const auto input_path = *path;

  StateMachine::State current_state = state_machine_.getState();
  RCLCPP_DEBUG(
    logger_, "lane_id = %ld, state = %s", lane_id_, StateMachine::toString(current_state).c_str());

  /* get current pose */
  geometry_msgs::msg::Pose current_pose = planner_data_->current_odometry->pose;

  /* get lanelet map */
  const auto lanelet_map_ptr = planner_data_->route_handler_->getLaneletMapPtr();
  const auto routing_graph_ptr = planner_data_->route_handler_->getRoutingGraphPtr();

  /* spline interpolation */
  const auto interpolated_path_info_opt = util::generateInterpolatedPath(
    lane_id_, associative_ids_, *path, planner_param_.path_interpolation_ds, logger_);
  if (!interpolated_path_info_opt) {
    RCLCPP_DEBUG_SKIPFIRST_THROTTLE(logger_, *clock_, 1000 /* ms */, "splineInterpolate failed");
    RCLCPP_DEBUG(logger_, "===== plan end =====");
    return false;
  }
  const auto & interpolated_path_info = interpolated_path_info_opt.value();
  if (!interpolated_path_info.lane_id_interval) {
    RCLCPP_WARN(logger_, "Path has no interval on intersection lane %ld", lane_id_);
    RCLCPP_DEBUG(logger_, "===== plan end =====");
    return false;
  }

  const auto local_footprint = planner_data_->vehicle_info_.createFootprint(0.0, 0.0);
  if (!first_conflicting_lanelet_) {
    const auto & assigned_lanelet = lanelet_map_ptr->laneletLayer.get(lane_id_);
    const auto conflicting_lanelets =
      lanelet::utils::getConflictingLanelets(routing_graph_ptr, assigned_lanelet);
    first_conflicting_lanelet_ = getFirstConflictingLanelet(
      conflicting_lanelets, interpolated_path_info, local_footprint,
      planner_data_->vehicle_info_.max_longitudinal_offset_m);
  }
  if (!first_conflicting_lanelet_) {
    return false;
  }
  const auto first_conflicting_lanelet = first_conflicting_lanelet_.value();

  const auto first_conflicting_idx_opt = getFirstPointInsidePolygonByFootprint(
    first_conflicting_lanelet.polygon3d(), interpolated_path_info, local_footprint,
    planner_data_->vehicle_info_.max_longitudinal_offset_m);
  if (!first_conflicting_idx_opt) {
    return false;
  }
  const auto stopline_idx_ip = first_conflicting_idx_opt.value();

  const auto stopline_idx_opt = insertPointIndex(
    interpolated_path_info.path.points.at(stopline_idx_ip).point.pose, path,
    planner_data_->ego_nearest_dist_threshold, planner_data_->ego_nearest_yaw_threshold);
  if (!stopline_idx_opt) {
    RCLCPP_DEBUG(logger_, "failed to insert stopline, ignore planning.");
    return true;
  }
  const auto stopline_idx = stopline_idx_opt.value();

  debug_data_.virtual_wall_pose = planning_utils::getAheadPose(
    stopline_idx, planner_data_->vehicle_info_.max_longitudinal_offset_m, *path);
  debug_data_.stop_point_pose = path->points.at(stopline_idx).point.pose;

  /* set stop speed */
  if (state_machine_.getState() == StateMachine::State::STOP) {
    constexpr double v = 0.0;
    planning_utils::setVelocityFromIndex(stopline_idx, v, path);

    /* get stop point and stop factor */
    tier4_planning_msgs::msg::StopFactor stop_factor;
    stop_factor.stop_pose = debug_data_.stop_point_pose;
    planning_utils::appendStopReason(stop_factor, stop_reason);
    const auto & stop_pose = path->points.at(stopline_idx).point.pose;
    velocity_factor_.set(
      path->points, planner_data_->current_odometry->pose, stop_pose, VelocityFactor::UNKNOWN);

    const double signed_arc_dist_to_stop_point = motion_utils::calcSignedArcLength(
      path->points, current_pose.position, path->points.at(stopline_idx).point.pose.position);

    if (
      signed_arc_dist_to_stop_point < planner_param_.stop_distance_threshold &&
      planner_data_->isVehicleStopped(planner_param_.stop_duration_sec)) {
      state_machine_.setState(StateMachine::State::GO);
      if (signed_arc_dist_to_stop_point < -planner_param_.stop_distance_threshold) {
        RCLCPP_ERROR(logger_, "Failed to stop near stop line but ego stopped. Change state to GO");
      }
    }

    return true;
  }

  return true;
}

autoware_auto_planning_msgs::msg::PathWithLaneId
MergeFromPrivateRoadModule::extractPathNearExitOfPrivateRoad(
  const autoware_auto_planning_msgs::msg::PathWithLaneId & path, const double extend_length)
{
  if (path.points.size() < 2) {
    return path;
  }

  autoware_auto_planning_msgs::msg::PathWithLaneId private_path = path;
  private_path.points.clear();

  double sum_dist = 0.0;
  bool prev_has_target_lane_id = false;
  for (int i = static_cast<int>(path.points.size()) - 2; i >= 0; i--) {
    bool has_target_lane_id = false;
    for (const auto path_id : path.points.at(i).lane_ids) {
      if (path_id == lane_id_) {
        has_target_lane_id = true;
      }
    }
    if (has_target_lane_id) {
      // add path point with target lane id
      // (lanelet with target lane id is exit of private road)
      private_path.points.emplace_back(path.points.at(i));
      prev_has_target_lane_id = true;
      continue;
    }
    if (prev_has_target_lane_id) {
      // extend path to the front
      private_path.points.emplace_back(path.points.at(i));
      sum_dist += tier4_autoware_utils::calcDistance2d(
        path.points.at(i).point.pose, path.points.at(i + 1).point.pose);
      if (sum_dist > extend_length) {
        break;
      }
    }
  }

  std::reverse(private_path.points.begin(), private_path.points.end());
  return private_path;
}
}  // namespace behavior_velocity_planner

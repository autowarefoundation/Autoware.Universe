// Copyright 2023 Tier IV, Inc.
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

#ifndef SCENE_MODULE__OUT_OF_LANE__DECISIONS_HPP_
#define SCENE_MODULE__OUT_OF_LANE__DECISIONS_HPP_

#include "scene_module/out_of_lane/out_of_lane_utils.hpp"
#include "scene_module/out_of_lane/overlapping_range.hpp"
#include "scene_module/out_of_lane/types.hpp"

#include <route_handler/route_handler.hpp>

#include <autoware_auto_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_auto_planning_msgs/msg/path_with_lane_id.hpp>

#include <lanelet2_core/LaneletMap.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace behavior_velocity_planner::out_of_lane_utils
{
inline double distance_along_path(const EgoData & ego_data, const size_t target_idx)
{
  return motion_utils::calcSignedArcLength(
    ego_data.path->points, ego_data.pose.position, ego_data.first_path_idx + target_idx);
}

inline double time_along_path(const EgoData & ego_data, const size_t target_idx)
{
  const auto dist = distance_along_path(ego_data, target_idx);
  // TODO(Maxime): improve estimate of velocity
  const auto v = std::max(
    ego_data.velocity,
    ego_data.path->points[ego_data.first_path_idx + target_idx].point.longitudinal_velocity_mps *
      0.5);
  return dist / v;
}

/// @brief estimate the times when an object will enter and exit an overlapping range using its
/// predicted paths
/// @details times when the predicted paths of the object enters/exits the range are calculated
/// but may not exist (e.g,, predicted path ends before reaching the end of the range)
/// so we also calculate the min/max time inside the range.
inline std::optional<std::pair<double, double>> object_time_to_range(
  const autoware_auto_perception_msgs::msg::PredictedObject & object, const OverlapRange & range)
{
  constexpr auto max_deviation = 2.0;
  const auto half_size = object.shape.dimensions.x / 2.0;

  geometry_msgs::msg::Pose pose;
  pose.position.set__x(range.entering_point.x()).set__y(range.entering_point.y());
  const auto range_enter_length =
    lanelet::utils::getArcCoordinates({range.lane}, pose).length - half_size;
  pose.position.set__x(range.exiting_point.x()).set__y(range.exiting_point.y());
  const auto range_exit_length =
    lanelet::utils::getArcCoordinates({range.lane}, pose).length + half_size;
  auto worst_enter_time = std::optional<double>();
  auto worst_exit_time = std::optional<double>();

  for (const auto & predicted_path : object.kinematics.predicted_paths) {
    const auto time_step = rclcpp::Duration(predicted_path.time_step).seconds();

    const auto enter_point =
      geometry_msgs::msg::Point().set__x(range.entering_point.x()).set__y(range.entering_point.y());
    const auto enter_segment_idx =
      motion_utils::findNearestSegmentIndex(predicted_path.path, enter_point);
    const auto enter_offset = motion_utils::calcLongitudinalOffsetToSegment(
      predicted_path.path, enter_segment_idx, enter_point);
    const auto enter_lat_dist =
      tier4_autoware_utils::calcDistance2d(predicted_path.path[enter_segment_idx], enter_point);
    const auto enter_segment_length = tier4_autoware_utils::calcDistance2d(
      predicted_path.path[enter_segment_idx], predicted_path.path[enter_segment_idx + 1]);
    const auto enter_offset_ratio = enter_offset / enter_segment_length;
    const auto enter_time = enter_segment_idx * time_step + enter_offset_ratio * time_step;

    const auto exit_point =
      geometry_msgs::msg::Point().set__x(range.exiting_point.x()).set__y(range.exiting_point.y());
    const auto exit_segment_idx =
      motion_utils::findNearestSegmentIndex(predicted_path.path, exit_point);
    const auto exit_offset = motion_utils::calcLongitudinalOffsetToSegment(
      predicted_path.path, exit_segment_idx, exit_point);
    const auto exit_lat_dist =
      tier4_autoware_utils::calcDistance2d(predicted_path.path[exit_segment_idx], exit_point);
    const auto exit_segment_length = tier4_autoware_utils::calcDistance2d(
      predicted_path.path[exit_segment_idx], predicted_path.path[exit_segment_idx + 1]);
    const auto exit_offset_ratio = exit_offset / static_cast<double>(exit_segment_length);
    const auto exit_time =
      static_cast<double>(exit_segment_idx) * time_step + exit_offset_ratio * time_step;

    // predicted path is too far from the overlapping range to be relevent
    const auto is_far_from_entering_point = enter_lat_dist > max_deviation;
    const auto is_far_from_exiting_point = exit_lat_dist > max_deviation;
    if (is_far_from_entering_point && is_far_from_exiting_point) continue;
    // else we rely on the interpolation to estimate beyond the end of the predicted path

    const auto same_driving_direction_as_ego = enter_time < exit_time;
    if (same_driving_direction_as_ego) {
      worst_enter_time = worst_enter_time ? std::min(*worst_enter_time, enter_time) : enter_time;
      worst_exit_time = worst_exit_time ? std::max(*worst_exit_time, exit_time) : exit_time;
    } else {
      worst_enter_time = worst_enter_time ? std::max(*worst_enter_time, enter_time) : enter_time;
      worst_exit_time = worst_exit_time ? std::min(*worst_exit_time, exit_time) : exit_time;
    }
  }
  if (worst_enter_time && worst_exit_time)
    return std::make_pair(*worst_enter_time, *worst_exit_time);
  return {};
}

/// @brief estimate the times when an object will enter and exit an overlapping range assuming it
/// follows some lanelet
/// @details the enter/exit is relative to ego and may be inversed if the object drives in the
/// opposite direction
inline std::optional<std::pair<double, double>> object_time_to_range(
  const autoware_auto_perception_msgs::msg::PredictedObject & object, const OverlapRange & range,
  const lanelet::ConstLanelets & lanelets,
  const std::shared_ptr<route_handler::RouteHandler> & route_handler)
{
  const auto & p = object.kinematics.initial_pose_with_covariance.pose.position;
  const auto object_point = lanelet::BasicPoint2d(p.x, p.y);
  const auto half_size = object.shape.dimensions.x / 2.0;
  lanelet::ConstLanelets object_lanelets;
  for (const auto & ll : lanelets)
    if (boost::geometry::within(object_point, ll.polygon2d().basicPolygon()))
      object_lanelets.push_back(ll);

  geometry_msgs::msg::Pose pose;
  pose.position.set__x(range.entering_point.x()).set__y(range.entering_point.y());
  const auto range_enter_length = lanelet::utils::getArcCoordinates({range.lane}, pose).length;
  pose.position.set__x(range.exiting_point.x()).set__y(range.exiting_point.y());
  const auto range_exit_length = lanelet::utils::getArcCoordinates({range.lane}, pose).length;
  const auto range_size = std::abs(range_enter_length - range_exit_length);
  // TODO(Maxime): rename arc_length -> dist
  auto enter_arc_length = std::optional<double>();
  auto exit_arc_length = std::optional<double>();
  for (const auto & lane : object_lanelets) {
    const auto path = route_handler->getRoutingGraphPtr()->shortestPath(lane, range.lane);
    std::printf(
      "\t\t\tPath ? %d [from %ld to %ld]\n", path.has_value(), lane.id(), range.lane.id());
    if (path) {
      lanelet::ConstLanelets lls;
      for (const auto & ll : *path) lls.push_back(ll);
      pose.position.set__x(object_point.x()).set__y(object_point.y());
      const auto object_curr_length = lanelet::utils::getArcCoordinates(lls, pose).length;
      pose.position.set__x(range.entering_point.x()).set__y(range.entering_point.y());
      const auto enter_length =
        lanelet::utils::getArcCoordinates(lls, pose).length - object_curr_length;
      pose.position.set__x(range.exiting_point.x()).set__y(range.exiting_point.y());
      const auto exit_length =
        lanelet::utils::getArcCoordinates(lls, pose).length - object_curr_length;
      std::printf(
        "\t\t\t%2.2f -> [%2.2f(%2.2f, %2.2f) - %2.2f(%2.2f, %2.2f)]\n", object_curr_length,
        enter_length, range.entering_point.x(), range.entering_point.y(), exit_length,
        range.exiting_point.x(), range.exiting_point.y());
      const auto already_entered_range = std::abs(enter_length - exit_length) > range_size * 2.0;
      if (already_entered_range) continue;
      // multiple paths to the overlap -> be conservative and use the "worst" case
      // "worst" = min/max arc length depending on if the lane is running opposite to the ego path
      const auto is_opposite = enter_length > exit_length;
      if (!enter_arc_length)
        enter_arc_length = enter_length;
      else if (is_opposite)
        enter_arc_length = std::max(*enter_arc_length, enter_length);
      else
        enter_arc_length = std::min(*enter_arc_length, enter_length);
      if (!exit_arc_length)
        exit_arc_length = exit_length;
      else if (is_opposite)
        exit_arc_length = std::max(*exit_arc_length, exit_length);
      else
        exit_arc_length = std::min(*exit_arc_length, exit_length);
    }
  }
  // TODO(Maxime): fix this for opposite lanes (and also test the "use_predicted_path")
  if (enter_arc_length && exit_arc_length) {
    const auto v = object.kinematics.initial_twist_with_covariance.twist.linear.x;
    return std::make_pair((*enter_arc_length - half_size) / v, (*exit_arc_length + half_size) / v);
  }
  return {};
}

inline std::vector<Slowdown> calculate_decisions(
  const OverlapRanges & ranges, const EgoData & ego_data,
  const autoware_auto_perception_msgs::msg::PredictedObjects & objects,
  const std::shared_ptr<route_handler::RouteHandler> & route_handler,
  const lanelet::ConstLanelets & lanelets, const PlannerParam & params, DebugData & debug)
{
  std::vector<Slowdown> decisions;
  std::cout << "** Decisions\n";
  for (const auto & range : ranges) {
    if (range.entering_path_idx == 0UL) continue;  // skip if we already entered the range
    bool should_not_enter = false;  // we will decide if we need to stop/slow before this range

    const auto ego_enter_time = time_along_path(ego_data, range.entering_path_idx);
    const auto ego_exit_time = time_along_path(ego_data, range.exiting_path_idx);
    const auto ego_dist_to_range = distance_along_path(ego_data, range.entering_path_idx);
    std::printf(
      "\t[%lu -> %lu] %ld | ego dist = %2.2f (enters at %2.2f, exits at %2.2f)\n",
      range.entering_path_idx, range.exiting_path_idx, range.lane.id(), ego_dist_to_range,
      ego_enter_time, ego_exit_time);

    // TODO(Maxime): remove debug ?
    debug.ranges.push_back(range);
    debug.ego_times.emplace_back(ego_enter_time, ego_exit_time);
    auto & npc_times = debug.npc_times.emplace_back();

    for (const auto & object : objects.objects) {
      auto & debug_pair = npc_times.emplace_back(0.0, 0.0);
      if (object.kinematics.initial_twist_with_covariance.twist.linear.x < params.objects_min_vel)
        continue;  // skip objects with velocity bellow a threshold
      // skip objects that are already on the interval
      const auto enter_exit_time = params.objects_use_predicted_paths
                                     ? object_time_to_range(object, range)
                                     : object_time_to_range(object, range, lanelets, route_handler);
      if (!enter_exit_time) continue;  // object is not driving towards the overlapping range

      const auto & [enter_time, exit_time] = *enter_exit_time;
      debug_pair.first = std::min(enter_time, exit_time);
      debug_pair.second = std::max(enter_time, exit_time);
      std::printf(
        "\t\t[%s] going at %2.2fm/s enter at %2.2fs, exits at %2.2fs\n",
        tier4_autoware_utils::toHexString(object.object_id).c_str(),
        object.kinematics.initial_twist_with_covariance.twist.linear.x, enter_time, exit_time);

      const auto object_is_going_opposite_way = enter_time > exit_time;
      if (params.mode == "threshold") {
        if (std::min(enter_time, exit_time) < params.time_threshold) should_not_enter = true;
      } else if (params.mode == "intervals") {
        if (object_is_going_opposite_way) {
          const auto ego_exits_before_object_enters =
            ego_exit_time + params.intervals_ego_buffer < enter_time + params.intervals_obj_buffer;
          if (ego_exits_before_object_enters) should_not_enter = true;
          std::printf(
            "\t\t\t[Intervals] (opposite way) ego exit %2.2fs < obj enter %2.2fs ? -> should not "
            "enter = %d\n",
            ego_exit_time + params.intervals_ego_buffer, enter_time + params.intervals_obj_buffer,
            ego_exits_before_object_enters);
        } else {
          const auto object_enters_during_overlap =
            ego_enter_time - params.intervals_ego_buffer <
              enter_time + params.intervals_obj_buffer &&
            enter_time - params.intervals_obj_buffer - ego_exit_time <
              ego_exit_time + params.intervals_ego_buffer;
          const auto object_exits_during_overlap =
            ego_enter_time - params.intervals_ego_buffer <
              exit_time + params.intervals_obj_buffer &&
            exit_time - params.intervals_obj_buffer - ego_exit_time <
              ego_exit_time + params.intervals_ego_buffer;
          if (object_enters_during_overlap || object_exits_during_overlap) should_not_enter = true;
        }
      } else if (params.mode == "ttc") {
        const auto ttc_at_enter = ego_enter_time - enter_time;
        const auto ttc_at_exit = ego_exit_time - exit_time;
        const auto collision_during_overlap = (ttc_at_enter < 0.0 != ttc_at_exit < 0.0);
        const auto ttc_is_bellow_threshold =
          std::min(std::abs(ttc_at_enter), std::abs(ttc_at_exit)) <= params.ttc_threshold;
        if (collision_during_overlap || ttc_is_bellow_threshold) should_not_enter = true;
        std::printf(
          "\t\t\t[TTC] (%2.2fs - %2.2fs) -> %d\n", ttc_at_enter, ttc_at_exit,
          (collision_during_overlap || ttc_is_bellow_threshold));
      }
    }
    if (should_not_enter) {
      Slowdown decision;
      auto stop_before_range = range;
      if (params.strict) {
        // find lowest entering_path_idx for ranges with overlapping start-end indexes
        bool found = true;
        while (found) {
          found = false;
          for (const auto & other_range : ranges) {
            if (
              other_range.entering_path_idx < stop_before_range.entering_path_idx &&
              other_range.exiting_path_idx >= stop_before_range.entering_path_idx) {
              stop_before_range = other_range;
              found = true;
            }
          }
        }
      }
      decision.target_path_idx =
        ego_data.first_path_idx + stop_before_range.entering_path_idx;  // add offset from curr pose
      decision.lane_to_avoid = stop_before_range.lane;

      if (ego_dist_to_range < params.stop_dist_threshold) {
        std::printf("\t\tWill stop\n");
        decision.velocity = 0.0;
        decisions.push_back(decision);
      } else if (ego_dist_to_range < params.slow_dist_threshold) {
        std::printf("\t\tWill slow\n");
        decision.velocity = params.slow_velocity;
        decisions.push_back(decision);
      }
    }
  }
  return decisions;
}

}  // namespace behavior_velocity_planner::out_of_lane_utils

#endif  // SCENE_MODULE__OUT_OF_LANE__DECISIONS_HPP_

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

#ifndef AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__UTIL_HPP_
#define AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__UTIL_HPP_

#include <autoware/route_handler/route_handler.hpp>

#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_routing/RoutingGraph.h>

#include <memory>
#include <optional>
#include <utility>

namespace autoware::behavior_velocity_planner
{

enum class TurnDirection { LEFT, RIGHT };

/**
 * @brief  wrapper class of interpolated path with lane id
 */
struct InterpolatedPathInfo
{
  /** the interpolated path */
  tier4_planning_msgs::msg::PathWithLaneId path;
  /** discretization interval of interpolation */
  double ds{0.0};
  /** the intersection lanelet id */
  lanelet::Id lane_id{0};
  /** the range of indices for the path points with associative lane id */
  std::optional<std::pair<size_t, size_t>> lane_id_interval{std::nullopt};
};

std::optional<InterpolatedPathInfo> generateInterpolatedPathInfo(
  const lanelet::Id lane_id, const tier4_planning_msgs::msg::PathWithLaneId & input_path,
  rclcpp::Logger logger);

std::optional<lanelet::ConstLanelet> getSiblingStraightLanelet(
  const lanelet::Lanelet assigned_lane,
  const lanelet::routing::RoutingGraphConstPtr routing_graph_ptr);

/**
 * @brief Create half lanelet
 * @param lanelet input lanelet
 * @return Half lanelet
 */
lanelet::ConstLanelet generateHalfLanelet(
  const lanelet::ConstLanelet lanelet, const TurnDirection & turn_direction,
  const double ignore_width_from_centerline);

lanelet::ConstLanelet generateExtendedAdjacentLanelet(
  const lanelet::ConstLanelet lanelet, const TurnDirection direction,
  const double adjacent_extend_width);
lanelet::ConstLanelet generateExtendedOppositeAdjacentLanelet(
  const lanelet::ConstLanelet lanelet, const TurnDirection direction,
  const double opposite_adjacent_extend_width);

lanelet::ConstLanelets generateBlindSpotLanelets(
  const std::shared_ptr<autoware::route_handler::RouteHandler> route_handler,
  const TurnDirection turn_direction, const lanelet::Id lane_id,
  const tier4_planning_msgs::msg::PathWithLaneId & path, const double ignore_width_from_centerline,
  const double adjacent_extend_width, const double opposite_adjacent_extend_width);

/**
 * @brief Make blind spot areas. Narrow area is made from closest path point to stop line index.
 * Broad area is made from backward expanded point to stop line point
 * @param path path information associated with lane id
 * @param closest_idx closest path point index from ego car in path points
 * @return Blind spot polygons
 */
std::optional<lanelet::CompoundPolygon3d> generateBlindSpotPolygons(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const size_t closest_idx,
  const lanelet::ConstLanelets & blind_spot_lanelets,
  const geometry_msgs::msg::Pose & stop_line_pose, const double backward_detection_length);

}  // namespace autoware::behavior_velocity_planner

#endif  // AUTOWARE__BEHAVIOR_VELOCITY_BLIND_SPOT_MODULE__UTIL_HPP_

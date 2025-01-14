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

#include <autoware/behavior_velocity_blind_spot_module/util.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/path_utilization.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>

#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/length.hpp>

#include <lanelet2_core/geometry/Polygon.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::behavior_velocity_planner
{

namespace
{
static bool hasLaneIds(
  const tier4_planning_msgs::msg::PathPointWithLaneId & p, const lanelet::Id id)
{
  for (const auto & pid : p.lane_ids) {
    if (pid == id) {
      return true;
    }
  }
  return false;
}

static std::optional<std::pair<size_t, size_t>> findLaneIdInterval(
  const tier4_planning_msgs::msg::PathWithLaneId & p, const lanelet::Id id)
{
  bool found = false;
  size_t start = 0;
  size_t end = p.points.size() > 0 ? p.points.size() - 1 : 0;
  if (start == end) {
    // there is only one point in the path
    return std::nullopt;
  }
  for (size_t i = 0; i < p.points.size(); ++i) {
    if (hasLaneIds(p.points.at(i), id)) {
      if (!found) {
        // found interval for the first time
        found = true;
        start = i;
      }
    } else if (found) {
      // prior point was in the interval. interval ended
      end = i;
      break;
    }
  }
  start = start > 0 ? start - 1 : 0;  // the idx of last point before the interval
  return found ? std::make_optional(std::make_pair(start, end)) : std::nullopt;
}
}  // namespace

std::optional<InterpolatedPathInfo> generateInterpolatedPathInfo(
  const lanelet::Id lane_id, const tier4_planning_msgs::msg::PathWithLaneId & input_path,
  rclcpp::Logger logger)
{
  constexpr double ds = 0.2;
  InterpolatedPathInfo interpolated_path_info;
  if (!splineInterpolate(input_path, ds, interpolated_path_info.path, logger)) {
    return std::nullopt;
  }
  interpolated_path_info.ds = ds;
  interpolated_path_info.lane_id = lane_id;
  interpolated_path_info.lane_id_interval =
    findLaneIdInterval(interpolated_path_info.path, lane_id);
  return interpolated_path_info;
}

std::optional<size_t> getFirstPointIntersectsLineByFootprint(
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
    if (boost::geometry::intersects(path_footprint, line2d)) {
      return std::make_optional<size_t>(i);
    }
  }
  return std::nullopt;
}

std::optional<lanelet::ConstLanelet> getSiblingStraightLanelet(
  const lanelet::Lanelet assigned_lane,
  const lanelet::routing::RoutingGraphConstPtr routing_graph_ptr)
{
  for (const auto & prev : routing_graph_ptr->previous(assigned_lane)) {
    for (const auto & following : routing_graph_ptr->following(prev)) {
      if (std::string(following.attributeOr("turn_direction", "else")) == "straight") {
        return following;
      }
    }
  }
  return std::nullopt;
}

lanelet::ConstLanelet generateHalfLanelet(
  const lanelet::ConstLanelet lanelet, const TurnDirection & turn_direction,
  const double ignore_width_from_centerline)
{
  lanelet::Points3d lefts, rights;

  const double offset = (turn_direction == TurnDirection::LEFT) ? ignore_width_from_centerline
                                                                : -ignore_width_from_centerline;
  const auto offset_centerline = lanelet::utils::getCenterlineWithOffset(lanelet, offset);

  const auto original_left_bound =
    (turn_direction == TurnDirection::LEFT) ? lanelet.leftBound() : offset_centerline;
  const auto original_right_bound =
    (turn_direction == TurnDirection::LEFT) ? offset_centerline : lanelet.rightBound();

  for (const auto & pt : original_left_bound) {
    lefts.emplace_back(pt);
  }
  for (const auto & pt : original_right_bound) {
    rights.emplace_back(pt);
  }
  const auto left_bound = lanelet::LineString3d(lanelet::InvalId, std::move(lefts));
  const auto right_bound = lanelet::LineString3d(lanelet::InvalId, std::move(rights));
  auto half_lanelet = lanelet::Lanelet(lanelet::InvalId, left_bound, right_bound);
  return half_lanelet;
}

lanelet::ConstLanelet generateExtendedAdjacentLanelet(
  const lanelet::ConstLanelet lanelet, const TurnDirection direction,
  const double adjacent_extend_width)
{
  const auto centerline = lanelet.centerline2d();
  const auto width =
    boost::geometry::area(lanelet.polygon2d().basicPolygon()) / boost::geometry::length(centerline);
  const double extend_width = std::min<double>(adjacent_extend_width, width);
  const auto left_bound_ =
    direction == TurnDirection::LEFT
      ? lanelet::utils::getCenterlineWithOffset(lanelet, -width / 2 + extend_width)
      : lanelet.leftBound();
  const auto right_bound_ =
    direction == TurnDirection::RIGHT
      ? lanelet::utils::getCenterlineWithOffset(lanelet, width / 2 - extend_width)
      : lanelet.rightBound();
  lanelet::Points3d lefts, rights;
  for (const auto & pt : left_bound_) {
    lefts.emplace_back(pt);
  }
  for (const auto & pt : right_bound_) {
    rights.emplace_back(pt);
  }
  const auto left_bound = lanelet::LineString3d(lanelet::InvalId, std::move(lefts));
  const auto right_bound = lanelet::LineString3d(lanelet::InvalId, std::move(rights));
  auto new_lanelet = lanelet::Lanelet(lanelet::InvalId, left_bound, right_bound);
  const auto new_centerline = lanelet::utils::generateFineCenterline(new_lanelet, 5.0);
  new_lanelet.setCenterline(new_centerline);
  return new_lanelet;
}

lanelet::ConstLanelet generateExtendedOppositeAdjacentLanelet(
  const lanelet::ConstLanelet lanelet, const TurnDirection direction,
  const double opposite_adjacent_extend_width)
{
  const auto centerline = lanelet.centerline2d();
  const auto width =
    boost::geometry::area(lanelet.polygon2d().basicPolygon()) / boost::geometry::length(centerline);
  const double extend_width = std::min<double>(opposite_adjacent_extend_width, width);
  const auto left_bound_ =
    direction == TurnDirection::RIGHT
      ? lanelet.rightBound().invert()
      : lanelet::utils::getCenterlineWithOffset(lanelet.invert(), -width / 2 + extend_width);
  const auto right_bound_ =
    direction == TurnDirection::RIGHT
      ? lanelet::utils::getCenterlineWithOffset(lanelet.invert(), width / 2 - extend_width)
      : lanelet.leftBound().invert();
  lanelet::Points3d lefts, rights;
  for (const auto & pt : left_bound_) {
    lefts.emplace_back(pt);
  }
  for (const auto & pt : right_bound_) {
    rights.emplace_back(pt);
  }
  const auto left_bound = lanelet::LineString3d(lanelet::InvalId, std::move(lefts));
  const auto right_bound = lanelet::LineString3d(lanelet::InvalId, std::move(rights));
  auto new_lanelet = lanelet::Lanelet(lanelet::InvalId, left_bound, right_bound);
  const auto new_centerline = lanelet::utils::generateFineCenterline(new_lanelet, 5.0);
  new_lanelet.setCenterline(new_centerline);
  return new_lanelet;
}

static lanelet::LineString3d removeConst(lanelet::ConstLineString3d line)
{
  lanelet::Points3d pts;
  for (const auto & pt : line) {
    pts.emplace_back(pt);
  }
  return lanelet::LineString3d(lanelet::InvalId, pts);
}

std::vector<lanelet::Id> find_lane_ids_upto(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const lanelet::Id lane_id)
{
  std::vector<int64_t> lane_ids;
  /* get lane ids until intersection */
  for (const auto & point : path.points) {
    bool found_intersection_lane = false;
    for (const auto id : point.lane_ids) {
      if (id == lane_id) {
        found_intersection_lane = true;
        break;
      }
      // make lane_ids unique
      if (std::find(lane_ids.begin(), lane_ids.end(), id) == lane_ids.end()) {
        lane_ids.push_back(id);
      }
    }
    if (found_intersection_lane) break;
  }
  return lane_ids;
}

lanelet::ConstLanelets generateBlindSpotLanelets(
  const std::shared_ptr<autoware::route_handler::RouteHandler> route_handler,
  const TurnDirection turn_direction, const std::vector<lanelet::Id> & lane_ids_upto_intersection,
  const double ignore_width_from_centerline, const double adjacent_extend_width,
  const double opposite_adjacent_extend_width)
{
  const auto lanelet_map_ptr = route_handler->getLaneletMapPtr();
  const auto routing_graph_ptr = route_handler->getRoutingGraphPtr();

  lanelet::ConstLanelets blind_spot_lanelets;
  for (const auto i : lane_ids_upto_intersection) {
    const auto lane = lanelet_map_ptr->laneletLayer.get(i);
    const auto ego_half_lanelet =
      generateHalfLanelet(lane, turn_direction, ignore_width_from_centerline);
    const auto assoc_adj =
      turn_direction == TurnDirection::LEFT
        ? (routing_graph_ptr->adjacentLeft(lane))
        : (turn_direction == TurnDirection::RIGHT ? (routing_graph_ptr->adjacentRight(lane))
                                                  : boost::none);
    const std::optional<lanelet::ConstLanelet> opposite_adj =
      [&]() -> std::optional<lanelet::ConstLanelet> {
      if (!!assoc_adj) {
        return std::nullopt;
      }
      if (turn_direction == TurnDirection::LEFT) {
        // this should exist in right-hand traffic
        const auto adjacent_lanes =
          lanelet_map_ptr->laneletLayer.findUsages(lane.leftBound().invert());
        if (adjacent_lanes.empty()) {
          return std::nullopt;
        }
        return adjacent_lanes.front();
      }
      if (turn_direction == TurnDirection::RIGHT) {
        // this should exist in left-hand traffic
        const auto adjacent_lanes =
          lanelet_map_ptr->laneletLayer.findUsages(lane.rightBound().invert());
        if (adjacent_lanes.empty()) {
          return std::nullopt;
        }
        return adjacent_lanes.front();
      } else {
        return std::nullopt;
      }
    }();

    const auto assoc_shoulder = [&]() -> std::optional<lanelet::ConstLanelet> {
      if (turn_direction == TurnDirection::LEFT) {
        return route_handler->getLeftShoulderLanelet(lane);
      } else if (turn_direction == TurnDirection::RIGHT) {
        return route_handler->getRightShoulderLanelet(lane);
      }
      return std::nullopt;
    }();
    if (assoc_shoulder) {
      const auto lefts = (turn_direction == TurnDirection::LEFT)
                           ? assoc_shoulder.value().leftBound()
                           : ego_half_lanelet.leftBound();
      const auto rights = (turn_direction == TurnDirection::LEFT)
                            ? ego_half_lanelet.rightBound()
                            : assoc_shoulder.value().rightBound();
      blind_spot_lanelets.emplace_back(lanelet::InvalId, removeConst(lefts), removeConst(rights));

    } else if (!!assoc_adj) {
      const auto adj_half_lanelet =
        generateExtendedAdjacentLanelet(assoc_adj.value(), turn_direction, adjacent_extend_width);
      const auto lefts = (turn_direction == TurnDirection::LEFT) ? adj_half_lanelet.leftBound()
                                                                 : ego_half_lanelet.leftBound();
      const auto rights = (turn_direction == TurnDirection::RIGHT) ? adj_half_lanelet.rightBound()
                                                                   : ego_half_lanelet.rightBound();
      blind_spot_lanelets.emplace_back(lanelet::InvalId, removeConst(lefts), removeConst(rights));
    } else if (opposite_adj) {
      const auto adj_half_lanelet = generateExtendedOppositeAdjacentLanelet(
        opposite_adj.value(), turn_direction, opposite_adjacent_extend_width);
      const auto lefts = (turn_direction == TurnDirection::LEFT) ? adj_half_lanelet.leftBound()
                                                                 : ego_half_lanelet.leftBound();
      const auto rights = (turn_direction == TurnDirection::LEFT) ? ego_half_lanelet.rightBound()
                                                                  : adj_half_lanelet.rightBound();
      blind_spot_lanelets.emplace_back(lanelet::InvalId, removeConst(lefts), removeConst(rights));
    } else {
      blind_spot_lanelets.push_back(ego_half_lanelet);
    }
  }
  return blind_spot_lanelets;
}

std::optional<lanelet::CompoundPolygon3d> generateBlindSpotPolygons(
  [[maybe_unused]] const tier4_planning_msgs::msg::PathWithLaneId & path,
  [[maybe_unused]] const size_t closest_idx, const lanelet::ConstLanelets & blind_spot_lanelets,
  const geometry_msgs::msg::Pose & stop_line_pose, const double backward_detection_length)
{
  const auto stop_line_arc_ego =
    lanelet::utils::getArcCoordinates(blind_spot_lanelets, stop_line_pose).length;
  const auto detection_area_start_length_ego =
    std::max<double>(stop_line_arc_ego - backward_detection_length, 0.0);
  return lanelet::utils::getPolygonFromArcLength(
    blind_spot_lanelets, detection_area_start_length_ego, stop_line_arc_ego);
}

}  // namespace autoware::behavior_velocity_planner

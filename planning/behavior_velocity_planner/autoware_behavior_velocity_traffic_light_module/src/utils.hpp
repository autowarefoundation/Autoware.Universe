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

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <optional>
#include <utility>
#include <vector>

#define EIGEN_MPL2_ONLY
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <autoware/behavior_velocity_planner_common/utilization/boost_geometry_helper.hpp>
#include <autoware/motion_utils/trajectory/path_with_lane_id.hpp>

#include <lanelet2_core/LaneletMap.h>

namespace autoware::behavior_velocity_planner
{

/**
 * @brief create offset point.
 * @param src point of vector. this point is a base point for offset point.
 * @param dst point of vector.
 * @param offset length.
 * @return offset point.
 */
auto getOffsetPoint(const Eigen::Vector2d & src, const Eigen::Vector2d & dst, const double length)
  -> Eigen::Vector2d;

/**
 * @brief find intersection point between a path segment and stop line and return the point.
 * @param path segment.
 * @param stop line.
 * @param base point to find nearest intersection point.
 * @return intersection point. if there is no intersection point, return std::nullopt.
 */
auto findNearestCollisionPoint(
  const LineString2d & line1, const LineString2d & line2, const Point2d & origin)
  -> std::optional<Point2d>;

/**
 * @brief find intersection point between path and stop line and return the point.
 * @param input path.
 * @param stop line.
 * @param longitudinal offset.
 * @return first: insert point index, second: insert point position. if there is no intersection
 * point, return std::nullopt.
 */
auto createTargetPoint(
  const tier4_planning_msgs::msg::PathWithLaneId & input, const LineString2d & stop_line,
  const double offset) -> std::optional<std::pair<size_t, Eigen::Vector2d>>;

/**
 * @brief find intersection point between path and stop line and return the point.
 * @param input path.
 * @param stop line.
 * @param longitudinal offset.
 * @param extend length to find intersection point between path and stop line.
 * @return first: insert point index, second: insert point position. if there is no intersection
 * point, return std::nullopt.
 */
auto calcStopPointAndInsertIndex(
  const tier4_planning_msgs::msg::PathWithLaneId & input_path,
  const lanelet::ConstLineString3d & lanelet_stop_lines, const double & offset,
  const double & stop_line_extend_length) -> std::optional<std::pair<size_t, Eigen::Vector2d>>;

}  // namespace autoware::behavior_velocity_planner

#endif  // UTILS_HPP_

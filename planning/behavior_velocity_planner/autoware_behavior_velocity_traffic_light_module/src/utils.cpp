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

#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>

#include <boost/geometry/algorithms/distance.hpp>
#include <boost/geometry/algorithms/intersection.hpp>

#include <vector>

namespace autoware::behavior_velocity_planner
{
namespace bg = boost::geometry;

auto getOffsetPoint(const Eigen::Vector2d & src, const Eigen::Vector2d & dst, const double length)
  -> Eigen::Vector2d
{
  Eigen::Vector2d line_vec = dst - src;
  Eigen::Vector2d backward_vec = length * line_vec.normalized();
  return src + backward_vec;
}

auto findNearestCollisionPoint(
  const LineString2d & line1, const LineString2d & line2, const Point2d & origin)
  -> std::optional<Point2d>
{
  std::vector<Point2d> collision_points;
  bg::intersection(line1, line2, collision_points);

  if (collision_points.empty()) {
    return std::nullopt;
  }

  // check nearest collision point
  Point2d nearest_collision_point;
  double min_dist = 0.0;

  for (size_t i = 0; i < collision_points.size(); ++i) {
    double dist = bg::distance(origin, collision_points.at(i));
    if (i == 0 || dist < min_dist) {
      min_dist = dist;
      nearest_collision_point = collision_points.at(i);
    }
  }
  return nearest_collision_point;
}

auto createTargetPoint(
  const tier4_planning_msgs::msg::PathWithLaneId & input, const LineString2d & stop_line,
  const double offset) -> std::optional<std::pair<size_t, Eigen::Vector2d>>
{
  if (input.points.size() < 2) {
    return std::nullopt;
  }
  size_t target_point_idx{};
  for (size_t i = 0; i < input.points.size() - 1; ++i) {
    Point2d path_line_begin = {
      input.points.at(i).point.pose.position.x, input.points.at(i).point.pose.position.y};
    Point2d path_line_end = {
      input.points.at(i + 1).point.pose.position.x, input.points.at(i + 1).point.pose.position.y};
    LineString2d path_line = {path_line_begin, path_line_end};

    // check nearest collision point
    const auto nearest_collision_point =
      findNearestCollisionPoint(path_line, stop_line, path_line_begin);
    if (!nearest_collision_point) {
      continue;
    }

    // search target point index
    target_point_idx = 0;
    double length_sum = 0;

    Eigen::Vector2d point1, point2;
    if (0 <= offset) {
      point1 << nearest_collision_point->x(), nearest_collision_point->y();
      point2 << path_line_begin.x(), path_line_begin.y();
      length_sum += (point2 - point1).norm();
      for (size_t j = i; 0 < j; --j) {
        if (offset < length_sum) {
          target_point_idx = j + 1;
          break;
        }
        point1 << input.points.at(j).point.pose.position.x,
          input.points.at(j).point.pose.position.y;
        point2 << input.points.at(j - 1).point.pose.position.x,
          input.points.at(j - 1).point.pose.position.y;
        length_sum += (point2 - point1).norm();
      }
    } else {
      point1 << nearest_collision_point->x(), nearest_collision_point->y();
      point2 << path_line_end.x(), path_line_end.y();
      length_sum -= (point2 - point1).norm();
      for (size_t j = i + 1; j < input.points.size() - 1; ++j) {
        if (length_sum < offset) {
          target_point_idx = j;
          break;
        }
        point1 << input.points.at(j).point.pose.position.x,
          input.points.at(j).point.pose.position.y;
        point2 << input.points.at(j + 1).point.pose.position.x,
          input.points.at(j + 1).point.pose.position.y;
        length_sum -= (point2 - point1).norm();
      }
    }
    // create target point
    return std::make_pair(
      target_point_idx, getOffsetPoint(point2, point1, std::fabs(length_sum - offset)));
  }
  return std::nullopt;
}

auto calcStopPointAndInsertIndex(
  const tier4_planning_msgs::msg::PathWithLaneId & input_path,
  const lanelet::ConstLineString3d & lanelet_stop_lines, const double & offset,
  const double & stop_line_extend_length) -> std::optional<std::pair<size_t, Eigen::Vector2d>>
{
  LineString2d stop_line;

  for (size_t i = 0; i < lanelet_stop_lines.size() - 1; ++i) {
    stop_line = planning_utils::extendLine(
      lanelet_stop_lines[i], lanelet_stop_lines[i + 1], stop_line_extend_length);

    // Calculate stop pose and insert index,
    // if there is a collision point between path and stop line
    const auto output = createTargetPoint(input_path, stop_line, offset);
    if (output.has_value()) {
      return output;
    }
  }
  return std::nullopt;
}
}  // namespace autoware::behavior_velocity_planner

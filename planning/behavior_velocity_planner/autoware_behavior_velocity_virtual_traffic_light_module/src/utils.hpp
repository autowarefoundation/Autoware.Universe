// Copyright 2024 Tier IV, Inc.
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

#include <autoware/behavior_velocity_planner_common/utilization/arc_lane_util.hpp>
#include <autoware_lanelet2_extension/regulatory_elements/virtual_traffic_light.hpp>

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <tier4_v2x_msgs/msg/key_value.hpp>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace autoware::behavior_velocity_planner::virtual_traffic_light
{
struct SegmentIndexWithPoint
{
  size_t index;
  geometry_msgs::msg::Point point;
};

struct SegmentIndexWithOffset
{
  size_t index;
};

tier4_v2x_msgs::msg::KeyValue createKeyValue(const std::string & key, const std::string & value);

autoware::universe_utils::LineString3d toAutowarePoints(
  const lanelet::ConstLineString3d & line_string);

std::optional<autoware::universe_utils::LineString3d> toAutowarePoints(
  const lanelet::Optional<lanelet::ConstLineString3d> & line_string);

std::vector<autoware::universe_utils::LineString3d> toAutowarePoints(
  const lanelet::ConstLineStrings3d & line_strings);

autoware::universe_utils::Point3d calcCenter(
  const autoware::universe_utils::LineString3d & line_string);

geometry_msgs::msg::Pose calcHeadPose(
  const geometry_msgs::msg::Pose & base_link_pose, const double base_link_to_front);

geometry_msgs::msg::Point convertToGeomPoint(const autoware::universe_utils::Point3d & p);

void insertStopVelocityFromStart(autoware_internal_planning_msgs::msg::PathWithLaneId * path);

std::optional<size_t> insertStopVelocityAtCollision(
  const SegmentIndexWithPoint & collision, const double offset,
  autoware_internal_planning_msgs::msg::PathWithLaneId * path);

template <class T>
std::optional<SegmentIndexWithPoint> findLastCollisionBeforeEndLine(
  const T & points, const autoware::universe_utils::LineString3d & target_line,
  const size_t end_line_idx)
{
  const auto target_line_p1 = convertToGeomPoint(target_line.at(0));
  const auto target_line_p2 = convertToGeomPoint(target_line.at(1));

  for (size_t i = end_line_idx; 0 < i;
       --i) {  // NOTE: size_t can be used since it will not be negative.
    const auto & p1 = autoware::universe_utils::getPoint(points.at(i));
    const auto & p2 = autoware::universe_utils::getPoint(points.at(i - 1));
    const auto collision_point =
      arc_lane_utils::checkCollision(p1, p2, target_line_p1, target_line_p2);

    if (collision_point) {
      return SegmentIndexWithPoint{i, collision_point.value()};
    }
  }

  return {};
}

template <class T>
std::optional<SegmentIndexWithPoint> findLastCollisionBeforeEndLine(
  const T & points, const std::vector<autoware::universe_utils::LineString3d> & lines,
  const size_t end_line_idx)
{
  for (const auto & line : lines) {
    const auto collision = findLastCollisionBeforeEndLine(points, line, end_line_idx);
    if (collision) {
      return collision;
    }
  }

  return {};
}

}  // namespace autoware::behavior_velocity_planner::virtual_traffic_light

#endif  // UTILS_HPP_

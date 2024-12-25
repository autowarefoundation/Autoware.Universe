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

#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/utilization/path_utilization.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>

#include <limits>
#include <string>
#include <vector>

namespace autoware::behavior_velocity_planner::virtual_traffic_light
{

using autoware::universe_utils::calcDistance2d;

tier4_v2x_msgs::msg::KeyValue createKeyValue(const std::string & key, const std::string & value)
{
  return tier4_v2x_msgs::build<tier4_v2x_msgs::msg::KeyValue>().key(key).value(value);
}

autoware::universe_utils::LineString3d toAutowarePoints(
  const lanelet::ConstLineString3d & line_string)
{
  autoware::universe_utils::LineString3d output;
  for (const auto & p : line_string) {
    output.emplace_back(p.x(), p.y(), p.z());
  }
  return output;
}

std::optional<autoware::universe_utils::LineString3d> toAutowarePoints(
  const lanelet::Optional<lanelet::ConstLineString3d> & line_string)
{
  if (!line_string) {
    return {};
  }
  return toAutowarePoints(*line_string);
}

std::vector<autoware::universe_utils::LineString3d> toAutowarePoints(
  const lanelet::ConstLineStrings3d & line_strings)
{
  std::vector<autoware::universe_utils::LineString3d> output;
  for (const auto & line_string : line_strings) {
    output.emplace_back(toAutowarePoints(line_string));
  }
  return output;
}

autoware::universe_utils::Point3d calcCenter(
  const autoware::universe_utils::LineString3d & line_string)
{
  const auto p1 = line_string.front();
  const auto p2 = line_string.back();
  const auto p_center = (p1 + p2) / 2;
  return {p_center.x(), p_center.y(), p_center.z()};
}

geometry_msgs::msg::Pose calcHeadPose(
  const geometry_msgs::msg::Pose & base_link_pose, const double base_link_to_front)
{
  return autoware::universe_utils::calcOffsetPose(base_link_pose, base_link_to_front, 0.0, 0.0);
}

geometry_msgs::msg::Point convertToGeomPoint(const autoware::universe_utils::Point3d & p)
{
  geometry_msgs::msg::Point geom_p;
  geom_p.x = p.x();
  geom_p.y = p.y();

  return geom_p;
}

void insertStopVelocityFromStart(tier4_planning_msgs::msg::PathWithLaneId * path)
{
  for (auto & p : path->points) {
    p.point.longitudinal_velocity_mps = 0.0;
  }
}

std::optional<size_t> insertStopVelocityAtCollision(
  const SegmentIndexWithPoint & collision, const double offset,
  tier4_planning_msgs::msg::PathWithLaneId * path)
{
  const auto collision_offset = autoware::motion_utils::calcLongitudinalOffsetToSegment(
    path->points, collision.index, collision.point);

  const auto offset_segment =
    arc_lane_utils::findOffsetSegment(*path, collision.index, offset + collision_offset);
  if (!offset_segment) {
    return {};
  }

  const auto interpolated_pose = arc_lane_utils::calcTargetPose(*path, *offset_segment);

  if (offset_segment->second < 0) {
    insertStopVelocityFromStart(path);
    return 0;
  }

  auto insert_index = static_cast<size_t>(offset_segment->first + 1);
  auto insert_point = path->points.at(insert_index);
  insert_point.point.pose = interpolated_pose;
  // Insert 0 velocity after stop point or replace velocity with 0
  autoware::behavior_velocity_planner::planning_utils::insertVelocity(
    *path, insert_point, 0.0, insert_index);
  return insert_index;
}

}  // namespace autoware::behavior_velocity_planner::virtual_traffic_light

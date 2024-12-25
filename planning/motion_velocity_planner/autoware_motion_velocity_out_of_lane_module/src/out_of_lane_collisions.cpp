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

#include "out_of_lane_collisions.hpp"

#include "types.hpp"

#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <rclcpp/duration.hpp>

#include <autoware_planning_msgs/msg/trajectory_point.hpp>

#include <boost/geometry/algorithms/disjoint.hpp>
#include <boost/geometry/index/predicates.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/primitives/BoundingBox.h>
#include <lanelet2_core/primitives/Polygon.h>

#include <algorithm>
#include <iterator>
#include <limits>
#include <unordered_set>
#include <vector>

namespace autoware::motion_velocity_planner::out_of_lane
{

void update_collision_times(
  OutOfLaneData & out_of_lane_data, const std::unordered_set<size_t> & potential_collision_indexes,
  const universe_utils::Polygon2d & object_footprint, const double time)
{
  for (const auto index : potential_collision_indexes) {
    auto & out_of_lane_point = out_of_lane_data.outside_points[index];
    if (out_of_lane_point.collision_times.count(time) == 0UL) {
      const auto has_collision =
        !boost::geometry::disjoint(out_of_lane_point.out_overlaps, object_footprint.outer());
      if (has_collision) {
        out_of_lane_point.collision_times.insert(time);
      }
    }
  }
}

void calculate_object_path_time_collisions(
  OutOfLaneData & out_of_lane_data,
  const autoware_perception_msgs::msg::PredictedPath & object_path,
  const autoware_perception_msgs::msg::Shape & object_shape)
{
  const auto time_step = rclcpp::Duration(object_path.time_step).seconds();
  auto time = time_step;
  for (const auto & object_pose : object_path.path) {
    time += time_step;
    const auto object_footprint = universe_utils::toPolygon2d(object_pose, object_shape);
    std::vector<OutAreaNode> query_results;
    out_of_lane_data.outside_areas_rtree.query(
      boost::geometry::index::intersects(object_footprint.outer()),
      std::back_inserter(query_results));
    std::unordered_set<size_t> potential_collision_indexes;
    for (const auto & [_, index] : query_results) {
      potential_collision_indexes.insert(index);
    }
    update_collision_times(out_of_lane_data, potential_collision_indexes, object_footprint, time);
  }
}

void calculate_objects_time_collisions(
  OutOfLaneData & out_of_lane_data,
  const std::vector<autoware_perception_msgs::msg::PredictedObject> & objects)
{
  for (const auto & object : objects) {
    for (const auto & path : object.kinematics.predicted_paths) {
      calculate_object_path_time_collisions(out_of_lane_data, path, object.shape);
    }
  }
}

void calculate_min_max_arrival_times(
  OutOfLanePoint & out_of_lane_point,
  const std::vector<autoware_planning_msgs::msg::TrajectoryPoint> & trajectory)
{
  auto min_time = std::numeric_limits<double>::infinity();
  auto max_time = -std::numeric_limits<double>::infinity();
  for (const auto & t : out_of_lane_point.collision_times) {
    min_time = std::min(t, min_time);
    max_time = std::max(t, max_time);
  }
  if (min_time <= max_time) {
    out_of_lane_point.min_object_arrival_time = min_time;
    out_of_lane_point.max_object_arrival_time = max_time;
    const auto & ego_time =
      rclcpp::Duration(trajectory[out_of_lane_point.trajectory_index].time_from_start).seconds();
    if (ego_time >= min_time && ego_time <= max_time) {
      out_of_lane_point.ttc = 0.0;
    } else {
      out_of_lane_point.ttc =
        std::min(std::abs(ego_time - min_time), std::abs(ego_time - max_time));
    }
  }
};

void calculate_collisions_to_avoid(
  OutOfLaneData & out_of_lane_data,
  const std::vector<autoware_planning_msgs::msg::TrajectoryPoint> & trajectory,
  const PlannerParam & params)
{
  for (auto & out_of_lane_point : out_of_lane_data.outside_points) {
    calculate_min_max_arrival_times(out_of_lane_point, trajectory);
  }
  for (auto & p : out_of_lane_data.outside_points) {
    if (params.mode == "ttc") {
      p.to_avoid = p.ttc && p.ttc <= params.ttc_threshold;
    } else {
      p.to_avoid = p.min_object_arrival_time && p.min_object_arrival_time <= params.time_threshold;
    }
  }
}

OutOfLanePoint calculate_out_of_lane_point(
  const lanelet::BasicPolygon2d & footprint, const lanelet::ConstLanelets & out_lanelets,
  const OutLaneletRtree & out_lanelets_rtree)
{
  OutOfLanePoint p;
  std::vector<LaneletNode> candidates;
  out_lanelets_rtree.query(
    boost::geometry::index::intersects(footprint), std::back_inserter(candidates));
  for (const auto & [_, idx] : candidates) {
    const auto & lanelet = out_lanelets[idx];
    lanelet::BasicPolygons2d intersections;
    boost::geometry::intersection(footprint, lanelet.polygon2d().basicPolygon(), intersections);
    for (const auto & intersection : intersections) {
      universe_utils::Polygon2d poly;
      boost::geometry::convert(intersection, poly);
      p.out_overlaps.push_back(poly);
    }
    if (!intersections.empty()) {
      p.overlapped_lanelets.push_back(lanelet);
    }
  }
  return p;
}
std::vector<OutOfLanePoint> calculate_out_of_lane_points(const EgoData & ego_data)
{
  std::vector<OutOfLanePoint> out_of_lane_points;
  for (auto i = 0UL; i < ego_data.trajectory_footprints.size(); ++i) {
    const auto & footprint = ego_data.trajectory_footprints[i];
    OutOfLanePoint p =
      calculate_out_of_lane_point(footprint, ego_data.out_lanelets, ego_data.out_lanelets_rtree);
    p.trajectory_index = i;
    if (!p.overlapped_lanelets.empty()) {
      out_of_lane_points.push_back(p);
    }
  }
  return out_of_lane_points;
}

void prepare_out_of_lane_areas_rtree(OutOfLaneData & out_of_lane_data)
{
  std::vector<OutAreaNode> rtree_nodes;
  for (auto i = 0UL; i < out_of_lane_data.outside_points.size(); ++i) {
    for (const auto & out_overlap : out_of_lane_data.outside_points[i].out_overlaps) {
      OutAreaNode n;
      n.first = boost::geometry::return_envelope<universe_utils::Box2d>(out_overlap);
      n.second = i;
      rtree_nodes.push_back(n);
    }
  }
  out_of_lane_data.outside_areas_rtree = {rtree_nodes.begin(), rtree_nodes.end()};
}

}  // namespace autoware::motion_velocity_planner::out_of_lane

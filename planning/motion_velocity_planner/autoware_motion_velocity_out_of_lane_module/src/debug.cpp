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

#include "debug.hpp"

#include "types.hpp"

#include <autoware/motion_utils/marker/virtual_wall_marker_creator.hpp>
#include <autoware/motion_velocity_planner_common_universe/planner_data.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/ros/marker_helper.hpp>

#include <autoware_planning_msgs/msg/trajectory_point.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <std_msgs/msg/color_rgba.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <boost/geometry/algorithms/centroid.hpp>
#include <boost/geometry/algorithms/for_each.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>

#include <string>
#include <vector>

namespace autoware::motion_velocity_planner::out_of_lane::debug
{
namespace
{

visualization_msgs::msg::Marker get_base_marker()
{
  visualization_msgs::msg::Marker base_marker;
  base_marker.header.frame_id = "map";
  base_marker.header.stamp = rclcpp::Time(0);
  base_marker.id = 0;
  base_marker.type = visualization_msgs::msg::Marker::LINE_STRIP;
  base_marker.action = visualization_msgs::msg::Marker::ADD;
  base_marker.pose.position = universe_utils::createMarkerPosition(0.0, 0.0, 0);
  base_marker.pose.orientation = universe_utils::createMarkerOrientation(0, 0, 0, 1.0);
  base_marker.scale = universe_utils::createMarkerScale(0.1, 0.1, 0.1);
  base_marker.color = universe_utils::createMarkerColor(1.0, 0.1, 0.1, 0.5);
  return base_marker;
}
void add_polygons_markers(
  visualization_msgs::msg::MarkerArray & debug_marker_array,
  const visualization_msgs::msg::Marker & base_marker, const lanelet::BasicPolygons2d & polygons)
{
  auto debug_marker = base_marker;
  debug_marker.type = visualization_msgs::msg::Marker::LINE_LIST;
  for (const auto & f : polygons) {
    boost::geometry::for_each_segment(f, [&](const auto & s) {
      const auto & [p1, p2] = s;
      debug_marker.points.push_back(universe_utils::createMarkerPosition(p1.x(), p1.y(), 0.0));
      debug_marker.points.push_back(universe_utils::createMarkerPosition(p2.x(), p2.y(), 0.0));
    });
  }
  debug_marker_array.markers.push_back(debug_marker);
}

void add_current_overlap_marker(
  visualization_msgs::msg::MarkerArray & debug_marker_array,
  const lanelet::BasicPolygon2d & current_footprint, const double z)
{
  auto debug_marker = get_base_marker();
  debug_marker.ns = "current_overlap";
  debug_marker.points.clear();
  for (const auto & p : current_footprint)
    debug_marker.points.push_back(universe_utils::createMarkerPosition(p.x(), p.y(), z));
  if (!debug_marker.points.empty()) debug_marker.points.push_back(debug_marker.points.front());
  debug_marker.color = universe_utils::createMarkerColor(1.0, 0.1, 0.1, 0.5);
  debug_marker_array.markers.push_back(debug_marker);
  debug_marker.id++;
}

void add_ttc_markers(
  visualization_msgs::msg::MarkerArray & debug_marker_array, const EgoData & ego_data,
  const OutOfLaneData & out_of_lane_data, const size_t prev_nb)
{
  auto debug_marker = get_base_marker();
  debug_marker.type = visualization_msgs::msg::Marker::TEXT_VIEW_FACING;
  debug_marker.scale.z = 0.5;
  debug_marker.ns = "ttcs";
  for (const auto & p : out_of_lane_data.outside_points) {
    if (p.to_avoid) {
      debug_marker.color.r = 1.0;
      debug_marker.color.g = 0.0;
    } else {
      debug_marker.color.r = 0.0;
      debug_marker.color.g = 1.0;
    }
    if (p.ttc) {
      debug_marker.pose = ego_data.trajectory_points[p.trajectory_index].pose;
      debug_marker.text = std::to_string(*p.ttc);
      debug_marker_array.markers.push_back(debug_marker);
      debug_marker.id++;
    }
  }
  debug_marker.action = visualization_msgs::msg::Marker::DELETE;
  for (; debug_marker.id < static_cast<int>(prev_nb); ++debug_marker.id) {
    debug_marker_array.markers.push_back(debug_marker);
  }
}
size_t add_stop_line_markers(
  visualization_msgs::msg::MarkerArray & debug_marker_array, const StopLinesRtree & rtree,
  const double z, const size_t prev_nb)
{
  auto debug_marker = get_base_marker();
  debug_marker.type = visualization_msgs::msg::Marker::LINE_STRIP;
  debug_marker.ns = "object_stop_lines";
  const auto & add_lanelets_markers = [&](const auto & lanelets) {
    for (const auto & ll : lanelets) {
      debug_marker.points.clear();
      for (const auto & p : ll.polygon2d().basicPolygon()) {
        debug_marker.points.push_back(universe_utils::createMarkerPosition(p.x(), p.y(), z));
      }
      debug_marker.points.push_back(debug_marker.points.front());
      debug_marker_array.markers.push_back(debug_marker);
      ++debug_marker.id;
    }
  };
  for (const auto & [_, stop_line] : rtree) {
    debug_marker.points.clear();
    debug_marker.color.r = 1.0;
    for (const auto & p : stop_line.stop_line) {
      debug_marker.points.push_back(universe_utils::createMarkerPosition(p.x(), p.y(), z));
    }
    debug_marker_array.markers.push_back(debug_marker);
    ++debug_marker.id;
    debug_marker.color.r = 0.0;
    add_lanelets_markers(stop_line.lanelets);
  }
  const auto max_id = debug_marker.id;
  debug_marker.action = visualization_msgs::msg::Marker::DELETE;
  for (; debug_marker.id < static_cast<int>(prev_nb); ++debug_marker.id) {
    debug_marker_array.markers.push_back(debug_marker);
  }
  return max_id;
}

void add_stop_line_markers(
  visualization_msgs::msg::MarkerArray & debug_marker_array,
  const std::vector<StopPoint> & map_stop_points, const double z)
{
  auto debug_marker = get_base_marker();
  debug_marker.type = visualization_msgs::msg::Marker::LINE_LIST;
  debug_marker.ns = "ego_stop_lines";
  debug_marker.scale = universe_utils::createMarkerScale(0.2, 0.2, 0.2);
  debug_marker.color = universe_utils::createMarkerColor(0.5, 0.0, 0.7, 1.0);
  geometry_msgs::msg::Point p1;
  geometry_msgs::msg::Point p2;
  p1.z = p2.z = z;
  for (const auto & stop_point : map_stop_points) {
    for (auto i = 0UL; i + 1 < stop_point.stop_line.size(); ++i) {
      p1.x = stop_point.stop_line[i].x();
      p1.y = stop_point.stop_line[i].y();
      p2.x = stop_point.stop_line[i + 1].x();
      p2.y = stop_point.stop_line[i + 1].y();
      debug_marker.points.push_back(p1);
      debug_marker.points.push_back(p2);
    }
  }
  debug_marker_array.markers.push_back(debug_marker);
}

void add_out_lanelets(
  visualization_msgs::msg::MarkerArray & marker_array, visualization_msgs::msg::Marker base_marker,
  const lanelet::ConstLanelets & out_lanelets)
{
  lanelet::BasicPolygons2d drivable_lane_polygons;
  for (const auto & ll : out_lanelets) {
    drivable_lane_polygons.push_back(ll.polygon2d().basicPolygon());
  }
  base_marker.ns = "out_lanelets";
  base_marker.color = universe_utils::createMarkerColor(0.0, 0.0, 1.0, 1.0);
  add_polygons_markers(marker_array, base_marker, drivable_lane_polygons);
}

void add_out_of_lane_overlaps(
  visualization_msgs::msg::MarkerArray & marker_array, visualization_msgs::msg::Marker base_marker,
  const std::vector<OutOfLanePoint> & outside_points,
  const std::vector<autoware_planning_msgs::msg::TrajectoryPoint> & trajectory)
{
  lanelet::BasicPolygons2d out_of_lane_overlaps;
  lanelet::BasicPolygon2d out_of_lane_overlap;
  for (const auto & p : outside_points) {
    for (const auto & overlap : p.out_overlaps) {
      boost::geometry::convert(overlap, out_of_lane_overlap);
      out_of_lane_overlaps.push_back(out_of_lane_overlap);
    }
  }
  base_marker.ns = "out_of_lane_areas";
  base_marker.color = universe_utils::createMarkerColor(1.0, 0.0, 0.0, 1.0);
  add_polygons_markers(marker_array, base_marker, out_of_lane_overlaps);
  for (const auto & p : outside_points) {
    for (const auto & a : p.out_overlaps) {
      marker_array.markers.back().points.push_back(trajectory[p.trajectory_index].pose.position);
      const auto centroid = boost::geometry::return_centroid<lanelet::BasicPoint2d>(a);
      marker_array.markers.back().points.push_back(
        geometry_msgs::msg::Point().set__x(centroid.x()).set__y(centroid.y()));
    }
  }
}
void add_predicted_paths(
  visualization_msgs::msg::MarkerArray & marker_array, visualization_msgs::msg::Marker base_marker,
  const autoware_perception_msgs::msg::PredictedObjects & objects,
  const geometry_msgs::msg::Pose & ego_pose)
{
  base_marker.ns = "objects";
  base_marker.color = universe_utils::createMarkerColor(0.0, 1.0, 0.0, 1.0);
  lanelet::BasicPolygons2d object_polygons;
  constexpr double max_draw_distance = 50.0;
  for (const auto & o : objects.objects) {
    for (const auto & path : o.kinematics.predicted_paths) {
      for (const auto & pose : path.path) {
        // limit the draw distance to improve performance
        if (universe_utils::calcDistance2d(pose, ego_pose) < max_draw_distance) {
          const auto poly = universe_utils::toPolygon2d(pose, o.shape).outer();
          lanelet::BasicPolygon2d ll_poly(poly.begin(), poly.end());
          object_polygons.push_back(ll_poly);
        }
      }
    }
  }
  add_polygons_markers(marker_array, base_marker, object_polygons);
}
}  // namespace

visualization_msgs::msg::MarkerArray create_debug_marker_array(
  const EgoData & ego_data, const OutOfLaneData & out_of_lane_data,
  const autoware_perception_msgs::msg::PredictedObjects & objects, DebugData & debug_data)
{
  const auto z = ego_data.pose.position.z + 0.5;
  visualization_msgs::msg::MarkerArray debug_marker_array;

  auto base_marker = get_base_marker();
  base_marker.pose.position.z = z;
  base_marker.ns = "footprints";
  base_marker.color = universe_utils::createMarkerColor(1.0, 1.0, 1.0, 1.0);
  // TODO(Maxime): move the debug marker publishing AFTER the trajectory generation
  // disabled to prevent performance issues when publishing the debug markers
  // add_polygons_markers(debug_marker_array, base_marker, ego_data.trajectory_footprints);
  add_out_lanelets(debug_marker_array, base_marker, ego_data.out_lanelets);
  add_out_of_lane_overlaps(
    debug_marker_array, base_marker, out_of_lane_data.outside_points, ego_data.trajectory_points);
  add_predicted_paths(debug_marker_array, base_marker, objects, ego_data.pose);

  add_current_overlap_marker(debug_marker_array, ego_data.current_footprint, z);

  add_ttc_markers(debug_marker_array, ego_data, out_of_lane_data, debug_data.prev_ttcs);
  debug_data.prev_ttcs = out_of_lane_data.outside_points.size();

  debug_data.prev_stop_line = add_stop_line_markers(
    debug_marker_array, ego_data.stop_lines_rtree, z, debug_data.prev_stop_line);

  add_stop_line_markers(debug_marker_array, ego_data.map_stop_points, z);

  return debug_marker_array;
}

motion_utils::VirtualWalls create_virtual_walls(
  const geometry_msgs::msg::Pose & pose, const bool stop, const PlannerParam & params)
{
  motion_utils::VirtualWalls virtual_walls;
  motion_utils::VirtualWall wall;
  wall.text = "out_of_lane";
  wall.longitudinal_offset = params.front_offset;
  wall.style = stop ? motion_utils::VirtualWallType::stop : motion_utils::VirtualWallType::slowdown;
  wall.pose = pose;
  virtual_walls.push_back(wall);
  return virtual_walls;
}
}  // namespace autoware::motion_velocity_planner::out_of_lane::debug

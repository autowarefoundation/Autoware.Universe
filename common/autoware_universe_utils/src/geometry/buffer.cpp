// // Copyright 2024 TIER IV, Inc.
// //
// // Licensed under the Apache License, Version 2.0 (the "License");
// // you may not use this file except in compliance with the License.
// // You may obtain a copy of the License at
// //
// //     http://www.apache.org/licenses/LICENSE-2.0
// //
// // Unless required by applicable law or agreed to in writing, software
// // distributed under the License is distributed on an "AS IS" BASIS,
// // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// // See the License for the specific language governing permissions and
// // limitations under the License.

#include "autoware/universe_utils/geometry/buffer.hpp"

#include "autoware/universe_utils/geometry/temp_polygon_clip.hpp"
#include "autoware/universe_utils/system/stop_watch.hpp"

#include <iostream>
#include <vector>

namespace autoware::universe_utils
{

namespace offset_buffer
{

Polygon2d dissolve(const Polygon2d & polygon)
{
  StopWatch<std::chrono::nanoseconds, std::chrono::nanoseconds> sw;

  auto extended_poly = polygon_clip::create_extended_polygon(polygon);

  auto polygon_vector = polygon_clip::construct_self_intersecting_polygons(extended_poly);

  return polygon_vector;
}

void create_arc(
  Polygon2d & vertices, const Point2d & center, double radius, const Point2d & end_vertex,
  const Point2d & start_vertex_next, double segments)
{
  const double PI2 = M_PI * 2;
  double start_angle =
    atan2(start_vertex_next.y() - center.y(), start_vertex_next.x() - center.x());
  double end_angle = atan2(end_vertex.y() - center.y(), end_vertex.x() - center.x());

  if (start_angle < 0) start_angle += PI2;
  if (end_angle < 0) end_angle += PI2;

  double angle_diff =
    ((start_angle > end_angle) ? (start_angle - end_angle) : (start_angle + PI2 - end_angle));
  if (angle_diff < 0) angle_diff += PI2;

  int dynamic_segments = static_cast<int>(segments * (angle_diff / PI2));
  if (dynamic_segments < 1) dynamic_segments = 1;

  double segment_angle = angle_diff / dynamic_segments;

  for (int i = 0; i <= dynamic_segments; ++i) {
    double angle = end_angle + i * segment_angle;
    double x = center.x() + radius * cos(angle);
    double y = center.y() + radius * sin(angle);

    vertices.outer().push_back(Point2d(x, y));
  }
}

void offset_segment(
  Polygon2d & vertices, const Point2d & v1, const Point2d & v2, const Point2d & next_vertex,
  double dist, double segments)
{
  // Calculate direction and normals
  double dx = v2.x() - v1.x();
  double dy = v2.y() - v1.y();

  double length = std::sqrt(dx * dx + dy * dy);
  double normal_x = -dy / length;
  double normal_y = dx / length;

  Point2d offset_v1(v1.x() - normal_x * dist, v1.y() - normal_y * dist);
  Point2d offset_v2(v2.x() - normal_x * dist, v2.y() - normal_y * dist);

  double next_dx = next_vertex.x() - v2.x();
  double next_dy = next_vertex.y() - v2.y();
  double length_next = std::sqrt(next_dx * next_dx + next_dy * next_dy);
  double normal_x_next = -next_dy / length_next;
  double normal_y_next = next_dx / length_next;
  Point2d offset_v1next(v2.x() - normal_x_next * dist, v2.y() - normal_y_next * dist);
  Point2d offset_v2next(
    next_vertex.x() - normal_x_next * dist, next_vertex.y() - normal_y_next * dist);

  double current_angle = atan2(dy, dx);
  double next_angle = atan2(next_dy, next_dx);

  if (current_angle < 0) current_angle += M_PI * 2;
  if (next_angle < 0) next_angle += M_PI * 2;

  double angle_current_next =
    ((next_angle > current_angle) ? (next_angle - current_angle)
                                  : (next_angle + M_PI * 2 - current_angle));

  if (angle_current_next < 0) {
    angle_current_next += M_PI * 2;
  }

  if (angle_current_next < M_PI) {
    vertices.outer().push_back(offset_v1);
    create_arc(vertices, v2, dist, offset_v2, offset_v1next, segments);
  } else {
    vertices.outer().push_back(offset_v1);
    vertices.outer().push_back(offset_v2);
  }
}

}  // namespace offset_buffer

Polygon2d buffer(const Polygon2d & input_polygon, double dist, double segments)
{
  Polygon2d offset_polygon;
  Polygon2d final_polygon;
  size_t vertices_count = input_polygon.outer().size();

  if (vertices_count < 2) {
    std::cerr << "Polygon needs at least 2 vertices!" << std::endl;
    return offset_polygon;
  }

  std::vector<Point2d> new_ring(input_polygon.outer().begin(), input_polygon.outer().end() - 1);
  size_t modified_vertices_count = new_ring.size();

  for (size_t i = modified_vertices_count; i > 0; --i) {
    const auto & v1 = new_ring[(i - 1) % modified_vertices_count];
    const auto & v2 = new_ring[(i - 2 + modified_vertices_count) % modified_vertices_count];
    const auto & next_vertex =
      new_ring[(i - 3 + modified_vertices_count) % modified_vertices_count];

    offset_buffer::offset_segment(offset_polygon, v1, v2, next_vertex, dist, segments);
  }
  boost::geometry::correct(offset_polygon);
  final_polygon = offset_buffer::dissolve(offset_polygon);
  return final_polygon;
}

Polygon2d buffer(const Point2d & point, double distance, double segments)
{
  Polygon2d offset_polygon;

  for (int i = 0; i < segments; ++i) {
    double angle = 2 * M_PI * i / segments;
    Point2d buffer_point(point.x() + distance * cos(angle), point.y() + distance * sin(angle));
    offset_polygon.outer().push_back(buffer_point);
  }
  boost::geometry::correct(offset_polygon);
  return offset_polygon;
}

Polygon2d buffer(const MultiPoint2d & multi_point, double distance, double segments)
{
  Polygon2d buffer_union;
  bool first = true;
  for (const auto & point : multi_point) {
    Polygon2d buffer_point = buffer(point, distance, segments);
    if (!first) {
      buffer_union = union_(buffer_point, buffer_union)[0];
    } else {
      buffer_union = buffer_point;
      first = false;
    }
  }
  return buffer_union;
}

}  // namespace autoware::universe_utils

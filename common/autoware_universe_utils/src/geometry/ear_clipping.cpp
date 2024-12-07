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

#include "autoware/universe_utils/geometry/ear_clipping.hpp"

#include <algorithm>
#include <limits>
#include <vector>

namespace autoware::universe_utils
{

void remove_point(const std::size_t p_index, std::vector<LinkedPoint> & points)
{
  std::size_t prev_index = points[p_index].prev_index.value();
  std::size_t next_index = points[p_index].next_index.value();

  points[prev_index].next_index = next_index;
  points[next_index].prev_index = prev_index;
}

std::size_t get_leftmost(const std::size_t start_idx, const std::vector<LinkedPoint> & points)
{
  std::optional<std::size_t> p_idx = points[start_idx].next_index;
  std::size_t left_most_idx = start_idx;

  while (p_idx.has_value() && p_idx.value() != start_idx) {
    if (
      points[p_idx.value()].x() < points[left_most_idx].x() ||
      (points[p_idx.value()].x() == points[left_most_idx].x() &&
       points[p_idx.value()].y() < points[left_most_idx].y())) {
      left_most_idx = p_idx.value();
    }
    p_idx = points[p_idx.value()].next_index;
  }

  return left_most_idx;
}

bool point_in_triangle(
  const double ax, const double ay, const double bx, const double by, const double cx,
  const double cy, const double px, const double py)
{
  return (cx - px) * (ay - py) >= (ax - px) * (cy - py) &&
         (ax - px) * (by - py) >= (bx - px) * (ay - py) &&
         (bx - px) * (cy - py) >= (cx - px) * (by - py);
}

double area(
  const std::vector<LinkedPoint> & points, const std::size_t p_idx, const std::size_t q_idx,
  const std::size_t r_idx)
{
  const LinkedPoint & p = points[p_idx];
  const LinkedPoint & q = points[q_idx];
  const LinkedPoint & r = points[r_idx];
  return (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
}

bool middle_inside(
  const std::size_t a_idx, const std::size_t b_idx, const std::vector<LinkedPoint> & points)
{
  std::optional<std::size_t> p_idx = a_idx;
  bool inside = false;
  double px = (points[a_idx].x() + points[b_idx].x()) / 2;
  double py = (points[a_idx].y() + points[b_idx].y()) / 2;
  std::size_t start_idx = a_idx;

  while (p_idx.has_value()) {
    std::size_t current_idx = p_idx.value();
    std::size_t next_idx = points[current_idx].next_index.value();

    if (
      ((points[current_idx].y() > py) != (points[next_idx].y() > py)) &&
      points[next_idx].y() != points[current_idx].y() &&
      (px < (points[next_idx].x() - points[current_idx].x()) * (py - points[current_idx].y()) /
                (points[next_idx].y() - points[current_idx].y()) +
              points[current_idx].x())) {
      inside = !inside;
    }

    if (next_idx == start_idx) {
      break;  // Break the loop if we have cycled back to the start index
    }

    p_idx = next_idx;
  }

  return inside;
}

bool equals(
  const std::size_t p1_idx, const std::size_t p2_idx, const std::vector<LinkedPoint> & points)
{
  return points[p1_idx].x() == points[p2_idx].x() && points[p1_idx].y() == points[p2_idx].y();
}

int sign(const double val)
{
  return (0.0 < val) - (val < 0.0);
}

bool on_segment(
  const std::vector<LinkedPoint> & points, const std::size_t p_idx, const std::size_t q_idx,
  const std::size_t r_idx)
{
  const LinkedPoint & p = points[p_idx];
  const LinkedPoint & q = points[q_idx];
  const LinkedPoint & r = points[r_idx];
  return q.x() <= std::max<double>(p.x(), r.x()) && q.x() >= std::min<double>(p.x(), r.x()) &&
         q.y() <= std::max<double>(p.y(), r.y()) && q.y() >= std::min<double>(p.y(), r.y());
}

bool locally_inside(
  const std::size_t a_idx, const std::size_t b_idx, const std::vector<LinkedPoint> & points)
{
  const auto & prev_idx = points[a_idx].prev_index;
  const auto & next_idx = points[a_idx].next_index;

  if (!prev_idx.has_value() || !next_idx.has_value()) {
    return false;
  }

  double area_prev = area(points, prev_idx.value(), a_idx, next_idx.value());
  double area_a_b_next = area(points, a_idx, b_idx, next_idx.value());
  double area_a_prev_b = area(points, a_idx, prev_idx.value(), b_idx);
  double area_a_b_prev = area(points, a_idx, b_idx, prev_idx.value());
  double area_a_next_b = area(points, a_idx, next_idx.value(), b_idx);

  return area_prev < 0 ? area_a_b_next >= 0 && area_a_prev_b >= 0
                       : area_a_b_prev < 0 || area_a_next_b < 0;
}

bool intersects(
  const std::size_t p1_idx, const std::size_t q1_idx, const std::size_t p2_idx,
  const std::size_t q2_idx, const std::vector<LinkedPoint> & points)
{
  int o1 = sign(area(points, p1_idx, q1_idx, p2_idx));
  int o2 = sign(area(points, p1_idx, q1_idx, q2_idx));
  int o3 = sign(area(points, p2_idx, q2_idx, p1_idx));
  int o4 = sign(area(points, p2_idx, q2_idx, q1_idx));

  if (o1 != o2 && o3 != o4) return true;

  if (o1 == 0 && on_segment(points, p1_idx, p2_idx, q1_idx)) return true;
  if (o2 == 0 && on_segment(points, p1_idx, q2_idx, q1_idx)) return true;
  if (o3 == 0 && on_segment(points, p2_idx, p1_idx, q2_idx)) return true;
  if (o4 == 0 && on_segment(points, p2_idx, q1_idx, q2_idx)) return true;

  return false;
}

bool intersects_polygon(
  const std::vector<LinkedPoint> & points, const std::size_t a_idx, const std::size_t b_idx)
{
  std::size_t p_idx = a_idx;
  std::optional<std::size_t> p_next_opt = points[p_idx].next_index;

  while (p_next_opt.has_value() && p_next_opt.value() != a_idx) {
    std::size_t p_next_idx = p_next_opt.value();

    if (p_idx != a_idx && p_next_idx != a_idx && p_idx != b_idx && p_next_idx != b_idx) {
      if (intersects(p_idx, p_next_idx, a_idx, b_idx, points)) {
        return true;
      }
    }

    p_idx = p_next_idx;
    p_next_opt = points[p_idx].next_index;
  }

  return false;
}

bool is_valid_diagonal(
  const std::size_t a_idx, const std::size_t b_idx, const std::vector<LinkedPoint> & points)
{
  if (
    !points[a_idx].next_index.has_value() || !points[a_idx].prev_index.has_value() ||
    !points[b_idx].next_index.has_value() || !points[b_idx].prev_index.has_value()) {
    return false;
  }

  std::size_t a_next_idx = points[a_idx].next_index.value();
  std::size_t a_prev_idx = points[a_idx].prev_index.value();
  std::size_t b_next_idx = points[b_idx].next_index.value();
  std::size_t b_prev_idx = points[b_idx].prev_index.value();

  if (a_next_idx == b_idx || a_prev_idx == b_idx || intersects_polygon(points, a_idx, b_idx)) {
    return false;
  }

  bool is_locally_inside_ab = locally_inside(a_idx, b_idx, points);
  bool is_locally_inside_ba = locally_inside(b_idx, a_idx, points);
  bool is_middle_inside = middle_inside(a_idx, b_idx, points);

  bool is_valid_diagonal =
    (is_locally_inside_ab && is_locally_inside_ba && is_middle_inside &&
     (area(points, a_prev_idx, a_idx, b_prev_idx) != 0.0 ||
      area(points, a_idx, b_prev_idx, b_idx) != 0.0)) ||
    (equals(a_idx, b_idx, points) && area(points, a_prev_idx, a_idx, a_next_idx) > 0 &&
     area(points, b_prev_idx, b_idx, b_next_idx) > 0);

  return is_valid_diagonal;
}

std::size_t insert_point(
  const alt::Point2d & pt, std::vector<LinkedPoint> & points,
  const std::optional<std::size_t> last_index)
{
  std::size_t p_idx = points.size();
  points.push_back(LinkedPoint(pt));

  // Making sure all next_index and prev_index will always have values
  if (!last_index.has_value()) {
    points[p_idx].prev_index = p_idx;
    points[p_idx].next_index = p_idx;
  } else {
    std::size_t last = last_index.value();
    std::size_t next = points[last].next_index.value();
    points[p_idx].prev_index = last;
    points[p_idx].next_index = next;
    points[last].next_index = p_idx;
    if (next != p_idx) {
      points[next].prev_index = p_idx;
    }
  }

  return p_idx;
}

std::size_t linked_list(
  const alt::PointList2d & ring, const bool forward, std::size_t & vertices,
  std::vector<LinkedPoint> & points)
{
  const std::size_t len = ring.size();
  std::optional<std::size_t> last_index = std::nullopt;

  // create forward linked list if forward is true and ring is counter-clockwise, or
  //                               forward is false and ring is clockwise
  // create reverse linked list if forward is true and ring is clockwise, or
  //                               forward is false and ring is counter-clockwise
  if (forward == !is_clockwise(ring)) {
    for (auto it = ring.begin(); it != ring.end(); ++it) {
      last_index = insert_point(*it, points, last_index);
    }
  } else {
    for (auto it = ring.rbegin(); it != ring.rend(); ++it) {
      last_index = insert_point(*it, points, last_index);
    }
  }

  if (last_index.has_value()) {
    std::size_t last_idx_value = last_index.value();
    std::optional<std::size_t> next_index = points[last_idx_value].next_index;

    if (next_index.has_value() && equals(last_idx_value, next_index.value(), points)) {
      std::size_t next_idx_value = next_index.value();
      remove_point(last_idx_value, points);
      last_index = next_idx_value;
    }
  }

  vertices += len;
  return last_index.value();
}

bool sector_contains_sector(
  const std::size_t m_idx, const std::size_t p_idx, const std::vector<LinkedPoint> & points)
{
  if (!points[m_idx].prev_index.has_value() || !points[m_idx].next_index.has_value()) {
    return false;
  }

  std::size_t m_prev_idx = points[m_idx].prev_index.value();
  std::size_t m_next_idx = points[m_idx].next_index.value();

  return area(points, m_prev_idx, m_idx, p_idx) < 0 && area(points, p_idx, m_next_idx, m_idx) < 0;
}

std::size_t find_hole_bridge(
  const std::size_t hole_index, const std::size_t outer_point_index,
  const std::vector<LinkedPoint> & points)
{
  std::size_t p = outer_point_index;
  double hx = points[hole_index].x();
  double hy = points[hole_index].y();
  double qx = -std::numeric_limits<double>::infinity();
  std::optional<std::size_t> bridge_index = std::nullopt;
  std::size_t next_index = points[p].next_index.value();

  while (p != outer_point_index) {
    if (
      hy <= points[p].y() && hy >= points[next_index].y() &&
      points[next_index].y() != points[p].y()) {
      double x = points[p].x() + (hy - points[p].y()) * (points[next_index].x() - points[p].x()) /
                                   (points[next_index].y() - points[p].y());
      if (x <= hx && x > qx) {
        qx = x;
        bridge_index = (points[p].x() < points[next_index].x()) ? p : next_index;
        if (x == hx) return bridge_index.value();
      }
    }
    p = next_index;
    next_index = points[p].next_index.value();
  }

  if (!bridge_index.has_value()) return outer_point_index;

  const std::size_t stop = bridge_index.value();
  double min_tan = std::numeric_limits<double>::infinity();

  p = bridge_index.value();
  double mx = points[p].x();
  double my = points[p].y();
  next_index = points[p].next_index.value();

  while (p != stop) {
    if (
      hx >= points[p].x() && points[p].x() >= mx && hx != points[p].x() &&
      point_in_triangle(
        hy < my ? hx : qx, hy, mx, my, hy < my ? qx : hx, hy, points[p].x(), points[p].y())) {
      double current_tan = std::abs(hy - points[p].y()) / (hx - points[p].x());

      if (
        locally_inside(p, hole_index, points) &&
        (current_tan < min_tan ||
         (current_tan == min_tan && (points[p].x() > points[bridge_index.value()].x() ||
                                     sector_contains_sector(bridge_index.value(), p, points))))) {
        bridge_index = p;
        min_tan = current_tan;
      }
    }

    p = next_index;
    next_index = points[p].next_index.value();
  }

  return bridge_index.value();
}

std::size_t split_polygon(
  std::size_t a_index, std::size_t b_index, std::vector<LinkedPoint> & points)
{
  std::size_t an_idx = points[a_index].next_index.value();
  std::size_t bp_idx = points[b_index].prev_index.value();

  std::size_t a2_idx = points.size();
  std::size_t b2_idx = points.size() + 1;
  points.push_back(points[a_index]);
  points.push_back(points[b_index]);

  points[a_index].next_index = b_index;
  points[a2_idx].prev_index = b2_idx;
  points[a2_idx].next_index = an_idx;

  points[b_index].prev_index = a_index;
  points[an_idx].prev_index = b2_idx;
  points[b2_idx].next_index = a2_idx;
  points[b2_idx].prev_index = bp_idx;

  if (bp_idx != b_index) {
    points[bp_idx].next_index = b2_idx;
  }

  return b2_idx;
}

std::size_t filter_points(
  const std::size_t start_index, const std::size_t end_index, std::vector<LinkedPoint> & points)
{
  auto p = start_index;
  bool again = true;

  while (again && p != end_index) {
    again = false;

    if (
      !points[p].steiner &&
      (equals(p, points[p].next_index.value(), points) ||
       area(points, points[p].prev_index.value(), p, points[p].next_index.value()) == 0)) {
      remove_point(p, points);
      p = points[p].prev_index.value();

      if (p == points[p].next_index.value()) {
        break;
      }
      again = true;
    } else {
      p = points[p].next_index.value();
    }
  }

  return end_index;
}

std::size_t eliminate_hole(
  const std::size_t hole_index, const std::size_t outer_index, std::vector<LinkedPoint> & points)
{
  auto bridge = find_hole_bridge(hole_index, outer_index, points);
  auto bridge_reverse = split_polygon(bridge, hole_index, points);

  auto next_index_bridge_reverse = points[bridge_reverse].next_index.value();
  filter_points(bridge_reverse, next_index_bridge_reverse, points);

  auto next_index_bridge = points[bridge].next_index.value();
  return filter_points(bridge, next_index_bridge, points);
}

std::size_t eliminate_holes(
  const std::vector<alt::PointList2d> & inners, std::size_t outer_index, std::size_t & vertices,
  std::vector<LinkedPoint> & points)
{
  std::vector<std::size_t> queue;

  for (const auto & ring : inners) {
    if (ring.empty()) {
      continue;
    }
    auto inner_index = linked_list(ring, false, vertices, points);

    if (points[inner_index].next_index.value() == inner_index) {
      points[inner_index].steiner = true;
    }

    queue.push_back(get_leftmost(inner_index, points));
  }

  std::sort(queue.begin(), queue.end(), [&](std::size_t a, std::size_t b) {
    return points[a].x() < points[b].x();
  });

  for (const auto & q : queue) {
    outer_index = eliminate_hole(q, outer_index, points);
  }

  return outer_index;
}

bool is_ear(const std::size_t ear_index, const std::vector<LinkedPoint> & points)
{
  const auto a_index = points[ear_index].prev_index.value();
  const auto b_index = ear_index;
  const auto c_index = points[ear_index].next_index.value();

  const auto a = points[a_index];
  const auto b = points[b_index];
  const auto c = points[c_index];

  if (area(points, a_index, b_index, c_index) >= 0) return false;
  auto p_index = points[c_index].next_index.value();
  while (p_index != a_index) {
    const auto p = points[p_index];
    if (
      point_in_triangle(a.x(), a.y(), b.x(), b.y(), c.x(), c.y(), p.x(), p.y()) &&
      area(points, p.prev_index.value(), p_index, p.next_index.value()) >= 0) {
      return false;
    }
    p_index = points[p_index].next_index.value();
  }

  return true;
}

std::size_t cure_local_intersections(
  std::size_t start_index, std::vector<std::size_t> & indices, std::vector<LinkedPoint> & points)
{
  auto p = start_index;
  bool updated = false;

  while (p != start_index || updated) {
    updated = false;
    auto a_idx = points[p].prev_index.value();
    auto b_idx = points[points[p].next_index.value()].next_index.value();

    if (
      !equals(a_idx, b_idx, points) &&
      intersects(
        a_idx, p, points[points[p].next_index.value()].next_index.value(), b_idx, points) &&
      locally_inside(a_idx, b_idx, points) && locally_inside(b_idx, a_idx, points)) {
      indices.push_back(a_idx);
      indices.push_back(p);
      indices.push_back(b_idx);

      remove_point(p, points);
      remove_point(points[p].next_index.value(), points);

      p = start_index = b_idx;
      updated = true;
    } else {
      p = points[p].next_index.value();
    }
  }

  return filter_points(p, p, points);
}

void split_ear_clipping(
  std::vector<LinkedPoint> & points, const std::size_t start_idx,
  std::vector<std::size_t> & indices)
{
  std::size_t a_idx = start_idx;
  do {
    std::size_t b_idx = points[points[a_idx].next_index.value()].next_index.value();
    while (b_idx != points[a_idx].prev_index.value()) {
      if (a_idx != b_idx && is_valid_diagonal(a_idx, b_idx, points)) {
        std::size_t c_idx = split_polygon(a_idx, b_idx, points);

        a_idx = filter_points(start_idx, points[a_idx].next_index.value(), points);
        c_idx = filter_points(start_idx, points[c_idx].next_index.value(), points);

        ear_clipping_linked(a_idx, indices, points);
        ear_clipping_linked(c_idx, indices, points);
        return;
      }
      b_idx = points[b_idx].next_index.value();
    }
    a_idx = points[a_idx].next_index.value();
  } while (a_idx != start_idx);
}

void ear_clipping_linked(
  std::size_t ear_index, std::vector<std::size_t> & indices, std::vector<LinkedPoint> & points,
  const int pass)
{
  auto stop = ear_index;
  std::optional<std::size_t> next = std::nullopt;

  while (points[ear_index].prev_index.value() != points[ear_index].next_index.value()) {
    next = points[ear_index].next_index;

    if (is_ear(ear_index, points)) {
      indices.push_back(points[ear_index].prev_index.value());
      indices.push_back(ear_index);
      indices.push_back(next.value());

      remove_point(ear_index, points);

      ear_index = points[next.value()].next_index.value();
      stop = points[next.value()].next_index.value();
      continue;
    }

    ear_index = next.value();

    if (ear_index == stop) {
      if (pass == 0) {
        ear_clipping_linked(filter_points(ear_index, ear_index, points), indices, points, 1);
      } else if (pass == 1) {
        ear_index =
          cure_local_intersections(filter_points(ear_index, ear_index, points), indices, points);
        ear_clipping_linked(ear_index, indices, points, 2);
      } else if (pass == 2) {
        split_ear_clipping(points, ear_index, indices);
      }
      break;
    }
  }
}

std::vector<LinkedPoint> perform_triangulation(
  const alt::Polygon2d & polygon, std::vector<std::size_t> & indices)
{
  indices.clear();
  std::vector<LinkedPoint> points;
  std::size_t vertices = 0;
  const auto & outer_ring = polygon.outer();
  std::size_t len = outer_ring.size();
  points.reserve(len * 3 / 2);

  if (polygon.outer().empty()) return points;

  indices.reserve(len + outer_ring.size());
  auto outer_point_index = linked_list(outer_ring, true, vertices, points);
  if (
    !points[outer_point_index].prev_index.has_value() ||
    outer_point_index == points[outer_point_index].prev_index.value()) {
    return points;
  }

  if (!polygon.inners().empty()) {
    outer_point_index = eliminate_holes(polygon.inners(), outer_point_index, vertices, points);
  }

  ear_clipping_linked(outer_point_index, indices, points);
  return points;
}

std::vector<alt::ConvexPolygon2d> triangulate(const alt::Polygon2d & poly)
{
  std::vector<std::size_t> indices;
  auto points = perform_triangulation(poly, indices);

  std::vector<alt::ConvexPolygon2d> triangles;
  const std::size_t num_indices = indices.size();

  if (num_indices % 3 != 0) {
    throw std::runtime_error("Indices size should be a multiple of 3");
  }

  for (std::size_t i = 0; i < num_indices; i += 3) {
    alt::PointList2d vertices;
    vertices.push_back(points[indices[i]].pt);
    vertices.push_back(points[indices[i + 1]].pt);
    vertices.push_back(points[indices[i + 2]].pt);
    vertices.push_back(points[indices[i]].pt);

    triangles.push_back(alt::ConvexPolygon2d::create(vertices).value());
  }
  points.clear();
  return triangles;
}

std::vector<Polygon2d> triangulate(const Polygon2d & poly)
{
  const auto alt_poly = alt::Polygon2d::create(poly);
  const auto alt_triangles = triangulate(alt_poly.value());
  std::vector<Polygon2d> triangles;
  for (const auto & alt_triangle : alt_triangles) {
    triangles.push_back(alt_triangle.to_boost());
  }
  return triangles;
}
}  // namespace autoware::universe_utils

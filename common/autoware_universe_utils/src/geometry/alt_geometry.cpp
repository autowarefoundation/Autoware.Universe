// Copyright 2023-2024 TIER IV, Inc.
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

#include "autoware/universe_utils/geometry/alt_geometry.hpp"

namespace autoware::universe_utils
{
// Alternatives for Boost.Geometry ----------------------------------------------------------------
namespace alt
{
std::optional<Polygon2d> Polygon2d::create(
  const PointList2d & outer, const std::vector<PointList2d> & inners) noexcept
{
  Polygon2d poly(outer, inners);
  correct(poly);

  if (poly.outer().size() < 4) {
    return std::nullopt;
  }

  for (const auto & inner : poly.inners()) {
    if (inner.size() < 4) {
      return std::nullopt;
    }
  }

  return poly;
}

std::optional<Polygon2d> Polygon2d::create(
  PointList2d && outer, std::vector<PointList2d> && inners) noexcept
{
  Polygon2d poly(std::move(outer), std::move(inners));
  correct(poly);

  if (poly.outer().size() < 4) {
    return std::nullopt;
  }

  for (const auto & inner : poly.inners()) {
    if (inner.size() < 4) {
      return std::nullopt;
    }
  }

  return poly;
}

std::optional<Polygon2d> Polygon2d::create(
  const autoware::universe_utils::Polygon2d & polygon) noexcept
{
  PointList2d outer;
  for (const auto & point : polygon.outer()) {
    outer.push_back(Point2d(point));
  }

  std::vector<PointList2d> inners;
  for (const auto & inner : polygon.inners()) {
    PointList2d _inner;
    for (const auto & point : inner) {
      _inner.push_back(Point2d(point));
    }
    inners.push_back(_inner);
  }

  return Polygon2d::create(outer, inners);
}

std::optional<ConvexPolygon2d> ConvexPolygon2d::create(const PointList2d & vertices) noexcept
{
  ConvexPolygon2d poly(vertices);
  correct(poly);

  if (poly.vertices().size() < 4) {
    return std::nullopt;
  }

  if (!is_convex(poly)) {
    return std::nullopt;
  }

  return poly;
}

std::optional<ConvexPolygon2d> ConvexPolygon2d::create(PointList2d && vertices) noexcept
{
  ConvexPolygon2d poly(std::move(vertices));
  correct(poly);

  if (poly.vertices().size() < 4) {
    return std::nullopt;
  }

  if (!is_convex(poly)) {
    return std::nullopt;
  }

  return poly;
}

std::optional<ConvexPolygon2d> ConvexPolygon2d::create(
  const autoware::universe_utils::Polygon2d & polygon) noexcept
{
  PointList2d vertices;
  for (const auto & point : polygon.outer()) {
    vertices.push_back(Point2d(point));
  }

  return ConvexPolygon2d::create(vertices);
}
}  // namespace alt

double area(const alt::ConvexPolygon2d & poly)
{
  const auto & vertices = poly.vertices();

  double area = 0.;
  for (auto it = std::next(vertices.cbegin()); it != std::prev(vertices.cend(), 2); ++it) {
    area += (*std::next(it) - vertices.front()).cross(*it - vertices.front()) / 2;
  }

  return area;
}
/*
alt::Polygon2d buffer(
  const alt::Points2d & points, const double left_dist, const double right_dist, const int strategy)
{
  constexpr std::size_t points_per_circle = 90;
  const bool join_round = strategy & BufferStrategy::JOIN_ROUND;
  const bool end_round = strategy & BufferStrategy::END_ROUND;
  const bool point_circle = strategy & BufferStrategy::POINT_CIRCLE;
  alt::Polygon2d buffered_poly;
  if (points.size() == 1) {
    const double dist = std::max(left_dist, right_dist);
    if (point_circle) {
      for (std::size_t i = 0; i < points_per_circle; ++i) {
        const double theta = 2 * M_PI * i / points_per_circle;
        buffered_poly.push_back(
          points.front() + dist * alt::Point2d{std::cos(theta), std::sin(theta)});
      }
    } else {
      const std::vector<alt::Point2d> vertices = {
        {-dist, -dist}, {dist, -dist}, {dist, dist}, {-dist, dist}};
      for (const auto & vertex : vertices) {
        buffered_poly.push_back(points.front() + vertex);
      }
    }
  } else {
    for (std::size_t i = 0; i < points.size(); ++i) {
      const auto & p1 = points[i];
      const auto & p2 = points[(i + 1) % points.size()];
      const auto v = (p2 - p1).normalized();
      const auto n = v.vector_triple(alt::Vector2d{0, 0}, 1);
      alt::Points2d buffer_points;
      const std::vector<alt::Point2d> square_vertices = {
        p1 - left_dist * n, p2 - left_dist * n, p2 + right_dist * n, p1 + right_dist * n};
      for (const auto & vertex : square_vertices) {
        buffer_points.push_back(vertex);
      }
      auto draw_arc = [](const auto & arc_center, const auto start_angle) {
        const auto radius = (right_dist + left_dist) / 2;
        for (std::size_t j = 0; j < points_per_circle; ++j) {
          const double theta = start_angle + M_PI * j / points_per_circle;
          buffer_points.push_back(
            arc_center + radius * alt::Point2d{std::cos(theta), std::sin(theta)});
        }
      };
      if ((i > 0 && join_round) || end_round) {
        const auto arc_center = p1 + (right_dist - left_dist) * n;
        const auto start_angle = std::atan2(-n.y(), -n.x());
        draw_arc(arc_center, start_angle);
      }
      if ((i < points.size() - 1 && join_round) || end_round) {
        const auto arc_center = p2 + (right_dist - left_dist) * n;
        const auto start_angle = std::atan2(n.y(), n.x());
        draw_arc(arc_center, start_angle);
      }
      buffered_poly = union_(buffered_poly, alt::ConvexPolygon2d{buffer_points});
    }
  }
  return buffered_poly;
}
*/
std::optional<alt::ConvexPolygon2d> convex_hull(const alt::Points2d & points)
{
  if (points.size() < 3) {
    return std::nullopt;
  }

  // QuickHull algorithm

  const auto p_minmax_itr = std::minmax_element(
    points.begin(), points.end(), [](const auto & a, const auto & b) { return a.x() < b.x(); });
  const auto & p_min = *p_minmax_itr.first;
  const auto & p_max = *p_minmax_itr.second;

  alt::PointList2d vertices;

  if (points.size() == 3) {
    std::rotate_copy(
      points.begin(), p_minmax_itr.first, points.end(), std::back_inserter(vertices));
  } else {
    auto make_hull = [&vertices](
                       auto self, const alt::Point2d & p1, const alt::Point2d & p2,
                       const alt::Points2d & points) {
      if (points.empty()) {
        return;
      }

      const auto & farthest = *find_farthest(points, p1, p2);

      alt::Points2d subset1, subset2;
      for (const auto & point : points) {
        if (is_above(point, p1, farthest)) {
          subset1.push_back(point);
        } else if (is_above(point, farthest, p2)) {
          subset2.push_back(point);
        }
      }

      self(self, p1, farthest, subset1);
      vertices.push_back(farthest);
      self(self, farthest, p2, subset2);
    };

    alt::Points2d above_points, below_points;
    for (const auto & point : points) {
      if (is_above(point, p_min, p_max)) {
        above_points.push_back(point);
      } else if (is_above(point, p_max, p_min)) {
        below_points.push_back(point);
      }
    }

    vertices.push_back(p_min);
    make_hull(make_hull, p_min, p_max, above_points);
    vertices.push_back(p_max);
    make_hull(make_hull, p_max, p_min, below_points);
  }

  auto hull = alt::ConvexPolygon2d::create(vertices);
  if (!hull) {
    return std::nullopt;
  }

  return hull;
}

void correct(alt::Polygon2d & poly)
{
  auto correct_vertices = [](alt::PointList2d & vertices) {
    // remove adjacent duplicate points
    const auto it = std::unique(
      vertices.begin(), vertices.end(),
      [](const auto & a, const auto & b) { return equals(a, b); });
    vertices.erase(it, vertices.end());

    if (!equals(vertices.front(), vertices.back())) {
      vertices.push_back(vertices.front());
    }

    if (!is_clockwise(vertices)) {
      std::reverse(std::next(vertices.begin()), std::prev(vertices.end()));
    }
  };

  correct_vertices(poly.outer());
  for (auto & inner : poly.inners()) {
    correct_vertices(inner);
  }
}

bool covered_by(const alt::Point2d & point, const alt::ConvexPolygon2d & poly)
{
  constexpr double epsilon = 1e-6;

  const auto & vertices = poly.vertices();
  std::size_t winding_number = 0;

  const auto [y_min_vertex, y_max_vertex] = std::minmax_element(
    vertices.begin(), std::prev(vertices.end()),
    [](const auto & a, const auto & b) { return a.y() < b.y(); });
  if (point.y() < y_min_vertex->y() || point.y() > y_max_vertex->y()) {
    return false;
  }

  double cross;
  for (auto it = vertices.cbegin(); it != std::prev(vertices.cend()); ++it) {
    const auto & p1 = *it;
    const auto & p2 = *std::next(it);

    if (p1.y() <= point.y() && p2.y() >= point.y()) {  // upward edge
      cross = (p2 - p1).cross(point - p1);
      if (cross > 0) {  // point is to the left of edge
        winding_number++;
        continue;
      }
    } else if (p1.y() >= point.y() && p2.y() <= point.y()) {  // downward edge
      cross = (p2 - p1).cross(point - p1);
      if (cross < 0) {  // point is to the left of edge
        winding_number--;
        continue;
      }
    } else {
      continue;
    }

    if (std::abs(cross) < epsilon) {  // point is on edge
      return true;
    }
  }

  return winding_number != 0;
}

bool disjoint(const alt::ConvexPolygon2d & poly1, const alt::ConvexPolygon2d & poly2)
{
  if (equals(poly1, poly2)) {
    return false;
  }

  if (intersects(poly1, poly2)) {
    return false;
  }

  for (const auto & vertex : poly1.vertices()) {
    if (touches(vertex, poly2)) {
      return false;
    }
  }

  return true;
}

double distance(
  const alt::Point2d & point, const alt::Point2d & seg_start, const alt::Point2d & seg_end)
{
  constexpr double epsilon = 1e-3;

  const auto seg_vec = seg_end - seg_start;
  const auto point_vec = point - seg_start;

  const double seg_vec_norm = seg_vec.norm();
  const double seg_point_dot = seg_vec.dot(point_vec);

  if (seg_vec_norm < epsilon || seg_point_dot < 0) {
    return point_vec.norm();
  } else if (seg_point_dot > std::pow(seg_vec_norm, 2)) {
    return (point - seg_end).norm();
  } else {
    return std::abs(seg_vec.cross(point_vec)) / seg_vec_norm;
  }
}

double distance(const alt::Point2d & point, const alt::ConvexPolygon2d & poly)
{
  if (covered_by(point, poly)) {
    return 0.0;
  }

  // TODO(mitukou1109): Use plane sweep method to improve performance?
  const auto & vertices = poly.vertices();
  double min_distance = std::numeric_limits<double>::max();
  for (auto it = vertices.cbegin(); it != std::prev(vertices.cend()); ++it) {
    min_distance = std::min(min_distance, distance(point, *it, *std::next(it)));
  }

  return min_distance;
}

std::optional<alt::ConvexPolygon2d> envelope(const alt::Polygon2d & poly)
{
  const auto [x_min_vertex, x_max_vertex] = std::minmax_element(
    poly.outer().begin(), std::prev(poly.outer().end()),
    [](const auto & a, const auto & b) { return a.x() < b.x(); });

  const auto [y_min_vertex, y_max_vertex] = std::minmax_element(
    poly.outer().begin(), std::prev(poly.outer().end()),
    [](const auto & a, const auto & b) { return a.y() < b.y(); });

  return alt::ConvexPolygon2d::create(alt::PointList2d{
    {x_min_vertex->x(), y_min_vertex->y()},
    {x_min_vertex->x(), y_max_vertex->y()},
    {x_max_vertex->x(), y_max_vertex->y()},
    {x_max_vertex->x(), y_min_vertex->y()}});
}

bool equals(const alt::Point2d & point1, const alt::Point2d & point2)
{
  constexpr double epsilon = 1e-3;
  return std::abs(point1.x() - point2.x()) < epsilon && std::abs(point1.y() - point2.y()) < epsilon;
}

bool equals(const alt::Polygon2d & poly1, const alt::Polygon2d & poly2)
{
  const auto outer_equals = std::equal(
    poly1.outer().begin(), std::prev(poly1.outer().end()), poly2.outer().begin(),
    std::prev(poly2.outer().end()), [](const auto & a, const auto & b) { return equals(a, b); });

  auto inners_equal = true;
  for (const auto & inner1 : poly1.inners()) {
    inners_equal &=
      std::any_of(poly2.inners().begin(), poly2.inners().end(), [&](const auto & inner2) {
        return std::equal(
          inner1.begin(), std::prev(inner1.end()), inner2.begin(), std::prev(inner2.end()),
          [](const auto & a, const auto & b) { return equals(a, b); });
      });
  }

  return outer_equals && inners_equal;
}

alt::Points2d::const_iterator find_farthest(
  const alt::Points2d & points, const alt::Point2d & seg_start, const alt::Point2d & seg_end)
{
  const auto seg_vec = seg_end - seg_start;
  return std::max_element(points.begin(), points.end(), [&](const auto & a, const auto & b) {
    return std::abs(seg_vec.cross(a - seg_start)) < std::abs(seg_vec.cross(b - seg_start));
  });
}

bool intersects(
  const alt::Point2d & seg1_start, const alt::Point2d & seg1_end, const alt::Point2d & seg2_start,
  const alt::Point2d & seg2_end)
{
  constexpr double epsilon = 1e-6;

  const auto v1 = seg1_end - seg1_start;
  const auto v2 = seg2_end - seg2_start;

  const auto det = v1.cross(v2);
  if (std::abs(det) < epsilon) {
    return false;
  }

  const auto v12 = seg2_end - seg1_end;
  const double t = v2.cross(v12) / det;
  const double s = v1.cross(v12) / det;
  if (t < 0 || 1 < t || s < 0 || 1 < s) {
    return false;
  }

  return true;
}

bool intersects(const alt::ConvexPolygon2d & poly1, const alt::ConvexPolygon2d & poly2)
{
  if (equals(poly1, poly2)) {
    return true;
  }

  // GJK algorithm

  auto find_support_vector = [](
                               const alt::ConvexPolygon2d & poly1,
                               const alt::ConvexPolygon2d & poly2,
                               const alt::Vector2d & direction) {
    auto find_farthest_vertex =
      [](const alt::ConvexPolygon2d & poly, const alt::Vector2d & direction) {
        return std::max_element(
          poly.vertices().begin(), std::prev(poly.vertices().end()),
          [&](const auto & a, const auto & b) { return direction.dot(a) <= direction.dot(b); });
      };
    return *find_farthest_vertex(poly1, direction) - *find_farthest_vertex(poly2, -direction);
  };

  alt::Vector2d direction = {1.0, 0.0};
  auto a = find_support_vector(poly1, poly2, direction);
  direction = -a;
  auto b = find_support_vector(poly1, poly2, direction);
  if (b.dot(direction) <= 0.0) {
    return false;
  }

  direction = (b - a).vector_triple(-a, b - a);
  while (true) {
    auto c = find_support_vector(poly1, poly2, direction);
    if (c.dot(direction) <= 0.0) {
      return false;
    }

    auto n_ca = (b - c).vector_triple(a - c, a - c);
    if (n_ca.dot(-c) > 0.0) {
      b = c;
      direction = n_ca;
    } else {
      auto n_cb = (a - c).vector_triple(b - c, b - c);
      if (n_cb.dot(-c) > 0.0) {
        a = c;
        direction = n_cb;
      } else {
        break;
      }
    }
  }

  return true;
}

bool is_above(
  const alt::Point2d & point, const alt::Point2d & seg_start, const alt::Point2d & seg_end)
{
  return (seg_end - seg_start).cross(point - seg_start) > 0;
}

bool is_clockwise(const alt::PointList2d & vertices)
{
  double sum = 0.;
  for (auto it = vertices.cbegin(); it != std::prev(vertices.cend()); ++it) {
    sum += (std::next(it)->x() - it->x()) * (std::next(it)->y() + it->y());
  }

  return sum > 0;
}

bool is_convex(const alt::Polygon2d & poly)
{
  constexpr double epsilon = 1e-6;

  if (!poly.inners().empty()) {
    return false;
  }

  const auto & outer = poly.outer();

  for (auto it = std::next(outer.cbegin()); it != std::prev(outer.cend()); ++it) {
    const auto & p1 = *--it;
    const auto & p2 = *it;
    const auto & p3 = *++it;

    if ((p2 - p1).cross(p3 - p2) > epsilon) {
      return false;
    }
  }

  return true;
}

alt::PointList2d simplify(const alt::PointList2d & line, const double max_distance)
{
  if (line.size() < 3) {
    return line;
  }

  alt::Points2d pending(std::next(line.begin()), std::prev(line.end()));
  alt::PointList2d simplified;

  // Douglas-Peucker algorithm

  auto douglas_peucker = [&max_distance, &pending, &simplified](
                           auto self, const alt::Point2d & seg_start,
                           const alt::Point2d & seg_end) {
    if (pending.empty()) {
      return;
    }

    const auto farthest_itr = find_farthest(pending, seg_start, seg_end);
    const auto farthest = *farthest_itr;
    pending.erase(farthest_itr);

    if (distance(farthest, seg_start, seg_end) <= max_distance) {
      return;
    }

    self(self, seg_start, farthest);
    simplified.push_back(farthest);
    self(self, farthest, seg_end);
  };

  simplified.push_back(line.front());
  douglas_peucker(douglas_peucker, line.front(), line.back());
  simplified.push_back(line.back());

  return simplified;
}

bool touches(
  const alt::Point2d & point, const alt::Point2d & seg_start, const alt::Point2d & seg_end)
{
  constexpr double epsilon = 1e-6;

  // if the cross product of the vectors from the start point and the end point to the point is 0
  // and the vectors opposite each other, the point is on the segment
  const auto start_vec = point - seg_start;
  const auto end_vec = point - seg_end;
  return std::abs(start_vec.cross(end_vec)) < epsilon && start_vec.dot(end_vec) <= 0;
}

bool touches(const alt::Point2d & point, const alt::ConvexPolygon2d & poly)
{
  const auto & vertices = poly.vertices();

  const auto [y_min_vertex, y_max_vertex] = std::minmax_element(
    vertices.begin(), std::prev(vertices.end()),
    [](const auto & a, const auto & b) { return a.y() < b.y(); });
  if (point.y() < y_min_vertex->y() || point.y() > y_max_vertex->y()) {
    return false;
  }

  for (auto it = vertices.cbegin(); it != std::prev(vertices.cend()); ++it) {
    // check if the point is on each edge of the polygon
    if (touches(point, *it, *std::next(it))) {
      return true;
    }
  }

  return false;
}

bool within(const alt::Point2d & point, const alt::ConvexPolygon2d & poly)
{
  constexpr double epsilon = 1e-6;

  const auto & vertices = poly.vertices();
  int64_t winding_number = 0;

  const auto [y_min_vertex, y_max_vertex] = std::minmax_element(
    vertices.begin(), std::prev(vertices.end()),
    [](const auto & a, const auto & b) { return a.y() < b.y(); });
  if (point.y() <= y_min_vertex->y() || point.y() >= y_max_vertex->y()) {
    return false;
  }

  double cross;
  for (auto it = vertices.cbegin(); it != std::prev(vertices.cend()); ++it) {
    const auto & p1 = *it;
    const auto & p2 = *std::next(it);

    if (p1.y() < point.y() && p2.y() > point.y()) {  // upward edge
      cross = (p2 - p1).cross(point - p1);
      if (cross > 0) {  // point is to the left of edge
        winding_number++;
        continue;
      }
    } else if (p1.y() > point.y() && p2.y() < point.y()) {  // downward edge
      cross = (p2 - p1).cross(point - p1);
      if (cross < 0) {  // point is to the left of edge
        winding_number--;
        continue;
      }
    } else {
      continue;
    }

    if (std::abs(cross) < epsilon) {  // point is on edge
      return false;
    }
  }

  return winding_number != 0;
}

bool within(
  const alt::ConvexPolygon2d & poly_contained, const alt::ConvexPolygon2d & poly_containing)
{
  if (equals(poly_contained, poly_containing)) {
    return true;
  }

  // check if all points of poly_contained are within poly_containing
  for (const auto & vertex : poly_contained.vertices()) {
    if (!within(vertex, poly_containing)) {
      return false;
    }
  }

  return true;
}
}  // namespace autoware::universe_utils

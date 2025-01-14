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

#include "autoware/universe_utils/geometry/temp_polygon_clip.hpp"

#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/simplify.hpp>
#include <boost/geometry/algorithms/within.hpp>

#include <set>

namespace autoware::universe_utils
{

namespace polygon_clip
{

void visit(
  std::vector<LinkedVertex> & vertices, std::vector<LinkedVertex> & vertices_2, std::size_t index)
{
  LinkedVertex & vertex = vertices[index];
  vertex.visited = true;

  if (vertex.corresponding.has_value()) {
    std::size_t corresponding_index = vertex.corresponding.value();
    if (corresponding_index < vertices_2.size()) {
      if (!vertices_2[corresponding_index].visited) {
        visit(vertices_2, vertices, corresponding_index);
      }
    }
  }
}

void visit(std::vector<LinkedVertex> & vertices, std::size_t index)
{
  LinkedVertex & vertex = vertices[index];
  vertex.visited = true;

  if (vertex.corresponding.has_value()) {
    std::size_t corresponding_index = vertex.corresponding.value();
    if (corresponding_index < vertices.size()) {
      if (!vertices[corresponding_index].visited) {
        visit(vertices, corresponding_index);
      }
    }
  }
}

bool is_inside(const LinkedVertex & v, const ExtendedPolygon & poly)
{
  bool contains = false;
  int32_t winding_num = 0;
  constexpr double tolerance = 1e-9;

  std::size_t vertexIndex = poly.first;
  std::size_t next_index = poly.vertices[vertexIndex].next.value_or(poly.first);

  do {
    const LinkedVertex & vertex = poly.vertices[vertexIndex];
    const LinkedVertex & next = poly.vertices[next_index];

    bool y_intersects = ((next.y < v.y) != (vertex.y < v.y)) &&
                        (v.x < (vertex.x - next.x) * (v.y - next.y) / (vertex.y - next.y) + next.x);

    if (y_intersects) {
      contains = !contains;

      if (std::abs(vertex.x - next.x) < tolerance && std::abs(vertex.y - next.y) < tolerance) {
        vertexIndex = next_index;
        next_index = poly.vertices[vertexIndex].next.value_or(poly.first);
        continue;
      }

      if (vertex.x < next.x - tolerance) {
        winding_num += 1;
      } else if (vertex.x > next.x + tolerance) {
        winding_num -= 1;
      }
    }

    vertexIndex = next_index;
    next_index = poly.vertices[vertexIndex].next.value_or(poly.first);
  } while (vertexIndex != poly.first);

  return contains;
}

Intersection intersection(
  const std::vector<LinkedVertex> & source_vertices, std::size_t s1_index, std::size_t s2_index,
  const std::vector<LinkedVertex> & clip_vertices, std::size_t c1_index, std::size_t c2_index)
{
  Intersection intersection_;

  const LinkedVertex & s1 = source_vertices[s1_index];
  const LinkedVertex & s2 = source_vertices[s2_index];
  const LinkedVertex & c1 = clip_vertices[c1_index];
  const LinkedVertex & c2 = clip_vertices[c2_index];

  double d = (c2.y - c1.y) * (s2.x - s1.x) - (c2.x - c1.x) * (s2.y - s1.y);

  if (std::abs(d) > 1e-9) {
    double t1 = ((c2.x - c1.x) * (s1.y - c1.y) - (c2.y - c1.y) * (s1.x - c1.x)) / d;
    double t2 = ((s2.x - s1.x) * (s1.y - c1.y) - (s2.y - s1.y) * (s1.x - c1.x)) / d;

    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
      intersection_.x = t1 * s2.x + (1.0 - t1) * s1.x;
      intersection_.y = t1 * s2.y + (1.0 - t1) * s1.y;
      intersection_.distance_to_source = t1;
      intersection_.distance_to_clip = t2;
      return intersection_;
    }
  }

  intersection_.x = std::numeric_limits<double>::quiet_NaN();
  intersection_.y = std::numeric_limits<double>::quiet_NaN();
  intersection_.distance_to_source = 0;
  intersection_.distance_to_clip = 0;

  return intersection_;
}

bool valid(const Intersection & intersection_)
{
  return (intersection_.distance_to_source > 0 && intersection_.distance_to_source < 1) &&
         (intersection_.distance_to_clip > 0 && intersection_.distance_to_clip < 1);
}

std::size_t add_vertex(
  ExtendedPolygon & polygon, const LinkedVertex & new_vertex, const std::size_t last_index)
{
  std::size_t p_idx = polygon.vertices.size();
  polygon.vertices.push_back(new_vertex);
  if (!polygon.vertices.empty()) {
    std::size_t last = last_index;
    std::size_t next = polygon.vertices[last].next.value_or(0);
    polygon.vertices[p_idx].prev = last;
    polygon.vertices[p_idx].next = next;
    polygon.vertices[last].next = p_idx;
    if (next != p_idx) {
      polygon.vertices[next].prev = p_idx;
    }
  }
  return p_idx;
}

ExtendedPolygon create_extended_polygon(const autoware::universe_utils::Polygon2d & poly2d)
{
  ExtendedPolygon polygon;

  const auto & outer = poly2d.outer();
  polygon.vertices.resize(outer.size());

  for (std::size_t i = 0; i < outer.size(); ++i) {
    const auto & point = outer[i];
    LinkedVertex vertex{point.x(), point.y()};

    vertex.prev = (i == 0) ? outer.size() - 1 : i - 1;
    vertex.next = (i + 1) % outer.size();

    polygon.vertices[i] = vertex;
  }
  return polygon;
}

ExtendedPolygon create_extended_polygon(const LinkedVertex & vertex)
{
  ExtendedPolygon polygon;

  polygon.vertices.push_back(vertex);

  polygon.vertices.back().prev = std::nullopt;
  polygon.vertices.back().next = std::nullopt;
  return polygon;
}

void insert_vertex(
  std::vector<LinkedVertex> & vertices, const std::size_t & vertex_index,
  const std::size_t start_index, const std::size_t end_index)
{
  std::size_t current_index = start_index;

  while (current_index != end_index &&
         vertices[current_index].distance < vertices[vertex_index].distance) {
    current_index = vertices[current_index].next.value();
  }

  vertices[vertex_index].next = current_index;
  vertices[vertex_index].prev = vertices[current_index].prev.value();
  std::size_t prev_index = vertices[current_index].prev.value();

  if (prev_index != current_index) {
    vertices[prev_index].next = vertex_index;
  }
  vertices[current_index].prev = vertex_index;

  if (current_index == start_index) {
    vertices[vertex_index].prev = start_index;
    vertices[vertex_index].next = start_index;
  }
}

std::size_t get_next(std::size_t index, const std::vector<LinkedVertex> & vertices)
{
  std::size_t current_index = index;
  while (vertices[current_index].is_intersection) {
    current_index = vertices[current_index].next.value();
  }
  return current_index;
}

std::size_t get_first_intersect(ExtendedPolygon & polygon)
{
  std::size_t v =
    polygon.first_intersect.has_value() ? polygon.first_intersect.value() : polygon.first;

  do {
    if (polygon.vertices[v].is_intersection && !polygon.vertices[v].visited) break;
    v = polygon.vertices[v].next.value();
  } while (v != polygon.first);

  polygon.first_intersect = v;
  return v;
}

bool has_unprocessed(ExtendedPolygon & polygon)
{
  std::size_t v =
    polygon.last_unprocessed.has_value() ? polygon.last_unprocessed.value() : polygon.first;

  do {
    if (polygon.vertices[v].is_intersection && !polygon.vertices[v].visited) {
      polygon.last_unprocessed = v;
      return true;
    }
    v = polygon.vertices[v].next.value();
  } while (v != polygon.first);
  polygon.last_unprocessed = std::nullopt;
  return false;
}

autoware::universe_utils::Polygon2d get_points(const ExtendedPolygon & polygon)
{
  autoware::universe_utils::Polygon2d poly;
  std::size_t v_index = polygon.first;
  std::size_t start_index = v_index;
  autoware::universe_utils::LinearRing2d outer_ringA;

  do {
    const auto & vertex = polygon.vertices[v_index];
    autoware::universe_utils::Point2d point(vertex.x, vertex.y);
    outer_ringA.push_back(point);
    v_index = vertex.next.value();
  } while (v_index != start_index);

  poly.outer() = outer_ringA;
  boost::geometry::correct(poly);
  return poly;
}

std::size_t find_next_valid(const std::vector<LinkedVertex> & vertices, std::size_t current_index)
{
  std::size_t index = vertices[current_index].next.value_or(current_index + 1);
  while (index < vertices.size() && vertices[index].is_intersection) {
    index = vertices[index].next.value_or(index + 1);
  }
  return index;
}

std::size_t find_leftmost_vertex(const ExtendedPolygon & polygon)
{
  if (polygon.vertices.empty()) {
    throw std::runtime_error("Polygon has no vertices.");
  }

  std::size_t leftmost_index = 0;
  for (std::size_t i = 1; i < polygon.vertices.size(); ++i) {
    const auto & current = polygon.vertices[i];
    const auto & leftmost = polygon.vertices[leftmost_index];

    if (current.x < leftmost.x || (current.x == leftmost.x && current.y < leftmost.y)) {
      leftmost_index = i;
    }
  }

  return leftmost_index;
}

void mark_self_intersections(ExtendedPolygon & source)
{
  std::size_t source_vertex_index = find_leftmost_vertex(source);
  std::size_t temp_index = find_leftmost_vertex(source);

  do {
    std::size_t next_vertex_index = source.vertices[source_vertex_index].next.value();
    std::size_t compare_vertex_index = source.vertices[next_vertex_index].next.value();

    do {
      Intersection i = intersection(
        source.vertices, source_vertex_index,
        get_next(source.vertices[source_vertex_index].next.value(), source.vertices),
        source.vertices, compare_vertex_index,
        get_next(source.vertices[compare_vertex_index].next.value(), source.vertices));

      if (!valid(i) || source.vertices[source_vertex_index].is_intersection) {
        compare_vertex_index = source.vertices[compare_vertex_index].next.value();
        continue;
      }

      LinkedVertex intersection_vertex_1{i.x,          i.y,          std::nullopt,
                                         std::nullopt, std::nullopt, i.distance_to_source,
                                         false,        true,         false};

      source.vertices.push_back(intersection_vertex_1);

      std::size_t index1 = source.vertices.size() - 1;
      insert_vertex(
        source.vertices, index1, source_vertex_index,
        get_next(source.vertices[source_vertex_index].next.value(), source.vertices));
      source.vertices[index1].next_2 = compare_vertex_index;
      source.vertices[index1].prev_2 =
        get_next(source.vertices[compare_vertex_index].next.value(), source.vertices);
      compare_vertex_index = source.vertices[compare_vertex_index].next.value();
    } while (compare_vertex_index != temp_index);

    source_vertex_index = source.vertices[source_vertex_index].next.value();
  } while (source_vertex_index != temp_index);
}

bool has_unvisited_intersections(const ExtendedPolygon & polygon)
{
  for (const auto & vertex : polygon.vertices) {
    if (vertex.is_intersection && !vertex.visited) {
      return true;
    }
  }
  return false;
}

std::size_t get_unvisited_intersection_index(const ExtendedPolygon & polygon)
{
  for (std::size_t i = 0; i < polygon.vertices.size(); ++i) {
    const auto & vertex = polygon.vertices[i];
    if (vertex.is_intersection && !vertex.visited) {
      return i;
    }
  }
  return static_cast<std::size_t>(-1);
}

void adjust_intersection_next(ExtendedPolygon & polygon)
{
  std::size_t current_index = find_leftmost_vertex(polygon);

  while (has_unvisited_intersections(polygon)) {
    do {
      if (!polygon.vertices[current_index].is_intersection) {
        current_index = polygon.vertices[current_index].next.value();
        continue;
      }
      polygon.vertices[current_index].visited = true;

      std::size_t index1 = current_index;
      std::size_t next_index = polygon.vertices[index1].next.value();
      std::size_t prev_index = polygon.vertices[index1].prev.value();
      std::size_t next_2_index = 0;
      std::size_t prev_2_index = 0;

      for (std::size_t i = index1; i < polygon.vertices.size(); ++i) {
        if (
          polygon.vertices[index1].next_2.value() ==
          polygon.vertices[(i + 1) % polygon.vertices.size()].prev.value()) {
          prev_2_index = (i + 1) % polygon.vertices.size();
          next_2_index = polygon.vertices[index1].next_2.value();
          break;
        } else {
          next_2_index = polygon.vertices[index1].next_2.value();
          prev_2_index = polygon.vertices[index1].prev_2.value();
        }
      }

      std::size_t best_index = next_index;
      float best_cross_product = -std::numeric_limits<float>::max();

      std::vector<std::size_t> candidates = {prev_2_index, next_2_index};
      for (std::size_t candidate : candidates) {
        if (candidate == index1) continue;

        float dx1 = polygon.vertices[candidate].x - polygon.vertices[index1].x;
        float dy1 = polygon.vertices[candidate].y - polygon.vertices[index1].y;

        float dx2 = polygon.vertices[prev_index].x - polygon.vertices[index1].x;
        float dy2 = polygon.vertices[prev_index].y - polygon.vertices[index1].y;

        float cross_product = dx1 * dy2 - dy1 * dx2;

        if (cross_product > best_cross_product) {
          best_cross_product = cross_product;
          best_index = candidate;
        }
      }

      polygon.vertices[index1].next = best_index;
      polygon.vertices[best_index].prev = index1;

      current_index = polygon.vertices[current_index].next.value();
    } while (!polygon.vertices[current_index].visited);

    current_index = get_unvisited_intersection_index(polygon);
  }
}

void mark_intersections(ExtendedPolygon & source, ExtendedPolygon & clip, bool & intersection_exist)
{
  std::size_t source_vertex_index = source.first;
  std::size_t clip_vertex_index;

  do {
    if (source.vertices[source_vertex_index].is_intersection) {
      source_vertex_index = source.vertices[source_vertex_index].next.value();
      continue;
    }

    clip_vertex_index = clip.first;

    do {
      if (clip.vertices[clip_vertex_index].is_intersection) {
        clip_vertex_index = clip.vertices[clip_vertex_index].next.value();
        continue;
      }

      Intersection i = intersection(
        source.vertices, source_vertex_index,
        get_next(source.vertices[source_vertex_index].next.value(), source.vertices), clip.vertices,
        clip_vertex_index, get_next(clip.vertices[clip_vertex_index].next.value(), clip.vertices));

      if (!valid(i)) {
        clip_vertex_index = clip.vertices[clip_vertex_index].next.value();
        continue;
      }

      intersection_exist = true;

      LinkedVertex intersection_vertex_1{i.x,          i.y,          std::nullopt,
                                         std::nullopt, std::nullopt, i.distance_to_source,
                                         false,        true,         false};
      LinkedVertex intersection_vertex_2{
        i.x, i.y, std::nullopt, std::nullopt, std::nullopt, i.distance_to_clip, false, true, false};

      source.vertices.push_back(intersection_vertex_1);
      clip.vertices.push_back(intersection_vertex_2);

      std::size_t index1 = source.vertices.size() - 1;
      std::size_t index2 = clip.vertices.size() - 1;

      source.vertices[index1].corresponding = index2;
      clip.vertices[index2].corresponding = index1;

      insert_vertex(
        source.vertices, index1, source_vertex_index,
        get_next(source.vertices[source_vertex_index].next.value(), source.vertices));
      insert_vertex(
        clip.vertices, index2, clip_vertex_index,
        get_next(clip.vertices[clip_vertex_index].next.value(), clip.vertices));

      clip_vertex_index = clip.vertices[clip_vertex_index].next.value();
    } while (clip_vertex_index != clip.first);

    source_vertex_index = source.vertices[source_vertex_index].next.value();
  } while (source_vertex_index != source.first);
}

void identify_entry_exit(
  ExtendedPolygon & source, ExtendedPolygon & clip, bool source_forwards, bool clip_forwards)
{
  std::size_t source_vertex_index = source.first;
  std::size_t clip_vertex_index = clip.first;

  bool source_in_clip = is_inside(source.vertices[source_vertex_index], clip);
  bool clip_in_source = is_inside(clip.vertices[clip_vertex_index], source);
  source_forwards ^= source_in_clip;
  clip_forwards ^= clip_in_source;

  do {
    if (source.vertices[source_vertex_index].is_intersection) {
      source.vertices[source_vertex_index].is_entry = source_forwards;
      source_forwards = !source_forwards;
    }
    source_vertex_index = source.vertices[source_vertex_index].next.value();
  } while (source_vertex_index != source.first);

  do {
    if (clip.vertices[clip_vertex_index].is_intersection) {
      clip.vertices[clip_vertex_index].is_entry = clip_forwards;
      clip_forwards = !clip_forwards;
    }
    clip_vertex_index = clip.vertices[clip_vertex_index].next.value();
  } while (clip_vertex_index != clip.first);
}

autoware::universe_utils::Polygon2d construct_self_intersecting_polygons(ExtendedPolygon & polygon)
{
  autoware::universe_utils::Polygon2d current_polygon;

  autoware::universe_utils::polygon_clip::mark_self_intersections(polygon);
  autoware::universe_utils::polygon_clip::adjust_intersection_next(polygon);
  std::size_t currentIndex = find_leftmost_vertex(polygon);
  std::size_t n = 0;

  do {
    n++;
    polygon.vertices[currentIndex].visited = true;
    autoware::universe_utils::Point2d point(
      polygon.vertices[currentIndex].x, polygon.vertices[currentIndex].y);
    current_polygon.outer().push_back(point);
    currentIndex = polygon.vertices[currentIndex].next.value();
    if (polygon.vertices[currentIndex].is_intersection && polygon.vertices[currentIndex].visited) {
      polygon.vertices[currentIndex].visited = false;
    }
  } while (!polygon.vertices[currentIndex].visited);
  boost::geometry::correct(current_polygon);
  return current_polygon;
}

std::vector<autoware::universe_utils::Polygon2d> construct_clipped_polygons(
  ExtendedPolygon & source, ExtendedPolygon & clip, const bool is_union)
{
  std::vector<autoware::universe_utils::Polygon2d> polygon_vector;

  while (has_unprocessed(source)) {
    std::size_t currentIndex = get_first_intersect(source);
    ExtendedPolygon clipped = create_extended_polygon(source.vertices[currentIndex]);
    std::size_t last_idx = 0;
    bool usingSource = true;
    do {
      visit(
        usingSource ? source.vertices : clip.vertices,
        usingSource ? clip.vertices : source.vertices, currentIndex);

      if (usingSource) {
        if (source.vertices[currentIndex].is_entry) {
          do {
            currentIndex = source.vertices[currentIndex].next.value();
            last_idx = add_vertex(clipped, source.vertices[currentIndex], last_idx);
          } while (!source.vertices[currentIndex].is_intersection);
        } else {
          do {
            currentIndex = source.vertices[currentIndex].prev.value();
            last_idx = add_vertex(clipped, source.vertices[currentIndex], last_idx);
          } while (!source.vertices[currentIndex].is_intersection);
        }
      } else {
        if (clip.vertices[currentIndex].is_entry) {
          do {
            currentIndex = clip.vertices[currentIndex].next.value();
            last_idx = add_vertex(clipped, clip.vertices[currentIndex], last_idx);
          } while (!clip.vertices[currentIndex].is_intersection);
        } else {
          do {
            currentIndex = clip.vertices[currentIndex].prev.value();
            last_idx = add_vertex(clipped, clip.vertices[currentIndex], last_idx);
          } while (!clip.vertices[currentIndex].is_intersection);
        }
      }

      currentIndex = (usingSource ? source.vertices[currentIndex] : clip.vertices[currentIndex])
                       .corresponding.value();
      usingSource = !usingSource;
    } while (
      !((usingSource ? source.vertices[currentIndex] : clip.vertices[currentIndex]).visited));
    auto points = get_points(clipped);

    if (is_union && !polygon_vector.empty()) {
      const auto & existing_polygon = polygon_vector[0];

      if (boost::geometry::within(points, existing_polygon)) {
        continue;
      } else if (boost::geometry::within(existing_polygon, points)) {
        polygon_vector[0] = points;
        continue;
      }

    } else {
      polygon_vector.push_back(points);
    }
  }
  return polygon_vector;
}

std::vector<autoware::universe_utils::Polygon2d> clip(
  ExtendedPolygon & source, ExtendedPolygon & clip, bool source_forwards, bool clip_forwards)
{
  bool intersection_exist = false;
  mark_intersections(source, clip, intersection_exist);
  identify_entry_exit(source, clip, source_forwards, clip_forwards);

  bool is_union = !source_forwards && !clip_forwards;
  bool is_intersection = source_forwards && clip_forwards;

  auto polygon_vector = construct_clipped_polygons(source, clip, is_union);

  if (!intersection_exist) {
    polygon_vector.clear();
    bool source_in_clip = is_inside(source.vertices[source.first], clip);
    bool clip_in_source = is_inside(clip.vertices[clip.first], source);

    if (is_union) {
      if (source_in_clip) {
        polygon_vector.push_back(get_points(clip));
      } else if (clip_in_source) {
        polygon_vector.push_back(get_points(source));
      } else {
        polygon_vector.push_back(get_points(source));
        polygon_vector.push_back(get_points(clip));
      }
    } else if (is_intersection) {
      if (source_in_clip) {
        polygon_vector.push_back(get_points(source));
      } else if (clip_in_source) {
        polygon_vector.push_back(get_points(clip));
      }
    } else {
      if (!source_in_clip) {
        polygon_vector.push_back(get_points(source));
      }
    }
  }

  return polygon_vector;
}

}  // namespace polygon_clip

std::vector<autoware::universe_utils::Polygon2d> apply_operation(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b, bool source_forwards, bool clip_forwards)
{
  if (polygon_a.outer().size() < 3 || polygon_b.outer().size() < 3) {
    return std::vector<autoware::universe_utils::Polygon2d>{polygon_a, polygon_b};
  }

  polygon_clip::ExtendedPolygon poly_a = polygon_clip::create_extended_polygon(polygon_a);
  polygon_clip::ExtendedPolygon poly_b = polygon_clip::create_extended_polygon(polygon_b);

  return polygon_clip::clip(poly_a, poly_b, source_forwards, clip_forwards);
}

// Difference function
std::vector<autoware::universe_utils::Polygon2d> difference(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b)
{
  return apply_operation(polygon_a, polygon_b, false, true);
}

// Union function
std::vector<autoware::universe_utils::Polygon2d> union_(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b)
{
  return apply_operation(polygon_a, polygon_b, false, false);
}

// Intersection function
std::vector<autoware::universe_utils::Polygon2d> intersection(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b)
{
  return apply_operation(polygon_a, polygon_b, true, true);
}

std::vector<autoware::universe_utils::Point2d> intersection(
  const autoware::universe_utils::Point2d & s1, const autoware::universe_utils::Point2d & s2,
  const autoware::universe_utils::Point2d & c1, const autoware::universe_utils::Point2d & c2)
{
  std::vector<autoware::universe_utils::Point2d> intersection_points;

  double d = (c2.y() - c1.y()) * (s2.x() - s1.x()) - (c2.x() - c1.x()) * (s2.y() - s1.y());

  if (std::abs(d) > 1e-9) {
    double t1 = ((c2.x() - c1.x()) * (s1.y() - c1.y()) - (c2.y() - c1.y()) * (s1.x() - c1.x())) / d;
    double t2 = ((s2.x() - s1.x()) * (s1.y() - c1.y()) - (s2.y() - s1.y()) * (s1.x() - c1.x())) / d;

    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
      autoware::universe_utils::Point2d intersection_;
      intersection_.x() = t1 * s2.x() + (1.0 - t1) * s1.x();
      intersection_.y() = t1 * s2.y() + (1.0 - t1) * s1.y();
      intersection_points.push_back(intersection_);
    }
  }

  return intersection_points;
}

std::vector<autoware::universe_utils::Point2d> intersection(
  const autoware::universe_utils::Segment2d & source_segment,
  const autoware::universe_utils::Segment2d & clip_segment)
{
  std::vector<autoware::universe_utils::Point2d> intersection_points;

  const auto & s1 = source_segment.first;
  const auto & s2 = source_segment.second;
  const auto & c1 = clip_segment.first;
  const auto & c2 = clip_segment.second;

  double d = (c2.y() - c1.y()) * (s2.x() - s1.x()) - (c2.x() - c1.x()) * (s2.y() - s1.y());

  if (std::abs(d) > 1e-9) {
    double t1 = ((c2.x() - c1.x()) * (s1.y() - c1.y()) - (c2.y() - c1.y()) * (s1.x() - c1.x())) / d;
    double t2 = ((s2.x() - s1.x()) * (s1.y() - c1.y()) - (s2.y() - s1.y()) * (s1.x() - c1.x())) / d;

    if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1) {
      autoware::universe_utils::Point2d intersection_;
      intersection_.x() = t1 * s2.x() + (1.0 - t1) * s1.x();
      intersection_.y() = t1 * s2.y() + (1.0 - t1) * s1.y();
      intersection_points.push_back(intersection_);
    }
  }

  return intersection_points;
}

}  // namespace autoware::universe_utils

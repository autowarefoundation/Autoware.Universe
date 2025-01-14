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

#ifndef AUTOWARE__UNIVERSE_UTILS__GEOMETRY__TEMP_POLYGON_CLIP_HPP_
#define AUTOWARE__UNIVERSE_UTILS__GEOMETRY__TEMP_POLYGON_CLIP_HPP_

#include "autoware/universe_utils/geometry/geometry.hpp"

#include <cmath>
#include <cstdint>
#include <optional>
#include <vector>

namespace autoware::universe_utils
{
namespace polygon_clip
{
struct LinkedVertex
{
  double x;
  double y;
  std::optional<std::size_t> next;
  std::optional<std::size_t> prev;
  std::optional<std::size_t> next_2;
  std::optional<std::size_t> prev_2;
  std::optional<std::size_t> corresponding;
  double distance;
  bool is_entry;
  bool is_intersection;
  bool visited;

  LinkedVertex()
  : x(0.0),
    y(0.0),
    next(std::nullopt),
    prev(std::nullopt),
    corresponding(std::nullopt),
    distance(0.0),
    is_entry(false),
    is_intersection(false),
    visited(false)
  {
  }

  LinkedVertex(
    double x_coord, double y_coord, std::optional<std::size_t> next_index = std::nullopt,
    std::optional<std::size_t> prev_index = std::nullopt,
    std::optional<std::size_t> corresponding_index = std::nullopt, double dist = 0.0,
    bool entry = false, bool intersection = false, bool visited_state = false)
  : x(x_coord),
    y(y_coord),
    next(next_index),
    prev(prev_index),
    corresponding(corresponding_index),
    distance(dist),
    is_entry(entry),
    is_intersection(intersection),
    visited(visited_state)
  {
  }
};

struct Intersection
{
  double x;
  double y;
  double distance_to_source;
  double distance_to_clip;
};

struct ExtendedPolygon
{
  std::size_t first;
  std::vector<LinkedVertex> vertices;
  std::optional<std::size_t> last_unprocessed;
  std::optional<std::size_t> first_intersect;

  ExtendedPolygon()
  : first(0), vertices(0), last_unprocessed(std::nullopt), first_intersect(std::nullopt)
  {
  }
};

/**
 * @brief creates an intersection between two polygon edges defined by vertex indices.
 * @param source_vertices a vector of LinkedVertex objects representing the source polygon's
 * vertices.
 * @param s1_index index of the first vertex of the source polygon edge.
 * @param s2_index index of the second vertex of the source polygon edge.
 * @param clip_vertices a vector of LinkedVertex objects representing the clipping polygon's
 * vertices.
 * @param c1_index index of the first vertex of the clipping polygon edge.
 * @param c2_index index of the second vertex of the clipping polygon edge.
 * @return intersection object with point and distance to each vertex.
 */
Intersection intersection(
  const std::vector<LinkedVertex> & source_vertices, std::size_t s1_index, std::size_t s2_index,
  const std::vector<LinkedVertex> & clip_vertices, std::size_t c1_index, std::size_t c2_index);

/**
 * @brief creates an ExtendedPolygon from a Polygon2d object.
 * @param poly2d the input Polygon2d object.
 * @return the created ExtendedPolygon object.
 */
ExtendedPolygon create_extended_polygon(const autoware::universe_utils::Polygon2d & poly2d);

/**
 * @brief adds a new vertex to the given ExtendedPolygon, updating the linked vertex structure.
 * @details inserts the new vertex at the end of the polygon's vertices list and updates
 * the doubly linked list's `next` and `prev` pointers. It also ensures that the new vertex links
 * properly with the previous and next vertices in the polygon.
 *
 * @param polygon The polygon to which the vertex will be added.
 * @param new_vertex The vertex to add.
 * @param last_index The index of the last vertex before adding the new one (used to link the new
 * vertex). Defaults to 0.
 *
 * @return The index of the newly added vertex.
 */
std::size_t add_vertex(
  ExtendedPolygon & polygon, const LinkedVertex & new_vertex, const std::size_t last_index = 0);

/**
 * @brief inserts a vertex into a vector of vertices between a given start and end index.
 * @details traverses the vector of vertices from `start_index` to `end_index` and
 * compares the distances of the vertices to find the correct insertion point for the new vertex.
 * once the appropriate position is found, it updates the `next` and `prev` pointers of the
 * surrounding vertices to maintain the doubly linked list structure.
 *
 * @param vertices A vector of LinkedVertex objects representing the vertices of the polygon.
 * @param vertex The vertex to be inserted.
 * @param start_index The starting index of the range in which to insert the vertex.
 * @param end_index The ending index of the range in which to insert the vertex.
 */
void insert_vertex(
  std::vector<LinkedVertex> & vertices, const std::size_t & vertex_index, const std::size_t start,
  const std::size_t end);

/**
 * @brief gets the index of the first intersection vertex in the polygon.
 * @param polygon the ExtendedPolygon to check.
 * @return the index of the first intersection vertex.
 */
std::size_t get_first_intersect(ExtendedPolygon & polygon);

// cSpell:ignore Greiner, Hormann
/**
 * @brief Clips one polygon using another and returns the resulting polygons.
 * @param source The source polygon to be clipped.
 * @param clip The clipping polygon.
 * @param source_forwards Specifies the direction of traversal for the source polygon.
 * @param clip_forwards Specifies the direction of traversal for the clipping polygon.
 * @details Based on Greiner-Hormann algorithm
 * https://www.inf.usi.ch/faculty/hormann/papers/Greiner.1998.ECO.pdf Greiner-Hormann algorithm
 * cannot handle degenerate cases, e.g. when intersections are falling on the edges of the polygon,
 * or vertices coincide. source_forwards and clip_forwards for each function:
 * - false, true : difference
 * - false, false : union
 * - true, true : intersection
 * The Clipping is splitted into three main parts:
 * 1. Marking Intersections
 * 2. Identify Entry and Exit Points
 * 3. Construct Clipped Polygons
 * @return A vector of Polygon2d objects representing the clipped result.
 *
 * @note This implementation may encounter precision errors due to the
 * difference in the methods used to calculate intersection points with the
 * Boost libraries. Results have been validated to be correct when compared
 * to the Shapely library in Python, and any discrepancies are
 * primarily due to the difference in the way Boost.Geometry handles
 * intersection points. The difference could reach 1.0 error.
 * In a cases where the found clipped polygon is small, Boost geometry will ignore/simplify the
 * multi polygon thus ignoring the small/negligible polygon. But that is not the case is shapely and
 * our custom polygon clip, as shown in 'test_geometry.cpp' line.
 */
std::vector<autoware::universe_utils::Polygon2d> clip(
  ExtendedPolygon & source, ExtendedPolygon & clip, bool source_forwards, bool clip_forwards);

/**
 * @brief Marks self-intersections in the given ExtendedPolygon.
 * @details This function identifies self-intersecting segments in the polygon and creates new
 * intersection vertices. The detected intersections are then marked and processed to update the
 * polygon structure accordingly.
 *
 * @param source The ExtendedPolygon in which to detect and mark self-intersections.
 * @param current_index The current index in the polygon being processed. This index is updated as
 * intersections are marked.
 */
void mark_self_intersections(ExtendedPolygon & source, std::size_t & current_index);

/**
 * @brief Adjusts the `next` pointer of the intersection vertex in the polygon to point to the
 * appropriate succeeding vertex.
 * @details Ensures that the polygon to always go left, so no self-intersection occurs.
 *
 * @param polygon The ExtendedPolygon containing the vertices.
 * @param current_index The index of the intersection vertex to adjust. This is updated within the
 * function if necessary.
 */
void adjust_intersection_next(ExtendedPolygon & polygon, std::size_t & current_index);

autoware::universe_utils::Polygon2d construct_self_intersecting_polygons(ExtendedPolygon & polygon);
}  // namespace polygon_clip

/**
 * @brief computes the difference between two polygons.
 * @param polygon_a The source polygon.
 * @param polygon_b The clip polygon.
 * @return a vector of Polygon2d objects representing the difference.
 */
std::vector<autoware::universe_utils::Polygon2d> difference(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b);

/**
 * @brief computes the union of two polygons.
 * @param polygon_a The source polygon.
 * @param polygon_b The clip polygon.
 * @return a vector of Polygon2d objects representing the union.
 */
std::vector<autoware::universe_utils::Polygon2d> union_(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b);

/**
 * @brief computes the intersection of two polygons.
 * @param polygon_a The source polygon.
 * @param polygon_b The clip polygon.
 * @return a vector of Polygon2d objects representing the intersection.
 */
std::vector<autoware::universe_utils::Polygon2d> intersection(
  const autoware::universe_utils::Polygon2d & polygon_a,
  const autoware::universe_utils::Polygon2d & polygon_b);

/**
 * @brief Calculates the intersection point between two line segments.
 * @details
 * This function computes the intersection point of two line segments defined by their endpoints:
 * (s1, s2) for the source segment and (c1, c2) for the clipping segment.
 * @param s1 The first endpoint of the source segment.
 * @param s2 The second endpoint of the source segment.
 * @param c1 The first endpoint of the clipping segment.
 * @param c2 The second endpoint of the clipping segment.
 * @return A vector of intersection points (typically one or none).
 */

std::vector<autoware::universe_utils::Point2d> intersection(
  const autoware::universe_utils::Point2d & s1, const autoware::universe_utils::Point2d & s2,
  const autoware::universe_utils::Point2d & c1, const autoware::universe_utils::Point2d & c2);

std::vector<autoware::universe_utils::Point2d> intersection(
  const autoware::universe_utils::Segment2d & source_segment,
  const autoware::universe_utils::Segment2d & clip_segment);

}  // namespace autoware::universe_utils

#endif  // AUTOWARE__UNIVERSE_UTILS__GEOMETRY__TEMP_POLYGON_CLIP_HPP_

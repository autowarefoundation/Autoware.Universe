// Copyright 2022 TIER IV, Inc.
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

#include "drivable_area_expander/drivable_area_expander.hpp"

#include "drivable_area_expander/obstacles.hpp"
#include "drivable_area_expander/parameters.hpp"
#include "drivable_area_expander/types.hpp"

#include <tier4_autoware_utils/geometry/geometry.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/transform.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

namespace drivable_area_expander
{
multipolygon_t filterFootprint(
  const std::vector<Footprint> & footprints, const std::vector<Footprint> & predicted_paths,
  const multilinestring_t & uncrossable_lines)
{
  namespace strategy = boost::geometry::strategy::buffer;
  multipolygon_t filtered_footprints;
  polygon_t filtered_footprint;
  multipolygon_t cuts;
  multipolygon_t uncrossable_polys;
  boost::geometry::buffer(
    uncrossable_lines, uncrossable_polys, strategy::distance_symmetric<double>(0.1),
    strategy::side_straight(), strategy::join_miter(), strategy::end_flat(),
    strategy::point_square());
  for (const auto & f : footprints) {
    filtered_footprint = f.footprint;
    cuts.clear();
    boost::geometry::difference(filtered_footprint, uncrossable_polys, cuts);
    for (const auto & cut : cuts) {
      if (boost::geometry::within(f.origin, cut)) {
        filtered_footprint = cut;
        break;
      }
    }
    for (const auto & p : predicted_paths) {
      cuts.clear();
      boost::geometry::difference(filtered_footprint, p.footprint, cuts);
      for (const auto & cut : cuts) {
        if (boost::geometry::within(f.origin, cut)) {
          filtered_footprint = cut;
          break;
        }
      }
    }
    if (!filtered_footprint.outer().empty()) filtered_footprints.push_back(filtered_footprint);
  }
  return filtered_footprints;
}

multilinestring_t expandDrivableArea(
  std::vector<Point> & left_bound, std::vector<Point> & right_bound,
  const multipolygon_t & footprint)
{
  const auto to_point = [](const auto & p) { return point_t{p.x, p.y}; };
  polygon_t original_da_poly;
  original_da_poly.outer().reserve(left_bound.size() + right_bound.size() + 1);
  for (const auto & p : left_bound) original_da_poly.outer().push_back(to_point(p));
  for (auto it = right_bound.rbegin(); it != right_bound.rend(); ++it)
    original_da_poly.outer().push_back(to_point(*it));
  original_da_poly.outer().push_back(original_da_poly.outer().front());
  // extend with filtered footprint
  multipolygon_t unions;
  auto extended_da_poly = original_da_poly;
  for (const auto & f : footprint) {
    unions.clear();
    boost::geometry::union_(extended_da_poly, f, unions);
    // assumes that the footprints are not disjoint from the original drivable area
    extended_da_poly = unions.front();
  }
  const auto left_front = to_point(left_bound.front());
  const auto right_front = to_point(right_bound.front());
  const auto left_back = to_point(left_bound.back());
  const auto right_back = to_point(right_bound.back());
  size_t lf_idx = 0;
  double lf_min_dist = std::numeric_limits<double>::max();
  size_t rf_idx = 0;
  double rf_min_dist = std::numeric_limits<double>::max();
  size_t lb_idx = 0;
  double lb_min_dist = std::numeric_limits<double>::max();
  size_t rb_idx = 0;
  double rb_min_dist = std::numeric_limits<double>::max();
  boost::geometry::correct(extended_da_poly);
  // remove the duplicated point (front == back) to prevent issue when splitting into left/right
  extended_da_poly.outer().resize(extended_da_poly.outer().size() - 1);
  for (auto i = 0lu; i < extended_da_poly.outer().size(); ++i) {
    const auto & p = extended_da_poly.outer()[i];
    const auto lf_dist = boost::geometry::distance(left_front, p);
    const auto rf_dist = boost::geometry::distance(right_front, p);
    const auto lb_dist = boost::geometry::distance(left_back, p);
    const auto rb_dist = boost::geometry::distance(right_back, p);
    if (lf_dist < lf_min_dist) {
      lf_idx = i;
      lf_min_dist = lf_dist;
    }
    if (rf_dist < rf_min_dist) {
      rf_idx = i;
      rf_min_dist = rf_dist;
    }
    if (lb_dist < lb_min_dist) {
      lb_idx = i;
      lb_min_dist = lb_dist;
    }
    if (rb_dist < rb_min_dist) {
      rb_idx = i;
      rb_min_dist = rb_dist;
    }
  }
  std::vector<size_t> left_indexes;
  std::vector<size_t> right_indexes;
  // NOTE: we use int when decreasing the index to avoid overflow after a value of 0
  if (lf_idx < lb_idx) {
    if (lf_idx < rb_idx && rb_idx < lb_idx) {  // [..., left front, ... right front, ..., right
                                               // back, ..., left back, ...]
      for (int i = lf_idx; i >= 0; --i) left_indexes.push_back(static_cast<size_t>(i));
      for (int i = extended_da_poly.outer().size() - 1; i >= static_cast<int>(lb_idx); --i)
        left_indexes.push_back(static_cast<size_t>(i));
      for (auto i = rf_idx; i <= rb_idx; ++i) right_indexes.push_back(i);
    } else {  // [..., right front, left front, ... left back, ..., right back, ..., ]
      for (auto i = lf_idx; i <= lb_idx; ++i) left_indexes.push_back(i);
      for (int i = rf_idx; i >= 0; --i) right_indexes.push_back(static_cast<size_t>(i));
      for (int i = extended_da_poly.outer().size() - 1; i >= static_cast<int>(rb_idx); --i)
        right_indexes.push_back(static_cast<size_t>(i));
    }
  } else {                                     // lb_idx < lf_idx
    if (lb_idx < rb_idx && rb_idx < lf_idx) {  // [ ..., left back, ..., right back , ..., right
                                               // front, ..., left front, ...]
      for (auto i = lf_idx; i < extended_da_poly.outer().size(); ++i) left_indexes.push_back(i);
      for (auto i = 0lu; i <= lb_idx; ++i) left_indexes.push_back(i);
      for (int i = rf_idx; i >= static_cast<int>(rb_idx); --i)
        right_indexes.push_back(static_cast<size_t>(i));
    } else {  // [ ..., right back, ..., left back, ..., left front, ..., right front, ...]
      for (int i = lf_idx; i >= static_cast<int>(lb_idx); --i)
        left_indexes.push_back(static_cast<size_t>(i));
      for (auto i = rf_idx; i < extended_da_poly.outer().size(); ++i) right_indexes.push_back(i);
      for (auto i = 0lu; i <= rb_idx; ++i) right_indexes.push_back(i);
    }
  }
  // TODO(Maxime): what about the z value ?
  Point point;
  left_bound.clear();
  right_bound.clear();
  linestring_t debug_left_ls;
  linestring_t debug_right_ls;
  for (const auto i : left_indexes) {
    const auto & p = extended_da_poly.outer()[i];
    debug_left_ls.push_back(p);
    point.x = p.x();
    point.y = p.y();
    left_bound.push_back(point);
  }
  for (const auto i : right_indexes) {
    const auto & p = extended_da_poly.outer()[i];
    debug_right_ls.push_back(p);
    point.x = p.x();
    point.y = p.y();
    right_bound.push_back(point);
  }
  return {debug_left_ls, debug_right_ls};
}

std::vector<Footprint> createPathFootprints(const Path & path, const ExpansionParameters & params)
{
  const auto left = params.ego_left_offset + params.ego_extra_left_offset;
  const auto right = params.ego_right_offset - params.ego_extra_right_offset;
  const auto rear = params.ego_rear_offset - params.ego_extra_rear_offset;
  const auto front = params.ego_front_offset + params.ego_extra_front_offset;
  polygon_t base_footprint;
  base_footprint.outer() = {
    point_t{front, left}, point_t{front, right}, point_t{rear, right}, point_t{rear, left},
    point_t{front, left}};
  std::vector<Footprint> footprints;
  footprints.reserve(path.points.size());
  for (const auto & point : path.points)
    footprints.push_back(createFootprint(point.pose, base_footprint));
  return footprints;
}

std::vector<Footprint> createPredictedPathFootprints(
  const autoware_auto_perception_msgs::msg::PredictedObjects & predicted_objects,
  const ExpansionParameters & params)
{
  if (params.avoid_dynamic_objects)
    return createObjectFootprints(predicted_objects, params);
  else
    return {};
}

linestring_t createMaxExpansionLine(const Path & path, const double max_expansion_distance)
{
  namespace strategy = boost::geometry::strategy::buffer;
  linestring_t max_expansion_line;
  if (max_expansion_distance > 0.0) {
    multipolygon_t polygons;
    linestring_t path_ls;
    for (const auto & p : path.points) path_ls.push_back({p.pose.position.x, p.pose.position.y});
    boost::geometry::buffer(
      path_ls, polygons, strategy::distance_symmetric<double>(max_expansion_distance),
      strategy::side_straight(), strategy::join_miter(), strategy::end_flat(),
      strategy::point_square());
    if (!polygons.empty()) {
      const auto & polygon = polygons.front();
      max_expansion_line.insert(
        max_expansion_line.end(), polygon.outer().begin(), polygon.outer().end());
    }
  }
  return max_expansion_line;
}
}  // namespace drivable_area_expander

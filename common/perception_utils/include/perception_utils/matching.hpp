
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

#ifndef PERCEPTION_UTILS__MATCHING_HPP_
#define PERCEPTION_UTILS__MATCHING_HPP_

#include "perception_utils/geometry.hpp"
#include "tier4_autoware_utils/geometry/boost_geometry.hpp"
#include "tier4_autoware_utils/geometry/boost_polygon_utils.hpp"

#include <boost/geometry.hpp>

#include <vector>

namespace perception_utils
{
template <class T1, class T2>
inline double get2dIoU(const T1 source_object, const T2 target_object)
{
  const auto & source_pose = getPose(source_object);
  const auto & target_pose = getPose(target_object);

  const auto source_polygon = tier4_autoware_utils::toPolygon2d(source_pose, source_object.shape);
  const auto target_polygon = tier4_autoware_utils::toPolygon2d(target_pose, target_object.shape);

  std::vector<tier4_autoware_utils::Polygon2d> union_polygons;
  std::vector<tier4_autoware_utils::Polygon2d> intersection_polygons;
  boost::geometry::union_(source_polygon, target_polygon, union_polygons);
  boost::geometry::intersection(source_polygon, target_polygon, intersection_polygons);

  double intersection_area = 0.0;
  double union_area = 0.0;
  for (const auto & intersection_polygon : intersection_polygons) {
    intersection_area += boost::geometry::area(intersection_polygon);
  }
  if (intersection_area == 0.0) return 0.0;

  for (const auto & union_polygon : union_polygons) {
    union_area += boost::geometry::area(union_polygon);
  }

  const double iou = union_area < 0.01 ? 0.0 : std::min(1.0, intersection_area / union_area);
  return iou;
}

template <class T1, class T2>
inline double get2dPrecision(const T1 source_object, const T2 target_object)
{
  const auto & source_pose = getPose(source_object);
  const auto & target_pose = getPose(target_object);

  const auto source_polygon = tier4_autoware_utils::toPolygon2d(source_pose, source_object.shape);
  const auto target_polygon = tier4_autoware_utils::toPolygon2d(target_pose, target_object.shape);

  std::vector<tier4_autoware_utils::Polygon2d> intersection_polygons;
  boost::geometry::intersection(source_polygon, target_polygon, intersection_polygons);

  double intersection_area = 0.0;
  double source_area = 0.0;
  for (const auto & intersection_polygon : intersection_polygons) {
    intersection_area += boost::geometry::area(intersection_polygon);
  }
  if (intersection_area == 0.0) return 0.0;

  source_area = boost::geometry::area(source_polygon);

  const double precision = std::min(1.0, intersection_area / source_area);
  return precision;
}

template <class T1, class T2>
inline double get2dRecall(const T1 source_object, const T2 target_object)
{
  const auto & source_pose = getPose(source_object);
  const auto & target_pose = getPose(target_object);

  const auto source_polygon = tier4_autoware_utils::toPolygon2d(source_pose, source_object.shape);
  const auto target_polygon = tier4_autoware_utils::toPolygon2d(target_pose, target_object.shape);

  std::vector<tier4_autoware_utils::Polygon2d> intersection_polygons;
  boost::geometry::intersection(source_polygon, target_polygon, intersection_polygons);

  double intersection_area = 0.0;
  double target_area = 0.0;
  for (const auto & intersection_polygon : intersection_polygons) {
    intersection_area += boost::geometry::area(intersection_polygon);
  }
  if (intersection_area == 0.0) return 0.0;

  target_area += boost::geometry::area(target_polygon);

  const double recall = std::min(1.0, intersection_area / target_area);
  return recall;
}

}  // namespace perception_utils

#endif  // PERCEPTION_UTILS__MATCHING_HPP_

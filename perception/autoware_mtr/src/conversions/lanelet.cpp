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

#include "autoware/mtr/conversions/lanelet.hpp"

#include "autoware/mtr/polyline.hpp"

#include <geometry_msgs/msg/detail/point__struct.hpp>

#include <cmath>
#include <optional>
#include <vector>

namespace autoware::mtr
{
std::optional<PolylineData> LaneletConverter::convert(
  const geometry_msgs::msg::Point & position, double distance_threshold) const
{
  std::vector<LanePoint> container;
  // parse lanelet layers
  for (const auto & lanelet : lanelet_map_ptr_->laneletLayer) {
    const auto lanelet_subtype = toSubtypeName(lanelet);
    if (isLaneLike(lanelet_subtype)) {
      // convert centerlines
      if (isRoadwayLike(lanelet_subtype)) {
        auto points = fromLinestring(lanelet.centerline3d(), position, distance_threshold);
        insertLanePoints(points, container);
      }
      // convert boundaries except of virtual lines
      if (!isTurnableIntersection(lanelet)) {
        const auto left_bound = lanelet.leftBound3d();
        if (isBoundaryLike(left_bound)) {
          auto points = fromLinestring(left_bound, position, distance_threshold);
          insertLanePoints(points, container);
        }
        const auto right_bound = lanelet.rightBound3d();
        if (isBoundaryLike(right_bound)) {
          auto points = fromLinestring(right_bound, position, distance_threshold);
          insertLanePoints(points, container);
        }
      }
    } else if (isCrosswalkLike(lanelet_subtype)) {
      auto points = fromPolygon(lanelet.polygon3d(), position, distance_threshold);
      insertLanePoints(points, container);
    }
  }

  // parse linestring layers
  for (const auto & linestring : lanelet_map_ptr_->lineStringLayer) {
    if (isBoundaryLike(linestring)) {
      auto points = fromLinestring(linestring, position, distance_threshold);
      insertLanePoints(points, container);
    }
  }

  return container.size() == 0
           ? std::nullopt
           : std::make_optional<PolylineData>(
               container, max_num_polyline_, max_num_point_, point_break_distance_);
}

std::vector<LanePoint> LaneletConverter::fromLinestring(
  const lanelet::ConstLineString3d & linestring, const geometry_msgs::msg::Point & position,
  double distance_threshold) const noexcept
{
  if (linestring.size() == 0) {
    return {};
  }

  std::vector<LanePoint> output;
  for (auto itr = linestring.begin(); itr != linestring.end(); ++itr) {
    if (auto distance =
          std::hypot(itr->x() - position.x, itr->y() - position.y, itr->z() - position.z);
        distance > distance_threshold) {
      continue;
    }
    double dx, dy, dz;
    constexpr double epsilon = 1e-6;
    if (itr == linestring.begin()) {
      dx = 0.0;
      dy = 0.0;
      dz = 0.0;
    } else {
      dx = itr->x() - (itr - 1)->x();
      dy = itr->y() - (itr - 1)->y();
      dz = itr->z() - (itr - 1)->z();
      const auto norm = std::hypot(dx, dy, dz);
      dx /= (norm + epsilon);
      dy /= (norm + epsilon);
      dz /= (norm + epsilon);
    }
    output.emplace_back(itr->x(), itr->y(), itr->z(), dx, dy, dz, 0.0);  // TODO(ktro2828): Label ID
  }
  return output;
}

std::vector<LanePoint> LaneletConverter::fromPolygon(
  const lanelet::CompoundPolygon3d & polygon, const geometry_msgs::msg::Point & position,
  double distance_threshold) const noexcept
{
  if (polygon.size() == 0) {
    return {};
  }

  std::vector<LanePoint> output;
  for (auto itr = polygon.begin(); itr != polygon.end(); ++itr) {
    if (auto distance =
          std::hypot(itr->x() - position.x, itr->y() - position.y, itr->z() - position.z);
        distance > distance_threshold) {
      continue;
    }
    double dx, dy, dz;
    constexpr double epsilon = 1e-6;
    if (itr == polygon.begin()) {
      dx = 0.0;
      dy = 0.0;
      dz = 0.0;
    } else {
      dx = itr->x() - (itr - 1)->x();
      dy = itr->y() - (itr - 1)->y();
      dz = itr->z() - (itr - 1)->z();
      const auto norm = std::hypot(dx, dy, dz);
      dx /= (norm + epsilon);
      dy /= (norm + epsilon);
      dz /= (norm + epsilon);
    }
    output.emplace_back(itr->x(), itr->y(), itr->z(), dx, dy, dz, 0.0);  // TODO(ktro2828): Label ID
  }
  return output;
}
}  // namespace autoware::mtr

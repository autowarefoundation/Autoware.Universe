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

#ifndef AUTOWARE__MTR__CONVERSIONS__LANELET_HPP_
#define AUTOWARE__MTR__CONVERSIONS__LANELET_HPP_

#include "autoware/mtr/polyline.hpp"

#include <geometry_msgs/msg/detail/point__struct.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/CompoundPolygon.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/utility/Optional.h>

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace autoware::mtr
{
/**
 * @brief Insert lane points into the container from the end of it.
 *
 * @param points Sequence of points to be inserted.
 * @param container Points container.
 */
inline void insertLanePoints(
  const std::vector<LanePoint> & points, std::vector<LanePoint> & container)
{
  container.reserve(container.size() * 2);
  container.insert(container.end(), points.begin(), points.end());
}

inline lanelet::Optional<std::string> toTypeName(const lanelet::ConstLanelet & lanelet)
{
  return lanelet.hasAttribute("type") ? lanelet.attribute("type").as<std::string>()
                                      : lanelet::Optional<std::string>();
}

inline lanelet::Optional<std::string> toTypeName(const lanelet::ConstLineString3d & linestring)
{
  return linestring.hasAttribute("type") ? linestring.attribute("type").as<std::string>()
                                         : lanelet::Optional<std::string>();
}

/**
 * @brief Extract the subtype name from a lanelet.
 *
 * @param lanelet Lanelet instance.
 * @return std::optional<string>
 */
inline lanelet::Optional<std::string> toSubtypeName(const lanelet::ConstLanelet & lanelet) noexcept
{
  return lanelet.hasAttribute("subtype") ? lanelet.attribute("subtype").as<std::string>()
                                         : lanelet::Optional<std::string>();
}

/**
 * @brief Extract the subtype name from a 3D linestring.
 *
 * @param linestring 3D linestring instance.
 * @return lanelet::Optional<std::string>
 */
inline lanelet::Optional<std::string> toSubtypeName(
  const lanelet::ConstLineString3d & linestring) noexcept
{
  return linestring.hasAttribute("subtype") ? linestring.attribute("subtype").as<std::string>()
                                            : lanelet::Optional<std::string>();
}

/**
 * @brief Check if the specified lanelet is the turnable intersection.
 *
 * @param lanelet Lanelet instance.
 * @return true if the lanelet has the attribute named turn_direction.
 */
inline bool isTurnableIntersection(const lanelet::ConstLanelet & lanelet) noexcept
{
  return lanelet.hasAttribute("turn_direction");
}

/**
 * @brief Check if the specified lanelet subtype is kind of lane.
 *
 * @param subtype
 * @return True if the lanelet subtype is the one of the (road, highway, road_shoulder,
 * pedestrian_lane, bicycle_lane, walkway).
 */
inline bool isLaneLike(const lanelet::Optional<std::string> & subtype)
{
  if (!subtype) {
    return false;
  }
  const auto & subtype_str = subtype.value();
  return (
    subtype_str == "road" || subtype_str == "highway" || subtype_str == "road_shoulder" ||
    subtype_str == "pedestrian_lane" || subtype_str == "bicycle_lane" || subtype_str == "walkway");
}

/**
 * @brief Check if the specified lanelet subtype is kind of the roadway.
 *
 * @param subtype Subtype of the corresponding lanelet.
 * @return True if the subtype is the one of the (road, highway, road_shoulder).
 */
inline bool isRoadwayLike(const lanelet::Optional<std::string> & subtype)
{
  if (!subtype) {
    return false;
  }
  const auto & subtype_str = subtype.value();
  return subtype_str == "road" || subtype_str == "highway" || subtype_str == "road_shoulder";
}

/**
 * @brief Check if the specified linestring is kind of the boundary.
 *
 * @param linestring 3D linestring.
 * @return True if the type is the one of the (line_thin, line_thick, road_boarder) and the subtype
 * is not virtual.
 */
inline bool isBoundaryLike(const lanelet::ConstLineString3d & linestring)
{
  const auto type = toTypeName(linestring);
  const auto subtype = toSubtypeName(linestring);
  if (!type || !subtype) {
    return false;
  }

  const auto & type_str = type.value();
  const auto & subtype_str = subtype.value();
  return (type_str == "line_thin" || type_str == "line_thick" || type_str == "road_boarder") &&
         subtype_str != "virtual";
}

/**
 * @brief Check if the specified linestring is the kind of crosswalk.
 *
 * @param subtype Subtype of the corresponding polygon.
 * @return True if the lanelet subtype is the one of the (crosswalk,).
 */
inline bool isCrosswalkLike(const lanelet::Optional<std::string> & subtype)
{
  if (!subtype) {
    return false;
  }

  const auto & subtype_str = subtype.value();
  return subtype_str == "crosswalk";
}

/**
 * @brief A class to convert lanelet map to polyline.
 */
class LaneletConverter
{
public:
  /**
   * @brief Construct a new Lanelet Converter object
   *
   * @param lanelet_map_ptr Pointer of loaded lanelet map.
   * @param max_num_polyline The max number of polylines to be contained in the tensor. If the total
   * number of polylines are less than this value, zero-filled polylines will be padded.
   * @param max_num_point The max number of points to be contained in a single polyline.
   * @param point_break_distance Distance threshold to separate two polylines.
   */
  explicit LaneletConverter(
    lanelet::LaneletMapConstPtr lanelet_map_ptr, size_t max_num_polyline, size_t max_num_point,
    float point_break_distance)
  : lanelet_map_ptr_(lanelet_map_ptr),
    max_num_polyline_(max_num_polyline),
    max_num_point_(max_num_point),
    point_break_distance_(point_break_distance)
  {
  }

  /**
   * @brief Convert a lanelet map to the polyline data except of points whose distance from the
   * specified position is farther than the threshold.
   *
   * @param position Origin to check the distance from this.
   * @param distance_threshold Distance threshold
   * @return std::optional<PolylineData>
   */
  std::optional<PolylineData> convert(
    const geometry_msgs::msg::Point & position, double distance_threshold) const;

private:
  /**
   * @brief Convert a linestring to the set of polylines.
   *
   * @param linestring Linestring instance.
   * @param position Origin to check the distance from this.
   * @param distance_threshold Distance threshold from the specified position.
   * @return std::vector<LanePoint>
   */
  std::vector<LanePoint> fromLinestring(
    const lanelet::ConstLineString3d & linestring, const geometry_msgs::msg::Point & position,
    double distance_threshold) const noexcept;

  /**
   * @brief Convert a polygon to the set of polylines.
   *
   * @param polygon Polygon instance.
   * @param position Origin to check the distance from this.
   * @param distance_threshold Distance threshold from the specified position.
   * @return std::vector<LanePoint>
   */
  std::vector<LanePoint> fromPolygon(
    const lanelet::CompoundPolygon3d & polygon, const geometry_msgs::msg::Point & position,
    double distance_threshold) const noexcept;

  lanelet::LaneletMapConstPtr lanelet_map_ptr_;  //!< Pointer of lanelet map.
  size_t max_num_polyline_;                      //!< The max number of polylines.
  size_t max_num_point_;                         //!< The max number of points.
  float point_break_distance_;                   //!< Distance threshold to separate two polylines.
};
}  // namespace autoware::mtr

#endif  // AUTOWARE__MTR__CONVERSIONS__LANELET_HPP_

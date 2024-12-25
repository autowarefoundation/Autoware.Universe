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

#include "lanelet_filter.hpp"

#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/algorithms/disjoint.hpp>
#include <boost/geometry/algorithms/intersects.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/geometry/LaneletMap.h>
#include <lanelet2_core/geometry/Polygon.h>

#include <optional>
#include <utility>
#include <vector>

namespace autoware::detected_object_validation
{
namespace lanelet_filter
{

using visualization_msgs::msg::Marker;
using visualization_msgs::msg::MarkerArray;

template <typename T>
std::optional<visualization_msgs::msg::Marker> createPolygonMarker(
  T polygon, rclcpp::Time stamp, std::string_view ns, size_t marker_id,
  std_msgs::msg::ColorRGBA color)
{
  if (polygon.empty()) {
    return std::nullopt;
  }
  const constexpr std::string_view FRAME_ID = "map";

  auto create_marker = [&](auto marker_type) {
    Marker marker;
    marker.id = marker_id;
    marker.header.frame_id = FRAME_ID;
    marker.header.stamp = stamp;
    marker.ns = ns;
    marker.type = marker_type;
    marker.action = Marker::ADD;
    marker.lifetime = rclcpp::Duration::from_seconds(0.1);
    return marker;
  };

  auto marker = create_marker(Marker::LINE_LIST);
  marker.pose.orientation.w = 1.0;
  marker.scale.x = 0.1;
  marker.color.a = color.a;
  marker.color.r = color.r;
  marker.color.g = color.g;
  marker.color.b = color.b;

  std::vector<geometry_msgs::msg::Point> points;
  points.reserve(polygon.size() * 2);

  for (auto it = polygon.begin(); it != std::prev(polygon.end()); ++it) {
    geometry_msgs::msg::Point point;

    point.x = it->x();
    point.y = it->y();
    points.push_back(point);

    const auto next = std::next(it);
    point.x = next->x();
    point.y = next->y();
    points.push_back(point);
  }
  geometry_msgs::msg::Point point;
  point.x = polygon.back().x();
  point.y = polygon.back().y();

  points.push_back(point);
  point.x = polygon.front().x();
  point.y = polygon.front().y();

  points.push_back(point);
  marker.points = points;
  return marker;
}

void ObjectLaneletFilterNode::publishDebugMarkers(
  rclcpp::Time stamp, const LinearRing2d & hull, const std::vector<BoxAndLanelet> & lanelets)
{
  using visualization_msgs::msg::Marker;
  using visualization_msgs::msg::MarkerArray;

  uint8_t marker_id = 0;
  Marker delete_marker;
  constexpr std::string_view lanelet_range = "lanelet_range";
  constexpr std::string_view roi = "roi";

  MarkerArray marker_array;

  std_msgs::msg::ColorRGBA color;
  color.a = 0.5f;
  color.r = 0.3f;
  color.g = 1.0f;
  color.b = 0.2f;
  if (auto marker = createPolygonMarker(hull, stamp, lanelet_range, ++marker_id, color); marker) {
    marker_array.markers.push_back(std::move(*marker));
  }
  for (const auto & box_and_lanelet : lanelets) {
    color.r = 0.2;
    color.g = 0.5;
    color.b = 1.0;
    auto p = box_and_lanelet.second.polygon;
    if (auto marker = createPolygonMarker(p, stamp, roi, ++marker_id, color); marker) {
      marker_array.markers.push_back(std::move(*marker));
    }
  }
  viz_pub_->publish(marker_array);
}
}  // namespace lanelet_filter
}  // namespace autoware::detected_object_validation

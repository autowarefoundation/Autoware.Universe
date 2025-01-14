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

#include "autoware/behavior_path_planner_common/marker_utils/colors.hpp"
#include "autoware/behavior_path_planner_common/marker_utils/utils.hpp"

#include <autoware/behavior_path_lane_change_module/utils/markers.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/ros/marker_helper.hpp>
#include <autoware_lanelet2_extension/visualization/visualization.hpp>
#include <magic_enum.hpp>

#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <geometry_msgs/msg/detail/pose__struct.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>
#include <visualization_msgs/msg/detail/marker__struct.hpp>
#include <visualization_msgs/msg/detail/marker_array__struct.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace marker_utils::lane_change_markers
{
using autoware::universe_utils::createDefaultMarker;
using autoware::universe_utils::createMarkerScale;
using geometry_msgs::msg::Point;

MarkerArray showAllValidLaneChangePath(
  const std::vector<LaneChangePath> & lane_change_paths, std::string && ns)
{
  if (lane_change_paths.empty()) {
    return MarkerArray{};
  }

  MarkerArray marker_array;
  const auto current_time{rclcpp::Clock{RCL_ROS_TIME}.now()};

  const auto colors = colors::colors_list();
  const auto loop_size = std::min(lane_change_paths.size(), colors.size());
  marker_array.markers.reserve(loop_size);

  for (std::size_t idx = 0; idx < loop_size; ++idx) {
    int32_t id{0};
    const auto & lc_path = lane_change_paths.at(idx);
    if (lc_path.path.points.empty()) {
      continue;
    }
    std::string ns_with_idx = ns + "[" + std::to_string(idx) + "]";
    const auto & color = colors.at(idx);
    const auto & points = lc_path.path.points;
    auto marker = createDefaultMarker(
      "map", current_time, ns_with_idx, ++id, Marker::LINE_STRIP, createMarkerScale(0.1, 0.1, 0.0),
      color);
    marker.points.reserve(points.size());

    for (const auto & point : points) {
      marker.points.push_back(point.point.pose.position);
    }

    const auto & info = lc_path.info;
    auto text_marker = createDefaultMarker(
      "map", current_time, ns_with_idx, ++id, visualization_msgs::msg::Marker::TEXT_VIEW_FACING,
      createMarkerScale(0.1, 0.1, 0.8), colors::yellow());
    const auto prep_idx = points.size() / 4;
    text_marker.pose = points.at(prep_idx).point.pose;
    text_marker.pose.position.z += 2.0;
    text_marker.text = fmt::format(
      "vel: {vel:.3f}[m/s] | lon_acc: {lon_acc:.3f}[m/s2] | t: {time:.3f}[s] | L: {length:.3f}[m]",
      fmt::arg("vel", info.velocity.prepare),
      fmt::arg("lon_acc", info.longitudinal_acceleration.prepare),
      fmt::arg("time", info.duration.prepare), fmt::arg("length", info.length.prepare));
    marker_array.markers.push_back(text_marker);

    const auto lc_idx = points.size() / 2;
    text_marker.id = ++id;
    text_marker.pose = points.at(lc_idx).point.pose;
    text_marker.text = fmt::format(
      "type: {type} | vel: {vel:.3f}[m/s] | lon_acc: {lon_acc:.3f}[m/s2] | lat_acc: "
      "{lat_acc:.3f}[m/s2] | t: "
      "{time:.3f}[s] | L: {length:.3f}[m]",
      fmt::arg("type", magic_enum::enum_name(lc_path.type)),
      fmt::arg("vel", info.velocity.lane_changing),
      fmt::arg("lon_acc", info.longitudinal_acceleration.lane_changing),
      fmt::arg("lat_acc", info.lateral_acceleration), fmt::arg("time", info.duration.lane_changing),
      fmt::arg("length", info.length.lane_changing));
    marker_array.markers.push_back(text_marker);

    marker_array.markers.push_back(marker);
  }
  return marker_array;
}

MarkerArray createLaneChangingVirtualWallMarker(
  const geometry_msgs::msg::Pose & lane_changing_pose, const std::string & module_name,
  const rclcpp::Time & now, const std::string & ns)
{
  int32_t id{0};
  MarkerArray marker_array{};
  marker_array.markers.reserve(2);
  {
    auto wall_marker = createDefaultMarker(
      "map", now, ns + "virtual_wall", id, visualization_msgs::msg::Marker::CUBE,
      createMarkerScale(0.1, 5.0, 2.0), colors::green());
    wall_marker.pose = lane_changing_pose;
    wall_marker.pose.position.z += 1.0;
    marker_array.markers.push_back(wall_marker);
  }

  {
    auto text_marker = createDefaultMarker(
      "map", now, ns + "_text", id, visualization_msgs::msg::Marker::TEXT_VIEW_FACING,
      createMarkerScale(0.0, 0.0, 1.0), colors::white());
    text_marker.pose = lane_changing_pose;
    text_marker.pose.position.z += 2.0;
    text_marker.text = module_name;
    marker_array.markers.push_back(text_marker);
  }

  return marker_array;
}

MarkerArray showFilteredObjects(
  const FilteredLanesObjects & filtered_objects, const std::string & ns)
{
  int32_t update_id = 0;
  MarkerArray marker_array;
  auto reserve_size = filtered_objects.current_lane.size() + filtered_objects.others.size() +
                      filtered_objects.target_lane_leading.size() +
                      filtered_objects.target_lane_trailing.size();
  marker_array.markers.reserve(2 * reserve_size);
  auto add_objects_to_marker =
    [&](const ExtendedPredictedObjects & objects, const ColorRGBA & color) {
      if (objects.empty()) {
        return;
      }

      auto marker = marker_utils::showFilteredObjects(objects, ns, color, update_id);
      update_id += static_cast<int32_t>(marker.markers.size());
      std::move(
        marker.markers.begin(), marker.markers.end(), std::back_inserter(marker_array.markers));
    };

  add_objects_to_marker(filtered_objects.current_lane, colors::yellow());
  add_objects_to_marker(filtered_objects.target_lane_leading.moving, colors::aqua());
  add_objects_to_marker(filtered_objects.target_lane_leading.stopped, colors::light_steel_blue());
  add_objects_to_marker(filtered_objects.target_lane_trailing, colors::blue());
  add_objects_to_marker(
    filtered_objects.target_lane_leading.stopped_at_bound, colors::light_pink());
  add_objects_to_marker(filtered_objects.others, colors::medium_orchid());

  return marker_array;
}

MarkerArray showExecutionInfo(
  const InterfaceDebug & interface_debug_data, const Debug & scene_debug_data,
  const geometry_msgs::msg::Pose & ego_pose)
{
  auto default_text_marker = [&]() {
    return createDefaultMarker(
      "map", rclcpp::Clock{RCL_ROS_TIME}.now(), "execution_info", 0, Marker::TEXT_VIEW_FACING,
      createMarkerScale(0.5, 0.5, 0.5), colors::white());
  };

  MarkerArray marker_array;

  auto safety_check_info_text = default_text_marker();
  safety_check_info_text.pose = ego_pose;
  safety_check_info_text.pose.position.z += 4.0;
  const auto lc_state = interface_debug_data.lc_state;
  const auto & failing_reason = interface_debug_data.failing_reason;

  safety_check_info_text.text = fmt::format(
    "{stuck} | {return_lane} | {state} : {reason}",
    fmt::arg("stuck", scene_debug_data.is_stuck ? "is stuck" : ""),
    fmt::arg(
      "return_lane", scene_debug_data.is_able_to_return_to_current_lane ? "" : "can't return"),
    fmt::arg("state", magic_enum::enum_name(lc_state)), fmt::arg("reason", failing_reason));
  marker_array.markers.push_back(safety_check_info_text);
  return marker_array;
}

MarkerArray ShowLaneChangeMetricsInfo(
  const Debug & debug_data, const geometry_msgs::msg::Pose & pose)
{
  MarkerArray marker_array;

  auto text_marker = createDefaultMarker(
    "map", rclcpp::Clock{RCL_ROS_TIME}.now(), "sampling_metrics", 0, Marker::TEXT_VIEW_FACING,
    createMarkerScale(0.6, 0.6, 0.6), colors::yellow());
  text_marker.pose = autoware::universe_utils::calcOffsetPose(pose, 10.0, 15.0, 0.0);

  if (!debug_data.lane_change_metrics.empty()) {
    text_marker.text =
      fmt::format("{:<12}", "") + fmt::format("{:^18}|", "lat_accel[m/s2]") +
      fmt::format("{:^18}|", "lon_accel[m/s2]") + fmt::format("{:^17}|", "velocity[m/s]") +
      fmt::format("{:^15}|", "duration[s]") + fmt::format("{:^15}|", "length[m]") +
      fmt::format("{:^20}|", "max_length_th[m]") + fmt::format("{:^15}\n", "path_index");
    for (const auto & metrics : debug_data.lane_change_metrics) {
      text_marker.text += fmt::format("{:-<190}\n", "");
      const auto & p_m = metrics.prep_metric;
      text_marker.text +=
        fmt::format("{:<17}", "prep_metrics:") + fmt::format("{:^10.3f}", p_m.lat_accel) +
        fmt::format("{:^21.3f}", p_m.actual_lon_accel) + fmt::format("{:^12.3f}", p_m.velocity) +
        fmt::format("{:^15.3f}", p_m.duration) + fmt::format("{:^15.3f}", p_m.length) +
        fmt::format("{:^17.3f}", metrics.max_prepare_length) + fmt::format("{:^15}\n", "-");
      text_marker.text += fmt::format("{:<20}\n", "lc_metrics:");
      for (const auto & lc_m : metrics.lc_metrics) {
        const auto & metric = lc_m.first;
        const auto path_index = lc_m.second < 0 ? "-" : std::to_string(lc_m.second);
        text_marker.text += fmt::format("{:<15}", "") + fmt::format("{:^10.3f}", metric.lat_accel) +
                            fmt::format("{:^21.3f}", metric.actual_lon_accel) +
                            fmt::format("{:^12.3f}", metric.velocity) +
                            fmt::format("{:^15.3f}", metric.duration) +
                            fmt::format("{:^15.3f}", metric.length) +
                            fmt::format("{:^17.3f}", metrics.max_lane_changing_length) +
                            fmt::format("{:^15}\n", path_index);
      }
    }
    marker_array.markers.push_back(text_marker);
  }

  if (!debug_data.frenet_states.empty()) {
    text_marker.text = fmt::format("{:<12}", "") + fmt::format("{:^18}|", "lon_accel[m/s2]") +
                       fmt::format("{:^17}|", "lon_vel[m/s]") +
                       fmt::format("{:^15}|", "duration[s]") + fmt::format("{:^15}|", "length[m]") +
                       fmt::format("{:^17}|", "lat_accel[m/s2]") +
                       fmt::format("{:^15}|", "lat_vel[m/s2]") + fmt::format("{:^15}|", "s[m]") +
                       fmt::format("{:^15}|", "d[m]") + fmt::format("{:^20}\n", "max_length_th[m]");
    for (const auto & metrics : debug_data.frenet_states) {
      text_marker.text += fmt::format("{:-<250}\n", "");
      const auto & p_m = metrics.prep_metric;
      const auto max_len = metrics.max_lane_changing_length;
      text_marker.text +=
        fmt::format("{:<17}", "prep_metrics:") + fmt::format("{:^13.3f}", p_m.actual_lon_accel) +
        fmt::format("{:^15.3f}", p_m.velocity) + fmt::format("{:^15.3f}", p_m.duration) +
        fmt::format("{:^12.3f}", p_m.length) +
        fmt::format("{:^13}", "") +           // Empty string for lat_accel
        fmt::format("{:^13}", "") +           // Empty string for lat_vel
        fmt::format("{:^15}", "") +           // Empty string for s
        fmt::format("{:^15}", "") +           // Empty string for d // Empty string for d
        fmt::format("{:^20.3f}\n", max_len);  // Empty string for max_length_t
      const auto & lc_m = metrics.sampling_parameter.target_state;  // Assuming lc_metric exists
      const auto duration = metrics.sampling_parameter.target_duration;
      text_marker.text +=
        fmt::format("{:<17}", "frenet_state:") +
        fmt::format("{:^15.3f}", lc_m.longitudinal_acceleration) +
        fmt::format("{:^13.3f}", lc_m.longitudinal_velocity) + fmt::format("{:^17.3f}", duration) +
        fmt::format("{:^10.3f}", lc_m.position.s) +
        fmt::format("{:^19.3f}", lc_m.lateral_acceleration) +
        fmt::format("{:^10.3f}", lc_m.lateral_velocity) +
        fmt::format("{:^18.3f}", lc_m.position.s) + fmt::format("{:^15.3f}", lc_m.position.d) +
        fmt::format("{:^16.3f}\n", max_len);  // Empty string for max_length_t
    }

    marker_array.markers.push_back(text_marker);
  }

  return marker_array;
}

MarkerArray createDebugMarkerArray(
  const InterfaceDebug & interface_debug_data, const Debug & scene_debug_data,
  const geometry_msgs::msg::Pose & ego_pose)
{
  using lanelet::visualization::laneletsAsTriangleMarkerArray;
  using marker_utils::showPolygon;
  using marker_utils::showPredictedPath;
  using marker_utils::showSafetyCheckInfo;
  using marker_utils::lane_change_markers::showAllValidLaneChangePath;
  using marker_utils::lane_change_markers::showFilteredObjects;

  const auto & debug_collision_check_object = scene_debug_data.collision_check_objects;
  const auto & debug_collision_check_object_after_approval =
    scene_debug_data.collision_check_objects_after_approval;
  const auto & debug_valid_paths = scene_debug_data.valid_paths;
  const auto & debug_filtered_objects = scene_debug_data.filtered_objects;

  MarkerArray debug_marker;
  const auto add = [&debug_marker](const MarkerArray & added) {
    autoware::universe_utils::appendMarkerArray(added, &debug_marker);
  };

  if (!scene_debug_data.execution_area.points.empty()) {
    add(createPolygonMarkerArray(
      scene_debug_data.execution_area, "execution_area", 0, 0.16, 1.0, 0.69, 0.1));
  }

  add(showExecutionInfo(interface_debug_data, scene_debug_data, ego_pose));
  add(ShowLaneChangeMetricsInfo(scene_debug_data, ego_pose));

  // lanes
  add(laneletsAsTriangleMarkerArray(
    "current_lanes", scene_debug_data.current_lanes, colors::light_yellow(0.2)));
  add(laneletsAsTriangleMarkerArray(
    "target_lanes", scene_debug_data.target_lanes, colors::aqua(0.2)));
  add(laneletsAsTriangleMarkerArray(
    "target_backward_lanes", scene_debug_data.target_backward_lanes, colors::blue(0.2)));

  add(showAllValidLaneChangePath(debug_valid_paths, "lane_change_valid_paths"));
  add(showFilteredObjects(debug_filtered_objects, "object_filtered"));

  if (!debug_collision_check_object.empty()) {
    add(showSafetyCheckInfo(debug_collision_check_object, "collision_check_object_info"));
    add(showPredictedPath(debug_collision_check_object, "ego_predicted_path"));
    add(showPolygon(debug_collision_check_object, "ego_and_target_polygon_relation"));
  }

  if (!debug_collision_check_object_after_approval.empty()) {
    add(showSafetyCheckInfo(
      debug_collision_check_object_after_approval, "object_debug_info_after_approval"));
    add(showPredictedPath(
      debug_collision_check_object_after_approval, "ego_predicted_path_after_approval"));
    add(showPolygon(
      debug_collision_check_object_after_approval,
      "ego_and_target_polygon_relation_after_approval"));
  }

  return debug_marker;
}
}  // namespace marker_utils::lane_change_markers

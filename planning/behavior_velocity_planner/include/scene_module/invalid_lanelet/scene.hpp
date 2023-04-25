// Copyright 2023 TIER IV, Inc.
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

#ifndef SCENE_MODULE__INVALID_LANELET__SCENE_HPP_
#define SCENE_MODULE__INVALID_LANELET__SCENE_HPP_

#include "scene_module/invalid_lanelet/util.hpp"
#include "scene_module/scene_module_interface.hpp"
#include "utilization/boost_geometry_helper.hpp"

#include <rclcpp/rclcpp.hpp>

#include <autoware_auto_planning_msgs/msg/path_with_lane_id.hpp>

#include <boost/optional.hpp>

#include <lanelet2_core/LaneletMap.h>

#include <memory>
#include <utility>
#include <vector>

namespace behavior_velocity_planner
{
using autoware_auto_planning_msgs::msg::PathWithLaneId;

class InvalidLaneletModule : public SceneModuleInterface
{
public:
  enum class State { INIT, APPROACHING, INSIDE_INVALID_LANELET, STOPPED };

  struct SegmentIndexWithPose
  {
    size_t index;
    geometry_msgs::msg::Pose pose;
  };

  struct DebugData
  {
    double base_link2front;
    PathWithInvalidLaneletPolygonIntersection path_polygon_intersection;
    std::vector<geometry_msgs::msg::Point> invalid_lanelet_polygon;
    geometry_msgs::msg::Pose stop_pose;
  };

  struct PlannerParam
  {
    double stop_margin;
    bool print_debug_info;
  };

  InvalidLaneletModule(
    const int64_t module_id, const int64_t lane_id, const PlannerParam & planner_param,
    const rclcpp::Logger logger, const rclcpp::Clock::SharedPtr clock);

  bool modifyPathVelocity(PathWithLaneId * path, StopReason * stop_reason) override;

  visualization_msgs::msg::MarkerArray createDebugMarkerArray() override;
  visualization_msgs::msg::MarkerArray createVirtualWallMarkerArray() override;

private:
  const int64_t lane_id_;

  // Parameter
  PlannerParam planner_param_;

  // Debug
  DebugData debug_data_;

  // State machine
  State state_;

  PathWithInvalidLaneletPolygonIntersection path_invalid_lanelet_polygon_intersection;
  geometry_msgs::msg::Point first_intersection_point;
  double distance_ego_first_intersection;

  std::shared_ptr<motion_utils::VirtualWallMarkerCreator> virtual_wall_marker_creator_ =
    std::make_shared<motion_utils::VirtualWallMarkerCreator>();

  void handle_init_state();
  void handle_approaching_state(PathWithLaneId * path, StopReason * stop_reason);
  void handle_inside_invalid_lanelet_state(PathWithLaneId * path, StopReason * stop_reason);
  void handle_stopped_state(PathWithLaneId * path, StopReason * stop_reason);
  void initialize_debug_data(
    const lanelet::Lanelet & invalid_lanelet, const geometry_msgs::msg::Point & ego_pos);
};
}  // namespace behavior_velocity_planner

#endif  // SCENE_MODULE__INVALID_LANELET__SCENE_HPP_

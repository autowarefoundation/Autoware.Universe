// Copyright 2021 Tier IV, Inc.
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

#ifndef BEHAVIOR_PATH_PLANNER__SCENE_MODULE__AVOIDANCE__DEBUG_HPP_
#define BEHAVIOR_PATH_PLANNER__SCENE_MODULE__AVOIDANCE__DEBUG_HPP_

#include <string>
#include <vector>

#include "lanelet2_core/LaneletMap.h"
#include "lanelet2_routing/RoutingGraph.h"

#include "autoware_perception_msgs/msg/dynamic_object_array.hpp"
#include "autoware_planning_msgs/msg/path_with_lane_id.hpp"
#include "autoware_utils/ros/marker_helper.hpp"
#include "geometry_msgs/msg/polygon.hpp"
#include "visualization_msgs/msg/marker_array.hpp"

#include "behavior_path_planner/scene_module/avoidance/avoidance_module.hpp"
#include "behavior_path_planner/path_shifter/path_shifter.hpp"

namespace marker_utils
{
using autoware_perception_msgs::msg::DynamicObjectArray;
using autoware_planning_msgs::msg::PathWithLaneId;
using behavior_path_planner::Frenet;
using behavior_path_planner::ShiftPoint;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Polygon;
using geometry_msgs::msg::Pose;
using visualization_msgs::msg::MarkerArray;

MarkerArray createShiftPointMarkerArray(
  const std::vector<ShiftPoint> & shift_points, const double base_shift,
  const std::string & ns, const double r, const double g, const double b);

MarkerArray createFrenetPointMarkerArray(
  const std::vector<Frenet> & frenet_points,
  const PathWithLaneId & path, const Point & ego_point,
  const std::string & ns, const double r, const double g, const double b);

MarkerArray createLaneletsAreaMarkerArray(
  const std::vector<lanelet::ConstLanelet> & lanelets, const std::string & ns,
  const double r, const double g, const double b);

MarkerArray createLaneletPolygonsMarkerArray(
  const std::vector<lanelet::CompoundPolygon3d> & polygons, const std::string & ns,
  const int64_t lane_id);

MarkerArray createPolygonMarkerArray(
  const Polygon & polygon, const std::string & ns, const int64_t lane_id,
  const double r, const double g, const double b);

MarkerArray createObjectsMarkerArray(
  const DynamicObjectArray & objects, const std::string & ns,
  const int64_t lane_id, const double r, const double g, const double b);

MarkerArray createPathMarkerArray(
  const PathWithLaneId & path, const std::string & ns,
  const int64_t lane_id, const double r, const double g, const double b);

MarkerArray createVirtualWallMarkerArray(
  const Pose & pose, const int64_t lane_id, const std::string & stop_factor);

MarkerArray createPoseMarkerArray(
  const Pose & pose, const std::string & ns, const int64_t id, const double r,
  const double g, const double b);

}  // namespace marker_utils

#endif  // BEHAVIOR_PATH_PLANNER__SCENE_MODULE__AVOIDANCE__DEBUG_HPP_

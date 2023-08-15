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

#include "behavior_path_planner/utils/start_planner/util.hpp"

#include "behavior_path_planner/utils/create_vehicle_footprint.hpp"
#include "behavior_path_planner/utils/path_shifter/path_shifter.hpp"
#include "behavior_path_planner/utils/path_utils.hpp"
#include "behavior_path_planner/utils/utils.hpp"

#include <lanelet2_extension/utility/utilities.hpp>
#include <rclcpp/rclcpp.hpp>
#include <tier4_autoware_utils/geometry/boost_geometry.hpp>

#include <boost/geometry/algorithms/dispatch/distance.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <tf2/utils.h>
#include <tf2_ros/transform_listener.h>

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace behavior_path_planner::start_planner_utils
{
PathWithLaneId combineReferencePath(const PathWithLaneId path1, const PathWithLaneId path2)
{
  PathWithLaneId path;
  path.points.insert(path.points.end(), path1.points.begin(), path1.points.end());

  // skip overlapping point
  path.points.insert(path.points.end(), next(path2.points.begin()), path2.points.end());

  PathWithLaneId filtered_path = path;
  filtered_path.points = motion_utils::removeOverlapPoints(filtered_path.points);
  return filtered_path;
}

PathWithLaneId getBackwardPath(
  const RouteHandler & route_handler, const lanelet::ConstLanelets & shoulder_lanes,
  const Pose & current_pose, const Pose & backed_pose, const double velocity)
{
  const auto current_pose_arc_coords =
    lanelet::utils::getArcCoordinates(shoulder_lanes, current_pose);
  const auto backed_pose_arc_coords =
    lanelet::utils::getArcCoordinates(shoulder_lanes, backed_pose);

  const double s_start = backed_pose_arc_coords.length;
  const double s_end = current_pose_arc_coords.length;

  PathWithLaneId backward_path;
  {
    // forward center line path
    backward_path = route_handler.getCenterLinePath(shoulder_lanes, s_start, s_end, true);

    // backward center line path
    std::reverse(backward_path.points.begin(), backward_path.points.end());
    for (auto & p : backward_path.points) {
      p.point.longitudinal_velocity_mps = velocity;
    }
    backward_path.points.back().point.longitudinal_velocity_mps = 0.0;

    // lateral shift to current_pose
    const double lateral_distance_to_shoulder_center = current_pose_arc_coords.distance;
    for (size_t i = 0; i < backward_path.points.size(); ++i) {
      auto & p = backward_path.points.at(i).point.pose;
      p = tier4_autoware_utils::calcOffsetPose(p, 0, lateral_distance_to_shoulder_center, 0);
    }
  }

  return backward_path;
}

Pose getBackedPose(
  const Pose & current_pose, const double & yaw_shoulder_lane, const double & back_distance)
{
  Pose backed_pose;
  backed_pose = current_pose;
  backed_pose.position.x -= std::cos(yaw_shoulder_lane) * back_distance;
  backed_pose.position.y -= std::sin(yaw_shoulder_lane) * back_distance;

  return backed_pose;
}

lanelet::ConstLanelets getPullOutLanes(
  const std::shared_ptr<const PlannerData> & planner_data, const double backward_length)
{
  const double & vehicle_width = planner_data->parameters.vehicle_width;
  const auto & route_handler = planner_data->route_handler;
  const auto & current_pose = planner_data->self_odometry->pose.pose;

  lanelet::ConstLanelet current_shoulder_lane;
  lanelet::ConstLanelets shoulder_lanes;
  if (route_handler->getPullOutStartLane(
        route_handler->getShoulderLanelets(), current_pose, vehicle_width,
        &current_shoulder_lane)) {
    // pull out from shoulder lane
    return route_handler->getShoulderLaneletSequence(current_shoulder_lane, current_pose);
  }

  // pull out from road lane
  return utils::getExtendedCurrentLanes(
    planner_data, backward_length,
    /*forward_length*/ std::numeric_limits<double>::max(),
    /*forward_only_in_route*/ true);
}

std::pair<double, bool> calcEndArcLength(
  const double s_start, const double forward_path_length, const lanelet::ConstLanelets & road_lanes,
  const Pose & goal_pose)
{
  const double s_forward_length = s_start + forward_path_length;
  // early return if the goal pose is not in the lanelets
  if (!utils::isInLanelets(goal_pose, road_lanes)) {
    return {s_forward_length, false};
  }

  const double s_goal = lanelet::utils::getArcCoordinates(road_lanes, goal_pose).length;

  // If the goal is behind ego or beyond the forward length, return early
  if (s_goal < s_start || s_goal >= s_forward_length) {
    return {s_forward_length, false};
  }

  // path end is goal ..?
  return {s_goal, true};
}

}  // namespace behavior_path_planner::start_planner_utils

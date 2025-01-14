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

#ifndef AUTOWARE__LANE_DEPARTURE_CHECKER__PARAMETERS_HPP_
#define AUTOWARE__LANE_DEPARTURE_CHECKER__PARAMETERS_HPP_

#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/pose_deviation.hpp>
#include <rclcpp/node.hpp>

#include <autoware_planning_msgs/msg/lanelet_route.hpp>
#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <autoware_planning_msgs/msg/trajectory_point.hpp>
#include <nav_msgs/msg/odometry.hpp>

#include <lanelet2_core/LaneletMap.h>

#include <map>
#include <string>
#include <vector>

namespace autoware::lane_departure_checker
{
using autoware::universe_utils::PoseDeviation;
using autoware_planning_msgs::msg::LaneletRoute;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;
using TrajectoryPoints = std::vector<TrajectoryPoint>;
using autoware::universe_utils::LinearRing2d;

struct Param
{
  static Param init(rclcpp::Node & node);
  double footprint_margin_scale{};
  double footprint_extra_margin{};
  double resample_interval{};
  double max_deceleration{};
  double delay_time{};
  double max_lateral_deviation{};
  double max_longitudinal_deviation{};
  double max_yaw_deviation_deg{};
  double min_braking_distance{};
  // nearest search to ego
  double ego_nearest_dist_threshold{};
  double ego_nearest_yaw_threshold{};
};

struct NodeParam
{
  static NodeParam init(rclcpp::Node & node);
  bool will_out_of_lane_checker{};
  bool out_of_lane_checker{};
  bool boundary_departure_checker{};

  double update_rate{};
  bool visualize_lanelet{};
  bool include_right_lanes{};
  bool include_left_lanes{};
  bool include_opposite_lanes{};
  bool include_conflicting_lanes{};
  std::vector<std::string> boundary_types_to_detect{};
};

struct Input
{
  nav_msgs::msg::Odometry::ConstSharedPtr current_odom{};
  lanelet::LaneletMapPtr lanelet_map{};
  LaneletRoute::ConstSharedPtr route{};
  lanelet::ConstLanelets route_lanelets{};
  lanelet::ConstLanelets shoulder_lanelets{};
  Trajectory::ConstSharedPtr reference_trajectory{};
  Trajectory::ConstSharedPtr predicted_trajectory{};
  std::vector<std::string> boundary_types_to_detect{};
};

struct Output
{
  std::map<std::string, double> processing_time_map{};
  bool will_leave_lane{};
  bool is_out_of_lane{};
  bool will_cross_boundary{};
  PoseDeviation trajectory_deviation{};
  lanelet::ConstLanelets candidate_lanelets{};
  TrajectoryPoints resampled_trajectory{};
  std::vector<LinearRing2d> vehicle_footprints{};
  std::vector<LinearRing2d> vehicle_passing_areas{};
};
}  // namespace autoware::lane_departure_checker

#endif  // AUTOWARE__LANE_DEPARTURE_CHECKER__PARAMETERS_HPP_

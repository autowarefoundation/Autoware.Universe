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

#ifndef AUTOWARE__PATH_GENERATOR__PLANNER_DATA_HPP_
#define AUTOWARE__PATH_GENERATOR__PLANNER_DATA_HPP_

#include <autoware_planning_msgs/msg/lanelet_route.hpp>

#include <lanelet2_routing/RoutingGraph.h>
#include <lanelet2_traffic_rules/TrafficRules.h>

namespace autoware::path_generator
{
struct PlannerData
{
  lanelet::LaneletMapPtr lanelet_map_ptr{nullptr};
  lanelet::traffic_rules::TrafficRulesPtr traffic_rules_ptr{nullptr};
  lanelet::routing::RoutingGraphPtr routing_graph_ptr{nullptr};

  geometry_msgs::msg::Pose current_pose;

  autoware_planning_msgs::msg::LaneletRoute::ConstSharedPtr route_ptr{nullptr};

  lanelet::ConstLanelets route_lanelets;
  lanelet::ConstLanelets preferred_lanelets;
  lanelet::ConstLanelets start_lanelets;
  lanelet::ConstLanelets goal_lanelets;

  double forward_path_length;
  double backward_path_length;
  double input_path_interval;
  double enable_akima_spline_first;
  double ego_nearest_dist_threshold;
  double ego_nearest_yaw_threshold;
};
}  // namespace autoware::path_generator

#endif  // AUTOWARE__PATH_GENERATOR__PLANNER_DATA_HPP_
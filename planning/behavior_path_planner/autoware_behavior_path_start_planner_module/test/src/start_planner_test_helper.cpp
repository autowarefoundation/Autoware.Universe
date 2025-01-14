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
// pull_out_test_utils.cpp
#include "start_planner_test_helper.hpp"

#include <autoware_planning_test_manager/autoware_planning_test_manager_utils.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <memory>
#include <vector>

namespace autoware::behavior_path_planner::testing
{
using autoware::test_utils::get_absolute_path_to_config;
using autoware_planning_test_manager::utils::makeBehaviorRouteFromLaneId;

rclcpp::NodeOptions StartPlannerTestHelper::make_node_options()
{
  // Load common configuration files
  auto node_options = rclcpp::NodeOptions{};

  const auto common_param_path =
    get_absolute_path_to_config("autoware_test_utils", "test_common.param.yaml");
  const auto nearest_search_param_path =
    get_absolute_path_to_config("autoware_test_utils", "test_nearest_search.param.yaml");
  const auto vehicle_info_param_path =
    get_absolute_path_to_config("autoware_test_utils", "test_vehicle_info.param.yaml");
  const auto behavior_path_planner_param_path = get_absolute_path_to_config(
    "autoware_behavior_path_planner", "behavior_path_planner.param.yaml");
  const auto drivable_area_expansion_param_path = get_absolute_path_to_config(
    "autoware_behavior_path_planner", "drivable_area_expansion.param.yaml");
  const auto scene_module_manager_param_path = get_absolute_path_to_config(
    "autoware_behavior_path_planner", "scene_module_manager.param.yaml");
  const auto start_planner_param_path = get_absolute_path_to_config(
    "autoware_behavior_path_start_planner_module", "start_planner.param.yaml");

  autoware::test_utils::updateNodeOptions(
    node_options, {common_param_path, nearest_search_param_path, vehicle_info_param_path,
                   behavior_path_planner_param_path, drivable_area_expansion_param_path,
                   scene_module_manager_param_path, start_planner_param_path});

  return node_options;
}

void StartPlannerTestHelper::set_odometry(
  std::shared_ptr<PlannerData> & planner_data, const geometry_msgs::msg::Pose & start_pose)
{
  auto odometry = std::make_shared<nav_msgs::msg::Odometry>();
  odometry->pose.pose = start_pose;
  odometry->header.frame_id = "map";
  planner_data->self_odometry = odometry;
}

void StartPlannerTestHelper::set_route(
  std::shared_ptr<PlannerData> & planner_data, const int route_start_lane_id,
  const int route_goal_lane_id)
{
  const auto shoulder_map_path = autoware::test_utils::get_absolute_path_to_lanelet_map(
    "autoware_test_utils", "road_shoulder/lanelet2_map.osm");
  const auto map_bin_msg = autoware::test_utils::make_map_bin_msg(shoulder_map_path, 0.5);
  auto route_handler = std::make_shared<autoware::route_handler::RouteHandler>(map_bin_msg);

  const auto route = makeBehaviorRouteFromLaneId(
    route_start_lane_id, route_goal_lane_id, "autoware_test_utils",
    "road_shoulder/lanelet2_map.osm");
  route_handler->setRoute(route);
  planner_data->route_handler = route_handler;
}

void StartPlannerTestHelper::set_costmap(
  std::shared_ptr<PlannerData> & planner_data, const geometry_msgs::msg::Pose & start_pose,
  const double grid_resolution, const double grid_length_x, const double grid_length_y)
{
  nav_msgs::msg::OccupancyGrid costmap;
  costmap.header.frame_id = "map";
  costmap.info.width = static_cast<uint>(grid_length_x / grid_resolution);
  costmap.info.height = static_cast<uint>(grid_length_y / grid_resolution);
  costmap.info.resolution = grid_resolution;

  costmap.info.origin.position.x = start_pose.position.x - grid_length_x / 2;
  costmap.info.origin.position.y = start_pose.position.y - grid_length_y / 2;
  costmap.data = std::vector<int8_t>(costmap.info.width * costmap.info.height, 0);

  planner_data->costmap = std::make_shared<nav_msgs::msg::OccupancyGrid>(costmap);
}

}  // namespace autoware::behavior_path_planner::testing

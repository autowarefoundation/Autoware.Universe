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
#pragma once

#include <autoware/behavior_path_start_planner_module/start_planner_module.hpp>
#include <rclcpp/rclcpp.hpp>

#include <memory>

namespace autoware::behavior_path_planner::testing
{

class StartPlannerTestHelper
{
public:
  static rclcpp::NodeOptions make_node_options();

  static void set_odometry(
    std::shared_ptr<PlannerData> & planner_data, const geometry_msgs::msg::Pose & start_pose);
  static void set_route(
    std::shared_ptr<PlannerData> & planner_data, const int route_start_lane_id,
    const int route_goal_lane_id);
  static void set_costmap(
    std::shared_ptr<PlannerData> & planner_data, const geometry_msgs::msg::Pose & start_pose,
    const double grid_resolution, const double grid_length_x, const double grid_length_y);
};

}  // namespace autoware::behavior_path_planner::testing

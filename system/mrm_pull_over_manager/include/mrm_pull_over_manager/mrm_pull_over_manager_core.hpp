// Copyright 2023 Tier IV, Inc.
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

#ifndef MRM_PULL_OVER_MANAGER__MRM_PULL_OVER_MANAGER_CORE_HPP_
#define MRM_PULL_OVER_MANAGER__MRM_PULL_OVER_MANAGER_CORE_HPP_

// C++
#include <map>
#include <memory>
#include <vector>

// ROS 2
#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/pose_array.hpp>
#include <nav_msgs/msg/odometry.hpp>

// Autoware
#include <motion_utils/trajectory/trajectory.hpp>
#include <route_handler/route_handler.hpp>

#include <autoware_auto_mapping_msgs/msg/had_map_bin.hpp>
#include <autoware_auto_planning_msgs/msg/trajectory.hpp>
#include <tier4_system_msgs/srv/activate_pull_over.hpp>

// lanelet
#include <lanelet2_extension/regulatory_elements/Forward.hpp>
#include <lanelet2_extension/utility/message_conversion.hpp>
#include <lanelet2_extension/utility/query.hpp>

#include <lanelet2_core/Attribute.h>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/LineStringOrPolygon.h>
#include <lanelet2_routing/RoutingGraph.h>
#include <lanelet2_routing/RoutingGraphContainer.h>
#include <lanelet2_traffic_rules/TrafficRulesFactory.h>

namespace mrm_pull_over_manager
{
class MrmPullOverManager : public rclcpp::Node
{
public:
  MrmPullOverManager();

private:
  using Odometry = nav_msgs::msg::Odometry;
  using HADMapBin = autoware_auto_mapping_msgs::msg::HADMapBin;
  using LaneletRoute = autoware_planning_msgs::msg::LaneletRoute;
  using PoseLaneIdMap = std::map<lanelet::Id, geometry_msgs::msg::Pose>;
  using PoseArray = geometry_msgs::msg::PoseArray;
  using Trajectory = autoware_auto_planning_msgs::msg::Trajectory;

  // Subscribtoers
  rclcpp::Subscription<Odometry>::SharedPtr sub_odom_;
  rclcpp::Subscription<LaneletRoute>::SharedPtr sub_route_;
  rclcpp::Subscription<HADMapBin>::SharedPtr sub_map_;
  rclcpp::Subscription<Trajectory>::SharedPtr sub_trajectory_;

  Odometry::ConstSharedPtr odom_;
  route_handler::RouteHandler route_handler_;
  Trajectory::ConstSharedPtr trajectory_;

  void on_odometry(const Odometry::ConstSharedPtr msg);
  void on_route(const LaneletRoute::ConstSharedPtr msg);
  void on_map(const HADMapBin::ConstSharedPtr msg);
  void on_trajectory(const Trajectory::ConstSharedPtr msg);

  // Server
  rclcpp::Service<tier4_system_msgs::srv::ActivatePullOver>::SharedPtr activate_pull_over_;

  void activatePullOver(
    const tier4_system_msgs::srv::ActivatePullOver::Request::SharedPtr request,
    const tier4_system_msgs::srv::ActivatePullOver::Response::SharedPtr response);

  // Publisher
  rclcpp::Publisher<PoseArray>::SharedPtr pub_pose_array_;

  // TODO: temporary for debug
  // Timer
  rclcpp::TimerBase::SharedPtr timer_;
  void on_timer();

  // Parameters
  // Param param_;

  PoseLaneIdMap candidate_goals_;

  // Algorithm
  bool is_data_ready();

  /**
   * @brief Find the goals within the lanelet and publish them
   */
  bool find_nearby_goals();

  /**
   * @brief Find the goals that have the same lanelet id with the candidate_lanelets
   * @param candidate_lanelets
   * @return
   */
  PoseArray find_goals_in_lanelets(const lanelet::ConstLanelets & candidate_lanelets) const;

  /**
   * @brief Find the goals that have the same lanelet id with the candidate_lanelets
   * @param poses Poses to be filtered
   * @return Filtered poses
   */
  std::vector<geometry_msgs::msg::Pose> filter_nearby_goals(
    const std::vector<geometry_msgs::msg::Pose> & poses);
};
}  // namespace mrm_pull_over_manager

#endif  // MRM_PULL_OVER_MANAGER__MRM_PULL_OVER_MANAGER_CORE_HPP_

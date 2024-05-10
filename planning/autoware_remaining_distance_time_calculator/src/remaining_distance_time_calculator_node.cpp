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

#include "remaining_distance_time_calculator_node.hpp"

#include <lanelet2_extension/utility/message_conversion.hpp>
#include <lanelet2_extension/utility/query.hpp>
#include <lanelet2_extension/utility/utilities.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/timer.hpp>
#include <tier4_autoware_utils/geometry/geometry.hpp>

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace autoware::remaining_distance_time_calculator
{

RemainingDistanceTimeCalculatorNode::RemainingDistanceTimeCalculatorNode(
  const rclcpp::NodeOptions & options)
: Node("remaining_distance_time_calculator", options),
  is_graph_ready_{false},
  has_received_route_{false}
{
  using std::placeholders::_1;

  sub_odometry_ = create_subscription<Odometry>(
    "~/input/odometry", 1, std::bind(&RemainingDistanceTimeCalculatorNode::on_odometry, this, _1));

  const auto qos_transient_local = rclcpp::QoS{1}.transient_local();

  sub_map_ = create_subscription<HADMapBin>(
    "~/input/map", qos_transient_local,
    std::bind(&RemainingDistanceTimeCalculatorNode::on_map, this, _1));
  sub_route_ = create_subscription<LaneletRoute>(
    "~/input/route", qos_transient_local,
    std::bind(&RemainingDistanceTimeCalculatorNode::on_route, this, _1));

  pub_mission_remaining_distance_time_ = create_publisher<MissionRemainingDistanceTime>(
    "~/output/mission_remaining_distance_time",
    rclcpp::QoS(rclcpp::KeepLast(10)).durability_volatile().reliable());

  node_param_.update_rate = declare_parameter<double>("update_rate");

  const auto period_ns = rclcpp::Rate(node_param_.update_rate).period();
  timer_ = rclcpp::create_timer(
    this, get_clock(), period_ns, std::bind(&RemainingDistanceTimeCalculatorNode::on_timer, this));
}

void RemainingDistanceTimeCalculatorNode::on_map(const HADMapBin::ConstSharedPtr & msg)
{
  route_handler_.setMap(*msg);
  lanelet_map_ptr_ = std::make_shared<lanelet::LaneletMap>();
  lanelet::utils::conversion::fromBinMsg(
    *msg, lanelet_map_ptr_, &traffic_rules_ptr_, &routing_graph_ptr_);
  lanelet::ConstLanelets all_lanelets = lanelet::utils::query::laneletLayer(lanelet_map_ptr_);
  road_lanelets_ = lanelet::utils::query::roadLanelets(all_lanelets);
  is_graph_ready_ = true;
}

void RemainingDistanceTimeCalculatorNode::on_odometry(const Odometry::ConstSharedPtr & msg)
{
  current_vehicle_pose_ = msg->pose.pose;
  current_vehicle_velocity_ = msg->twist.twist.linear;
}

void RemainingDistanceTimeCalculatorNode::on_route(const LaneletRoute::ConstSharedPtr & msg)
{
  goal_pose_ = msg->goal_pose;
  has_received_route_ = true;
}

void RemainingDistanceTimeCalculatorNode::on_timer()
{
  if (is_graph_ready_ && has_received_route_) {
    double remaining_distance = calculate_remaining_distance();
    double remaining_time = calculate_remaining_time(remaining_distance);
    publish_mission_remaining_distance_time(remaining_distance, remaining_time);
  }
}

double RemainingDistanceTimeCalculatorNode::calculate_remaining_distance() const
{
  double remaining_distance = 0.0;
  size_t index = 0;

  lanelet::ConstLanelet current_lanelet;
  lanelet::utils::query::getClosestLanelet(road_lanelets_, current_vehicle_pose_, &current_lanelet);

  lanelet::ConstLanelet goal_lanelet;
  lanelet::utils::query::getClosestLanelet(road_lanelets_, goal_pose_, &goal_lanelet);

  const lanelet::Optional<lanelet::routing::Route> optional_route =
    routing_graph_ptr_->getRoute(current_lanelet, goal_lanelet, 0);

  lanelet::routing::LaneletPath remaining_shortest_path;
  remaining_shortest_path = optional_route->shortestPath();

  for (auto & llt : remaining_shortest_path) {
    if (remaining_shortest_path.size() == 1) {
      remaining_distance +=
        tier4_autoware_utils::calcDistance2d(current_vehicle_pose_.position, goal_pose_.position);
      break;
    }

    if (index == 0) {
      lanelet::ArcCoordinates arc_coord =
        lanelet::utils::getArcCoordinates({llt}, current_vehicle_pose_);
      double this_lanelet_length = lanelet::utils::getLaneletLength2d(llt);
      remaining_distance += this_lanelet_length - arc_coord.length;
    } else if (index == (remaining_shortest_path.size() - 1)) {
      lanelet::ArcCoordinates arc_coord = lanelet::utils::getArcCoordinates({llt}, goal_pose_);
      remaining_distance += arc_coord.length;
    } else {
      remaining_distance += lanelet::utils::getLaneletLength2d(llt);
    }

    index++;
  }

  return remaining_distance;
}

double RemainingDistanceTimeCalculatorNode::calculate_remaining_time(
  const double remaining_distance) const
{
  double current_velocity_norm = std::sqrt(
    current_vehicle_velocity_.x * current_vehicle_velocity_.x +
    current_vehicle_velocity_.y * current_vehicle_velocity_.y);

  if (remaining_distance < 0.01 || current_velocity_norm < 0.01) {
    return 0.0;
  }

  double remaining_time = remaining_distance / current_velocity_norm;

  return remaining_time;
}

void RemainingDistanceTimeCalculatorNode::publish_mission_remaining_distance_time(
  const double remaining_distance, const double remaining_time) const
{
  MissionRemainingDistanceTime mission_remaining_distance_time;

  mission_remaining_distance_time.remaining_distance = remaining_distance;
  mission_remaining_distance_time.remaining_time = remaining_time;
  pub_mission_remaining_distance_time_->publish(mission_remaining_distance_time);
}

}  // namespace autoware::remaining_distance_time_calculator

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(
  autoware::remaining_distance_time_calculator::RemainingDistanceTimeCalculatorNode)

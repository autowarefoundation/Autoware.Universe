// Copyright 2019 Autoware Foundation
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

#include "mission_planner.hpp"

#include "type_conversion.hpp"

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

namespace mission_planner
{

MissionPlanner::MissionPlanner(const rclcpp::NodeOptions & options)
: Node("mission_planner", options),
  arrival_checker_(this),
  plugin_loader_("mission_planner", "mission_planner::PlannerPlugin"),
  tf_buffer_(get_clock()),
  tf_listener_(tf_buffer_)
{
  map_frame_ = declare_parameter<std::string>("map_frame");
  base_link_frame_ = declare_parameter<std::string>("base_link_frame");

  planner_ = plugin_loader_.createSharedInstance("mission_planner::lanelet2::DefaultPlanner");
  planner_->initialize(this);

  const auto durable_qos = rclcpp::QoS(1).transient_local();
  pub_had_route_ = create_publisher<HADMapRoute>("output/route", durable_qos);
  pub_marker_ = create_publisher<MarkerArray>("debug/route_marker", durable_qos);

  const auto period = rclcpp::Duration::from_seconds(1.0);
  timer_ = rclcpp::create_timer(this, get_clock(), period, [this]() { on_arrival_check(); });

  const auto adaptor = component_interface_utils::NodeAdaptor(this);
  adaptor.init_pub(pub_state_);
  adaptor.init_pub(pub_api_route_);
  adaptor.init_srv(srv_clear_route_, this, &MissionPlanner::on_clear_route);
  adaptor.init_srv(srv_set_route_, this, &MissionPlanner::on_set_route);
  adaptor.init_srv(srv_set_route_points_, this, &MissionPlanner::on_set_route_points);

  change_state(RouteState::Message::UNSET);
}

PoseStamped MissionPlanner::get_ego_vehicle_pose()
{
  PoseStamped base_link_origin;
  base_link_origin.header.frame_id = base_link_frame_;
  base_link_origin.pose.position.x = 0;
  base_link_origin.pose.position.y = 0;
  base_link_origin.pose.position.z = 0;
  base_link_origin.pose.orientation.x = 0;
  base_link_origin.pose.orientation.y = 0;
  base_link_origin.pose.orientation.z = 0;
  base_link_origin.pose.orientation.w = 1;

  //  transform base_link frame origin to map_frame to get vehicle positions
  return transform_pose(base_link_origin);
}

PoseStamped MissionPlanner::transform_pose(const PoseStamped & input)
{
  PoseStamped output;
  geometry_msgs::msg::TransformStamped transform;
  try {
    transform = tf_buffer_.lookupTransform(map_frame_, input.header.frame_id, tf2::TimePointZero);
    tf2::doTransform(input, output, transform);
    return output;
  } catch (tf2::TransformException & error) {
    throw component_interface_utils::TransformError(error.what());
  }
}

void MissionPlanner::on_arrival_check()
{
  // NOTE: Do not check in the changing state as goal may change.
  if (state_.state == RouteState::Message::SET) {
    if (arrival_checker_.is_arrived(get_ego_vehicle_pose())) {
      change_state(RouteState::Message::ARRIVED);
    }
  }
}

void MissionPlanner::change_route()
{
  arrival_checker_.reset_goal();
  pub_api_route_->publish(conversion::create_empty_route(now()));
}

void MissionPlanner::change_route(const HADMapRoute & route)
{
  // TODO(Takagi, Isamu): replace when modified goal is always published
  // arrival_checker_.reset_goal();
  PoseStamped goal;
  goal.header = route.header;
  goal.pose = route.goal_pose;
  arrival_checker_.reset_goal(goal);

  pub_api_route_->publish(conversion::convert_route(route));
  pub_had_route_->publish(route);
  pub_marker_->publish(planner_->visualize(route));
}

void MissionPlanner::change_state(RouteState::Message::_state_type state)
{
  state_.stamp = now();
  state_.state = state;
  pub_state_->publish(state_);
}

void MissionPlanner::on_clear_route(
  const ClearRoute::Service::Request::SharedPtr, const ClearRoute::Service::Response::SharedPtr res)
{
  // NOTE: The route services should be mutually exclusive by callback group.
  RCLCPP_INFO_STREAM(get_logger(), "ClearRoute");

  change_route();
  change_state(RouteState::Message::UNSET);
  res->status.success = true;
}

void MissionPlanner::on_set_route(
  const SetRoute::Service::Request::SharedPtr req, const SetRoute::Service::Response::SharedPtr res)
{
  // NOTE: The route services should be mutually exclusive by callback group.
  if (state_.state != RouteState::Message::UNSET) {
    throw component_interface_utils::ServiceException(
      SetRoute::Service::Response::ERROR_ROUTE_EXISTS, "The route is already set.");
  }

  // Use common header for transforms
  PoseStamped pose;
  pose.header = req->header;
  pose.pose = req->goal;

  // Convert route points.
  autoware_ad_api_msgs::msg::RouteData data;
  data.start = get_ego_vehicle_pose().pose;
  data.goal = transform_pose(pose).pose;
  data.segments = req->segments;

  // Convert route.
  HADMapRoute route = conversion::convert_route(data);
  route.header.stamp = req->header.stamp;
  route.header.frame_id = map_frame_;

  // Update route.
  change_route(route);
  change_state(RouteState::Message::SET);
  res->status.success = true;
}

void MissionPlanner::on_set_route_points(
  const SetRoutePoints::Service::Request::SharedPtr req,
  const SetRoutePoints::Service::Response::SharedPtr res)
{
  // NOTE: The route services should be mutually exclusive by callback group.
  if (state_.state != RouteState::Message::UNSET) {
    throw component_interface_utils::ServiceException(
      SetRoute::Service::Response::ERROR_ROUTE_EXISTS, "The route is already set.");
  }
  if (!planner_->ready()) {
    throw component_interface_utils::ServiceException(
      SetRoutePoints::Service::Response::ERROR_PLANNER_UNREADY, "The planner is not ready.");
  }

  // Use common header for transforms.
  PoseStamped pose;
  pose.header = req->header;

  // Convert route points.
  PlannerPlugin::RoutePoints points;
  points.push_back(get_ego_vehicle_pose().pose);
  for (const auto & waypoint : req->waypoints) {
    pose.pose = waypoint;
    points.push_back(transform_pose(pose).pose);
  }
  pose.pose = req->goal;
  points.push_back(transform_pose(pose).pose);

  // Plan route.
  HADMapRoute route = planner_->plan(points);
  if (route.segments.empty()) {
    throw component_interface_utils::ServiceException(
      SetRoutePoints::Service::Response::ERROR_PLANNER_FAILED, "The planned route is empty.");
  }
  route.header.stamp = req->header.stamp;
  route.header.frame_id = map_frame_;

  // Update route.
  change_route(route);
  change_state(RouteState::Message::SET);
  res->status.success = true;
}

}  // namespace mission_planner

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(mission_planner::MissionPlanner)

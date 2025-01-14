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

#ifndef ROUTING_HPP_
#define ROUTING_HPP_

#include <autoware/adapi_specs/routing.hpp>
#include <autoware/component_interface_specs/planning.hpp>
#include <autoware/component_interface_specs/system.hpp>
#include <autoware/component_interface_utils/status.hpp>
#include <rclcpp/rclcpp.hpp>

// This file should be included after messages.
#include "utils/types.hpp"

namespace autoware::default_adapi
{

class RoutingNode : public rclcpp::Node
{
public:
  explicit RoutingNode(const rclcpp::NodeOptions & options);

private:
  using OperationModeState = autoware::component_interface_specs::system::OperationModeState;
  using State = autoware::component_interface_specs::planning::RouteState;
  using Route = autoware::component_interface_specs::planning::LaneletRoute;

  rclcpp::CallbackGroup::SharedPtr group_cli_;
  Pub<autoware::adapi_specs::routing::RouteState> pub_state_;
  Pub<autoware::adapi_specs::routing::Route> pub_route_;
  Srv<autoware::adapi_specs::routing::SetRoutePoints> srv_set_route_points_;
  Srv<autoware::adapi_specs::routing::SetRoute> srv_set_route_;
  Srv<autoware::adapi_specs::routing::ChangeRoutePoints> srv_change_route_points_;
  Srv<autoware::adapi_specs::routing::ChangeRoute> srv_change_route_;
  Srv<autoware::adapi_specs::routing::ClearRoute> srv_clear_route_;
  Sub<autoware::component_interface_specs::planning::RouteState> sub_state_;
  Sub<autoware::component_interface_specs::planning::LaneletRoute> sub_route_;
  Cli<autoware::component_interface_specs::planning::SetWaypointRoute> cli_set_waypoint_route_;
  Cli<autoware::component_interface_specs::planning::SetLaneletRoute> cli_set_lanelet_route_;
  Cli<autoware::component_interface_specs::planning::ClearRoute> cli_clear_route_;
  Cli<autoware::component_interface_specs::system::ChangeOperationMode> cli_operation_mode_;
  Sub<autoware::component_interface_specs::system::OperationModeState> sub_operation_mode_;
  bool is_autoware_control_;
  bool is_auto_mode_;
  State::Message state_;

  void change_stop_mode();
  void on_operation_mode(const OperationModeState::Message::ConstSharedPtr msg);
  void on_state(const State::Message::ConstSharedPtr msg);
  void on_route(const Route::Message::ConstSharedPtr msg);
  void on_clear_route(
    const autoware::adapi_specs::routing::ClearRoute::Service::Request::SharedPtr req,
    const autoware::adapi_specs::routing::ClearRoute::Service::Response::SharedPtr res);
  void on_set_route_points(
    const autoware::adapi_specs::routing::SetRoutePoints::Service::Request::SharedPtr req,
    const autoware::adapi_specs::routing::SetRoutePoints::Service::Response::SharedPtr res);
  void on_set_route(
    const autoware::adapi_specs::routing::SetRoute::Service::Request::SharedPtr req,
    const autoware::adapi_specs::routing::SetRoute::Service::Response::SharedPtr res);
  void on_change_route_points(
    const autoware::adapi_specs::routing::SetRoutePoints::Service::Request::SharedPtr req,
    const autoware::adapi_specs::routing::SetRoutePoints::Service::Response::SharedPtr res);
  void on_change_route(
    const autoware::adapi_specs::routing::SetRoute::Service::Request::SharedPtr req,
    const autoware::adapi_specs::routing::SetRoute::Service::Response::SharedPtr res);
};

}  // namespace autoware::default_adapi

#endif  // ROUTING_HPP_

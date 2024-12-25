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

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/behavior_path_start_planner_module/geometric_pull_out.hpp>
#include <autoware/behavior_path_start_planner_module/start_planner_module.hpp>
#include <autoware/behavior_path_start_planner_module/util.hpp>
#include <autoware/lane_departure_checker/lane_departure_checker.hpp>
#include <autoware/route_handler/route_handler.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_planning_test_manager/autoware_planning_test_manager_utils.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using autoware::behavior_path_planner::GeometricPullOut;
using autoware::behavior_path_planner::StartPlannerParameters;
using autoware::lane_departure_checker::LaneDepartureChecker;
using autoware::test_utils::get_absolute_path_to_config;
using autoware_planning_msgs::msg::LaneletRoute;
using RouteSections = std::vector<autoware_planning_msgs::msg::LaneletSegment>;
using autoware_planning_test_manager::utils::makeBehaviorRouteFromLaneId;

namespace autoware::behavior_path_planner
{

class TestGeometricPullOut : public ::testing::Test
{
public:
  std::optional<PullOutPath> plan(
    const Pose & start_pose, const Pose & goal_pose, PlannerDebugData & planner_debug_data)
  {
    return geometric_pull_out_->plan(start_pose, goal_pose, planner_debug_data);
  }

protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    node_ = rclcpp::Node::make_shared("geometric_pull_out", get_node_options());

    load_parameters();
    initialize_vehicle_info();
    initialize_lane_departure_checker();
    initialize_routeHandler();
    initialize_geometric_pull_out_planner();
    initialize_planner_data();
  }

  void TearDown() override { rclcpp::shutdown(); }
  // Member variables
  std::shared_ptr<rclcpp::Node> node_;
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler_;
  autoware::vehicle_info_utils::VehicleInfo vehicle_info_;
  std::shared_ptr<GeometricPullOut> geometric_pull_out_;
  std::shared_ptr<LaneDepartureChecker> lane_departure_checker_;
  PlannerData planner_data_;

private:
  rclcpp::NodeOptions get_node_options() const
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

  void load_parameters()
  {
    const auto dp_double = [&](const std::string & s) {
      return node_->declare_parameter<double>(s);
    };
    const auto dp_bool = [&](const std::string & s) { return node_->declare_parameter<bool>(s); };
    // Load parameters required for planning
    const std::string ns = "start_planner.";
    lane_departure_check_expansion_margin_ =
      dp_double(ns + "lane_departure_check_expansion_margin");
    pull_out_max_steer_angle_ = dp_double(ns + "pull_out_max_steer_angle");
    pull_out_arc_path_interval_ = dp_double(ns + "arc_path_interval");
    center_line_path_interval_ = dp_double(ns + "center_line_path_interval");
    th_moving_object_velocity_ = dp_double(ns + "th_moving_object_velocity");
    divide_pull_out_path_ = dp_bool(ns + "divide_pull_out_path");
    backward_path_length_ = dp_double("backward_path_length");
    forward_path_length_ = dp_double("forward_path_length");
  }

  void initialize_vehicle_info()
  {
    vehicle_info_ = autoware::vehicle_info_utils::VehicleInfoUtils(*node_).getVehicleInfo();
  }

  void initialize_lane_departure_checker()
  {
    lane_departure_checker_ = std::make_shared<LaneDepartureChecker>();
    lane_departure_checker_->setVehicleInfo(vehicle_info_);

    autoware::lane_departure_checker::Param lane_departure_checker_params{};
    lane_departure_checker_params.footprint_extra_margin = lane_departure_check_expansion_margin_;
    lane_departure_checker_->setParam(lane_departure_checker_params);
  }

  void initialize_routeHandler()
  {
    // Load a sample lanelet map and create a route handler
    const auto shoulder_map_path = autoware::test_utils::get_absolute_path_to_lanelet_map(
      "autoware_test_utils", "road_shoulder/lanelet2_map.osm");
    const auto map_bin_msg = autoware::test_utils::make_map_bin_msg(shoulder_map_path, 0.5);

    route_handler_ = std::make_shared<autoware::route_handler::RouteHandler>(map_bin_msg);
  }

  void initialize_geometric_pull_out_planner()
  {
    auto parameters = std::make_shared<StartPlannerParameters>();
    parameters->parallel_parking_parameters.pull_out_max_steer_angle = pull_out_max_steer_angle_;
    parameters->parallel_parking_parameters.pull_out_arc_path_interval =
      pull_out_arc_path_interval_;
    parameters->parallel_parking_parameters.center_line_path_interval = center_line_path_interval_;
    parameters->th_moving_object_velocity = th_moving_object_velocity_;
    parameters->divide_pull_out_path = divide_pull_out_path_;

    auto time_keeper = std::make_shared<autoware::universe_utils::TimeKeeper>();
    geometric_pull_out_ =
      std::make_shared<GeometricPullOut>(*node_, *parameters, lane_departure_checker_, time_keeper);
  }

  void initialize_planner_data()
  {
    planner_data_.parameters.backward_path_length = backward_path_length_;
    planner_data_.parameters.forward_path_length = forward_path_length_;
    planner_data_.parameters.wheel_base = vehicle_info_.wheel_base_m;
    planner_data_.parameters.wheel_tread = vehicle_info_.wheel_tread_m;
    planner_data_.parameters.front_overhang = vehicle_info_.front_overhang_m;
    planner_data_.parameters.left_over_hang = vehicle_info_.left_overhang_m;
    planner_data_.parameters.right_over_hang = vehicle_info_.right_overhang_m;
  }

  // Parameter variables
  double lane_departure_check_expansion_margin_{0.0};
  double pull_out_max_steer_angle_{0.0};
  double pull_out_arc_path_interval_{0.0};
  double center_line_path_interval_{0.0};
  double th_moving_object_velocity_{0.0};
  double backward_path_length_{0.0};
  double forward_path_length_{0.0};
  bool divide_pull_out_path_{false};
};

TEST_F(TestGeometricPullOut, GenerateValidGeometricPullOutPath)
{
  const auto start_pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(geometry_msgs::build<geometry_msgs::msg::Point>().x(362.181).y(362.164).z(100.000))
      .orientation(
        geometry_msgs::build<geometry_msgs::msg::Quaternion>().x(0.0).y(0.0).z(0.709650).w(
          0.704554));

  const auto goal_pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(geometry_msgs::build<geometry_msgs::msg::Point>().x(365.658).y(507.253).z(100.000))
      .orientation(
        geometry_msgs::build<geometry_msgs::msg::Quaternion>().x(0.0).y(0.0).z(0.705897).w(
          0.708314));

  // Set up current odometry at start pose
  auto odometry = std::make_shared<nav_msgs::msg::Odometry>();
  odometry->pose.pose = start_pose;
  odometry->header.frame_id = "map";
  planner_data_.self_odometry = odometry;

  // Setup route
  const auto route = makeBehaviorRouteFromLaneId(
    4619, 4635, "autoware_test_utils", "road_shoulder/lanelet2_map.osm");
  route_handler_->setRoute(route);

  // Update planner data with the route handler
  planner_data_.route_handler = route_handler_;
  geometric_pull_out_->setPlannerData(std::make_shared<PlannerData>(planner_data_));

  // Plan the pull out path
  PlannerDebugData debug_data;
  auto result = plan(start_pose, goal_pose, debug_data);

  // Assert that a valid geometric geometric pull out path is generated
  ASSERT_TRUE(result.has_value()) << "Geometric pull out path generation failed.";
  EXPECT_EQ(result->partial_paths.size(), 2UL)
    << "Generated geometric pull out path does not have the expected number of partial paths.";
  EXPECT_EQ(debug_data.conditions_evaluation.back(), "success")
    << "Geometric pull out path planning did not succeed.";
}

}  // namespace autoware::behavior_path_planner

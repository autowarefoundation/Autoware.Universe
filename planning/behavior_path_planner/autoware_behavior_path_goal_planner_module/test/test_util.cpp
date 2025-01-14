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
#include <autoware/behavior_path_goal_planner_module/util.hpp>
#include <autoware/route_handler/route_handler.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

class TestUtilWithMap : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    // parameters
    auto node_options = rclcpp::NodeOptions{};
    node_options.arguments(std::vector<std::string>{
      "--ros-args", "--params-file",
      ament_index_cpp::get_package_share_directory("autoware_test_utils") +
        "/config/test_vehicle_info.param.yaml"});
    auto node = rclcpp::Node::make_shared("test", node_options);
    vehicle_info = autoware::vehicle_info_utils::VehicleInfoUtils(*node).getVehicleInfo();

    // lanelet map
    const std::string shoulder_map_path = autoware::test_utils::get_absolute_path_to_lanelet_map(
      "autoware_test_utils", "road_shoulder/lanelet2_map.osm");
    const auto map_bin_msg = autoware::test_utils::make_map_bin_msg(shoulder_map_path, 0.5);

    // load map
    route_handler = std::make_shared<autoware::route_handler::RouteHandler>(map_bin_msg);
  }

  void TearDown() override { rclcpp::shutdown(); }

public:
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler;
  autoware::vehicle_info_utils::VehicleInfo vehicle_info;
};

class DISABLED_TestUtilWithMap : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    // parameters
    auto node_options = rclcpp::NodeOptions{};
    node_options.arguments(std::vector<std::string>{
      "--ros-args", "--params-file",
      ament_index_cpp::get_package_share_directory("autoware_test_utils") +
        "/config/test_vehicle_info.param.yaml"});
    auto node = rclcpp::Node::make_shared("test", node_options);
    vehicle_info = autoware::vehicle_info_utils::VehicleInfoUtils(*node).getVehicleInfo();

    // lanelet map
    const std::string shoulder_map_path = autoware::test_utils::get_absolute_path_to_lanelet_map(
      "autoware_test_utils", "road_shoulder/lanelet2_map.osm");
    const auto map_bin_msg = autoware::test_utils::make_map_bin_msg(shoulder_map_path, 0.5);

    // load map
    route_handler = std::make_shared<autoware::route_handler::RouteHandler>(map_bin_msg);
  }

  void TearDown() override { rclcpp::shutdown(); }

public:
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler;
  autoware::vehicle_info_utils::VehicleInfo vehicle_info;
};

TEST_F(TestUtilWithMap, getBusStopAreaPolygons)
{
  const auto lanes = lanelet::utils::query::laneletLayer(route_handler->getLaneletMapPtr());
  const auto shoulder_lanes = lanelet::utils::query::shoulderLanelets(lanes);
  const auto bus_stop_area_polygons =
    autoware::behavior_path_planner::goal_planner_utils::getBusStopAreaPolygons(shoulder_lanes);
  EXPECT_EQ(bus_stop_area_polygons.size(), 2);
}

TEST_F(DISABLED_TestUtilWithMap, isWithinAreas)
{
  const auto lanes = lanelet::utils::query::laneletLayer(route_handler->getLaneletMapPtr());
  const auto shoulder_lanes = lanelet::utils::query::shoulderLanelets(lanes);
  const auto bus_stop_area_polygons =
    autoware::behavior_path_planner::goal_planner_utils::getBusStopAreaPolygons(shoulder_lanes);

  const auto footprint = vehicle_info.createFootprint();
  const geometry_msgs::msg::Pose baselink_pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(
        geometry_msgs::build<geometry_msgs::msg::Point>().x(273.102814).y(402.194794).z(0.0))
      .orientation(
        geometry_msgs::build<geometry_msgs::msg::Quaternion>().x(0.0).y(0.0).z(0.707390).w(
          0.706824));
  const auto baselink_footprint = autoware::universe_utils::transformVector(
    footprint, autoware::universe_utils::pose2transform(baselink_pose));
  EXPECT_EQ(
    autoware::behavior_path_planner::goal_planner_utils::isWithinAreas(
      baselink_footprint, bus_stop_area_polygons),
    true);
}

TEST_F(TestUtilWithMap, combineLanePoints)
{
  // 1) combine points with no duplicate IDs
  {
    lanelet::Points3d points{
      {lanelet::Point3d(1, 0, 0), lanelet::Point3d(2, 0, 0), lanelet::Point3d(3, 0, 0)}};
    lanelet::Points3d points_next{
      {lanelet::Point3d(4, 0, 0), lanelet::Point3d(5, 0, 0), lanelet::Point3d(6, 0, 0)}};

    const auto combined_points =
      autoware::behavior_path_planner::goal_planner_utils::combineLanePoints(points, points_next);
    EXPECT_EQ(combined_points.size(), 6);
  }

  // 2) combine points with duplicate IDs
  {
    lanelet::Points3d points{
      {lanelet::Point3d(1, 0, 0), lanelet::Point3d(2, 0, 0), lanelet::Point3d(3, 0, 0)}};
    lanelet::Points3d points_next{
      {lanelet::Point3d(3, 0, 0), lanelet::Point3d(4, 0, 0), lanelet::Point3d(5, 0, 0)}};

    const auto combined_points =
      autoware::behavior_path_planner::goal_planner_utils::combineLanePoints(points, points_next);
    EXPECT_EQ(combined_points.size(), 5);
  }
}

TEST_F(DISABLED_TestUtilWithMap, createDepartureCheckLanelet)
{
  const auto lanelet_map_ptr = route_handler->getLaneletMapPtr();

  const geometry_msgs::msg::Pose goal_pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(geometry_msgs::build<geometry_msgs::msg::Point>()
                  .x(433.42254638671875)
                  .y(465.3381652832031)
                  .z(0.0))
      .orientation(geometry_msgs::build<geometry_msgs::msg::Quaternion>()
                     .x(0.0)
                     .y(0.0)
                     .z(0.306785474523741)
                     .w(0.9517786888879384));

  // 1) get target shoulder lane and check it's lane id
  const auto target_shoulder_lane = route_handler->getPullOverTarget(goal_pose);
  EXPECT_EQ(target_shoulder_lane.has_value(), true);
  EXPECT_EQ(target_shoulder_lane.value().id(), 18391);

  // 2) get shoulder lane sequence
  const auto target_shoulder_lanes =
    route_handler->getShoulderLaneletSequence(target_shoulder_lane.value(), goal_pose);
  EXPECT_EQ(target_shoulder_lanes.size(), 3);

  // 3) check if the right bound of the departure check lane extended to the right end matches the
  // right bound of the rightmost lanelet
  const auto to_points3d = [](const lanelet::ConstLineString3d & bound) {
    lanelet::Points3d points;
    for (const auto & pt : bound) {
      points.push_back(lanelet::Point3d(pt));
    }
    return points;
  };

  const auto departure_check_lane =
    autoware::behavior_path_planner::goal_planner_utils::createDepartureCheckLanelet(
      target_shoulder_lanes, *route_handler, true);
  const auto departure_check_lane_right_bound_points =
    to_points3d(departure_check_lane.rightBound());

  const std::vector<lanelet::Id> most_right_lanelet_ids = {18381, 18383, 18388};
  lanelet::Points3d right_bound_points;
  for (const auto & id : most_right_lanelet_ids) {
    const auto lanelet = lanelet_map_ptr->laneletLayer.get(id);
    right_bound_points = autoware::behavior_path_planner::goal_planner_utils::combineLanePoints(
      right_bound_points, to_points3d(lanelet.rightBound()));
  }

  EXPECT_EQ(departure_check_lane_right_bound_points.size(), right_bound_points.size());
  for (size_t i = 0; i < departure_check_lane_right_bound_points.size(); ++i) {
    EXPECT_EQ(departure_check_lane_right_bound_points.at(i).id(), right_bound_points.at(i).id());
  }
}

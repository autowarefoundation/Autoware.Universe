// Copyright 2024 TIER IV
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

#include <../src/lanelet2_plugins/default_planner.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <boost/geometry/io/wkt/write.hpp>

#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/Polygon.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <tf2/utils.h>

#include <memory>
#include <string>
#include <vector>

using autoware::universe_utils::calcOffsetPose;
using autoware::universe_utils::createQuaternionFromRPY;
using autoware_planning_msgs::msg::LaneletRoute;
using geometry_msgs::msg::Pose;
using RoutePoints = std::vector<geometry_msgs::msg::Pose>;

// inherit DefaultPlanner to access protected methods and make wrapper to private methods
struct DefaultPlanner : public autoware::mission_planner::lanelet2::DefaultPlanner
{
  // todo(someone): create tests with various kinds of maps
  void set_default_test_map() { route_handler_.setMap(autoware::test_utils::makeMapBinMsg()); }
  [[nodiscard]] bool check_goal_inside_lanes(
    const lanelet::ConstLanelet & closest_lanelet_to_goal,
    const lanelet::ConstLanelets & path_lanelets,
    const autoware::universe_utils::Polygon2d & goal_footprint) const
  {
    return check_goal_footprint_inside_lanes(
      closest_lanelet_to_goal, path_lanelets, goal_footprint);
  }
  bool is_goal_valid_wrapper(
    const geometry_msgs::msg::Pose & goal, const lanelet::ConstLanelets & path_lanelets)
  {
    return is_goal_valid(goal, path_lanelets);
  }

  lanelet::ConstLanelets get_lanelets_from_ids(const std::vector<lanelet::Id> & ids)
  {
    const auto lanelet_map_ptr = route_handler_.getLaneletMapPtr();

    lanelet::ConstLanelets lanelets;

    for (const auto & id : ids) {
      lanelets.push_back(lanelet_map_ptr->laneletLayer.get(id));
    }
    return lanelets;
  }
};

class DefaultPlannerTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);

    rclcpp::NodeOptions options;

    const auto autoware_test_utils_dir =
      ament_index_cpp::get_package_share_directory("autoware_test_utils");
    const auto mission_planner_dir =
      ament_index_cpp::get_package_share_directory("autoware_mission_planner");
    options.arguments(
      {"--ros-args", "--params-file",
       autoware_test_utils_dir + "/config/test_vehicle_info.param.yaml", "--params-file",
       mission_planner_dir + "/config/mission_planner.param.yaml"});
    // NOTE: vehicle width and length set by test_vehicle_info.param.yaml are as follows
    // vehicle_width: 1.83, vehicle_length: 4.77

    node_ = std::make_shared<rclcpp::Node>("test_node", options);
    planner_.initialize(node_.get());
  }

  ~DefaultPlannerTest() override { rclcpp::shutdown(); }

  std::shared_ptr<rclcpp::Node> node_;

  DefaultPlanner planner_;
};

TEST_F(DefaultPlannerTest, checkGoalInsideLane)
{
  // Test with dummy map such that only the lanelets provided as inputs are used for the ego lane
  planner_.set_default_test_map();
  // vehicle max longitudinal offset is used to retrieve additional lanelets from the map
  lanelet::LineString3d left_bound;
  lanelet::LineString3d right_bound;
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, -1, -1});
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 0, -1});
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, -1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, -1, 1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 0, 1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, 1});
  lanelet::ConstLanelet goal_lanelet{lanelet::InvalId, left_bound, right_bound};

  // simple case where the footprint is completely inside the lane
  autoware::universe_utils::Polygon2d goal_footprint;
  goal_footprint.outer().emplace_back(0, 0);
  goal_footprint.outer().emplace_back(0, 0.5);
  goal_footprint.outer().emplace_back(0.5, 0.5);
  goal_footprint.outer().emplace_back(0.5, 0);
  goal_footprint.outer().emplace_back(0, 0);
  EXPECT_TRUE(planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet}, goal_footprint));

  // the footprint touches the border of the lanelet
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(0, 0);
  goal_footprint.outer().emplace_back(0, 1);
  goal_footprint.outer().emplace_back(1, 1);
  goal_footprint.outer().emplace_back(1, 0);
  goal_footprint.outer().emplace_back(0, 0);
  EXPECT_TRUE(planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet}, goal_footprint));

  // add lanelets such that the footprint touches the linestring shared by the combined lanelets
  lanelet::LineString3d next_left_bound;
  lanelet::LineString3d next_right_bound;
  next_left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, -1});
  next_left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 2, -1});
  next_right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, 1});
  next_right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 2, 1});
  lanelet::ConstLanelet next_lanelet{lanelet::InvalId, next_left_bound, next_right_bound};
  EXPECT_TRUE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));

  // the footprint is inside the other lanelet
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(1.1, -0.5);
  goal_footprint.outer().emplace_back(1.1, 0.5);
  goal_footprint.outer().emplace_back(1.6, 0.5);
  goal_footprint.outer().emplace_back(1.6, -0.5);
  goal_footprint.outer().emplace_back(1.1, -0.5);
  EXPECT_TRUE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));

  // the footprint is completely outside of the lanelets
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(1.1, 1.5);
  goal_footprint.outer().emplace_back(1.1, 2.0);
  goal_footprint.outer().emplace_back(1.6, 2.0);
  goal_footprint.outer().emplace_back(1.6, 1.5);
  goal_footprint.outer().emplace_back(1.1, 1.5);
  EXPECT_FALSE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));

  // the footprint is outside of the lanelets but touches an edge
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(1.1, 1.0);
  goal_footprint.outer().emplace_back(1.1, 2.0);
  goal_footprint.outer().emplace_back(1.6, 2.0);
  goal_footprint.outer().emplace_back(1.6, 1.0);
  goal_footprint.outer().emplace_back(1.1, 1.0);
  EXPECT_FALSE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));

  // the footprint is outside of the lanelets but share a point
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(2.0, 1.0);
  goal_footprint.outer().emplace_back(2.0, 2.0);
  goal_footprint.outer().emplace_back(3.0, 2.0);
  goal_footprint.outer().emplace_back(3.0, 1.0);
  goal_footprint.outer().emplace_back(2.0, 1.0);
  EXPECT_FALSE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));

  // ego footprint that overlaps both lanelets
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(-0.5, -0.5);
  goal_footprint.outer().emplace_back(-0.5, 0.5);
  goal_footprint.outer().emplace_back(1.5, 0.5);
  goal_footprint.outer().emplace_back(1.5, -0.5);
  goal_footprint.outer().emplace_back(-0.5, -0.5);
  EXPECT_TRUE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));

  // ego footprint that goes further than the next lanelet
  goal_footprint.clear();
  goal_footprint.outer().emplace_back(-0.5, -0.5);
  goal_footprint.outer().emplace_back(-0.5, 0.5);
  goal_footprint.outer().emplace_back(2.5, 0.5);
  goal_footprint.outer().emplace_back(2.5, -0.5);
  goal_footprint.outer().emplace_back(-0.5, -0.5);
  EXPECT_FALSE(
    planner_.check_goal_inside_lanes(goal_lanelet, {goal_lanelet, next_lanelet}, goal_footprint));
}

TEST_F(DefaultPlannerTest, isValidGoal)
{
  planner_.set_default_test_map();

  std::vector<lanelet::Id> path_lanelet_ids = {9102, 9540, 9546, 9178, 52, 124};
  const auto path_lanelets = planner_.get_lanelets_from_ids(path_lanelet_ids);

  const double yaw_threshold = 0.872665;  // 50 degrees

  /**
   * 1) goal pose within the lanelet
   */
  // goal pose within the lanelet
  Pose goal_pose;
  goal_pose.position.x = 3810.24951171875;
  goal_pose.position.y = 73769.2578125;
  goal_pose.position.z = 0.0;
  goal_pose.orientation.x = 0.0;
  goal_pose.orientation.y = 0.0;
  goal_pose.orientation.z = 0.23908402523702438;
  goal_pose.orientation.w = 0.9709988820160721;
  const double yaw = tf2::getYaw(goal_pose.orientation);
  EXPECT_TRUE(planner_.is_goal_valid_wrapper(goal_pose, path_lanelets));

  // move 1m to the right to make the goal outside of the lane
  Pose right_offset_goal_pose = calcOffsetPose(goal_pose, 0.0, 1.0, 0.0);
  EXPECT_FALSE(planner_.is_goal_valid_wrapper(right_offset_goal_pose, path_lanelets));

  // move 1m to the left to make the goal outside of the lane
  Pose left_offset_goal_pose = calcOffsetPose(goal_pose, 0.0, -1.0, 0.0);
  EXPECT_FALSE(planner_.is_goal_valid_wrapper(left_offset_goal_pose, path_lanelets));

  // rotate to the right
  Pose right_rotated_goal_pose = goal_pose;
  right_rotated_goal_pose.orientation = createQuaternionFromRPY(0.0, 0.0, yaw + yaw_threshold);
  EXPECT_FALSE(planner_.is_goal_valid_wrapper(right_rotated_goal_pose, path_lanelets));

  // rotate to the left
  Pose left_rotated_goal_pose = goal_pose;
  left_rotated_goal_pose.orientation = createQuaternionFromRPY(0.0, 0.0, yaw - yaw_threshold);
  EXPECT_FALSE(planner_.is_goal_valid_wrapper(left_rotated_goal_pose, path_lanelets));

  /**
   * 2) goal pose on the road shoulder
   */
  std::vector<lanelet::Id> path_lanelet_to_road_shoulder_ids = {9102, 9540, 9546};
  lanelet::Id target_road_shoulder_id = 10304;  // left road shoulder of 9546
  const auto path_lanelets_to_road_shoulder =
    planner_.get_lanelets_from_ids(path_lanelet_to_road_shoulder_ids);
  const auto target_road_shoulder =
    planner_.get_lanelets_from_ids({target_road_shoulder_id}).front();

  Pose goal_pose_on_road_shoulder;
  goal_pose_on_road_shoulder.position.x = 3742.3825513144147;
  goal_pose_on_road_shoulder.position.y = 73737.75783925217;
  goal_pose_on_road_shoulder.position.z = 0.0;
  goal_pose_on_road_shoulder.orientation.x = 0.0;
  goal_pose_on_road_shoulder.orientation.y = 0.0;
  goal_pose_on_road_shoulder.orientation.z = 0.23721495449671745;
  goal_pose_on_road_shoulder.orientation.w = 0.9714571865826721;
  EXPECT_TRUE(
    planner_.is_goal_valid_wrapper(goal_pose_on_road_shoulder, path_lanelets_to_road_shoulder));

  // put goal on the left bound of the road shoulder
  // if the goal is on the road shoulder, the footprint can be outside of the lane, and only the
  // angle is checked
  const auto left_bound = target_road_shoulder.leftBound();
  Pose goal_pose_on_road_shoulder_left_bound = goal_pose_on_road_shoulder;
  goal_pose_on_road_shoulder_left_bound.position.x = left_bound.front().x();
  goal_pose_on_road_shoulder_left_bound.position.y = left_bound.front().y();
  EXPECT_TRUE(planner_.is_goal_valid_wrapper(
    goal_pose_on_road_shoulder_left_bound, path_lanelets_to_road_shoulder));

  // move goal pose outside of the road shoulder
  Pose goal_pose_outside_road_shoulder =
    calcOffsetPose(goal_pose_on_road_shoulder_left_bound, 0.0, 0.1, 0.0);
  EXPECT_FALSE(planner_.is_goal_valid_wrapper(
    goal_pose_outside_road_shoulder, path_lanelets_to_road_shoulder));

  // rotate goal to the right
  Pose right_rotated_goal_pose_on_road_shoulder = goal_pose_on_road_shoulder;
  right_rotated_goal_pose_on_road_shoulder.orientation =
    createQuaternionFromRPY(0.0, 0.0, yaw + yaw_threshold);
  EXPECT_FALSE(planner_.is_goal_valid_wrapper(
    right_rotated_goal_pose_on_road_shoulder, path_lanelets_to_road_shoulder));

  /**
   * todo(someone): add more test for parking area
   */
}

TEST_F(DefaultPlannerTest, plan)
{
  planner_.set_default_test_map();

  /**
   * 1) goal pose within the lanelet
   */
  RoutePoints route_points;
  Pose start_pose;
  start_pose.position.x = 3717.239501953125;
  start_pose.position.y = 73720.84375;
  start_pose.position.z = 0.0;
  start_pose.orientation.x = 0.00012620055018808463;
  start_pose.orientation.y = -0.0005077247816171834;
  start_pose.orientation.z = 0.2412209576008544;

  Pose goal_pose;
  goal_pose.position.x = 3810.24951171875;
  goal_pose.position.y = 73769.2578125;
  goal_pose.position.z = 0.0;
  goal_pose.orientation.x = 0.0;
  goal_pose.orientation.y = 0.0;
  goal_pose.orientation.z = 0.23908402523702438;
  goal_pose.orientation.w = 0.9709988820160721;

  route_points.push_back(start_pose);
  route_points.push_back(goal_pose);
  const auto route = planner_.plan(route_points);

  EXPECT_EQ(route.start_pose.position.x, start_pose.position.x);
  EXPECT_EQ(route.start_pose.position.y, start_pose.position.y);
  EXPECT_EQ(route.goal_pose.position.x, goal_pose.position.x);
  EXPECT_EQ(route.goal_pose.position.y, goal_pose.position.y);
  std::vector<lanelet::Id> path_lanelet_ids = {9102, 9540, 9546, 9178, 52, 124};
  EXPECT_EQ(route.segments.size(), path_lanelet_ids.size());
  for (size_t i = 0; i < route.segments.size(); i++) {
    EXPECT_EQ(route.segments[i].preferred_primitive.id, path_lanelet_ids[i]);
    const auto & primitives = route.segments[i].primitives;
    EXPECT_TRUE(std::find_if(primitives.begin(), primitives.end(), [&](const auto & primitive) {
                  return primitive.id == path_lanelet_ids[i];
                }) != primitives.end());
  }

  /**
   * 2) goal pose on the road shoulder
   */
  Pose goal_pose_on_road_shoulder;
  goal_pose_on_road_shoulder.position.x = 3742.3825513144147;
  goal_pose_on_road_shoulder.position.y = 73737.75783925217;
  goal_pose_on_road_shoulder.position.z = 0.0;
  goal_pose_on_road_shoulder.orientation.x = 0.0;
  goal_pose_on_road_shoulder.orientation.y = 0.0;
  goal_pose_on_road_shoulder.orientation.z = 0.23721495449671745;
  goal_pose_on_road_shoulder.orientation.w = 0.9714571865826721;

  RoutePoints route_points_to_road_shoulder;
  route_points_to_road_shoulder.push_back(start_pose);
  route_points_to_road_shoulder.push_back(goal_pose_on_road_shoulder);
  const auto route_to_road_shoulder = planner_.plan(route_points_to_road_shoulder);

  EXPECT_EQ(route_to_road_shoulder.start_pose.position.x, start_pose.position.x);
  EXPECT_EQ(route_to_road_shoulder.start_pose.position.y, start_pose.position.y);
  EXPECT_EQ(route_to_road_shoulder.goal_pose.position.x, goal_pose_on_road_shoulder.position.x);
  EXPECT_EQ(route_to_road_shoulder.goal_pose.position.y, goal_pose_on_road_shoulder.position.y);
  std::vector<lanelet::Id> path_lanelet_to_road_shoulder_ids = {9102, 9540, 9546};
  EXPECT_EQ(route_to_road_shoulder.segments.size(), path_lanelet_to_road_shoulder_ids.size());
  for (size_t i = 0; i < route_to_road_shoulder.segments.size(); i++) {
    EXPECT_EQ(
      route_to_road_shoulder.segments[i].preferred_primitive.id,
      path_lanelet_to_road_shoulder_ids[i]);
    const auto & primitives = route_to_road_shoulder.segments[i].primitives;
    EXPECT_TRUE(std::find_if(primitives.begin(), primitives.end(), [&](const auto & primitive) {
                  return primitive.id == path_lanelet_to_road_shoulder_ids[i];
                }) != primitives.end());
  }
}

//  `visualize` function is used for user too, so it is more important than debug functions
TEST_F(DefaultPlannerTest, visualize)
{
  DefaultPlanner planner;
  planner_.set_default_test_map();
  const LaneletRoute route = autoware::test_utils::makeBehaviorNormalRoute();
  const auto marker_array = planner_.visualize(route);
  // clang-format off
  const std::vector<std::string> expected_ns = {
    // 1) lanelet::visualization::laneletsBoundaryAsMarkerArray
    //    center line is not set in the test lanelets,
    //    so "center_lane_line" and "center_line_arrows" are not visualized
    "left_lane_bound", "right_lane_bound", "lane_start_bound",
    // 2) lanelet::visualization::laneletsAsMarkerArray
    "route_lanelets",
    // 3) lanelet::visualization::laneletsAsMarkerArray
    "goal_lanelets"};
  // clang-format on
  for (const auto & marker : marker_array.markers) {
    EXPECT_TRUE(std::find(expected_ns.begin(), expected_ns.end(), marker.ns) != expected_ns.end());
  }
}

TEST_F(DefaultPlannerTest, visualizeDebugFootprint)
{
  DefaultPlanner planner;
  planner_.set_default_test_map();

  autoware::universe_utils::LinearRing2d footprint;
  footprint.push_back({1.0, 1.0});
  footprint.push_back({1.0, -1.0});
  footprint.push_back({0.0, -1.0});
  footprint.push_back({-1.0, -1.0});
  footprint.push_back({-1.0, 1.0});
  footprint.push_back({0.0, 1.0});
  footprint.push_back({1.0, 1.0});

  const auto marker_array = planner_.visualize_debug_footprint(footprint);
  EXPECT_EQ(marker_array.markers.size(), 1);
  EXPECT_EQ(marker_array.markers[0].points.size(), 7);
}

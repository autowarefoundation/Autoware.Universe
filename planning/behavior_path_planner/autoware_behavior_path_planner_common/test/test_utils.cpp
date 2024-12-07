// Copyright 2024 TIER IV, Inc. All rights reserved.
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

#include "autoware/behavior_path_planner_common/utils/utils.hpp"

#include <autoware_test_utils/autoware_test_utils.hpp>
#include <autoware_test_utils/mock_data_parser.hpp>

#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>

#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>

using autoware::universe_utils::Point2d;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_planning_msgs::msg::Trajectory;
using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;
using ObjectClassification = autoware_perception_msgs::msg::ObjectClassification;
using autoware::behavior_path_planner::PlannerData;
using autoware_planning_msgs::msg::LaneletRoute;

using autoware::test_utils::createPose;
using autoware::test_utils::generateTrajectory;

class BehaviorPathPlanningUtilTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    planner_data_ = std::make_shared<PlannerData>();
    const auto test_data_file =
      ament_index_cpp::get_package_share_directory("autoware_behavior_path_planner_common") +
      "/test_data/test_traffic_light.yaml";
    YAML::Node config = YAML::LoadFile(test_data_file);

    set_current_pose(config);
    set_route_handler(config);
  }

  void set_current_pose(YAML::Node config)
  {
    const auto self_odometry =
      autoware::test_utils::parse<nav_msgs::msg::Odometry>(config["self_odometry"]);
    planner_data_->self_odometry = std::make_shared<const nav_msgs::msg::Odometry>(self_odometry);
  }

  void set_route_handler(YAML::Node config)
  {
    const auto route = autoware::test_utils::parse<LaneletRoute>(config["route"]);
    const auto intersection_map =
      autoware::test_utils::make_map_bin_msg(autoware::test_utils::get_absolute_path_to_lanelet_map(
        "autoware_test_utils", "intersection/lanelet2_map.osm"));
    planner_data_->route_handler->setMap(intersection_map);
    planner_data_->route_handler->setRoute(route);

    for (const auto & segment : route.segments) {
      current_lanelets.push_back(
        planner_data_->route_handler->getLaneletsFromId(segment.preferred_primitive.id));
    }
  }

  std::shared_ptr<PlannerData> planner_data_;
  lanelet::ConstLanelets current_lanelets;
  const double epsilon = 1e-06;
};

TEST_F(BehaviorPathPlanningUtilTest, l2Norm)
{
  using autoware::behavior_path_planner::utils::l2Norm;

  geometry_msgs::msg::Vector3 vector = autoware::universe_utils::createVector3(0.0, 0.0, 0.0);
  auto norm = l2Norm(vector);
  EXPECT_DOUBLE_EQ(norm, 0.0);

  vector = autoware::universe_utils::createVector3(1.0, 2.0, 2.0);
  norm = l2Norm(vector);
  EXPECT_DOUBLE_EQ(norm, 3.0);
}

TEST_F(BehaviorPathPlanningUtilTest, checkCollisionBetweenPathFootprintsAndObjects)
{
  using autoware::behavior_path_planner::utils::checkCollisionBetweenPathFootprintsAndObjects;

  autoware::universe_utils::LinearRing2d base_footprint = {
    Point2d{1.0, 1.0}, Point2d{1.0, -1.0}, Point2d{-1.0, -1.0}, Point2d{-1.0, 1.0},
    Point2d{1.0, -1.0}};
  double margin = 0.2;
  PredictedObjects objs;
  PredictedObject obj;
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 2.0;
  obj.shape.dimensions.y = 2.0;
  obj.kinematics.initial_pose_with_covariance.pose = createPose(9.0, 1.0, 0.0, 0.0, 0.0, 0.0);
  objs.objects.push_back(obj);

  PathWithLaneId ego_path;

  // Condition: no path
  EXPECT_FALSE(
    checkCollisionBetweenPathFootprintsAndObjects(base_footprint, ego_path, objs, margin));

  // Condition: object in front of path
  ego_path = generateTrajectory<PathWithLaneId>(5, 1.0);
  EXPECT_FALSE(
    checkCollisionBetweenPathFootprintsAndObjects(base_footprint, ego_path, objs, margin));

  // Condition: object overlapping path
  ego_path = generateTrajectory<PathWithLaneId>(10, 1.0);
  EXPECT_TRUE(
    checkCollisionBetweenPathFootprintsAndObjects(base_footprint, ego_path, objs, margin));
}

TEST_F(BehaviorPathPlanningUtilTest, checkCollisionBetweenFootprintAndObjects)
{
  using autoware::behavior_path_planner::utils::checkCollisionBetweenFootprintAndObjects;

  auto ego_pose = createPose(1.0, 1.0, 0.0, 0.0, 0.0, 0.0);
  autoware::universe_utils::LinearRing2d base_footprint = {
    Point2d{1.0, 1.0}, Point2d{1.0, -1.0}, Point2d{-1.0, -1.0}, Point2d{-1.0, 1.0},
    Point2d{1.0, -1.0}};
  double margin = 0.2;
  PredictedObjects objs;

  // Condition: no object
  EXPECT_FALSE(checkCollisionBetweenFootprintAndObjects(base_footprint, ego_pose, objs, margin));

  // Condition: no collision
  PredictedObject obj;
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 2.0;
  obj.shape.dimensions.y = 2.0;
  obj.kinematics.initial_pose_with_covariance.pose = createPose(9.0, 9.0, 0.0, 0.0, 0.0, 0.0);
  objs.objects.push_back(obj);
  EXPECT_FALSE(checkCollisionBetweenFootprintAndObjects(base_footprint, ego_pose, objs, margin));

  // Condition: collision
  obj.kinematics.initial_pose_with_covariance.pose.position.x = 1.0;
  obj.kinematics.initial_pose_with_covariance.pose.position.y = 1.0;
  objs.objects.push_back(obj);
  EXPECT_TRUE(checkCollisionBetweenFootprintAndObjects(base_footprint, ego_pose, objs, margin));
}

TEST_F(BehaviorPathPlanningUtilTest, calcLateralDistanceFromEgoToObject)
{
  using autoware::behavior_path_planner::utils::calcLateralDistanceFromEgoToObject;

  auto ego_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  double vehicle_width = 2.0;

  PredictedObject obj;
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 2.0;
  obj.shape.dimensions.y = 2.0;

  // Condition: overlapping
  obj.kinematics.initial_pose_with_covariance.pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(calcLateralDistanceFromEgoToObject(ego_pose, vehicle_width, obj), 0.0);

  // Condition: object on left
  obj.kinematics.initial_pose_with_covariance.pose.position.y = 5.0;
  EXPECT_DOUBLE_EQ(calcLateralDistanceFromEgoToObject(ego_pose, vehicle_width, obj), 3.0);

  // Condition: object on right
  obj.kinematics.initial_pose_with_covariance.pose.position.y = -5.0;
  EXPECT_DOUBLE_EQ(calcLateralDistanceFromEgoToObject(ego_pose, vehicle_width, obj), 3.0);
}

TEST_F(BehaviorPathPlanningUtilTest, calc_longitudinal_distance_from_ego_to_object)
{
  using autoware::behavior_path_planner::utils::calc_longitudinal_distance_from_ego_to_object;

  auto ego_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  PredictedObject obj;
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 2.0;
  obj.shape.dimensions.y = 1.0;

  // Condition: overlapping
  double base_link2front = 0.0;
  double base_link2rear = 0.0;
  obj.kinematics.initial_pose_with_covariance.pose = createPose(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(
    calc_longitudinal_distance_from_ego_to_object(ego_pose, base_link2front, base_link2rear, obj),
    0.0);

  // Condition: object in front
  base_link2front = 1.0;
  base_link2rear = -1.0;
  obj.kinematics.initial_pose_with_covariance.pose.position.x = 4.0;
  obj.kinematics.initial_pose_with_covariance.pose.position.y = 2.0;
  EXPECT_DOUBLE_EQ(
    calc_longitudinal_distance_from_ego_to_object(ego_pose, base_link2front, base_link2rear, obj),
    2.0);

  // Condition: object in rear
  obj.kinematics.initial_pose_with_covariance.pose.position.x = -4.0;
  EXPECT_DOUBLE_EQ(
    calc_longitudinal_distance_from_ego_to_object(ego_pose, base_link2front, base_link2rear, obj),
    2.0);
}

TEST_F(BehaviorPathPlanningUtilTest, calcLongitudinalDistanceFromEgoToObjects)
{
  using autoware::behavior_path_planner::utils::calcLongitudinalDistanceFromEgoToObjects;

  auto ego_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  double base_link2front = 1.0;
  double base_link2rear = -1.0;

  PredictedObjects objs;

  // Condition: none object
  EXPECT_DOUBLE_EQ(
    calcLongitudinalDistanceFromEgoToObjects(ego_pose, base_link2front, base_link2rear, objs),
    std::numeric_limits<double>::max());

  // Condition: both object in front
  PredictedObject near_obj;
  near_obj.kinematics.initial_pose_with_covariance.pose = createPose(5.0, 2.0, 0.0, 0.0, 0.0, 0.0);
  near_obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  near_obj.shape.dimensions.x = 2.0;
  near_obj.shape.dimensions.y = 1.0;

  PredictedObject far_obj;
  far_obj.kinematics.initial_pose_with_covariance.pose = createPose(25.0, 2.0, 0.0, 0.0, 0.0, 0.0);
  far_obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  far_obj.shape.dimensions.x = 5.0;
  far_obj.shape.dimensions.y = 1.0;

  objs.objects.push_back(near_obj);
  objs.objects.push_back(far_obj);
  EXPECT_DOUBLE_EQ(
    calcLongitudinalDistanceFromEgoToObjects(ego_pose, base_link2front, base_link2rear, objs), 3.0);
}

TEST_F(BehaviorPathPlanningUtilTest, refineGoal)
{
  using autoware::behavior_path_planner::utils::refineGoal;

  {
    const auto goal_lanelet = current_lanelets.front();
    const auto goal_pose = planner_data_->self_odometry->pose.pose;
    const auto refined_pose = refineGoal(goal_pose, goal_lanelet);
    EXPECT_DOUBLE_EQ(refined_pose.position.x, goal_pose.position.x);
    EXPECT_DOUBLE_EQ(refined_pose.position.y, goal_pose.position.y);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, refinePathForGoal)
{
  using autoware::behavior_path_planner::utils::refinePathForGoal;

  auto path = generateTrajectory<PathWithLaneId>(10, 1.0, 3.0);
  const double search_rad_range = M_PI;
  const auto goal_pose = createPose(5.2, 0.0, 0.0, 0.0, 0.0, 0.0);
  const int64_t goal_lane_id = 5;
  {
    const double search_radius_range = 1.0;
    const auto refined_path =
      refinePathForGoal(search_radius_range, search_rad_range, path, goal_pose, goal_lane_id);
    EXPECT_EQ(refined_path.points.size(), 7);
    EXPECT_DOUBLE_EQ(refined_path.points.back().point.longitudinal_velocity_mps, 0.0);
    EXPECT_DOUBLE_EQ(refined_path.points.back().point.pose.position.x, 5.2);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, isInLanelets)
{
  using autoware::behavior_path_planner::utils::isInLanelets;
  using autoware::behavior_path_planner::utils::isInLaneletWithYawThreshold;

  {
    const auto pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    EXPECT_FALSE(isInLanelets(pose, current_lanelets));
    EXPECT_FALSE(isInLaneletWithYawThreshold(pose, current_lanelets.front(), M_PI_2));
  }
  {
    EXPECT_TRUE(isInLanelets(planner_data_->self_odometry->pose.pose, current_lanelets));
    EXPECT_TRUE(isInLaneletWithYawThreshold(
      planner_data_->self_odometry->pose.pose, current_lanelets.front(), M_PI_2));
  }
}

TEST_F(BehaviorPathPlanningUtilTest, isEgoWithinOriginalLane)
{
  using autoware::behavior_path_planner::utils::isEgoWithinOriginalLane;
  BehaviorPathPlannerParameters common_param;
  common_param.vehicle_width = 1.0;
  common_param.base_link2front = 1.0;
  common_param.base_link2rear = 1.0;

  {
    const auto pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    EXPECT_FALSE(isEgoWithinOriginalLane(current_lanelets, pose, common_param));
  }
  {
    EXPECT_TRUE(isEgoWithinOriginalLane(
      current_lanelets, planner_data_->self_odometry->pose.pose, common_param));
  }
}

TEST_F(BehaviorPathPlanningUtilTest, getDistanceToNextIntersection)
{
  using autoware::behavior_path_planner::utils::getDistanceToNextIntersection;

  const auto current_pose = planner_data_->self_odometry->pose.pose;
  {
    const lanelet::ConstLanelets empty_lanelets;
    EXPECT_DOUBLE_EQ(
      getDistanceToNextIntersection(current_pose, empty_lanelets),
      std::numeric_limits<double>::infinity());
  }
  {
    EXPECT_NEAR(
      getDistanceToNextIntersection(current_pose, current_lanelets), 117.1599371, epsilon);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, getDistanceToCrosswalk)
{
  using autoware::behavior_path_planner::utils::getDistanceToCrosswalk;

  const auto current_pose = planner_data_->self_odometry->pose.pose;
  const auto routing_graph = planner_data_->route_handler->getOverallGraphPtr();
  {
    const lanelet::ConstLanelets empty_lanelets;
    EXPECT_DOUBLE_EQ(
      getDistanceToCrosswalk(current_pose, empty_lanelets, *routing_graph),
      std::numeric_limits<double>::infinity());
  }
  {
    EXPECT_NEAR(
      getDistanceToCrosswalk(current_pose, current_lanelets, *routing_graph), 120.4423193, epsilon);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, insertStopPoint)
{
  using autoware::behavior_path_planner::utils::insertStopPoint;

  {
    const double length = 100.0;
    auto path = generateTrajectory<PathWithLaneId>(10, 1.0, 1.0);
    EXPECT_DOUBLE_EQ(insertStopPoint(length, path).point.pose.position.x, 0.0);
  }
  {
    const double length = 5.0;
    auto path = generateTrajectory<PathWithLaneId>(10, 1.0, 1.0);
    EXPECT_DOUBLE_EQ(insertStopPoint(length, path).point.pose.position.x, 5.0);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, getSignedDistanceFromBoundary)
{
  using autoware::behavior_path_planner::utils::getSignedDistanceFromBoundary;

  {
    lanelet::ConstLanelets empty_lanelets;
    EXPECT_DOUBLE_EQ(
      getSignedDistanceFromBoundary(empty_lanelets, planner_data_->self_odometry->pose.pose, true),
      0.0);
  }
  {
    EXPECT_NEAR(
      getSignedDistanceFromBoundary(
        current_lanelets, planner_data_->self_odometry->pose.pose, true),
      -1.4952926, epsilon);
    EXPECT_NEAR(
      getSignedDistanceFromBoundary(
        current_lanelets, planner_data_->self_odometry->pose.pose, false),
      1.504715076, epsilon);
  }
  {
    const double vehicle_width = 1.0;
    const double base_link2front = 1.0;
    const double base_link2rear = 1.0;

    const auto left_distance = getSignedDistanceFromBoundary(
      current_lanelets, vehicle_width, base_link2front, base_link2rear,
      planner_data_->self_odometry->pose.pose, true);
    ASSERT_TRUE(left_distance.has_value());
    EXPECT_NEAR(left_distance.value(), -0.9946984, epsilon);

    const auto right_distance = getSignedDistanceFromBoundary(
      current_lanelets, vehicle_width, base_link2front, base_link2rear,
      planner_data_->self_odometry->pose.pose, false);
    ASSERT_TRUE(right_distance.has_value());
    EXPECT_NEAR(right_distance.value(), 1.0041208, epsilon);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, getArcLengthToTargetLanelet)
{
  using autoware::behavior_path_planner::utils::getArcLengthToTargetLanelet;
  {
    auto target_lane = current_lanelets.front();
    EXPECT_DOUBLE_EQ(
      getArcLengthToTargetLanelet(
        current_lanelets, target_lane, planner_data_->self_odometry->pose.pose),
      0.0);
  }
  {
    auto target_lane = current_lanelets.at(1);
    EXPECT_NEAR(
      getArcLengthToTargetLanelet(
        current_lanelets, target_lane, planner_data_->self_odometry->pose.pose),
      86.78265658, epsilon);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, toPolygon2d)
{
  using autoware::behavior_path_planner::utils::toPolygon2d;

  const auto lanelet = current_lanelets.front();
  const auto lanelet_polygon = lanelet.polygon2d().basicPolygon();

  auto polygon_converted_from_lanelet = toPolygon2d(lanelet);
  auto polygon_converted_from_basic_polygon = toPolygon2d(lanelet_polygon);

  EXPECT_EQ(
    polygon_converted_from_lanelet.outer().size(),
    polygon_converted_from_basic_polygon.outer().size());

  for (size_t i = 0; i < polygon_converted_from_lanelet.outer().size(); i++) {
    EXPECT_DOUBLE_EQ(
      polygon_converted_from_lanelet.outer().at(i).x(),
      polygon_converted_from_basic_polygon.outer().at(i).x());
    EXPECT_DOUBLE_EQ(
      polygon_converted_from_lanelet.outer().at(i).y(),
      polygon_converted_from_basic_polygon.outer().at(i).y());
  }
}

TEST_F(BehaviorPathPlanningUtilTest, getHighestProbLabel)
{
  using autoware::behavior_path_planner::utils::getHighestProbLabel;

  PredictedObject obj;
  ObjectClassification classification;

  // Condition: no classification
  EXPECT_EQ(getHighestProbLabel(obj.classification), ObjectClassification::Type::UNKNOWN);

  // Condition: with 2 label
  obj.classification.emplace_back(autoware_perception_msgs::build<ObjectClassification>()
                                    .label(ObjectClassification::CAR)
                                    .probability(0.4));
  obj.classification.emplace_back(autoware_perception_msgs::build<ObjectClassification>()
                                    .label(ObjectClassification::TRUCK)
                                    .probability(0.6));
  EXPECT_EQ(getHighestProbLabel(obj.classification), ObjectClassification::Type::TRUCK);
}

TEST_F(BehaviorPathPlanningUtilTest, getPrecedingLanelets)
{
  using autoware::behavior_path_planner::utils::getPrecedingLanelets;
  const auto & route_handler_ptr = planner_data_->route_handler;

  {
    const lanelet::ConstLanelets target_lanes;
    EXPECT_TRUE(getPrecedingLanelets(
                  *route_handler_ptr, target_lanes, planner_data_->self_odometry->pose.pose, 10.0)
                  .empty());
  }
  {
    lanelet::ConstLanelets target_lanes;
    target_lanes.push_back(route_handler_ptr->getLaneletsFromId(1001));
    EXPECT_TRUE(getPrecedingLanelets(
                  *route_handler_ptr, target_lanes, planner_data_->self_odometry->pose.pose, 0.0)
                  .empty());
  }
  {
    lanelet::ConstLanelets target_lanes;
    target_lanes.push_back(route_handler_ptr->getLaneletsFromId(1011));
    target_lanes.push_back(route_handler_ptr->getLaneletsFromId(1101));
    const auto preceding_lanelets = getPrecedingLanelets(
      *route_handler_ptr, target_lanes, planner_data_->self_odometry->pose.pose, 10.0);
    ASSERT_FALSE(preceding_lanelets.empty());
    EXPECT_EQ(preceding_lanelets.front().data()->id(), 1001);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, getBackwardLanelets)
{
  using autoware::behavior_path_planner::utils::getBackwardLanelets;

  const auto & route_handler_ptr = planner_data_->route_handler;
  lanelet::ConstLanelets target_lanes;
  target_lanes.push_back(route_handler_ptr->getLaneletsFromId(1011));
  const auto backward_lanelets = getBackwardLanelets(
    *route_handler_ptr, target_lanes, planner_data_->self_odometry->pose.pose, 10.0);
  ASSERT_FALSE(backward_lanelets.empty());
  EXPECT_EQ(backward_lanelets.front().id(), 1001);
}

TEST_F(BehaviorPathPlanningUtilTest, calcLaneAroundPose)
{
  using autoware::behavior_path_planner::utils::calcLaneAroundPose;

  {
    const auto pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    std::shared_ptr<autoware::behavior_path_planner::RouteHandler> route_handler_null =
      std::make_shared<autoware::behavior_path_planner::RouteHandler>();
    const auto lane = calcLaneAroundPose(route_handler_null, pose, 10.0, 0.0);
    EXPECT_TRUE(lane.empty());
  }
  {
    const auto lane = calcLaneAroundPose(
      planner_data_->route_handler, planner_data_->self_odometry->pose.pose, 10.0, 0.0);
    EXPECT_EQ(lane.size(), 1);
    EXPECT_EQ(lane.front().id(), 1001);
  }
}

TEST_F(BehaviorPathPlanningUtilTest, checkPathRelativeAngle)
{
  using autoware::behavior_path_planner::utils::checkPathRelativeAngle;

  {
    auto path = generateTrajectory<PathWithLaneId>(2, 1.0);
    EXPECT_TRUE(checkPathRelativeAngle(path, 0.0));
  }
  {
    auto path = generateTrajectory<PathWithLaneId>(10, 1.0);
    EXPECT_TRUE(checkPathRelativeAngle(path, M_PI_2));
  }
}

TEST_F(BehaviorPathPlanningUtilTest, convertToSnakeCase)
{
  using autoware::behavior_path_planner::utils::convertToSnakeCase;

  std::string input_string = "testString";
  auto converted_string = convertToSnakeCase(input_string);
  EXPECT_EQ(converted_string, "test_string");
}

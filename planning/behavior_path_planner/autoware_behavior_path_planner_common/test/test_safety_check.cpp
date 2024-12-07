// Copyright 2023 TIER IV, Inc.
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

#include "autoware/behavior_path_planner_common/utils/path_safety_checker/path_safety_checker_parameters.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/safety_check.hpp"

#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <autoware_vehicle_info_utils/vehicle_info.hpp>

#include <autoware_perception_msgs/msg/detail/shape__struct.hpp>
#include <geometry_msgs/msg/detail/pose__struct.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <tier4_planning_msgs/msg/detail/path_with_lane_id__struct.hpp>

#include <boost/geometry.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

constexpr double epsilon = 1e-6;

using autoware::behavior_path_planner::utils::path_safety_checker::CollisionCheckDebug;
using autoware::behavior_path_planner::utils::path_safety_checker::CollisionCheckDebugMap;
using autoware::behavior_path_planner::utils::path_safety_checker::ExtendedPredictedObject;
using autoware::behavior_path_planner::utils::path_safety_checker::
  PoseWithVelocityAndPolygonStamped;
using autoware::behavior_path_planner::utils::path_safety_checker::PoseWithVelocityStamped;
using autoware::behavior_path_planner::utils::path_safety_checker::PredictedPathWithPolygon;
using autoware::behavior_path_planner::utils::path_safety_checker::RSSparams;
using autoware::test_utils::createPose;
using autoware::test_utils::generateTrajectory;
using autoware::universe_utils::Polygon2d;
using autoware_perception_msgs::msg::Shape;
using geometry_msgs::msg::Pose;
using tier4_planning_msgs::msg::PathWithLaneId;

std::vector<PoseWithVelocityStamped> create_test_path()
{
  std::vector<PoseWithVelocityStamped> path;
  path.emplace_back(0.0, createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), 1.0);
  path.emplace_back(1.0, createPose(1.0, 0.0, 0.0, 0.0, 0.0, 0.0), 2.0);
  path.emplace_back(2.0, createPose(2.0, 0.0, 0.0, 0.0, 0.0, 0.0), 3.0);
  return path;
}

RSSparams create_rss_parameters(
  double longitudinal_velocity_delta_time = 2.0, double rear_vehicle_reaction_time = 1.0,
  double rear_vehicle_safety_time_margin = 1.0, double longitudinal_distance_min_threshold = 3.0,
  double rear_vehicle_deceleration = -1.0, double front_vehicle_deceleration = -2.0)
{
  RSSparams params;
  params.longitudinal_velocity_delta_time = longitudinal_velocity_delta_time;
  params.rear_vehicle_reaction_time = rear_vehicle_reaction_time;
  params.rear_vehicle_safety_time_margin = rear_vehicle_safety_time_margin;
  params.longitudinal_distance_min_threshold = longitudinal_distance_min_threshold;
  params.rear_vehicle_deceleration = rear_vehicle_deceleration;
  params.front_vehicle_deceleration = front_vehicle_deceleration;
  params.extended_polygon_policy = "rectangle";

  return params;
}

std::vector<PoseWithVelocityAndPolygonStamped> create_path_with_velocity_and_polygon(
  const Pose initial_pose, const Shape & shape, size_t point_num, double interval, double velocity)
{
  std::vector<PoseWithVelocityAndPolygonStamped> predicted_path;
  predicted_path.reserve(point_num);
  Pose pose = initial_pose;

  for (size_t i = 0; i < point_num; i++) {
    double time = static_cast<double>(i) * interval;
    pose.position.x = initial_pose.position.x + time * velocity;
    PoseWithVelocityAndPolygonStamped obj_pose_with_poly(
      time, pose, velocity, autoware::universe_utils::toPolygon2d(pose, shape));
    predicted_path.push_back(obj_pose_with_poly);
  }

  return predicted_path;
}

PredictedPathWithPolygon create_predicted_path_with_polygon(Pose pose, float confidence)
{
  PredictedPathWithPolygon path;
  Shape shape;
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 1.0;
  shape.dimensions.y = 1.0;
  path.path = create_path_with_velocity_and_polygon(pose, shape, 10, 1.0, 1.0);
  path.confidence = confidence;

  return path;
}

ExtendedPredictedObject create_extended_predicted_object(Pose pose, float confidence)
{
  ExtendedPredictedObject object;
  Shape shape;
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 1.0;
  shape.dimensions.y = 1.0;
  object.initial_pose = pose;
  object.shape = shape;
  object.initial_polygon = autoware::universe_utils::toPolygon2d(pose, shape);
  object.predicted_paths.push_back(create_predicted_path_with_polygon(pose, confidence));

  return object;
}

TEST(BehaviorPathPlanningSafetyUtilsTest, isTargetObjectOncoming)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::isTargetObjectOncoming;

  auto vehicle_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  auto object_pose = createPose(10.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  // Condition: same orientation
  EXPECT_FALSE(isTargetObjectOncoming(vehicle_pose, object_pose));

  // Condition: facing each other
  object_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(M_PI);
  EXPECT_TRUE(isTargetObjectOncoming(vehicle_pose, object_pose));

  // Condition: Narrow angle threshold
  double angle_threshold = 0.75 * M_PI;
  object_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(M_PI_2);
  EXPECT_FALSE(isTargetObjectOncoming(vehicle_pose, object_pose, angle_threshold));
}

TEST(BehaviorPathPlanningSafetyUtilsTest, isTargetObjectFront)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::isTargetObjectFront;

  auto ego_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  double base_to_front = 0.5;
  Shape shape;
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 5.0;
  shape.dimensions.y = 2.0;
  auto obj_polygon =
    autoware::universe_utils::toPolygon2d(createPose(10.0, 0.0, 0.0, 0.0, 0.0, 0.0), shape);

  // Condition: object in front
  EXPECT_TRUE(isTargetObjectFront(ego_pose, obj_polygon, base_to_front));

  // Condition: object behind
  obj_polygon =
    autoware::universe_utils::toPolygon2d(createPose(-10.0, 0.0, 0.0, 0.0, 0.0, 0.0), shape);
  EXPECT_FALSE(isTargetObjectFront(ego_pose, obj_polygon, base_to_front));

  // Condition: object overlapping
  obj_polygon =
    autoware::universe_utils::toPolygon2d(createPose(3.0, 0.0, 0.0, 0.0, 0.0, 0.0), shape);
  EXPECT_TRUE(isTargetObjectFront(ego_pose, obj_polygon, base_to_front));
}

TEST(BehaviorPathPlanningSafetyUtilsTest, createExtendedEgoPolygon)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::createExtendedPolygon;

  autoware::vehicle_info_utils::VehicleInfo vehicle_info;
  vehicle_info.max_longitudinal_offset_m = 4.0;
  vehicle_info.vehicle_width_m = 2.0;
  vehicle_info.rear_overhang_m = 1.0;
  CollisionCheckDebug debug;

  {
    Pose ego_pose;
    ego_pose.position = autoware::universe_utils::createPoint(0.0, 0.0, 0.0);
    ego_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(0.0);

    const double lon_length = 10.0;
    const double lat_margin = 2.0;
    const bool is_stopped_object = false;

    const auto polygon = createExtendedPolygon(
      ego_pose, vehicle_info, lon_length, lat_margin, is_stopped_object, debug);

    EXPECT_EQ(polygon.outer().size(), static_cast<unsigned int>(5));

    const auto & p1 = polygon.outer().at(0);
    const auto & p2 = polygon.outer().at(1);
    const auto & p3 = polygon.outer().at(2);
    const auto & p4 = polygon.outer().at(3);
    EXPECT_NEAR(p1.x(), 14.0, epsilon);
    EXPECT_NEAR(p1.y(), 3.0, epsilon);
    EXPECT_NEAR(p2.x(), 14.0, epsilon);
    EXPECT_NEAR(p2.y(), -3.0, epsilon);
    EXPECT_NEAR(p3.x(), -1.0, epsilon);
    EXPECT_NEAR(p3.y(), -3.0, epsilon);
    EXPECT_NEAR(p4.x(), -1.0, epsilon);
    EXPECT_NEAR(p4.y(), 3.0, epsilon);
  }

  {
    Pose ego_pose;
    ego_pose.position = autoware::universe_utils::createPoint(3.0, 4.0, 0.0);
    ego_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(0.0);

    const double lon_length = 10.0;
    const double lat_margin = 2.0;
    const bool is_stopped_object = false;

    const auto polygon = createExtendedPolygon(
      ego_pose, vehicle_info, lon_length, lat_margin, is_stopped_object, debug);

    EXPECT_EQ(polygon.outer().size(), static_cast<unsigned int>(5));

    const auto & p1 = polygon.outer().at(0);
    const auto & p2 = polygon.outer().at(1);
    const auto & p3 = polygon.outer().at(2);
    const auto & p4 = polygon.outer().at(3);
    EXPECT_NEAR(p1.x(), 17.0, epsilon);
    EXPECT_NEAR(p1.y(), 7.0, epsilon);
    EXPECT_NEAR(p2.x(), 17.0, epsilon);
    EXPECT_NEAR(p2.y(), 1.0, epsilon);
    EXPECT_NEAR(p3.x(), 2.0, epsilon);
    EXPECT_NEAR(p3.y(), 1.0, epsilon);
    EXPECT_NEAR(p4.x(), 2.0, epsilon);
    EXPECT_NEAR(p4.y(), 7.0, epsilon);
  }

  {
    Pose ego_pose;
    ego_pose.position = autoware::universe_utils::createPoint(0.0, 0.0, 0.0);
    ego_pose.orientation =
      autoware::universe_utils::createQuaternionFromYaw(autoware::universe_utils::deg2rad(60));

    const double lon_length = 10.0;
    const double lat_margin = 2.0;
    const bool is_stopped_object = false;

    const auto polygon = createExtendedPolygon(
      ego_pose, vehicle_info, lon_length, lat_margin, is_stopped_object, debug);

    EXPECT_EQ(polygon.outer().size(), static_cast<unsigned int>(5));

    const auto & p1 = polygon.outer().at(0);
    const auto & p2 = polygon.outer().at(1);
    const auto & p3 = polygon.outer().at(2);
    const auto & p4 = polygon.outer().at(3);
    EXPECT_NEAR(p1.x(), 7.0 - 1.5 * std::sqrt(3), epsilon);
    EXPECT_NEAR(p1.y(), 7.0 * std::sqrt(3) + 1.5, epsilon);
    EXPECT_NEAR(p2.x(), 7.0 + 1.5 * std::sqrt(3), epsilon);
    EXPECT_NEAR(p2.y(), 7.0 * std::sqrt(3) - 1.5, epsilon);
    EXPECT_NEAR(p3.x(), 1.5 * std::sqrt(3) - 0.5, epsilon);
    EXPECT_NEAR(p3.y(), -1.5 - std::sqrt(3) / 2.0, epsilon);
    EXPECT_NEAR(p4.x(), -1.5 * std::sqrt(3) - 0.5, epsilon);
    EXPECT_NEAR(p4.y(), 1.5 - std::sqrt(3) / 2.0, epsilon);
  }
}

TEST(BehaviorPathPlanningSafetyUtilsTest, createExtendedObjPolygon)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::createExtendedPolygon;
  using autoware::universe_utils::createPoint;
  using autoware::universe_utils::createQuaternionFromYaw;

  {
    Pose obj_pose;
    obj_pose.position = createPoint(0.0, 0.0, 0.0);
    obj_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(0.0);

    Shape shape;
    shape.type = autoware_perception_msgs::msg::Shape::POLYGON;
    shape.footprint.points.resize(5);
    shape.footprint.points.at(0).x = 3.0;
    shape.footprint.points.at(0).y = 0.0;
    shape.footprint.points.at(1).x = 0.0;
    shape.footprint.points.at(1).y = -2.0;
    shape.footprint.points.at(2).x = -2.0;
    shape.footprint.points.at(2).y = 0.0;
    shape.footprint.points.at(3).x = -1.0;
    shape.footprint.points.at(3).y = 0.5;
    shape.footprint.points.at(4).x = 2.0;
    shape.footprint.points.at(4).y = 1.0;

    const double lon_length = 10.0;
    const double lat_margin = 2.0;
    const bool is_stopped_object = false;

    CollisionCheckDebug debug;

    PoseWithVelocityAndPolygonStamped obj_pose_with_poly(
      0.0, obj_pose, 0.0, autoware::universe_utils::toPolygon2d(obj_pose, shape));
    const auto polygon =
      createExtendedPolygon(obj_pose_with_poly, lon_length, lat_margin, is_stopped_object, debug);

    EXPECT_EQ(polygon.outer().size(), static_cast<unsigned int>(5));

    const auto & p1 = polygon.outer().at(0);
    const auto & p2 = polygon.outer().at(1);
    const auto & p3 = polygon.outer().at(2);
    const auto & p4 = polygon.outer().at(3);
    EXPECT_NEAR(p1.x(), 13.0, epsilon);
    EXPECT_NEAR(p1.y(), 3.0, epsilon);
    EXPECT_NEAR(p2.x(), 13.0, epsilon);
    EXPECT_NEAR(p2.y(), -4.0, epsilon);
    EXPECT_NEAR(p3.x(), -2.0, epsilon);
    EXPECT_NEAR(p3.y(), -4.0, epsilon);
    EXPECT_NEAR(p4.x(), -2.0, epsilon);
    EXPECT_NEAR(p4.y(), 3.0, epsilon);
  }
}

TEST(BehaviorPathPlanningSafetyUtilsTest, convertToPredictedPath)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::convertToPredictedPath;

  auto path = create_test_path();
  double time_resolution = 1.0;
  auto predicted_path = convertToPredictedPath(path, time_resolution);
  EXPECT_EQ(predicted_path.path.size(), 3);
  EXPECT_DOUBLE_EQ(predicted_path.time_step.sec, 1.0);
  EXPECT_DOUBLE_EQ(predicted_path.time_step.nanosec, 0.0);
}

TEST(BehaviorPathPlanningSafetyUtilsTest, calcRssDistance)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::calcRssDistance;

  {
    const double front_vel = 5.0;
    const double rear_vel = 10.0;
    auto params = create_rss_parameters();

    EXPECT_NEAR(calcRssDistance(front_vel, rear_vel, params), 63.75, epsilon);
  }
}

TEST(BehaviorPathPlanningSafetyUtilsTest, calc_minimum_longitudinal_length)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::
    calc_minimum_longitudinal_length;

  double front_object_velocity = 10.0;
  double rear_object_velocity = 5.0;
  auto param = create_rss_parameters();
  param.longitudinal_distance_min_threshold = 4.0;

  // Condition: front is faster than rear object
  EXPECT_DOUBLE_EQ(
    calc_minimum_longitudinal_length(front_object_velocity, rear_object_velocity, param), 24.0);

  // Condition: front is faster than rear object
}

// Basic interpolation test
TEST(CalcInterpolatedPoseWithVelocityTest, BasicInterpolation)
{
  auto path = create_test_path();
  auto result = calc_interpolated_pose_with_velocity(path, 0.5);

  ASSERT_TRUE(result.has_value());
  EXPECT_NEAR(result->time, 0.5, 1e-6);
  EXPECT_NEAR(result->pose.position.x, 0.5, 1e-6);
  EXPECT_NEAR(result->velocity, 1.5, 1e-6);
}

// Boundary conditions test
TEST(CalcInterpolatedPoseWithVelocityTest, BoundaryConditions)
{
  auto path = create_test_path();

  // First point of the path
  auto start_result = calc_interpolated_pose_with_velocity(path, 0.0);
  ASSERT_TRUE(start_result.has_value());
  EXPECT_NEAR(start_result->time, 0.0, 1e-6);
  EXPECT_NEAR(start_result->pose.position.x, 0.0, 1e-6);
  EXPECT_NEAR(start_result->velocity, 1.0, 1e-6);

  // Last point of the path
  auto end_result = calc_interpolated_pose_with_velocity(path, 2.0);
  ASSERT_TRUE(end_result.has_value());
  EXPECT_NEAR(end_result->time, 2.0, 1e-6);
  EXPECT_NEAR(end_result->pose.position.x, 2.0, 1e-6);
  EXPECT_NEAR(end_result->velocity, 3.0, 1e-6);
}

// Invalid input test
TEST(CalcInterpolatedPoseWithVelocityTest, InvalidInput)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::
    calc_interpolated_pose_with_velocity;

  auto path = create_test_path();

  // Empty path
  EXPECT_FALSE(calc_interpolated_pose_with_velocity({}, 1.0).has_value());

  // Negative relative time
  EXPECT_FALSE(calc_interpolated_pose_with_velocity(path, -1.0).has_value());

  // Relative time greater than the last time in the path
  EXPECT_FALSE(calc_interpolated_pose_with_velocity(path, 3.0).has_value());
}

// Special cases test
TEST(CalcInterpolatedPoseWithVelocityTest, DISABLED_SpecialCases)
{
  // Case with consecutive points at the same time
  std::vector<PoseWithVelocityStamped> same_time_path;
  same_time_path.emplace_back(0.0, createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), 1.0);
  same_time_path.emplace_back(0.0, createPose(1.0, 0.0, 0.0, 0.0, 0.0, 0.0), 2.0);
  same_time_path.emplace_back(1.0, createPose(2.0, 0.0, 0.0, 0.0, 0.0, 0.0), 3.0);

  auto same_time_result = calc_interpolated_pose_with_velocity(same_time_path, 0.0);
  ASSERT_TRUE(same_time_result.has_value());
  EXPECT_NEAR(same_time_result->pose.position.x, 0.0, 1e-6);
  EXPECT_NEAR(same_time_result->velocity, 1.0, 1e-6);

  // Case with reversed time order
  std::vector<PoseWithVelocityStamped> reverse_time_path;
  reverse_time_path.emplace_back(2.0, createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), 1.0);
  reverse_time_path.emplace_back(1.0, createPose(1.0, 0.0, 0.0, 0.0, 0.0, 0.0), 2.0);
  reverse_time_path.emplace_back(0.0, createPose(2.0, 0.0, 0.0, 0.0, 0.0, 0.0), 3.0);

  auto reverse_time_result = calc_interpolated_pose_with_velocity(reverse_time_path, 1.5);
  ASSERT_FALSE(reverse_time_result.has_value());
}

TEST(BehaviorPathPlanningSafetyUtilsTest, get_interpolated_pose_with_velocity_and_polygon_stamped)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::
    get_interpolated_pose_with_velocity_and_polygon_stamped;

  std::vector<PoseWithVelocityStamped> pred_path;
  double current_time = 0.5;
  autoware::vehicle_info_utils::VehicleInfo vehicle_info{};
  vehicle_info.max_longitudinal_offset_m = 1.0;
  vehicle_info.rear_overhang_m = 1.0;
  vehicle_info.vehicle_width_m = 2.0;

  // Condition: empty path
  EXPECT_FALSE(
    get_interpolated_pose_with_velocity_and_polygon_stamped(pred_path, current_time, vehicle_info)
      .has_value());

  // Condition: with path
  pred_path = create_test_path();
  auto interpolation_result =
    get_interpolated_pose_with_velocity_and_polygon_stamped(pred_path, current_time, vehicle_info);
  EXPECT_TRUE(interpolation_result.has_value());
}

TEST(BehaviorPathPlanningSafetyUtilsTest, filterPredictedPathAfterTargetPose)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::
    filterPredictedPathAfterTargetPose;

  auto path = create_test_path();
  Pose pose = createPose(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  auto filtered_path = filterPredictedPathAfterTargetPose(path, pose);
  EXPECT_EQ(filtered_path.size(), 2);
  double x_target = 1.0;
  for (const auto & pose_with_velocity : filtered_path) {
    EXPECT_DOUBLE_EQ(pose_with_velocity.pose.position.x, x_target);
    x_target += 1.0;
  }
}

TEST(BehaviorPathPlanningSafetyUtilsTest, checkSafetyWithRSS)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::checkSafetyWithRSS;

  auto planned_path = generateTrajectory<PathWithLaneId>(3, 1.0);
  auto ego_predicted_path = create_test_path();
  autoware::vehicle_info_utils::VehicleInfo vehicle_info{};
  vehicle_info.max_longitudinal_offset_m = 1.0;
  vehicle_info.rear_overhang_m = 1.0;
  vehicle_info.vehicle_width_m = 2.0;
  std::vector<ExtendedPredictedObject> objects;
  objects.push_back(
    create_extended_predicted_object(createPose(10.0, 8.0, 0.0, 0.0, 0.0, 0.0), 0.5));
  objects.push_back(
    create_extended_predicted_object(createPose(0.0, 1.0, 0.0, 0.0, 0.0, 0.0), 0.6));
  CollisionCheckDebugMap debug_map;
  BehaviorPathPlannerParameters parameters;
  parameters.vehicle_info = vehicle_info;
  auto rss_params = create_rss_parameters();
  double hysteresis_factor = 1.0;
  const double yaw_difference_th = M_PI_2;

  EXPECT_FALSE(checkSafetyWithRSS(
    planned_path, ego_predicted_path, objects, debug_map, parameters, rss_params, true,
    hysteresis_factor, yaw_difference_th));
  objects.pop_back();
  EXPECT_TRUE(checkSafetyWithRSS(
    planned_path, ego_predicted_path, objects, debug_map, parameters, rss_params, true,
    hysteresis_factor, yaw_difference_th));
}

TEST(BehaviorPathPlanningSafetyUtilsTest, checkSafetyWithIntegralPredictedPolygon)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::
    checkSafetyWithIntegralPredictedPolygon;

  auto ego_predicted_path = create_test_path();
  autoware::vehicle_info_utils::VehicleInfo vehicle_info{};
  vehicle_info.max_longitudinal_offset_m = 1.0;
  vehicle_info.rear_overhang_m = 1.0;
  vehicle_info.vehicle_width_m = 2.0;
  autoware::behavior_path_planner::utils::path_safety_checker::ExtendedPredictedObjects objects;
  objects.push_back(
    create_extended_predicted_object(createPose(10.0, 8.0, 0.0, 0.0, 0.0, 0.0), 0.5));
  objects.push_back(
    create_extended_predicted_object(createPose(0.0, 1.0, 0.0, 0.0, 0.0, 0.0), 0.6));

  autoware::behavior_path_planner::utils::path_safety_checker::IntegralPredictedPolygonParams
    params{1.0, 1.0, 1.0, 2.0};
  CollisionCheckDebugMap debug_map;

  EXPECT_FALSE(checkSafetyWithIntegralPredictedPolygon(
    ego_predicted_path, vehicle_info, objects, true, params, debug_map));

  objects.pop_back();
  EXPECT_TRUE(checkSafetyWithIntegralPredictedPolygon(
    ego_predicted_path, vehicle_info, objects, true, params, debug_map));
}

TEST(BehaviorPathPlanningSafetyUtilsTest, checkCollision)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::checkCollision;
  using autoware::behavior_path_planner::utils::path_safety_checker::get_collided_polygons;

  auto planned_path = autoware::test_utils::generateTrajectory<PathWithLaneId>(3, 1.0);
  auto predicted_ego_path = create_test_path();

  ExtendedPredictedObject target_object;
  target_object.initial_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  Shape shape;
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 2.0;
  shape.dimensions.y = 2.0;
  auto object_path =
    create_predicted_path_with_polygon(createPose(0.0, 1.0, 0.0, 0.0, 0.0, 0.0), 1.0);

  autoware::vehicle_info_utils::VehicleInfo vehicle_info;
  vehicle_info.max_longitudinal_offset_m = 4.0;
  vehicle_info.vehicle_width_m = 2.0;
  vehicle_info.rear_overhang_m = 1.0;
  BehaviorPathPlannerParameters common_parameters;
  common_parameters.vehicle_info = vehicle_info;

  auto rss_parameters = create_rss_parameters();
  double hysteresis_factor = 1.0;
  double max_velocity_limit = 10.0;
  const double yaw_difference_th = M_PI_2;
  CollisionCheckDebug debug;

  {
    EXPECT_FALSE(checkCollision(
      planned_path, predicted_ego_path, target_object, object_path, common_parameters,
      rss_parameters, hysteresis_factor, yaw_difference_th, debug));
    auto collide_polygon = get_collided_polygons(
      planned_path, predicted_ego_path, target_object, object_path, vehicle_info, rss_parameters,
      hysteresis_factor, max_velocity_limit, yaw_difference_th, debug);
    EXPECT_EQ(collide_polygon.size(), 3);
  }
  {
    target_object.initial_pose = createPose(0.0, 4.0, 0.0, 0.0, 0.0, 0.0);
    object_path = create_predicted_path_with_polygon(createPose(0.0, 4.0, 0.0, 0.0, 0.0, 0.0), 1.0);
    auto collide_polygon = get_collided_polygons(
      planned_path, predicted_ego_path, target_object, object_path, vehicle_info, rss_parameters,
      hysteresis_factor, max_velocity_limit, yaw_difference_th, debug);
    EXPECT_TRUE(checkCollision(
      planned_path, predicted_ego_path, target_object, object_path, common_parameters,
      rss_parameters, hysteresis_factor, yaw_difference_th, debug));
    EXPECT_TRUE(collide_polygon.empty());
  }
  {
    target_object.initial_pose = createPose(10.0, 4.0, 0.0, 0.0, 0.0, 0.0);
    object_path =
      create_predicted_path_with_polygon(createPose(10.0, 4.0, 0.0, 0.0, 0.0, 0.0), 1.0);
    EXPECT_TRUE(checkCollision(
      planned_path, predicted_ego_path, target_object, object_path, common_parameters,
      rss_parameters, hysteresis_factor, yaw_difference_th, debug));
    auto collide_polygon = get_collided_polygons(
      planned_path, predicted_ego_path, target_object, object_path, vehicle_info, rss_parameters,
      hysteresis_factor, max_velocity_limit, yaw_difference_th, debug);
    EXPECT_TRUE(collide_polygon.empty());
  }
  {
    rss_parameters.extended_polygon_policy = "along_path";
    target_object.initial_pose = createPose(10.0, 4.0, 0.0, 0.0, 0.0, 0.0);
    object_path =
      create_predicted_path_with_polygon(createPose(10.0, 4.0, 0.0, 0.0, 0.0, 0.0), 1.0);
    EXPECT_TRUE(checkCollision(
      planned_path, predicted_ego_path, target_object, object_path, common_parameters,
      rss_parameters, hysteresis_factor, yaw_difference_th, debug));
    auto collide_polygon = get_collided_polygons(
      planned_path, predicted_ego_path, target_object, object_path, vehicle_info, rss_parameters,
      hysteresis_factor, max_velocity_limit, yaw_difference_th, debug);
    EXPECT_TRUE(collide_polygon.empty());
  }
}

TEST(BehaviorPathPlanningSafetyUtilsTest, checkPolygonsIntersects)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::checkPolygonsIntersects;

  std::vector<Polygon2d> poly_1;
  std::vector<Polygon2d> poly_2;
  Shape shape;
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 5.0;
  shape.dimensions.y = 2.0;

  poly_1.push_back(
    autoware::universe_utils::toPolygon2d(createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), shape));
  poly_2.push_back(
    autoware::universe_utils::toPolygon2d(createPose(10.0, 2.0, 0.0, 0.0, 0.0, 0.0), shape));

  // Condition: no collision
  EXPECT_FALSE(checkPolygonsIntersects(poly_1, poly_2));

  // Condition: collide
  poly_2.push_back(
    autoware::universe_utils::toPolygon2d(createPose(1.0, 2.0, 0.0, 0.0, 0.0, 0.0), shape));
  EXPECT_TRUE(checkPolygonsIntersects(poly_1, poly_2));
}

TEST(BehaviorPathPlanningSafetyUtilsTest, calc_obstacle_length)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::calc_obstacle_max_length;
  using autoware::behavior_path_planner::utils::path_safety_checker::calc_obstacle_min_length;

  Shape shape;

  // Condition: bounding box
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 6.0;
  shape.dimensions.y = 8.0;
  EXPECT_DOUBLE_EQ(calc_obstacle_min_length(shape), 3.0);
  EXPECT_DOUBLE_EQ(calc_obstacle_max_length(shape), 5.0);

  // Condition: cylinder
  shape.type = autoware_perception_msgs::msg::Shape::CYLINDER;
  EXPECT_DOUBLE_EQ(calc_obstacle_min_length(shape), 3.0);
  EXPECT_DOUBLE_EQ(calc_obstacle_max_length(shape), 3.0);

  // Condition: polygon
  shape.type = autoware_perception_msgs::msg::Shape::POLYGON;
  shape.footprint.points.resize(5);
  shape.footprint.points.at(0).x = 3.0;
  shape.footprint.points.at(0).y = 0.0;
  shape.footprint.points.at(1).x = 0.0;
  shape.footprint.points.at(1).y = -2.0;
  shape.footprint.points.at(2).x = -2.0;
  shape.footprint.points.at(2).y = 0.0;
  shape.footprint.points.at(3).x = 0.0;
  shape.footprint.points.at(3).y = 1.0;
  shape.footprint.points.at(4).x = 1.0;
  shape.footprint.points.at(4).y = 1.0;
  EXPECT_DOUBLE_EQ(calc_obstacle_min_length(shape), 1.0);
  EXPECT_DOUBLE_EQ(calc_obstacle_max_length(shape), 3.0);

  // Condition: invalid shape
  shape.type = 100;
  EXPECT_ANY_THROW(calc_obstacle_min_length(shape));
  EXPECT_ANY_THROW(calc_obstacle_max_length(shape));
}

TEST(BehaviorPathPlanningSafetyUtilsTest, checkObjectsCollisionRough)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::checkObjectsCollisionRough;

  auto path = generateTrajectory<PathWithLaneId>(10, 1.0);
  autoware_perception_msgs::msg::PredictedObjects objs;
  double margin = 0.1;
  BehaviorPathPlannerParameters param;
  param.vehicle_width = 2.0;
  param.front_overhang = 1.0;
  param.rear_overhang = 1.0;
  bool use_offset_ego_point = true;

  // Condition: no object
  auto rough_object_collision =
    checkObjectsCollisionRough(path, objs, margin, param, use_offset_ego_point);
  EXPECT_FALSE(rough_object_collision.first);
  EXPECT_FALSE(rough_object_collision.second);

  // Condition: collides with minimum distance
  autoware_perception_msgs::msg::PredictedObject obj;
  obj.kinematics.initial_pose_with_covariance.pose = createPose(8.0, 3.0, 0.0, 0.0, 0.0, 0.0);
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 3.0;
  obj.shape.dimensions.y = 1.0;
  objs.objects.push_back(obj);

  rough_object_collision =
    checkObjectsCollisionRough(path, objs, margin, param, use_offset_ego_point);
  EXPECT_TRUE(rough_object_collision.first);
  EXPECT_FALSE(rough_object_collision.second);

  // Condition: collides with both distance
  obj.kinematics.initial_pose_with_covariance.pose = createPose(2.0, 1.0, 0.0, 0.0, 0.0, 0.0);
  objs.objects.clear();
  objs.objects.push_back(obj);
  rough_object_collision =
    checkObjectsCollisionRough(path, objs, margin, param, use_offset_ego_point);
  EXPECT_TRUE(rough_object_collision.first);
  EXPECT_TRUE(rough_object_collision.second);

  // Condition: use_offset_ego_point set to false
  use_offset_ego_point = false;
  rough_object_collision =
    checkObjectsCollisionRough(path, objs, margin, param, use_offset_ego_point);
  EXPECT_TRUE(rough_object_collision.first);
  EXPECT_TRUE(rough_object_collision.second);
}

TEST(BehaviorPathPlanningSafetyUtilsTest, calculateRoughDistanceToObjects)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::
    calculateRoughDistanceToObjects;

  auto path = generateTrajectory<PathWithLaneId>(10, 0.1);
  autoware_perception_msgs::msg::PredictedObjects objs;
  BehaviorPathPlannerParameters param;
  param.vehicle_width = 2.0;
  param.front_overhang = 1.0;
  param.rear_overhang = 1.0;
  bool use_offset_ego_point = true;
  std::string distance_type = "none";

  autoware_perception_msgs::msg::PredictedObject obj;
  obj.kinematics.initial_pose_with_covariance.pose = createPose(8.0, 1.0, 0.0, 0.0, 0.0, 0.0);
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions.x = 3.0;
  obj.shape.dimensions.y = 1.0;
  objs.objects.push_back(obj);

  // Condition: invalid distance_type
  EXPECT_ANY_THROW(
    calculateRoughDistanceToObjects(path, objs, param, use_offset_ego_point, distance_type));

  // Condition: minimum distance
  distance_type = "min";
  EXPECT_NEAR(
    calculateRoughDistanceToObjects(path, objs, param, use_offset_ego_point, distance_type),
    4.1747243156, epsilon);

  // Condition: maximum distance
  distance_type = "max";
  EXPECT_NEAR(
    calculateRoughDistanceToObjects(path, objs, param, use_offset_ego_point, distance_type),
    6.1700767081, epsilon);
}

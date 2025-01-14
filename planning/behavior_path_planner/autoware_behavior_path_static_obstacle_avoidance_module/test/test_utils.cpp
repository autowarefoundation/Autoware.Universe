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

#include "../src/utils.cpp"  // NOLINT
#include "autoware/behavior_path_static_obstacle_avoidance_module/data_structs.hpp"
#include "autoware/behavior_path_static_obstacle_avoidance_module/helper.hpp"
#include "autoware/behavior_path_static_obstacle_avoidance_module/type_alias.hpp"
#include "autoware/behavior_path_static_obstacle_avoidance_module/utils.hpp"
#include "autoware/universe_utils/math/unit_conversion.hpp"
#include "autoware_test_utils/autoware_test_utils.hpp"

#include <autoware_perception_msgs/msg/object_classification.hpp>
#include <autoware_perception_msgs/msg/shape.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <limits>
#include <memory>

namespace autoware::behavior_path_planner::utils::static_obstacle_avoidance
{

using autoware::behavior_path_planner::AvoidanceParameters;
using autoware::behavior_path_planner::ObjectData;
using autoware::behavior_path_planner::helper::static_obstacle_avoidance::AvoidanceHelper;
using autoware::route_handler::Direction;
using autoware::universe_utils::createPoint;
using autoware::universe_utils::createQuaternionFromRPY;
using autoware::universe_utils::createVector3;
using autoware::universe_utils::deg2rad;
using autoware::universe_utils::generateUUID;

constexpr double epsilon = 1e-6;

ObjectData create_test_object(
  const Pose & pose, const Vector3 & velocity, const Shape & shape, const uint8_t type)
{
  ObjectData object_data;
  object_data.object.shape = shape;
  object_data.object.kinematics.initial_pose_with_covariance.pose = pose;
  object_data.object.kinematics.initial_twist_with_covariance.twist.linear = velocity;
  object_data.object.classification.emplace_back(
    autoware_perception_msgs::build<ObjectClassification>().label(type).probability(1.0));
  return object_data;
}

auto get_planner_data() -> std::shared_ptr<PlannerData>
{
  PlannerData planner_data;

  // set odometry.
  Odometry odometry;
  odometry.pose.pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                         .position(createPoint(0.0, 0.0, 0.0))
                         .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));
  odometry.twist.twist.linear = createVector3(0.0, 0.0, 0.0);

  planner_data.self_odometry = std::make_shared<Odometry>(odometry);

  // set vehicle info.
  planner_data.parameters.vehicle_width = 2.0;

  return std::make_shared<PlannerData>(planner_data);
}

auto get_parameters() -> std::shared_ptr<AvoidanceParameters>
{
  AvoidanceParameters parameters;

  parameters.object_check_backward_distance = 1.0;
  parameters.object_check_goal_distance = 2.0;
  parameters.object_check_yaw_deviation = deg2rad(20);
  parameters.object_check_backward_distance = 1.0;
  parameters.object_check_goal_distance = 2.0;
  parameters.object_check_shiftable_ratio = 0.6;
  parameters.object_check_min_road_shoulder_width = 0.5;
  parameters.object_last_seen_threshold = 0.2;
  parameters.threshold_distance_object_is_on_center = 1.0;
  parameters.hard_drivable_bound_margin = 0.1;
  parameters.soft_drivable_bound_margin = 0.5;
  parameters.lateral_execution_threshold = 0.5;

  parameters.min_prepare_distance = 1.5;
  parameters.max_prepare_time = 3.0;
  parameters.nominal_avoidance_speed = 8.0;
  parameters.velocity_map = {1.0, 3.0, 10.0};
  parameters.lateral_min_jerk_map = {0.1, 1.0, 10.0};
  parameters.lateral_max_jerk_map = {0.4, 1.5, 15.0};
  parameters.lateral_max_accel_map = {0.7, 0.8, 0.9};

  const auto get_truck_param = []() {
    ObjectParameter param{};
    param.is_avoidance_target = true;
    param.is_safety_check_target = true;
    param.moving_time_threshold = 0.5;
    param.moving_speed_threshold = 1.0;
    param.lateral_soft_margin = 0.7;
    param.lateral_hard_margin = 0.2;
    param.lateral_hard_margin_for_parked_vehicle = 0.7;
    param.envelope_buffer_margin = 0.0;
    param.th_error_eclipse_long_radius = 2.0;
    return param;
  };

  parameters.object_parameters.emplace(ObjectClassification::TRUCK, get_truck_param());

  const auto get_unknown_param = []() {
    ObjectParameter param{};
    param.is_avoidance_target = false;
    param.is_safety_check_target = false;
    param.moving_time_threshold = 0.5;
    param.moving_speed_threshold = 1.0;
    param.lateral_soft_margin = 0.7;
    param.lateral_hard_margin = 0.2;
    param.lateral_hard_margin_for_parked_vehicle = 0.7;
    param.envelope_buffer_margin = 0.0;
    param.th_error_eclipse_long_radius = 2.0;
    return param;
  };

  parameters.object_parameters.emplace(ObjectClassification::UNKNOWN, get_unknown_param());

  return std::make_shared<AvoidanceParameters>(parameters);
}

TEST(TestUtils, isMovingObject)
{
  {
    ObjectData object_data;
    object_data.move_time = 0.49;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    EXPECT_FALSE(filtering_utils::isMovingObject(object_data, get_parameters()));
  }

  {
    ObjectData object_data;
    object_data.move_time = 0.51;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    EXPECT_TRUE(filtering_utils::isMovingObject(object_data, get_parameters()));
  }
}

TEST(TestUtils, getObjectBehavior)
{
  const auto parameters = get_parameters();

  lanelet::LineString3d left_bound;
  lanelet::LineString3d right_bound;

  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, -1, -1});
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 0, -1});
  left_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, -1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, -1, 1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 0, 1});
  right_bound.push_back(lanelet::Point3d{lanelet::InvalId, 1, 1});

  lanelet::Lanelet lanelet{lanelet::InvalId, left_bound, right_bound};

  lanelet::LineString3d centerline;
  centerline.push_back(lanelet::Point3d{lanelet::InvalId, -1, 0});
  centerline.push_back(lanelet::Point3d{lanelet::InvalId, 0, 0});
  centerline.push_back(lanelet::Point3d{lanelet::InvalId, 1, 0});

  lanelet.setCenterline(centerline);

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::LEFT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, 0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::NONE);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::RIGHT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, -0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(170)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::NONE);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::LEFT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, 0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(30)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::DEVIATING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::RIGHT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, -0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(30)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::MERGING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::LEFT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, 0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(-150)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::DEVIATING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::RIGHT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, -0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(-150)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::MERGING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::LEFT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, 0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(-30)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::MERGING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::RIGHT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, -0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(-30)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::DEVIATING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::LEFT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, 0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(150)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::MERGING);
  }

  {
    ObjectData object_data;
    object_data.overhang_lanelet = lanelet;
    object_data.direction = Direction::RIGHT;
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(0.0, -0.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(150)));

    EXPECT_EQ(
      filtering_utils::getObjectBehavior(object_data, parameters), ObjectData::Behavior::DEVIATING);
  }
}

TEST(TestUtils, isNoNeedAvoidanceBehavior)
{
  const auto parameters = get_parameters();

  const auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
    50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

  const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                             .position(createPoint(0.0, 0.0, 0.0))
                             .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto nearest_path_pose =
    path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
      .point.pose;

  const auto shape = autoware_perception_msgs::build<Shape>()
                       .type(Shape::BOUNDING_BOX)
                       .footprint(geometry_msgs::msg::Polygon{})
                       .dimensions(createVector3(2.8284271247461901, 1.41421356237309505, 2.0));

  auto object_data =
    create_test_object(object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::CAR);

  // object is NOT avoidable. but there is possibility that the ego has to avoid it.
  {
    object_data.avoid_margin = std::nullopt;

    EXPECT_FALSE(filtering_utils::isNoNeedAvoidanceBehavior(object_data, parameters));
  }

  // don't have to avoid.
  {
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(2.5, 3.6, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));
    object_data.direction = Direction::LEFT;
    object_data.avoid_margin = 2.0;
    object_data.envelope_poly = createEnvelopePolygon(object_data, nearest_path_pose, 0.0);
    object_data.overhang_points = calcEnvelopeOverhangDistance(object_data, path);

    EXPECT_TRUE(filtering_utils::isNoNeedAvoidanceBehavior(object_data, parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::ENOUGH_LATERAL_DISTANCE);
  }

  // smaller than execution threshold.
  {
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(2.5, 3.4, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));
    object_data.direction = Direction::LEFT;
    object_data.avoid_margin = 2.0;
    object_data.envelope_poly = createEnvelopePolygon(object_data, nearest_path_pose, 0.0);
    object_data.overhang_points = calcEnvelopeOverhangDistance(object_data, path);

    EXPECT_TRUE(filtering_utils::isNoNeedAvoidanceBehavior(object_data, parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::LESS_THAN_EXECUTION_THRESHOLD);
  }

  // larger than execution threshold.
  {
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(2.5, 2.9, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));
    object_data.direction = Direction::LEFT;
    object_data.avoid_margin = 2.0;
    object_data.envelope_poly = createEnvelopePolygon(object_data, nearest_path_pose, 0.0);
    object_data.overhang_points = calcEnvelopeOverhangDistance(object_data, path);

    EXPECT_FALSE(filtering_utils::isNoNeedAvoidanceBehavior(object_data, parameters));
  }
}

TEST(TestUtils, getAvoidMargin)
{
  const auto parameters = get_parameters();

  const auto planner_data = get_planner_data();

  // wide road
  {
    ObjectData object_data;
    object_data.is_parked = false;
    object_data.distance_factor = 1.0;
    object_data.to_road_shoulder_distance = 5.0;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    const auto output = filtering_utils::getAvoidMargin(object_data, planner_data, parameters);
    ASSERT_TRUE(output.has_value());
    EXPECT_DOUBLE_EQ(output.value(), 1.9);
  }

  // narrow road (relax lateral soft margin)
  {
    ObjectData object_data;
    object_data.is_parked = false;
    object_data.distance_factor = 1.0;
    object_data.to_road_shoulder_distance = 3.0;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    const auto output = filtering_utils::getAvoidMargin(object_data, planner_data, parameters);
    ASSERT_TRUE(output.has_value());
    EXPECT_DOUBLE_EQ(output.value(), 1.5);
  }

  // narrow road (relax drivable bound margin)
  {
    ObjectData object_data;
    object_data.is_parked = false;
    object_data.distance_factor = 1.0;
    object_data.to_road_shoulder_distance = 2.5;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    const auto output = filtering_utils::getAvoidMargin(object_data, planner_data, parameters);
    ASSERT_TRUE(output.has_value());
    EXPECT_DOUBLE_EQ(output.value(), 1.2);
  }

  // road width is not enough.
  {
    ObjectData object_data;
    object_data.is_parked = true;
    object_data.distance_factor = 1.0;
    object_data.to_road_shoulder_distance = 2.5;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    const auto output = filtering_utils::getAvoidMargin(object_data, planner_data, parameters);
    EXPECT_FALSE(output.has_value());
  }
}

TEST(TestUtils, isSafetyCheckTargetObjectType)
{
  const auto parameters = get_parameters();

  {
    ObjectData object_data;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));

    EXPECT_TRUE(filtering_utils::isSafetyCheckTargetObjectType(object_data.object, parameters));
    EXPECT_TRUE(filtering_utils::isVehicleTypeObject(object_data));
    EXPECT_FALSE(filtering_utils::isUnknownTypeObject(object_data));
  }

  {
    ObjectData object_data;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::UNKNOWN)
        .probability(1.0));

    EXPECT_FALSE(filtering_utils::isSafetyCheckTargetObjectType(object_data.object, parameters));
    EXPECT_TRUE(filtering_utils::isVehicleTypeObject(object_data));
    EXPECT_TRUE(filtering_utils::isUnknownTypeObject(object_data));
  }

  {
    ObjectData object_data;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::PEDESTRIAN)
        .probability(1.0));

    EXPECT_FALSE(filtering_utils::isSafetyCheckTargetObjectType(object_data.object, parameters));
    EXPECT_FALSE(filtering_utils::isVehicleTypeObject(object_data));
    EXPECT_FALSE(filtering_utils::isUnknownTypeObject(object_data));
  }
}

TEST(TestUtils, isSatisfiedWithCommonCondition)
{
  constexpr double forward_detection_range = 5.5;

  const auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
    50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

  const auto parameters = get_parameters();

  // no configuration for this object.
  {
    ObjectData object_data;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::PEDESTRIAN)
        .probability(1.0));

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(0.0, 0.0, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::IS_NOT_TARGET_OBJECT);
  }

  // not target object.
  {
    ObjectData object_data;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::UNKNOWN)
        .probability(1.0));

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(0.0, 0.0, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::IS_NOT_TARGET_OBJECT);
  }

  // moving object.
  {
    ObjectData object_data;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.6;

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(0.0, 0.0, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::MOVING_OBJECT);
  }

  // object behind the ego.
  {
    const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                               .position(createPoint(2.5, 1.0, 0.0))
                               .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    ObjectData object_data;
    object_data.object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    object_data.object.shape.dimensions.x = 2.8284271247461901;
    object_data.object.shape.dimensions.y = 1.41421356237309505;
    object_data.object.kinematics.initial_pose_with_covariance.pose = object_pose;
    object_data.direction = Direction::LEFT;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.4;

    constexpr double margin = 0.0;
    const auto pose =
      path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
        .point.pose;

    object_data.envelope_poly = createEnvelopePolygon(object_data, pose, margin);

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(8.0, 0.5, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::FURTHER_THAN_THRESHOLD);
  }

  // farther than detection range.
  {
    const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                               .position(createPoint(7.5, 1.0, 0.0))
                               .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    ObjectData object_data;
    object_data.object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    object_data.object.shape.dimensions.x = 2.8284271247461901;
    object_data.object.shape.dimensions.y = 1.41421356237309505;
    object_data.object.kinematics.initial_pose_with_covariance.pose = object_pose;
    object_data.direction = Direction::LEFT;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.4;

    constexpr double margin = 0.0;
    const auto pose =
      path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
        .point.pose;

    object_data.envelope_poly = createEnvelopePolygon(object_data, pose, margin);

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(0.0, 0.0, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::FURTHER_THAN_THRESHOLD);
  }

  // farther than goal position.
  {
    const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                               .position(createPoint(7.0, 1.0, 0.0))
                               .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    ObjectData object_data;
    object_data.object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    object_data.object.shape.dimensions.x = 2.8284271247461901;
    object_data.object.shape.dimensions.y = 1.41421356237309505;
    object_data.object.kinematics.initial_pose_with_covariance.pose = object_pose;
    object_data.direction = Direction::LEFT;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.4;

    constexpr double margin = 0.0;
    const auto pose =
      path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
        .point.pose;

    object_data.envelope_poly = createEnvelopePolygon(object_data, pose, margin);

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(0.0, 0.0, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::FURTHER_THAN_GOAL);
  }

  // within detection range.
  {
    const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                               .position(createPoint(4.5, 1.0, 0.0))
                               .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    ObjectData object_data;
    object_data.object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    object_data.object.shape.dimensions.x = 2.8284271247461901;
    object_data.object.shape.dimensions.y = 1.41421356237309505;
    object_data.object.kinematics.initial_pose_with_covariance.pose = object_pose;
    object_data.direction = Direction::LEFT;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.4;

    constexpr double margin = 0.0;
    const auto pose =
      path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
        .point.pose;

    object_data.envelope_poly = createEnvelopePolygon(object_data, pose, margin);

    EXPECT_FALSE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 6.4, createPoint(0.0, 0.0, 0.0), false,
      parameters));
    EXPECT_EQ(object_data.info, ObjectInfo::TOO_NEAR_TO_GOAL);
  }

  // within detection range.
  {
    const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                               .position(createPoint(4.5, 1.0, 0.0))
                               .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    ObjectData object_data;
    object_data.object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    object_data.object.shape.dimensions.x = 2.8284271247461901;
    object_data.object.shape.dimensions.y = 1.41421356237309505;
    object_data.object.kinematics.initial_pose_with_covariance.pose = object_pose;
    object_data.direction = Direction::LEFT;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.4;

    constexpr double margin = 0.0;
    const auto pose =
      path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
        .point.pose;

    object_data.envelope_poly = createEnvelopePolygon(object_data, pose, margin);

    EXPECT_TRUE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 6.6, createPoint(0.0, 0.0, 0.0), false,
      parameters));
  }

  // within detection range.
  {
    const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                               .position(createPoint(4.5, 1.0, 0.0))
                               .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    ObjectData object_data;
    object_data.object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    object_data.object.shape.dimensions.x = 2.8284271247461901;
    object_data.object.shape.dimensions.y = 1.41421356237309505;
    object_data.object.kinematics.initial_pose_with_covariance.pose = object_pose;
    object_data.direction = Direction::LEFT;
    object_data.object.classification.emplace_back(
      autoware_perception_msgs::build<ObjectClassification>()
        .label(ObjectClassification::TRUCK)
        .probability(1.0));
    object_data.move_time = 0.4;

    constexpr double margin = 0.0;
    const auto pose =
      path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
        .point.pose;

    object_data.envelope_poly = createEnvelopePolygon(object_data, pose, margin);

    EXPECT_TRUE(filtering_utils::isSatisfiedWithCommonCondition(
      object_data, path, forward_detection_range, 4.0, createPoint(0.0, 0.0, 0.0), true,
      parameters));
  }
}

TEST(TestUtils, isSameDirectionShift)
{
  constexpr double negative_shift_length = -1.0;
  constexpr double positive_shift_length = 1.0;

  {
    ObjectData object;
    object.direction = Direction::RIGHT;

    EXPECT_TRUE(isSameDirectionShift(isOnRight(object), negative_shift_length));
    EXPECT_FALSE(isSameDirectionShift(isOnRight(object), positive_shift_length));
  }

  {
    ObjectData object;
    object.direction = Direction::LEFT;
    EXPECT_TRUE(isSameDirectionShift(isOnRight(object), positive_shift_length));
    EXPECT_FALSE(isSameDirectionShift(isOnRight(object), negative_shift_length));
  }

  {
    ObjectData object;
    object.direction = Direction::NONE;
    EXPECT_THROW(isSameDirectionShift(isOnRight(object), positive_shift_length), std::logic_error);
  }
}

TEST(TestUtils, isShiftNecessary)
{
  constexpr double negative_shift_length = -1.0;
  constexpr double positive_shift_length = 1.0;

  {
    ObjectData object;
    object.direction = Direction::RIGHT;

    EXPECT_TRUE(isShiftNecessary(isOnRight(object), positive_shift_length));
    EXPECT_FALSE(isShiftNecessary(isOnRight(object), negative_shift_length));
  }

  {
    ObjectData object;
    object.direction = Direction::LEFT;
    EXPECT_TRUE(isShiftNecessary(isOnRight(object), negative_shift_length));
    EXPECT_FALSE(isShiftNecessary(isOnRight(object), positive_shift_length));
  }

  {
    ObjectData object;
    object.direction = Direction::NONE;
    EXPECT_THROW(isShiftNecessary(isOnRight(object), positive_shift_length), std::logic_error);
  }
}

TEST(TestUtils, calcShiftLength)
{
  {
    constexpr bool is_on_right = true;
    constexpr double overhang = -1.0;
    constexpr double margin = 1.5;

    const auto output = calcShiftLength(is_on_right, overhang, margin);
    EXPECT_DOUBLE_EQ(output, 0.5);
  }

  {
    constexpr bool is_on_right = false;
    constexpr double overhang = -1.0;
    constexpr double margin = 1.5;

    const auto output = calcShiftLength(is_on_right, overhang, margin);
    EXPECT_DOUBLE_EQ(output, -2.5);
  }
}

TEST(TestUtils, insertDecelPoint)
{
  // invalid target decel point
  {
    auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
      50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

    const auto ego_position = createPoint(2.5, 0.5, 0.0);
    constexpr double offset = 100.0;
    constexpr double velocity = 1.0;

    PoseWithDetailOpt p_out{std::nullopt};
    insertDecelPoint(ego_position, offset, velocity, path, p_out);

    EXPECT_FALSE(p_out.has_value());
    std::for_each(path.points.begin(), path.points.end(), [](const auto & p) {
      EXPECT_DOUBLE_EQ(p.point.longitudinal_velocity_mps, 20.0);
    });
  }

  // nominal case
  {
    auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
      50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

    const auto ego_position = createPoint(3.5, 0.5, 0.0);
    constexpr double offset = 3.0;
    constexpr double velocity = 1.0;

    PoseWithDetailOpt p_out{std::nullopt};
    insertDecelPoint(ego_position, offset, velocity, path, p_out);

    ASSERT_TRUE(p_out.has_value());
    EXPECT_DOUBLE_EQ(p_out.value().pose.position.x, 6.5);
    EXPECT_DOUBLE_EQ(p_out.value().pose.position.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.value().pose.position.z, 0.0);
    for (size_t i = 7; i < path.points.size(); i++) {
      EXPECT_DOUBLE_EQ(path.points.at(i).point.longitudinal_velocity_mps, 1.0);
    }
  }
}

TEST(TestUtils, DISABLED_fillObjectMovingTime)
{
  using namespace std::literals::chrono_literals;

  const auto parameters = get_parameters();

  const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                             .position(createPoint(0.0, 0.0, 0.0))
                             .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto shape = autoware_perception_msgs::build<Shape>()
                       .type(Shape::BOUNDING_BOX)
                       .footprint(geometry_msgs::msg::Polygon{})
                       .dimensions(createVector3(2.0, 1.0, 2.0));

  const auto uuid = generateUUID();

  auto old_object = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);

  // set previous object data.
  {
    old_object.object.object_id = uuid;
    old_object.last_move = rclcpp::Clock{RCL_ROS_TIME}.now();
    old_object.last_stop = rclcpp::Clock{RCL_ROS_TIME}.now();
    old_object.move_time = 0.0;
    old_object.object.kinematics.initial_twist_with_covariance.twist.linear =
      createVector3(0.0, 0.0, 0.0);
  }

  rclcpp::sleep_for(490ms);

  auto new_object = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);

  // find new stop object
  {
    new_object.object.object_id = generateUUID();
    new_object.object.kinematics.initial_twist_with_covariance.twist.linear =
      createVector3(0.99, 0.0, 0.0);

    ObjectDataArray buffer{};
    fillObjectMovingTime(new_object, buffer, parameters);
    EXPECT_NEAR(new_object.stop_time, 0.0, 1e-3);
    EXPECT_NEAR(new_object.move_time, 0.0, 1e-3);
    EXPECT_FALSE(buffer.empty());
  }

  // find new move object
  {
    new_object.object.object_id = generateUUID();
    new_object.object.kinematics.initial_twist_with_covariance.twist.linear =
      createVector3(1.01, 0.0, 0.0);

    ObjectDataArray buffer{};
    fillObjectMovingTime(new_object, buffer, parameters);
    EXPECT_NEAR(new_object.stop_time, 0.0, 1e-3);
    EXPECT_TRUE(buffer.empty());
  }

  // stop to move (moving time < threshold)
  {
    new_object.object.object_id = uuid;
    new_object.object.kinematics.initial_twist_with_covariance.twist.linear =
      createVector3(1.01, 0.0, 0.0);

    ObjectDataArray buffer{old_object};
    fillObjectMovingTime(new_object, buffer, parameters);

    EXPECT_NEAR(new_object.stop_time, 0.0, 1e-3);
    EXPECT_NEAR(new_object.move_time, 0.49, 1e-3);
    EXPECT_FALSE(buffer.empty());
  }

  // stop to stop
  {
    new_object.object.object_id = uuid;
    new_object.object.kinematics.initial_twist_with_covariance.twist.linear =
      createVector3(0.99, 0.0, 0.0);

    ObjectDataArray buffer{old_object};
    fillObjectMovingTime(new_object, buffer, parameters);

    EXPECT_NEAR(new_object.stop_time, 0.49, 1e-3);
    EXPECT_NEAR(new_object.move_time, 0.0, 1e-3);
    EXPECT_FALSE(buffer.empty());
  }

  rclcpp::sleep_for(20ms);

  // stop to move (threshold < moving time)
  {
    new_object.object.object_id = uuid;
    new_object.object.kinematics.initial_twist_with_covariance.twist.linear =
      createVector3(1.01, 0.0, 0.0);

    ObjectDataArray buffer{old_object};
    fillObjectMovingTime(new_object, buffer, parameters);

    EXPECT_NEAR(new_object.stop_time, 0.0, 1e-3);
    EXPECT_NEAR(new_object.move_time, 0.51, 1e-3);
    EXPECT_TRUE(buffer.empty());
  }
}

TEST(TestUtils, calcEnvelopeOverhangDistance)
{
  const auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
    50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

  const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                             .position(createPoint(0.0, 0.0, 0.0))
                             .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto nearest_path_pose =
    path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
      .point.pose;

  const auto shape = autoware_perception_msgs::build<Shape>()
                       .type(Shape::BOUNDING_BOX)
                       .footprint(geometry_msgs::msg::Polygon{})
                       .dimensions(createVector3(2.8284271247461901, 1.41421356237309505, 2.0));

  auto object_data = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);

  {
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(2.5, 1.0, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));
    object_data.direction = Direction::LEFT;
    object_data.envelope_poly = createEnvelopePolygon(object_data, nearest_path_pose, 0.0);

    const auto output = calcEnvelopeOverhangDistance(object_data, path);

    ASSERT_EQ(output.size(), 5);
    EXPECT_NEAR(output.at(0).first, -0.5, epsilon);
    EXPECT_NEAR(output.at(1).first, -0.5, epsilon);
    EXPECT_NEAR(output.at(2).first, -0.5, epsilon);
    EXPECT_NEAR(output.at(3).first, 2.5, epsilon);
    EXPECT_NEAR(output.at(4).first, 2.5, epsilon);
  }

  {
    object_data.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(2.5, -1.0, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));
    object_data.direction = Direction::RIGHT;
    object_data.envelope_poly = createEnvelopePolygon(object_data, nearest_path_pose, 0.0);

    const auto output = calcEnvelopeOverhangDistance(object_data, path);

    ASSERT_EQ(output.size(), 5);
    EXPECT_NEAR(output.at(0).first, 0.5, epsilon);
    EXPECT_NEAR(output.at(1).first, 0.5, epsilon);
    EXPECT_NEAR(output.at(2).first, -2.5, epsilon);
    EXPECT_NEAR(output.at(3).first, -2.5, epsilon);
    EXPECT_NEAR(output.at(4).first, -2.5, epsilon);
  }
}

TEST(TestUtils, createEnvelopePolygon)
{
  Polygon2d footprint;
  footprint.outer() = {
    Point2d{1.0, 0.0}, Point2d{3.0, 0.0}, Point2d{3.0, 1.0}, Point2d{1.0, 1.0}, Point2d{1.0, 0.0}};

  constexpr double margin = 0.353553390593273762;
  const auto pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                      .position(createPoint(0.0, 0.0, 0.0))
                      .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto output = createEnvelopePolygon(footprint, pose, margin);

  ASSERT_EQ(output.outer().size(), 5);
  EXPECT_NEAR(output.outer().at(0).x(), 2.0, epsilon);
  EXPECT_NEAR(output.outer().at(0).y(), -1.5, epsilon);
  EXPECT_NEAR(output.outer().at(1).x(), 0.0, epsilon);
  EXPECT_NEAR(output.outer().at(1).y(), 0.5, epsilon);
  EXPECT_NEAR(output.outer().at(2).x(), 2.0, epsilon);
  EXPECT_NEAR(output.outer().at(2).y(), 2.5, epsilon);
  EXPECT_NEAR(output.outer().at(3).x(), 4.0, epsilon);
  EXPECT_NEAR(output.outer().at(3).y(), 0.5, epsilon);
  EXPECT_NEAR(output.outer().at(4).x(), 2.0, epsilon);
  EXPECT_NEAR(output.outer().at(4).y(), -1.5, epsilon);
}

TEST(TestUtils, generateObstaclePolygonsForDrivableArea)
{
  const auto create_params = [](
                               const double envelope_buffer, const double hard, const double soft) {
    ObjectParameter param{};
    param.envelope_buffer_margin = envelope_buffer;
    param.lateral_soft_margin = soft;
    param.lateral_hard_margin = hard;
    return param;
  };

  constexpr double vehicle_width = 1.0;
  constexpr double envelope_buffer = 0.353553390593273762;
  constexpr double lateral_soft_margin = 0.707106781186547524;

  const auto parameters = std::make_shared<AvoidanceParameters>();
  parameters->object_parameters.emplace(
    ObjectClassification::TRUCK, create_params(envelope_buffer, 0.5, lateral_soft_margin));

  const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                             .position(createPoint(0.0, 0.0, 0.0))
                             .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto shape = autoware_perception_msgs::build<Shape>()
                       .type(Shape::BOUNDING_BOX)
                       .footprint(geometry_msgs::msg::Polygon{})
                       .dimensions(createVector3(2.0, 1.0, 2.0));

  auto object_data = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);

  // empty
  {
    ObjectDataArray objects{};
    const auto output = generateObstaclePolygonsForDrivableArea(objects, parameters, vehicle_width);
    EXPECT_TRUE(output.empty());
  }

  // invalid margin
  {
    const auto object_type = utils::getHighestProbLabel(object_data.object.classification);
    const auto object_parameter = parameters->object_parameters.at(object_type);
    object_data.avoid_margin = std::nullopt;

    Polygon2d footprint;
    footprint.outer() = {
      Point2d{1.0, 0.0}, Point2d{3.0, 0.0}, Point2d{3.0, 1.0}, Point2d{1.0, 1.0},
      Point2d{1.0, 0.0}};

    object_data.envelope_poly =
      createEnvelopePolygon(footprint, object_pose, object_parameter.envelope_buffer_margin);

    ObjectDataArray objects{object_data};
    const auto output = generateObstaclePolygonsForDrivableArea(objects, parameters, vehicle_width);
    EXPECT_TRUE(output.empty());
  }

  // invalid envelope polygon
  {
    const auto object_type = utils::getHighestProbLabel(object_data.object.classification);
    const auto object_parameter = parameters->object_parameters.at(object_type);
    object_data.avoid_margin = object_parameter.lateral_soft_margin + 0.5 * vehicle_width;
    object_data.envelope_poly = {};

    ObjectDataArray objects{object_data};
    const auto output = generateObstaclePolygonsForDrivableArea(objects, parameters, vehicle_width);
    EXPECT_TRUE(output.empty());
  }

  // nominal case
  {
    object_data.direction = Direction::RIGHT;
    const auto object_type = utils::getHighestProbLabel(object_data.object.classification);
    const auto object_parameter = parameters->object_parameters.at(object_type);
    object_data.avoid_margin = object_parameter.lateral_soft_margin + 0.5 * vehicle_width;

    Polygon2d footprint;
    footprint.outer() = {
      Point2d{1.0, 0.0}, Point2d{3.0, 0.0}, Point2d{3.0, 1.0}, Point2d{1.0, 1.0},
      Point2d{1.0, 0.0}};

    object_data.envelope_poly =
      createEnvelopePolygon(footprint, object_pose, object_parameter.envelope_buffer_margin);

    ObjectDataArray objects{object_data};
    const auto output = generateObstaclePolygonsForDrivableArea(objects, parameters, vehicle_width);
    EXPECT_FALSE(output.empty());

    ASSERT_EQ(output.front().poly.outer().size(), 5);
    EXPECT_NEAR(output.front().poly.outer().at(0).x(), 2.0, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(0).y(), -2.0, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(1).x(), -0.5, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(1).y(), 0.5, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(2).x(), 2.0, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(2).y(), 3.0, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(3).x(), 4.5, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(3).y(), 0.5, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(4).x(), 2.0, epsilon);
    EXPECT_NEAR(output.front().poly.outer().at(4).y(), -2.0, epsilon);
  }
}

TEST(TestUtils, fillLongitudinalAndLengthByClosestEnvelopeFootprint)
{
  const auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
    50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

  const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                             .position(createPoint(2.5, 1.0, 0.0))
                             .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto nearest_path_pose =
    path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
      .point.pose;

  const auto shape = autoware_perception_msgs::build<Shape>()
                       .type(Shape::BOUNDING_BOX)
                       .footprint(geometry_msgs::msg::Polygon{})
                       .dimensions(createVector3(2.8284271247461901, 1.41421356237309505, 2.0));

  auto object_data = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);
  object_data.direction = Direction::LEFT;
  object_data.envelope_poly = createEnvelopePolygon(object_data, nearest_path_pose, 0.0);

  fillLongitudinalAndLengthByClosestEnvelopeFootprint(
    path, createPoint(0.0, 0.0, 0.0), object_data);
  EXPECT_NEAR(object_data.longitudinal, 1.0, epsilon);
  EXPECT_NEAR(object_data.length, 3.0, epsilon);
}

TEST(TestUtils, fillObjectEnvelopePolygon)
{
  const auto path = autoware::test_utils::generateTrajectory<PathWithLaneId>(
    50L, 1.0, 20.0, 0.0, 0.0, std::numeric_limits<size_t>::max());

  const auto parameters = get_parameters();

  const auto uuid = generateUUID();

  const auto object_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                             .position(createPoint(2.5, 1.0, 0.0))
                             .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

  const auto nearest_path_pose =
    path.points.at(autoware::motion_utils::findNearestIndex(path.points, object_pose.position))
      .point.pose;

  const auto shape = autoware_perception_msgs::build<Shape>()
                       .type(Shape::BOUNDING_BOX)
                       .footprint(geometry_msgs::msg::Polygon{})
                       .dimensions(createVector3(2.8284271247461901, 1.41421356237309505, 2.0));

  auto stored_object = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);
  stored_object.object.object_id = uuid;
  stored_object.envelope_poly = createEnvelopePolygon(stored_object, nearest_path_pose, 0.0);
  stored_object.object.kinematics.initial_pose_with_covariance =
    geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>()
      .pose(object_pose)
      .covariance({1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                   0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                   0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
  stored_object.error_eclipse_max =
    calcErrorEclipseLongRadius(stored_object.object.kinematics.initial_pose_with_covariance);
  stored_object.direction = Direction::LEFT;

  auto object_data = create_test_object(
    object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);

  // new object.
  {
    ObjectDataArray stored_objects{};

    object_data.object.object_id = generateUUID();
    object_data.object.kinematics.initial_pose_with_covariance =
      geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>()
        .pose(object_pose)
        .covariance({1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                     0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    object_data.direction = Direction::LEFT;
    fillObjectEnvelopePolygon(object_data, stored_objects, nearest_path_pose, parameters);

    ASSERT_EQ(object_data.envelope_poly.outer().size(), 5);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).y(), -0.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).x(), 4.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).x(), 4.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).y(), -0.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).y(), -0.5, epsilon);
  }

  // update envelope polygon by new pose.
  {
    ObjectDataArray stored_objects{stored_object};

    const auto new_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                            .position(createPoint(3.0, 0.5, 0.0))
                            .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    object_data.object.object_id = uuid;
    object_data.object.kinematics.initial_pose_with_covariance =
      geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>().pose(new_pose).covariance(
        {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    object_data.direction = Direction::LEFT;
    fillObjectEnvelopePolygon(object_data, stored_objects, nearest_path_pose, parameters);

    ASSERT_EQ(object_data.envelope_poly.outer().size(), 5);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).y(), -1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).x(), 4.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).x(), 4.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).y(), -1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).y(), -1.0, epsilon);
  }

  // use previous envelope polygon because new pose's error eclipse long radius is larger than
  // threshold. error eclipse long radius: 2.1213203435596
  {
    ObjectDataArray stored_objects{stored_object};

    const auto new_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                            .position(createPoint(3.0, 0.5, 0.0))
                            .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    object_data.object.object_id = uuid;
    object_data.object.kinematics.initial_pose_with_covariance =
      geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>().pose(new_pose).covariance(
        {2.5, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    object_data.direction = Direction::LEFT;
    fillObjectEnvelopePolygon(object_data, stored_objects, nearest_path_pose, parameters);

    ASSERT_EQ(object_data.envelope_poly.outer().size(), 5);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).y(), -0.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).x(), 4.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).x(), 4.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).y(), -0.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).y(), -0.5, epsilon);
  }

  // use new envelope polygon because new pose's error eclipse long radius is smaller than
  // threshold.
  {
    auto huge_covariance_object = create_test_object(
      object_pose, createVector3(0.0, 0.0, 0.0), shape, ObjectClassification::TRUCK);
    huge_covariance_object.object.object_id = uuid;
    huge_covariance_object.object.kinematics.initial_pose_with_covariance =
      geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>()
        .pose(object_pose)
        .covariance({5.0, 4.0, 0.0, 0.0, 0.0, 0.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0,
                     0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    huge_covariance_object.error_eclipse_max = calcErrorEclipseLongRadius(
      huge_covariance_object.object.kinematics.initial_pose_with_covariance);
    huge_covariance_object.direction = Direction::LEFT;

    huge_covariance_object.envelope_poly =
      createEnvelopePolygon(huge_covariance_object, nearest_path_pose, 0.0);

    ObjectDataArray stored_objects{huge_covariance_object};

    const auto new_pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                            .position(createPoint(3.0, 0.5, 0.0))
                            .orientation(createQuaternionFromRPY(0.0, 0.0, deg2rad(45)));

    object_data.object.object_id = uuid;
    object_data.object.kinematics.initial_pose_with_covariance =
      geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>().pose(new_pose).covariance(
        {2.5, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    object_data.direction = Direction::LEFT;
    fillObjectEnvelopePolygon(object_data, stored_objects, nearest_path_pose, parameters);

    ASSERT_EQ(object_data.envelope_poly.outer().size(), 5);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).x(), 1.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).y(), -1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).x(), 1.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).y(), 2.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).x(), 4.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).y(), 2.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).x(), 4.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).y(), -1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).x(), 1.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).y(), -1.0, epsilon);
  }

  // use previous envelope polygon because the new one is within old one.
  {
    ObjectDataArray stored_objects{stored_object};

    object_data.object.object_id = uuid;
    object_data.object.kinematics.initial_pose_with_covariance =
      geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>()
        .pose(object_pose)
        .covariance({1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                     0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                     0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    object_data.direction = Direction::LEFT;
    fillObjectEnvelopePolygon(object_data, stored_objects, nearest_path_pose, parameters);

    ASSERT_EQ(object_data.envelope_poly.outer().size(), 5);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(0).y(), -0.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(1).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).x(), 4.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(2).y(), 2.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).x(), 4.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(3).y(), -0.5, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).x(), 1.0, epsilon);
    EXPECT_NEAR(object_data.envelope_poly.outer().at(4).y(), -0.5, epsilon);
  }
}

TEST(TestUtils, compensateLostTargetObjects)
{
  using namespace std::literals::chrono_literals;

  const auto parameters = get_parameters();

  const auto planner_data = get_planner_data();

  const auto uuid = generateUUID();

  const auto init_time = rclcpp::Clock{RCL_ROS_TIME}.now();

  ObjectData stored_object;
  stored_object.object.object_id = uuid;
  stored_object.last_seen = init_time;
  stored_object.object.kinematics.initial_pose_with_covariance.pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(createPoint(1.0, 1.0, 0.0))
      .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));

  rclcpp::sleep_for(100ms);

  // add stored objects.
  {
    const auto now = rclcpp::Clock{RCL_ROS_TIME}.now();

    ObjectDataArray stored_objects{};

    ObjectData new_object;
    new_object.object.object_id = generateUUID();
    new_object.last_seen = now;
    new_object.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(2.0, 5.0, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));

    AvoidancePlanningData avoidance_planning_data;
    avoidance_planning_data.target_objects = {new_object};
    avoidance_planning_data.other_objects = {};

    compensateLostTargetObjects(
      stored_objects, avoidance_planning_data, now, planner_data, parameters);
    ASSERT_FALSE(stored_objects.empty());
    EXPECT_EQ(stored_objects.front().object.object_id, new_object.object.object_id);
  }

  // compensate detection lost.
  {
    const auto now = rclcpp::Clock{RCL_ROS_TIME}.now();

    ObjectDataArray stored_objects{stored_object};

    AvoidancePlanningData avoidance_planning_data;
    avoidance_planning_data.target_objects = {};
    avoidance_planning_data.other_objects = {};

    compensateLostTargetObjects(
      stored_objects, avoidance_planning_data, now, planner_data, parameters);
    ASSERT_FALSE(avoidance_planning_data.target_objects.empty());
    EXPECT_EQ(
      avoidance_planning_data.target_objects.front().object.object_id,
      stored_object.object.object_id);
  }

  // update stored objects (same uuid).
  {
    const auto now = rclcpp::Clock{RCL_ROS_TIME}.now();

    ObjectDataArray stored_objects{stored_object};

    ObjectData detected_object;
    detected_object.object.object_id = uuid;
    detected_object.last_seen = now;
    detected_object.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(1.0, 1.0, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));

    AvoidancePlanningData avoidance_planning_data;
    avoidance_planning_data.target_objects = {detected_object};
    avoidance_planning_data.other_objects = {};

    compensateLostTargetObjects(
      stored_objects, avoidance_planning_data, now, planner_data, parameters);
    ASSERT_FALSE(stored_objects.empty());
    EXPECT_EQ(stored_objects.front().last_seen, detected_object.last_seen);
  }

  // update stored objects (detected near the stored object).
  {
    const auto now = rclcpp::Clock{RCL_ROS_TIME}.now();

    ObjectDataArray stored_objects{stored_object};

    ObjectData detected_object;
    detected_object.object.object_id = generateUUID();
    detected_object.last_seen = now;
    detected_object.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(1.1, 1.1, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));

    AvoidancePlanningData avoidance_planning_data;
    avoidance_planning_data.target_objects = {detected_object};
    avoidance_planning_data.other_objects = {};

    compensateLostTargetObjects(
      stored_objects, avoidance_planning_data, now, planner_data, parameters);
    ASSERT_FALSE(stored_objects.empty());
    EXPECT_EQ(stored_objects.front().last_seen, detected_object.last_seen);
  }

  // don't update stored object because there is no matched object.
  {
    const auto now = rclcpp::Clock{RCL_ROS_TIME}.now();

    ObjectDataArray stored_objects{stored_object};

    ObjectData detected_object;
    detected_object.object.object_id = generateUUID();
    detected_object.last_seen = now;
    detected_object.object.kinematics.initial_pose_with_covariance.pose =
      geometry_msgs::build<geometry_msgs::msg::Pose>()
        .position(createPoint(3.0, 3.0, 0.0))
        .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));

    AvoidancePlanningData avoidance_planning_data;
    avoidance_planning_data.target_objects = {detected_object};
    avoidance_planning_data.other_objects = {};

    compensateLostTargetObjects(
      stored_objects, avoidance_planning_data, now, planner_data, parameters);
    ASSERT_FALSE(stored_objects.empty());
    EXPECT_EQ(stored_objects.front().last_seen, init_time);
  }

  rclcpp::sleep_for(200ms);

  // don't compensate detection lost because time elapses more than threshold.
  {
    const auto now = rclcpp::Clock{RCL_ROS_TIME}.now();

    ObjectDataArray stored_objects{stored_object};

    AvoidancePlanningData avoidance_planning_data;
    avoidance_planning_data.target_objects = {};
    avoidance_planning_data.other_objects = {};

    compensateLostTargetObjects(
      stored_objects, avoidance_planning_data, now, planner_data, parameters);
    EXPECT_TRUE(avoidance_planning_data.target_objects.empty());
  }
}

TEST(TestUtils, calcErrorEclipseLongRadius)
{
  const auto pose = geometry_msgs::build<geometry_msgs::msg::Pose>()
                      .position(createPoint(3.0, 3.0, 0.0))
                      .orientation(createQuaternionFromRPY(0.0, 0.0, 0.0));
  const auto pose_with_covariance =
    geometry_msgs::build<geometry_msgs::msg::PoseWithCovariance>().pose(pose).covariance(
      {5.0, 4.0, 0.0, 0.0, 0.0, 0.0, 4.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
       0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0});

  EXPECT_DOUBLE_EQ(calcErrorEclipseLongRadius(pose_with_covariance), 3.0);
}
}  // namespace autoware::behavior_path_planner::utils::static_obstacle_avoidance

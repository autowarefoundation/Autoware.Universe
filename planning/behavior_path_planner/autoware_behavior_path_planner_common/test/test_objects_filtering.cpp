// Copyright 2024 Tier IV, Inc. All rights reserved.
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

#include "autoware/behavior_path_planner_common/utils/path_safety_checker/objects_filtering.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/path_safety_checker_parameters.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <geometry_msgs/msg/detail/pose__struct.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

using PredictedObject = autoware_perception_msgs::msg::PredictedObject;
using PredictedObjects = autoware_perception_msgs::msg::PredictedObjects;
using ObjectClassification = autoware_perception_msgs::msg::ObjectClassification;
using PoseWithCovariance = geometry_msgs::msg::PoseWithCovariance;
using TwistWithCovariance = geometry_msgs::msg::TwistWithCovariance;
using autoware_perception_msgs::msg::PredictedPath;
using autoware_planning_msgs::msg::Trajectory;
using tier4_planning_msgs::msg::PathPointWithLaneId;

using autoware::test_utils::createPose;
using autoware::test_utils::generateTrajectory;
using autoware::test_utils::make_lanelet;
using autoware::universe_utils::createPoint;
using autoware::universe_utils::createVector3;

constexpr double epsilon = 1e-6;

const auto intersection_map =
  autoware::test_utils::make_map_bin_msg(autoware::test_utils::get_absolute_path_to_lanelet_map(
    "autoware_test_utils", "intersection/lanelet2_map.osm"));

PredictedObject create_bounding_box_object(
  const geometry_msgs::msg::Pose pose,
  const geometry_msgs::msg::Vector3 velocity = geometry_msgs::msg::Vector3(),
  const double x_dimension = 1.0, const double y_dimension = 1.0,
  const std::vector<ObjectClassification> & classification = std::vector<ObjectClassification>())
{
  PredictedObject object;
  object.object_id = autoware::universe_utils::generateUUID();
  object.kinematics.initial_pose_with_covariance.pose = pose;
  object.kinematics.initial_twist_with_covariance.twist.linear = velocity;
  object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  object.shape.dimensions.x = x_dimension;
  object.shape.dimensions.y = y_dimension;
  object.classification = classification;

  return object;
}

std::vector<PathPointWithLaneId> trajectory_to_path_with_lane_id(const Trajectory & trajectory)
{
  std::vector<PathPointWithLaneId> path_with_lane_id;
  PathPointWithLaneId path_point_with_lane_id;
  for (const auto & point : trajectory.points) {
    path_point_with_lane_id.point.pose = point.pose;
    path_point_with_lane_id.point.lateral_velocity_mps = point.lateral_velocity_mps;
    path_point_with_lane_id.point.longitudinal_velocity_mps = point.longitudinal_velocity_mps;
    path_point_with_lane_id.point.heading_rate_rps = point.heading_rate_rps;
    path_with_lane_id.push_back(path_point_with_lane_id);
  }
  return path_with_lane_id;
}

PredictedPath trajectory_to_predicted_path(const Trajectory & trajectory)
{
  PredictedPath path;
  geometry_msgs::msg::Pose pose;
  for (const auto & point : trajectory.points) {
    pose = point.pose;
    path.path.push_back(pose);
  }
  return path;
}

TEST(BehaviorPathPlanningObjectsFiltering, velocity_filter)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filter::velocity_filter;

  PredictedObject predicted_obj;
  auto & twist = predicted_obj.kinematics.initial_twist_with_covariance.twist;
  twist.linear.x = 4.0;
  twist.linear.y = 3.0;

  EXPECT_TRUE(velocity_filter(twist, 4.0, 10.0));
  EXPECT_FALSE(velocity_filter(twist, 6.0, 10.0));
  EXPECT_FALSE(velocity_filter(twist, 2.0, 4.9));
  EXPECT_FALSE(velocity_filter(twist, 6.0, 2.0));
}

TEST(BehaviorPathPlanningObjectsFiltering, position_filter)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filter::position_filter;

  auto current_pos = createPoint(0.0, 0.0, 0.0);
  PredictedObject object = create_bounding_box_object(createPose(10.0, 0.0, 0.0, 0.0, 0.0, 0.0));
  auto straight_trajectory = generateTrajectory<Trajectory>(20, 1.0);
  double forward_distance = 20.0;
  double backward_distance = 1.0;

  std::vector<PathPointWithLaneId> empty_path;
  auto straight_path = trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(20, 1.0));
  auto curved_path =
    trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(20, 1.0, 0.0, 0.0, 0.01));

  EXPECT_TRUE(
    position_filter(object, straight_path, current_pos, forward_distance, -backward_distance));
  EXPECT_FALSE(
    position_filter(object, empty_path, current_pos, forward_distance, -backward_distance));
  EXPECT_TRUE(
    position_filter(object, curved_path, current_pos, forward_distance, -backward_distance));

  forward_distance = 2.0;
  EXPECT_FALSE(
    position_filter(object, straight_path, current_pos, forward_distance, -backward_distance));
  EXPECT_FALSE(
    position_filter(object, curved_path, current_pos, forward_distance, -backward_distance));
}

TEST(BehaviorPathPlanningObjectsFiltering, is_within_circle)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filter::is_within_circle;

  auto object_pos = createPoint(0.0, 0.0, 0.0);
  auto ref_point = createPoint(0.0, 0.0, 0.0);
  double search_radius = 1.0;

  EXPECT_TRUE(is_within_circle(object_pos, ref_point, search_radius));

  object_pos.x = 2.0;
  object_pos.x = 2.0;

  EXPECT_FALSE(is_within_circle(object_pos, ref_point, search_radius));

  ref_point.x = 2.5;
  ref_point.y = 2.4;
  object_pos.x = 3.0;
  object_pos.y = 3.0;

  EXPECT_TRUE(is_within_circle(object_pos, ref_point, search_radius));
}

TEST(BehaviorPathPlanningObjectsFiltering, isCentroidWithinLanelet)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::isCentroidWithinLanelet;
  using autoware::behavior_path_planner::utils::path_safety_checker::isCentroidWithinLanelets;

  auto object = create_bounding_box_object(createPose(0.5, 0.0, 0.0, 0.0, 0.0, 0.0));
  auto lanelet = make_lanelet({0.0, 1.0}, {5.0, 1.0}, {0.0, -1.0}, {5.0, -1.0});
  double yaw_threshold = M_PI_2;

  EXPECT_TRUE(isCentroidWithinLanelet(object, lanelet, yaw_threshold));

  object.kinematics.initial_pose_with_covariance.pose.position.x = 8.0;
  EXPECT_FALSE(isCentroidWithinLanelet(object, lanelet, yaw_threshold));

  lanelet::ConstLanelets target_lanelets;
  target_lanelets.push_back(lanelet);
  target_lanelets.push_back(make_lanelet({5.0, 1.0}, {10.0, 1.0}, {5.0, -1.0}, {10.0, -1.0}));
  EXPECT_TRUE(isCentroidWithinLanelets(object, target_lanelets));
}

TEST(BehaviorPathPlanningObjectsFiltering, isPolygonOverlapLanelet)
{
  using autoware::behavior_path_planner::utils::toPolygon2d;
  using autoware::behavior_path_planner::utils::path_safety_checker::isPolygonOverlapLanelet;

  PredictedObject object = create_bounding_box_object(createPose(0.5, 0.0, 0.0, 0.0, 0.0, 0.0));

  auto lanelet = make_lanelet({0.0, 1.0}, {5.0, 1.0}, {0.0, -1.0}, {5.0, -1.0});
  double yaw_threshold = M_PI_2;

  EXPECT_TRUE(isPolygonOverlapLanelet(object, lanelet.polygon2d().basicPolygon()));
  EXPECT_TRUE(isPolygonOverlapLanelet(object, toPolygon2d(lanelet)));
  EXPECT_TRUE(isPolygonOverlapLanelet(object, lanelet, yaw_threshold));

  object.kinematics.initial_pose_with_covariance.pose = createPose(10.0, 10.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_FALSE(isPolygonOverlapLanelet(object, lanelet.polygon2d().basicPolygon()));
  EXPECT_FALSE(isPolygonOverlapLanelet(object, toPolygon2d(lanelet)));
  EXPECT_FALSE(isPolygonOverlapLanelet(object, lanelet, yaw_threshold));
}

TEST(BehaviorPathPlanningObjectsFiltering, filterObjects)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filterObjects;
  using autoware::behavior_path_planner::utils::path_safety_checker::ObjectsFilteringParams;
  using autoware::universe_utils::createVector3;

  std::shared_ptr<PredictedObjects> objects = std::make_shared<PredictedObjects>();
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler =
    std::make_shared<autoware::route_handler::RouteHandler>();
  std::shared_ptr<ObjectsFilteringParams> params = std::make_shared<ObjectsFilteringParams>();
  params->ignore_object_velocity_threshold = false;
  params->object_check_forward_distance = 20.0;
  params->object_check_backward_distance = 10.0;
  params->object_types_to_check.check_car = true;
  route_handler->setMap(intersection_map);
  lanelet::ConstLanelets current_lanes;

  current_lanes.push_back(route_handler->getLaneletsFromId(1000));
  current_lanes.push_back(route_handler->getLaneletsFromId(1010));
  auto current_pose = createPoint(360.22, 600.51, 0.0);

  EXPECT_TRUE(
    filterObjects(objects, route_handler, current_lanes, current_pose, params).objects.empty());

  ObjectClassification classification;
  classification.label = ObjectClassification::Type::CAR;
  classification.probability = 1.0;

  auto target_object = create_bounding_box_object(
    createPose(360.22, 605.51, 0.0, 0.0, 0.0, 0.0), createVector3(1.0, 1.0, 0.0));
  target_object.classification.push_back(classification);
  auto other_object = create_bounding_box_object(
    createPose(370.22, 600.51, 0.0, 0.0, 0.0, 0.0), createVector3(1.0, 1.0, 0.0));
  other_object.classification.push_back(classification);

  objects->objects.push_back(target_object);
  objects->objects.push_back(other_object);

  auto filtered_object = filterObjects(objects, route_handler, current_lanes, current_pose, params);
  EXPECT_FALSE(filtered_object.objects.empty());
  EXPECT_EQ(filtered_object.objects.front().object_id, target_object.object_id);
}

TEST(BehaviorPathPlanningObjectsFiltering, filterObjectsByVelocity)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filterObjectsByVelocity;

  PredictedObjects objects;
  auto slow_obj = create_bounding_box_object(
    createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), createVector3(2.0, 0.0, 0.0));
  auto fast_obj = create_bounding_box_object(
    createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0), createVector3(10.0, 0.0, 0.0));
  double vel_thr = 5.0;

  objects.objects.push_back(slow_obj);
  objects.objects.push_back(fast_obj);

  auto filtered_obj = filterObjectsByVelocity(objects, vel_thr, false);
  ASSERT_FALSE(filtered_obj.objects.empty());
  EXPECT_EQ(filtered_obj.objects.front().object_id, slow_obj.object_id);

  filtered_obj = filterObjectsByVelocity(objects, vel_thr, true);
  ASSERT_FALSE(filtered_obj.objects.empty());
  EXPECT_EQ(filtered_obj.objects.front().object_id, fast_obj.object_id);

  vel_thr = 0.0;
  filtered_obj = filterObjectsByVelocity(objects, vel_thr, false);
  EXPECT_TRUE(filtered_obj.objects.empty());

  filtered_obj = filterObjectsByVelocity(objects, vel_thr, true);
  EXPECT_EQ(filtered_obj.objects.size(), 2);
}

TEST(BehaviorPathPlanningObjectsFiltering, filterObjectsByPosition)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filterObjectsByPosition;
  using autoware::behavior_path_planner::utils::path_safety_checker::filterObjectsWithinRadius;

  auto current_pos = createPoint(0.0, 0.0, 0.0);
  auto straight_path = trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(20, 1.0));
  double forward_distance = 10.0;
  double backward_distance = 1.0;
  double search_radius = 10.0;

  PredictedObjects objects;
  auto far_obj = create_bounding_box_object(createPose(50.0, 0.0, 0.0, 0.0, 0.0, 0.0));
  auto near_obj = create_bounding_box_object(createPose(5.0, 0.0, 0.0, 0.0, 0.0, 0.0));

  objects.objects.push_back(near_obj);
  objects.objects.push_back(far_obj);
  auto target_uuid = near_obj.object_id;

  filterObjectsByPosition(objects, straight_path, current_pos, forward_distance, backward_distance);

  ASSERT_FALSE(objects.objects.empty());
  EXPECT_EQ(objects.objects.front().object_id, target_uuid);

  objects.objects.clear();
  objects.objects.push_back(far_obj);
  objects.objects.push_back(near_obj);

  filterObjectsWithinRadius(objects, current_pos, search_radius);
  ASSERT_FALSE(objects.objects.empty());
  EXPECT_EQ(objects.objects.front().object_id, target_uuid);
}

TEST(BehaviorPathPlanningObjectsFiltering, separateObjectsByLanelets)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::isPolygonOverlapLanelet;
  using autoware::behavior_path_planner::utils::path_safety_checker::
    separateObjectIndicesByLanelets;
  using autoware::behavior_path_planner::utils::path_safety_checker::separateObjectsByLanelets;

  double yaw_threshold = M_PI_2;

  auto target_object = create_bounding_box_object(createPose(0.5, 0.0, 0.0, 0.0, 0.0, 0.0));
  auto other_object = create_bounding_box_object(createPose(-1.5, 0.0, 0.0, 0.0, 0.0, 0.0));

  PredictedObjects objects;
  objects.objects.push_back(target_object);
  objects.objects.push_back(other_object);

  lanelet::ConstLanelets target_lanelets;
  {
    auto object_indices = separateObjectIndicesByLanelets(
      objects, target_lanelets,
      [](const auto & obj, const auto & lane, const auto & yaw_threshold) {
        return isPolygonOverlapLanelet(obj, lane, yaw_threshold);
      },
      yaw_threshold);
    EXPECT_TRUE(object_indices.first.empty());
    EXPECT_TRUE(object_indices.second.empty());
  }
  {
    target_lanelets.push_back(make_lanelet({0.0, 1.0}, {5.0, 1.0}, {0.0, -1.0}, {5.0, -1.0}));
    target_lanelets.push_back(make_lanelet({5.0, 1.0}, {10.0, 1.0}, {5.0, -1.0}, {10.0, -1.0}));
    auto object_indices = separateObjectIndicesByLanelets(
      objects, target_lanelets,
      [](const auto & obj, const auto & lane, const auto & yaw_threshold) {
        return isPolygonOverlapLanelet(obj, lane, yaw_threshold);
      },
      yaw_threshold);
    EXPECT_FALSE(object_indices.first.empty());
    EXPECT_FALSE(object_indices.second.empty());
    EXPECT_EQ(object_indices.first.front(), 0);
    EXPECT_EQ(object_indices.second.front(), 1);

    auto filtered_object = separateObjectsByLanelets(
      objects, target_lanelets,
      [](const auto & obj, const auto & lane, const auto & yaw_threshold) {
        return isPolygonOverlapLanelet(obj, lane, yaw_threshold);
      },
      yaw_threshold);
    EXPECT_FALSE(filtered_object.first.objects.empty());
    EXPECT_FALSE(filtered_object.second.objects.empty());
    EXPECT_EQ(filtered_object.first.objects.front().object_id, target_object.object_id);
    EXPECT_EQ(filtered_object.second.objects.front().object_id, other_object.object_id);
  }
}

TEST(BehaviorPathPlanningObjectsFiltering, getPredictedPathFromObj)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::getPredictedPathFromObj;
  using autoware::behavior_path_planner::utils::path_safety_checker::
    PoseWithVelocityAndPolygonStamped;
  using autoware::behavior_path_planner::utils::path_safety_checker::PredictedPathWithPolygon;

  autoware::behavior_path_planner::utils::path_safety_checker::ExtendedPredictedObject object;
  std::vector<PredictedPathWithPolygon> predicted_paths;
  PredictedPathWithPolygon predicted_path;

  autoware_perception_msgs::msg::Shape shape;
  shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  shape.dimensions.x = 1.0;
  shape.dimensions.y = 1.0;

  double velocity = 1.0;

  const auto path = [&](geometry_msgs::msg::Pose initial_pose) {
    std::vector<PoseWithVelocityAndPolygonStamped> path;
    geometry_msgs::msg::Pose pose;
    for (size_t i = 0; i < 10; i++) {
      auto time = static_cast<double>(i);
      pose.position.x = initial_pose.position.x + time * velocity;
      pose.position.y = initial_pose.position.y;
      PoseWithVelocityAndPolygonStamped obj_pose_with_poly(
        time, pose, velocity, autoware::universe_utils::toPolygon2d(pose, shape));
      path.push_back(obj_pose_with_poly);
    }
    return path;
  };

  for (size_t i = 0; i < 2; i++) {
    predicted_path.path = path(createPose(0.0, static_cast<double>(i), 0.0, 0.0, 0.0, 0.0));
    predicted_path.confidence = 0.1f * (static_cast<float>(i) + 1.0f);
    predicted_paths.push_back(predicted_path);
  }
  object.predicted_paths = predicted_paths;

  bool use_all_predicted_path = true;
  EXPECT_EQ(getPredictedPathFromObj(object, use_all_predicted_path).size(), 2);

  use_all_predicted_path = false;
  auto extracted_path = getPredictedPathFromObj(object, use_all_predicted_path);
  EXPECT_EQ(extracted_path.size(), 1);
  EXPECT_DOUBLE_EQ(extracted_path.front().path.front().pose.position.y, 1.0);
}

TEST(BehaviorPathPlanningObjectsFiltering, createPredictedPath)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::createPredictedPath;

  std::shared_ptr<
    autoware::behavior_path_planner::utils::path_safety_checker::EgoPredictedPathParams>
    param = std::make_shared<
      autoware::behavior_path_planner::utils::path_safety_checker::EgoPredictedPathParams>();

  param->min_velocity = -1.0;
  param->acceleration = 0.1;
  param->max_velocity = 5.0;
  param->time_horizon_for_front_object = 2.0;
  param->time_horizon_for_rear_object = 1.0;
  param->time_resolution = 0.5;
  param->delay_until_departure = 0.1;

  auto vehicle_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  std::vector<PathPointWithLaneId> empty_path;
  auto predicted_path = createPredictedPath(param, empty_path, vehicle_pose, 0.0, 0, false, false);
  EXPECT_TRUE(predicted_path.empty());

  size_t i = 0;
  auto straight_path = trajectory_to_path_with_lane_id(generateTrajectory<Trajectory>(20, 1.0));
  predicted_path = createPredictedPath(param, straight_path, vehicle_pose, 0.0, 0, true, false);

  ASSERT_EQ(predicted_path.size(), 4);
  for (const auto & point : predicted_path) {
    auto time =
      std::max(0.0, param->time_resolution * static_cast<double>(i) - param->delay_until_departure);
    EXPECT_NEAR(point.time, i * param->time_resolution, epsilon);
    EXPECT_NEAR(point.pose.position.x, std::pow(time, 2.0) * param->acceleration * 0.5, epsilon);
    EXPECT_NEAR(point.velocity, time * param->acceleration, epsilon);
    i++;
  }

  i = 0;
  predicted_path = createPredictedPath(param, straight_path, vehicle_pose, 0.0, 0, false, false);

  ASSERT_EQ(predicted_path.size(), 2);
  for (const auto & point : predicted_path) {
    auto time =
      std::max(0.0, param->time_resolution * static_cast<double>(i) - param->delay_until_departure);
    EXPECT_NEAR(point.time, i * param->time_resolution, epsilon);
    EXPECT_NEAR(point.pose.position.x, std::pow(time, 2.0) * param->acceleration * 0.5, epsilon);
    EXPECT_NEAR(point.velocity, time * param->acceleration, epsilon);
    i++;
  }
}

TEST(BehaviorPathPlanningObjectsFiltering, transform)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::transform;
  auto velocity = autoware::universe_utils::createVector3(2.0, 0.0, 0.0);

  auto obj = create_bounding_box_object(
    createPose(2.0, 0.0, 0.0, 0.0, 0.0, 0.0), createVector3(2.0, 0.0, 0.0), 2.0, 1.0);
  autoware_perception_msgs::msg::PredictedPath predicted_path;
  auto straight_path = trajectory_to_predicted_path(generateTrajectory<Trajectory>(5, 1.0));
  straight_path.confidence = 0.6;
  straight_path.time_step.sec = 1.0;
  obj.kinematics.predicted_paths.push_back(straight_path);

  double safety_check_time_horizon = 2.0;
  double safety_check_time_resolution = 0.5;

  auto extended_obj = transform(obj, safety_check_time_horizon, safety_check_time_resolution);
  EXPECT_NEAR(
    extended_obj.predicted_paths.front().confidence,
    obj.kinematics.predicted_paths.front().confidence, epsilon);
  ASSERT_FALSE(extended_obj.predicted_paths.empty());
  EXPECT_EQ(extended_obj.predicted_paths.front().path.size(), 5);

  size_t i = 0;
  for (const auto & point : extended_obj.predicted_paths.front().path) {
    EXPECT_NEAR(point.pose.position.x, i * 0.5, epsilon);
    EXPECT_NEAR(point.pose.position.y, 0.0, epsilon);
    EXPECT_NEAR(point.velocity, velocity.x, epsilon);
    ASSERT_EQ(point.poly.outer().size(), 5);

    EXPECT_NEAR(
      point.poly.outer().at(0).x(), point.pose.position.x + 0.5 * obj.shape.dimensions.x, epsilon);
    EXPECT_NEAR(
      point.poly.outer().at(0).y(), point.pose.position.y + 0.5 * obj.shape.dimensions.y, epsilon);

    EXPECT_NEAR(
      point.poly.outer().at(1).x(), point.pose.position.x + 0.5 * obj.shape.dimensions.x, epsilon);
    EXPECT_NEAR(
      point.poly.outer().at(1).y(), point.pose.position.y - 0.5 * obj.shape.dimensions.y, epsilon);

    EXPECT_NEAR(
      point.poly.outer().at(2).x(), point.pose.position.x - 0.5 * obj.shape.dimensions.x, epsilon);
    EXPECT_NEAR(
      point.poly.outer().at(2).y(), point.pose.position.y - 0.5 * obj.shape.dimensions.y, epsilon);

    EXPECT_NEAR(
      point.poly.outer().at(3).x(), point.pose.position.x - 0.5 * obj.shape.dimensions.x, epsilon);
    EXPECT_NEAR(
      point.poly.outer().at(3).y(), point.pose.position.y + 0.5 * obj.shape.dimensions.y, epsilon);
    i++;
  }
}

TEST(BehaviorPathPlanningObjectsFiltering, filterObjectsByClass)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::filterObjectsByClass;

  PredictedObjects objects;
  PredictedObject car;
  PredictedObject truck;
  PredictedObject pedestrian;
  ObjectClassification classification;
  autoware::behavior_path_planner::utils::path_safety_checker::ObjectTypesToCheck types_to_check;

  car.object_id = autoware::universe_utils::generateUUID();
  classification.label = ObjectClassification::Type::CAR;
  classification.probability = 1.0;
  car.classification.push_back(classification);
  objects.objects.push_back(car);

  truck.object_id = autoware::universe_utils::generateUUID();
  classification.label = ObjectClassification::Type::TRUCK;
  classification.probability = 1.0;
  truck.classification.push_back(classification);
  objects.objects.push_back(truck);

  pedestrian.object_id = autoware::universe_utils::generateUUID();
  classification.label = ObjectClassification::Type::PEDESTRIAN;
  classification.probability = 1.0;
  pedestrian.classification.push_back(classification);
  objects.objects.push_back(pedestrian);

  filterObjectsByClass(objects, types_to_check);
  EXPECT_EQ(objects.objects.size(), 3);

  types_to_check.check_pedestrian = false;
  filterObjectsByClass(objects, types_to_check);
  EXPECT_EQ(objects.objects.size(), 2);

  types_to_check.check_car = false;
  types_to_check.check_truck = false;
  filterObjectsByClass(objects, types_to_check);
  EXPECT_TRUE(objects.objects.empty());
}

TEST(BehaviorPathPlanningObjectsFiltering, createTargetObjectsOnLane)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::createTargetObjectsOnLane;
  using autoware::behavior_path_planner::utils::path_safety_checker::ObjectsFilteringParams;
  using autoware::universe_utils::createVector3;

  PredictedObjects objects;
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler =
    std::make_shared<autoware::route_handler::RouteHandler>();
  std::shared_ptr<ObjectsFilteringParams> params = std::make_shared<ObjectsFilteringParams>();
  params->object_lane_configuration = {true, true, true, true, true};
  params->include_opposite_lane = true;
  params->invert_opposite_lane = false;
  params->safety_check_time_horizon = 10.0;
  params->safety_check_time_resolution = 1.0;
  route_handler->setMap(intersection_map);
  lanelet::ConstLanelets current_lanes;

  current_lanes.push_back(route_handler->getLaneletsFromId(1001));
  current_lanes.push_back(route_handler->getLaneletsFromId(1011));

  ObjectClassification classification;
  classification.label = ObjectClassification::Type::CAR;
  classification.probability = 1.0;

  PredictedObject current_lane_object =
    create_bounding_box_object(createPose(363.64, 565.03, 0.0, 0.0, 0.0, 0.0));
  PredictedObject right_lane_object =
    create_bounding_box_object(createPose(366.91, 523.47, 0.0, 0.0, 0.0, 0.0));

  objects.objects.push_back(current_lane_object);
  objects.objects.push_back(right_lane_object);

  auto target_objects_on_lane =
    createTargetObjectsOnLane(current_lanes, route_handler, objects, params);
  EXPECT_FALSE(target_objects_on_lane.on_current_lane.empty());
  EXPECT_FALSE(target_objects_on_lane.on_right_lane.empty());
  EXPECT_TRUE(target_objects_on_lane.on_left_lane.empty());
  EXPECT_TRUE(target_objects_on_lane.on_other_lane.empty());

  EXPECT_EQ(target_objects_on_lane.on_current_lane.front().uuid, current_lane_object.object_id);
  EXPECT_EQ(target_objects_on_lane.on_right_lane.front().uuid, right_lane_object.object_id);
}

TEST(BehaviorPathPlanningObjectsFiltering, isTargetObjectType)
{
  using autoware::behavior_path_planner::utils::path_safety_checker::isTargetObjectType;

  autoware::behavior_path_planner::utils::path_safety_checker::ObjectTypesToCheck types_to_check;
  PredictedObject obj;
  ObjectClassification classification;
  classification.label = ObjectClassification::Type::CAR;
  classification.probability = 0.6;
  obj.classification.push_back(classification);
  classification.label = ObjectClassification::Type::TRUCK;
  classification.probability = 0.4;
  obj.classification.push_back(classification);

  EXPECT_TRUE(isTargetObjectType(obj, types_to_check));
  types_to_check.check_car = false;
  EXPECT_FALSE(isTargetObjectType(obj, types_to_check));

  obj.classification.front().probability = 0.4;
  obj.classification.at(1).probability = 0.6;
  EXPECT_TRUE(isTargetObjectType(obj, types_to_check));

  obj.classification.at(1).label = ObjectClassification::Type::PEDESTRIAN;
  EXPECT_TRUE(isTargetObjectType(obj, types_to_check));
}

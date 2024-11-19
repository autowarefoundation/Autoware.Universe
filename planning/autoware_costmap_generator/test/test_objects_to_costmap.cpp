// Copyright 2022 The Autoware Contributors
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

#include <autoware/costmap_generator/utils/objects_to_costmap.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <rclcpp/rclcpp.hpp>

#include <gtest/gtest.h>
#include <tf2/utils.h>

namespace
{
geometry_msgs::msg::Point32 toPoint32(const geometry_msgs::msg::Pose & pose)
{
  geometry_msgs::msg::Point32 point;
  point.x = pose.position.x;
  point.y = pose.position.y;
  point.z = pose.position.z;
  return point;
}
}  // namespace

namespace autoware::costmap_generator
{
using LABEL = autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_perception_msgs::msg::Shape;

class ObjectsToCostMapTest : public ::testing::Test
{
protected:
  void SetUp() override { rclcpp::init(0, nullptr); }

  ~ObjectsToCostMapTest() override { rclcpp::shutdown(); }

  grid_map::GridMap construct_gridmap();

  [[nodiscard]] PredictedObject get_object(
    const geometry_msgs::msg::Pose & pose, const geometry_msgs::msg::Vector3 dimension,
    const bool box_type = true) const;

public:
  double grid_resolution_ = 1;
  double grid_length_x_ = 21;
  double grid_length_y_ = 21;
  double grid_position_x_ = 0;
  double grid_position_y_ = 0;
};

grid_map::GridMap ObjectsToCostMapTest::construct_gridmap()
{
  grid_map::GridMap gm;

  // set gridmap size
  gm.setFrameId("map");
  gm.setGeometry(
    grid_map::Length(grid_length_x_, grid_length_y_), grid_resolution_,
    grid_map::Position(grid_position_x_, grid_position_y_));

  // set initial value
  gm.add("objects", 0.);

  // set car postion in map frame to center of grid
  grid_map::Position p;
  p.x() = 0;
  p.y() = 0;
  gm.setPosition(p);

  return gm;
}

PredictedObject ObjectsToCostMapTest::get_object(
  const geometry_msgs::msg::Pose & pose, const geometry_msgs::msg::Vector3 dimension,
  const bool box_type) const
{
  PredictedObject object;
  object.classification.push_back(LABEL{});
  object.classification.at(0).label = LABEL::CAR;
  object.classification.at(0).probability = 0.8;
  object.kinematics.initial_pose_with_covariance.pose = pose;
  object.shape.dimensions = dimension;
  if (box_type) {
    object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
    return object;
  }

  object.shape.type = autoware_perception_msgs::msg::Shape::POLYGON;

  object.shape.footprint.points.emplace_back(toPoint32(
    autoware::universe_utils::calcOffsetPose(pose, -0.5 * dimension.x, -0.5 * dimension.y, 0.0)));

  object.shape.footprint.points.emplace_back(toPoint32(
    autoware::universe_utils::calcOffsetPose(pose, -0.5 * dimension.x, 0.5 * dimension.y, 0.0)));

  object.shape.footprint.points.emplace_back(toPoint32(
    autoware::universe_utils::calcOffsetPose(pose, 0.5 * dimension.x, 0.5 * dimension.y, 0.0)));

  object.shape.footprint.points.emplace_back(toPoint32(
    autoware::universe_utils::calcOffsetPose(pose, 0.5 * dimension.x, -0.5 * dimension.y, 0.0)));

  return object;
}

/*
grid_y
|
|     map_x---
|            |
|            |
|            map_y
|--------grid_x
*/
TEST_F(ObjectsToCostMapTest, TestMakeCostmapFromObjects_BoxType)
{
  auto objs = std::make_shared<PredictedObjects>();

  geometry_msgs::msg::Pose obj_pose;
  obj_pose.position.x = 1;
  obj_pose.position.y = 2;
  obj_pose.position.z = 1;
  obj_pose.orientation.x = 0;
  obj_pose.orientation.y = 0;
  obj_pose.orientation.z = 0;
  obj_pose.orientation.w = 1;

  geometry_msgs::msg::Vector3 dimension;
  dimension.x = 5;
  dimension.y = 3;
  dimension.z = 2;

  const auto object = get_object(obj_pose, dimension);
  objs->objects.push_back(object);

  grid_map::GridMap gridmap = construct_gridmap();
  ObjectsToCostmap objectsToCostmap;

  const double expand_polygon_size = 0.0;
  const double size_of_expansion_kernel = 1;  // do not expand for easy test check
  grid_map::Matrix objects_costmap = objectsToCostmap.makeCostmapFromObjects(
    gridmap, expand_polygon_size, size_of_expansion_kernel, objs);

  // yaw = 0,so we can just calculate like this easily
  int expected_non_empty_cost_grid_num =
    (object.shape.dimensions.x * object.shape.dimensions.y) / grid_resolution_;

  // check if cost is correct
  int non_empty_cost_grid_num = 0;
  for (int i = 0; i < objects_costmap.rows(); i++) {
    for (int j = 0; j < objects_costmap.cols(); j++) {
      if (objects_costmap(i, j) == object.classification.at(0).probability) {
        non_empty_cost_grid_num += 1;
      }
    }
  }

  EXPECT_EQ(non_empty_cost_grid_num, expected_non_empty_cost_grid_num);

  float obj_center_x =
    grid_length_x_ / 2 - object.kinematics.initial_pose_with_covariance.pose.position.x;
  float obj_center_y =
    grid_length_y_ / 2 - object.kinematics.initial_pose_with_covariance.pose.position.y;
  float obj_left_x = obj_center_x - object.shape.dimensions.x / 2.;
  float obj_right_x = obj_center_x + object.shape.dimensions.x / 2.;
  float obj_bottom_y = obj_center_y - object.shape.dimensions.y / 2.;
  float obj_top_y = obj_center_y + object.shape.dimensions.y / 2.;
  int index_x_min = static_cast<int>(obj_left_x / grid_resolution_);
  int index_x_max = static_cast<int>(obj_right_x / grid_resolution_);
  int index_y_min = static_cast<int>(obj_bottom_y / grid_resolution_);
  int index_y_max = static_cast<int>(obj_top_y / grid_resolution_);
  for (int i = index_x_min; i < index_x_max; i++) {
    for (int j = index_y_min; j < index_y_max; j++) {
      EXPECT_DOUBLE_EQ(objects_costmap(i, j), object.classification.at(0).probability);
    }
  }
}

TEST_F(ObjectsToCostMapTest, TestMakeCostmapFromObjects_PolygonType)
{
  auto objs = std::make_shared<PredictedObjects>();

  geometry_msgs::msg::Pose obj_pose;
  obj_pose.position.x = 1;
  obj_pose.position.y = 2;
  obj_pose.position.z = 1;
  obj_pose.orientation.x = 0;
  obj_pose.orientation.y = 0;
  obj_pose.orientation.z = 0;
  obj_pose.orientation.w = 1;

  geometry_msgs::msg::Vector3 dimension;
  dimension.x = 5;
  dimension.y = 3;
  dimension.z = 2;

  const auto object = get_object(obj_pose, dimension, false);
  objs->objects.push_back(object);

  grid_map::GridMap gridmap = construct_gridmap();
  ObjectsToCostmap objectsToCostmap;

  const double expand_polygon_size = 0.0;
  const double size_of_expansion_kernel = 1;  // do not expand for easy test check
  grid_map::Matrix objects_costmap = objectsToCostmap.makeCostmapFromObjects(
    gridmap, expand_polygon_size, size_of_expansion_kernel, objs);

  // yaw = 0,so we can just calculate like this easily
  int expected_non_empty_cost_grid_num =
    (object.shape.dimensions.x * object.shape.dimensions.y) / grid_resolution_;

  // check if cost is correct
  int non_empty_cost_grid_num = 0;
  for (int i = 0; i < objects_costmap.rows(); i++) {
    for (int j = 0; j < objects_costmap.cols(); j++) {
      if (objects_costmap(i, j) == object.classification.at(0).probability) {
        non_empty_cost_grid_num += 1;
      }
    }
  }

  EXPECT_EQ(non_empty_cost_grid_num, expected_non_empty_cost_grid_num);

  float obj_center_x =
    grid_length_x_ / 2 - object.kinematics.initial_pose_with_covariance.pose.position.x;
  float obj_center_y =
    grid_length_y_ / 2 - object.kinematics.initial_pose_with_covariance.pose.position.y;
  float obj_left_x = obj_center_x - object.shape.dimensions.x / 2.;
  float obj_right_x = obj_center_x + object.shape.dimensions.x / 2.;
  float obj_bottom_y = obj_center_y - object.shape.dimensions.y / 2.;
  float obj_top_y = obj_center_y + object.shape.dimensions.y / 2.;
  int index_x_min = static_cast<int>(obj_left_x / grid_resolution_);
  int index_x_max = static_cast<int>(obj_right_x / grid_resolution_);
  int index_y_min = static_cast<int>(obj_bottom_y / grid_resolution_);
  int index_y_max = static_cast<int>(obj_top_y / grid_resolution_);
  for (int i = index_x_min; i < index_x_max; i++) {
    for (int j = index_y_min; j < index_y_max; j++) {
      EXPECT_DOUBLE_EQ(objects_costmap(i, j), object.classification.at(0).probability);
    }
  }
}
}  // namespace autoware::costmap_generator

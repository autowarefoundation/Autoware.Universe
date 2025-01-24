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

#include "autoware/behavior_path_planner_common/utils/occupancy_grid_based_collision_detector/occupancy_grid_based_collision_detector.hpp"

#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gtest/gtest.h>

using autoware::behavior_path_planner::OccupancyGridBasedCollisionDetector;
using autoware::behavior_path_planner::OccupancyGridMapParam;
using autoware::test_utils::createPose;

class OccupancyGridBasedCollisionDetectorTest : public ::testing::Test
{
protected:
  OccupancyGridBasedCollisionDetector detector_;
  nav_msgs::msg::OccupancyGrid costmap_;
  OccupancyGridMapParam param_;

  void SetUp() override
  {
    costmap_.info.width = 40;
    costmap_.info.height = 40;
    costmap_.info.resolution = 0.25;
    costmap_.data = std::vector<int8_t>(1600, 0);
    costmap_.data[28 * costmap_.info.width + 28] = 100;
    costmap_.info.origin = createPose(1.0, 1.0, 0.0, 0.0, 0.0, 0.0);

    param_.theta_size = 8;
    param_.obstacle_threshold = 1;
    param_.vehicle_shape.base2back = 0.0;
    param_.vehicle_shape.length = 4.0;
    param_.vehicle_shape.width = 2.0;
    detector_.setParam(param_);
  }
};

TEST_F(OccupancyGridBasedCollisionDetectorTest, discretize_angle)
{
  using autoware::behavior_path_planner::discretize_angle;

  int theta_size = 8;

  EXPECT_EQ(discretize_angle(0.0, theta_size), 0);
  EXPECT_EQ(discretize_angle(M_PI * 0.25, theta_size), 1);
  EXPECT_EQ(discretize_angle(2.5 * M_PI, theta_size), 2);
}

TEST_F(OccupancyGridBasedCollisionDetectorTest, pose2index)
{
  using autoware::behavior_path_planner::pose2index;

  auto pose_local = createPose(2.0, 3.0, 0.0, 0.0, 0.0, 0.0);

  int theta_size = 8;
  auto index = pose2index(costmap_, pose_local, theta_size);

  EXPECT_EQ(index.x, 8);
  EXPECT_EQ(index.y, 12);
  EXPECT_EQ(index.theta, 0);
}

TEST_F(OccupancyGridBasedCollisionDetectorTest, index2pose)
{
  using autoware::behavior_path_planner::index2pose;

  autoware::behavior_path_planner::IndexXYT index{4, 6, 2};

  int theta_size = 8;
  auto pose_local = index2pose(costmap_, index, theta_size);

  EXPECT_DOUBLE_EQ(pose_local.position.x, 1.0);
  EXPECT_DOUBLE_EQ(pose_local.position.y, 1.5);
  EXPECT_DOUBLE_EQ(tf2::getYaw(pose_local.orientation), M_PI * 0.5);
}

TEST_F(OccupancyGridBasedCollisionDetectorTest, global2local)
{
  using autoware::behavior_path_planner::global2local;

  auto pose_global = createPose(2.0, 2.0, 0.0, 0.0, 0.0, 0.0);
  auto pose_local = global2local(costmap_, pose_global);

  EXPECT_DOUBLE_EQ(pose_local.position.x, 1.0);
  EXPECT_DOUBLE_EQ(pose_local.position.y, 1.0);
}

TEST_F(OccupancyGridBasedCollisionDetectorTest, detectCollision)
{
  using autoware::behavior_path_planner::IndexXYT;

  // Condition: map not set
  IndexXYT base_index{0, 0, 0};
  base_index.x = 24;
  base_index.y = 24;
  EXPECT_FALSE(detector_.detectCollision(base_index, true));

  // Condition: with object
  detector_.setMap(costmap_);
  EXPECT_TRUE(detector_.detectCollision(base_index, true));

  // Condition: position without obstacle
  base_index.x = 4;
  base_index.y = 4;
  EXPECT_FALSE(detector_.detectCollision(base_index, true));

  // Condition: position out of range
  base_index.x = -100;
  base_index.y = -100;
  EXPECT_TRUE(detector_.detectCollision(base_index, true));
  EXPECT_FALSE(detector_.detectCollision(base_index, false));
}

TEST_F(OccupancyGridBasedCollisionDetectorTest, hasObstacleOnPath)
{
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  detector_.setMap(costmap_);

  // Condition: empty path
  EXPECT_FALSE(detector_.hasObstacleOnPath(path, true));

  // Condition: no obstacle on path
  size_t path_length = 10;
  path.points.reserve(path_length);
  for (size_t i = 0; i < path_length; i++) {
    autoware_internal_planning_msgs::msg::PathPointWithLaneId path_point;
    path_point.point.pose = createPose(static_cast<double>(i), 0.0, 0.0, 0.0, 0.0, 0.0);
    path.points.push_back(path_point);
  }
  EXPECT_FALSE(detector_.hasObstacleOnPath(path, false));

  // Condition: obstacle on path
  for (size_t i = 0; i < path_length; i++) {
    path.points.at(i).point.pose.position.y = 8.0;
  }
  EXPECT_TRUE(detector_.hasObstacleOnPath(path, false));
}

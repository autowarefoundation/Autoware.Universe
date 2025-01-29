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

#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"
#include "autoware/behavior_path_planner_common/utils/path_utils.hpp"

#include <autoware_test_utils/autoware_test_utils.hpp>

#include <autoware_internal_planning_msgs/msg/detail/path_with_lane_id__struct.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

using autoware_internal_planning_msgs::msg::PathWithLaneId;

using autoware::test_utils::createPose;
using autoware::test_utils::generateTrajectory;

TEST(BehaviorPathPlanningPathUtilTest, calcPathArcLengthArray)
{
  using autoware::behavior_path_planner::utils::calcPathArcLengthArray;

  auto path = generateTrajectory<PathWithLaneId>(10, 1.0);
  auto arc_length_array = calcPathArcLengthArray(path);

  ASSERT_EQ(arc_length_array.size(), path.points.size());

  size_t i = 0;
  for (const auto & arc_length : arc_length_array) {
    EXPECT_DOUBLE_EQ(arc_length, 1.0 * i);
    i++;
  }
}

TEST(BehaviorPathPlanningPathUtilTest, resamplePathWithSpline)
{
  using autoware::behavior_path_planner::utils::resamplePathWithSpline;

  // Condition: path point less than 2
  auto path = generateTrajectory<PathWithLaneId>(1, 1.0);
  EXPECT_EQ(resamplePathWithSpline(path, 1.0).points.size(), path.points.size());

  // Condition: not enough point for spline interpolation
  path = generateTrajectory<PathWithLaneId>(10, 0.01);
  EXPECT_EQ(resamplePathWithSpline(path, 1.0).points.size(), path.points.size());

  // Condition: spline interpolation
  path = generateTrajectory<PathWithLaneId>(10, 0.1);
  auto resampled_path = resamplePathWithSpline(path, 1.0);
  EXPECT_EQ(resampled_path.points.size(), 5);
}

TEST(BehaviorPathPlanningPathUtilTest, getIdxByArclength)
{
  using autoware::behavior_path_planner::utils::getIdxByArclength;

  autoware_internal_planning_msgs::msg::PathWithLaneId path;

  // Condition: empty points
  EXPECT_ANY_THROW(getIdxByArclength(path, 5, 1.0));

  // Condition: negative arc with idx 0
  path = generateTrajectory<PathWithLaneId>(10, 1.0);
  EXPECT_EQ(getIdxByArclength(path, 0, -1.0), 0);

  // Condition: negative arc
  EXPECT_EQ(getIdxByArclength(path, 5, -2.0), 2);

  // Condition: positive arc
  EXPECT_EQ(getIdxByArclength(path, 3, 4.0), 8);
}

TEST(BehaviorPathPlanningPathUtilTest, clipPathLength)
{
  using autoware::behavior_path_planner::utils::clipPathLength;

  // Condition: path point less than 3
  auto path = generateTrajectory<PathWithLaneId>(2, 1.0);
  clipPathLength(path, 5, 10.0, 1.0);
  EXPECT_EQ(path.points.size(), 2);

  // Condition: path to be cropped
  path = generateTrajectory<PathWithLaneId>(10, 1.0);
  clipPathLength(path, 5, 10.0, 1.0);
  EXPECT_EQ(path.points.size(), 7);

  size_t i = 0;
  for (const auto & point : path.points) {
    EXPECT_DOUBLE_EQ(point.point.pose.position.x, 3.0 + i);
    i++;
  }
}

TEST(BehaviorPathPlanningPathUtilTest, getReversingIndices)
{
  using autoware::behavior_path_planner::utils::getReversingIndices;

  size_t target_index = 7;
  auto path = generateTrajectory<PathWithLaneId>(10, 1.0, 1.0);
  path.points.at(target_index).point.longitudinal_velocity_mps = -1.0;
  auto reversing_indices = getReversingIndices(path);
  EXPECT_EQ(reversing_indices.size(), 2);

  size_t i = 0;
  for (const auto & index : reversing_indices) {
    EXPECT_EQ(index, target_index - 1 + i);
    i++;
  }
}

TEST(BehaviorPathPlanningPathUtilTest, dividePath)
{
  using autoware::behavior_path_planner::utils::dividePath;
  auto path = generateTrajectory<PathWithLaneId>(10, 1.0);

  // Condition: empty indices
  std::vector<size_t> indices;
  auto divided_path = dividePath(path, indices);
  EXPECT_EQ(divided_path.size(), 1);

  // Condition: divide path
  indices = {3, 5, 8};
  divided_path = dividePath(path, indices);
  ASSERT_EQ(divided_path.size(), 4);
  EXPECT_EQ(divided_path.at(0).points.size(), 4);
  EXPECT_EQ(divided_path.at(1).points.size(), 3);
  EXPECT_EQ(divided_path.at(2).points.size(), 4);
  EXPECT_EQ(divided_path.at(3).points.size(), 2);
}

TEST(BehaviorPathPlanningPathUtilTest, correctDividedPathVelocity)
{
  using autoware::behavior_path_planner::utils::correctDividedPathVelocity;

  double velocity = 1.0;
  std::vector<PathWithLaneId> divided_paths;
  // forward driving
  divided_paths.push_back(generateTrajectory<PathWithLaneId>(10, 1.0, velocity));
  // reverse driving
  divided_paths.push_back(generateTrajectory<PathWithLaneId>(10, -1.0, velocity, M_PI));
  correctDividedPathVelocity(divided_paths);

  for (const auto & point : divided_paths.at(0).points) {
    if (point == divided_paths.at(0).points.back()) {
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, 0.0);
    } else {
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, velocity);
    }
  }

  for (const auto & point : divided_paths.at(1).points) {
    if (point == divided_paths.at(1).points.back()) {
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, 0.0);
    } else {
      EXPECT_DOUBLE_EQ(point.point.longitudinal_velocity_mps, -velocity);
    }
  }
}

TEST(BehaviorPathPlanningPathUtilTest, interpolatePose)
{
  using autoware::behavior_path_planner::utils::interpolatePose;

  auto start_pose = createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  auto end_pose = createPose(10.0, 2.0, 0.0, 0.0, 0.0, M_PI_2);
  double resample_interval = 1.0;

  auto resampled_pose = interpolatePose(start_pose, end_pose, resample_interval);
  EXPECT_EQ(resampled_pose.size(), 10);
}

TEST(BehaviorPathPlanningPathUtilTest, getUnshiftedEgoPose)
{
  using autoware::behavior_path_planner::utils::getUnshiftedEgoPose;

  auto ego_pose = createPose(2.0, 4.0, 0.0, 0.0, 0.0, 0.0);
  autoware::behavior_path_planner::ShiftedPath shifted_path;

  // Condition: empty path
  auto unshifted_ego_pose = getUnshiftedEgoPose(ego_pose, shifted_path);
  EXPECT_DOUBLE_EQ(unshifted_ego_pose.position.x, ego_pose.position.x);
  EXPECT_DOUBLE_EQ(unshifted_ego_pose.position.y, ego_pose.position.y);

  shifted_path.path = generateTrajectory<PathWithLaneId>(10, 1.0);
  for (size_t i = 0; i < shifted_path.path.points.size(); i++) {
    shifted_path.shift_length.push_back(static_cast<double>(i) * 0.1);
  }

  // Condition: path with increasing offset
  unshifted_ego_pose = getUnshiftedEgoPose(ego_pose, shifted_path);
  EXPECT_DOUBLE_EQ(unshifted_ego_pose.position.x, ego_pose.position.x);
  EXPECT_DOUBLE_EQ(unshifted_ego_pose.position.y, -0.2);
}

TEST(BehaviorPathPlanningPathUtilTest, combinePath)
{
  using autoware::behavior_path_planner::utils::combinePath;

  PathWithLaneId first_path;
  auto second_path = generateTrajectory<PathWithLaneId>(10, 1.0);

  // Condition: first path empty
  auto combined_path = combinePath(first_path, second_path);
  EXPECT_EQ(combined_path.points.size(), 10);
  size_t i = 0;
  for (const auto & point : combined_path.points) {
    EXPECT_DOUBLE_EQ(point.point.pose.position.x, static_cast<double>(i));
    i++;
  }

  // Condition: second path empty
  first_path = generateTrajectory<PathWithLaneId>(4, 0.5);
  second_path.points.clear();
  combined_path = combinePath(first_path, second_path);
  EXPECT_EQ(combined_path.points.size(), 4);
  i = 0;
  for (const auto & point : combined_path.points) {
    EXPECT_DOUBLE_EQ(point.point.pose.position.x, static_cast<double>(i) * 0.5);
    i++;
  }

  // Condition: combine path
  second_path = generateTrajectory<PathWithLaneId>(20, 0.25);
  for (auto & point : second_path.points) {
    point.point.pose.position.x += 1.5;
  }
  combined_path = combinePath(first_path, second_path);
  EXPECT_EQ(combined_path.points.size(), 23);
  i = 0;
  for (const auto & point : combined_path.points) {
    if (i < 4)
      EXPECT_DOUBLE_EQ(point.point.pose.position.x, static_cast<double>(i) * 0.5);
    else
      EXPECT_DOUBLE_EQ(point.point.pose.position.x, static_cast<double>(i - 3) * 0.25 + 1.5);

    i++;
  }
}

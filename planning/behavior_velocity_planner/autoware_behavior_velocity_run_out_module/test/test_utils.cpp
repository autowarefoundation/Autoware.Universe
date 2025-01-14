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

#include "dynamic_obstacle.hpp"
#include "path_utils.hpp"
#include "utils.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/time.hpp>

#include <autoware_perception_msgs/msg/detail/object_classification__struct.hpp>
#include <tier4_planning_msgs/msg/detail/path_point_with_lane_id__struct.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>
#include <vector>

using autoware::behavior_velocity_planner::DynamicObstacle;
using autoware::behavior_velocity_planner::run_out_utils::createBoostPolyFromMsg;
using autoware::behavior_velocity_planner::run_out_utils::createExtendPathPoint;
using autoware::behavior_velocity_planner::run_out_utils::decimatePathPoints;
using autoware::behavior_velocity_planner::run_out_utils::DetectionMethod;
using autoware::behavior_velocity_planner::run_out_utils::findFirstStopPointIdx;
using autoware::behavior_velocity_planner::run_out_utils::findLateralSameSidePoints;
using autoware::behavior_velocity_planner::run_out_utils::getHighestConfidencePath;
using autoware::behavior_velocity_planner::run_out_utils::getHighestProbLabel;
using autoware::behavior_velocity_planner::run_out_utils::insertPathVelocityFromIndex;
using autoware::behavior_velocity_planner::run_out_utils::insertPathVelocityFromIndexLimited;
using autoware::behavior_velocity_planner::run_out_utils::isSamePoint;
using autoware::behavior_velocity_planner::run_out_utils::lerpByPose;
using autoware::behavior_velocity_planner::run_out_utils::pathIntersectsEgoCutLine;
using autoware::behavior_velocity_planner::run_out_utils::PathPointsWithLaneId;
using autoware::behavior_velocity_planner::run_out_utils::PredictedPath;
using autoware::behavior_velocity_planner::run_out_utils::toEnum;
using autoware::behavior_velocity_planner::run_out_utils::trimPathFromSelfPose;

using autoware_perception_msgs::msg::ObjectClassification;
using geometry_msgs::msg::Point;
using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;

class TestRunOutUtils : public ::testing::Test
{
  void SetUp() override {}
};

TEST_F(TestRunOutUtils, testFindLongitudinalNearestPoint)
{
  std::vector<geometry_msgs::msg::Point> poly;
  poly.push_back(autoware::universe_utils::createPoint(0.0, 1.0, 0.0));
  poly.push_back(autoware::universe_utils::createPoint(0.0, -1.0, 0.0));
  poly.push_back(autoware::universe_utils::createPoint(1.0, 1.0, 0.0));
  poly.push_back(autoware::universe_utils::createPoint(1.0, 0.0, 0.0));

  const auto boost_poly = createBoostPolyFromMsg(poly);
  EXPECT_FALSE(boost_poly.outer().empty());
  EXPECT_EQ(boost_poly.outer().size(), poly.size() + 1);
}

TEST_F(TestRunOutUtils, testGetHighestProbLabel)
{
  ObjectClassification classification_1;
  classification_1.label = ObjectClassification::BICYCLE;
  classification_1.probability = 0.7;

  ObjectClassification classification_2;
  classification_2.label = ObjectClassification::MOTORCYCLE;
  classification_2.probability = 0.1;

  ObjectClassification classification_3;
  classification_3.label = ObjectClassification::TRUCK;
  classification_3.probability = 0.2;

  std::vector<ObjectClassification> classifications{
    classification_1, classification_2, classification_3};
  const auto classification = getHighestProbLabel(classifications);

  EXPECT_EQ(classification, classification_1.label);
}

TEST_F(TestRunOutUtils, testGetHighestConfidencePath)
{
  std::vector<PredictedPath> predicted_paths{};
  const auto empty_path = getHighestConfidencePath(predicted_paths);
  EXPECT_TRUE(empty_path.empty());
  geometry_msgs::msg::Pose p1;
  p1.position.x = 1.0;

  geometry_msgs::msg::Pose p2;
  p2.position.x = 2.0;

  PredictedPath predicted_path_1{};
  predicted_path_1.path = {p1};
  predicted_path_1.confidence = 0.85;

  PredictedPath predicted_path_2{};
  predicted_path_2.path = {p2};
  predicted_path_2.confidence = 0.15;

  predicted_paths.push_back(predicted_path_1);
  predicted_paths.push_back(predicted_path_2);

  const auto high_confidence_path = getHighestConfidencePath(predicted_paths);
  const auto path_point = high_confidence_path.front();
  EXPECT_TRUE(isSamePoint(path_point.position, p1.position));
}

TEST_F(TestRunOutUtils, testLerpByPose)
{
  geometry_msgs::msg::Pose p1;
  p1.position.x = 1.0;
  geometry_msgs::msg::Pose p2;
  p2.position.x = 2.0;
  const auto p3 = lerpByPose(p1, p2, 0.5);
  EXPECT_DOUBLE_EQ(1.5, p3.position.x);
}

TEST_F(TestRunOutUtils, testFindLateralSameSidePoints)
{
  std::vector<geometry_msgs::msg::Point> points;
  for (size_t i = 0; i < 10; ++i) {
    geometry_msgs::msg::Point p;
    p.x = static_cast<double>(i);
    p.y = 0.0;
    points.push_back(p);
  }

  {
    geometry_msgs::msg::Pose base_pose;
    base_pose.position.x = 1.0;
    base_pose.position.y = -1.0;

    geometry_msgs::msg::Point target_pose;
    target_pose.x = 2.0;
    target_pose.y = 2.0;
    const auto same_points = findLateralSameSidePoints(points, base_pose, target_pose);
    EXPECT_EQ(same_points.size(), points.size());
  }

  {
    geometry_msgs::msg::Pose base_pose;
    base_pose.position.x = 1.0;
    base_pose.position.y = 1.0;

    geometry_msgs::msg::Point target_pose;
    target_pose.x = 2.0;
    target_pose.y = 2.0;

    const auto same_points = findLateralSameSidePoints(points, base_pose, target_pose);
    EXPECT_FALSE(same_points.empty());
  }
}

TEST_F(TestRunOutUtils, testInsertPathVelocity)
{
  constexpr double path_velocity = 2.0;
  constexpr size_t n_path_points = 100;

  PathPointWithLaneId p;
  p.point.longitudinal_velocity_mps = path_velocity;
  PathPointsWithLaneId path(n_path_points, p);

  const size_t middle_index = path.size() / 2;

  constexpr double high_velocity = 2.0 * path_velocity;
  insertPathVelocityFromIndexLimited(middle_index, high_velocity, path);
  const auto middle_itr = path.begin() + path.size() / 2;
  const bool is_velocity_not_modified =
    std::all_of(middle_itr, path.end(), [path_velocity](const auto & v) {
      return std::abs(v.point.longitudinal_velocity_mps - path_velocity) <
             std::numeric_limits<double>::epsilon();
    });
  EXPECT_TRUE(is_velocity_not_modified);

  constexpr double low_velocity = 0.5 * path_velocity;
  insertPathVelocityFromIndexLimited(middle_index, low_velocity, path);
  const bool is_velocity_modified =
    std::all_of(middle_itr, path.end(), [low_velocity](const auto & v) {
      return std::abs(v.point.longitudinal_velocity_mps - low_velocity) <
             std::numeric_limits<double>::epsilon();
    });
  EXPECT_TRUE(is_velocity_modified);

  insertPathVelocityFromIndex(0, high_velocity, path);
  const bool all_velocities_modified =
    std::all_of(path.begin(), path.end(), [high_velocity](const auto & v) {
      return std::abs(v.point.longitudinal_velocity_mps - high_velocity) <
             std::numeric_limits<double>::epsilon();
    });
  EXPECT_TRUE(all_velocities_modified);

  auto first_stop_point = findFirstStopPointIdx(path);
  EXPECT_FALSE(first_stop_point.has_value());
  insertPathVelocityFromIndex(middle_index, 0.0, path);
  first_stop_point = findFirstStopPointIdx(path);
  EXPECT_TRUE(first_stop_point.has_value());
  EXPECT_EQ(middle_index, first_stop_point.value());
}

TEST_F(TestRunOutUtils, testPathIntersectsEgoCutLine)
{
  std::vector<geometry_msgs::msg::Pose> poses;
  geometry_msgs::msg::Pose ego_pose;
  constexpr double half_line_length = 2.0;
  std::vector<geometry_msgs::msg::Point> ego_cut_line;
  EXPECT_FALSE(pathIntersectsEgoCutLine(poses, ego_pose, half_line_length, ego_cut_line));
  for (size_t i = 0; i < 10; ++i) {
    geometry_msgs::msg::Pose p;
    p.position.x = static_cast<double>(i) - 5.0;
    p.position.y = 0.5;
    poses.push_back(p);
  }

  EXPECT_TRUE(pathIntersectsEgoCutLine(poses, ego_pose, half_line_length, ego_cut_line));
  ego_pose.position.y = 3.0;
  EXPECT_FALSE(pathIntersectsEgoCutLine(poses, ego_pose, half_line_length, ego_cut_line));
}

TEST_F(TestRunOutUtils, testExcludeObstaclesOutSideOfLine)
{
  constexpr size_t n_path_points{100};

  PathPointsWithLaneId path;

  PathPointWithLaneId base_point;

  for (size_t i = 0; i < n_path_points; ++i) {
    const PathPointWithLaneId p = createExtendPathPoint(static_cast<double>(i) / 10.0, base_point);
    path.push_back(p);
  }

  path = decimatePathPoints(path, 1.0);
  EXPECT_EQ(path.size(), n_path_points / 10);

  lanelet::BasicPolygon2d partition;
  partition.emplace_back(1.0, 1.0);
  partition.emplace_back(2.0, 1.0);
  partition.emplace_back(3.0, 1.0);
  partition.emplace_back(4.0, 1.0);

  constexpr double long_position{2.0};
  constexpr double lat_position_of_excluded_obstacle{2.0};
  constexpr double lat_position_of_included_obstacle{-2.0};

  DynamicObstacle obstacle_1;
  obstacle_1.pose.position.x = long_position;
  obstacle_1.pose.position.y = lat_position_of_included_obstacle;

  DynamicObstacle obstacle_2;
  obstacle_2.pose.position.x = long_position;
  obstacle_2.pose.position.y = lat_position_of_excluded_obstacle;
  std::vector<DynamicObstacle> obstacles{obstacle_1, obstacle_2};

  const auto filtered_obstacles = excludeObstaclesOutSideOfLine(obstacles, path, partition);

  EXPECT_EQ(filtered_obstacles.size(), 1);
  EXPECT_DOUBLE_EQ(filtered_obstacles.front().pose.position.y, lat_position_of_included_obstacle);
}

TEST_F(TestRunOutUtils, testTrimPathFromSelfPose)
{
  constexpr double point_interval{1e-2};
  const auto path =
    autoware::test_utils::generateTrajectory<PathWithLaneId>(1000, point_interval, 1.0, 0.0, 0.0);

  geometry_msgs::msg::Pose self_pose;
  constexpr double trim_distance{10.0};

  const auto trimmed_path = trimPathFromSelfPose(path, self_pose, trim_distance);
  const auto path_length = autoware::motion_utils::calcArcLength(path.points);
  EXPECT_TRUE(
    path_length - trim_distance < point_interval + std::numeric_limits<double>::epsilon());
}

TEST_F(TestRunOutUtils, testToEnum)
{
  EXPECT_EQ(toEnum("Object"), DetectionMethod::Object);
  EXPECT_EQ(toEnum("ObjectWithoutPath"), DetectionMethod::ObjectWithoutPath);
  EXPECT_EQ(toEnum("Points"), DetectionMethod::Points);
  EXPECT_EQ(toEnum("Autoware"), DetectionMethod::Unknown);
}

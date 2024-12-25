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
#include "autoware/trajectory/path_point_with_lane_id.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <vector>

using Trajectory = autoware::trajectory::Trajectory<tier4_planning_msgs::msg::PathPointWithLaneId>;

tier4_planning_msgs::msg::PathPointWithLaneId path_point_with_lane_id(
  double x, double y, uint8_t lane_id)
{
  tier4_planning_msgs::msg::PathPointWithLaneId point;
  point.point.pose.position.x = x;
  point.point.pose.position.y = y;
  point.lane_ids.emplace_back(lane_id);
  return point;
}

TEST(TrajectoryCreatorTest, create)
{
  {
    std::vector<tier4_planning_msgs::msg::PathPointWithLaneId> points{
      path_point_with_lane_id(0.00, 0.00, 0)};
    auto trajectory = Trajectory::Builder{}.build(points);
    ASSERT_TRUE(!trajectory);
  }
  {
    std::vector<tier4_planning_msgs::msg::PathPointWithLaneId> points{
      path_point_with_lane_id(0.00, 0.00, 0), path_point_with_lane_id(0.81, 1.68, 0),
      path_point_with_lane_id(1.65, 2.98, 0), path_point_with_lane_id(3.30, 4.01, 1)};
    auto trajectory = Trajectory::Builder{}.build(points);
    ASSERT_TRUE(trajectory);
  }
}

class TrajectoryTest : public ::testing::Test
{
public:
  std::optional<Trajectory> trajectory;

  void SetUp() override
  {
    std::vector<tier4_planning_msgs::msg::PathPointWithLaneId> points{
      path_point_with_lane_id(0.00, 0.00, 0), path_point_with_lane_id(0.81, 1.68, 0),
      path_point_with_lane_id(1.65, 2.98, 0), path_point_with_lane_id(3.30, 4.01, 1),
      path_point_with_lane_id(4.70, 4.52, 1), path_point_with_lane_id(6.49, 5.20, 1),
      path_point_with_lane_id(8.11, 6.07, 1), path_point_with_lane_id(8.76, 7.23, 1),
      path_point_with_lane_id(9.36, 8.74, 1), path_point_with_lane_id(10.0, 10.0, 1)};

    trajectory = Trajectory::Builder{}.build(points);
    ASSERT_TRUE(trajectory);
  }
};

TEST_F(TrajectoryTest, compute)
{
  double length = trajectory->length();

  trajectory->longitudinal_velocity_mps.range(trajectory->length() / 3.0, trajectory->length())
    .set(10.0);
  auto point = trajectory->compute(length / 2.0);

  EXPECT_LT(0, point.point.pose.position.x);
  EXPECT_LT(point.point.pose.position.x, 10);

  EXPECT_LT(0, point.point.pose.position.y);
  EXPECT_LT(point.point.pose.position.y, 10);

  EXPECT_EQ(1, point.lane_ids[0]);
}

TEST_F(TrajectoryTest, manipulate_velocity)
{
  trajectory->longitudinal_velocity_mps = 10.0;
  trajectory->longitudinal_velocity_mps
    .range(trajectory->length() / 3, 2.0 * trajectory->length() / 3)
    .set(5.0);
  auto point1 = trajectory->compute(0.0);
  auto point2 = trajectory->compute(trajectory->length() / 2.0);
  auto point3 = trajectory->compute(trajectory->length());

  EXPECT_EQ(10.0, point1.point.longitudinal_velocity_mps);
  EXPECT_EQ(5.0, point2.point.longitudinal_velocity_mps);
  EXPECT_EQ(10.0, point3.point.longitudinal_velocity_mps);
}

TEST_F(TrajectoryTest, direction)
{
  double dir = trajectory->azimuth(0.0);
  EXPECT_LT(0, dir);
  EXPECT_LT(dir, M_PI / 2);
}

TEST_F(TrajectoryTest, curvature)
{
  double curvature_val = trajectory->curvature(0.0);
  EXPECT_LT(-1.0, curvature_val);
  EXPECT_LT(curvature_val, 1.0);
}

TEST_F(TrajectoryTest, restore)
{
  using autoware::trajectory::Trajectory;
  trajectory->longitudinal_velocity_mps.range(4.0, trajectory->length()).set(5.0);
  {
    auto points = static_cast<Trajectory<geometry_msgs::msg::Point> &>(*trajectory).restore(0);
    EXPECT_EQ(10, points.size());
  }

  {
    auto points = static_cast<Trajectory<geometry_msgs::msg::Pose> &>(*trajectory).restore(0);
    EXPECT_EQ(10, points.size());
  }

  {
    auto points =
      static_cast<Trajectory<autoware_planning_msgs::msg::PathPoint> &>(*trajectory).restore(0);
    EXPECT_EQ(11, points.size());
  }

  {
    auto points = trajectory->restore(0);
    EXPECT_EQ(11, points.size());
  }
}

TEST_F(TrajectoryTest, crossed)
{
  geometry_msgs::msg::Pose pose1;
  pose1.position.x = 0.0;
  pose1.position.y = 10.0;
  geometry_msgs::msg::Pose pose2;
  pose2.position.x = 10.0;
  pose2.position.y = 0.0;

  auto crossed_point = trajectory->crossed(pose1, pose2);
  EXPECT_TRUE(crossed_point.has_value());

  EXPECT_LT(0.0, *crossed_point);
  EXPECT_LT(*crossed_point, trajectory->length());
}

TEST_F(TrajectoryTest, closest)
{
  geometry_msgs::msg::Pose pose;
  pose.position.x = 5.0;
  pose.position.y = 5.0;

  std::cerr << "Closest: " << trajectory->closest(pose) << std::endl;

  auto closest_pose = trajectory->compute(trajectory->closest(pose));

  double distance = std::hypot(
    closest_pose.point.pose.position.x - pose.position.x,
    closest_pose.point.pose.position.y - pose.position.y);

  EXPECT_LT(distance, 3.0);
}

TEST_F(TrajectoryTest, crop)
{
  double length = trajectory->length();

  auto start_point_expect = trajectory->compute(length / 3.0);
  auto end_point_expect = trajectory->compute(length / 3.0 + 1.0);

  trajectory->crop(length / 3.0, 1.0);

  EXPECT_EQ(trajectory->length(), 1.0);

  auto start_point_actual = trajectory->compute(0.0);
  auto end_point_actual = trajectory->compute(trajectory->length());

  EXPECT_EQ(start_point_expect.point.pose.position.x, start_point_actual.point.pose.position.x);
  EXPECT_EQ(start_point_expect.point.pose.position.y, start_point_actual.point.pose.position.y);
  EXPECT_EQ(start_point_expect.lane_ids[0], start_point_actual.lane_ids[0]);

  EXPECT_EQ(end_point_expect.point.pose.position.x, end_point_actual.point.pose.position.x);
  EXPECT_EQ(end_point_expect.point.pose.position.y, end_point_actual.point.pose.position.y);
  EXPECT_EQ(end_point_expect.lane_ids[0], end_point_actual.lane_ids[0]);
}

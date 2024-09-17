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

#include "autoware/lane_departure_checker/util.hpp"

#include <Eigen/Core>

#include <gtest/gtest.h>

using autoware_planning_msgs::msg::TrajectoryPoint;
using TrajectoryPoints = std::vector<TrajectoryPoint>;

TrajectoryPoints create_trajectory(const std::vector<Eigen::Vector3d> & points)
{
  TrajectoryPoints trajectory;
  for (const auto & point : points) {
    TrajectoryPoint p;
    p.pose.position.x = point.x();
    p.pose.position.y = point.y();
    p.pose.position.z = point.z();
    trajectory.push_back(p);
  }
  return trajectory;
}

class CutTrajectoryTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    trajectory_ = create_trajectory({{0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}});
    length_ = 1.5;
  }

  TrajectoryPoints trajectory_;
  double length_;
};

TEST_F(CutTrajectoryTest, test_cut_empty_trajectory)
{
  trajectory_ = create_trajectory({});
  const auto cut = autoware::lane_departure_checker::cutTrajectory(trajectory_, length_);
  EXPECT_TRUE(cut.empty());
}

TEST_F(CutTrajectoryTest, test_cut_single_point_trajectory)
{
  trajectory_ = create_trajectory({{0.0, 0.0, 0.0}});
  const auto cut = autoware::lane_departure_checker::cutTrajectory(trajectory_, length_);
  ASSERT_EQ(cut.size(), 1);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.x, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.z, 0.0);
}

TEST_F(CutTrajectoryTest, test_interpolation_at_viapoint)
{
  length_ = 1.0;
  const auto cut = autoware::lane_departure_checker::cutTrajectory(trajectory_, length_);
  ASSERT_EQ(cut.size(), 2);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.x, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.z, 0.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.x, 1.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.z, 0.0);
}

TEST_F(CutTrajectoryTest, test_interpolation_in_middle)
{
  const auto cut = autoware::lane_departure_checker::cutTrajectory(trajectory_, length_);
  ASSERT_EQ(cut.size(), 3);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.x, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.z, 0.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.x, 1.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.z, 0.0);
  EXPECT_DOUBLE_EQ(cut[2].pose.position.x, 1.5);
  EXPECT_DOUBLE_EQ(cut[2].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[2].pose.position.z, 0.0);
}

TEST_F(CutTrajectoryTest, test_no_interpolation)
{
  length_ = 3.0;
  const auto cut = autoware::lane_departure_checker::cutTrajectory(trajectory_, length_);
  ASSERT_EQ(cut.size(), 3);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.x, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[0].pose.position.z, 0.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.x, 1.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[1].pose.position.z, 0.0);
  EXPECT_DOUBLE_EQ(cut[2].pose.position.x, 2.0);
  EXPECT_DOUBLE_EQ(cut[2].pose.position.y, 0.0);
  EXPECT_DOUBLE_EQ(cut[2].pose.position.z, 0.0);
}

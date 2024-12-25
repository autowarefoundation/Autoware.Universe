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

#include "autoware/lane_departure_checker/utils.hpp"

#include <Eigen/Core>

#include <gtest/gtest.h>

#include <string>
#include <vector>

using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;

namespace
{
Trajectory create_trajectory(const std::vector<Eigen::Vector3d> & points)
{
  Trajectory trajectory;
  for (const auto & point : points) {
    TrajectoryPoint p;
    p.pose.position.x = point.x();
    p.pose.position.y = point.y();
    p.pose.position.z = point.z();
    trajectory.points.push_back(p);
  }
  return trajectory;
}
}  // namespace

struct ResampleTrajectoryTestParam
{
  std::string description;
  std::vector<Eigen::Vector3d> trajectory_points;
  double interval;
  std::vector<Eigen::Vector3d> expected_points;
};

std::ostream & operator<<(std::ostream & os, const ResampleTrajectoryTestParam & p)
{
  return os << p.description;
}

class ResampleTrajectoryTest : public ::testing::TestWithParam<ResampleTrajectoryTestParam>
{
};

TEST_P(ResampleTrajectoryTest, test_resample_trajectory)
{
  const auto p = GetParam();
  const auto trajectory = create_trajectory(p.trajectory_points);
  const auto resampled =
    autoware::lane_departure_checker::utils::resampleTrajectory(trajectory, p.interval);

  ASSERT_EQ(resampled.size(), p.expected_points.size());

  for (size_t i = 0; i < p.expected_points.size(); ++i) {
    EXPECT_DOUBLE_EQ(resampled[i].pose.position.x, p.expected_points[i].x());
    EXPECT_DOUBLE_EQ(resampled[i].pose.position.y, p.expected_points[i].y());
    EXPECT_DOUBLE_EQ(resampled[i].pose.position.z, p.expected_points[i].z());
  }
}

INSTANTIATE_TEST_SUITE_P(
  LaneDepartureCheckerTest, ResampleTrajectoryTest,
  ::testing::Values(
    ResampleTrajectoryTestParam{"EmptyTrajectory", {}, 1.0, {}},
    ResampleTrajectoryTestParam{"SinglePointTrajectory", {{1.0, 0.0, 0.0}}, 1.0, {{1.0, 0.0, 0.0}}},
    ResampleTrajectoryTestParam{
      "IntervalIsLessThanDistance",
      {{1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {3.0, 0.0, 0.0}},
      0.5,
      {{1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {3.0, 0.0, 0.0}}},
    ResampleTrajectoryTestParam{
      "IntervalIsEqualToDistance",
      {{1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {3.0, 0.0, 0.0}},
      1.0,
      {{1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {3.0, 0.0, 0.0}}},
    ResampleTrajectoryTestParam{
      "IntervalIsGreaterThanDistance",
      {{1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}, {3.0, 0.0, 0.0}},
      1.5,
      {{1.0, 0.0, 0.0}, {3.0, 0.0, 0.0}}}),
  ::testing::PrintToStringParamName());

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

using autoware::universe_utils::PoseDeviation;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;

namespace
{
Trajectory create_trajectory(const std::vector<Eigen::Vector2d> & points)
{
  Trajectory trajectory;
  for (const auto & point : points) {
    TrajectoryPoint p;
    p.pose.position.x = point.x();
    p.pose.position.y = point.y();
    trajectory.points.push_back(p);
  }
  return trajectory;
}

geometry_msgs::msg::Pose create_pose(const Eigen::Vector3d & x_y_yaw)
{
  geometry_msgs::msg::Pose pose;
  pose.position.x = x_y_yaw[0];
  pose.position.y = x_y_yaw[1];
  pose.orientation.z = std::sin(x_y_yaw[2] / 2);
  pose.orientation.w = std::cos(x_y_yaw[2] / 2);
  return pose;
}

constexpr double ego_nearest_dist_threshold = 3.0;
constexpr double ego_nearest_yaw_threshold = 1.046;
}  // namespace

struct CalcTrajectoryDeviationTestParam
{
  std::string description;
  std::vector<Eigen::Vector2d> trajectory_points;
  Eigen::Vector3d x_y_yaw;
  bool exception_expected;
  PoseDeviation expected_deviation;
};

std::ostream & operator<<(std::ostream & os, const CalcTrajectoryDeviationTestParam & p)
{
  return os << p.description;
}

class CalcTrajectoryDeviationTest
: public ::testing::TestWithParam<CalcTrajectoryDeviationTestParam>
{
};

TEST_P(CalcTrajectoryDeviationTest, test_calc_trajectory_deviation)
{
  const auto p = GetParam();
  const auto trajectory = create_trajectory(p.trajectory_points);
  const auto pose = create_pose(p.x_y_yaw);
  if (p.exception_expected) {
    EXPECT_ANY_THROW({
      autoware::lane_departure_checker::utils::calcTrajectoryDeviation(
        trajectory, pose, ego_nearest_dist_threshold, ego_nearest_yaw_threshold);
    });
  } else {
    const auto deviation = autoware::lane_departure_checker::utils::calcTrajectoryDeviation(
      trajectory, pose, ego_nearest_dist_threshold, ego_nearest_yaw_threshold);

    EXPECT_DOUBLE_EQ(deviation.lateral, p.expected_deviation.lateral);
    EXPECT_DOUBLE_EQ(deviation.longitudinal, p.expected_deviation.longitudinal);
    EXPECT_DOUBLE_EQ(deviation.yaw, p.expected_deviation.yaw);
  }
}

INSTANTIATE_TEST_SUITE_P(
  LaneDepartureCheckerTest, CalcTrajectoryDeviationTest,
  ::testing::Values(
    CalcTrajectoryDeviationTestParam{"EmptyTrajectory", {}, {}, true, {}},
    CalcTrajectoryDeviationTestParam{
      "SinglePointTrajectory", {{0.0, 0.0}}, {0.0, 0.0, 0.0}, false, {0.0, 0.0, 0.0}}),
  ::testing::PrintToStringParamName());

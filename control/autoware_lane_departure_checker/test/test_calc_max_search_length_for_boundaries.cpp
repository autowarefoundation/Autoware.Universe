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

// reference:
// https://github.com/autowarefoundation/sample_vehicle_launch/blob/main/sample_vehicle_description/config/vehicle_info.param.yaml
constexpr double wheel_radius_m = 0.383;
constexpr double wheel_width_m = 0.235;
constexpr double wheel_base_m = 2.79;
constexpr double wheel_tread_m = 1.64;
constexpr double front_overhang_m = 1.0;
constexpr double rear_overhang_m = 1.1;
constexpr double left_overhang_m = 0.128;
constexpr double right_overhang_m = 0.128;
constexpr double vehicle_height_m = 2.5;
constexpr double max_steer_angle_rad = 0.70;
}  // namespace

struct CalcMaxSearchLengthForBoundariesParam
{
  std::string description;
  std::vector<Eigen::Vector2d> trajectory_points;
  double expected_max_search_length;
};

std::ostream & operator<<(std::ostream & os, const CalcMaxSearchLengthForBoundariesParam & p)
{
  return os << p.description;
}

class CalcMaxSearchLengthForBoundariesTest
: public ::testing::TestWithParam<CalcMaxSearchLengthForBoundariesParam>
{
protected:
  void SetUp() override
  {
    vehicle_info = autoware::vehicle_info_utils::createVehicleInfo(
      wheel_radius_m, wheel_width_m, wheel_base_m, wheel_tread_m, front_overhang_m, rear_overhang_m,
      left_overhang_m, right_overhang_m, vehicle_height_m, max_steer_angle_rad);
  }

  autoware::vehicle_info_utils::VehicleInfo vehicle_info;
};

TEST_P(CalcMaxSearchLengthForBoundariesTest, test_calc_max_search_length_for_boundaries)
{
  const auto p = GetParam();
  const auto trajectory = create_trajectory(p.trajectory_points);

  const auto max_search_length =
    autoware::lane_departure_checker::utils::calcMaxSearchLengthForBoundaries(
      trajectory, vehicle_info);

  EXPECT_DOUBLE_EQ(max_search_length, p.expected_max_search_length);
}

INSTANTIATE_TEST_SUITE_P(
  LaneDepartureCheckerTest, CalcMaxSearchLengthForBoundariesTest,
  ::testing::Values(
    CalcMaxSearchLengthForBoundariesParam{
      "EmptyTrajectory",
      {},
      std::hypot(front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m)},
    CalcMaxSearchLengthForBoundariesParam{
      "SinglePointTrajectory",
      {{0.0, 0.0}},
      std::hypot(front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m)},
    CalcMaxSearchLengthForBoundariesParam{
      "MultiPointTrajectory",
      {{0.0, 0.0}, {1.0, 0.0}},
      1.0 + std::hypot(front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m)}),
  ::testing::PrintToStringParamName());

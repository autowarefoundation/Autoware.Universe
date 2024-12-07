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

using autoware::universe_utils::LinearRing2d;
using autoware_planning_msgs::msg::TrajectoryPoint;
using geometry_msgs::msg::PoseWithCovariance;
using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;
using TrajectoryPoints = std::vector<TrajectoryPoint>;

namespace
{
PoseWithCovariance create_pose_with_covariance(
  const Eigen::Matrix2d & covariance_xy, const double yaw)
{
  PoseWithCovariance pose_with_covariance;
  pose_with_covariance.covariance[0 * 6 + 0] = covariance_xy(0, 0);
  pose_with_covariance.covariance[0 * 6 + 1] = covariance_xy(0, 1);
  pose_with_covariance.covariance[1 * 6 + 0] = covariance_xy(1, 0);
  pose_with_covariance.covariance[1 * 6 + 1] = covariance_xy(1, 1);
  pose_with_covariance.pose.orientation.z = std::sin(yaw / 2);
  pose_with_covariance.pose.orientation.w = std::cos(yaw / 2);
  return pose_with_covariance;
}

TrajectoryPoints create_trajectory_points(
  const std::vector<std::pair<Eigen::Vector2d, double>> & xy_yaws)
{
  TrajectoryPoints trajectory_points;
  for (const auto & [xy, yaw] : xy_yaws) {
    TrajectoryPoint p;
    p.pose.position.x = xy(0);
    p.pose.position.y = xy(1);
    p.pose.orientation.z = std::sin(yaw / 2);
    p.pose.orientation.w = std::cos(yaw / 2);
    trajectory_points.push_back(p);
  }
  return trajectory_points;
}

PathWithLaneId create_path(const std::vector<std::pair<Eigen::Vector2d, double>> & xy_yaws)
{
  PathWithLaneId path;
  for (const auto & [xy, yaw] : xy_yaws) {
    PathPointWithLaneId p;
    p.point.pose.position.x = xy(0);
    p.point.pose.position.y = xy(1);
    p.point.pose.orientation.z = std::sin(yaw / 2);
    p.point.pose.orientation.w = std::cos(yaw / 2);
    path.points.push_back(p);
  }
  return path;
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

struct CreateVehicleFootprintsAlongTrajectoryParam
{
  std::string description;
  Eigen::Matrix2d covariance_xy;
  double yaw;
  std::vector<std::pair<Eigen::Vector2d, double>> trajectory_points;
  double footprint_margin_scale;
  std::vector<LinearRing2d> expected_footprints;
};

std::ostream & operator<<(std::ostream & os, const CreateVehicleFootprintsAlongTrajectoryParam & p)
{
  return os << p.description;
}

struct CreateVehicleFootprintsAlongPathParam
{
  std::string description;
  std::vector<std::pair<Eigen::Vector2d, double>> path_points;
  double footprint_extra_margin;
  std::vector<LinearRing2d> expected_footprints;
};

std::ostream & operator<<(std::ostream & os, const CreateVehicleFootprintsAlongPathParam & p)
{
  return os << p.description;
}

class CreateVehicleFootprintsAlongTrajectoryTest
: public ::testing::TestWithParam<CreateVehicleFootprintsAlongTrajectoryParam>
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

class CreateVehicleFootprintsAlongPathTest
: public ::testing::TestWithParam<CreateVehicleFootprintsAlongPathParam>
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

TEST_P(CreateVehicleFootprintsAlongTrajectoryTest, test_create_vehicle_footprints)
{
  const auto p = GetParam();
  const auto pose_with_covariance = create_pose_with_covariance(p.covariance_xy, p.yaw);
  const auto trajectory_points = create_trajectory_points(p.trajectory_points);
  const auto footprints = autoware::lane_departure_checker::utils::createVehicleFootprints(
    pose_with_covariance, trajectory_points, vehicle_info, p.footprint_margin_scale);

  ASSERT_EQ(footprints.size(), p.expected_footprints.size());
  for (size_t i = 0; i < footprints.size(); ++i) {
    const auto & footprint = footprints.at(i);
    const auto & expected_footprint = p.expected_footprints.at(i);
    ASSERT_EQ(footprint.size(), expected_footprint.size());
    for (size_t j = 0; j < footprint.size(); ++j) {
      EXPECT_DOUBLE_EQ(footprint.at(j).x(), expected_footprint.at(j).x());
      EXPECT_DOUBLE_EQ(footprint.at(j).y(), expected_footprint.at(j).y());
    }
  }
}

TEST_P(CreateVehicleFootprintsAlongPathTest, test_create_vehicle_footprints)
{
  const auto p = GetParam();
  const auto path = create_path(p.path_points);
  const auto footprints = autoware::lane_departure_checker::utils::createVehicleFootprints(
    path, vehicle_info, p.footprint_extra_margin);

  ASSERT_EQ(footprints.size(), p.expected_footprints.size());
  for (size_t i = 0; i < footprints.size(); ++i) {
    const auto & footprint = footprints.at(i);
    const auto & expected_footprint = p.expected_footprints.at(i);
    ASSERT_EQ(footprint.size(), expected_footprint.size());
    for (size_t j = 0; j < footprint.size(); ++j) {
      EXPECT_DOUBLE_EQ(footprint.at(j).x(), expected_footprint.at(j).x());
      EXPECT_DOUBLE_EQ(footprint.at(j).y(), expected_footprint.at(j).y());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  LaneDepartureCheckerTest, CreateVehicleFootprintsAlongTrajectoryTest,
  ::testing::Values(
    CreateVehicleFootprintsAlongTrajectoryParam{
      "EmptyTrajectory", Eigen::Matrix2d{{0.0, 0.0}, {0.0, 0.0}}, 0.0, {}, 0.0, {}},
    CreateVehicleFootprintsAlongTrajectoryParam{
      "SinglePointTrajectory",
      Eigen::Matrix2d{{0.0, 0.0}, {0.0, 0.0}},
      0.0,
      {{{0.0, 0.0}, 0.0}},
      0.0,
      {{{front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m},
        {front_overhang_m + wheel_base_m, -(wheel_tread_m / 2.0 + right_overhang_m)},
        {wheel_base_m / 2.0, -(wheel_tread_m / 2.0 + right_overhang_m)},
        {-rear_overhang_m, -(wheel_tread_m / 2.0 + right_overhang_m)},
        {-rear_overhang_m, wheel_tread_m / 2.0 + left_overhang_m},
        {wheel_base_m / 2.0, wheel_tread_m / 2.0 + left_overhang_m},
        {front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m}}}},
    CreateVehicleFootprintsAlongTrajectoryParam{
      "NonZeroMargin",
      Eigen::Matrix2d{{0.1, 0.0}, {0.0, 0.2}},
      0.0,
      {{{0.0, 0.0}, 0.0}, {{1.0, 0.0}, 0.0}},
      1.0,
      {{{front_overhang_m + wheel_base_m + 0.1, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {wheel_base_m / 2.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1), -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1), wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {wheel_base_m / 2.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1, wheel_tread_m / 2.0 + left_overhang_m + 0.2}},
       {{front_overhang_m + wheel_base_m + 0.1 + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1 + 1.0,
         -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {wheel_base_m / 2.0 + 1.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1) + 1.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1) + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {wheel_base_m / 2.0 + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1 + 1.0,
         wheel_tread_m / 2.0 + left_overhang_m + 0.2}}}},
    CreateVehicleFootprintsAlongTrajectoryParam{
      "NonZeroYaw",
      Eigen::Matrix2d{{0.2, 0.0}, {0.0, 0.1}},
      M_PI_2,
      {{{0.0, 0.0}, 0.0}, {{1.0, 0.0}, 0.0}},
      1.0,
      {{{front_overhang_m + wheel_base_m + 0.1, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {wheel_base_m / 2.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1), -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1), wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {wheel_base_m / 2.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1, wheel_tread_m / 2.0 + left_overhang_m + 0.2}},
       {{front_overhang_m + wheel_base_m + 0.1 + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1 + 1.0,
         -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {wheel_base_m / 2.0 + 1.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1) + 1.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.2)},
        {-(rear_overhang_m + 0.1) + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {wheel_base_m / 2.0 + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.2},
        {front_overhang_m + wheel_base_m + 0.1 + 1.0,
         wheel_tread_m / 2.0 + left_overhang_m + 0.2}}}}),
  ::testing::PrintToStringParamName());

INSTANTIATE_TEST_SUITE_P(
  LaneDepartureCheckerTest, CreateVehicleFootprintsAlongPathTest,
  ::testing::Values(
    CreateVehicleFootprintsAlongPathParam{"EmptyTrajectory", {}, 0.0, {}},
    CreateVehicleFootprintsAlongPathParam{
      "SinglePointTrajectory",
      {{{0.0, 0.0}, 0.0}},
      0.0,
      {{{front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m},
        {front_overhang_m + wheel_base_m, -(wheel_tread_m / 2.0 + right_overhang_m)},
        {wheel_base_m / 2.0, -(wheel_tread_m / 2.0 + right_overhang_m)},
        {-rear_overhang_m, -(wheel_tread_m / 2.0 + right_overhang_m)},
        {-rear_overhang_m, wheel_tread_m / 2.0 + left_overhang_m},
        {wheel_base_m / 2.0, wheel_tread_m / 2.0 + left_overhang_m},
        {front_overhang_m + wheel_base_m, wheel_tread_m / 2.0 + left_overhang_m}}}},
    CreateVehicleFootprintsAlongPathParam{
      "NonZeroMargin",
      {{{0.0, 0.0}, 0.0}, {{1.0, 0.0}, 0.0}},
      0.1,
      {{{front_overhang_m + wheel_base_m + 0.1, wheel_tread_m / 2.0 + left_overhang_m + 0.1},
        {front_overhang_m + wheel_base_m + 0.1, -(wheel_tread_m / 2.0 + right_overhang_m + 0.1)},
        {wheel_base_m / 2.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.1)},
        {-(rear_overhang_m + 0.1), -(wheel_tread_m / 2.0 + right_overhang_m + 0.1)},
        {-(rear_overhang_m + 0.1), wheel_tread_m / 2.0 + left_overhang_m + 0.1},
        {wheel_base_m / 2.0, wheel_tread_m / 2.0 + left_overhang_m + 0.1},
        {front_overhang_m + wheel_base_m + 0.1, wheel_tread_m / 2.0 + left_overhang_m + 0.1}},
       {{front_overhang_m + wheel_base_m + 0.1 + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.1},
        {front_overhang_m + wheel_base_m + 0.1 + 1.0,
         -(wheel_tread_m / 2.0 + right_overhang_m + 0.1)},
        {wheel_base_m / 2.0 + 1.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.1)},
        {-(rear_overhang_m + 0.1) + 1.0, -(wheel_tread_m / 2.0 + right_overhang_m + 0.1)},
        {-(rear_overhang_m + 0.1) + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.1},
        {wheel_base_m / 2.0 + 1.0, wheel_tread_m / 2.0 + left_overhang_m + 0.1},
        {front_overhang_m + wheel_base_m + 0.1 + 1.0,
         wheel_tread_m / 2.0 + left_overhang_m + 0.1}}}}),
  ::testing::PrintToStringParamName());

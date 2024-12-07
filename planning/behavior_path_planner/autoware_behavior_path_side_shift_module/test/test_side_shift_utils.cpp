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
#include "autoware/behavior_path_side_shift_module/utils.hpp"

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <gtest/gtest.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>

#include <vector>

namespace autoware::behavior_path_planner
{

class SideShiftUtilsTest : public ::testing::Test
{
protected:
  PathWithLaneId generateStraightPath(const double length, const double width)
  {
    PathWithLaneId path;
    const double interval = 1.0;
    const size_t point_num = static_cast<size_t>(length / interval);

    for (size_t i = 0; i < point_num; ++i) {
      PathPointWithLaneId p;
      p.point.pose.position.x = i * interval;
      p.point.pose.position.y = width;
      p.point.pose.position.z = 0.0;

      tf2::Quaternion q;
      q.setRPY(0, 0, 0);
      p.point.pose.orientation.x = q.x();
      p.point.pose.orientation.y = q.y();
      p.point.pose.orientation.z = q.z();
      p.point.pose.orientation.w = q.w();

      path.points.push_back(p);
    }
    return path;
  }

  ShiftedPath generateShiftedPath(const double length, const std::vector<double> & shifts)
  {
    ShiftedPath shifted_path;
    shifted_path.path = generateStraightPath(length, 0.0);
    shifted_path.shift_length = shifts;
    return shifted_path;
  }
};

TEST_F(SideShiftUtilsTest, SetOrientationStraightPath)
{
  // Generate straight path
  auto path = generateStraightPath(10.0, 0.0);

  // Set orientation
  setOrientation(&path);

  // Check orientation for each point
  for (const auto & p : path.points) {
    double yaw = tf2::getYaw(p.point.pose.orientation);
    EXPECT_NEAR(yaw, 0.0, 1e-6);  // Should be facing forward (0 rad)
  }
}

TEST_F(SideShiftUtilsTest, SetOrientationCurvedPath)
{
  PathWithLaneId path;

  // Create a 90-degree turn path
  PathPointWithLaneId p1, p2, p3;

  p1.point.pose.position.x = 0.0;
  p1.point.pose.position.y = 0.0;
  p1.point.pose.position.z = 0.0;

  p2.point.pose.position.x = 1.0;
  p2.point.pose.position.y = 0.0;
  p2.point.pose.position.z = 0.0;

  p3.point.pose.position.x = 1.0;
  p3.point.pose.position.y = 1.0;
  p3.point.pose.position.z = 0.0;

  path.points = {p1, p2, p3};

  setOrientation(&path);

  // First segment should face east (0 rad)
  EXPECT_NEAR(tf2::getYaw(path.points[0].point.pose.orientation), 0.0, 1e-6);

  // Last segment should face north (π/2 rad)
  EXPECT_NEAR(tf2::getYaw(path.points[2].point.pose.orientation), M_PI_2, 1e-6);
}

TEST_F(SideShiftUtilsTest, GetClosestShiftLengthEmptyPath)
{
  ShiftedPath empty_path;
  geometry_msgs::msg::Point ego_point;
  ego_point.x = 0.0;
  ego_point.y = 0.0;

  double shift = getClosestShiftLength(empty_path, ego_point);
  EXPECT_DOUBLE_EQ(shift, 0.0);
}

TEST_F(SideShiftUtilsTest, GetClosestShiftLengthStraightPath)
{
  // Generate path with constant shift
  std::vector<double> shifts(10, 2.0);  // 10 points with 2.0m shift
  auto shifted_path = generateShiftedPath(10.0, shifts);

  // Test points at different positions
  geometry_msgs::msg::Point ego_point;

  // Point at start
  ego_point.x = 0.0;
  ego_point.y = 0.0;
  EXPECT_DOUBLE_EQ(getClosestShiftLength(shifted_path, ego_point), 2.0);

  // Point in middle
  ego_point.x = 5.0;
  ego_point.y = 0.0;
  EXPECT_DOUBLE_EQ(getClosestShiftLength(shifted_path, ego_point), 2.0);

  // Point at end
  ego_point.x = 9.0;
  ego_point.y = 0.0;
  EXPECT_DOUBLE_EQ(getClosestShiftLength(shifted_path, ego_point), 2.0);
}

}  // namespace autoware::behavior_path_planner

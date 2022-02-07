// Copyright 2021 Tier IV, Inc.
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

#include "utils.hpp"

#include <scene_module/occlusion_spot/occlusion_spot_utils.hpp>
#include <scene_module/occlusion_spot/risk_predictive_braking.hpp>

#include <gtest/gtest.h>

#include <limits>

TEST(compareSafeVelocity, min_max)
{
  using behavior_velocity_planner::occlusion_spot_utils::compareSafeVelocity;
  const double inf = std::numeric_limits<double>::max();
  // upper bound rpb_vel
  ASSERT_EQ(compareSafeVelocity(inf, inf, inf, inf), inf);
  // lower bound org_vel = 0
  ASSERT_EQ(compareSafeVelocity(inf, inf, inf, 0), 0.0);
  // lower bound min = 0
  ASSERT_EQ(compareSafeVelocity(inf, inf, 0, inf), inf);
}

TEST(insertSafeVelocityToPath, replace_original_at_too_close_case)
{
  /* straight path
    0 1 2 3 4
  0 x x z x x
    --->
  straight path
     0 1 2 3 4
  0  x x r xox
  */
  using behavior_velocity_planner::occlusion_spot_utils::insertSafeVelocityToPath;
  const int num_path_point = 5;
  const double inf = std::numeric_limits<double>::max();
  behavior_velocity_planner::occlusion_spot_utils::PlannerParam param;
  param.angle_thr = 1.0;
  param.dist_thr = 10.0;
  autoware_auto_planning_msgs::msg::PathWithLaneId path =
    test::generatePath(0.0, 0.0, static_cast<double>(num_path_point - 1), 0.0, num_path_point);
  geometry_msgs::msg::Pose pose{};
  pose.position.x = 2;
  double safe_vel = inf;
  // insert x=2 new point -> replace original
  insertSafeVelocityToPath(pose, safe_vel, param, &path);
  ASSERT_EQ(path.points.size(), static_cast<size_t>(num_path_point));
  pose.position.x = 3.4;
  insertSafeVelocityToPath(pose, safe_vel, param, &path);
  ASSERT_EQ(path.points.size(), static_cast<size_t>(num_path_point + 1));
}

TEST(insertSafeVelocityToPath, dont_insert_last_point)
{
  /* straight path
      0 1 2 3 4
    0 x x z x xo
    --->
    straight path
      0 1 2 3 4
    0 x x x x x
  */
  using behavior_velocity_planner::occlusion_spot_utils::insertSafeVelocityToPath;
  const int num_path = 5;
  geometry_msgs::msg::Pose pose{};
  pose.position.x = 5;
  pose.position.y = 0;
  pose.position.z = 0;
  double safe_vel = 10;
  behavior_velocity_planner::occlusion_spot_utils::PlannerParam param;
  param.angle_thr = 1.0;
  param.dist_thr = 10.0;
  autoware_auto_planning_msgs::msg::PathWithLaneId path =
    test::generatePath(0.0, 0.0, static_cast<double>(num_path - 1), 0.0, num_path);
  ASSERT_EQ(insertSafeVelocityToPath(pose, safe_vel, param, &path), -1);
  ASSERT_EQ(path.points.size(), static_cast<size_t>(num_path));
}

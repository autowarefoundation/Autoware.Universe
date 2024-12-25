// Copyright 2022-2024 Tier IV, Inc. All rights reserved.
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

#include "../src/obstacle_collision_checker.cpp"  // NOLINT
#include "gtest/gtest.h"

#include <autoware/universe_utils/geometry/geometry.hpp>

#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <autoware_planning_msgs/msg/trajectory_point.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <gtest/gtest-death-test.h>
#include <pcl/point_cloud.h>

#include <limits>
#include <memory>
#include <vector>

namespace
{
pcl::PointXYZ pcl_point(const float x, const float y)
{
  pcl::PointXYZ p;
  p.x = x;
  p.y = y;
  p.z = 0.0;
  return p;
}

pcl::PointCloud<pcl::PointXYZ> pcl_pointcloud(const std::vector<std::pair<float, float>> & points)
{
  pcl::PointCloud<pcl::PointXYZ> pcl;
  for (const auto & p : points) {
    pcl.push_back(pcl_point(p.first, p.second));
  }
  return pcl;
}

bool point_in_pcl_pointcloud(const pcl::PointXYZ & pt, const pcl::PointCloud<pcl::PointXYZ> & pcd)
{
  for (const auto & p : pcd) {
    if (p.x == pt.x && p.y == pt.y && p.z == pt.z) {
      return true;
    }
  }
  return false;
}
}  // namespace

TEST(test_obstacle_collision_checker, filterPointCloudByTrajectory)
{
  pcl::PointCloud<pcl::PointXYZ> pcl;
  autoware_planning_msgs::msg::Trajectory trajectory;
  pcl::PointXYZ pcl_point;
  autoware_planning_msgs::msg::TrajectoryPoint traj_point;
  pcl_point.y = 0.0;
  traj_point.pose.position.y = 0.99;
  for (float x = 0.0; x < 10.0; x += 1.0) {
    pcl_point.x = x;
    traj_point.pose.position.x = x;
    trajectory.points.push_back(traj_point);
    pcl.push_back(pcl_point);
  }
  // radius < 1: all points are filtered
  for (auto radius = 0.0; radius <= 0.99; radius += 0.1) {
    const auto filtered_pcl = filter_point_cloud_by_trajectory(pcl, trajectory, radius);
    EXPECT_EQ(filtered_pcl.size(), 0ul);
  }
  // radius >= 1.0: all points are kept
  for (auto radius = 1.0; radius < 10.0; radius += 0.1) {
    const auto filtered_pcl = filter_point_cloud_by_trajectory(pcl, trajectory, radius);
    ASSERT_EQ(pcl.size(), filtered_pcl.size());
    for (size_t i = 0; i < pcl.size(); ++i) {
      EXPECT_EQ(pcl[i].x, filtered_pcl[i].x);
      EXPECT_EQ(pcl[i].y, filtered_pcl[i].y);
    }
  }
}

TEST(test_obstacle_collision_checker, getTransformedPointCloud)
{
  sensor_msgs::msg::PointCloud2 pcd_msg;
  const auto pcl_pcd = pcl_pointcloud({
    {0.0, 0.0},
    {1.0, 1.0},
    {-2.0, 3.0},
  });
  pcl::toROSMsg(pcl_pcd, pcd_msg);

  {  // empty transform, expect same points
    geometry_msgs::msg::Transform transform;
    const auto transformed_pcd = get_transformed_point_cloud(pcd_msg, transform);
    EXPECT_EQ(pcl_pcd.size(), transformed_pcd.size());
    for (const auto & p : transformed_pcd.points) {
      EXPECT_TRUE(point_in_pcl_pointcloud(p, pcl_pcd));
    }
  }

  {  // translation
    geometry_msgs::msg::Transform transform;
    transform.translation.x = 2.0;
    transform.translation.y = 1.5;
    const auto transformed_pcd = get_transformed_point_cloud(pcd_msg, transform);
    EXPECT_EQ(pcl_pcd.size(), transformed_pcd.size());
    for (const auto & p : transformed_pcd.points) {
      auto transformed_p = p;
      transformed_p.x -= static_cast<float>(transform.translation.x);
      transformed_p.y -= static_cast<float>(transform.translation.y);
      EXPECT_TRUE(point_in_pcl_pointcloud(transformed_p, pcl_pcd));
    }
  }
  {  // rotation
    geometry_msgs::msg::Transform transform;
    transform.rotation = autoware::universe_utils::createQuaternionFromRPY(0.0, 0.0, M_PI);
    const auto transformed_pcd = get_transformed_point_cloud(pcd_msg, transform);
    EXPECT_EQ(pcl_pcd.size(), transformed_pcd.size());
    EXPECT_TRUE(point_in_pcl_pointcloud(pcl_point(0.0, 0.0), transformed_pcd));
    EXPECT_TRUE(point_in_pcl_pointcloud(pcl_point(-1.0, -1.0), transformed_pcd));
    EXPECT_TRUE(point_in_pcl_pointcloud(pcl_point(2.0, -3.0), transformed_pcd));
  }
  {  // translation + rotation
    geometry_msgs::msg::Transform transform;
    transform.translation.x = 0.5;
    transform.translation.y = -0.5;
    transform.rotation = autoware::universe_utils::createQuaternionFromRPY(0.0, 0.0, M_PI);
    const auto transformed_pcd = get_transformed_point_cloud(pcd_msg, transform);
    EXPECT_EQ(pcl_pcd.size(), transformed_pcd.size());
    EXPECT_TRUE(point_in_pcl_pointcloud(pcl_point(0.5, -0.5), transformed_pcd));
    EXPECT_TRUE(point_in_pcl_pointcloud(pcl_point(-0.5, -1.5), transformed_pcd));
    EXPECT_TRUE(point_in_pcl_pointcloud(pcl_point(2.5, -3.5), transformed_pcd));
  }
}

TEST(test_obstacle_collision_checker, calcBrakingDistance)
{
  EXPECT_TRUE(std::isnan(calc_braking_distance(0.0, 0.0, 0.0)));
  // if we cannot decelerate (max_decel = 0.0), then the result is infinity
  EXPECT_DOUBLE_EQ(calc_braking_distance(1.0, 0.0, 1.0), std::numeric_limits<double>::infinity());
  EXPECT_DOUBLE_EQ(
    calc_braking_distance(1.0, 1.0, 1.0),
    // 1s * 1m/s = 1m for the delay, then 1->0m/s at 1m/s² = 0.5m
    1.0 + 0.5);
}

TEST(test_obstacle_collision_checker, check_for_collisions)
{
  autoware::obstacle_collision_checker::Input input;
  // call with empty input causes a segfault
  // const auto output = check_for_collisions(input);
  input.param.delay_time = 1.0;
  input.param.footprint_margin = 0.0;
  input.param.max_deceleration = 1.0;
  input.param.resample_interval = 1.0;
  input.param.search_radius = 10.0;
  geometry_msgs::msg::PoseStamped ego_pose;  // default (0,0) ego pose
  geometry_msgs::msg::TransformStamped tf;   // (0,0) transform
  tf.transform.rotation.w = 1.0;
  input.current_pose = std::make_shared<const geometry_msgs::msg::PoseStamped>(ego_pose);
  input.obstacle_transform = std::make_shared<const geometry_msgs::msg::TransformStamped>(tf);
  // 2mx2m footprint
  input.vehicle_info.front_overhang_m = 1.0;
  input.vehicle_info.wheel_base_m = 0.0;
  input.vehicle_info.rear_overhang_m = 1.0;
  input.vehicle_info.left_overhang_m = 1.0;
  input.vehicle_info.right_overhang_m = 1.0;
  autoware_planning_msgs::msg::Trajectory trajectory;
  autoware_planning_msgs::msg::TrajectoryPoint point;
  point.pose.position.y = 0.0;
  for (auto x = 0.0; x < 6.0; x += 0.1) {
    point.pose.position.x = x;
    trajectory.points.push_back(point);
  }
  input.predicted_trajectory =
    std::make_shared<const autoware_planning_msgs::msg::Trajectory>(trajectory);
  input.reference_trajectory = {};  // TODO(someone): the reference trajectory is not used
  {
    // obstacle point on the trajectory
    sensor_msgs::msg::PointCloud2 pcd_msg;
    const auto pcl_pcd = pcl_pointcloud({
      {5.0, 0.0},
    });
    pcl::toROSMsg(pcl_pcd, pcd_msg);
    input.obstacle_pointcloud = std::make_shared<const sensor_msgs::msg::PointCloud2>(pcd_msg);
    geometry_msgs::msg::Twist twist;  // no velocity -> no collision
    twist.linear.x = 0.0;
    input.current_twist = std::make_shared<const geometry_msgs::msg::Twist>(twist);
    const auto output = check_for_collisions(input);
    EXPECT_FALSE(output.will_collide);
    // zero velocity: only the 1st point of the trajectory is kept
    EXPECT_EQ(output.resampled_trajectory.points.size(), 1UL);
  }
  {
    // moderate velocity: short braking distance so the trajectory is cut before the collision
    geometry_msgs::msg::Twist twist;
    twist.linear.x = 1.0;
    input.current_twist = std::make_shared<const geometry_msgs::msg::Twist>(twist);
    const auto output = check_for_collisions(input);
    EXPECT_FALSE(output.will_collide);
    // 1s * 1m/s = 1m for the delay, then 1->0m/s at 1m/s² = 0.5m -> 1.5m braking distance
    EXPECT_DOUBLE_EQ(
      autoware::universe_utils::calcDistance2d(
        output.resampled_trajectory.points.front(), output.resampled_trajectory.points.back()),
      1.5);
  }
  {
    // high velocity -> collision
    geometry_msgs::msg::Twist twist;
    twist.linear.x = 10.0;
    input.current_twist = std::make_shared<const geometry_msgs::msg::Twist>(twist);
    const auto output = check_for_collisions(input);
    EXPECT_TRUE(output.will_collide);
    // high velocity: the full trajectory is resampled (original interval = 0.1, resample interval
    // = 1.0)
    EXPECT_EQ(
      output.resampled_trajectory.points.size(),
      input.predicted_trajectory->points.size() / 10 + 1);
  }
  {
    // obstacle point on the side of the trajectory but inside the ego footprint -> collision
    sensor_msgs::msg::PointCloud2 pcd_msg;
    const auto pcl_pcd = pcl_pointcloud({
      {5.0, 0.5},
    });
    pcl::toROSMsg(pcl_pcd, pcd_msg);
    input.obstacle_pointcloud = std::make_shared<const sensor_msgs::msg::PointCloud2>(pcd_msg);
    const auto output = check_for_collisions(input);
    EXPECT_TRUE(output.will_collide);
  }
  {
    // obstacle point on the side of the trajectory and outside the ego footprint -> no collision
    sensor_msgs::msg::PointCloud2 pcd_msg;
    const auto pcl_pcd = pcl_pointcloud({
      {5.0, 1.5},
    });
    pcl::toROSMsg(pcl_pcd, pcd_msg);
    input.obstacle_pointcloud = std::make_shared<const sensor_msgs::msg::PointCloud2>(pcd_msg);
    const auto output = check_for_collisions(input);
    EXPECT_FALSE(output.will_collide);
  }
}

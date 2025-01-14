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

#include "debug.hpp"
#include "dynamic_obstacle.hpp"
#include "path_utils.hpp"
#include "scene.hpp"
#include "utils.hpp"

#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/math/normalization.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/time.hpp>

#include <autoware_perception_msgs/msg/detail/object_classification__struct.hpp>
#include <geometry_msgs/msg/detail/point__struct.hpp>
#include <tier4_planning_msgs/msg/detail/path_point_with_lane_id__struct.hpp>
#include <tier4_planning_msgs/msg/detail/path_with_lane_id__struct.hpp>

#include <Eigen/src/Core/Matrix.h>
#include <gtest/gtest.h>
#include <pcl_conversions/pcl_conversions.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>
#include <vector>

using autoware::universe_utils::Point2d;
using autoware::universe_utils::Polygon2d;
using autoware_perception_msgs::msg::ObjectClassification;
using geometry_msgs::msg::Point;
using Polygons2d = std::vector<Polygon2d>;

using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;
using PathPointsWithLaneId = std::vector<tier4_planning_msgs::msg::PathPointWithLaneId>;

using autoware::behavior_velocity_planner::applyVoxelGridFilter;
using autoware::behavior_velocity_planner::createPredictedPath;
using autoware::behavior_velocity_planner::createQuaternionFacingToTrajectory;
using autoware::behavior_velocity_planner::extractLateralNearestPoints;
using autoware::behavior_velocity_planner::extractObstaclePointsWithinPolygon;
using autoware::behavior_velocity_planner::groupPointsWithNearestSegmentIndex;

using autoware::behavior_velocity_planner::calculateLateralNearestPoint;
using autoware::behavior_velocity_planner::calculateMinAndMaxVelFromCovariance;
using autoware::behavior_velocity_planner::concatPointCloud;
using autoware::behavior_velocity_planner::convertDurationToDouble;
using autoware::behavior_velocity_planner::createPathToPredictionTime;
using autoware::behavior_velocity_planner::DynamicObstacle;
using autoware::behavior_velocity_planner::DynamicObstacleCreatorForObject;
using autoware::behavior_velocity_planner::DynamicObstacleCreatorForObjectWithoutPath;
using autoware::behavior_velocity_planner::DynamicObstacleCreatorForPoints;
using autoware::behavior_velocity_planner::DynamicObstacleParam;
using autoware::behavior_velocity_planner::isAheadOf;
using autoware::behavior_velocity_planner::PointCloud2;
using autoware::behavior_velocity_planner::RunOutDebug;
using autoware::behavior_velocity_planner::selectLateralNearestPoints;
using autoware::behavior_velocity_planner::transformPointCloud;
using autoware::behavior_velocity_planner::run_out_utils::createExtendPathPoint;

class TestDynamicObstacleMethods : public ::testing::Test
{
  void SetUp() override {}
};

class TestDynamicObstacle : public ::testing::Test
{
  void SetUp() override { init_param(); }

  void init_param()
  {
    auto node_options = rclcpp::NodeOptions{};
    node_ptr_ = std::make_shared<rclcpp::Node>(name_, node_options);
    debug_ptr_ = std::make_shared<RunOutDebug>(*node_ptr_);
    object_creator_for_points_ =
      std::make_shared<DynamicObstacleCreatorForPoints>(*node_ptr_, debug_ptr_, param_);
    object_creator_for_objects_ =
      std::make_shared<DynamicObstacleCreatorForObject>(*node_ptr_, debug_ptr_, param_);
    object_creator_for_objects_without_path_ =
      std::make_shared<DynamicObstacleCreatorForObjectWithoutPath>(*node_ptr_, debug_ptr_, param_);
  }
  std::string name_{"test_dynamic_obstacle_creators"};

  std::shared_ptr<DynamicObstacleCreatorForPoints> object_creator_for_points_;
  std::shared_ptr<DynamicObstacleCreatorForObject> object_creator_for_objects_;
  std::shared_ptr<DynamicObstacleCreatorForObjectWithoutPath>
    object_creator_for_objects_without_path_;
  DynamicObstacleParam param_;
  std::shared_ptr<RunOutDebug> debug_ptr_;
  std::shared_ptr<rclcpp::Node> node_ptr_;
};

pcl::PointCloud<pcl::PointXYZ> generate_pointcloud(
  const size_t number_points_in_axis, const double resolution)
{
  pcl::PointCloud<pcl::PointXYZ> point_cloud;
  for (size_t i = 0; i < number_points_in_axis; ++i) {
    for (size_t j = 0; j < number_points_in_axis; ++j) {
      for (size_t k = 0; k < number_points_in_axis; ++k) {
        pcl::PointXYZ p;
        p.x = i * resolution;
        p.y = j * resolution;
        p.z = k * resolution;
        point_cloud.push_back(p);
      }
    }
  }
  return point_cloud;
};

TEST_F(TestDynamicObstacleMethods, testCreateQuaternionFacingToTrajectory)
{
  constexpr size_t n_path_points{10};
  PathPointsWithLaneId path;
  PathPointWithLaneId base_point;
  for (size_t i = 0; i < n_path_points; ++i) {
    const PathPointWithLaneId p = createExtendPathPoint(static_cast<double>(i), base_point);
    path.push_back(p);
  }

  /*
                 path
                  |           ^ x
                  |           |
                  |    y      |
                  |    <------- ref frame
                  |
                  |
     (2.0,0.0)    |<------0 (2.0,-2.0) point
                  |    ^
                  |    |
                  | 90 deg yaw
  */

  {
    geometry_msgs::msg::Point point;
    point.x = 2.0;
    point.y = -2.0;

    const auto quaternion_facing_traj = createQuaternionFacingToTrajectory(path, point);
    const auto rpy = autoware::universe_utils::getRPY(quaternion_facing_traj);
    const auto yaw = autoware::universe_utils::normalizeRadian(rpy.z);
    EXPECT_DOUBLE_EQ(yaw, M_PI_2);
    geometry_msgs::msg::Pose geom_base_point;
    geom_base_point.position = base_point.point.pose.position;
    EXPECT_TRUE(isAheadOf(point, geom_base_point));
  }

  {
    geometry_msgs::msg::Point point;
    point.x = 2.75;  // path resolution is 1.0, this makes sure the point is "behind" the closest
                     // point on the path.
    point.y = -0.25;

    const auto quaternion_facing_traj = createQuaternionFacingToTrajectory(path, point);
    const auto rpy = autoware::universe_utils::getRPY(quaternion_facing_traj);
    const auto yaw = autoware::universe_utils::normalizeRadian(rpy.z);
    EXPECT_DOUBLE_EQ(std::abs(yaw), M_PI_2);
    geometry_msgs::msg::Pose geom_base_point;
    geom_base_point.position = base_point.point.pose.position;
    EXPECT_TRUE(isAheadOf(point, geom_base_point));
  }
}

TEST_F(TestDynamicObstacleMethods, testCreatePredictedPath)
{
  using autoware::behavior_velocity_planner::run_out_utils::isSamePoint;

  constexpr float time_step{0.1};
  constexpr float max_velocity_mps{3.0};
  constexpr float max_prediction_time{5.0};
  geometry_msgs::msg::Pose initial_pose;

  const auto traj =
    createPredictedPath(initial_pose, time_step, max_velocity_mps, max_prediction_time);
  EXPECT_EQ(traj.size(), static_cast<size_t>(max_prediction_time / time_step));
  auto last_pose = traj.back();
  geometry_msgs::msg::Point expected_point;
  expected_point.x = max_prediction_time * max_velocity_mps - time_step * max_velocity_mps;
  EXPECT_TRUE(std::abs(expected_point.x - last_pose.position.x) < 1e-3);
}

TEST_F(TestDynamicObstacleMethods, testApplyVoxelGridFilter)
{
  pcl::PointCloud<pcl::PointXYZ> point_cloud = generate_pointcloud(10, 0.025);

  {
    auto filtered_point_cloud = applyVoxelGridFilter(point_cloud);
    EXPECT_FALSE(filtered_point_cloud.empty());
    EXPECT_TRUE(filtered_point_cloud.size() < point_cloud.size());
    const bool points_have_no_height = std::all_of(
      filtered_point_cloud.begin(), filtered_point_cloud.end(),
      [](const auto & p) { return std::abs(p.z) < std::numeric_limits<double>::epsilon(); });
    EXPECT_TRUE(points_have_no_height);

    Polygons2d polys;
    const auto empty_cloud = extractObstaclePointsWithinPolygon(filtered_point_cloud, polys);
    EXPECT_TRUE(empty_cloud.empty());

    Polygon2d poly;
    Point2d p1;
    p1.x() = 1.0;
    p1.y() = 0.0;

    Point2d p2;
    p2.x() = -1.0;
    p2.y() = 2.0;

    Point2d p3;
    p3.x() = -1.0;
    p3.y() = -2.0;
    poly.outer().push_back(p1);
    poly.outer().push_back(p2);
    poly.outer().push_back(p3);
    poly.outer().push_back(p1);
    polys.push_back(poly);
    const auto all_points_in_cloud =
      extractObstaclePointsWithinPolygon(filtered_point_cloud, polys);
    EXPECT_FALSE(all_points_in_cloud.empty());
    EXPECT_EQ(all_points_in_cloud.size(), filtered_point_cloud.size());
  }

  sensor_msgs::msg::PointCloud2 ros_pointcloud;
  pcl::toROSMsg(point_cloud, ros_pointcloud);

  {
    auto filtered_point_cloud = applyVoxelGridFilter(ros_pointcloud);
    EXPECT_FALSE(filtered_point_cloud.empty());
    EXPECT_TRUE(filtered_point_cloud.size() < point_cloud.size());
    const bool points_have_no_height = std::all_of(
      filtered_point_cloud.begin(), filtered_point_cloud.end(),
      [](const auto & p) { return std::abs(p.z) < std::numeric_limits<double>::epsilon(); });
    EXPECT_TRUE(points_have_no_height);
  }
}

TEST_F(TestDynamicObstacleMethods, testGroupPointsWithNearestSegmentIndex)
{
  constexpr size_t n_points{10};
  constexpr double points_resolution{0.025};

  pcl::PointCloud<pcl::PointXYZ> point_cloud = generate_pointcloud(n_points, points_resolution);
  constexpr size_t n_path_points{10};
  PathPointsWithLaneId path;
  PathPointWithLaneId base_point;
  for (size_t i = 0; i < n_path_points; ++i) {
    const PathPointWithLaneId p = createExtendPathPoint(static_cast<double>(i), base_point);
    path.push_back(p);
  }
  const auto grouped_points = groupPointsWithNearestSegmentIndex(point_cloud, path);
  EXPECT_FALSE(grouped_points.empty());
  EXPECT_EQ(grouped_points.size(), path.size());
  // first point in path is the closest to all points
  EXPECT_EQ(grouped_points.at(0).size(), point_cloud.size());
}

TEST_F(TestDynamicObstacleMethods, testCalculateLateralNearestPoint)
{
  constexpr size_t n_points{10};
  constexpr double points_resolution{1.0};

  pcl::PointCloud<pcl::PointXYZ> point_cloud = generate_pointcloud(n_points, points_resolution);
  geometry_msgs::msg::Pose base_pose;
  base_pose.position.y = 10.0;
  auto nearest_point = calculateLateralNearestPoint(point_cloud, base_pose);
  EXPECT_DOUBLE_EQ(nearest_point.y, (n_points - 1) * points_resolution);

  PathPointsWithLaneId path;
  PathPointWithLaneId base_point;
  constexpr size_t n_path_points{10};
  for (size_t i = 0; i < n_path_points; ++i) {
    const PathPointWithLaneId p = createExtendPathPoint(static_cast<double>(i), base_point);
    path.push_back(p);
  }

  const auto grouped_points = groupPointsWithNearestSegmentIndex(point_cloud, path);
  auto lateral_nearest_points = selectLateralNearestPoints(grouped_points, path);
  EXPECT_FALSE(grouped_points.empty());
  EXPECT_EQ(grouped_points.size(), path.size());
  EXPECT_TRUE(lateral_nearest_points.size() <= n_path_points);
  for (size_t i = 0; i < lateral_nearest_points.size(); ++i) {
    const auto p = path.at(i);
    const auto curr_nearest_point = lateral_nearest_points.at(i);
    auto deviation = std::abs(autoware::universe_utils::calcLateralDeviation(
      p.point.pose,
      autoware::universe_utils::createPoint(curr_nearest_point.x, curr_nearest_point.y, 0)));
    EXPECT_DOUBLE_EQ(deviation, 0.0);
  }

  constexpr double interval{1.0};
  const auto path_with_lane_id =
    autoware::test_utils::generateTrajectory<PathWithLaneId>(n_path_points, interval);
  {
    const auto interp_lateral_nearest_points =
      extractLateralNearestPoints(point_cloud, path_with_lane_id, interval / 4.0);
    EXPECT_EQ(interp_lateral_nearest_points.size(), path_with_lane_id.points.size());
  }

  {
    const auto interp_lateral_nearest_points =
      extractLateralNearestPoints(point_cloud, path_with_lane_id, interval * 2.0);
    EXPECT_EQ(interp_lateral_nearest_points.size(), path_with_lane_id.points.size() / 2);
  }
}

TEST_F(TestDynamicObstacleMethods, testConcatPointCloud)
{
  constexpr size_t n_points{10};
  constexpr double points_resolution{0.025};

  pcl::PointCloud<pcl::PointXYZ> point_cloud_1 = generate_pointcloud(n_points, points_resolution);
  pcl::PointCloud<pcl::PointXYZ> point_cloud_2 =
    generate_pointcloud(n_points * 2, points_resolution * 2.0);
  auto point_cloud_concat = concatPointCloud(point_cloud_1, point_cloud_2);
  // the pcl method used by this function generates a pointcloud that has a way bigger size than
  // just the sum of both point clouds
  EXPECT_TRUE(point_cloud_concat.data.size() >= point_cloud_1.size() + point_cloud_2.size());

  Eigen::Matrix3f R;
  R = Eigen::Matrix3f::Identity();
  Eigen::Vector3f T;
  T.setOnes();
  Eigen::Matrix4f transform;  // Your Transformation Matrix
  transform.setIdentity();    // Set to Identity to make bottom row of Matrix 0,0,0,1
  transform.block<3, 3>(0, 0) = R;
  transform.block<3, 1>(0, 3) = T;

  Eigen::Affine3f m;
  m.matrix() = transform;

  PointCloud2 ros_pointcloud;
  pcl::toROSMsg(point_cloud_1, ros_pointcloud);

  auto transformed_pointcloud = transformPointCloud(ros_pointcloud, m);
  EXPECT_TRUE(transformed_pointcloud.at(0).x > T.x() - std::numeric_limits<double>::epsilon());
  EXPECT_TRUE(transformed_pointcloud.at(0).y > T.y() - std::numeric_limits<double>::epsilon());
  EXPECT_TRUE(transformed_pointcloud.at(0).z > T.z() - std::numeric_limits<double>::epsilon());
}

TEST_F(TestDynamicObstacleMethods, testCalculateMinAndMaxVelFromCovariance)
{
  geometry_msgs::msg::TwistWithCovariance twist;
  twist.covariance[0] = 1.0;
  twist.covariance[7] = 1.0;
  twist.twist.linear.x = 1.0;
  twist.twist.linear.y = 1.0;

  constexpr double std_dev_multiplier{1.0};
  DynamicObstacle dynamic_obstacle;
  calculateMinAndMaxVelFromCovariance(twist, std_dev_multiplier, dynamic_obstacle);
  EXPECT_TRUE(std::abs(dynamic_obstacle.max_velocity_mps - std::hypot(2.0, 2.0)) < 1e-3);
  EXPECT_DOUBLE_EQ(dynamic_obstacle.min_velocity_mps, std::hypot(0.0, 0.0));
}

TEST_F(TestDynamicObstacleMethods, testCreatePathToPredictionTime)
{
  autoware_perception_msgs::msg::PredictedPath predicted_path;
  constexpr double prediction_time{5.0};
  predicted_path.time_step = rclcpp::Duration(0.0, 100000000.0);

  geometry_msgs::msg::Pose initial_pose;

  constexpr double max_velocity_mps = 1.0;
  constexpr double max_prediction_time = 2.0 * prediction_time;
  const double time_step = convertDurationToDouble(predicted_path.time_step);

  const auto traj =
    createPredictedPath(initial_pose, time_step, max_velocity_mps, max_prediction_time);
  predicted_path.path = traj;

  {
    auto path_to_prediction_time = createPathToPredictionTime(predicted_path, 0.0);
    EXPECT_EQ(0, path_to_prediction_time.size());
  }

  {
    auto path_to_prediction_time = createPathToPredictionTime(predicted_path, prediction_time);
    EXPECT_EQ(predicted_path.path.size() / 2, path_to_prediction_time.size());
    EXPECT_TRUE(
      std::abs(path_to_prediction_time.back().position.x - prediction_time * max_velocity_mps) <
      time_step * max_velocity_mps + std::numeric_limits<double>::epsilon());
  }
}

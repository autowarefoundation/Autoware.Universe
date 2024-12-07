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

// Note: To regenerate the ground truth (GT) for the expected undistorted point cloud values,
// set the "debug_" value to true to display the point cloud values. Then,
// replace the expected values with the newly displayed undistorted point cloud values.
//
// Also, make sure the point stamp, twist stamp, and imu stamp are not identical.
// In the current hardcoded design, timestamp of pointcloud, twist, and imu msg are listed below
// pointcloud (1 msgs, 10points):
// 10.10, 10.11, 10.12, 10.13, 10.14, 10.15, 10.16, 10.17, 10.18, 10.19
// twist (6msgs):
// 10.095, 10.119, 10.143, 10.167, 10.191, 10.215
// imu (6msgs):
// 10.09, 10.117, 10.144, 10.171, 10.198, 10.225

#include "autoware/pointcloud_preprocessor/distortion_corrector/distortion_corrector.hpp"
#include "autoware/universe_utils/math/constants.hpp"
#include "autoware/universe_utils/math/trigonometry.hpp"

#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/twist_stamped.hpp>
#include <geometry_msgs/msg/twist_with_covariance_stamped.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <gtest/gtest.h>
#include <tf2_ros/static_transform_broadcaster.h>

#include <cassert>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

enum AngleCoordinateSystem { HESAI, VELODYNE, CARTESIAN };
class DistortionCorrectorTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    node_ = std::make_shared<rclcpp::Node>("test_node");
    distortion_corrector_2d_ =
      std::make_shared<autoware::pointcloud_preprocessor::DistortionCorrector2D>(*node_, true);
    distortion_corrector_3d_ =
      std::make_shared<autoware::pointcloud_preprocessor::DistortionCorrector3D>(*node_, true);

    // Setup TF
    tf_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(node_);
    tf_broadcaster_->sendTransform(generate_static_transform_msgs());

    // Spin the node for a while to ensure transforms are published
    auto start = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(100);
    while (std::chrono::steady_clock::now() - start < timeout) {
      rclcpp::spin_some(node_);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  void TearDown() override {}

  static void check_input(int ms) { ASSERT_LT(ms, 1000) << "ms should be less than a second."; }

  static rclcpp::Time add_milliseconds(const rclcpp::Time & stamp, int ms)
  {
    check_input(ms);
    auto ms_in_ns = rclcpp::Duration(0, ms * 1000000);
    return stamp + ms_in_ns;
  }

  static rclcpp::Time subtract_milliseconds(const rclcpp::Time & stamp, int ms)
  {
    check_input(ms);
    auto ms_in_ns = rclcpp::Duration(0, ms * 1000000);
    return stamp - ms_in_ns;
  }

  static geometry_msgs::msg::TransformStamped generate_transform_msg(
    const std::string & parent_frame, const std::string & child_frame, double x, double y, double z,
    double qx, double qy, double qz, double qw)
  {
    rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
    geometry_msgs::msg::TransformStamped tf_msg;
    tf_msg.header.stamp = timestamp;
    tf_msg.header.frame_id = parent_frame;
    tf_msg.child_frame_id = child_frame;
    tf_msg.transform.translation.x = x;
    tf_msg.transform.translation.y = y;
    tf_msg.transform.translation.z = z;
    tf_msg.transform.rotation.x = qx;
    tf_msg.transform.rotation.y = qy;
    tf_msg.transform.rotation.z = qz;
    tf_msg.transform.rotation.w = qw;
    return tf_msg;
  }

  static std::vector<geometry_msgs::msg::TransformStamped> generate_static_transform_msgs()
  {
    // generate defined transformations
    return {
      generate_transform_msg("base_link", "lidar_top", 5.0, 5.0, 5.0, 0.683, 0.5, 0.183, 0.499),
      generate_transform_msg("base_link", "imu_link", 1.0, 1.0, 3.0, 0.278, 0.717, 0.441, 0.453)};
  }

  static std::shared_ptr<geometry_msgs::msg::TwistWithCovarianceStamped> generate_twist_msg(
    double linear_x, double angular_z, const rclcpp::Time & stamp)
  {
    auto twist_msg = std::make_shared<geometry_msgs::msg::TwistWithCovarianceStamped>();
    twist_msg->header.stamp = stamp;
    twist_msg->header.frame_id = "base_link";
    twist_msg->twist.twist.linear.x = linear_x;
    twist_msg->twist.twist.angular.z = angular_z;
    return twist_msg;
  }

  static std::shared_ptr<sensor_msgs::msg::Imu> generate_imu_msg(
    double angular_vel_x, double angular_vel_y, double angular_vel_z, const rclcpp::Time & stamp)
  {
    auto imu_msg = std::make_shared<sensor_msgs::msg::Imu>();
    imu_msg->header.stamp = stamp;
    imu_msg->header.frame_id = "imu_link";
    imu_msg->angular_velocity.x = angular_vel_x;
    imu_msg->angular_velocity.y = angular_vel_y;
    imu_msg->angular_velocity.z = angular_vel_z;
    return imu_msg;
  }

  static std::vector<std::shared_ptr<geometry_msgs::msg::TwistWithCovarianceStamped>>
  generate_twist_msgs(const rclcpp::Time & pointcloud_timestamp)
  {
    std::vector<std::shared_ptr<geometry_msgs::msg::TwistWithCovarianceStamped>> twist_msgs;
    rclcpp::Time twist_stamp = subtract_milliseconds(pointcloud_timestamp, 5);

    for (int i = 0; i < number_of_twist_msgs; ++i) {
      auto twist_msg = generate_twist_msg(
        twist_linear_x + i * twist_linear_x_increment,
        twist_angular_z + i * twist_angular_z_increment, twist_stamp);
      twist_msgs.push_back(twist_msg);

      twist_stamp = add_milliseconds(twist_stamp, twist_msgs_interval_ms);
    }

    return twist_msgs;
  }

  static std::vector<std::shared_ptr<sensor_msgs::msg::Imu>> generate_imu_msgs(
    const rclcpp::Time & pointcloud_timestamp)
  {
    std::vector<std::shared_ptr<sensor_msgs::msg::Imu>> imu_msgs;
    rclcpp::Time imu_stamp = subtract_milliseconds(pointcloud_timestamp, 10);

    for (int i = 0; i < number_of_imu_msgs; ++i) {
      auto imu_msg = generate_imu_msg(
        imu_angular_x + i * imu_angular_x_increment, imu_angular_y + i * imu_angular_y_increment,
        imu_angular_z + i * imu_angular_z_increment, imu_stamp);
      imu_msgs.push_back(imu_msg);
      imu_stamp = add_milliseconds(imu_stamp, imu_msgs_interval_ms);
    }

    return imu_msgs;
  }

  static std::tuple<std::vector<Eigen::Vector3f>, std::vector<float>> generate_default_pointcloud(
    AngleCoordinateSystem coordinate_system)
  {
    // Generate all combinations of signs { -, 0, + } x { -, 0, + } for x and y.
    // Also include the case of (0, 0 ,0)
    std::vector<Eigen::Vector3f> default_points = {{
      Eigen::Vector3f(0.0f, 0.0f, 0.0f),    // point 1
      Eigen::Vector3f(0.0f, 0.0f, 0.0f),    // point 2
      Eigen::Vector3f(10.0f, 0.0f, 1.0f),   // point 3
      Eigen::Vector3f(5.0f, -5.0f, 2.0f),   // point 4
      Eigen::Vector3f(0.0f, -10.0f, 3.0f),  // point 5
      Eigen::Vector3f(-5.0f, -5.0f, 4.0f),  // point 6
      Eigen::Vector3f(-10.0f, 0.0f, 5.0f),  // point 7
      Eigen::Vector3f(-5.0f, 5.0f, -5.0f),  // point 8
      Eigen::Vector3f(0.0f, 10.0f, -4.0f),  // point 9
      Eigen::Vector3f(5.0f, 5.0f, -3.0f),   // point 10
    }};

    std::vector<float> default_azimuths;
    for (const auto & point : default_points) {
      if (coordinate_system == AngleCoordinateSystem::VELODYNE) {
        // velodyne coordinates: x-axis is 0 degrees, y-axis is 270 degrees, angle increase in
        // clockwise direction
        float cartesian_deg = std::atan2(point.y(), point.x()) * 180 / autoware::universe_utils::pi;
        if (cartesian_deg < 0) cartesian_deg += 360;
        float velodyne_deg = 360 - cartesian_deg;
        if (velodyne_deg == 360) velodyne_deg = 0;
        default_azimuths.push_back(velodyne_deg * autoware::universe_utils::pi / 180);
      } else if (coordinate_system == AngleCoordinateSystem::HESAI) {
        // hesai coordinates: y-axis is 0 degrees, x-axis is 90 degrees, angle increase in clockwise
        // direction
        float cartesian_deg = std::atan2(point.y(), point.x()) * 180 / autoware::universe_utils::pi;
        if (cartesian_deg < 0) cartesian_deg += 360;
        float hesai_deg = 90 - cartesian_deg < 0 ? 90 - cartesian_deg + 360 : 90 - cartesian_deg;
        if (hesai_deg == 360) hesai_deg = 0;
        default_azimuths.push_back(hesai_deg * autoware::universe_utils::pi / 180);
      } else if (coordinate_system == AngleCoordinateSystem::CARTESIAN) {
        // Cartesian coordinates: x-axis is 0 degrees, y-axis is 90 degrees, angle increase in
        // counterclockwise direction
        default_azimuths.push_back(std::atan2(point.y(), point.x()));
      } else {
        throw std::runtime_error("Invalid angle coordinate system");
      }
    }

    return std::make_tuple(default_points, default_azimuths);
  }

  sensor_msgs::msg::PointCloud2 generate_empty_pointcloud_msg(const rclcpp::Time & stamp)
  {
    auto empty_pointcloud_msg = generate_pointcloud_msg(true, stamp, {}, {});
    return empty_pointcloud_msg;
  }

  sensor_msgs::msg::PointCloud2 generate_pointcloud_msg(
    bool is_lidar_frame, const rclcpp::Time & stamp, std::vector<Eigen::Vector3f> points,
    std::vector<float> azimuths)
  {
    sensor_msgs::msg::PointCloud2 pointcloud_msg;
    pointcloud_msg.header.stamp = stamp;
    pointcloud_msg.header.frame_id = is_lidar_frame ? "lidar_top" : "base_link";
    pointcloud_msg.height = 1;
    pointcloud_msg.is_dense = true;
    pointcloud_msg.is_bigendian = false;

    // Generate timestamps for the points
    std::vector<std::uint32_t> timestamps = generate_point_timestamps(stamp, points.size());

    sensor_msgs::PointCloud2Modifier modifier(pointcloud_msg);
    modifier.setPointCloud2Fields(
      10, "x", 1, sensor_msgs::msg::PointField::FLOAT32, "y", 1,
      sensor_msgs::msg::PointField::FLOAT32, "z", 1, sensor_msgs::msg::PointField::FLOAT32,
      "intensity", 1, sensor_msgs::msg::PointField::UINT8, "return_type", 1,
      sensor_msgs::msg::PointField::UINT8, "channel", 1, sensor_msgs::msg::PointField::UINT16,
      "azimuth", 1, sensor_msgs::msg::PointField::FLOAT32, "elevation", 1,
      sensor_msgs::msg::PointField::FLOAT32, "distance", 1, sensor_msgs::msg::PointField::FLOAT32,
      "time_stamp", 1, sensor_msgs::msg::PointField::UINT32);

    modifier.resize(points.size());

    sensor_msgs::PointCloud2Iterator<float> iter_x(pointcloud_msg, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(pointcloud_msg, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(pointcloud_msg, "z");
    sensor_msgs::PointCloud2Iterator<float> iter_azimuth(pointcloud_msg, "azimuth");
    sensor_msgs::PointCloud2Iterator<float> iter_distance(pointcloud_msg, "distance");
    sensor_msgs::PointCloud2Iterator<std::uint32_t> iter_t(pointcloud_msg, "time_stamp");

    for (size_t i = 0; i < points.size(); ++i) {
      *iter_x = points[i].x();
      *iter_y = points[i].y();
      *iter_z = points[i].z();

      *iter_azimuth = azimuths[i];
      *iter_distance = points[i].norm();
      *iter_t = timestamps[i];
      ++iter_x;
      ++iter_y;
      ++iter_z;
      ++iter_azimuth;
      ++iter_distance;
      ++iter_t;
    }

    return pointcloud_msg;
  }

  std::vector<std::uint32_t> generate_point_timestamps(
    const rclcpp::Time & pointcloud_timestamp, size_t number_of_points)
  {
    std::vector<std::uint32_t> timestamps;
    rclcpp::Time global_point_stamp = pointcloud_timestamp;
    for (size_t i = 0; i < number_of_points; ++i) {
      std::uint32_t relative_timestamp = (global_point_stamp - pointcloud_timestamp).nanoseconds();
      timestamps.push_back(relative_timestamp);
      global_point_stamp = add_milliseconds(global_point_stamp, points_interval_ms);
    }

    return timestamps;
  }

  template <typename T>
  void generate_and_process_twist_msgs(
    const std::shared_ptr<T> & distortion_corrector, const rclcpp::Time & timestamp)
  {
    auto twist_msgs = generate_twist_msgs(timestamp);
    for (const auto & twist_msg : twist_msgs) {
      distortion_corrector->process_twist_message(twist_msg);
    }
  }

  template <typename T>
  void generate_and_process_imu_msgs(
    const std::shared_ptr<T> & distortion_corrector, const rclcpp::Time & timestamp)
  {
    auto imu_msgs = generate_imu_msgs(timestamp);
    for (const auto & imu_msg : imu_msgs) {
      distortion_corrector->process_imu_message("base_link", imu_msg);
    }
  }

  std::shared_ptr<rclcpp::Node> node_;
  std::shared_ptr<autoware::pointcloud_preprocessor::DistortionCorrector2D>
    distortion_corrector_2d_;
  std::shared_ptr<autoware::pointcloud_preprocessor::DistortionCorrector3D>
    distortion_corrector_3d_;
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_broadcaster_;

  static constexpr float standard_tolerance{1e-4};
  static constexpr float coarse_tolerance{5e-3};
  static constexpr int number_of_twist_msgs{6};
  static constexpr int number_of_imu_msgs{6};
  static constexpr size_t number_of_points{10};
  static constexpr int32_t timestamp_seconds{10};
  static constexpr uint32_t timestamp_nanoseconds{100000000};

  static constexpr double twist_linear_x{10.0};
  static constexpr double twist_angular_z{0.02};
  static constexpr double twist_linear_x_increment{2.0};
  static constexpr double twist_angular_z_increment{0.01};

  static constexpr double imu_angular_x{0.01};
  static constexpr double imu_angular_y{-0.02};
  static constexpr double imu_angular_z{0.05};
  static constexpr double imu_angular_x_increment{0.005};
  static constexpr double imu_angular_y_increment{0.005};
  static constexpr double imu_angular_z_increment{0.005};

  static constexpr int points_interval_ms{10};
  static constexpr int twist_msgs_interval_ms{24};
  static constexpr int imu_msgs_interval_ms{27};

  // for debugging or regenerating the ground truth point cloud
  bool debug_{false};
};

TEST_F(DistortionCorrectorTest, TestProcessTwistMessage)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto twist_msg = generate_twist_msg(twist_linear_x, twist_angular_z, timestamp);
  distortion_corrector_2d_->process_twist_message(twist_msg);

  ASSERT_FALSE(distortion_corrector_2d_->get_twist_queue().empty());
  EXPECT_EQ(distortion_corrector_2d_->get_twist_queue().front().twist.linear.x, twist_linear_x);
  EXPECT_EQ(distortion_corrector_2d_->get_twist_queue().front().twist.angular.z, twist_angular_z);
}

TEST_F(DistortionCorrectorTest, TestProcessImuMessage)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto imu_msg = generate_imu_msg(imu_angular_x, imu_angular_y, imu_angular_z, timestamp);
  distortion_corrector_2d_->process_imu_message("base_link", imu_msg);

  ASSERT_FALSE(distortion_corrector_2d_->get_angular_velocity_queue().empty());
  EXPECT_NEAR(
    distortion_corrector_2d_->get_angular_velocity_queue().front().vector.z, -0.03159,
    standard_tolerance);
}

TEST_F(DistortionCorrectorTest, TestIsPointcloudValid)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);

  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);
  auto result = distortion_corrector_2d_->is_pointcloud_valid(pointcloud);
  EXPECT_TRUE(result);

  // input empty pointcloud
  auto empty_pointcloud = generate_empty_pointcloud_msg(timestamp);
  result = distortion_corrector_2d_->is_pointcloud_valid(empty_pointcloud);
  EXPECT_FALSE(result);
}

TEST_F(DistortionCorrectorTest, TestSetPointcloudTransformWithBaseLink)
{
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "base_link");
  EXPECT_TRUE(distortion_corrector_2d_->pointcloud_transform_exists());
  EXPECT_FALSE(distortion_corrector_2d_->pointcloud_transform_needed());
}

TEST_F(DistortionCorrectorTest, TestSetPointcloudTransformWithLidarFrame)
{
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "lidar_top");
  EXPECT_TRUE(distortion_corrector_2d_->pointcloud_transform_exists());
  EXPECT_TRUE(distortion_corrector_2d_->pointcloud_transform_needed());
}

TEST_F(DistortionCorrectorTest, TestSetPointcloudTransformWithMissingFrame)
{
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "missing_lidar_frame");
  EXPECT_FALSE(distortion_corrector_2d_->pointcloud_transform_exists());
  EXPECT_FALSE(distortion_corrector_2d_->pointcloud_transform_needed());
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudWithEmptyTwist)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  // Generate the point cloud message
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Process empty twist queue
  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->undistort_pointcloud(false, std::nullopt, pointcloud);

  // Verify the point cloud is not changed
  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, 0.0f),
     Eigen::Vector3f(10.0f, 0.0f, 1.0f), Eigen::Vector3f(5.0f, -5.0f, 2.0f),
     Eigen::Vector3f(0.0f, -10.0f, 3.0f), Eigen::Vector3f(-5.0f, -5.0f, 4.0f),
     Eigen::Vector3f(-10.0f, 0.0f, 5.0f), Eigen::Vector3f(-5.0f, 5.0f, -5.0f),
     Eigen::Vector3f(0.0f, 10.0f, -4.0f), Eigen::Vector3f(5.0f, 5.0f, -3.0f)}};

  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudWithEmptyPointCloud)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);
  // Generate an empty point cloud message

  auto empty_pointcloud = generate_empty_pointcloud_msg(timestamp);
  // Process empty point cloud
  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->undistort_pointcloud(true, std::nullopt, empty_pointcloud);

  // Verify the point cloud is still empty
  EXPECT_EQ(empty_pointcloud.width, 0);
  EXPECT_EQ(empty_pointcloud.row_step, 0);
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloud2dWithoutImuInBaseLink)
{
  // Generate the point cloud message
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Test using only twist
  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_2d_->undistort_pointcloud(false, std::nullopt, pointcloud);

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  // Expected undistorted point cloud values
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.12f, 3.45146e-05f, 0.0f),
     Eigen::Vector3f(10.26f, 0.00684635f, 1.0f), Eigen::Vector3f(5.40527f, -4.99443f, 2.0f),
     Eigen::Vector3f(0.55534f, -9.99949f, 3.0f), Eigen::Vector3f(-4.28992f, -5.00924f, 4.0f),
     Eigen::Vector3f(-9.13997f, -0.0237086f, 5.0f), Eigen::Vector3f(-3.97532f, 4.98642f, -5.0f),
     Eigen::Vector3f(1.18261f, 10.0024f, -4.0f), Eigen::Vector3f(6.37838f, 5.02475f, -3.0f)}};

  // Verify each point in the undistorted point cloud
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloud2dWithImuInBaseLink)
{
  // Generate the point cloud message
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_2d_, timestamp);

  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_2d_->undistort_pointcloud(true, std::nullopt, pointcloud);

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  // Expected undistorted point cloud values
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.12f, -3.45146e-05f, 0.0f),
     Eigen::Vector3f(10.26f, -0.00586748f, 1.0f), Eigen::Vector3f(5.39568f, -5.00455f, 2.0f),
     Eigen::Vector3f(0.528495f, -10.0004f, 3.0f), Eigen::Vector3f(-4.30719f, -4.99343f, 4.0f),
     Eigen::Vector3f(-9.13999f, 0.0163541f, 5.0f), Eigen::Vector3f(-3.94992f, 5.0088f, -5.0f),
     Eigen::Vector3f(1.24205f, 9.99831f, -4.0f), Eigen::Vector3f(6.41245f, 4.98541f, -3.0f)}};

  // Verify each point in the undistorted point cloud
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloud2dWithImuInLidarFrame)
{
  // Generate the point cloud message
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_2d_, timestamp);

  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "lidar_top");
  distortion_corrector_2d_->undistort_pointcloud(true, std::nullopt, pointcloud);

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  // Expected undistorted point cloud values
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0512387f, 0.0608269f, 0.0917824f),
     Eigen::Vector3f(10.1106f, 0.134026f, 1.20356f), Eigen::Vector3f(5.17128f, -4.79604f, 2.30806f),
     Eigen::Vector3f(0.232686f, -9.7275f, 3.40938f),
     Eigen::Vector3f(-4.70281f, -4.65034f, 4.52609f),
     Eigen::Vector3f(-9.64009f, 0.425434f, 5.64106f),
     Eigen::Vector3f(-4.55139f, 5.5241f, -4.21327f),
     Eigen::Vector3f(0.519385f, 10.6188f, -3.06522f),
     Eigen::Vector3f(5.5992f, 5.71475f, -1.91985f)}};

  // Verify each point in the undistorted point cloud
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloud3dWithoutImuInBaseLink)
{
  // Generate the point cloud message
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_3d_, timestamp);

  // Test using only twist
  distortion_corrector_3d_->initialize();
  distortion_corrector_3d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_3d_->undistort_pointcloud(false, std::nullopt, pointcloud);

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  // Expected undistorted point cloud values
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.12f, 2.38419e-05f, 0.0f),
     Eigen::Vector3f(10.26f, 0.0070927f, 1.0f), Eigen::Vector3f(5.4055f, -4.99428f, 2.0f),
     Eigen::Vector3f(0.555f, -9.99959f, 3.0f), Eigen::Vector3f(-4.28999f, -5.00928f, 4.0f),
     Eigen::Vector3f(-9.13997f, -0.0239053f, 5.0f), Eigen::Vector3f(-3.97548f, 4.98614f, -5.0f),
     Eigen::Vector3f(1.183f, 10.0023f, -4.0f), Eigen::Vector3f(6.37845f, 5.02458f, -3.0f)}};

  // Verify each point in the undistorted point cloud
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloud3dWithImuInBaseLink)
{
  // Generate the point cloud message
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_3d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_3d_, timestamp);

  distortion_corrector_3d_->initialize();
  distortion_corrector_3d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_3d_->undistort_pointcloud(true, std::nullopt, pointcloud);

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  // Expected undistorted point cloud values
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 0.0f, 0.0f), Eigen::Vector3f(0.12f, -1.86084e-05f, -1.63216e-05f),
     Eigen::Vector3f(10.2606f, -0.00683919f, 0.993812f),
     Eigen::Vector3f(5.39753f, -5.00722f, 1.9883f), Eigen::Vector3f(0.532273f, -10.0057f, 2.98165f),
     Eigen::Vector3f(-4.30025f, -5.0024f, 3.99665f),
     Eigen::Vector3f(-9.12918f, 0.00256404f, 5.02064f),
     Eigen::Vector3f(-3.96298f, 5.02511f, -4.97218f),
     Eigen::Vector3f(1.23005f, 10.0137f, -3.96452f),
     Eigen::Vector3f(6.40165f, 4.99868f, -2.99944f)}};

  // Verify each point in the undistorted point cloud
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloud3dWithImuInLidarFrame)
{
  // Generate the point cloud message
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud = generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_3d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_3d_, timestamp);

  distortion_corrector_3d_->initialize();
  distortion_corrector_3d_->set_pointcloud_transform("base_link", "lidar_top");
  distortion_corrector_3d_->undistort_pointcloud(true, std::nullopt, pointcloud);

  sensor_msgs::PointCloud2ConstIterator<float> iter_x(pointcloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> iter_y(pointcloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> iter_z(pointcloud, "z");

  // Expected undistorted point cloud values
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(0.0f, 4.76837e-07f, 0.0f), Eigen::Vector3f(0.049716f, 0.0622373f, 0.0935726f),
     Eigen::Vector3f(10.1082f, 0.139472f, 1.20323f), Eigen::Vector3f(5.17113f, -4.79225f, 2.30392f),
     Eigen::Vector3f(0.23695f, -9.72807f, 3.39875f),
     Eigen::Vector3f(-4.70053f, -4.65832f, 4.53053f),
     Eigen::Vector3f(-9.64065f, 0.407413f, 5.66885f), Eigen::Vector3f(-4.5738f, 5.5446f, -4.17022f),
     Eigen::Vector3f(0.489763f, 10.6448f, -3.00165f),
     Eigen::Vector3f(5.57566f, 5.74589f, -1.88189f)}};

  // Verify each point in the undistorted point cloud
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z << ")\n";
    EXPECT_NEAR(*iter_x, expected_pointcloud[i].x(), standard_tolerance);
    EXPECT_NEAR(*iter_y, expected_pointcloud[i].y(), standard_tolerance);
    EXPECT_NEAR(*iter_z, expected_pointcloud[i].z(), standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudWithPureLinearMotion)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto test2d_pointcloud =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);
  auto test3d_pointcloud =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process a single twist message with constant linear velocity
  auto twist_msg = generate_twist_msg(1.0, 0.0, timestamp);

  distortion_corrector_2d_->process_twist_message(twist_msg);
  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_2d_->undistort_pointcloud(false, std::nullopt, test2d_pointcloud);

  distortion_corrector_3d_->process_twist_message(twist_msg);
  distortion_corrector_3d_->initialize();
  distortion_corrector_3d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_3d_->undistort_pointcloud(false, std::nullopt, test3d_pointcloud);

  // Generate expected point cloud for testing
  auto expected_pointcloud =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Calculate expected point cloud values based on constant linear motion
  double velocity = 1.0;  // 1 m/s linear velocity
  sensor_msgs::PointCloud2Iterator<float> iter_x(expected_pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> iter_y(expected_pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> iter_z(expected_pointcloud, "z");
  sensor_msgs::PointCloud2Iterator<std::uint32_t> iter_t(expected_pointcloud, "time_stamp");

  std::vector<Eigen::Vector3f> expected_points;
  for (; iter_t != iter_t.end(); ++iter_t, ++iter_x, ++iter_y, ++iter_z) {
    double time_offset = static_cast<double>(*iter_t) / 1e9;
    expected_points.emplace_back(
      *iter_x + static_cast<float>(velocity * time_offset), *iter_y, *iter_z);
  }

  // Verify each point in the undistorted point cloud
  sensor_msgs::PointCloud2Iterator<float> test2d_iter_x(test2d_pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> test2d_iter_y(test2d_pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> test2d_iter_z(test2d_pointcloud, "z");
  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; test2d_iter_x != test2d_iter_x.end();
       ++test2d_iter_x, ++test2d_iter_y, ++test2d_iter_z, ++i) {
    oss << "Point " << i << ": (" << *test2d_iter_x << ", " << *test2d_iter_y << ", "
        << *test2d_iter_z << ")\n";
    EXPECT_FLOAT_EQ(*test2d_iter_x, expected_points[i].x());
    EXPECT_FLOAT_EQ(*test2d_iter_y, expected_points[i].y());
    EXPECT_FLOAT_EQ(*test2d_iter_z, expected_points[i].z());
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }

  // Verify each point in the undistorted 2d point cloud with undistorted 3d point cloud
  test2d_iter_x = sensor_msgs::PointCloud2Iterator<float>(test2d_pointcloud, "x");
  test2d_iter_y = sensor_msgs::PointCloud2Iterator<float>(test2d_pointcloud, "y");
  test2d_iter_z = sensor_msgs::PointCloud2Iterator<float>(test2d_pointcloud, "z");
  sensor_msgs::PointCloud2Iterator<float> test3d_iter_x(test3d_pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> test3d_iter_y(test3d_pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> test3d_iter_z(test3d_pointcloud, "z");

  i = 0;
  oss.str("");
  oss.clear();

  oss << "Expected pointcloud:\n";
  for (; test2d_iter_x != test2d_iter_x.end(); ++test2d_iter_x, ++test2d_iter_y, ++test2d_iter_z,
                                               ++test3d_iter_x, ++test3d_iter_y, ++test3d_iter_z,
                                               ++i) {
    oss << "Point " << i << " - 2D: (" << *test2d_iter_x << ", " << *test2d_iter_y << ", "
        << *test2d_iter_z << ")"
        << " vs 3D: (" << *test3d_iter_x << ", " << *test3d_iter_y << ", " << *test3d_iter_z
        << ")\n";
    EXPECT_FLOAT_EQ(*test2d_iter_x, *test3d_iter_x);
    EXPECT_FLOAT_EQ(*test2d_iter_y, *test3d_iter_y);
    EXPECT_FLOAT_EQ(*test2d_iter_z, *test3d_iter_z);
  }
  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudWithPureRotationalMotion)
{
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto test2d_pointcloud =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);
  auto test3d_pointcloud =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process a single twist message with constant angular velocity
  auto twist_msg = generate_twist_msg(0.0, 0.1, timestamp);

  distortion_corrector_2d_->process_twist_message(twist_msg);
  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_2d_->undistort_pointcloud(false, std::nullopt, test2d_pointcloud);

  distortion_corrector_3d_->process_twist_message(twist_msg);
  distortion_corrector_3d_->initialize();
  distortion_corrector_3d_->set_pointcloud_transform("base_link", "base_link");
  distortion_corrector_3d_->undistort_pointcloud(false, std::nullopt, test3d_pointcloud);

  // Generate expected point cloud for testing
  auto expected_pointcloud =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Calculate expected point cloud values based on constant rotational motion
  double angular_velocity = 0.1;  // 0.1 rad/s rotational velocity
  sensor_msgs::PointCloud2Iterator<float> iter_x(expected_pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> iter_y(expected_pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> iter_z(expected_pointcloud, "z");
  sensor_msgs::PointCloud2Iterator<std::uint32_t> iter_t(expected_pointcloud, "time_stamp");

  std::vector<Eigen::Vector3f> expected_points;
  for (; iter_t != iter_t.end(); ++iter_t, ++iter_x, ++iter_y, ++iter_z) {
    double time_offset = static_cast<double>(*iter_t) / 1e9;
    float angle = angular_velocity * time_offset;

    // Set the quaternion for the current angle
    tf2::Quaternion quaternion;
    quaternion.setValue(
      0, 0, autoware::universe_utils::sin(angle * 0.5f),
      autoware::universe_utils::cos(angle * 0.5f));

    tf2::Vector3 point(*iter_x, *iter_y, *iter_z);
    tf2::Vector3 rotated_point = tf2::quatRotate(quaternion, point);
    expected_points.emplace_back(
      static_cast<float>(rotated_point.x()), static_cast<float>(rotated_point.y()),
      static_cast<float>(rotated_point.z()));
  }

  // Verify each point in the undistorted 2D point cloud
  sensor_msgs::PointCloud2Iterator<float> test2d_iter_x(test2d_pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> test2d_iter_y(test2d_pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> test2d_iter_z(test2d_pointcloud, "z");

  size_t i = 0;
  std::ostringstream oss;
  oss << "Expected pointcloud:\n";

  for (; test2d_iter_x != test2d_iter_x.end();
       ++test2d_iter_x, ++test2d_iter_y, ++test2d_iter_z, ++i) {
    oss << "Point " << i << ": (" << *test2d_iter_x << ", " << *test2d_iter_y << ", "
        << *test2d_iter_z << ")\n";
    EXPECT_FLOAT_EQ(*test2d_iter_x, expected_points[i].x());
    EXPECT_FLOAT_EQ(*test2d_iter_y, expected_points[i].y());
    EXPECT_FLOAT_EQ(*test2d_iter_z, expected_points[i].z());
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }

  // Verify each point in the undistorted 2D point cloud with undistorted 3D point cloud
  test2d_iter_x = sensor_msgs::PointCloud2Iterator<float>(test2d_pointcloud, "x");
  test2d_iter_y = sensor_msgs::PointCloud2Iterator<float>(test2d_pointcloud, "y");
  test2d_iter_z = sensor_msgs::PointCloud2Iterator<float>(test2d_pointcloud, "z");
  sensor_msgs::PointCloud2Iterator<float> test3d_iter_x(test3d_pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> test3d_iter_y(test3d_pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> test3d_iter_z(test3d_pointcloud, "z");

  i = 0;
  oss.str("");
  oss.clear();

  oss << "Expected pointcloud:\n";
  for (; test2d_iter_x != test2d_iter_x.end(); ++test2d_iter_x, ++test2d_iter_y, ++test2d_iter_z,
                                               ++test3d_iter_x, ++test3d_iter_y, ++test3d_iter_z,
                                               ++i) {
    oss << "Point " << i << " - 2D: (" << *test2d_iter_x << ", " << *test2d_iter_y << ", "
        << *test2d_iter_z << ")"
        << " vs 3D: (" << *test3d_iter_x << ", " << *test3d_iter_y << ", " << *test3d_iter_z
        << ")\n";
    EXPECT_NEAR(*test2d_iter_x, *test3d_iter_x, coarse_tolerance);
    EXPECT_NEAR(*test2d_iter_y, *test3d_iter_y, coarse_tolerance);
    EXPECT_NEAR(*test2d_iter_z, *test3d_iter_z, coarse_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudNotUpdatingAzimuthAndDistance)
{
  // Test the case when the cloud will not update the azimuth and distance values
  // 1. when pointcloud is in base_link (pointcloud_transform_needed() is false)

  // Generate the point cloud message in base_link
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::CARTESIAN);
  auto pointcloud_base_link =
    generate_pointcloud_msg(false, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_2d_, timestamp);

  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "base_link");
  auto angle_conversion_opt =
    distortion_corrector_2d_->try_compute_angle_conversion(pointcloud_base_link);

  // Test for expected runtime error
  EXPECT_THROW(
    {
      distortion_corrector_2d_->undistort_pointcloud(
        true, angle_conversion_opt, pointcloud_base_link);
    },
    std::runtime_error);

  // Test the case when the cloud will not update the azimuth and distance values
  // 2. when the return value of try_compute_angle_conversion is std::nullopt (couldn't find the
  // angle conversion)

  // Generate the point cloud message in sensor frame
  auto pointcloud_lidar_top =
    generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_2d_, timestamp);

  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "lidar_top");

  angle_conversion_opt = std::nullopt;
  distortion_corrector_2d_->undistort_pointcloud(true, angle_conversion_opt, pointcloud_lidar_top);

  auto original_pointcloud_lidar_top =
    generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  sensor_msgs::PointCloud2ConstIterator<float> test_iter_azimuth_lidar_top(
    pointcloud_lidar_top, "azimuth");
  sensor_msgs::PointCloud2ConstIterator<float> test_iter_distance_lidar_top(
    pointcloud_lidar_top, "distance");

  sensor_msgs::PointCloud2ConstIterator<float> original_iter_azimuth_lidar_top(
    original_pointcloud_lidar_top, "azimuth");
  sensor_msgs::PointCloud2ConstIterator<float> original_iter_distance_lidar_top(
    original_pointcloud_lidar_top, "distance");

  size_t i = 0;
  std::ostringstream oss;

  oss << "Expected pointcloud:\n";
  for (; test_iter_azimuth_lidar_top != test_iter_azimuth_lidar_top.end();
       ++test_iter_azimuth_lidar_top, ++test_iter_distance_lidar_top,
       ++original_iter_azimuth_lidar_top, ++original_iter_distance_lidar_top, ++i) {
    oss << "Point " << i << " - Output azimuth and distance: (" << *test_iter_azimuth_lidar_top
        << ", " << *test_iter_distance_lidar_top << ")"
        << " vs Original azimuth and distance: (" << *original_iter_azimuth_lidar_top << ", "
        << *original_iter_distance_lidar_top << ")\n";

    EXPECT_FLOAT_EQ(*test_iter_azimuth_lidar_top, *original_iter_azimuth_lidar_top);
    EXPECT_FLOAT_EQ(*test_iter_distance_lidar_top, *original_iter_distance_lidar_top);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudUpdateAzimuthAndDistanceInVelodyne)
{
  // Generate the point cloud message in sensor frame
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::VELODYNE);
  auto pointcloud = generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_2d_, timestamp);

  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "lidar_top");

  auto angle_conversion_opt = distortion_corrector_2d_->try_compute_angle_conversion(pointcloud);
  distortion_corrector_2d_->undistort_pointcloud(true, angle_conversion_opt, pointcloud);

  auto original_pointcloud =
    generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  sensor_msgs::PointCloud2ConstIterator<float> iter_azimuth(pointcloud, "azimuth");
  sensor_msgs::PointCloud2ConstIterator<float> iter_distance(pointcloud, "distance");

  // Expected undistorted azimuth and distance values
  std::array<std::array<float, 2>, 10> expected_azimuth_distance = {{
    {0.0f, 0.0f},           // points: (0.0f, 0.0f, 0.0f)
    {5.41248f, 0.121447f},  // points: (0.0512387f, 0.0608269f, 0.0917824f)
    {6.26993f, 10.1829f},   // points: (10.1106f, 0.134026f, 1.20356f)
    {0.747926f, 7.421f},    // points: (5.17128f, -4.79604f, 2.30806f)
    {1.54689f, 10.3103f},   // points: (0.232686f, -9.7275f, 3.40938f)
    {2.36187f, 8.01421f},   // points: (-4.70281f, -4.65034f, 4.52609f)
    {3.18569f, 11.1774f},   // points: (-9.64009f, 0.425434f, 5.64106f)
    {4.02323f, 8.30557f},   // points: (-4.55139f, 5.5241f, -4.21327f)
    {4.76125f, 11.0645f},   // points: (0.519385f, 10.6188f, -3.06522f)
    {5.48757f, 8.22771f}    // points: (5.5992f, 5.71475f, -1.91985f)
  }};

  size_t i = 0;
  std::ostringstream oss;

  oss << "Expected pointcloud:\n";
  for (; iter_azimuth != iter_azimuth.end(); ++iter_azimuth, ++iter_distance, ++i) {
    oss << "Point " << i << " - Output azimuth and distance: (" << *iter_azimuth << ", "
        << *iter_distance << ")"
        << " vs Expected azimuth and distance: (" << expected_azimuth_distance[i][0] << ", "
        << expected_azimuth_distance[i][1] << ")\n";

    EXPECT_NEAR(*iter_azimuth, expected_azimuth_distance[i][0], standard_tolerance);
    EXPECT_NEAR(*iter_distance, expected_azimuth_distance[i][1], standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestUndistortPointcloudUpdateAzimuthAndDistanceInHesai)
{
  // Generate the point cloud message in sensor frame
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);

  auto [default_points, default_azimuths] =
    generate_default_pointcloud(AngleCoordinateSystem::HESAI);
  auto pointcloud = generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  // Generate and process multiple IMU messages
  generate_and_process_imu_msgs(distortion_corrector_2d_, timestamp);

  distortion_corrector_2d_->initialize();
  distortion_corrector_2d_->set_pointcloud_transform("base_link", "lidar_top");

  auto angle_conversion_opt = distortion_corrector_2d_->try_compute_angle_conversion(pointcloud);
  distortion_corrector_2d_->undistort_pointcloud(true, angle_conversion_opt, pointcloud);

  auto original_pointcloud =
    generate_pointcloud_msg(true, timestamp, default_points, default_azimuths);

  sensor_msgs::PointCloud2ConstIterator<float> iter_azimuth(pointcloud, "azimuth");
  sensor_msgs::PointCloud2ConstIterator<float> iter_distance(pointcloud, "distance");

  // Expected undistorted azimuth and distance values
  std::array<std::array<float, 2>, 10> expected_azimuth_distance = {{
    {1.5708f, 0.0f},         // points: (0.0f, 0.0f, 0.0f)
    {0.70009f, 0.121447f},   // points: (0.0512387f, 0.0608269f, 0.0917824f)
    {1.55754f, 10.1829f},    // points: (10.1106f, 0.134026f, 1.20356f)
    {2.31872f, 7.421f},      // points: (5.17128f, -4.79604f, 2.30806f)
    {3.11768f, 10.3103f},    // points: (0.232686f, -9.7275f, 3.40938f)
    {3.93267f, 8.01421f},    // points: (-4.70281f, -4.65034f, 4.52609f)
    {4.75648f, 11.1774f},    // points: (-9.64009f, 0.425434f, 5.64106f)
    {5.59403f, 8.30557f},    // points: (-4.55139f, 5.5241f, -4.21327f)
    {0.0488634f, 11.0645f},  // points: (0.519385f, 10.6188f, -3.06522f)
    {0.775183f, 8.22771f}    // points: (5.5992f, 5.71475f, -1.91985f)
  }};

  size_t i = 0;
  std::ostringstream oss;

  oss << "Expected pointcloud:\n";
  for (; iter_azimuth != iter_azimuth.end(); ++iter_azimuth, ++iter_distance, ++i) {
    oss << "Point " << i << " - Output azimuth and distance: (" << *iter_azimuth << ", "
        << *iter_distance << ")"
        << " vs Expected azimuth and distance: (" << expected_azimuth_distance[i][0] << ", "
        << expected_azimuth_distance[i][1] << ")\n";

    EXPECT_NEAR(*iter_azimuth, expected_azimuth_distance[i][0], standard_tolerance);
    EXPECT_NEAR(*iter_distance, expected_azimuth_distance[i][1], standard_tolerance);
  }

  if (debug_) {
    RCLCPP_INFO(node_->get_logger(), "%s", oss.str().c_str());
  }
}

TEST_F(DistortionCorrectorTest, TestTryComputeAngleConversionOnEmptyPointcloud)
{
  // test empty pointcloud
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);

  auto empty_pointcloud = generate_empty_pointcloud_msg(timestamp);
  auto angle_conversion_opt =
    distortion_corrector_2d_->try_compute_angle_conversion(empty_pointcloud);

  EXPECT_FALSE(angle_conversion_opt.has_value());
}

TEST_F(DistortionCorrectorTest, TestTryComputeAngleConversionOnVelodynePointcloud)
{
  // test velodyne pointcloud (x-axis: 0 degree, y-axis: 270 degree)
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);

  std::vector<Eigen::Vector3f> velodyne_points = {
    Eigen::Vector3f(0.0f, 0.0f, 0.0f),
    Eigen::Vector3f(1.0f, -1.0f, 1.0f),
    Eigen::Vector3f(0.0f, -2.0f, 1.0f),
  };
  std::vector<float> velodyne_azimuths = {
    0.0f, autoware::universe_utils::pi / 4, autoware::universe_utils::pi / 2};

  auto velodyne_pointcloud =
    generate_pointcloud_msg(true, timestamp, velodyne_points, velodyne_azimuths);
  auto angle_conversion_opt =
    distortion_corrector_2d_->try_compute_angle_conversion(velodyne_pointcloud);
  EXPECT_TRUE(angle_conversion_opt.has_value());

  EXPECT_EQ(angle_conversion_opt->sign, -1);
  EXPECT_NEAR(angle_conversion_opt->offset_rad, 0, standard_tolerance);
}

TEST_F(DistortionCorrectorTest, TestTryComputeAngleConversionOnHesaiPointcloud)
{
  // test hesai pointcloud (x-axis: 90 degree, y-axis: 0 degree)
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  std::vector<Eigen::Vector3f> hesai_points = {
    Eigen::Vector3f(0.0f, 0.0f, 0.0f),
    Eigen::Vector3f(1.0f, -1.0f, 1.0f),
    Eigen::Vector3f(0.0f, -2.0f, 1.0f),
  };
  std::vector<float> hesai_azimuths = {
    autoware::universe_utils::pi / 2, autoware::universe_utils::pi * 3 / 4,
    autoware::universe_utils::pi};

  auto hesai_pointcloud = generate_pointcloud_msg(true, timestamp, hesai_points, hesai_azimuths);
  auto angle_conversion_opt =
    distortion_corrector_2d_->try_compute_angle_conversion(hesai_pointcloud);

  EXPECT_TRUE(angle_conversion_opt.has_value());
  EXPECT_EQ(angle_conversion_opt->sign, -1);
  EXPECT_NEAR(
    angle_conversion_opt->offset_rad, autoware::universe_utils::pi / 2, standard_tolerance);
}

TEST_F(DistortionCorrectorTest, TestTryComputeAngleConversionCartesianPointcloud)
{
  // test pointcloud that use cartesian coordinate for azimuth (x-axis: 0 degree, y-axis: 90 degree)
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  std::vector<Eigen::Vector3f> cartesian_points = {
    Eigen::Vector3f(0.0f, 0.0f, 0.0f),
    Eigen::Vector3f(1.0f, 1.0f, 1.0f),
    Eigen::Vector3f(0.0f, 2.0f, 1.0f),
  };
  std::vector<float> cartesian_azimuths = {
    0, autoware::universe_utils::pi / 4, autoware::universe_utils::pi / 2};

  auto cartesian_pointcloud =
    generate_pointcloud_msg(true, timestamp, cartesian_points, cartesian_azimuths);
  auto angle_conversion_opt =
    distortion_corrector_2d_->try_compute_angle_conversion(cartesian_pointcloud);

  EXPECT_TRUE(angle_conversion_opt.has_value());
  EXPECT_EQ(angle_conversion_opt->sign, 1);
  EXPECT_NEAR(angle_conversion_opt->offset_rad, 0, standard_tolerance);
}

TEST_F(DistortionCorrectorTest, TestTryComputeAngleConversionOnRandomPointcloud)
{
  // test pointcloud that use coordinate (x-axis: 270 degree, y-axis: 0 degree)
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  std::vector<Eigen::Vector3f> points = {
    Eigen::Vector3f(0.0f, 1.0f, 0.0f),
    Eigen::Vector3f(2.0f, 0.0f, 1.0f),
    Eigen::Vector3f(1.0f, 1.0f, 1.0f),
  };
  std::vector<float> azimuths = {
    0, autoware::universe_utils::pi * 3 / 2, autoware::universe_utils::pi * 7 / 4};

  auto pointcloud = generate_pointcloud_msg(true, timestamp, points, azimuths);
  auto angle_conversion_opt = distortion_corrector_2d_->try_compute_angle_conversion(pointcloud);

  EXPECT_TRUE(angle_conversion_opt.has_value());
  EXPECT_EQ(angle_conversion_opt->sign, 1);
  EXPECT_NEAR(
    angle_conversion_opt->offset_rad, autoware::universe_utils::pi * 3 / 2, standard_tolerance);
}

TEST_F(DistortionCorrectorTest, TestTryComputeAngleConversionOnBadAzimuthPointcloud)
{
  // test pointcloud that can cause the angle conversion to fail.
  // 1. angle difference is 0
  // 2. azimuth value is wrong
  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  // Generate and process multiple twist messages
  generate_and_process_twist_msgs(distortion_corrector_2d_, timestamp);

  std::vector<Eigen::Vector3f> points = {
    Eigen::Vector3f(0.0f, 1.0f, 0.0f),
    Eigen::Vector3f(2.0f, 0.0f, 1.0f),
    Eigen::Vector3f(1.0f, 1.0f, 1.0f),
  };

  // generate random bad azimuths
  std::vector<float> azimuths = {0, 0, autoware::universe_utils::pi};

  auto pointcloud = generate_pointcloud_msg(true, timestamp, points, azimuths);
  auto angle_conversion_opt = distortion_corrector_2d_->try_compute_angle_conversion(pointcloud);

  EXPECT_FALSE(angle_conversion_opt.has_value());
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);
  int ret = RUN_ALL_TESTS();
  rclcpp::shutdown();
  return ret;
}

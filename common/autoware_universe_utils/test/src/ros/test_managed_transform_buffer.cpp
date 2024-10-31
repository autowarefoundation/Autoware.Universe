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

#include "autoware/universe_utils/ros/managed_transform_buffer.hpp"

#include <rclcpp/rclcpp.hpp>
#include <tf2_eigen/tf2_eigen.hpp>

#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <gtest/gtest.h>
#include <tf2_ros/static_transform_broadcaster.h>

#include <chrono>
#include <memory>

class TestManagedTransformBuffer : public ::testing::Test
{
protected:
  std::shared_ptr<rclcpp::Node> node_{nullptr};
  std::shared_ptr<autoware::universe_utils::ManagedTransformBuffer> managed_tf_buffer_{nullptr};
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_broadcaster_;
  geometry_msgs::msg::TransformStamped tf_base_to_lidar_;
  Eigen::Matrix4f eigen_base_to_lidar_;
  std::unique_ptr<sensor_msgs::msg::PointCloud2> cloud_in_;
  rclcpp::Time time_;
  std::chrono::milliseconds timeout_;
  double precision_;

  geometry_msgs::msg::TransformStamped generateTransformMsg(
    const int32_t seconds, const uint32_t nanoseconds, const std::string & parent_frame,
    const std::string & child_frame, double x, double y, double z, double qx, double qy, double qz,
    double qw)
  {
    rclcpp::Time timestamp(seconds, nanoseconds, RCL_ROS_TIME);
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

  void SetUp() override
  {
    node_ = std::make_unique<rclcpp::Node>("test_managed_transform_buffer");
    managed_tf_buffer_ =
      std::make_unique<autoware::universe_utils::ManagedTransformBuffer>(node_.get(), true);
    tf_broadcaster_ = std::make_unique<tf2_ros::StaticTransformBroadcaster>(node_);

    tf_base_to_lidar_ = generateTransformMsg(
      10, 100'000'000, "base_link", "lidar_top", 0.690, 0.000, 2.100, -0.007, -0.007, 0.692, 0.722);
    eigen_base_to_lidar_ = tf2::transformToEigen(tf_base_to_lidar_).matrix().cast<float>();
    cloud_in_ = std::make_unique<sensor_msgs::msg::PointCloud2>();
    time_ = rclcpp::Time(0);
    timeout_ = std::chrono::milliseconds(100);
    precision_ = 0.01;

    // Set up the fields for x, y, and z coordinates
    cloud_in_->fields.resize(3);
    sensor_msgs::PointCloud2Modifier modifier(*cloud_in_);
    modifier.setPointCloud2FieldsByString(1, "xyz");

    // Resize the cloud to hold points_per_pointcloud_ points
    modifier.resize(10);

    // Create an iterator for the x, y, z fields
    sensor_msgs::PointCloud2Iterator<float> iter_x(*cloud_in_, "x");
    sensor_msgs::PointCloud2Iterator<float> iter_y(*cloud_in_, "y");
    sensor_msgs::PointCloud2Iterator<float> iter_z(*cloud_in_, "z");

    // Populate the point cloud
    for (size_t i = 0; i < modifier.size(); ++i, ++iter_x, ++iter_y, ++iter_z) {
      *iter_x = static_cast<float>(i);
      *iter_y = static_cast<float>(i);
      *iter_z = static_cast<float>(i);
    }

    // Set up cloud header
    cloud_in_->header.frame_id = "lidar_top";
    cloud_in_->header.stamp = rclcpp::Time(10, 100'000'000);

    ASSERT_TRUE(rclcpp::ok());
  }

  void TearDown() override { managed_tf_buffer_.reset(); }
};

TEST_F(TestManagedTransformBuffer, TestReturn)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "lidar_top", time_, timeout_);
  EXPECT_TRUE(eigen_transform.has_value());
  auto tf2_transform =
    managed_tf_buffer_->getTransform<tf2::Transform>("base_link", "lidar_top", time_, timeout_);
  ;
  EXPECT_TRUE(tf2_transform.has_value());
  auto tf_msg_transform = managed_tf_buffer_->getTransform<geometry_msgs::msg::TransformStamped>(
    "base_link", "lidar_top", time_, timeout_);
  ;
  EXPECT_TRUE(tf_msg_transform.has_value());
}

TEST_F(TestManagedTransformBuffer, TestTransformNoExist)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "fake_link", time_, timeout_);
  ;
  EXPECT_FALSE(eigen_transform.has_value());
}

TEST_F(TestManagedTransformBuffer, TestTransformBase)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto eigen_base_to_lidar =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "lidar_top", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_base_to_lidar.has_value());
  EXPECT_TRUE(eigen_base_to_lidar.value().isApprox(eigen_base_to_lidar_, precision_));
}

TEST_F(TestManagedTransformBuffer, TestTransformSameFrame)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto eigen_base_to_base =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "base_link", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_base_to_base.has_value());
  EXPECT_TRUE(eigen_base_to_base.value().isApprox(Eigen::Matrix4f::Identity(), precision_));
}

TEST_F(TestManagedTransformBuffer, TestTransformInverse)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto eigen_lidar_to_base =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("lidar_top", "base_link", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_lidar_to_base.has_value());
  EXPECT_TRUE(eigen_lidar_to_base.value().isApprox(eigen_base_to_lidar_.inverse(), precision_));
}

TEST_F(TestManagedTransformBuffer, TestTransformMultipleCall)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  std::optional<Eigen::Matrix4f> eigen_transform;
  eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "fake_link", time_, timeout_);
  ;
  EXPECT_FALSE(eigen_transform.has_value());
  eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("lidar_top", "base_link", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_transform.has_value());
  EXPECT_TRUE(eigen_transform.value().isApprox(eigen_base_to_lidar_.inverse(), precision_));
  eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("fake_link", "fake_link", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_transform.has_value());
  EXPECT_TRUE(eigen_transform.value().isApprox(Eigen::Matrix4f::Identity(), precision_));
  eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "lidar_top", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_transform.has_value());
  EXPECT_TRUE(eigen_transform.value().isApprox(eigen_base_to_lidar_, precision_));
  eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("fake_link", "lidar_top", time_, timeout_);
  ;
  EXPECT_FALSE(eigen_transform.has_value());
  eigen_transform =
    managed_tf_buffer_->getTransform<Eigen::Matrix4f>("base_link", "lidar_top", time_, timeout_);
  ;
  ASSERT_TRUE(eigen_transform.has_value());
  EXPECT_TRUE(eigen_transform.value().isApprox(eigen_base_to_lidar_, precision_));
}

TEST_F(TestManagedTransformBuffer, TestTransformEmptyPointCloud)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto cloud_in = std::make_unique<sensor_msgs::msg::PointCloud2>();
  cloud_in->header.frame_id = "lidar_top";
  cloud_in->header.stamp = rclcpp::Time(10, 100'000'000);
  auto cloud_out = std::make_unique<sensor_msgs::msg::PointCloud2>();

  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("lidar_top", *cloud_in, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("base_link", *cloud_in, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("fake_link", *cloud_in, *cloud_out, time_, timeout_));
}

TEST_F(TestManagedTransformBuffer, TestTransformEmptyPointCloudNoHeader)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto cloud_in = std::make_unique<sensor_msgs::msg::PointCloud2>();
  auto cloud_out = std::make_unique<sensor_msgs::msg::PointCloud2>();

  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("lidar_top", *cloud_in, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("base_link", *cloud_in, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("fake_link", *cloud_in, *cloud_out, time_, timeout_));
}

TEST_F(TestManagedTransformBuffer, TestTransformPointCloud)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto cloud_out = std::make_unique<sensor_msgs::msg::PointCloud2>();

  // Transform cloud with header
  EXPECT_TRUE(
    managed_tf_buffer_->transformPointcloud("lidar_top", *cloud_in_, *cloud_out, time_, timeout_));
  EXPECT_TRUE(
    managed_tf_buffer_->transformPointcloud("base_link", *cloud_in_, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("fake_link", *cloud_in_, *cloud_out, time_, timeout_));
}

TEST_F(TestManagedTransformBuffer, TestTransformPointCloudNoHeader)
{
  tf_broadcaster_->sendTransform(tf_base_to_lidar_);

  auto cloud_out = std::make_unique<sensor_msgs::msg::PointCloud2>();

  // Transform cloud without header
  auto cloud_in = std::make_unique<sensor_msgs::msg::PointCloud2>(*cloud_in_);
  cloud_in->header.frame_id = "";
  cloud_in->header.stamp = rclcpp::Time(0, 0);
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("lidar_top", *cloud_in, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("base_link", *cloud_in, *cloud_out, time_, timeout_));
  EXPECT_FALSE(
    managed_tf_buffer_->transformPointcloud("fake_link", *cloud_in, *cloud_out, time_, timeout_));
}

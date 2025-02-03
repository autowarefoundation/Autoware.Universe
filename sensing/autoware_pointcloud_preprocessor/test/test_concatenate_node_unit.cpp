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

#include "autoware/pointcloud_preprocessor/concatenate_data/cloud_collector.hpp"
#include "autoware/pointcloud_preprocessor/concatenate_data/combine_cloud_handler.hpp"
#include "autoware/pointcloud_preprocessor/concatenate_data/concatenate_and_time_sync_node.hpp"

#include <rclcpp/rclcpp.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <gtest/gtest.h>
#include <tf2_ros/static_transform_broadcaster.h>

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class ConcatenateCloudTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::NodeOptions node_options;
    // Instead of "input_topics", other parameters are not used.
    // They just helps to setup the concatenate node
    node_options.parameter_overrides(
      {{"use_cuda", false},
       {"debug_mode", false},
       {"has_static_tf_only", false},
       {"rosbag_length", 0.0},
       {"maximum_queue_size", 5},
       {"timeout_sec", 0.2},
       {"is_motion_compensated", true},
       {"publish_synchronized_pointcloud", true},
       {"keep_input_frame_in_synchronized_pointcloud", true},
       {"publish_previous_but_late_pointcloud", false},
       {"synchronized_pointcloud_postfix", "pointcloud"},
       {"input_twist_topic_type", "twist"},
       {"input_topics", std::vector<std::string>{"lidar_top", "lidar_left", "lidar_right"}},
       {"output_frame", "base_link"},
       {"matching_strategy.type", "advanced"},
       {"matching_strategy.lidar_timestamp_offsets", std::vector<double>{0.0, 0.04, 0.08}},
       {"matching_strategy.lidar_timestamp_noise_window", std::vector<double>{0.01, 0.01, 0.01}}});

    concatenate_node_ = std::make_shared<
      autoware::pointcloud_preprocessor::PointCloudConcatenateDataSynchronizerComponent>(
      node_options);
    combine_cloud_handler_ = std::make_shared<
      autoware::pointcloud_preprocessor::CombineCloudHandler<sensor_msgs::msg::PointCloud2>>(
      *concatenate_node_, "base_link", true, true, true, false);

    collector_ = std::make_shared<
      autoware::pointcloud_preprocessor::CloudCollector<sensor_msgs::msg::PointCloud2>>(
      std::dynamic_pointer_cast<
        autoware::pointcloud_preprocessor::PointCloudConcatenateDataSynchronizerComponent>(
        concatenate_node_->shared_from_this()),
      combine_cloud_handler_, number_of_pointcloud, timeout_sec, collector_debug_mode);

    // Setup TF
    tf_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(concatenate_node_);
    tf_broadcaster_->sendTransform(generate_static_transform_msgs());

    // Spin the node for a while to ensure transforms are published
    auto start = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(100);
    while (std::chrono::steady_clock::now() - start < timeout) {
      rclcpp::spin_some(concatenate_node_);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
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

  static sensor_msgs::msg::PointCloud2 generate_pointcloud_msg(
    bool generate_points, bool is_lidar_frame, const std::string & topic_name,
    const rclcpp::Time & stamp)
  {
    sensor_msgs::msg::PointCloud2 pointcloud_msg;
    pointcloud_msg.header.stamp = stamp;
    pointcloud_msg.header.frame_id = is_lidar_frame ? topic_name : "base_link";
    pointcloud_msg.height = 1;
    pointcloud_msg.is_dense = true;
    pointcloud_msg.is_bigendian = false;

    if (generate_points) {
      std::array<Eigen::Vector3f, number_of_points> points = {{
        Eigen::Vector3f(10.0f, 0.0f, 0.0f),  // point 1
        Eigen::Vector3f(0.0f, 10.0f, 0.0f),  // point 2
        Eigen::Vector3f(0.0f, 0.0f, 10.0f),  // point 3
      }};

      sensor_msgs::PointCloud2Modifier modifier(pointcloud_msg);
      modifier.setPointCloud2Fields(
        10, "x", 1, sensor_msgs::msg::PointField::FLOAT32, "y", 1,
        sensor_msgs::msg::PointField::FLOAT32, "z", 1, sensor_msgs::msg::PointField::FLOAT32,
        "intensity", 1, sensor_msgs::msg::PointField::UINT8, "return_type", 1,
        sensor_msgs::msg::PointField::UINT8, "channel", 1, sensor_msgs::msg::PointField::UINT16,
        "azimuth", 1, sensor_msgs::msg::PointField::FLOAT32, "elevation", 1,
        sensor_msgs::msg::PointField::FLOAT32, "distance", 1, sensor_msgs::msg::PointField::FLOAT32,
        "time_stamp", 1, sensor_msgs::msg::PointField::UINT32);

      modifier.resize(number_of_points);

      sensor_msgs::PointCloud2Iterator<float> iter_x(pointcloud_msg, "x");
      sensor_msgs::PointCloud2Iterator<float> iter_y(pointcloud_msg, "y");
      sensor_msgs::PointCloud2Iterator<float> iter_z(pointcloud_msg, "z");
      sensor_msgs::PointCloud2Iterator<std::uint32_t> iter_t(pointcloud_msg, "time_stamp");

      for (size_t i = 0; i < number_of_points; ++i) {
        *iter_x = points[i].x();
        *iter_y = points[i].y();
        *iter_z = points[i].z();
        *iter_t = 0;
        ++iter_x;
        ++iter_y;
        ++iter_z;
        ++iter_t;
      }
    } else {
      pointcloud_msg.width = 0;
      pointcloud_msg.row_step = 0;
    }

    return pointcloud_msg;
  }

  static std::vector<geometry_msgs::msg::TransformStamped> generate_static_transform_msgs()
  {
    // generate defined transformations
    return {
      generate_transform_msg("base_link", "lidar_top", 5.0, 5.0, 5.0, 0.683, 0.5, 0.183, 0.499),
      generate_transform_msg("base_link", "lidar_left", 1.0, 1.0, 3.0, 0.278, 0.717, 0.441, 0.453)};
    generate_transform_msg("base_link", "lidar_right", 1.0, 1.0, 3.0, 0.278, 0.717, 0.441, 0.453);
  }

  std::shared_ptr<autoware::pointcloud_preprocessor::PointCloudConcatenateDataSynchronizerComponent>
    concatenate_node_;
  std::shared_ptr<
    autoware::pointcloud_preprocessor::CombineCloudHandler<sensor_msgs::msg::PointCloud2>>
    combine_cloud_handler_;
  std::shared_ptr<autoware::pointcloud_preprocessor::CloudCollector<sensor_msgs::msg::PointCloud2>>
    collector_;
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> tf_broadcaster_;

  static constexpr int32_t timestamp_seconds{10};
  static constexpr uint32_t timestamp_nanoseconds{100'000'000};
  static constexpr size_t number_of_points{3};
  static constexpr float standard_tolerance{1e-4};
  static constexpr int number_of_pointcloud{3};
  static constexpr float timeout_sec{0.2};
  static constexpr bool collector_debug_mode{false};  // For showing collector information
  bool debug_{false};                                 // For the Unit test
};

//////////////////////////////// Test combine_cloud_handler ////////////////////////////////
TEST_F(ConcatenateCloudTest, TestProcessTwist)
{
  auto twist_msg = std::make_shared<geometry_msgs::msg::TwistWithCovarianceStamped>();
  twist_msg->header.stamp = rclcpp::Time(10, 0);
  twist_msg->twist.twist.linear.x = 1.0;
  twist_msg->twist.twist.angular.z = 0.1;

  combine_cloud_handler_->process_twist(twist_msg);

  ASSERT_FALSE(combine_cloud_handler_->get_twist_queue().empty());
  EXPECT_EQ(combine_cloud_handler_->get_twist_queue().front().twist.linear.x, 1.0);
  EXPECT_EQ(combine_cloud_handler_->get_twist_queue().front().twist.angular.z, 0.1);
}

TEST_F(ConcatenateCloudTest, TestProcessOdometry)
{
  auto odom_msg = std::make_shared<nav_msgs::msg::Odometry>();
  odom_msg->header.stamp = rclcpp::Time(10, 0);
  odom_msg->twist.twist.linear.x = 1.0;
  odom_msg->twist.twist.angular.z = 0.1;

  combine_cloud_handler_->process_odometry(odom_msg);

  ASSERT_FALSE(combine_cloud_handler_->get_twist_queue().empty());
  EXPECT_EQ(combine_cloud_handler_->get_twist_queue().front().twist.linear.x, 1.0);
  EXPECT_EQ(combine_cloud_handler_->get_twist_queue().front().twist.angular.z, 0.1);
}

TEST_F(ConcatenateCloudTest, TestComputeTransformToAdjustForOldTimestamp)
{
  // If time difference between twist msg and pointcloud stamp is more than 100 miliseconds, return
  // Identity transformation. case 1: time difference larger than 100 miliseconds
  rclcpp::Time pointcloud_stamp1(10, 100'000'000, RCL_ROS_TIME);
  rclcpp::Time pointcloud_stamp2(10, 210'000'000, RCL_ROS_TIME);
  auto twist_msg1 = std::make_shared<geometry_msgs::msg::TwistWithCovarianceStamped>();
  twist_msg1->header.stamp = rclcpp::Time(9, 130'000'000, RCL_ROS_TIME);
  twist_msg1->twist.twist.linear.x = 1.0;
  twist_msg1->twist.twist.angular.z = 0.1;
  combine_cloud_handler_->process_twist(twist_msg1);

  auto twist_msg2 = std::make_shared<geometry_msgs::msg::TwistWithCovarianceStamped>();
  twist_msg2->header.stamp = rclcpp::Time(9, 160'000'000, RCL_ROS_TIME);
  twist_msg2->twist.twist.linear.x = 1.0;
  twist_msg2->twist.twist.angular.z = 0.1;
  combine_cloud_handler_->process_twist(twist_msg2);

  Eigen::Matrix4f transform = combine_cloud_handler_->compute_transform_to_adjust_for_old_timestamp(
    pointcloud_stamp1, pointcloud_stamp2);

  // translation
  EXPECT_NEAR(transform(0, 3), 0.0, standard_tolerance);
  EXPECT_NEAR(transform(1, 3), 0.0, standard_tolerance);

  EXPECT_NEAR(transform(0, 0), 1.0, standard_tolerance);
  EXPECT_NEAR(transform(0, 1), 0.0, standard_tolerance);
  EXPECT_NEAR(transform(1, 0), 0.0, standard_tolerance);
  EXPECT_NEAR(transform(1, 1), 1.0, standard_tolerance);

  std::ostringstream oss;
  oss << "Transformation matrix from cloud 2 to cloud 1:\n" << transform;

  if (debug_) {
    RCLCPP_INFO(concatenate_node_->get_logger(), "%s", oss.str().c_str());
  }

  // case 2: time difference smaller than 100 miliseconds
  rclcpp::Time pointcloud_stamp3(11, 100'000'000, RCL_ROS_TIME);
  rclcpp::Time pointcloud_stamp4(11, 150'000'000, RCL_ROS_TIME);
  auto twist_msg3 = std::make_shared<geometry_msgs::msg::TwistWithCovarianceStamped>();
  twist_msg3->header.stamp = rclcpp::Time(11, 130'000'000, RCL_ROS_TIME);
  twist_msg3->twist.twist.linear.x = 1.0;
  twist_msg3->twist.twist.angular.z = 0.1;
  combine_cloud_handler_->process_twist(twist_msg3);

  auto twist_msg4 = std::make_shared<geometry_msgs::msg::TwistWithCovarianceStamped>();
  twist_msg4->header.stamp = rclcpp::Time(11, 160'000'000, RCL_ROS_TIME);
  twist_msg4->twist.twist.linear.x = 1.0;
  twist_msg4->twist.twist.angular.z = 0.1;
  combine_cloud_handler_->process_twist(twist_msg4);

  transform = combine_cloud_handler_->compute_transform_to_adjust_for_old_timestamp(
    pointcloud_stamp3, pointcloud_stamp4);

  // translation
  EXPECT_NEAR(transform(0, 3), 0.0499996, standard_tolerance);
  EXPECT_NEAR(transform(1, 3), 0.000189999, standard_tolerance);

  // rotation, yaw = 0.005
  EXPECT_NEAR(transform(0, 0), 0.999987, standard_tolerance);
  EXPECT_NEAR(transform(0, 1), -0.00499998, standard_tolerance);
  EXPECT_NEAR(transform(1, 0), 0.00499998, standard_tolerance);
  EXPECT_NEAR(transform(1, 1), 0.999987, standard_tolerance);

  oss.str("");
  oss.clear();
  oss << "Transformation matrix from cloud 4 to cloud 3:\n" << transform;

  if (debug_) {
    RCLCPP_INFO(concatenate_node_->get_logger(), "%s", oss.str().c_str());
  }
}

//////////////////////////////// Test cloud_collector ////////////////////////////////

TEST_F(ConcatenateCloudTest, TestSetAndGetNaiveCollectorInfo)
{
  auto naive_info = std::make_shared<autoware::pointcloud_preprocessor::NaiveCollectorInfo>();
  naive_info->timestamp = 15.0;

  collector_->set_info(naive_info);
  auto collector_info_new = collector_->get_info();

  auto naive_info_new =
    std::dynamic_pointer_cast<autoware::pointcloud_preprocessor::NaiveCollectorInfo>(
      collector_info_new);
  ASSERT_NE(naive_info_new, nullptr) << "Collector info is not of type NaiveCollectorInfo";

  EXPECT_DOUBLE_EQ(naive_info_new->timestamp, 15.0);
}

TEST_F(ConcatenateCloudTest, TestSetAndGetAdvancedCollectorInfo)
{
  auto advanced_info = std::make_shared<autoware::pointcloud_preprocessor::AdvancedCollectorInfo>();
  advanced_info->timestamp = 10.0;
  advanced_info->noise_window = 0.1;
  collector_->set_info(advanced_info);
  auto collector_info_new = collector_->get_info();
  auto advanced_info_new =
    std::dynamic_pointer_cast<autoware::pointcloud_preprocessor::AdvancedCollectorInfo>(
      collector_info_new);
  ASSERT_NE(advanced_info_new, nullptr) << "Collector info is not of type AdvancedCollectorInfo";

  // Validate the values
  auto min = advanced_info_new->timestamp - advanced_info_new->noise_window;
  auto max = advanced_info_new->timestamp + advanced_info_new->noise_window;
  EXPECT_DOUBLE_EQ(min, 9.9);
  EXPECT_DOUBLE_EQ(max, 10.1);
}

TEST_F(ConcatenateCloudTest, TestConcatenateClouds)
{
  rclcpp::Time top_timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  rclcpp::Time left_timestamp(timestamp_seconds, timestamp_nanoseconds + 40'000'000, RCL_ROS_TIME);
  rclcpp::Time right_timestamp(timestamp_seconds, timestamp_nanoseconds + 80'000'000, RCL_ROS_TIME);
  sensor_msgs::msg::PointCloud2 top_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_top", top_timestamp);
  sensor_msgs::msg::PointCloud2 left_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_left", left_timestamp);
  sensor_msgs::msg::PointCloud2 right_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_right", right_timestamp);

  sensor_msgs::msg::PointCloud2::SharedPtr top_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(top_pointcloud);
  sensor_msgs::msg::PointCloud2::SharedPtr left_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(left_pointcloud);
  sensor_msgs::msg::PointCloud2::SharedPtr right_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(right_pointcloud);

  std::unordered_map<std::string, sensor_msgs::msg::PointCloud2::ConstSharedPtr> topic_to_cloud_map;
  topic_to_cloud_map["lidar_top"] = top_pointcloud_ptr;
  topic_to_cloud_map["lidar_left"] = left_pointcloud_ptr;
  topic_to_cloud_map["lidar_right"] = right_pointcloud_ptr;

  auto [concatenate_cloud_ptr, topic_to_transformed_cloud_map, topic_to_original_stamp_map] =
    collector_->concatenate_pointclouds(topic_to_cloud_map);

  // test output concatenate cloud
  // No input twist, so it will not do the motion compensation
  std::array<Eigen::Vector3f, 10> expected_pointcloud = {
    {Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f),
     Eigen::Vector3f(0.0f, 0.0f, 10.0f), Eigen::Vector3f(10.0f, 0.0f, 0.0f),
     Eigen::Vector3f(0.0f, 10.0f, 0.0f), Eigen::Vector3f(0.0f, 0.0f, 10.0f),
     Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f),
     Eigen::Vector3f(0.0f, 0.0f, 10.0f)}};

  size_t i = 0;
  std::ostringstream oss;
  oss << "Concatenated pointcloud:\n";

  sensor_msgs::PointCloud2Iterator<float> iter_x(*concatenate_cloud_ptr, "x");
  sensor_msgs::PointCloud2Iterator<float> iter_y(*concatenate_cloud_ptr, "y");
  sensor_msgs::PointCloud2Iterator<float> iter_z(*concatenate_cloud_ptr, "z");

  for (; iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z, ++i) {
    oss << "Concatenated point " << i << ": (" << *iter_x << ", " << *iter_y << ", " << *iter_z
        << ")\n";
    EXPECT_FLOAT_EQ(*iter_x, expected_pointcloud[i].x());
    EXPECT_FLOAT_EQ(*iter_y, expected_pointcloud[i].y());
    EXPECT_FLOAT_EQ(*iter_z, expected_pointcloud[i].z());
  }

  if (debug_) {
    RCLCPP_INFO(concatenate_node_->get_logger(), "%s", oss.str().c_str());
  }

  // test concatenate cloud has the oldest pointcloud's timestamp
  EXPECT_FLOAT_EQ(
    top_timestamp.seconds(), rclcpp::Time(concatenate_cloud_ptr->header.stamp).seconds());

  // test separated transformed cloud
  std::array<Eigen::Vector3f, 10> expected_top_pointcloud = {
    {Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f),
     Eigen::Vector3f(0.0f, 0.0f, 10.0f)}};
  std::array<Eigen::Vector3f, 10> expected_left_pointcloud = {
    {Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f),
     Eigen::Vector3f(0.0f, 0.0f, 10.0f)}};
  std::array<Eigen::Vector3f, 10> expected_right_pointcloud = {
    {Eigen::Vector3f(10.0f, 0.0f, 0.0f), Eigen::Vector3f(0.0f, 10.0f, 0.0f),
     Eigen::Vector3f(0.0f, 0.0f, 10.0f)}};

  oss.clear();
  oss.str("");
  i = 0;

  sensor_msgs::PointCloud2Iterator<float> top_pc_iter_x(
    *(topic_to_transformed_cloud_map.value().at("lidar_top")), "x");
  sensor_msgs::PointCloud2Iterator<float> top_pc_iter_y(
    *(topic_to_transformed_cloud_map.value().at("lidar_top")), "y");
  sensor_msgs::PointCloud2Iterator<float> top_pc_iter_z(
    *(topic_to_transformed_cloud_map.value().at("lidar_top")), "z");

  for (; top_pc_iter_x != top_pc_iter_x.end();
       ++top_pc_iter_x, ++top_pc_iter_y, ++top_pc_iter_z, ++i) {
    oss << "Top point " << i << ": (" << *top_pc_iter_x << ", " << *top_pc_iter_y << ", "
        << *top_pc_iter_z << ")\n";
    EXPECT_FLOAT_EQ(*top_pc_iter_x, expected_top_pointcloud[i].x());
    EXPECT_FLOAT_EQ(*top_pc_iter_y, expected_top_pointcloud[i].y());
    EXPECT_FLOAT_EQ(*top_pc_iter_z, expected_top_pointcloud[i].z());
  }

  if (debug_) {
    RCLCPP_INFO(concatenate_node_->get_logger(), "%s", oss.str().c_str());
  }

  oss.clear();
  oss.str("");
  i = 0;
  sensor_msgs::PointCloud2Iterator<float> left_pc_iter_x(
    *(topic_to_transformed_cloud_map.value().at("lidar_left")), "x");
  sensor_msgs::PointCloud2Iterator<float> left_pc_iter_y(
    *(topic_to_transformed_cloud_map.value().at("lidar_left")), "y");
  sensor_msgs::PointCloud2Iterator<float> left_pc_iter_z(
    *(topic_to_transformed_cloud_map.value().at("lidar_left")), "z");

  for (; left_pc_iter_x != left_pc_iter_x.end();
       ++left_pc_iter_x, ++left_pc_iter_y, ++left_pc_iter_z, ++i) {
    oss << "Left point " << i << ": (" << *left_pc_iter_x << ", " << *left_pc_iter_y << ", "
        << *left_pc_iter_z << ")\n";
    EXPECT_FLOAT_EQ(*left_pc_iter_x, expected_left_pointcloud[i].x());
    EXPECT_FLOAT_EQ(*left_pc_iter_y, expected_left_pointcloud[i].y());
    EXPECT_FLOAT_EQ(*left_pc_iter_z, expected_left_pointcloud[i].z());
  }

  if (debug_) {
    RCLCPP_INFO(concatenate_node_->get_logger(), "%s", oss.str().c_str());
  }

  oss.clear();
  oss.str("");
  i = 0;
  sensor_msgs::PointCloud2Iterator<float> right_pc_iter_x(
    *(topic_to_transformed_cloud_map.value().at("lidar_right")), "x");
  sensor_msgs::PointCloud2Iterator<float> right_pc_iter_y(
    *(topic_to_transformed_cloud_map.value().at("lidar_right")), "y");
  sensor_msgs::PointCloud2Iterator<float> right_pc_iter_z(
    *(topic_to_transformed_cloud_map.value().at("lidar_right")), "z");

  for (; right_pc_iter_x != right_pc_iter_x.end();
       ++right_pc_iter_x, ++right_pc_iter_y, ++right_pc_iter_z, ++i) {
    oss << "Right point " << i << ": (" << *right_pc_iter_x << ", " << *right_pc_iter_y << ", "
        << *right_pc_iter_z << ")\n";
    EXPECT_FLOAT_EQ(*right_pc_iter_x, expected_right_pointcloud[i].x());
    EXPECT_FLOAT_EQ(*right_pc_iter_y, expected_right_pointcloud[i].y());
    EXPECT_FLOAT_EQ(*right_pc_iter_z, expected_right_pointcloud[i].z());
  }

  if (debug_) {
    RCLCPP_INFO(concatenate_node_->get_logger(), "%s", oss.str().c_str());
  }

  // test original cloud's timestamps
  EXPECT_FLOAT_EQ(top_timestamp.seconds(), topic_to_original_stamp_map["lidar_top"]);
  EXPECT_FLOAT_EQ(left_timestamp.seconds(), topic_to_original_stamp_map["lidar_left"]);
  EXPECT_FLOAT_EQ(right_timestamp.seconds(), topic_to_original_stamp_map["lidar_right"]);
}

TEST_F(ConcatenateCloudTest, TestProcessSingleCloud)
{
  concatenate_node_->add_cloud_collector(collector_);

  rclcpp::Time timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  sensor_msgs::msg::PointCloud2 top_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_top", timestamp);
  sensor_msgs::msg::PointCloud2::SharedPtr top_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(top_pointcloud);
  collector_->process_pointcloud("lidar_top", top_pointcloud_ptr);

  auto topic_to_cloud_map = collector_->get_topic_to_cloud_map();
  EXPECT_EQ(topic_to_cloud_map["lidar_top"], top_pointcloud_ptr);
  EXPECT_FALSE(collector_->concatenate_finished());
  concatenate_node_->manage_collector_list();
  EXPECT_FALSE(concatenate_node_->get_cloud_collectors<sensor_msgs::msg::PointCloud2>().empty());

  // Sleep for timeout seconds (200 ms)
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  rclcpp::spin_some(concatenate_node_);

  // Collector should concatenate and publish the pointcloud, also delete itself.
  EXPECT_TRUE(collector_->concatenate_finished());
  concatenate_node_->manage_collector_list();
  EXPECT_TRUE(concatenate_node_->get_cloud_collectors<sensor_msgs::msg::PointCloud2>().empty());
}

TEST_F(ConcatenateCloudTest, TestProcessMultipleCloud)
{
  concatenate_node_->add_cloud_collector(collector_);

  rclcpp::Time top_timestamp(timestamp_seconds, timestamp_nanoseconds, RCL_ROS_TIME);
  rclcpp::Time left_timestamp(timestamp_seconds, timestamp_nanoseconds + 40'000'000, RCL_ROS_TIME);
  rclcpp::Time right_timestamp(timestamp_seconds, timestamp_nanoseconds + 80'000'000, RCL_ROS_TIME);
  sensor_msgs::msg::PointCloud2 top_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_top", top_timestamp);
  sensor_msgs::msg::PointCloud2 left_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_left", left_timestamp);
  sensor_msgs::msg::PointCloud2 right_pointcloud =
    generate_pointcloud_msg(true, false, "lidar_right", right_timestamp);

  sensor_msgs::msg::PointCloud2::SharedPtr top_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(top_pointcloud);
  sensor_msgs::msg::PointCloud2::SharedPtr left_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(left_pointcloud);
  sensor_msgs::msg::PointCloud2::SharedPtr right_pointcloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>(right_pointcloud);

  collector_->process_pointcloud("lidar_top", top_pointcloud_ptr);
  collector_->process_pointcloud("lidar_left", left_pointcloud_ptr);
  collector_->process_pointcloud("lidar_right", right_pointcloud_ptr);

  EXPECT_TRUE(collector_->concatenate_finished());
  concatenate_node_->manage_collector_list();
  EXPECT_TRUE(concatenate_node_->get_cloud_collectors<sensor_msgs::msg::PointCloud2>().empty());
}

int main(int argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);
  int ret = RUN_ALL_TESTS();
  rclcpp::shutdown();
  return ret;
}

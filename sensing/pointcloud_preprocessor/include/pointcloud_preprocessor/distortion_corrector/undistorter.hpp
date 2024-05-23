// Copyright 2020 Tier IV, Inc.
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

#ifndef POINTCLOUD_PREPROCESSOR__DISTORTION_CORRECTOR__UNDISTORTER_HPP_
#define POINTCLOUD_PREPROCESSOR__DISTORTION_CORRECTOR__UNDISTORTER_HPP_

#include <Eigen/Core>
#include <rclcpp/rclcpp.hpp>
#include <sophus/se3.hpp>

#include <geometry_msgs/msg/twist_stamped.hpp>
#include <geometry_msgs/msg/twist_with_covariance_stamped.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <tf2/convert.h>
#include <tf2/transform_datatypes.h>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

// Include tier4 autoware utils
#include <tier4_autoware_utils/ros/debug_publisher.hpp>
#include <tier4_autoware_utils/system/stop_watch.hpp>

#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace pointcloud_preprocessor
{
class Undistorter
{
public:
  bool is_pointcloud_transform_needed = false;
  bool is_pointcloud_transfrom_exist = false;
  bool is_imu_transfrom_exist = false;
  tf2_ros::Buffer & tf2_buffer;
  geometry_msgs::msg::TransformStamped::SharedPtr geometry_imu_to_base_link_ptr;

  explicit Undistorter(tf2_ros::Buffer & buffer) : tf2_buffer(buffer) {}
  void setIMUTransform(const std::string & target_frame, const std::string & source_frame);

  virtual void initialize() = 0;
  virtual void undistortPoint(
    sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
    sensor_msgs::PointCloud2Iterator<float> & it_z,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, float time_offset,
    bool is_twist_valid, bool is_imu_valid) = 0;

  virtual void setPointCloudTransform(
    const std::string & base_link_frame, const std::string & lidar_frame) = 0;
};

class Undistorter2D : public Undistorter
{
public:
  // defined outside of for loop for performance reasons.
  tf2::Quaternion baselink_quat;
  tf2::Transform baselink_tf_odom;
  tf2::Vector3 point_tf;
  tf2::Vector3 undistorted_point_tf;
  float theta;
  float x;
  float y;

  // TF
  tf2::Transform tf2_lidar_to_base_link;
  tf2::Transform tf2_base_link_to_lidar;

  explicit Undistorter2D(tf2_ros::Buffer & buffer) : Undistorter(buffer) {}
  void initialize() override;
  void undistortPoint(
    sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
    sensor_msgs::PointCloud2Iterator<float> & it_z,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, float time_offset,
    bool is_twist_valid, bool is_imu_valid) override;

  void setPointCloudTransform(const std::string & base_link_frame, const std::string & lidar_frame);
};

class Undistorter3D : public Undistorter
{
public:
  // defined outside of for loop for performance reasons.
  Eigen::Vector4f point_eigen;
  Eigen::Vector4f undistorted_point_eigen;
  Eigen::Matrix4f transformation_matrix;
  Eigen::Matrix4f prev_transformation_matrix;

  // TF
  Eigen::Matrix4f eigen_lidar_to_base_link;
  Eigen::Matrix4f eigen_base_link_to_lidar;

  explicit Undistorter3D(tf2_ros::Buffer & buffer) : Undistorter(buffer) {}
  void initialize() override;
  void undistortPoint(
    sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
    sensor_msgs::PointCloud2Iterator<float> & it_z,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, float time_offset,
    bool is_twist_valid, bool is_imu_valid) override;
  void setPointCloudTransform(const std::string & base_link_frame, const std::string & lidar_frame);
};

}  // namespace pointcloud_preprocessor

#endif  // POINTCLOUD_PREPROCESSOR__DISTORTION_CORRECTOR__UNDISTORTER_HPP_

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

#ifndef AUTOWARE__POINTCLOUD_PREPROCESSOR__DISTORTION_CORRECTOR__DISTORTION_CORRECTOR_HPP_
#define AUTOWARE__POINTCLOUD_PREPROCESSOR__DISTORTION_CORRECTOR__DISTORTION_CORRECTOR_HPP_

#include <Eigen/Core>
#include <autoware/universe_utils/ros/managed_transform_buffer.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sophus/se3.hpp>

#include <geometry_msgs/msg/twist_stamped.hpp>
#include <geometry_msgs/msg/twist_with_covariance_stamped.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <tf2/LinearMath/Transform.h>
#include <tf2/convert.h>
#include <tf2/transform_datatypes.h>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

#include <tf2_ros/buffer.h>
#include <tf2_ros/buffer_interface.h>
#include <tf2_ros/transform_listener.h>

#include <deque>
#include <memory>
#include <string>

namespace autoware::pointcloud_preprocessor
{

struct AngleConversion
{
  // Equation for the conversion between sensor azimuth coordinates and Cartesian coordinates:
  // sensor azimuth coordinates = offset_rad + sign * cartesian coordinates;
  // offset_rad is restricted to be a multiple of 90, and sign is restricted to be 1 or -1.
  float offset_rad{0};
  float sign{1};
  static constexpr float offset_rad_threshold{(5.0f / 180.0f) * M_PI};

  static constexpr float sign_threshold{0.1f};
};

class DistortionCorrectorBase
{
protected:
  geometry_msgs::msg::TransformStamped::SharedPtr geometry_imu_to_base_link_ptr_;
  bool pointcloud_transform_needed_{false};
  bool pointcloud_transform_exists_{false};
  bool imu_transform_exists_{false};
  std::unique_ptr<autoware::universe_utils::ManagedTransformBuffer> managed_tf_buffer_{nullptr};

  std::deque<geometry_msgs::msg::TwistStamped> twist_queue_;
  std::deque<geometry_msgs::msg::Vector3Stamped> angular_velocity_queue_;

  rclcpp::Node & node_;

  void get_imu_transformation(const std::string & base_frame, const std::string & imu_frame);
  void enqueue_imu(const sensor_msgs::msg::Imu::ConstSharedPtr imu_msg);
  void get_twist_and_imu_iterator(
    bool use_imu, double first_point_time_stamp_sec,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu);
  void warn_if_timestamp_is_too_late(
    bool is_twist_time_stamp_too_late, bool is_imu_time_stamp_too_late);
  static tf2::Transform convert_matrix_to_transform(const Eigen::Matrix4f & matrix);

public:
  explicit DistortionCorrectorBase(rclcpp::Node & node, const bool & has_static_tf_only)
  : node_(node)
  {
    managed_tf_buffer_ =
      std::make_unique<autoware::universe_utils::ManagedTransformBuffer>(&node, has_static_tf_only);
  }

  virtual ~DistortionCorrectorBase() = default;

  [[nodiscard]] bool pointcloud_transform_exists() const;
  [[nodiscard]] bool pointcloud_transform_needed() const;
  std::deque<geometry_msgs::msg::TwistStamped> get_twist_queue();
  std::deque<geometry_msgs::msg::Vector3Stamped> get_angular_velocity_queue();
  void process_twist_message(
    const geometry_msgs::msg::TwistWithCovarianceStamped::ConstSharedPtr twist_msg);

  void process_imu_message(
    const std::string & base_frame, const sensor_msgs::msg::Imu::ConstSharedPtr imu_msg);

  std::optional<AngleConversion> try_compute_angle_conversion(
    sensor_msgs::msg::PointCloud2 & pointcloud);

  bool is_pointcloud_valid(sensor_msgs::msg::PointCloud2 & pointcloud);

  virtual void set_pointcloud_transform(
    const std::string & base_frame, const std::string & lidar_frame) = 0;
  virtual void initialize() = 0;
  virtual void undistort_pointcloud(
    bool use_imu, std::optional<AngleConversion> angle_conversion_opt,
    sensor_msgs::msg::PointCloud2 & pointcloud) = 0;
};

template <class T>
class DistortionCorrector : public DistortionCorrectorBase
{
public:
  explicit DistortionCorrector(rclcpp::Node & node, const bool & has_static_tf_only)
  : DistortionCorrectorBase(node, has_static_tf_only)
  {
  }

  void undistort_pointcloud(
    bool use_imu, std::optional<AngleConversion> angle_conversion_opt,
    sensor_msgs::msg::PointCloud2 & pointcloud) override;

  void undistort_point(
    sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
    sensor_msgs::PointCloud2Iterator<float> & it_z,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, float const & time_offset,
    const bool & is_twist_valid, const bool & is_imu_valid)
  {
    static_cast<T *>(this)->undistort_point_implementation(
      it_x, it_y, it_z, it_twist, it_imu, time_offset, is_twist_valid, is_imu_valid);
  };
};

class DistortionCorrector2D : public DistortionCorrector<DistortionCorrector2D>
{
private:
  // defined outside of for loop for performance reasons.
  tf2::Quaternion baselink_quat_;
  tf2::Transform baselink_tf_odom_;
  tf2::Vector3 point_tf_;
  tf2::Vector3 undistorted_point_tf_;
  float theta_;
  float x_;
  float y_;

  // TF
  tf2::Transform tf2_lidar_to_base_link_;
  tf2::Transform tf2_base_link_to_lidar_;

public:
  explicit DistortionCorrector2D(rclcpp::Node & node, const bool & has_static_tf_only)
  : DistortionCorrector(node, has_static_tf_only)
  {
  }
  void initialize() override;
  void set_pointcloud_transform(
    const std::string & base_frame, const std::string & lidar_frame) override;
  void undistort_point_implementation(
    sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
    sensor_msgs::PointCloud2Iterator<float> & it_z,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, const float & time_offset,
    const bool & is_twist_valid, const bool & is_imu_valid);
};

class DistortionCorrector3D : public DistortionCorrector<DistortionCorrector3D>
{
private:
  // defined outside of for loop for performance reasons.
  Eigen::Vector4f point_eigen_;
  Eigen::Vector4f undistorted_point_eigen_;
  Eigen::Matrix4f transformation_matrix_;
  Eigen::Matrix4f prev_transformation_matrix_;

  // TF
  Eigen::Matrix4f eigen_lidar_to_base_link_;
  Eigen::Matrix4f eigen_base_link_to_lidar_;

public:
  explicit DistortionCorrector3D(rclcpp::Node & node, const bool & has_static_tf_only)
  : DistortionCorrector(node, has_static_tf_only)
  {
  }
  void initialize() override;
  void set_pointcloud_transform(
    const std::string & base_frame, const std::string & lidar_frame) override;
  void undistort_point_implementation(
    sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
    sensor_msgs::PointCloud2Iterator<float> & it_z,
    std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
    std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, const float & time_offset,
    const bool & is_twist_valid, const bool & is_imu_valid);
};

}  // namespace autoware::pointcloud_preprocessor

#endif  // AUTOWARE__POINTCLOUD_PREPROCESSOR__DISTORTION_CORRECTOR__DISTORTION_CORRECTOR_HPP_

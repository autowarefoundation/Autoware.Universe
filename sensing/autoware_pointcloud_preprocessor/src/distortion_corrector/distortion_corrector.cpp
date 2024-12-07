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

#include "autoware/pointcloud_preprocessor/distortion_corrector/distortion_corrector.hpp"

#include "autoware/pointcloud_preprocessor/utility/memory.hpp"
#include "autoware/universe_utils/math/constants.hpp"

#include <autoware/universe_utils/math/trigonometry.hpp>
#include <tf2_eigen/tf2_eigen.hpp>

#include <deque>
#include <memory>
#include <string>

namespace autoware::pointcloud_preprocessor
{

bool DistortionCorrectorBase::pointcloud_transform_exists() const
{
  return pointcloud_transform_exists_;
}

bool DistortionCorrectorBase::pointcloud_transform_needed() const
{
  return pointcloud_transform_needed_;
}

std::deque<geometry_msgs::msg::TwistStamped> DistortionCorrectorBase::get_twist_queue()
{
  return twist_queue_;
}

std::deque<geometry_msgs::msg::Vector3Stamped> DistortionCorrectorBase::get_angular_velocity_queue()
{
  return angular_velocity_queue_;
}

void DistortionCorrectorBase::process_twist_message(
  const geometry_msgs::msg::TwistWithCovarianceStamped::ConstSharedPtr twist_msg)
{
  geometry_msgs::msg::TwistStamped msg;
  msg.header = twist_msg->header;
  msg.twist = twist_msg->twist.twist;

  // If time jumps backwards (e.g. when a rosbag restarts), clear buffer
  if (!twist_queue_.empty()) {
    if (rclcpp::Time(twist_queue_.front().header.stamp) > rclcpp::Time(msg.header.stamp)) {
      twist_queue_.clear();
    }
  }

  // Twist data in the queue that is older than the current twist by 1 second will be cleared.
  auto cutoff_time = rclcpp::Time(msg.header.stamp) - rclcpp::Duration::from_seconds(1.0);

  while (!twist_queue_.empty()) {
    if (rclcpp::Time(twist_queue_.front().header.stamp) > cutoff_time) {
      break;
    }
    twist_queue_.pop_front();
  }

  twist_queue_.push_back(msg);
}

void DistortionCorrectorBase::process_imu_message(
  const std::string & base_frame, const sensor_msgs::msg::Imu::ConstSharedPtr imu_msg)
{
  get_imu_transformation(base_frame, imu_msg->header.frame_id);
  enqueue_imu(imu_msg);
}

void DistortionCorrectorBase::get_imu_transformation(
  const std::string & base_frame, const std::string & imu_frame)
{
  if (imu_transform_exists_) {
    return;
  }

  Eigen::Matrix4f eigen_imu_to_base_link;
  imu_transform_exists_ =
    managed_tf_buffer_->getTransform(base_frame, imu_frame, eigen_imu_to_base_link);
  tf2::Transform tf2_imu_to_base_link = convert_matrix_to_transform(eigen_imu_to_base_link);

  geometry_imu_to_base_link_ptr_ = std::make_shared<geometry_msgs::msg::TransformStamped>();
  geometry_imu_to_base_link_ptr_->transform.rotation =
    tf2::toMsg(tf2_imu_to_base_link.getRotation());
}

void DistortionCorrectorBase::enqueue_imu(const sensor_msgs::msg::Imu::ConstSharedPtr imu_msg)
{
  geometry_msgs::msg::Vector3Stamped angular_velocity;
  angular_velocity.vector = imu_msg->angular_velocity;

  geometry_msgs::msg::Vector3Stamped transformed_angular_velocity;
  tf2::doTransform(angular_velocity, transformed_angular_velocity, *geometry_imu_to_base_link_ptr_);
  transformed_angular_velocity.header = imu_msg->header;

  // If time jumps backwards (e.g. when a rosbag restarts), clear buffer
  if (!angular_velocity_queue_.empty()) {
    if (
      rclcpp::Time(angular_velocity_queue_.front().header.stamp) >
      rclcpp::Time(imu_msg->header.stamp)) {
      angular_velocity_queue_.clear();
    }
  }

  // IMU data in the queue that is older than the current imu msg by 1 second will be cleared.
  auto cutoff_time = rclcpp::Time(imu_msg->header.stamp) - rclcpp::Duration::from_seconds(1.0);

  while (!angular_velocity_queue_.empty()) {
    if (rclcpp::Time(angular_velocity_queue_.front().header.stamp) > cutoff_time) {
      break;
    }
    angular_velocity_queue_.pop_front();
  }

  angular_velocity_queue_.push_back(transformed_angular_velocity);
}

void DistortionCorrectorBase::get_twist_and_imu_iterator(
  bool use_imu, double first_point_time_stamp_sec,
  std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
  std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu)
{
  it_twist = std::lower_bound(
    std::begin(twist_queue_), std::end(twist_queue_), first_point_time_stamp_sec,
    [](const geometry_msgs::msg::TwistStamped & x, const double t) {
      return rclcpp::Time(x.header.stamp).seconds() < t;
    });
  it_twist = it_twist == std::end(twist_queue_) ? std::end(twist_queue_) - 1 : it_twist;

  if (use_imu && !angular_velocity_queue_.empty()) {
    it_imu = std::lower_bound(
      std::begin(angular_velocity_queue_), std::end(angular_velocity_queue_),
      first_point_time_stamp_sec, [](const geometry_msgs::msg::Vector3Stamped & x, const double t) {
        return rclcpp::Time(x.header.stamp).seconds() < t;
      });
    it_imu =
      it_imu == std::end(angular_velocity_queue_) ? std::end(angular_velocity_queue_) - 1 : it_imu;
  }
}

bool DistortionCorrectorBase::is_pointcloud_valid(sensor_msgs::msg::PointCloud2 & pointcloud)
{
  if (pointcloud.data.empty()) {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), 10000 /* ms */, "Input pointcloud is empty.");
    return false;
  }

  auto time_stamp_field_it = std::find_if(
    std::cbegin(pointcloud.fields), std::cend(pointcloud.fields),
    [](const sensor_msgs::msg::PointField & field) { return field.name == "time_stamp"; });
  if (time_stamp_field_it == pointcloud.fields.cend()) {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
      "Required field time stamp doesn't exist in the point cloud.");
    return false;
  }

  if (!utils::is_data_layout_compatible_with_point_xyzircaedt(pointcloud)) {
    RCLCPP_ERROR(
      node_.get_logger(), "The pointcloud layout is not compatible with PointXYZIRCAEDT. Aborting");

    if (utils::is_data_layout_compatible_with_point_xyziradrt(pointcloud)) {
      RCLCPP_ERROR(
        node_.get_logger(),
        "The pointcloud layout is compatible with PointXYZIRADRT. You may be using legacy "
        "code/data");
    }

    return false;
  }

  return true;
}

std::optional<AngleConversion> DistortionCorrectorBase::try_compute_angle_conversion(
  sensor_msgs::msg::PointCloud2 & pointcloud)
{
  // This function tries to compute the angle conversion from Cartesian coordinates to LiDAR azimuth
  // coordinates system

  if (!is_pointcloud_valid(pointcloud)) return std::nullopt;

  AngleConversion angle_conversion;

  sensor_msgs::PointCloud2Iterator<float> it_x(pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> it_y(pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> it_azimuth(pointcloud, "azimuth");

  auto next_it_x = it_x;
  auto next_it_y = it_y;
  auto next_it_azimuth = it_azimuth;

  if (it_x != it_x.end() && it_x + 1 != it_x.end()) {
    next_it_x = it_x + 1;
    next_it_y = it_y + 1;
    next_it_azimuth = it_azimuth + 1;
  } else {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
      "Current point cloud only has a single point. Could not calculate the angle conversion.");
    return std::nullopt;
  }

  for (; next_it_x != it_x.end();
       ++it_x, ++it_y, ++it_azimuth, ++next_it_x, ++next_it_y, ++next_it_azimuth) {
    auto current_cartesian_rad = autoware::universe_utils::opencv_fast_atan2(*it_y, *it_x);
    auto next_cartesian_rad = autoware::universe_utils::opencv_fast_atan2(*next_it_y, *next_it_x);

    // If the angle exceeds 180 degrees, it may cross the 0-degree axis,
    // which could disrupt the calculation of the formula.
    if (
      std::abs(*next_it_azimuth - *it_azimuth) == 0 ||
      std::abs(next_cartesian_rad - current_cartesian_rad) == 0) {
      RCLCPP_DEBUG_STREAM_THROTTLE(
        node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
        "Angle between two points is 0 degrees. Iterate to next point ...");
      continue;
    }

    // restrict the angle difference between [-180, 180] (degrees)
    float azimuth_diff =
      std::abs(*next_it_azimuth - *it_azimuth) > autoware::universe_utils::pi
        ? std::abs(*next_it_azimuth - *it_azimuth) - 2 * autoware::universe_utils::pi
        : *next_it_azimuth - *it_azimuth;
    float cartesian_rad_diff =
      std::abs(next_cartesian_rad - current_cartesian_rad) > autoware::universe_utils::pi
        ? std::abs(next_cartesian_rad - current_cartesian_rad) - 2 * autoware::universe_utils::pi
        : next_cartesian_rad - current_cartesian_rad;

    float sign = azimuth_diff / cartesian_rad_diff;

    // Check if 'sign' can be adjusted to 1 or -1
    if (std::abs(sign - 1.0f) <= angle_conversion.sign_threshold) {
      angle_conversion.sign = 1.0f;
    } else if (std::abs(sign + 1.0f) <= angle_conversion.sign_threshold) {
      angle_conversion.sign = -1.0f;
    } else {
      RCLCPP_DEBUG_STREAM_THROTTLE(
        node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
        "Value of sign is not close to 1 or -1. Iterate to next point ...");
      continue;
    }

    float offset_rad = *it_azimuth - sign * current_cartesian_rad;
    // Check if 'offset_rad' can be adjusted to offset_rad multiple of π/2
    int multiple_of_90_degrees = std::round(offset_rad / (autoware::universe_utils::pi / 2));
    if (
      std::abs(offset_rad - multiple_of_90_degrees * (autoware::universe_utils::pi / 2)) >
      angle_conversion.offset_rad_threshold) {
      RCLCPP_DEBUG_STREAM_THROTTLE(
        node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
        "Value of offset_rad is not close to multiplication of 90 degrees. Iterate to next point "
        "...");
      continue;
    }

    // Limit the range of offset_rad in [0, 360)
    multiple_of_90_degrees = (multiple_of_90_degrees % 4 + 4) % 4;

    angle_conversion.offset_rad = multiple_of_90_degrees * (autoware::universe_utils::pi / 2);

    return angle_conversion;
  }
  return std::nullopt;
}

void DistortionCorrectorBase::warn_if_timestamp_is_too_late(
  bool is_twist_time_stamp_too_late, bool is_imu_time_stamp_too_late)
{
  if (is_twist_time_stamp_too_late) {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
      "Twist time_stamp is too late. Could not interpolate.");
  }

  if (is_imu_time_stamp_too_late) {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), 10000 /* ms */,
      "IMU time_stamp is too late. Could not interpolate.");
  }
}

tf2::Transform DistortionCorrectorBase::convert_matrix_to_transform(const Eigen::Matrix4f & matrix)
{
  tf2::Transform transform;
  transform.setOrigin(tf2::Vector3(matrix(0, 3), matrix(1, 3), matrix(2, 3)));
  transform.setBasis(tf2::Matrix3x3(
    matrix(0, 0), matrix(0, 1), matrix(0, 2), matrix(1, 0), matrix(1, 1), matrix(1, 2),
    matrix(2, 0), matrix(2, 1), matrix(2, 2)));
  return transform;
}

template <class T>
void DistortionCorrector<T>::undistort_pointcloud(
  bool use_imu, std::optional<AngleConversion> angle_conversion_opt,
  sensor_msgs::msg::PointCloud2 & pointcloud)
{
  if (!is_pointcloud_valid(pointcloud)) return;
  if (twist_queue_.empty()) {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), 10000 /* ms */, "Twist queue is empty.");
    return;
  }

  sensor_msgs::PointCloud2Iterator<float> it_x(pointcloud, "x");
  sensor_msgs::PointCloud2Iterator<float> it_y(pointcloud, "y");
  sensor_msgs::PointCloud2Iterator<float> it_z(pointcloud, "z");
  sensor_msgs::PointCloud2Iterator<float> it_azimuth(pointcloud, "azimuth");
  sensor_msgs::PointCloud2Iterator<float> it_distance(pointcloud, "distance");
  sensor_msgs::PointCloud2ConstIterator<std::uint32_t> it_time_stamp(pointcloud, "time_stamp");

  double prev_time_stamp_sec{
    pointcloud.header.stamp.sec + 1e-9 * (pointcloud.header.stamp.nanosec + *it_time_stamp)};
  const double first_point_time_stamp_sec{
    pointcloud.header.stamp.sec + 1e-9 * (pointcloud.header.stamp.nanosec + *it_time_stamp)};

  std::deque<geometry_msgs::msg::TwistStamped>::iterator it_twist;
  std::deque<geometry_msgs::msg::Vector3Stamped>::iterator it_imu;
  get_twist_and_imu_iterator(use_imu, first_point_time_stamp_sec, it_twist, it_imu);

  // For performance, do not instantiate `rclcpp::Time` inside of the for-loop
  double twist_stamp = rclcpp::Time(it_twist->header.stamp).seconds();
  double imu_stamp{0.0};
  if (use_imu && !angular_velocity_queue_.empty()) {
    imu_stamp = rclcpp::Time(it_imu->header.stamp).seconds();
  }

  // If there is a point in a pointcloud that cannot be associated, record it to issue a warning
  bool is_twist_time_stamp_too_late = false;
  bool is_imu_time_stamp_too_late = false;

  for (; it_x != it_x.end(); ++it_x, ++it_y, ++it_z, ++it_time_stamp) {
    bool is_twist_valid = true;
    bool is_imu_valid = true;

    const double global_point_stamp =
      pointcloud.header.stamp.sec + 1e-9 * (pointcloud.header.stamp.nanosec + *it_time_stamp);

    // Get closest twist information
    while (it_twist != std::end(twist_queue_) - 1 && global_point_stamp > twist_stamp) {
      ++it_twist;
      twist_stamp = rclcpp::Time(it_twist->header.stamp).seconds();
    }
    if (std::abs(global_point_stamp - twist_stamp) > 0.1) {
      is_twist_time_stamp_too_late = true;
      is_twist_valid = false;
    }

    // Get closest IMU information
    if (use_imu && !angular_velocity_queue_.empty()) {
      while (it_imu != std::end(angular_velocity_queue_) - 1 && global_point_stamp > imu_stamp) {
        ++it_imu;
        imu_stamp = rclcpp::Time(it_imu->header.stamp).seconds();
      }

      if (std::abs(global_point_stamp - imu_stamp) > 0.1) {
        is_imu_time_stamp_too_late = true;
        is_imu_valid = false;
      }
    } else {
      is_imu_valid = false;
    }

    auto time_offset = static_cast<float>(global_point_stamp - prev_time_stamp_sec);

    // Undistort a single point based on the strategy
    undistort_point(it_x, it_y, it_z, it_twist, it_imu, time_offset, is_twist_valid, is_imu_valid);

    if (angle_conversion_opt.has_value()) {
      if (!pointcloud_transform_needed_) {
        throw std::runtime_error(
          "The pointcloud is not in the sensor's frame and thus azimuth and distance cannot be "
          "updated. "
          "Please change the input pointcloud or set update_azimuth_and_distance to false.");
      }
      float cartesian_coordinate_azimuth =
        autoware::universe_utils::opencv_fast_atan2(*it_y, *it_x);
      float updated_azimuth = angle_conversion_opt->offset_rad +
                              angle_conversion_opt->sign * cartesian_coordinate_azimuth;
      if (updated_azimuth < 0) {
        updated_azimuth += autoware::universe_utils::pi * 2;
      } else if (updated_azimuth > 2 * autoware::universe_utils::pi) {
        updated_azimuth -= autoware::universe_utils::pi * 2;
      }

      *it_azimuth = updated_azimuth;
      *it_distance = sqrt(*it_x * *it_x + *it_y * *it_y + *it_z * *it_z);

      ++it_azimuth;
      ++it_distance;
    }

    prev_time_stamp_sec = global_point_stamp;
  }

  warn_if_timestamp_is_too_late(is_twist_time_stamp_too_late, is_imu_time_stamp_too_late);
}

///////////////////////// Functions for different undistortion strategies /////////////////////////

void DistortionCorrector2D::initialize()
{
  x_ = 0.0f;
  y_ = 0.0f;
  theta_ = 0.0f;
}

void DistortionCorrector3D::initialize()
{
  prev_transformation_matrix_ = Eigen::Matrix4f::Identity();
}

void DistortionCorrector2D::set_pointcloud_transform(
  const std::string & base_frame, const std::string & lidar_frame)
{
  if (pointcloud_transform_exists_) {
    return;
  }

  Eigen::Matrix4f eigen_lidar_to_base_link;
  pointcloud_transform_exists_ =
    managed_tf_buffer_->getTransform(base_frame, lidar_frame, eigen_lidar_to_base_link);
  tf2_lidar_to_base_link_ = convert_matrix_to_transform(eigen_lidar_to_base_link);
  tf2_base_link_to_lidar_ = tf2_lidar_to_base_link_.inverse();
  pointcloud_transform_needed_ = base_frame != lidar_frame && pointcloud_transform_exists_;
}

void DistortionCorrector3D::set_pointcloud_transform(
  const std::string & base_frame, const std::string & lidar_frame)
{
  if (pointcloud_transform_exists_) {
    return;
  }

  pointcloud_transform_exists_ =
    managed_tf_buffer_->getTransform(base_frame, lidar_frame, eigen_lidar_to_base_link_);
  eigen_base_link_to_lidar_ = eigen_lidar_to_base_link_.inverse();
  pointcloud_transform_needed_ = base_frame != lidar_frame && pointcloud_transform_exists_;
}

inline void DistortionCorrector2D::undistort_point_implementation(
  sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
  sensor_msgs::PointCloud2Iterator<float> & it_z,
  std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
  std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, const float & time_offset,
  const bool & is_twist_valid, const bool & is_imu_valid)
{
  // Initialize linear velocity and angular velocity
  float v{0.0f};
  float w{0.0f};
  if (is_twist_valid) {
    v = static_cast<float>(it_twist->twist.linear.x);
    w = static_cast<float>(it_twist->twist.angular.z);
  }
  if (is_imu_valid) {
    w = static_cast<float>(it_imu->vector.z);
  }

  // Undistort point
  point_tf_.setValue(*it_x, *it_y, *it_z);

  if (pointcloud_transform_needed_) {
    point_tf_ = tf2_lidar_to_base_link_ * point_tf_;
  }
  theta_ += w * time_offset;
  auto [sin_half_theta, cos_half_theta] = autoware::universe_utils::sin_and_cos(theta_ * 0.5f);
  auto [sin_theta, cos_theta] = autoware::universe_utils::sin_and_cos(theta_);

  baselink_quat_.setValue(
    0, 0, sin_half_theta, cos_half_theta);  // baselink_quat.setRPY(0.0, 0.0, theta); (Note that the
                                            // value is slightly different)
  const float dis = v * time_offset;
  x_ += dis * cos_theta;
  y_ += dis * sin_theta;

  baselink_tf_odom_.setOrigin(tf2::Vector3(x_, y_, 0.0));
  baselink_tf_odom_.setRotation(baselink_quat_);

  undistorted_point_tf_ = baselink_tf_odom_ * point_tf_;

  if (pointcloud_transform_needed_) {
    undistorted_point_tf_ = tf2_base_link_to_lidar_ * undistorted_point_tf_;
  }

  *it_x = static_cast<float>(undistorted_point_tf_.getX());
  *it_y = static_cast<float>(undistorted_point_tf_.getY());
  *it_z = static_cast<float>(undistorted_point_tf_.getZ());
}

inline void DistortionCorrector3D::undistort_point_implementation(
  sensor_msgs::PointCloud2Iterator<float> & it_x, sensor_msgs::PointCloud2Iterator<float> & it_y,
  sensor_msgs::PointCloud2Iterator<float> & it_z,
  std::deque<geometry_msgs::msg::TwistStamped>::iterator & it_twist,
  std::deque<geometry_msgs::msg::Vector3Stamped>::iterator & it_imu, const float & time_offset,
  const bool & is_twist_valid, const bool & is_imu_valid)
{
  // Initialize linear velocity and angular velocity
  float v_x{0.0f};
  float v_y{0.0f};
  float v_z{0.0f};
  float w_x{0.0f};
  float w_y{0.0f};
  float w_z{0.0f};
  if (is_twist_valid) {
    v_x = static_cast<float>(it_twist->twist.linear.x);
    v_y = static_cast<float>(it_twist->twist.linear.y);
    v_z = static_cast<float>(it_twist->twist.linear.z);
    w_x = static_cast<float>(it_twist->twist.angular.x);
    w_y = static_cast<float>(it_twist->twist.angular.y);
    w_z = static_cast<float>(it_twist->twist.angular.z);
  }
  if (is_imu_valid) {
    w_x = static_cast<float>(it_imu->vector.x);
    w_y = static_cast<float>(it_imu->vector.y);
    w_z = static_cast<float>(it_imu->vector.z);
  }

  // Undistort point
  point_eigen_ << *it_x, *it_y, *it_z, 1.0;
  if (pointcloud_transform_needed_) {
    point_eigen_ = eigen_lidar_to_base_link_ * point_eigen_;
  }

  Sophus::SE3f::Tangent twist(v_x, v_y, v_z, w_x, w_y, w_z);
  twist = twist * time_offset;
  transformation_matrix_ = Sophus::SE3f::exp(twist).matrix();
  transformation_matrix_ = transformation_matrix_ * prev_transformation_matrix_;
  undistorted_point_eigen_ = transformation_matrix_ * point_eigen_;

  if (pointcloud_transform_needed_) {
    undistorted_point_eigen_ = eigen_base_link_to_lidar_ * undistorted_point_eigen_;
  }
  *it_x = undistorted_point_eigen_[0];
  *it_y = undistorted_point_eigen_[1];
  *it_z = undistorted_point_eigen_[2];

  prev_transformation_matrix_ = transformation_matrix_;
}

template class DistortionCorrector<DistortionCorrector2D>;
template class DistortionCorrector<DistortionCorrector3D>;

}  // namespace autoware::pointcloud_preprocessor

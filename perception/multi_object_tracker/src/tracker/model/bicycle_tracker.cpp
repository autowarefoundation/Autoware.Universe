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
//
//
// Author: v1.0 Yukihiro Saito
//

#include "multi_object_tracker/tracker/model/bicycle_tracker.hpp"

#include "multi_object_tracker/utils/utils.hpp"

#include <tier4_autoware_utils/geometry/boost_polygon_utils.hpp>
#include <tier4_autoware_utils/math/normalization.hpp>
#include <tier4_autoware_utils/math/unit_conversion.hpp>

#include <bits/stdc++.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif
#include "object_recognition_utils/object_recognition_utils.hpp"

#define EIGEN_MPL2_ONLY
#include <Eigen/Core>
#include <Eigen/Geometry>

using Label = autoware_auto_perception_msgs::msg::ObjectClassification;

BicycleTracker::BicycleTracker(
  const rclcpp::Time & time, const autoware_auto_perception_msgs::msg::DetectedObject & object,
  const geometry_msgs::msg::Transform & /*self_transform*/)
: Tracker(time, object.classification),
  logger_(rclcpp::get_logger("BicycleTracker")),
  last_update_time_(time),
  z_(object.kinematics.pose_with_covariance.pose.position.z)
{
  object_ = object;

  // initialize params
  float r_stddev_x = 0.6;                                     // [m]
  float r_stddev_y = 0.4;                                     // [m]
  float r_stddev_yaw = tier4_autoware_utils::deg2rad(30);     // [rad]
  float p0_stddev_x = 0.8;                                    // [m/s]
  float p0_stddev_y = 0.5;                                    // [m/s]
  float p0_stddev_yaw = tier4_autoware_utils::deg2rad(1000);  // [rad/s]
  float p0_stddev_vx = tier4_autoware_utils::kmph2mps(1000);  // [m/(s*s)]
  float p0_stddev_slip = tier4_autoware_utils::deg2rad(10);   // [rad/(s*s)]
  ekf_params_.q_stddev_acc_long = 9.8 * 0.5;  // [m/(s*s)] uncertain longitudinal acceleration
  ekf_params_.q_stddev_acc_lat = 9.8 * 0.3;   // [m/(s*s)] uncertain lateral acceleration
  ekf_params_.q_stddev_slip_rate =
    tier4_autoware_utils::deg2rad(5);  // [rad/s] uncertain head angle change rate
  ekf_params_.q_max_slip_angle = tier4_autoware_utils::deg2rad(30);  // [rad] max slip angle
  ekf_params_.r_cov_x = std::pow(r_stddev_x, 2.0);
  ekf_params_.r_cov_y = std::pow(r_stddev_y, 2.0);
  ekf_params_.r_cov_yaw = std::pow(r_stddev_yaw, 2.0);
  ekf_params_.p0_cov_x = std::pow(p0_stddev_x, 2.0);
  ekf_params_.p0_cov_y = std::pow(p0_stddev_y, 2.0);
  ekf_params_.p0_cov_yaw = std::pow(p0_stddev_yaw, 2.0);
  ekf_params_.p0_cov_vx = std::pow(p0_stddev_vx, 2.0);
  ekf_params_.p0_cov_slip = std::pow(p0_stddev_slip, 2.0);
  max_vel_ = tier4_autoware_utils::kmph2mps(100);  // [m/s]
  max_slip_ = tier4_autoware_utils::deg2rad(30);  // [rad/s]

  // initialize X matrix
  Eigen::MatrixXd X(ekf_params_.dim_x, 1);
  X(IDX::X) = object.kinematics.pose_with_covariance.pose.position.x;
  X(IDX::Y) = object.kinematics.pose_with_covariance.pose.position.y;
  X(IDX::YAW) = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
  if (object.kinematics.has_twist) {
    X(IDX::VEL) = object.kinematics.twist_with_covariance.twist.linear.x;
  } else {
    X(IDX::VEL) = 0.0;
  }
  X(IDX::SLIP) = 0.0;

  // initialize P matrix
  Eigen::MatrixXd P = Eigen::MatrixXd::Zero(ekf_params_.dim_x, ekf_params_.dim_x);

  if (!object.kinematics.has_position_covariance) {
    const double cos_yaw = std::cos(X(IDX::YAW));
    const double sin_yaw = std::sin(X(IDX::YAW));
    const double sin_2yaw = std::sin(2.0f * X(IDX::YAW));
    // Rotate the covariance matrix according to the vehicle yaw
    // because p0_cov_x and y are in the vehicle coordinate system.
    P(IDX::X, IDX::X) =
      ekf_params_.p0_cov_x * cos_yaw * cos_yaw + ekf_params_.p0_cov_y * sin_yaw * sin_yaw;
    P(IDX::X, IDX::Y) = 0.5f * (ekf_params_.p0_cov_x - ekf_params_.p0_cov_y) * sin_2yaw;
    P(IDX::Y, IDX::Y) =
      ekf_params_.p0_cov_x * sin_yaw * sin_yaw + ekf_params_.p0_cov_y * cos_yaw * cos_yaw;
    P(IDX::Y, IDX::X) = P(IDX::X, IDX::Y);
    P(IDX::YAW, IDX::YAW) = ekf_params_.p0_cov_yaw;
    P(IDX::VEL, IDX::VEL) = ekf_params_.p0_cov_vx;
    P(IDX::SLIP, IDX::SLIP) = ekf_params_.p0_cov_slip;
  } else {
    P(IDX::X, IDX::X) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::X_X];
    P(IDX::X, IDX::Y) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::X_Y];
    P(IDX::Y, IDX::Y) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::Y_Y];
    P(IDX::Y, IDX::X) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::Y_X];
    P(IDX::YAW, IDX::YAW) =
      object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::YAW_YAW];
    if (object.kinematics.has_twist_covariance) {
      P(IDX::VEL, IDX::VEL) =
        object.kinematics.twist_with_covariance.covariance[utils::MSG_COV_IDX::X_X];
    } else {
      P(IDX::VEL, IDX::VEL) = ekf_params_.p0_cov_vx;
    }
    P(IDX::SLIP, IDX::SLIP) = ekf_params_.p0_cov_slip;
  }

  if (object.shape.type == autoware_auto_perception_msgs::msg::Shape::BOUNDING_BOX) {
    bounding_box_ = {
      object.shape.dimensions.x, object.shape.dimensions.y, object.shape.dimensions.z};
  } else {
    bounding_box_ = {1.0, 0.5, 1.7};
  }
  ekf_.init(X, P);

  // Set lf, lr
  lf_ = bounding_box_.length * 0.3;  // 30% front from the center
  lr_ = bounding_box_.length * 0.3;  // 30% rear from the center
}

bool BicycleTracker::predict(const rclcpp::Time & time)
{
  const double dt = (time - last_update_time_).seconds();
  if (dt < 0.0) {
    RCLCPP_WARN(logger_, "dt is negative. (%f)", dt);
    return false;
  }
  // if dt is too large, shorten dt and repeat prediction
  const double dt_max = 0.11;
  const uint32_t repeat = std::ceil(dt / dt_max);
  const double dt_ = dt / repeat;
  bool ret = false;
  for (uint32_t i = 0; i < repeat; ++i) {
    ret = predict(dt_, ekf_);
    if (!ret) {
      return false;
    }
  }
  if (ret) {
    last_update_time_ = time;
  }
  return ret;
}

bool BicycleTracker::predict(const double dt, KalmanFilter & ekf) const
{
  /*  static bicycle model (constant slip angle, constant velocity)
   *
   * w_k = vx_k * sin(slip_k) / l_r
   * x_{k+1}   = x_k + vx_k*cos(yaw_k+slip_k)*dt - 0.5*w_k*vx_k*sin(yaw_k+slip_k)*dt*dt
   * y_{k+1}   = y_k + vx_k*sin(yaw_k+slip_k)*dt + 0.5*w_k*vx_k*cos(yaw_k+slip_k)*dt*dt
   * yaw_{k+1} = yaw_k + w_k * dt
   * vx_{k+1}  = vx_k
   * slip_{k+1}  = slip_k
   *
   */

  /*  Jacobian Matrix
   *
   * A = [ 1, 0, -vx*sin(yaw+slip)*dt - 0.5*w_k*vx_k*cos(yaw+slip)*dt*dt,
   *   cos(yaw+slip)*dt - w*sin(yaw+slip)*dt*dt,
   *   -vx*sin(yaw+slip)*dt - 0.5*vx*vx/l_r*(cos(slip)sin(yaw+slip)+sin(slip)cos(yaw+slip))*dt*dt ]
   *     [ 0, 1,  vx*cos(yaw+slip)*dt - 0.5*w_k*vx_k*sin(yaw+slip)*dt*dt,
   *   sin(yaw+slip)*dt + w*cos(yaw+slip)*dt*dt,
   *   vx*cos(yaw+slip)*dt + 0.5*vx*vx/l_r*(cos(slip)cos(yaw+slip)-sin(slip)sin(yaw+slip))*dt*dt ]
   *     [ 0, 0,  1, 1/l_r*sin(slip)*dt, vx/l_r*cos(slip)*dt]
   *     [ 0, 0,  0,                  1,                   0]
   *     [ 0, 0,  0,                  0,                   1]
   *
   */

  // X t
  Eigen::MatrixXd X_t(ekf_params_.dim_x, 1);  // predicted state
  ekf.getX(X_t);
  const double cos_yaw = std::cos(X_t(IDX::YAW) + X_t(IDX::SLIP));
  const double sin_yaw = std::sin(X_t(IDX::YAW) + X_t(IDX::SLIP));
  const double cos_slip = std::cos(X_t(IDX::SLIP));
  const double sin_slip = std::sin(X_t(IDX::SLIP));
  const double vx = X_t(IDX::VEL);
  const double sin_2yaw = std::sin(2.0f * X_t(IDX::YAW));
  const double w = vx * sin_slip / lr_;
  const double w_dtdt = w * dt * dt;
  const double vv_dtdt__lr = vx * vx * dt * dt / lr_;

  // X t+1
  Eigen::MatrixXd X_next_t(ekf_params_.dim_x, 1);  // predicted state
  X_next_t(IDX::X) =
    X_t(IDX::X) + vx * cos_yaw * dt - 0.5 * vx * sin_slip * w_dtdt;  // dx = v * cos(yaw)
  X_next_t(IDX::Y) =
    X_t(IDX::Y) + vx * sin_yaw * dt + 0.5 * vx * cos_slip * w_dtdt;  // dy = v * sin(yaw)
  X_next_t(IDX::YAW) = X_t(IDX::YAW) + vx / lr_ * sin_slip * dt;     // dyaw = omega
  X_next_t(IDX::VEL) = X_t(IDX::VEL);
  X_next_t(IDX::SLIP) = X_t(IDX::SLIP);

  // A
  Eigen::MatrixXd A = Eigen::MatrixXd::Identity(ekf_params_.dim_x, ekf_params_.dim_x);
  A(IDX::X, IDX::YAW) = -vx * sin_yaw * dt - 0.5 * vx * cos_yaw * w_dtdt;
  A(IDX::X, IDX::VEL) = cos_yaw * dt - sin_yaw * w_dtdt;
  A(IDX::X, IDX::SLIP) =
    -vx * sin_yaw * dt - 0.5 * (cos_slip * sin_yaw + sin_slip * cos_yaw) * vv_dtdt__lr;
  A(IDX::Y, IDX::YAW) = vx * cos_yaw * dt - 0.5 * vx * sin_yaw * w_dtdt;
  A(IDX::Y, IDX::VEL) = sin_yaw * dt + cos_yaw * w_dtdt;
  A(IDX::Y, IDX::SLIP) =
    vx * cos_yaw * dt + 0.5 * (cos_slip * cos_yaw - sin_slip * sin_yaw) * vv_dtdt__lr;
  A(IDX::YAW, IDX::VEL) = 1.0 / lr_ * sin_slip * dt;
  A(IDX::YAW, IDX::SLIP) = vx / lr_ * cos_slip * dt;

  // Q
  const float q_stddev_yaw_rate = std::min(
    ekf_params_.q_stddev_acc_lat * 2.0 / vx,
    vx / lr_ * std::sin(ekf_params_.q_max_slip_angle));  // [rad/s] uncertain yaw rate limited by
                                                         // lateral acceleration or slip angle
  const float q_cov_x = std::pow(ekf_params_.q_stddev_acc_long * dt * dt, 2.0);
  const float q_cov_y = std::pow(ekf_params_.q_stddev_acc_lat * dt * dt, 2.0);
  const float q_cov_yaw = std::pow(q_stddev_yaw_rate * dt, 2.0);
  const float q_cov_vx = std::pow(ekf_params_.q_stddev_acc_long * dt, 2.0);
  const float q_cov_slip = std::pow(ekf_params_.q_stddev_slip_rate * dt, 2.0);

  Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(ekf_params_.dim_x, ekf_params_.dim_x);
  // Rotate the covariance matrix according to the vehicle yaw
  // because q_cov_x and y are in the vehicle coordinate system.
  Q(IDX::X, IDX::X) = (q_cov_x * cos_yaw * cos_yaw + q_cov_y * sin_yaw * sin_yaw);
  Q(IDX::X, IDX::Y) = (0.5f * (q_cov_x - q_cov_y) * sin_2yaw);
  Q(IDX::Y, IDX::Y) = (q_cov_x * sin_yaw * sin_yaw + q_cov_y * cos_yaw * cos_yaw);
  Q(IDX::Y, IDX::X) = Q(IDX::X, IDX::Y);
  Q(IDX::YAW, IDX::YAW) = q_cov_yaw;
  Q(IDX::VEL, IDX::VEL) = q_cov_vx;
  Q(IDX::SLIP, IDX::SLIP) = q_cov_slip;
  Eigen::MatrixXd B = Eigen::MatrixXd::Zero(ekf_params_.dim_x, ekf_params_.dim_x);
  Eigen::MatrixXd u = Eigen::MatrixXd::Zero(ekf_params_.dim_x, 1);

  if (!ekf.predict(X_next_t, A, Q)) {
    RCLCPP_WARN(logger_, "Cannot predict");
  }

  return true;
}

/**
 * @brief measurement update with ekf on receiving detected_object
 *
 * @param object : Detected object
 * @return bool : measurement is updatable
 */
bool BicycleTracker::measureWithPose(
  const autoware_auto_perception_msgs::msg::DetectedObject & object)
{
  // yaw measurement
  double measurement_yaw = tier4_autoware_utils::normalizeRadian(
    tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation));

  // prediction
  Eigen::MatrixXd X_t(ekf_params_.dim_x, 1);
  ekf_.getX(X_t);

  // validate if orientation is available
  bool use_orientation_information = false;
  if (
    object.kinematics.orientation_availability ==
    autoware_auto_perception_msgs::msg::DetectedObjectKinematics::SIGN_UNKNOWN) {  // has 180 degree
                                                                                   // uncertainty
    // fix orientation
    while (M_PI_2 <= X_t(IDX::YAW) - measurement_yaw) {
      measurement_yaw = measurement_yaw + M_PI;
    }
    while (M_PI_2 <= measurement_yaw - X_t(IDX::YAW)) {
      measurement_yaw = measurement_yaw - M_PI;
    }
    use_orientation_information = true;

  } else if (
    object.kinematics.orientation_availability ==
    autoware_auto_perception_msgs::msg::DetectedObjectKinematics::AVAILABLE) {  // know full angle

    use_orientation_information = true;
  }

  const int dim_y =
    use_orientation_information ? 3 : 2;  // pos x, pos y, (pos yaw) depending on Pose output

  /* Set measurement matrix */
  Eigen::MatrixXd Y(dim_y, 1);
  Y(IDX::X, 0) = object.kinematics.pose_with_covariance.pose.position.x;
  Y(IDX::Y, 0) = object.kinematics.pose_with_covariance.pose.position.y;

  /* Set measurement matrix */
  Eigen::MatrixXd C = Eigen::MatrixXd::Zero(dim_y, ekf_params_.dim_x);
  C(0, IDX::X) = 1.0;  // for pos x
  C(1, IDX::Y) = 1.0;  // for pos y

  /* Set measurement noise covariance */
  Eigen::MatrixXd R = Eigen::MatrixXd::Zero(dim_y, dim_y);

  if (!object.kinematics.has_position_covariance) {
    R(0, 0) = ekf_params_.r_cov_x;  // x - x
    R(0, 1) = 0.0;                  // x - y
    R(1, 1) = ekf_params_.r_cov_y;  // y - y
    R(1, 0) = R(0, 1);              // y - x
  } else {
    R(0, 0) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::X_X];
    R(0, 1) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::X_Y];
    R(1, 0) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::Y_X];
    R(1, 1) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::Y_Y];
  }

  // if there are orientation available
  if (dim_y == 3) {
    // fill yaw observation and measurement matrix
    Y(IDX::YAW, 0) = measurement_yaw;
    C(2, IDX::YAW) = 1.0;

    // fill yaw covariance
    if (!object.kinematics.has_position_covariance) {
      R(2, 2) = ekf_params_.r_cov_yaw;  // yaw - yaw
    } else {
      R(0, 2) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::X_YAW];
      R(1, 2) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::Y_YAW];
      R(2, 0) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::YAW_X];
      R(2, 1) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::YAW_Y];
      R(2, 2) = object.kinematics.pose_with_covariance.covariance[utils::MSG_COV_IDX::YAW_YAW];
    }
  }

  // update with ekf
  if (!ekf_.update(Y, C, R)) {
    RCLCPP_WARN(logger_, "Cannot update");
  }

  // normalize yaw and limit vx, wz
  {
    Eigen::MatrixXd X_t(ekf_params_.dim_x, 1);
    Eigen::MatrixXd P_t(ekf_params_.dim_x, ekf_params_.dim_x);
    ekf_.getX(X_t);
    ekf_.getP(P_t);
    X_t(IDX::YAW) = tier4_autoware_utils::normalizeRadian(X_t(IDX::YAW));
    if (!(-max_vel_ <= X_t(IDX::VEL) && X_t(IDX::VEL) <= max_vel_)) {
      X_t(IDX::VEL) = X_t(IDX::VEL) < 0 ? -max_vel_ : max_vel_;
    }
    if (!(-max_slip_ <= X_t(IDX::SLIP) && X_t(IDX::SLIP) <= max_slip_)) {
      X_t(IDX::SLIP) = X_t(IDX::SLIP) < 0 ? -max_slip_ : max_slip_;
    }
    ekf_.init(X_t, P_t);
  }

  // position z
  constexpr float gain = 0.9;
  z_ = gain * z_ + (1.0 - gain) * object.kinematics.pose_with_covariance.pose.position.z;

  return true;
}

bool BicycleTracker::measureWithShape(
  const autoware_auto_perception_msgs::msg::DetectedObject & object)
{
  if (object.shape.type != autoware_auto_perception_msgs::msg::Shape::BOUNDING_BOX) {
    return false;
  }
  constexpr float gain = 0.9;

  bounding_box_.length = gain * bounding_box_.length + (1.0 - gain) * object.shape.dimensions.x;
  bounding_box_.width = gain * bounding_box_.width + (1.0 - gain) * object.shape.dimensions.y;
  bounding_box_.height = gain * bounding_box_.height + (1.0 - gain) * object.shape.dimensions.z;

  // update lf, lr
  lf_ = bounding_box_.length * 0.3;  // 30% front from the center
  lr_ = bounding_box_.length * 0.3;  // 30% rear from the center

  return true;
}

bool BicycleTracker::measure(
  const autoware_auto_perception_msgs::msg::DetectedObject & object, const rclcpp::Time & time,
  const geometry_msgs::msg::Transform & /*self_transform*/)
{
  const auto & current_classification = getClassification();
  object_ = object;
  if (object_recognition_utils::getHighestProbLabel(object.classification) == Label::UNKNOWN) {
    setClassification(current_classification);
  }

  if (0.01 /*10msec*/ < std::fabs((time - last_update_time_).seconds())) {
    RCLCPP_WARN(
      logger_, "There is a large gap between predicted time and measurement time. (%f)",
      (time - last_update_time_).seconds());
  }

  measureWithPose(object);
  measureWithShape(object);

  return true;
}

bool BicycleTracker::getTrackedObject(
  const rclcpp::Time & time, autoware_auto_perception_msgs::msg::TrackedObject & object) const
{
  object = object_recognition_utils::toTrackedObject(object_);
  object.object_id = getUUID();
  object.classification = getClassification();

  // predict kinematics
  KalmanFilter tmp_ekf_for_no_update = ekf_;
  const double dt = (time - last_update_time_).seconds();
  if (0.001 /*1msec*/ < dt) {
    predict(dt, tmp_ekf_for_no_update);
  }
  Eigen::MatrixXd X_t(ekf_params_.dim_x, 1);                // predicted state
  Eigen::MatrixXd P(ekf_params_.dim_x, ekf_params_.dim_x);  // predicted state
  tmp_ekf_for_no_update.getX(X_t);
  tmp_ekf_for_no_update.getP(P);

  auto & pose_with_cov = object.kinematics.pose_with_covariance;
  auto & twist_with_cov = object.kinematics.twist_with_covariance;

  // position
  pose_with_cov.pose.position.x = X_t(IDX::X);
  pose_with_cov.pose.position.y = X_t(IDX::Y);
  pose_with_cov.pose.position.z = z_;
  // quaternion
  {
    double roll, pitch, yaw;
    tf2::Quaternion original_quaternion;
    tf2::fromMsg(object_.kinematics.pose_with_covariance.pose.orientation, original_quaternion);
    tf2::Matrix3x3(original_quaternion).getRPY(roll, pitch, yaw);
    tf2::Quaternion filtered_quaternion;
    filtered_quaternion.setRPY(roll, pitch, X_t(IDX::YAW));
    pose_with_cov.pose.orientation.x = filtered_quaternion.x();
    pose_with_cov.pose.orientation.y = filtered_quaternion.y();
    pose_with_cov.pose.orientation.z = filtered_quaternion.z();
    pose_with_cov.pose.orientation.w = filtered_quaternion.w();
    object.kinematics.orientation_availability =
      autoware_auto_perception_msgs::msg::TrackedObjectKinematics::SIGN_UNKNOWN;
  }
  // position covariance
  constexpr double z_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  constexpr double r_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  constexpr double p_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  pose_with_cov.covariance[utils::MSG_COV_IDX::X_X] = P(IDX::X, IDX::X);
  pose_with_cov.covariance[utils::MSG_COV_IDX::X_Y] = P(IDX::X, IDX::Y);
  pose_with_cov.covariance[utils::MSG_COV_IDX::Y_X] = P(IDX::Y, IDX::X);
  pose_with_cov.covariance[utils::MSG_COV_IDX::Y_Y] = P(IDX::Y, IDX::Y);
  pose_with_cov.covariance[utils::MSG_COV_IDX::Z_Z] = z_cov;
  pose_with_cov.covariance[utils::MSG_COV_IDX::ROLL_ROLL] = r_cov;
  pose_with_cov.covariance[utils::MSG_COV_IDX::PITCH_PITCH] = p_cov;
  pose_with_cov.covariance[utils::MSG_COV_IDX::YAW_YAW] = P(IDX::YAW, IDX::YAW);

  // twist
  twist_with_cov.twist.linear.x = X_t(IDX::VEL) * std::cos(X_t(IDX::SLIP));
  twist_with_cov.twist.linear.y = X_t(IDX::VEL) * std::sin(X_t(IDX::SLIP));
  twist_with_cov.twist.angular.z =
    X_t(IDX::VEL) / lr_ * std::sin(X_t(IDX::SLIP));  // yaw_rate = vx_k / l_r * sin(slip_k)
  // twist covariance
  constexpr double vy_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  constexpr double vz_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  constexpr double wx_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  constexpr double wy_cov = 0.1 * 0.1;  // TODO(yukkysaito) Currently tentative
  twist_with_cov.covariance[utils::MSG_COV_IDX::X_X] = P(IDX::VEL, IDX::VEL);
  twist_with_cov.covariance[utils::MSG_COV_IDX::Y_Y] = vy_cov;
  twist_with_cov.covariance[utils::MSG_COV_IDX::Z_Z] = vz_cov;
  twist_with_cov.covariance[utils::MSG_COV_IDX::X_YAW] = P(IDX::VEL, IDX::SLIP);
  twist_with_cov.covariance[utils::MSG_COV_IDX::YAW_X] = P(IDX::SLIP, IDX::VEL);
  twist_with_cov.covariance[utils::MSG_COV_IDX::ROLL_ROLL] = wx_cov;
  twist_with_cov.covariance[utils::MSG_COV_IDX::PITCH_PITCH] = wy_cov;
  twist_with_cov.covariance[utils::MSG_COV_IDX::YAW_YAW] = P(IDX::SLIP, IDX::SLIP);

  // set shape
  object.shape.dimensions.x = bounding_box_.length;
  object.shape.dimensions.y = bounding_box_.width;
  object.shape.dimensions.z = bounding_box_.height;
  const auto origin_yaw = tf2::getYaw(object_.kinematics.pose_with_covariance.pose.orientation);
  const auto ekf_pose_yaw = tf2::getYaw(pose_with_cov.pose.orientation);
  object.shape.footprint =
    tier4_autoware_utils::rotatePolygon(object.shape.footprint, origin_yaw - ekf_pose_yaw);

  return true;
}

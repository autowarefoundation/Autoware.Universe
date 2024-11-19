// Copyright 2024 Tier IV, Inc.
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
// Author: v1.0 Taekjin Lee

#include "autoware/multi_object_tracker/uncertainty/uncertainty_processor.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace autoware::multi_object_tracker
{
namespace uncertainty
{
using autoware::universe_utils::xyzrpy_covariance_index::XYZRPY_COV_IDX;

object_model::StateCovariance covarianceFromObjectClass(const ObjectClassification & object_class)
{
  const auto & label = object_class.label;
  ObjectModel obj_class_model(object_model::ObjectModelType::Unknown);
  switch (label) {
    case ObjectClassification::CAR:
      obj_class_model = object_model::normal_vehicle;
      break;
    case ObjectClassification::BUS:
    case ObjectClassification::TRUCK:
    case ObjectClassification::TRAILER:
      obj_class_model = object_model::big_vehicle;
      break;
    case ObjectClassification::BICYCLE:
    case ObjectClassification::MOTORCYCLE:
      obj_class_model = object_model::bicycle;
      break;
    case ObjectClassification::PEDESTRIAN:
      obj_class_model = object_model::pedestrian;
      break;
    default:
      obj_class_model = object_model::normal_vehicle;
      break;
  }
  return obj_class_model.measurement_covariance;
}

DetectedObject modelUncertaintyByClass(
  const DetectedObject & object, const ObjectClassification & object_class)
{
  DetectedObject updating_object = object;

  // measurement noise covariance
  const object_model::StateCovariance measurement_covariance =
    covarianceFromObjectClass(object_class);

  const auto & r_cov_x = measurement_covariance.pos_x;
  const auto & r_cov_y = measurement_covariance.pos_y;

  // yaw angle
  const double pose_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);

  // fill position covariance matrix
  auto & pose_cov = updating_object.kinematics.pose_with_covariance.covariance;
  const double cos_yaw = std::cos(pose_yaw);
  const double sin_yaw = std::sin(pose_yaw);
  const double sin_2yaw = std::sin(2.0 * pose_yaw);
  pose_cov[XYZRPY_COV_IDX::X_X] =
    r_cov_x * cos_yaw * cos_yaw + r_cov_y * sin_yaw * sin_yaw;           // x - x
  pose_cov[XYZRPY_COV_IDX::X_Y] = 0.5 * (r_cov_x - r_cov_y) * sin_2yaw;  // x - y
  pose_cov[XYZRPY_COV_IDX::Y_Y] =
    r_cov_x * sin_yaw * sin_yaw + r_cov_y * cos_yaw * cos_yaw;     // y - y
  pose_cov[XYZRPY_COV_IDX::Y_X] = pose_cov[XYZRPY_COV_IDX::X_Y];   // y - x
  pose_cov[XYZRPY_COV_IDX::X_YAW] = 0.0;                           // x - yaw
  pose_cov[XYZRPY_COV_IDX::Y_YAW] = 0.0;                           // y - yaw
  pose_cov[XYZRPY_COV_IDX::YAW_X] = 0.0;                           // yaw - x
  pose_cov[XYZRPY_COV_IDX::YAW_Y] = 0.0;                           // yaw - y
  pose_cov[XYZRPY_COV_IDX::YAW_YAW] = measurement_covariance.yaw;  // yaw - yaw
  const bool is_yaw_available =
    object.kinematics.orientation_availability !=
    autoware_perception_msgs::msg::DetectedObjectKinematics::UNAVAILABLE;
  if (!is_yaw_available) {
    pose_cov[XYZRPY_COV_IDX::YAW_YAW] *= 1e3;  // yaw is not available, multiply large value
  }

  // fill twist covariance matrix
  auto & twist_cov = updating_object.kinematics.twist_with_covariance.covariance;
  twist_cov[XYZRPY_COV_IDX::X_X] = measurement_covariance.vel_long;
  twist_cov[XYZRPY_COV_IDX::X_Y] = 0.0;
  twist_cov[XYZRPY_COV_IDX::Y_X] = 0.0;
  twist_cov[XYZRPY_COV_IDX::Y_Y] = measurement_covariance.vel_lat;

  return updating_object;
}

DetectedObjects modelUncertainty(const DetectedObjects & detected_objects)
{
  DetectedObjects updating_objects;
  updating_objects.header = detected_objects.header;
  for (const auto & object : detected_objects.objects) {
    if (object.kinematics.has_position_covariance) {
      updating_objects.objects.push_back(object);
      continue;
    }
    const ObjectClassification & object_class =
      autoware::object_recognition_utils::getHighestProbClassification(object.classification);
    updating_objects.objects.push_back(modelUncertaintyByClass(object, object_class));
  }
  return updating_objects;
}

void normalizeUncertainty(DetectedObjects & detected_objects)
{
  constexpr double min_cov_dist = 1e-4;
  constexpr double min_cov_rad = 1e-6;
  constexpr double min_cov_vel = 1e-4;

  for (auto & object : detected_objects.objects) {
    // normalize position covariance
    auto & pose_cov = object.kinematics.pose_with_covariance.covariance;
    pose_cov[XYZRPY_COV_IDX::X_X] = std::max(pose_cov[XYZRPY_COV_IDX::X_X], min_cov_dist);
    pose_cov[XYZRPY_COV_IDX::Y_Y] = std::max(pose_cov[XYZRPY_COV_IDX::Y_Y], min_cov_dist);
    pose_cov[XYZRPY_COV_IDX::Z_Z] = std::max(pose_cov[XYZRPY_COV_IDX::Z_Z], min_cov_dist);
    pose_cov[XYZRPY_COV_IDX::YAW_YAW] = std::max(pose_cov[XYZRPY_COV_IDX::YAW_YAW], min_cov_rad);

    // normalize twist covariance
    auto & twist_cov = object.kinematics.twist_with_covariance.covariance;
    twist_cov[XYZRPY_COV_IDX::X_X] = std::max(twist_cov[XYZRPY_COV_IDX::X_X], min_cov_vel);
    twist_cov[XYZRPY_COV_IDX::Y_Y] = std::max(twist_cov[XYZRPY_COV_IDX::Y_Y], min_cov_vel);
  }
}

void addOdometryUncertainty(const Odometry & odometry, DetectedObjects & detected_objects)
{
  const auto & odom_pose = odometry.pose.pose;
  const auto & odom_pose_cov = odometry.pose.covariance;
  const auto & odom_twist = odometry.twist.twist;
  const auto & odom_twist_cov = odometry.twist.covariance;

  // ego motion uncertainty, velocity multiplied by time uncertainty
  const double ego_yaw = tf2::getYaw(odom_pose.orientation);
  const double dt =
    (rclcpp::Time(odometry.header.stamp) - rclcpp::Time(detected_objects.header.stamp)).seconds();
  const double dt2 = dt * dt;
  Eigen::MatrixXd m_rot_ego = Eigen::Rotation2D(ego_yaw).toRotationMatrix();
  Eigen::MatrixXd m_cov_motion = Eigen::MatrixXd(2, 2);
  m_cov_motion << odom_twist.linear.x * odom_twist.linear.x * dt2, 0, 0,
    odom_twist.linear.y * odom_twist.linear.y * dt2;

  // ego position uncertainty, position covariance + motion covariance
  Eigen::MatrixXd m_cov_ego_pose = Eigen::MatrixXd(2, 2);
  m_cov_ego_pose << odom_pose_cov[0], odom_pose_cov[1], odom_pose_cov[6], odom_pose_cov[7];
  m_cov_ego_pose = m_cov_ego_pose + m_rot_ego * m_cov_motion * m_rot_ego.transpose();

  // ego yaw uncertainty, position covariance + yaw motion covariance
  const double & cov_ego_yaw = odom_pose_cov[35];
  const double cov_yaw = cov_ego_yaw + odom_twist.angular.z * odom_twist.angular.z * dt2;

  // ego velocity uncertainty, velocity covariance
  Eigen::MatrixXd m_cov_ego_twist = Eigen::MatrixXd(2, 2);
  m_cov_ego_twist << odom_twist_cov[0], odom_twist_cov[1], odom_twist_cov[6], odom_twist_cov[7];
  const double & cov_yaw_rate = odom_twist_cov[35];

  for (auto & object : detected_objects.objects) {
    auto & object_pose = object.kinematics.pose_with_covariance.pose;
    auto & object_pose_cov = object.kinematics.pose_with_covariance.covariance;
    const double dx = object_pose.position.x - odom_pose.position.x;
    const double dy = object_pose.position.y - odom_pose.position.y;
    const double r2 = dx * dx + dy * dy;
    const double theta = std::atan2(dy, dx);

    // 1. add odometry position and motion uncertainty to the object position covariance
    Eigen::MatrixXd m_pose_cov = Eigen::MatrixXd(2, 2);
    m_pose_cov << object_pose_cov[XYZRPY_COV_IDX::X_X], object_pose_cov[XYZRPY_COV_IDX::X_Y],
      object_pose_cov[XYZRPY_COV_IDX::Y_X], object_pose_cov[XYZRPY_COV_IDX::Y_Y];

    // 1-a. add odometry position uncertainty to the object position covariance
    // object position and it covariance is based on the world frame (map)
    m_pose_cov = m_pose_cov + m_cov_ego_pose;

    // 1-b. add odometry heading uncertainty to the object position covariance
    // uncertainty is proportional to the distance and the uncertainty orientation is vertical to
    // the vector to the object
    {
      const double cov_by_yaw = cov_ego_yaw * r2;
      // rotate the covariance matrix, add the yaw uncertainty, and rotate back
      Eigen::MatrixXd m_rot_theta = Eigen::Rotation2D(theta).toRotationMatrix();
      Eigen::MatrixXd m_cov_rot = m_rot_theta.transpose() * m_pose_cov * m_rot_theta;
      m_cov_rot(1, 1) += cov_by_yaw;  // yaw uncertainty is added to y-y element
      m_pose_cov = m_rot_theta * m_cov_rot * m_rot_theta.transpose();
    }
    // 1-c. add odometry yaw uncertainty to the object yaw covariance
    object_pose_cov[XYZRPY_COV_IDX::YAW_YAW] += cov_yaw;  // yaw-yaw

    // update the covariance matrix
    object_pose_cov[XYZRPY_COV_IDX::X_X] = m_pose_cov(0, 0);
    object_pose_cov[XYZRPY_COV_IDX::X_Y] = m_pose_cov(0, 1);
    object_pose_cov[XYZRPY_COV_IDX::Y_X] = m_pose_cov(1, 0);
    object_pose_cov[XYZRPY_COV_IDX::Y_Y] = m_pose_cov(1, 1);

    // 2. add odometry velocity uncertainty to the object velocity covariance
    auto & object_twist_cov = object.kinematics.twist_with_covariance.covariance;
    Eigen::MatrixXd m_twist_cov = Eigen::MatrixXd(2, 2);
    m_twist_cov << object_twist_cov[XYZRPY_COV_IDX::X_X], object_twist_cov[XYZRPY_COV_IDX::X_Y],
      object_twist_cov[XYZRPY_COV_IDX::Y_X], object_twist_cov[XYZRPY_COV_IDX::Y_Y];

    // 2-a. add odometry velocity uncertainty to the object linear twist covariance
    {
      const double obj_yaw = tf2::getYaw(object_pose.orientation);  // object yaw is global frame
      Eigen::MatrixXd m_rot_theta = Eigen::Rotation2D(obj_yaw - ego_yaw).toRotationMatrix();
      m_twist_cov = m_twist_cov + m_rot_theta.transpose() * m_cov_ego_twist * m_rot_theta;
    }

    // 2-b. add odometry yaw rate uncertainty to the object linear twist covariance
    {
      const double cov_by_yaw_rate = cov_yaw_rate * r2;
      Eigen::MatrixXd m_rot_theta = Eigen::Rotation2D(theta).toRotationMatrix();
      Eigen::MatrixXd m_twist_cov_rot = m_rot_theta.transpose() * m_twist_cov * m_rot_theta;
      m_twist_cov_rot(1, 1) += cov_by_yaw_rate;  // yaw rate uncertainty is added to y-y element
      m_twist_cov = m_rot_theta * m_twist_cov_rot * m_rot_theta.transpose();
    }

    // update the covariance matrix
    object_twist_cov[XYZRPY_COV_IDX::X_X] = m_twist_cov(0, 0);
    object_twist_cov[XYZRPY_COV_IDX::X_Y] = m_twist_cov(0, 1);
    object_twist_cov[XYZRPY_COV_IDX::Y_X] = m_twist_cov(1, 0);
    object_twist_cov[XYZRPY_COV_IDX::Y_Y] = m_twist_cov(1, 1);
  }
}

}  // namespace uncertainty
}  // namespace autoware::multi_object_tracker

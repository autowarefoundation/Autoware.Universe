// Copyright 2015-2019 Autoware Foundation
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

#include "localization_evaluator/localization_evaluator_core.hpp"

#include "localization_evaluator/geodetic.hpp"
#include "localization_evaluator/interpolation.hpp"

#include <deque>

// double rotationError(Eigen::Matrix4d &pose_error) {
//   double a = pose_error(0, 0);
//   double b = pose_error(1, 1);
//   double c = pose_error(2, 2);
//   double d = 0.5*(a+b+c-1.0);
//   return acos(max(min(d,1.0),-1.0));
// }

// double translationError(Eigen::Matrix4d &pose_error) {
//   double dx = pose_error(0, 3);
//   double dy = pose_error(1, 3);
//   double dz = pose_error(2, 3);
//   return sqrt(dx*dx+dy*dy+dz*dz);
// }

void msgToMatrix(geometry_msgs::msg::PoseStamped::ConstSharedPtr & pose_ptr, Eigen::Affine3d & out)
{
  Eigen::Affine3d t(Eigen::Translation3d(Eigen::Vector3d(
    pose_ptr->pose.position.x, pose_ptr->pose.position.y, pose_ptr->pose.position.z)));
  Eigen::Quaternion q(
    pose_ptr->pose.orientation.w, pose_ptr->pose.orientation.x, pose_ptr->pose.orientation.y,
    pose_ptr->pose.orientation.z);
  out = t * q;
}

LocalizationEvaluator::LocalizationEvaluator() : rclcpp::Node("localization_evaluator")
{
  std::string vehicle_pose_topic =
    declare_parameter("input_vehicle_pose_topic", "/localization/pose_twist_fusion_filter/pose");
  std::string ground_truth_pose_topic =
    declare_parameter("input_ground_truth_pose_topic", "/ground_truth");
  std::string error_pose_topic =
    declare_parameter("output_pose_error_topic", "/relative_pose_error");
  vehicle_pose_sub_ = create_subscription<geometry_msgs::msg::PoseStamped>(
    vehicle_pose_topic, rclcpp::QoS{100},
    std::bind(&LocalizationEvaluator::callbackVehicleOdometry, this, std::placeholders::_1));
  ground_truth_pose_sub_ = create_subscription<geometry_msgs::msg::PoseStamped>(
    ground_truth_pose_topic, rclcpp::QoS{100},
    std::bind(&LocalizationEvaluator::callbackGroundTruthOdometry, this, std::placeholders::_1));
  pose_error_pub_ = create_publisher<geometry_msgs::msg::PoseStamped>(error_pose_topic, 1);
  has_ground_truth_ = false;
  last_vehicle_trans_.setIdentity();
  last_ground_truth_trans_.setIdentity();
}

LocalizationEvaluator::~LocalizationEvaluator() {}

void LocalizationEvaluator::callbackGroundTruthOdometry(
  geometry_msgs::msg::PoseStamped::ConstSharedPtr pose_ground_truth_msg_ptr)
{
  curr_ground_truth_pose_ = pose_ground_truth_msg_ptr;
  has_ground_truth_ = true;
}

void LocalizationEvaluator::callbackVehicleOdometry(
  geometry_msgs::msg::PoseStamped::ConstSharedPtr pose_msg_ptr)
{
  if (
    has_ground_truth_ && rclcpp::Time(pose_msg_ptr->header.stamp) >
                           rclcpp::Time(curr_ground_truth_pose_->header.stamp)) {
    Eigen::Affine3d prev_trans, post_trans, curr_trans;
    rclcpp::Time prev_time, post_time, curr_time;
    prev_trans.setIdentity();
    if (prev_vehicle_pose_) {
      msgToMatrix(prev_vehicle_pose_, prev_trans);
      prev_time = rclcpp::Time(prev_vehicle_pose_->header.stamp);
    } else {
      prev_time = get_clock()->now();
      RCLCPP_INFO(get_logger(), "No prev pose");
    }
    msgToMatrix(pose_msg_ptr, post_trans);
    post_time = rclcpp::Time(pose_msg_ptr->header.stamp);
    curr_time = rclcpp::Time(curr_ground_truth_pose_->header.stamp);

    double t = interpolation::getTimeCoeffients(curr_time, prev_time, post_time);
    RCLCPP_INFO(get_logger(), "Interpolate Coefficient : %lf", t);
    if (t > 1.0 || t < 0.0) {
      RCLCPP_WARN(get_logger(), "Interpolate Coefficient has to be in range of [0,1]");
      return;
      // RCLCPP_INFO(get_logger(), "duration : %lf", static_cast<double>((post_time -
      // prev_time).nanoseconds())); RCLCPP_INFO(get_logger(), "step : %lf",
      // static_cast<double>((curr_time - prev_time).nanoseconds())); RCLCPP_INFO(get_logger(),
      // "post > curr ? : %d", static_cast<int>(post_time > curr_time)); RCLCPP_INFO(get_logger(),
      // "prev < curr ? : %d", static_cast<int>(prev_time < curr_time));
    }

    interpolation::interpolateTransform(t, prev_trans, post_trans, curr_trans);

    Eigen::Affine3d curr_ground_truth_trans, error_trans;
    msgToMatrix(curr_ground_truth_pose_, curr_ground_truth_trans);

    calculateError(curr_trans, curr_ground_truth_trans, error_trans);
    publishError(error_trans);

    last_vehicle_trans_ = curr_trans;
    last_ground_truth_trans_ = curr_ground_truth_trans;
    last_ground_truth_pose_ = curr_ground_truth_pose_;
    has_ground_truth_ = false;
  }

  prev_vehicle_pose_ = pose_msg_ptr;
}

// void KittiEvaluator::callbackGroundTruthOdometry(
//   geometry_msgs::msg::PoseStamped::ConstSharedPtr pose_ground_truth_msg_ptr)
// {
//   if (!vehicle_pose_queue_.empty()) {
//     RCLCPP_INFO(get_logger(), "We have ekf pose! Size : %ld", vehicle_pose_queue_.size());
//   }

//   Eigen::Affine3d ground_truth_trans;
//   msgToMatrix(pose_ground_truth_msg_ptr, ground_truth_trans);

//   rclcpp::Time curr_time(pose_ground_truth_msg_ptr->header.stamp);
//   geometry_msgs::msg::PoseStamped::ConstSharedPtr prev_pose;
//   while (!vehicle_pose_queue_.empty() && rclcpp::Time(vehicle_pose_queue_.front()->header.stamp)
//   <
//                                           curr_time - rclcpp::Duration::from_seconds(delta)) {
//     prev_pose = vehicle_pose_queue_.front();
//     vehicle_pose_queue_.pop_front();
//   }
//   if (!vehicle_pose_queue_.empty()) {
//     auto post_pose = vehicle_pose_queue_.front();
//     vehicle_pose_queue_.pop_front();

//     Eigen::Affine3d prev_trans;
//     rclcpp::Time prev_time = get_clock()->now();
//     prev_trans.setIdentity();
//     if (prev_pose) {
//       prev_time = rclcpp::Time(prev_pose->header.stamp);
//       msgToMatrix(prev_pose, prev_trans);
//     }
//     Eigen::Affine3d post_trans;
//     rclcpp::Time post_time(post_pose->header.stamp);
//     msgToMatrix(post_pose, post_trans);

//     rclcpp::Time fake_time = curr_time - rclcpp::Duration::from_seconds(delta);
//     double t = interpolation::getTimeCoeffients(fake_time, prev_time, post_time);
//     if (t > 1 || t < 0) {
//       RCLCPP_INFO(get_logger(), "????????????");
//     }
//     RCLCPP_INFO(get_logger(), "Interpolate Coefficient : %lf", t);
//     Eigen::Affine3d vehicle_trans;
//     interpolation::interpolateTransform(t, prev_trans, post_trans, vehicle_trans);

//     // RCLCPP_INFO(get_logger(), "%lf %lf %lf %lf", vehicle_trans(0,0), vehicle_trans(0,1),
//     // vehicle_trans(0,2), vehicle_trans(0,3)); RCLCPP_INFO(get_loggerrocker
//     // "%lf %lf %lf %lf", vehicle_trans(1,0),
//     // vehicle_trans(1,1), vehicle_trans(1,2), vehicle_trans(1,3)); RCLCPP_INFO(get_logger(),
//     "%lf
//     // %lf %lf %lf", vehicle_trans(2,0), vehicle_trans(2,1), vehicle_trans(2,2),
//     // vehicle_trans(2,3)); RCLCPP_INFO(get_logger(), "%lf %lf %lf %lf", vehicle_trans(3,0),
//     // vehicle_trans(3,1), vehicle_trans(3,2), vehicle_trans(3,3));

//     Eigen::Affine3d error_trans;
//     calculateError(vehicle_trans, ground_truth_trans, error_trans);
//     last_vehicle_trans_ = vehicle_trans;
//     publishError(error_trans);
//   } else {
//     RCLCPP_INFO(get_logger(), "Cannot Find Valid Pose!");
//   }

//   last_groud_truth_trans_ = ground_truth_trans;
// }

// void KittiEvaluator::callbackVehicleOdometry(
//   geometry_msgs::msg::PoseStamped::ConstSharedPtr pose_msg_ptr)
// {
//   vehicle_pose_queue_.push_back(pose_msg_ptr);
// }

void LocalizationEvaluator::calculateError(
  Eigen::Affine3d & vehicle_trans, Eigen::Affine3d & ground_truth_trans,
  Eigen::Affine3d & error_trans)
{
  Eigen::Affine3d delta_ground_truth = last_ground_truth_trans_.inverse() * ground_truth_trans;
  Eigen::Affine3d delta_vehicle = last_vehicle_trans_.inverse() * vehicle_trans;
  error_trans = delta_vehicle.inverse() * delta_ground_truth;

  Eigen::Matrix4d trans = error_trans.matrix();

  RCLCPP_INFO(
    get_logger(), "Translation Error: x = %lf, y = %lf, z = %lf ", trans(0, 3), trans(1, 3),
    trans(2, 3));

  Eigen::Vector3d euler = trans.block<3, 3>(0, 0).eulerAngles(0, 1, 2);
  RCLCPP_INFO(
    get_logger(), "Rotation Error: roll = %lf, pitch = %lf, yaw = %lf ", euler(0), euler(1),
    euler(2));
}

void LocalizationEvaluator::publishError(Eigen::Affine3d & error_trans)
{
  geometry_msgs::msg::PoseStamped pose;
  Eigen::Quaterniond q(error_trans.rotation());
  Eigen::Vector3d t = error_trans.translation();
  pose.header.stamp = get_clock()->now();
  pose.pose.position.x = t.x();
  pose.pose.position.y = t.y();
  pose.pose.position.z = t.z();
  pose.pose.orientation.w = q.w();
  pose.pose.orientation.x = q.x();
  pose.pose.orientation.y = q.y();
  pose.pose.orientation.z = q.z();
  pose_error_pub_->publish(pose);
}

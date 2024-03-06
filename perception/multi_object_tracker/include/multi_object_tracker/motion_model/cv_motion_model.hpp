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
//

#ifndef MULTI_OBJECT_TRACKER__MOTION_MODEL__CV_MOTION_MODEL_HPP_
#define MULTI_OBJECT_TRACKER__MOTION_MODEL__CV_MOTION_MODEL_HPP_

#include "multi_object_tracker/utils/utils.hpp"

#include <Eigen/Core>
#include <kalman_filter/kalman_filter.hpp>
#include <rclcpp/rclcpp.hpp>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

class CVMotionModel
{
private:
  // attributes
  rclcpp::Logger logger_;
  rclcpp::Time last_update_time_;

private:
  // state
  bool is_initialized_{false};
  KalmanFilter ekf_;

  // motion parameters
  struct MotionParams
  {
    double q_cov_x;
    double q_cov_y;
    double q_cov_vx;
    double q_cov_vy;
    double max_vx;
    double max_vy;
  } motion_params_;

public:
  CVMotionModel();

  enum IDX { X = 0, Y = 1, VX = 2, VY = 3 };
  const char DIM = 4;

  bool init(const rclcpp::Time & time, const Eigen::MatrixXd & X, const Eigen::MatrixXd & P);
  bool init(
    const rclcpp::Time & time, const double & x, const double & y,
    const std::array<double, 36> & pose_cov, const double & vx, const double & vy,
    const std::array<double, 36> & twist_cov);

  bool checkInitialized() const
  {
    // if the state is not initialized, return false
    if (!is_initialized_) {
      RCLCPP_WARN(logger_, "CVMotionModel is not initialized.");
      return false;
    }
    return true;
  }

  void setDefaultParams();

  void setMotionParams(
    const double & q_stddev_x, const double & q_stddev_y, const double & q_stddev_vx,
    const double & q_stddev_vy);

  void setMotionLimits(const double & max_vx, const double & max_vy);

  Eigen::MatrixXd getStateVector() const
  {
    Eigen::MatrixXd X_t(DIM, 1);
    ekf_.getX(X_t);
    return X_t;
  }

  double getStateElement(unsigned int idx) const { return ekf_.getXelement(idx); }

  bool updateStatePose(const double & x, const double & y, const std::array<double, 36> & pose_cov);

  bool updateStatePoseVel(
    const double & x, const double & y, const std::array<double, 36> & pose_cov, const double & vx,
    const double & vy, const std::array<double, 36> & twist_cov);

  bool adjustPosition(const double & x, const double & y);

  bool limitStates();

  bool predictState(const rclcpp::Time & time);

  bool predictState(const double dt, KalmanFilter & ekf) const;

  bool getPredictedState(const rclcpp::Time & time, Eigen::MatrixXd & X, Eigen::MatrixXd & P) const;

  bool getPredictedState(
    const rclcpp::Time & time, geometry_msgs::msg::Pose & pose, std::array<double, 36> & pose_cov,
    geometry_msgs::msg::Twist & twist, std::array<double, 36> & twist_cov) const;
};

#endif  // MULTI_OBJECT_TRACKER__MOTION_MODEL__CV_MOTION_MODEL_HPP_

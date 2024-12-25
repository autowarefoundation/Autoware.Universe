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

#ifndef AUTOWARE__PURE_PURSUIT__UTIL__PLANNING_UTILS_HPP_
#define AUTOWARE__PURE_PURSUIT__UTIL__PLANNING_UTILS_HPP_

#define EIGEN_MPL2_ONLY

#include "interpolate.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <rclcpp/rclcpp.hpp>

#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

#include <tf2/transform_datatypes.h>
#include <tf2/utils.h>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

#include <memory>
#include <utility>
#include <vector>

#define PLANNING_UTILS_LOGGER "planning_utils"

namespace autoware::pure_pursuit
{
namespace planning_utils
{
constexpr double ERROR = 1e-6;
double calcArcLengthFromWayPoint(
  const autoware_planning_msgs::msg::Trajectory & input_path, const size_t src_idx,
  const size_t dst_idx);
double calcCurvature(
  const geometry_msgs::msg::Point & target, const geometry_msgs::msg::Pose & curr_pose);
double calcDistSquared2D(const geometry_msgs::msg::Point & p, const geometry_msgs::msg::Point & q);
double calcStopDistanceWithConstantJerk(const double & v_init, const double & j);
double calcLateralError2D(
  const geometry_msgs::msg::Point & a_start, const geometry_msgs::msg::Point & a_end,
  const geometry_msgs::msg::Point & b);
double calcRadius(
  const geometry_msgs::msg::Point & target, const geometry_msgs::msg::Pose & current_pose);
double convertCurvatureToSteeringAngle(double wheel_base, double kappa);

std::vector<geometry_msgs::msg::Pose> extractPoses(
  const autoware_planning_msgs::msg::Trajectory & motions);

std::pair<bool, int32_t> findClosestIdxWithDistAngThr(
  const std::vector<geometry_msgs::msg::Pose> & poses,
  const geometry_msgs::msg::Pose & current_pose, const double th_dist = 3.0,
  const double th_yaw = M_PI_2);

int8_t getLaneDirection(const std::vector<geometry_msgs::msg::Pose> & poses, double th_dist = 0.5);

double normalizeEulerAngle(const double euler);

geometry_msgs::msg::Point transformToAbsoluteCoordinate2D(
  const geometry_msgs::msg::Point & point, const geometry_msgs::msg::Pose & current_pose);

geometry_msgs::msg::Point transformToRelativeCoordinate2D(
  const geometry_msgs::msg::Point & point, const geometry_msgs::msg::Pose & current_pose);

geometry_msgs::msg::Quaternion getQuaternionFromYaw(const double _yaw);

}  // namespace planning_utils
}  // namespace autoware::pure_pursuit

#endif  // AUTOWARE__PURE_PURSUIT__UTIL__PLANNING_UTILS_HPP_

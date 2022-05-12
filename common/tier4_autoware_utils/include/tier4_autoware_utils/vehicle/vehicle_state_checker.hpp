// Copyright 2022 Tier IV, Inc.
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

#ifndef TIER4_AUTOWARE_UTILS__VEHICLE__VEHICLE_STATE_CHECKER_HPP_
#define TIER4_AUTOWARE_UTILS__VEHICLE__VEHICLE_STATE_CHECKER_HPP_

#include <rclcpp/rclcpp.hpp>

#include <autoware_auto_planning_msgs/msg/trajectory.hpp>
#include <autoware_auto_vehicle_msgs/msg/velocity_report.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>

#include <deque>
#include <memory>

namespace tier4_autoware_utils
{

using autoware_auto_planning_msgs::msg::Trajectory;
using autoware_auto_vehicle_msgs::msg::VelocityReport;

class VehicleStateChecker
{
public:
  explicit VehicleStateChecker(rclcpp::Node * node);

  bool checkStopped(const double stop_duration) const;

  bool checkStoppedAtStopPoint(const double stop_duration) const;

  bool checkStoppedWithoutLocalization(const double stop_duration) const;

  rclcpp::Logger getLogger() { return logger_; }

private:
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub_odom_;
  rclcpp::Subscription<VelocityReport>::SharedPtr sub_velocity_status_;
  rclcpp::Subscription<Trajectory>::SharedPtr sub_trajectory_;

  rclcpp::Clock::SharedPtr clock_;
  rclcpp::Logger logger_;

  nav_msgs::msg::Odometry::SharedPtr odometry_ptr_;
  Trajectory::SharedPtr trajectory_ptr_;

  std::deque<geometry_msgs::msg::TwistStamped> twist_buffer_;
  std::deque<VelocityReport> velocity_status_buffer_;

  static constexpr double velocity_buffer_time_sec = 10.0;
  static constexpr double th_arrived_distance_m = 1.0;

  void onOdom(const nav_msgs::msg::Odometry::SharedPtr msg);
  void onVelocityStatus(const VelocityReport::SharedPtr msg);
  void onTrajectory(const Trajectory::SharedPtr msg);
};
}  // namespace tier4_autoware_utils

#endif  // TIER4_AUTOWARE_UTILS__VEHICLE__VEHICLE_STATE_CHECKER_HPP_

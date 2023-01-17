// Copyright 2021 Tier IV, Inc.
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

#ifndef PLANNING_VALIDATOR__UTILS_HPP_
#define PLANNING_VALIDATOR__UTILS_HPP_

#include <rclcpp/rclcpp.hpp>

#include <autoware_auto_planning_msgs/msg/trajectory.hpp>

#include <string>
#include <utility>
#include <vector>

namespace planning_validator
{
using autoware_auto_planning_msgs::msg::Trajectory;
using autoware_auto_planning_msgs::msg::TrajectoryPoint;

std::pair<double, size_t> getMaxValAndIdx(const std::vector<double> & v);

std::pair<double, size_t> getMinValAndIdx(const std::vector<double> & v);

std::pair<double, size_t> getAbsMaxValAndIdx(const std::vector<double> & v);

Trajectory resampleTrajectory(const Trajectory & trajectory, const double min_interval);

void calcCurvature(const Trajectory & trajectory, std::vector<double> & v);

void calcIntervalDistance(const Trajectory & trajectory, std::vector<double> & v);

void calcLateralAcceleration(const Trajectory & trajectory, std::vector<double> & v);

void getLongitudinalAccArray(const Trajectory & trajectory, std::vector<double> & v);

void calcRelativeAngles(const Trajectory & trajectory, std::vector<double> & v);

void calcSteeringAngles(const Trajectory & trajectory, const double wheelbase, std::vector<double> & v);

void calcSteeringRates(const Trajectory & trajectory, const double wheelbase, std::vector<double> & v);

bool checkFinite(const TrajectoryPoint & point);

void shiftPose(geometry_msgs::msg::Pose & pose, double longitudinal);

}  // namespace planning_validator

#endif  // PLANNING_VALIDATOR__UTILS_HPP_

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

#ifndef AUTOWARE__PLANNING_EVALUATOR__METRICS__DEVIATION_METRICS_HPP_
#define AUTOWARE__PLANNING_EVALUATOR__METRICS__DEVIATION_METRICS_HPP_

#include "autoware/universe_utils/math/accumulator.hpp"

#include "autoware_planning_msgs/msg/trajectory.hpp"
#include "autoware_planning_msgs/msg/trajectory_point.hpp"

namespace planning_diagnostics
{
namespace metrics
{
using autoware::universe_utils::Accumulator;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;

/**
 * @brief calculate lateral deviation of the given trajectory from the reference trajectory
 * @param [in] ref reference trajectory
 * @param [in] traj input trajectory
 * @return calculated statistics
 */
Accumulator<double> calcLateralDeviation(const Trajectory & ref, const Trajectory & traj);

/**
 * @brief calculate lateral trajectory displacement from the previous trajectory and the trajectory
 * @param [in] prev previous trajectory
 * @param [in] traj input trajectory
 * @param [in] base_pose base pose
 * @return calculated statistics
 */
Accumulator<double> calcLocalLateralTrajectoryDisplacement(
  const Trajectory & prev, const Trajectory & traj, const Pose & base_pose);

/**
 * @brief calculate yaw deviation of the given trajectory from the reference trajectory
 * @param [in] ref reference trajectory
 * @param [in] traj input trajectory
 * @return calculated statistics
 */
Accumulator<double> calcYawDeviation(const Trajectory & ref, const Trajectory & traj);

/**
 * @brief calculate velocity deviation of the given trajectory from the reference trajectory
 * @param [in] ref reference trajectory
 * @param [in] traj input trajectory
 * @return calculated statistics
 */
Accumulator<double> calcVelocityDeviation(const Trajectory & ref, const Trajectory & traj);

/**
 * @brief calculate longitudinal deviation of the given ego pose from the modified goal pose
 * @param [in] base_pose base pose
 * @param [in] target_point target point
 * @return calculated statistics
 */
Accumulator<double> calcLongitudinalDeviation(const Pose & base_pose, const Point & target_point);

/**
 * @brief calculate lateral deviation of the given ego pose from the modified goal pose
 * @param [in] base_pose base pose
 * @param [in] target_point target point
 * @return calculated statistics
 */
Accumulator<double> calcLateralDeviation(const Pose & base_pose, const Point & target_point);

/**
 * @brief calculate yaw deviation of the given ego pose from the modified goal pose
 * @param [in] base_pose base pose
 * @param [in] target_pose target pose
 * @return calculated statistics
 */
Accumulator<double> calcYawDeviation(const Pose & base_pose, const Pose & target_pose);

}  // namespace metrics
}  // namespace planning_diagnostics

#endif  // AUTOWARE__PLANNING_EVALUATOR__METRICS__DEVIATION_METRICS_HPP_

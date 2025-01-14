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

#ifndef AUTOWARE__PLANNING_EVALUATOR__METRICS__STABILITY_METRICS_HPP_
#define AUTOWARE__PLANNING_EVALUATOR__METRICS__STABILITY_METRICS_HPP_

#include "autoware/universe_utils/math/accumulator.hpp"

#include "autoware_planning_msgs/msg/trajectory.hpp"
#include <nav_msgs/msg/odometry.hpp>

namespace planning_diagnostics
{
namespace metrics
{
using autoware::universe_utils::Accumulator;
using autoware_planning_msgs::msg::Trajectory;

/**
 * @brief calculate the discrete Frechet distance between two trajectories
 * @param [in] traj1 first trajectory
 * @param [in] traj2 second trajectory
 * @return calculated statistics
 */
Accumulator<double> calcFrechetDistance(const Trajectory & traj1, const Trajectory & traj2);

/**
 * @brief calculate the lateral distance between two trajectories
 * @param [in] traj1 first trajectory
 * @param [in] traj2 second trajectory
 * @return calculated statistics
 */
Accumulator<double> calcLateralDistance(const Trajectory & traj1, const Trajectory & traj2);

/**
 * @brief calculate the total lateral displacement between two trajectories
 * @details Evaluates the cumulative absolute lateral displacement by sampling points
 *          along the first trajectory and measuring their offset from the second trajectory.
 *          The evaluation section length is determined by the ego vehicle's velocity and
 *          the specified evaluation time.
 *
 * @param traj1 first trajectory to compare
 * @param traj2 second trajectory to compare against
 * @param [in] ego_odom current ego vehicle odometry containing pose and velocity
 * @param [in]  trajectory_eval_time_s time duration for trajectory evaluation in seconds
 * @return statistical accumulator containing the total lateral displacement
 */
Accumulator<double> calcLookaheadLateralTrajectoryDisplacement(
  const Trajectory traj1, const Trajectory traj2, const nav_msgs::msg::Odometry & ego_odom,
  const double trajectory_eval_time_s);

}  // namespace metrics
}  // namespace planning_diagnostics

#endif  // AUTOWARE__PLANNING_EVALUATOR__METRICS__STABILITY_METRICS_HPP_

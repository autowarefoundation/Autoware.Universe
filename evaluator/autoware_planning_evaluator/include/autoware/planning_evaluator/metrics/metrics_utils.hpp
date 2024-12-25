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

#ifndef AUTOWARE__PLANNING_EVALUATOR__METRICS__METRICS_UTILS_HPP_
#define AUTOWARE__PLANNING_EVALUATOR__METRICS__METRICS_UTILS_HPP_

#include "autoware_planning_msgs/msg/trajectory.hpp"
#include "autoware_planning_msgs/msg/trajectory_point.hpp"

namespace planning_diagnostics
{
namespace metrics
{
namespace utils
{
using autoware_planning_msgs::msg::Trajectory;
using geometry_msgs::msg::Pose;

/**
 * @brief find the index in the trajectory at the given distance of the given index
 * @param [in] traj input trajectory
 * @param [in] curr_id index
 * @param [in] distance distance
 * @return index of the trajectory point at distance ahead of traj[curr_id]
 */
size_t getIndexAfterDistance(const Trajectory & traj, const size_t curr_id, const double distance);

/**
 * @brief trim a trajectory from the current ego pose to some fixed time or distance
 * @param [in] traj input trajectory to trim
 * @param [in] max_dist_m [m] maximum distance ahead of the ego pose
 * @param [in] max_time_s [s] maximum time ahead of the ego pose
 * @return sub-trajectory starting from the ego pose and of maximum length max_dist_m, maximum
 * duration max_time_s
 */
Trajectory get_lookahead_trajectory(
  const Trajectory & traj, const Pose & ego_pose, const double max_dist_m, const double max_time_s);

/**
 * @brief calculate the total distance from ego position to the end of trajectory
 * @details finds the nearest point to ego position on the trajectory and calculates
 *          the cumulative distance by summing up the distances between consecutive points
 *          from that position to the end of the trajectory.
 *
 * @param [in] traj input trajectory to calculate distance along
 * @param [in] ego_pose current ego vehicle pose
 * @return total distance from ego position to trajectory end in meters
 */
double calc_lookahead_trajectory_distance(const Trajectory & traj, const Pose & ego_pose);

}  // namespace utils
}  // namespace metrics
}  // namespace planning_diagnostics
#endif  // AUTOWARE__PLANNING_EVALUATOR__METRICS__METRICS_UTILS_HPP_

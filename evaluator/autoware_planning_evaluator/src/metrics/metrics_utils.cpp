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

#include "autoware/motion_utils/trajectory/trajectory.hpp"
#include "autoware/planning_evaluator/metrics/trajectory_metrics.hpp"
#include "autoware/universe_utils/geometry/geometry.hpp"

namespace planning_diagnostics
{
namespace metrics
{
namespace utils
{
using autoware::universe_utils::calcDistance2d;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;
using geometry_msgs::msg::Pose;

size_t getIndexAfterDistance(const Trajectory & traj, const size_t curr_id, const double distance)
{
  // Get Current Trajectory Point
  const TrajectoryPoint & curr_p = traj.points.at(curr_id);

  size_t target_id = curr_id;
  for (size_t traj_id = curr_id + 1; traj_id < traj.points.size(); ++traj_id) {
    double current_distance = calcDistance2d(traj.points.at(traj_id), curr_p);
    if (current_distance >= distance) {
      target_id = traj_id;
      break;
    }
  }

  return target_id;
}

Trajectory get_lookahead_trajectory(
  const Trajectory & traj, const Pose & ego_pose, const double max_dist_m, const double max_time_s)
{
  if (traj.points.empty()) {
    return traj;
  }

  const auto ego_index =
    autoware::motion_utils::findNearestSegmentIndex(traj.points, ego_pose.position);
  Trajectory lookahead_traj;
  lookahead_traj.header = traj.header;
  double dist = 0.0;
  double time = 0.0;
  auto curr_point_it = std::next(traj.points.begin(), ego_index);
  auto prev_point_it = curr_point_it;
  while (curr_point_it != traj.points.end() && dist <= max_dist_m && time <= max_time_s) {
    lookahead_traj.points.push_back(*curr_point_it);
    const auto d = autoware::universe_utils::calcDistance2d(
      prev_point_it->pose.position, curr_point_it->pose.position);
    dist += d;
    if (prev_point_it->longitudinal_velocity_mps != 0.0) {
      time += d / std::abs(prev_point_it->longitudinal_velocity_mps);
    }
    prev_point_it = curr_point_it;
    ++curr_point_it;
  }
  return lookahead_traj;
}

double calc_lookahead_trajectory_distance(const Trajectory & traj, const Pose & ego_pose)
{
  const auto ego_index =
    autoware::motion_utils::findNearestSegmentIndex(traj.points, ego_pose.position);
  double dist = 0.0;
  auto curr_point_it = std::next(traj.points.begin(), ego_index);
  auto prev_point_it = curr_point_it;
  for (size_t i = 0; i < traj.points.size(); ++i) {
    const auto d = autoware::universe_utils::calcDistance2d(
      prev_point_it->pose.position, curr_point_it->pose.position);
    dist += d;
    prev_point_it = curr_point_it;
    ++curr_point_it;
  }

  return dist;
}
}  // namespace utils
}  // namespace metrics
}  // namespace planning_diagnostics

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

#include "autoware/planning_evaluator/metrics/stability_metrics.hpp"

#include "autoware/motion_utils/resample/resample.hpp"
#include "autoware/motion_utils/trajectory/trajectory.hpp"
#include "autoware/planning_evaluator/metrics/metrics_utils.hpp"
#include "autoware/universe_utils/geometry/geometry.hpp"

#include <Eigen/Core>

#include "autoware_planning_msgs/msg/trajectory_point.hpp"

#include <algorithm>

namespace planning_diagnostics
{
namespace metrics
{
using autoware_planning_msgs::msg::TrajectoryPoint;

Accumulator<double> calcFrechetDistance(const Trajectory & traj1, const Trajectory & traj2)
{
  Accumulator<double> stat;

  if (traj1.points.empty() || traj2.points.empty()) {
    return stat;
  }

  Eigen::MatrixXd ca = Eigen::MatrixXd::Zero(traj1.points.size(), traj2.points.size());

  for (size_t i = 0; i < traj1.points.size(); ++i) {
    for (size_t j = 0; j < traj2.points.size(); ++j) {
      const double dist =
        autoware::universe_utils::calcDistance2d(traj1.points[i], traj2.points[j]);
      if (i > 0 && j > 0) {
        ca(i, j) = std::max(std::min(ca(i - 1, j), std::min(ca(i - 1, j - 1), ca(i, j - 1))), dist);
      } else if (i > 0 /*&& j == 0*/) {
        ca(i, j) = std::max(ca(i - 1, 0), dist);
      } else if (j > 0 /*&& i == 0*/) {
        ca(i, j) = std::max(ca(0, j - 1), dist);
      } else { /* i == j == 0 */
        ca(i, j) = dist;
      }
    }
  }
  stat.add(ca(traj1.points.size() - 1, traj2.points.size() - 1));
  return stat;
}

Accumulator<double> calcLateralDistance(const Trajectory & traj1, const Trajectory & traj2)
{
  Accumulator<double> stat;
  if (traj1.points.empty()) {
    return stat;
  }
  for (const auto & point : traj2.points) {
    const auto p0 = autoware::universe_utils::getPoint(point);
    // find nearest segment
    const size_t nearest_segment_idx =
      autoware::motion_utils::findNearestSegmentIndex(traj1.points, p0);
    double dist;
    // distance to segment
    if (
      nearest_segment_idx == traj1.points.size() - 2 &&
      autoware::motion_utils::calcLongitudinalOffsetToSegment(
        traj1.points, nearest_segment_idx, p0) >
        autoware::universe_utils::calcDistance2d(
          traj1.points[nearest_segment_idx], traj1.points[nearest_segment_idx + 1])) {
      // distance to last point
      dist = autoware::universe_utils::calcDistance2d(traj1.points.back(), p0);
    } else if (  // NOLINT
      nearest_segment_idx == 0 && autoware::motion_utils::calcLongitudinalOffsetToSegment(
                                    traj1.points, nearest_segment_idx, p0) <= 0) {
      // distance to first point
      dist = autoware::universe_utils::calcDistance2d(traj1.points.front(), p0);
    } else {
      // orthogonal distance
      const auto p1 = autoware::universe_utils::getPoint(traj1.points[nearest_segment_idx]);
      const auto p2 = autoware::universe_utils::getPoint(traj1.points[nearest_segment_idx + 1]);
      dist = std::abs((p2.x - p1.x) * (p1.y - p0.y) - (p1.x - p0.x) * (p2.y - p1.y)) /
             std::sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
    }
    stat.add(dist);
  }
  return stat;
}

Accumulator<double> calcLookaheadLateralTrajectoryDisplacement(
  const Trajectory traj1, const Trajectory traj2, const nav_msgs::msg::Odometry & ego_odom,
  const double trajectory_eval_time_s)
{
  Accumulator<double> stat;

  if (traj1.points.empty() || traj2.points.empty()) {
    return stat;
  }

  const double ego_velocity =
    std::hypot(ego_odom.twist.twist.linear.x, ego_odom.twist.twist.linear.y);

  const double evaluation_section_length = trajectory_eval_time_s * std::abs(ego_velocity);

  const double traj1_lookahead_distance =
    utils::calc_lookahead_trajectory_distance(traj1, ego_odom.pose.pose);
  const double traj2_lookahead_distance =
    utils::calc_lookahead_trajectory_distance(traj2, ego_odom.pose.pose);

  if (
    traj1_lookahead_distance < evaluation_section_length ||
    traj2_lookahead_distance < evaluation_section_length) {
    return stat;
  }

  constexpr double num_evaluation_points = 10.0;
  const double interval = evaluation_section_length / num_evaluation_points;

  const auto resampled_traj1 = autoware::motion_utils::resampleTrajectory(
    utils::get_lookahead_trajectory(
      traj1, ego_odom.pose.pose, evaluation_section_length, trajectory_eval_time_s),
    interval);

  for (const auto & point : resampled_traj1.points) {
    const auto p0 = autoware::universe_utils::getPoint(point);
    const double dist = autoware::motion_utils::calcLateralOffset(traj2.points, p0);
    stat.add(std::abs(dist));
  }
  return stat;
}

}  // namespace metrics
}  // namespace planning_diagnostics

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

#include "autoware/planning_evaluator/metrics_calculator.hpp"

#include "autoware/motion_utils/trajectory/trajectory.hpp"
#include "autoware/planning_evaluator/metrics/deviation_metrics.hpp"
#include "autoware/planning_evaluator/metrics/metrics_utils.hpp"
#include "autoware/planning_evaluator/metrics/obstacle_metrics.hpp"
#include "autoware/planning_evaluator/metrics/stability_metrics.hpp"
#include "autoware/planning_evaluator/metrics/trajectory_metrics.hpp"
#include "autoware/universe_utils/geometry/geometry.hpp"
namespace planning_diagnostics
{
std::optional<Accumulator<double>> MetricsCalculator::calculate(
  const Metric metric, const Trajectory & traj) const
{
  // Functions to calculate trajectory metrics
  switch (metric) {
    case Metric::curvature:
      return metrics::calcTrajectoryCurvature(traj);
    case Metric::point_interval:
      return metrics::calcTrajectoryInterval(traj);
    case Metric::relative_angle:
      return metrics::calcTrajectoryRelativeAngle(traj, parameters.trajectory.min_point_dist_m);
    case Metric::length:
      return metrics::calcTrajectoryLength(traj);
    case Metric::duration:
      return metrics::calcTrajectoryDuration(traj);
    case Metric::velocity:
      return metrics::calcTrajectoryVelocity(traj);
    case Metric::acceleration:
      return metrics::calcTrajectoryAcceleration(traj);
    case Metric::jerk:
      return metrics::calcTrajectoryJerk(traj);
    case Metric::lateral_deviation:
      return metrics::calcLateralDeviation(reference_trajectory_, traj);
    case Metric::yaw_deviation:
      return metrics::calcYawDeviation(reference_trajectory_, traj);
    case Metric::velocity_deviation:
      return metrics::calcVelocityDeviation(reference_trajectory_, traj);
    case Metric::lateral_trajectory_displacement_local:
      return metrics::calcLocalLateralTrajectoryDisplacement(previous_trajectory_, traj, ego_pose_);
    case Metric::lateral_trajectory_displacement_lookahead:
      return metrics::calcLookaheadLateralTrajectoryDisplacement(
        previous_trajectory_, traj, ego_odometry_, parameters.trajectory.evaluation_time_s);
    case Metric::stability_frechet:
      return metrics::calcFrechetDistance(
        metrics::utils::get_lookahead_trajectory(
          previous_trajectory_, ego_pose_, parameters.trajectory.lookahead.max_dist_m,
          parameters.trajectory.lookahead.max_time_s),
        metrics::utils::get_lookahead_trajectory(
          traj, ego_pose_, parameters.trajectory.lookahead.max_dist_m,
          parameters.trajectory.lookahead.max_time_s));
    case Metric::stability:
      return metrics::calcLateralDistance(
        metrics::utils::get_lookahead_trajectory(
          previous_trajectory_, ego_pose_, parameters.trajectory.lookahead.max_dist_m,
          parameters.trajectory.lookahead.max_time_s),
        metrics::utils::get_lookahead_trajectory(
          traj, ego_pose_, parameters.trajectory.lookahead.max_dist_m,
          parameters.trajectory.lookahead.max_time_s));
    case Metric::obstacle_distance:
      return metrics::calcDistanceToObstacle(dynamic_objects_, traj);
    case Metric::obstacle_ttc:
      return metrics::calcTimeToCollision(dynamic_objects_, traj, parameters.obstacle.dist_thr_m);
    default:
      return {};
  }
}

std::optional<Accumulator<double>> MetricsCalculator::calculate(
  const Metric metric, const Pose & base_pose, const Pose & target_pose) const
{
  // Functions to calculate pose metrics
  switch (metric) {
    case Metric::modified_goal_longitudinal_deviation:
      return metrics::calcLongitudinalDeviation(base_pose, target_pose.position);
    case Metric::modified_goal_lateral_deviation:
      return metrics::calcLateralDeviation(base_pose, target_pose.position);
    case Metric::modified_goal_yaw_deviation:
      return metrics::calcYawDeviation(base_pose, target_pose);
    default:
      return {};
  }
}

void MetricsCalculator::setReferenceTrajectory(const Trajectory & traj)
{
  reference_trajectory_ = traj;
}

void MetricsCalculator::setPreviousTrajectory(const Trajectory & traj)
{
  previous_trajectory_ = traj;
}

void MetricsCalculator::setPredictedObjects(const PredictedObjects & objects)
{
  dynamic_objects_ = objects;
}

void MetricsCalculator::setEgoPose(const nav_msgs::msg::Odometry & ego_odometry)
{
  ego_pose_ = ego_odometry.pose.pose;
  ego_odometry_ = ego_odometry;
}

Pose MetricsCalculator::getEgoPose()
{
  return ego_pose_;
}

}  // namespace planning_diagnostics

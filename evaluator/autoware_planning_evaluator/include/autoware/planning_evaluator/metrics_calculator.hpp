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

#ifndef AUTOWARE__PLANNING_EVALUATOR__METRICS_CALCULATOR_HPP_
#define AUTOWARE__PLANNING_EVALUATOR__METRICS_CALCULATOR_HPP_
#include "autoware/planning_evaluator/metrics/metric.hpp"
#include "autoware/planning_evaluator/parameters.hpp"
#include "autoware/universe_utils/math/accumulator.hpp"

#include "autoware_perception_msgs/msg/predicted_objects.hpp"
#include "autoware_planning_msgs/msg/pose_with_uuid_stamped.hpp"
#include "autoware_planning_msgs/msg/trajectory.hpp"
#include "autoware_planning_msgs/msg/trajectory_point.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include <nav_msgs/msg/odometry.hpp>

#include <optional>

namespace planning_diagnostics
{
using autoware::universe_utils::Accumulator;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_planning_msgs::msg::PoseWithUuidStamped;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;

class MetricsCalculator
{
public:
  Parameters parameters;

  MetricsCalculator() = default;

  /**
   * @brief calculate
   * @param [in] metric Metric enum value
   * @return string describing the requested metric
   */
  std::optional<Accumulator<double>> calculate(const Metric metric, const Trajectory & traj) const;
  std::optional<Accumulator<double>> calculate(
    const Metric metric, const Pose & base_pose, const Pose & target_pose) const;

  /**
   * @brief set the reference trajectory used to calculate the deviation metrics
   * @param [in] traj input reference trajectory
   */
  void setReferenceTrajectory(const Trajectory & traj);

  /**
   * @brief set the previous trajectory used to calculate the stability metrics
   * @param [in] traj input previous trajectory
   */
  void setPreviousTrajectory(const Trajectory & traj);

  /**
   * @brief set the dynamic objects used to calculate obstacle metrics
   * @param [in] traj input previous trajectory
   */
  void setPredictedObjects(const PredictedObjects & objects);

  /**
   * @brief set the ego pose
   * @param [in] traj input previous trajectory
   */
  void setEgoPose(const nav_msgs::msg::Odometry & ego_odometry);

  /**
   * @brief get the ego pose
   * @return ego pose
   */
  Pose getEgoPose();

private:
  Trajectory reference_trajectory_;
  Trajectory reference_trajectory_lookahead_;
  Trajectory previous_trajectory_;
  Trajectory previous_trajectory_lookahead_;
  PredictedObjects dynamic_objects_;
  geometry_msgs::msg::Pose ego_pose_;
  nav_msgs::msg::Odometry ego_odometry_;
  PoseWithUuidStamped modified_goal_;
};  // class MetricsCalculator

}  // namespace planning_diagnostics

#endif  // AUTOWARE__PLANNING_EVALUATOR__METRICS_CALCULATOR_HPP_

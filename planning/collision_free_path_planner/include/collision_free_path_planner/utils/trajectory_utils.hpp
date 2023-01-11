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

#ifndef COLLISION_FREE_PATH_PLANNER__UTILS__TRAJECTORY_UTILS_HPP_
#define COLLISION_FREE_PATH_PLANNER__UTILS__TRAJECTORY_UTILS_HPP_

#include "collision_free_path_planner/common_structs.hpp"
#include "collision_free_path_planner/type_alias.hpp"
#include "eigen3/Eigen/Core"
#include "interpolation/linear_interpolation.hpp"
#include "interpolation/spline_interpolation.hpp"
#include "interpolation/spline_interpolation_points_2d.hpp"
#include "motion_utils/trajectory/trajectory.hpp"

#include "autoware_auto_planning_msgs/msg/path_point.hpp"
#include "autoware_auto_planning_msgs/msg/trajectory.hpp"

#include "boost/optional/optional_fwd.hpp"

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace tier4_autoware_utils
{
template <>
geometry_msgs::msg::Point getPoint(const collision_free_path_planner::ReferencePoint & p);

template <>
geometry_msgs::msg::Pose getPose(const collision_free_path_planner::ReferencePoint & p);
}  // namespace tier4_autoware_utils

namespace collision_free_path_planner
{
namespace trajectory_utils
{
template <typename T>
size_t findForwardIndex(const T & points, const size_t begin_idx, const double forward_length)
{
  double sum_length = 0.0;
  for (size_t i = begin_idx; i < points.size() - 1; ++i) {
    sum_length += tier4_autoware_utils::calcDistance2d(points.at(i), points.at(i + 1));
    if (sum_length > forward_length) {
      return i;
    }
  }
  return points.size() - 1;
}

template <typename T>
T cropPointsAfterOffsetPoint(
  const T & points, const geometry_msgs::msg::Point & target_pos, const size_t target_seg_idx,
  const double offset)
{
  if (points.empty()) {
    return T{};
  }

  double sum_length =
    -motion_utils::calcLongitudinalOffsetToSegment(points, target_seg_idx, target_pos);

  // search forward
  if (sum_length < offset) {
    for (size_t i = target_seg_idx + 1; i < points.size(); ++i) {
      sum_length += tier4_autoware_utils::calcDistance2d(points.at(i), points.at(i - 1));
      if (offset < sum_length) {
        return T{points.begin() + i - 1, points.end()};
      }
    }

    return T{};
  }

  // search backward
  for (size_t i = target_seg_idx; 0 < i;
       --i) {  // NOTE: use size_t since i is always positive value
    sum_length -= tier4_autoware_utils::calcDistance2d(points.at(i), points.at(i - 1));
    if (sum_length < offset) {
      return T{points.begin() + i - 1, points.end()};
    }
  }

  return points;
}

template <typename T>
T cropForwardPoints(
  const T & points, const geometry_msgs::msg::Point & target_pos, const size_t target_seg_idx,
  const double forward_length)
{
  if (points.empty()) {
    return T{};
  }

  double sum_length =
    -motion_utils::calcLongitudinalOffsetToSegment(points, target_seg_idx, target_pos);
  std::cerr << "sum length " << sum_length << std::endl;
  for (size_t i = target_seg_idx + 1; i < points.size(); ++i) {
    sum_length += tier4_autoware_utils::calcDistance2d(points.at(i), points.at(i - 1));
    if (forward_length < sum_length) {
      const size_t end_idx = i;
      return T{points.begin(), points.begin() + end_idx};
    }
  }

  return points;
}

template <typename T>
T cropBackwardPoints(
  const T & points, const geometry_msgs::msg::Point & target_pos, const size_t target_seg_idx,
  const double backward_length)
{
  if (points.empty()) {
    return T{};
  }

  double sum_length =
    -motion_utils::calcLongitudinalOffsetToSegment(points, target_seg_idx, target_pos);
  for (size_t i = target_seg_idx; 0 < i;
       --i) {  // NOTE: use size_t since i is always positive value
    sum_length -= tier4_autoware_utils::calcDistance2d(points.at(i), points.at(i - 1));
    if (sum_length < backward_length) {
      const size_t begin_idx = i - 1;
      return T{points.begin() + begin_idx, points.end()};
    }
  }

  return points;
}

template <typename T>
T cropPoints(
  const T & points, const geometry_msgs::msg::Point & target_pos, const size_t target_seg_idx,
  const double forward_length, const double backward_length)
{
  if (points.empty()) {
    return T{};
  }

  // NOTE: Cropping forward must be done first in order to keep target_seg_idx.
  const auto cropped_forward_points =
    cropForwardPoints(points, target_pos, target_seg_idx, forward_length);

  const size_t modified_target_seg_idx =
    std::min(target_seg_idx, cropped_forward_points.size() - 2);
  const auto cropped_points = cropBackwardPoints(
    cropped_forward_points, target_pos, modified_target_seg_idx, backward_length);

  return cropped_points;
}

template <typename T>
T clipForwardPoints(const T & points, const size_t begin_idx, const double forward_length)
{
  if (points.empty()) {
    return T{};
  }

  const size_t end_idx = findForwardIndex(points, begin_idx, forward_length);
  return T{points.begin() + begin_idx, points.begin() + end_idx};
}

template <typename T>
T clipBackwardPoints(
  const T & points, const size_t target_idx, const double backward_length,
  const double delta_length)
{
  if (points.empty()) {
    return T{};
  }

  const int begin_idx =
    std::max(0, static_cast<int>(target_idx) - static_cast<int>(backward_length / delta_length));
  return T{points.begin() + begin_idx, points.end()};
}

template <typename T>
T clipBackwardPoints(
  const T & points, const geometry_msgs::msg::Pose pose, const double backward_length,
  const double delta_length, const double delta_yaw)
{
  if (points.empty()) {
    return T{};
  }

  const auto target_idx_optional =
    motion_utils::findNearestIndex(points, pose, std::numeric_limits<double>::max(), delta_yaw);

  const size_t target_idx = target_idx_optional
                              ? *target_idx_optional
                              : motion_utils::findNearestIndex(points, pose.position);

  const int begin_idx =
    std::max(0, static_cast<int>(target_idx) - static_cast<int>(backward_length / delta_length));
  return T{points.begin() + begin_idx, points.end()};
}

// NOTE: acceleration is not converted
template <typename T>
std::vector<geometry_msgs::msg::Point> convertToPoints(const std::vector<T> & points)
{
  std::vector<geometry_msgs::msg::Point> geom_points;
  for (const auto & point : points) {
    geom_points.push_back(tier4_autoware_utils::getPoint(point));
  }
  return geom_points;
}

template <typename T>
std::vector<geometry_msgs::msg::Pose> convertToPoses(const std::vector<T> & points)
{
  std::vector<geometry_msgs::msg::Pose> geom_points;
  for (const auto & point : points) {
    geom_points.push_back(tier4_autoware_utils::getPose(point));
  }
  return geom_points;
}

template <typename T>
TrajectoryPoint convertToTrajectoryPoint(const T & point)
{
  TrajectoryPoint traj_point;
  traj_point.pose = tier4_autoware_utils::getPose(point);
  traj_point.longitudinal_velocity_mps = point.longitudinal_velocity_mps;
  traj_point.lateral_velocity_mps = point.lateral_velocity_mps;
  traj_point.heading_rate_rps = point.heading_rate_rps;
  return traj_point;
}

template <>
inline TrajectoryPoint convertToTrajectoryPoint(const ReferencePoint & point)
{
  TrajectoryPoint traj_point;
  traj_point.pose = tier4_autoware_utils::getPose(point);
  return traj_point;
}

// functions to convert to another type of points
template <typename T>
std::vector<TrajectoryPoint> convertToTrajectoryPoints(const std::vector<T> & points)
{
  std::vector<TrajectoryPoint> traj_points;
  for (const auto & point : points) {
    const auto traj_point = convertToTrajectoryPoint(point);
    traj_points.push_back(traj_point);
  }
  return traj_points;
}

ReferencePoint convertToReferencePoint(const TrajectoryPoint & traj_point);
std::vector<ReferencePoint> convertToReferencePoints(
  const std::vector<TrajectoryPoint> & traj_points);

/*
template <typename T>
ReferencePoint convertToReferencePoint(const T & point);

template <typename T>
std::vector<ReferencePoint> convertToReferencePoints(const std::vector<T> & points)
{
  std::vector<ReferencePoint> ref_points;
  for (const auto & point : points) {
    const auto ref_point = convertToReferencePoint(point);
    ref_points.push_back(ref_point);
  }
  return ref_points;
}
*/

std::vector<geometry_msgs::msg::Pose> convertToPosesWithYawEstimation(
  const std::vector<geometry_msgs::msg::Point> points);

void compensateLastPose(
  const PathPoint & last_path_point, std::vector<TrajectoryPoint> & traj_points,
  const double delta_dist_threshold, const double delta_yaw_threshold);

template <typename T>
std::vector<double> calcCurvature(const T & points, const size_t num_sampling_points)
{
  std::vector<double> res(points.size());
  const size_t num_points = static_cast<int>(points.size());

  /* calculate curvature by circle fitting from three points */
  geometry_msgs::msg::Point p1, p2, p3;
  size_t max_smoothing_num = static_cast<size_t>(std::floor(0.5 * (num_points - 1)));
  size_t L = std::min(num_sampling_points, max_smoothing_num);
  for (size_t i = L; i < num_points - L; ++i) {
    p1 = tier4_autoware_utils::getPoint(points.at(i - L));
    p2 = tier4_autoware_utils::getPoint(points.at(i));
    p3 = tier4_autoware_utils::getPoint(points.at(i + L));
    double den = std::max(
      tier4_autoware_utils::calcDistance2d(p1, p2) * tier4_autoware_utils::calcDistance2d(p2, p3) *
        tier4_autoware_utils::calcDistance2d(p3, p1),
      0.0001);
    const double curvature =
      2.0 * ((p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x)) / den;
    res.at(i) = curvature;
  }

  /* first and last curvature is copied from next value */
  for (size_t i = 0; i < std::min(L, num_points); ++i) {
    res.at(i) = res.at(std::min(L, num_points - 1));
    res.at(num_points - i - 1) =
      res.at(std::max(static_cast<int>(num_points) - static_cast<int>(L) - 1, 0));
  }
  return res;
}

geometry_msgs::msg::Point getNearestPosition(
  const std::vector<ReferencePoint> & points, const int target_idx, const double offset);

template <typename T>
bool isNearLastPathPoint(
  const T & ref_point, const std::vector<PathPoint> & path_points,
  const double delta_dist_threshold, const double delta_yaw_threshold)
{
  const geometry_msgs::msg::Pose last_ref_pose = tier4_autoware_utils::getPose(ref_point);

  if (
    tier4_autoware_utils::calcDistance2d(last_ref_pose, path_points.back().pose) >
    delta_dist_threshold) {
    return false;
  }
  if (
    std::fabs(tier4_autoware_utils::calcYawDeviation(last_ref_pose, path_points.back().pose)) >
    delta_yaw_threshold) {
    return false;
  }
  return true;
}

template <class T>
size_t findEgoIndex(
  const std::vector<T> & points, const geometry_msgs::msg::Pose & ego_pose,
  const EgoNearestParam & ego_nearest_param)
{
  return motion_utils::findFirstNearestIndexWithSoftConstraints(
    points, ego_pose, ego_nearest_param.dist_threshold, ego_nearest_param.yaw_threshold);
}

template <class T>
size_t findEgoSegmentIndex(
  const std::vector<T> & points, const geometry_msgs::msg::Pose & ego_pose,
  const EgoNearestParam & ego_nearest_param)
{
  return motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
    points, ego_pose, ego_nearest_param.dist_threshold, ego_nearest_param.yaw_threshold);
}

template <class T>
size_t findSoftNearestIndex(
  const std::vector<T> & points, const geometry_msgs::msg::Pose & pose, const double dist_threshold,
  const double yaw_threshold)
{
  const auto nearest_idx_opt =
    motion_utils::findNearestIndex(points, pose, dist_threshold, yaw_threshold);
  return nearest_idx_opt ? nearest_idx_opt.get()
                         : motion_utils::findNearestIndex(points, pose.position);
}

template <class T>
size_t findSoftNearestSegmentIndex(
  const std::vector<T> & points, const geometry_msgs::msg::Pose & pose, const double dist_threshold,
  const double yaw_threshold)
{
  const auto nearest_idx_opt =
    motion_utils::findNearestSegmentIndex(points, pose, dist_threshold, yaw_threshold);
  return nearest_idx_opt ? nearest_idx_opt.get()
                         : motion_utils::findNearestSegmentIndex(points, pose.position);
}

Trajectory createTrajectory(
  const std_msgs::msg::Header & header, const std::vector<TrajectoryPoint> & traj_points);

std::vector<TrajectoryPoint> resampleTrajectoryPoints(
  const std::vector<TrajectoryPoint> traj_points, const double interval);
}  // namespace trajectory_utils
}  // namespace collision_free_path_planner
#endif  // COLLISION_FREE_PATH_PLANNER__UTILS__TRAJECTORY_UTILS_HPP_

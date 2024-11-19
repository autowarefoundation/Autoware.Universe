// Copyright 2024 TIER IV, Inc.
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

#include "autoware/trajectory/pose.hpp"

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Vector3.h>

#include <vector>

namespace autoware::trajectory
{

using PointType = geometry_msgs::msg::Pose;

bool Trajectory<PointType>::build(const std::vector<PointType> & points)
{
  std::vector<geometry_msgs::msg::Point> path_points;
  std::vector<geometry_msgs::msg::Quaternion> orientations;
  path_points.reserve(points.size());
  orientations.reserve(points.size());
  for (const auto & point : points) {
    path_points.emplace_back(point.position);
    orientations.emplace_back(point.orientation);
  }

  bool is_valid = true;
  is_valid &= BaseClass::build(path_points);
  is_valid &= orientation_interpolator_->build(bases_, orientations);
  return is_valid;
}

PointType Trajectory<PointType>::compute(double s) const
{
  PointType result;
  result.position = BaseClass::compute(s);
  s = clamp(s);
  result.orientation = orientation_interpolator_->compute(s);
  return result;
}

void Trajectory<PointType>::align_orientation_with_trajectory_direction()
{
  std::vector<geometry_msgs::msg::Quaternion> aligned_orientations;
  for (const auto & s : bases_) {
    double azimuth = this->azimuth(s);
    double elevation = this->elevation(s);
    geometry_msgs::msg::Quaternion current_orientation = orientation_interpolator_->compute(s);
    tf2::Quaternion current_orientation_tf2(
      current_orientation.x, current_orientation.y, current_orientation.z, current_orientation.w);
    current_orientation_tf2.normalize();
    tf2::Vector3 x_axis(1.0, 0.0, 0.0);
    tf2::Vector3 current_x_axis = tf2::quatRotate(current_orientation_tf2, x_axis);

    tf2::Vector3 desired_x_axis(
      std::cos(elevation) * std::cos(azimuth), std::cos(elevation) * std::sin(azimuth),
      std::sin(elevation));
    tf2::Vector3 rotation_axis = current_x_axis.cross(desired_x_axis).normalized();
    double dot_product = current_x_axis.dot(desired_x_axis);
    double rotation_angle = std::acos(dot_product);

    tf2::Quaternion delta_q(rotation_axis, rotation_angle);
    tf2::Quaternion aligned_orientation_tf2 = (delta_q * current_orientation_tf2).normalized();

    geometry_msgs::msg::Quaternion aligned_orientation;
    aligned_orientation.x = aligned_orientation_tf2.x();
    aligned_orientation.y = aligned_orientation_tf2.y();
    aligned_orientation.z = aligned_orientation_tf2.z();
    aligned_orientation.w = aligned_orientation_tf2.w();

    aligned_orientations.emplace_back(aligned_orientation);
  }
  orientation_interpolator_->build(bases_, aligned_orientations);
}

std::vector<PointType> Trajectory<PointType>::restore(const size_t & min_points) const
{
  auto bases = detail::crop_bases(bases_, start_, end_);
  bases = detail::fill_bases(bases, min_points);
  std::vector<PointType> points;
  points.reserve(bases.size());
  for (const auto & s : bases) {
    PointType p;
    p.position = BaseClass::compute(s);
    p.orientation = orientation_interpolator_->compute(s);
    points.emplace_back(p);
  }
  return points;
}

}  // namespace autoware::trajectory

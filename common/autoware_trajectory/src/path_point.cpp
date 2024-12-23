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

#include "autoware/trajectory/path_point.hpp"

#include "autoware/trajectory/detail/interpolated_array.hpp"
#include "autoware/trajectory/detail/utils.hpp"
#include "autoware/trajectory/forward.hpp"
#include "autoware/trajectory/interpolator/stairstep.hpp"
#include "autoware/trajectory/pose.hpp"

#include <autoware_planning_msgs/msg/path_point.hpp>

#include <vector>

namespace autoware::trajectory
{

using PointType = autoware_planning_msgs::msg::PathPoint;

Trajectory<PointType>::Trajectory()
: longitudinal_velocity_mps(
    detail::InterpolatedArray<double>(std::make_shared<interpolator::Stairstep<double>>())),
  lateral_velocity_mps(
    detail::InterpolatedArray<double>(std::make_shared<interpolator::Stairstep<double>>())),
  heading_rate_rps(
    detail::InterpolatedArray<double>(std::make_shared<interpolator::Stairstep<double>>()))
{
}

Trajectory<PointType>::Trajectory(const Trajectory & rhs)
: Trajectory<geometry_msgs::msg::Pose>(rhs)
{
  longitudinal_velocity_mps = rhs.longitudinal_velocity_mps;
  lateral_velocity_mps = rhs.lateral_velocity_mps;
  heading_rate_rps = rhs.heading_rate_rps;
}

Trajectory<PointType> & Trajectory<PointType>::operator=(const Trajectory & rhs)
{
  if (this != &rhs) {
    BaseClass::operator=(rhs);
    longitudinal_velocity_mps = rhs.longitudinal_velocity_mps;
    lateral_velocity_mps = rhs.lateral_velocity_mps;
    heading_rate_rps = rhs.heading_rate_rps;
  }
  return *this;
}

bool Trajectory<PointType>::build(const std::vector<PointType> & points)
{
  std::vector<geometry_msgs::msg::Pose> poses;
  std::vector<double> longitudinal_velocity_mps_values;
  std::vector<double> lateral_velocity_mps_values;
  std::vector<double> heading_rate_rps_values;

  for (const auto & point : points) {
    poses.emplace_back(point.pose);
    longitudinal_velocity_mps_values.emplace_back(point.longitudinal_velocity_mps);
    lateral_velocity_mps_values.emplace_back(point.lateral_velocity_mps);
    heading_rate_rps_values.emplace_back(point.heading_rate_rps);
  }

  bool is_valid = true;

  is_valid &= Trajectory<geometry_msgs::msg::Pose>::build(poses);
  is_valid &= this->longitudinal_velocity_mps.build(bases_, longitudinal_velocity_mps_values);
  is_valid &= this->lateral_velocity_mps.build(bases_, lateral_velocity_mps_values);
  is_valid &= this->heading_rate_rps.build(bases_, heading_rate_rps_values);

  return is_valid;
}

std::vector<double> Trajectory<PointType>::get_internal_bases() const
{
  auto get_bases = [](const auto & interpolated_array) {
    auto [bases, values] = interpolated_array.get_data();
    return bases;
  };

  auto bases = detail::merge_vectors(
    bases_, get_bases(this->longitudinal_velocity_mps), get_bases(this->lateral_velocity_mps),
    get_bases(this->heading_rate_rps));

  bases = detail::crop_bases(bases, start_, end_);
  std::transform(
    bases.begin(), bases.end(), bases.begin(), [this](const double & s) { return s - start_; });
  return bases;
}

PointType Trajectory<PointType>::compute(double s) const
{
  PointType result;
  result.pose = Trajectory<geometry_msgs::msg::Pose>::compute(s);
  s = clamp(s);
  result.longitudinal_velocity_mps = static_cast<float>(this->longitudinal_velocity_mps.compute(s));
  result.lateral_velocity_mps = static_cast<float>(this->lateral_velocity_mps.compute(s));
  result.heading_rate_rps = static_cast<float>(this->heading_rate_rps.compute(s));
  return result;
}

std::vector<PointType> Trajectory<PointType>::restore(const size_t & min_points) const
{
  std::vector<double> bases = get_internal_bases();
  bases = detail::fill_bases(bases, min_points);

  std::vector<PointType> points;
  points.reserve(bases.size());
  for (const auto & s : bases) {
    points.emplace_back(compute(s));
  }
  return points;
}

}  // namespace autoware::trajectory

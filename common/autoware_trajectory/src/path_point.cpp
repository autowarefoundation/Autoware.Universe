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

#include "autoware/trajectory/detail/utils.hpp"

#include <autoware_planning_msgs/msg/path_point.hpp>

#include <vector>

namespace autoware::trajectory
{

using PointType = autoware_planning_msgs::msg::PathPoint;

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
  auto get_bases = [](const auto & interpolated_array) {
    auto [bases, values] = interpolated_array.get_data();
    return bases;
  };

  auto bases = detail::merge_vectors(
    bases_, get_bases(this->longitudinal_velocity_mps), get_bases(this->lateral_velocity_mps),
    get_bases(this->heading_rate_rps));

  bases = detail::crop_bases(bases, start_, end_);
  bases = detail::fill_bases(bases, min_points);

  std::vector<PointType> points;
  points.reserve(bases.size());
  for (const auto & s : bases) {
    PointType p;
    p.pose = Trajectory<geometry_msgs::msg::Pose>::compute(s - start_);
    p.longitudinal_velocity_mps = static_cast<float>(this->longitudinal_velocity_mps.compute(s));
    p.lateral_velocity_mps = static_cast<float>(this->lateral_velocity_mps.compute(s));
    p.heading_rate_rps = static_cast<float>(this->heading_rate_rps.compute(s));
    points.emplace_back(p);
  }

  return points;
}

}  // namespace autoware::trajectory

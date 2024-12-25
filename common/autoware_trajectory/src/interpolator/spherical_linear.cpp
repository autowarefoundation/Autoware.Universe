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

#include "autoware/trajectory/interpolator/spherical_linear.hpp"

#include <Eigen/Geometry>

#include <vector>

namespace autoware::trajectory::interpolator
{

void SphericalLinear::build_impl(
  const std::vector<double> & bases,
  const std::vector<geometry_msgs::msg::Quaternion> & quaternions)
{
  this->bases_ = bases;
  this->quaternions_ = quaternions;
}

geometry_msgs::msg::Quaternion SphericalLinear::compute_impl(const double & s) const
{
  const int32_t idx = this->get_index(s);
  const double x0 = this->bases_.at(idx);
  const double x1 = this->bases_.at(idx + 1);
  const geometry_msgs::msg::Quaternion y0 = this->quaternions_.at(idx);
  const geometry_msgs::msg::Quaternion y1 = this->quaternions_.at(idx + 1);

  // Spherical linear interpolation (Slerp)
  const double t = (s - x0) / (x1 - x0);

  // Convert quaternions to Eigen vectors for calculation
  Eigen::Quaterniond q0(y0.w, y0.x, y0.y, y0.z);
  Eigen::Quaterniond q1(y1.w, y1.x, y1.y, y1.z);

  // Perform Slerp
  Eigen::Quaterniond q_slerp = q0.slerp(t, q1);

  // Convert the result back to geometry_msgs::msg::Quaternion
  geometry_msgs::msg::Quaternion result;
  result.w = q_slerp.w();
  result.x = q_slerp.x();
  result.y = q_slerp.y();
  result.z = q_slerp.z();

  return result;
}

size_t SphericalLinear::minimum_required_points() const
{
  return 2;
}
}  // namespace autoware::trajectory::interpolator

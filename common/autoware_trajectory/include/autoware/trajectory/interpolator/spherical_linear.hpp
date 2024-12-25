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

#ifndef AUTOWARE__TRAJECTORY__INTERPOLATOR__SPHERICAL_LINEAR_HPP_
#define AUTOWARE__TRAJECTORY__INTERPOLATOR__SPHERICAL_LINEAR_HPP_

#include "autoware/trajectory/interpolator/detail/interpolator_mixin.hpp"

#include <geometry_msgs/msg/quaternion.hpp>

#include <vector>

namespace autoware::trajectory::interpolator
{

/**
 * @brief Class for SphericalLinear interpolation.
 *
 * This class provides methods to perform SphericalLinear interpolation on a set of data points.
 */
class SphericalLinear
: public detail::InterpolatorMixin<SphericalLinear, geometry_msgs::msg::Quaternion>
{
private:
  std::vector<geometry_msgs::msg::Quaternion> quaternions_;

  /**
   * @brief Build the interpolator with the given values.
   *
   * @param bases The bases values.
   * @param values The values to interpolate.
   * @return True if the interpolator was built successfully, false otherwise.
   */
  void build_impl(
    const std::vector<double> & bases,
    const std::vector<geometry_msgs::msg::Quaternion> & quaternions) override;

  /**
   * @brief Compute the interpolated value at the given point.
   *
   * @param s The point at which to compute the interpolated value.
   * @return The interpolated value.
   */
  [[nodiscard]] geometry_msgs::msg::Quaternion compute_impl(const double & s) const override;

public:
  /**
   * @brief Default constructor.
   */
  SphericalLinear() = default;

  /**
   * @brief Get the minimum number of required points for the interpolator.
   *
   * @return The minimum number of required points.
   */
  [[nodiscard]] size_t minimum_required_points() const override;
};

}  // namespace autoware::trajectory::interpolator

#endif  // AUTOWARE__TRAJECTORY__INTERPOLATOR__SPHERICAL_LINEAR_HPP_

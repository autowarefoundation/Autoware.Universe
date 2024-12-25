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

#ifndef AUTOWARE__TRAJECTORY__INTERPOLATOR__CUBIC_SPLINE_HPP_
#define AUTOWARE__TRAJECTORY__INTERPOLATOR__CUBIC_SPLINE_HPP_

#include "autoware/trajectory/interpolator/detail/interpolator_mixin.hpp"

#include <Eigen/Dense>

#include <vector>

namespace autoware::trajectory::interpolator
{

/**
 * @brief Class for cubic spline interpolation.
 *
 * This class provides methods to perform cubic spline interpolation on a set of data points.
 */
class CubicSpline : public detail::InterpolatorMixin<CubicSpline, double>
{
private:
  Eigen::VectorXd a_, b_, c_, d_;  ///< Coefficients for the cubic spline.
  Eigen::VectorXd h_;              ///< Interval sizes between bases points.

  /**
   * @brief Compute the spline parameters.
   *
   * This method computes the coefficients for the cubic spline.
   *
   * @param bases The bases values.
   * @param values The values to interpolate.
   */
  void compute_parameters(
    const Eigen::Ref<const Eigen::VectorXd> & bases,
    const Eigen::Ref<const Eigen::VectorXd> & values);

  /**
   * @brief Build the interpolator with the given values.
   *
   * @param bases The bases values.
   * @param values The values to interpolate.
   * @return True if the interpolator was built successfully, false otherwise.
   */
  void build_impl(const std::vector<double> & bases, const std::vector<double> & values) override;

  /**
   * @brief Compute the interpolated value at the given point.
   *
   * @param s The point at which to compute the interpolated value.
   * @return The interpolated value.
   */
  [[nodiscard]] double compute_impl(const double & s) const override;

  /**
   * @brief Compute the first derivative at the given point.
   *
   * @param s The point at which to compute the first derivative.
   * @return The first derivative.
   */
  [[nodiscard]] double compute_first_derivative_impl(const double & s) const override;

  /**
   * @brief Compute the second derivative at the given point.
   *
   * @param s The point at which to compute the second derivative.
   * @return The second derivative.
   */
  [[nodiscard]] double compute_second_derivative_impl(const double & s) const override;

public:
  CubicSpline() = default;

  /**
   * @brief Get the minimum number of required points for the interpolator.
   *
   * @return The minimum number of required points.
   */
  [[nodiscard]] size_t minimum_required_points() const override { return 4; }
};

}  // namespace autoware::trajectory::interpolator

#endif  // AUTOWARE__TRAJECTORY__INTERPOLATOR__CUBIC_SPLINE_HPP_

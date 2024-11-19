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

#ifndef AUTOWARE__TRAJECTORY__INTERPOLATOR__DETAIL__INTERPOLATOR_COMMON_INTERFACE_HPP_
#define AUTOWARE__TRAJECTORY__INTERPOLATOR__DETAIL__INTERPOLATOR_COMMON_INTERFACE_HPP_

#include <Eigen/Dense>
#include <rclcpp/logging.hpp>

#include <vector>

namespace autoware::trajectory::interpolator::detail
{
/**
 * @brief Base class for interpolation implementations.
 *
 * This class provides the basic interface and common functionality for different types
 * of interpolation. It is intended to be subclassed by specific interpolation algorithms.
 *
 * @tparam T The type of the values being interpolated.
 */
template <typename T>
class InterpolatorCommonInterface
{
protected:
  std::vector<double> bases_;  ///< bases values for the interpolation.

  /**
   * @brief Get the start of the interpolation range.
   */
  [[nodiscard]] double start() const { return bases_.front(); }

  /**
   * @brief Get the end of the interpolation range.
   */
  [[nodiscard]] double end() const { return bases_.back(); }

  /**
   * @brief Compute the interpolated value at the given point.
   *
   * This method should be overridden by subclasses to provide the specific interpolation logic.
   *
   * @param s The point at which to compute the interpolated value.
   * @return The interpolated value.
   */
  [[nodiscard]] virtual T compute_impl(const double & s) const = 0;

  /**
   * @brief Build the interpolator with the given values.
   *
   * This method should be overridden by subclasses to provide the specific build logic.
   *
   * @param bases The bases values.
   * @param values The values to interpolate.
   */
  virtual void build_impl(const std::vector<double> & bases, const std::vector<T> & values) = 0;

  /**
   * @brief Validate the input to the compute method.
   *
   * Checks that the interpolator has been built and that the input value is within range.
   *
   * @param s The input value.
   * @return The input value, clamped to the range of the interpolator.
   */
  [[nodiscard]] double validate_compute_input(const double & s) const
  {
    if (s < start() || s > end()) {
      RCLCPP_WARN(
        rclcpp::get_logger("Interpolator"),
        "Input value %f is outside the range of the interpolator [%f, %f].", s, start(), end());
    }
    return std::clamp(s, start(), end());
  }

  /**
   * @brief Get the index of the interval containing the input value.
   *
   * This method determines the index of the interval in the bases array that contains the given
   * value. It assumes that the bases array is sorted in ascending order.
   *
   * If `end_inclusive` is true and the input value matches the end of the bases array,
   * the method returns the index of the second-to-last interval.
   *
   * @param s The input value for which to find the interval index.
   * @param end_inclusive Whether to include the end value in the last interval. Defaults to true.
   * @return The index of the interval containing the input value.
   *
   * @throw std::out_of_range if the input value is outside the range of the bases array.
   */
  [[nodiscard]] int32_t get_index(const double & s, bool end_inclusive = true) const
  {
    if (end_inclusive && s == end()) {
      return static_cast<int32_t>(bases_.size()) - 2;
    }
    auto comp = [](const double & a, const double & b) { return a <= b; };
    return std::distance(bases_.begin(), std::lower_bound(bases_.begin(), bases_.end(), s, comp)) -
           1;
  }

public:
  InterpolatorCommonInterface() = default;
  virtual ~InterpolatorCommonInterface() = default;
  InterpolatorCommonInterface(const InterpolatorCommonInterface & other) = default;
  InterpolatorCommonInterface & operator=(const InterpolatorCommonInterface & other) = default;
  InterpolatorCommonInterface(InterpolatorCommonInterface && other) noexcept = default;
  InterpolatorCommonInterface & operator=(InterpolatorCommonInterface && other) noexcept = default;

  /**
   * @brief Build the interpolator with the given bases and values.
   *
   * @param bases The bases values.
   * @param values The values to interpolate.
   * @return True if the interpolator was built successfully, false otherwise.
   */
  bool build(const std::vector<double> & bases, const std::vector<T> & values)
  {
    if (bases.size() != values.size()) {
      return false;
    }
    if (bases.size() < minimum_required_points()) {
      return false;
    }
    build_impl(bases, values);
    return true;
  }

  /**
   * @brief Get the minimum number of required points for the interpolator.
   *
   * This method should be overridden by subclasses to return the specific requirement.
   *
   * @return The minimum number of required points.
   */
  [[nodiscard]] virtual size_t minimum_required_points() const = 0;

  /**
   * @brief Compute the interpolated value at the given point.
   *
   * @param s The point at which to compute the interpolated value.
   * @return The interpolated value.
   */
  [[nodiscard]] T compute(const double & s) const
  {
    double clamped_s = validate_compute_input(s);
    return compute_impl(clamped_s);
  }
};
}  // namespace autoware::trajectory::interpolator::detail

#endif  // AUTOWARE__TRAJECTORY__INTERPOLATOR__DETAIL__INTERPOLATOR_COMMON_INTERFACE_HPP_

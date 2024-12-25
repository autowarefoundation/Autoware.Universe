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

// clang-format off
#ifndef AUTOWARE__TRAJECTORY__DETAIL__INTERPOLATED_ARRAY_HPP_  // NOLINT
#define AUTOWARE__TRAJECTORY__DETAIL__INTERPOLATED_ARRAY_HPP_  // NOLINT
// clang-format on

#include "autoware/trajectory/interpolator/interpolator.hpp"

#include <Eigen/Core>
#include <rclcpp/logging.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace autoware::trajectory::detail
{

/**
 * @brief Class representing an array with interpolatable values that can be manipulated.
 * @tparam T The type of values stored in the array.
 */
template <typename T>
class InterpolatedArray
{
  using InterpolatorType = interpolator::InterpolatorInterface<T>;

private:
  std::vector<double> bases_;
  std::vector<T> values_;
  std::shared_ptr<interpolator::InterpolatorInterface<T>> interpolator_;

public:
  /**
   * @brief Construct a InterpolatedArray with a given interpolator.
   * @param interpolator Shared pointer to the interpolator.
   */
  explicit InterpolatedArray(const std::shared_ptr<InterpolatorType> & interpolator)
  : interpolator_(interpolator)
  {
  }

  /**
   * @brief Copy constructor.
   * @param other The InterpolatedArray to copy from.
   */
  InterpolatedArray(const InterpolatedArray & other)
  : bases_(other.bases_), values_(other.values_), interpolator_(other.interpolator_->clone())
  {
  }

  InterpolatedArray(InterpolatedArray && other) = default;

  bool build(const std::vector<double> & bases, const std::vector<T> & values)
  {
    bases_ = bases;
    values_ = values;
    return interpolator_->build(bases_, values_);
  }

  /**
   * @brief Move constructor.
   * @param other The InterpolatedArray to move from.
   */
  InterpolatedArray & operator=(InterpolatedArray && other) = default;

  /**
   * @brief Copy assignment operator.
   * @param other The InterpolatedArray to copy from.
   * @return Reference to this InterpolatedArray.
   */
  InterpolatedArray & operator=(const InterpolatedArray & other)
  {
    bases_ = other.bases_;
    values_ = other.values_;
    interpolator_ = other.interpolator_->clone();
    return *this;
  }

  // Destructor
  ~InterpolatedArray() = default;

  /**
   * @brief Get the start value of the base.
   * @return The start value.
   */
  [[nodiscard]] double start() const { return bases_.front(); }

  /**
   * @brief Get the end value of the base.
   * @return The end value.
   */
  [[nodiscard]] double end() const { return bases_.at(bases_.size() - 1); }

  class Segment
  {
    friend class InterpolatedArray;

    double start_;
    double end_;
    InterpolatedArray<T> & parent_;
    Segment(InterpolatedArray<T> & parent, double start, double end)
    : start_(start), end_(end), parent_(parent)
    {
    }

  public:
    void set(const T & value)
    {
      std::vector<double> & bases = parent_.bases_;
      std::vector<T> & values = parent_.values_;

      auto insert_if_not_present = [&](double val) -> size_t {
        auto it = std::lower_bound(bases.begin(), bases.end(), val);
        size_t index = std::distance(bases.begin(), it);

        if (it != bases.end() && *it == val) {
          // Return the index if the value already exists
          return index;
        }  // Insert into bases
        bases.insert(it, val);
        // Insert into values at the corresponding position
        values.insert(values.begin() + index, value);
        return index;
      };

      // Insert the start value if not present
      size_t start_index = insert_if_not_present(start_);

      // Insert the end value if not present
      size_t end_index = insert_if_not_present(end_);

      // Ensure the indices are in ascending order
      if (start_index > end_index) {
        std::swap(start_index, end_index);
      }

      // Set the values in the specified range
      std::fill(values.begin() + start_index, values.begin() + end_index + 1, value);

      parent_.interpolator_->build(bases, values);

      // return *this;
    }
  };

  /**
   * @brief Get a Segment object to set values in a specific range.
   * @param start Start of the range.
   * @param end End of the range.
   * @return RangeSetter object.
   */
  Segment range(double start, double end)
  {
    if (start < this->start() || end > this->end()) {
      RCLCPP_WARN(
        rclcpp::get_logger("InterpolatedArray"),
        "The range [%f, %f] is out of the array range [%f, %f]", start, end, this->start(),
        this->end());
      start = std::max(start, this->start());
      end = std::min(end, this->end());
    }
    return Segment{*this, start, end};
  }

  /**
   * @brief Assign a value to the entire array.
   * @param value Value to be assigned.
   * @return Reference to the InterpolatedArray object.
   */
  InterpolatedArray & operator=(const T & value)
  {
    std::fill(values_.begin(), values_.end(), value);
    interpolator_->build(bases_, values_);
    return *this;
  }

  /**
   * @brief Compute the interpolated value at a given position.
   * @param x The position to compute the value at.
   * @return The interpolated value.
   */
  [[nodiscard]] T compute(const double & x) const { return interpolator_->compute(x); }

  /**
   * @brief Get the underlying data of the array.
   * @return A pair containing the axis and values.
   */
  [[nodiscard]] std::pair<std::vector<double>, std::vector<T>> get_data() const
  {
    return {bases_, values_};
  }
};

}  // namespace autoware::trajectory::detail

// clang-format off
#endif  // AUTOWARE__TRAJECTORY__DETAIL__INTERPOLATED_ARRAY_HPP_  // NOLINT
// clang-format on

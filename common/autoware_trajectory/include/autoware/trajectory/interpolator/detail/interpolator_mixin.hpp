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

#ifndef AUTOWARE__TRAJECTORY__INTERPOLATOR__DETAIL__INTERPOLATOR_MIXIN_HPP_
#define AUTOWARE__TRAJECTORY__INTERPOLATOR__DETAIL__INTERPOLATOR_MIXIN_HPP_

#include "autoware/trajectory/interpolator/interpolator.hpp"

#include <Eigen/Dense>

#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace autoware::trajectory::interpolator::detail
{

/**
 * @brief Base class for interpolator implementations.
 *
 * This class implements the core functionality for interpolator implementations.
 *
 * @tparam InterpolatorType The type of the interpolator implementation.
 * @tparam T The type of the values being interpolated.
 */
template <class InterpolatorType, class T>
struct InterpolatorMixin : public InterpolatorInterface<T>
{
  [[nodiscard]] std::shared_ptr<InterpolatorInterface<T>> clone() const override
  {
    return std::make_shared<InterpolatorType>(static_cast<const InterpolatorType &>(*this));
  }

  class Builder
  {
  private:
    std::vector<double> bases_;
    std::vector<T> values_;

  public:
    [[nodiscard]] Builder & set_bases(const Eigen::Ref<const Eigen::VectorXd> & bases)
    {
      bases_ = std::vector<double>(bases.begin(), bases.end());
      return *this;
    }

    [[nodiscard]] Builder & set_bases(const std::vector<double> & bases)
    {
      bases_ = bases;
      return *this;
    }

    [[nodiscard]] Builder & set_values(const std::vector<T> & values)
    {
      values_ = values;
      return *this;
    }

    template <typename... Args>
    [[nodiscard]] std::optional<InterpolatorType> build(Args &&... args)
    {
      auto interpolator = InterpolatorType(std::forward<Args>(args)...);
      bool success = interpolator.build(bases_, values_);
      if (!success) {
        return std::nullopt;
      }
      return interpolator;
    }
  };
};

}  // namespace autoware::trajectory::interpolator::detail

#endif  // AUTOWARE__TRAJECTORY__INTERPOLATOR__DETAIL__INTERPOLATOR_MIXIN_HPP_

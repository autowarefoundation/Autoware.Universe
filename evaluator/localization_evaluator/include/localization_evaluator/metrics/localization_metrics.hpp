// Copyright 2021 Tier IV, Inc.
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

#ifndef LOCALIZATION_EVALUATOR__METRICS__LOCALIZATION_METRICS_HPP_
#define LOCALIZATION_EVALUATOR__METRICS__LOCALIZATION_METRICS_HPP_

#include "autoware/universe_utils/math/accumulator.hpp"

#include <nav_msgs/msg/odometry.hpp>

namespace localization_diagnostics
{
using autoware::universe_utils::Accumulator;
namespace metrics
{
/**
 * @brief calculate lateral localization error
 * @param [in] stat_prev statistics to update
 * @param [in] lateral_pos lateral position of the vehicle
 * @param [in] lateral_ref reference lateral position
 * @return calculated statistics
 */
Accumulator<double> updateLateralStats(
  const Accumulator<double> stat_prev, const double & lateral_pos, const double & lateral_ref);

/**
 * @brief calculate absolute localization error
 * @param [in] stat_prev statistics to update
 * @param [in] pos current position of the vehicle
 * @param [in] pos_ref reference position of the vehicle
 * @return calculated statistics
 */
Accumulator<double> updateAbsoluteStats(
  const Accumulator<double> stat_prev, const geometry_msgs::msg::Point & pos,
  const geometry_msgs::msg::Point & pos_ref);

}  // namespace metrics
}  // namespace localization_diagnostics

#endif  // LOCALIZATION_EVALUATOR__METRICS__LOCALIZATION_METRICS_HPP_

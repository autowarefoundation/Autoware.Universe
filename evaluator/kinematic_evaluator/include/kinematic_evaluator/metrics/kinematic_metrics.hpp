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

#ifndef KINEMATIC_EVALUATOR__METRICS__KINEMATIC_METRICS_HPP_
#define KINEMATIC_EVALUATOR__METRICS__KINEMATIC_METRICS_HPP_

#include "autoware/universe_utils/math/accumulator.hpp"

#include <nav_msgs/msg/odometry.hpp>

namespace kinematic_diagnostics
{
namespace metrics
{
using autoware::universe_utils::Accumulator;
using nav_msgs::msg::Odometry;

/**
 * @brief calculate velocity deviation of the given trajectory from the reference trajectory
 * @param [in] value new value
 * @param [in] stat_prev input trajectory
 * @return calculated statistics
 */
Accumulator<double> updateVelocityStats(const double & value, const Accumulator<double> stat_prev);

}  // namespace metrics
}  // namespace kinematic_diagnostics

#endif  // KINEMATIC_EVALUATOR__METRICS__KINEMATIC_METRICS_HPP_

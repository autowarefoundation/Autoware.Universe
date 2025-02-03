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

#ifndef AUTOWARE__MTR__CONVERSIONS__HISTORY_HPP_
#define AUTOWARE__MTR__CONVERSIONS__HISTORY_HPP_

#include "autoware/mtr/agent.hpp"

#include <cmath>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace autoware::mtr
{

/**
 * @brief Transform a 2D point with a reference point and rotation
 *
 * @param history history to modify.
 * @param reference_state reference state used as reference frame.
 * @return  vector of histories, each in the reference frame of the last state.
 */
[[nodiscard]] std::pair<float, float> transform2D(
  const std::pair<float, float> input_xy, const std::pair<float, float> reference_xy,
  const std::pair<float, float> rotation_cos_sin);

/**
 * @brief Get histories with agent states in the reference frame of the last state
 *
 * @param history history to modify.
 * @param reference_state reference state used as reference frame.
 * @return  vector of histories, each in the reference frame of the last state.
 */
[[nodiscard]] AgentHistory getAgentCentricHistory(
  const AgentHistory & history, const AgentState & reference_state);

/**
 * @brief Get histories with agent states in the reference frame of the last state
 *
 * @param histories vector of histories to modify.
 * @return  vector of histories, each in the reference frame of the last state.
 */
[[nodiscard]] std::vector<AgentHistory> getAgentCentricHistories(
  const std::vector<AgentHistory> & histories);

}  // namespace autoware::mtr

#endif  // AUTOWARE__MTR__CONVERSIONS__HISTORY_HPP_

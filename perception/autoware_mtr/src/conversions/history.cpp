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

#include "autoware/mtr/conversions/history.hpp"

#include <utility>
#include <vector>

namespace autoware::mtr
{
using adl = autoware::mtr::AgentDimLabels;

/**
 * @brief Transform a 2D point with a reference point and rotation
 *
 * @param history history to modify.
 * @param reference_state reference state used as reference frame.
 * @return  vector of histories, each in the reference frame of the last state.
 */
[[nodiscard]] std::pair<float, float> transform2D(
  const std::pair<float, float> input_xy, const std::pair<float, float> reference_xy,
  const std::pair<float, float> rotation_cos_sin)
{
  auto [x, y] = input_xy;
  auto [ref_x, ref_y] = reference_xy;
  auto [cos, sin] = rotation_cos_sin;
  x -= ref_x;
  y -= ref_y;
  return {x * cos - y * sin, x * sin + y * cos};
}

/**
 * @brief Get histories with agent states in the reference frame of the last state
 *
 * @param history history to modify.
 * @param reference_state reference state used as reference frame.
 * @return  vector of histories, each in the reference frame of the last state.
 */
[[nodiscard]] AgentHistory getAgentCentricHistory(
  const AgentHistory & history, const AgentState & reference_state)
{
  const auto latest_valid_state = history.get_latest_valid_state();
  const auto & reference_state_dim = autoware::mtr::AgentState::dim();
  const auto & history_state_dim = autoware::mtr::AgentHistory::state_dim();

  if (!latest_valid_state.has_value() || reference_state_dim != history_state_dim) {
    return history;
  }
  AgentHistory agent_centric_history = history;
  const auto reference_array = reference_state.as_array();
  auto agent_centric_states_array = agent_centric_history.as_array();
  const auto ref_x = reference_array.at(adl::X);
  const auto ref_y = reference_array.at(adl::Y);
  const auto ref_z = reference_array.at(adl::Z);
  const auto ref_yaw = reference_array.at(adl::YAW);
  const auto cos = std::cos(-ref_yaw);
  const auto sin = std::sin(-ref_yaw);

  const auto & d = reference_state_dim;
  // Use i as the index for each new state
  for (size_t i = 0; i < agent_centric_states_array.size(); i += d) {
    auto & x = agent_centric_states_array.at(i + adl::X);
    auto & y = agent_centric_states_array.at(i + adl::Y);
    auto & vx = agent_centric_states_array.at(i + adl::VX);
    auto & vy = agent_centric_states_array.at(i + adl::VY);
    auto & ax = agent_centric_states_array.at(i + adl::AX);
    auto & ay = agent_centric_states_array.at(i + adl::AY);
    auto & z = agent_centric_states_array.at(i + adl::Z);
    auto & yaw = agent_centric_states_array.at(i + adl::YAW);

    std::tie(x, y) = transform2D({x, y}, {ref_x, ref_y}, {cos, sin});
    std::tie(vx, vy) = transform2D({vx, vy}, {0.0, 0.0}, {cos, sin});
    std::tie(ax, ay) = transform2D({x, y}, {0.0, 0.0}, {cos, sin});

    z -= ref_z;
    yaw -= ref_yaw;
  }
  return agent_centric_history;
};

/**
 * @brief Get histories with agent states in the reference frame of the last state
 *
 * @param histories vector of histories to modify.
 * @return  vector of histories, each in the reference frame of the last state.
 */
[[nodiscard]] std::vector<AgentHistory> getAgentCentricHistories(
  const std::vector<AgentHistory> & histories)
{
  std::vector<AgentHistory> agent_centric_histories;
  agent_centric_histories.reserve(histories.size());
  for (const auto & history : histories) {
    const auto & reference_state =
      history.get_latest_state();  // Todo(Daniel): should it be the latest VALID state?
    agent_centric_histories.push_back(getAgentCentricHistory(history, reference_state));
  }
  return agent_centric_histories;
};

}  // namespace autoware::mtr

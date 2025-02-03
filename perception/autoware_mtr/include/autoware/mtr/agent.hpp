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

#ifndef AUTOWARE__MTR__AGENT_HPP_
#define AUTOWARE__MTR__AGENT_HPP_

#include "autoware/mtr/fixed_queue.hpp"

#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/vector3.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
namespace autoware::mtr
{
constexpr size_t AgentStateDim = 12;

enum AgentLabel { VEHICLE = 0, PEDESTRIAN = 1, CYCLIST = 2 };

enum AgentDimLabels {
  X = 0,
  Y = 1,
  Z = 2,
  L = 3,
  W = 4,
  H = 5,
  YAW = 6,
  VX = 7,
  VY = 8,
  AX = 9,
  AY = 10,
  VALIDITY = 11
};

/**
 * @brief A class to represent a single state of an agent.
 */
struct AgentState
{
  // Construct a new instance filling all elements by `0.0f`.
  AgentState() {}

  /**
   * @brief Construct a new instance with specified values.
   *
   * @param position 3D position [m].
   * @param dimension Box dimension [m].
   * @param yaw Heading yaw angle [rad].
   * @param velocity Velocity [m/s].
   * @param acceleration Acceleration [m/s^2].
   * @param is_valid `1.0f` if valid, otherwise `0.0f`.
   */
  AgentState(
    const geometry_msgs::msg::Point & position, const geometry_msgs::msg::Vector3 & dimension,
    float yaw, const geometry_msgs::msg::Vector3 & velocity,
    const geometry_msgs::msg::Vector3 & acceleration, float is_valid)
  : position_(position),
    dimension_(dimension),
    yaw_(yaw),
    velocity_(velocity),
    acceleration_(acceleration),
    is_valid_(is_valid)
  {
  }

  // Construct a new instance filling all elements by `0.0f`.
  static AgentState empty() noexcept { return AgentState(); }

  // Return the agent state dimensions `D`.
  static size_t dim() { return AgentStateDim; }

  // Return the x position.
  float x() const { return position_.x; }

  // Return the y position.
  float y() const { return position_.y; }

  // Return the z position.
  float z() const { return position_.z; }

  // Return the length of object size.
  float length() const { return dimension_.x; }

  // Return the width of object size.
  float width() const { return dimension_.y; }

  // Return the height of object size.
  float height() const { return dimension_.z; }

  // Return the yaw angle `[rad]`.
  float yaw() const { return yaw_; }

  // Return the x velocity.
  float vx() const { return velocity_.x; }

  // Return the y velocity.
  float vy() const { return velocity_.y; }

  // Return the x acceleration.
  float ax() const { return acceleration_.x; }

  // Return the y acceleration.
  float ay() const { return acceleration_.y; }

  // Return `true` if the value is `1.0`.
  bool is_valid() const { return is_valid_ == 1.0; }

  // Return the state attribute as an array.
  std::array<float, AgentStateDim> as_array() const noexcept
  {
    return {x(), y(), z(), length(), width(), height(), yaw(), vx(), vy(), ax(), ay(), is_valid_};
  }

private:
  geometry_msgs::msg::Point position_;
  geometry_msgs::msg::Vector3 dimension_;
  float yaw_{0.0f};
  geometry_msgs::msg::Vector3 velocity_;
  geometry_msgs::msg::Vector3 acceleration_;
  float is_valid_{0.0f};
};

/**
 * @brief A class to represent the state history of an agent.
 */
struct AgentHistory
{
  /**
   * @brief Construct a new Agent History filling the latest state by input state.
   *
   * @param state Object current state.
   * @param object_id Object ID.
   * @param label_id Label ID.
   * @param current_time Current timestamp.
   * @param max_time_length History length.
   */
  AgentHistory(
    const AgentState & state, const std::string & object_id, const size_t label_id,
    const double current_time, const size_t max_time_length)
  : queue_(max_time_length),
    object_id_(object_id),
    label_id_(label_id),
    latest_time_(current_time),
    max_time_length_(max_time_length)
  {
    queue_.push_back(state);
  }

  // Return the history time length `T`.
  size_t length() const { return max_time_length_; }

  // Return the number of agent state dimensions `D`.
  static size_t state_dim() { return AgentStateDim; }

  // Return the data size of history `T * D`.
  size_t size() const { return max_time_length_ * state_dim(); }

  // Return the shape of history matrix ordering in `(T, D)`.
  std::tuple<size_t, size_t> shape() const { return {max_time_length_, state_dim()}; }

  // Return the object id.
  const std::string & object_id() const { return object_id_; }

  // Return the label id.
  size_t label_id() const { return label_id_; }

  /**
   * @brief Return the last timestamp when non-empty state was pushed.
   *
   * @return double
   */
  double latest_time() const { return latest_time_; }

  /**
   * @brief Update history with input state and latest time.
   *
   * @param current_time The current timestamp.
   * @param state The current agent state.
   */
  void update(double current_time, const AgentState & state) noexcept
  {
    queue_.push_back(state);
    latest_time_ = current_time;
  }

  // Update history with all-zeros state, but latest time is not updated.
  void update_empty() noexcept
  {
    const auto state = AgentState::empty();
    queue_.push_back(state);
  }

  // Return a history states as an array.
  std::vector<float> as_array() const noexcept
  {
    std::vector<float> output;
    for (const auto & state : queue_) {
      for (const auto & v : state.as_array()) {
        output.push_back(v);
      }
    }
    return output;
  }

  /**
   * @brief Check whether the latest valid state is too old or not.
   *
   * @param current_time Current timestamp.
   * @param threshold Time difference threshold value.
   * @return true If the difference is greater than threshold.
   * @return false Otherwise
   */
  bool is_ancient(double current_time, double threshold) const
  {
    /* TODO: Raise error if the current time is smaller than latest */
    return current_time - latest_time_ >= threshold;
  }

  /**
   * @brief Check whether the latest state data is valid.
   *
   * @return true If the end of element is 1.0f.
   * @return false Otherwise.
   */
  bool is_valid_latest() const { return get_latest_state().is_valid(); }

  // Get the latest agent state at `T`.
  const AgentState & get_latest_state() const { return queue_.back(); }

  // Get the latest agent state at `T`.
  std::optional<AgentState> get_latest_valid_state() const
  {
    auto latest_valid_state = std::find_if(
      queue_.rbegin(), queue_.rend(), [](const auto & state) { return state.is_valid(); });
    return (latest_valid_state != queue_.rend()) ? std::make_optional(*latest_valid_state)
                                                 : std::nullopt;
  }

private:
  FixedQueue<AgentState> queue_;
  const std::string object_id_;
  const size_t label_id_;
  double latest_time_;
  const size_t max_time_length_;
};

/**
 * @brief A class containing whole state histories of all agent.
 */
struct AgentData
{
  /**
   * @brief Construct a new instance.
   *
   * @param histories An array of histories for each object.
   * @param ego_index An index of ego.
   * @param target_indices Indices of target agents.
   * @param label_ids An array of label indices for each object.
   * @param timestamps An array of timestamps.
   */
  AgentData(
    const std::vector<AgentHistory> & histories, const size_t ego_index,
    const std::vector<size_t> & target_indices, const std::vector<size_t> & label_ids,
    const std::vector<float> & timestamps)
  : num_target_(target_indices.size()),
    num_agent_(histories.size()),
    time_length_(timestamps.size()),
    ego_index_(ego_index),
    target_indices_(target_indices),
    label_ids_(label_ids),
    timestamps_(timestamps)
  {
    data_.reserve(num_agent_ * time_length_ * state_dim());
    for (auto & history : histories) {
      for (const auto & v : history.as_array()) {
        data_.push_back(v);
      }
    }

    current_target_data_.reserve(num_target_ * state_dim());  // (B, D)
    target_label_ids_.reserve(num_target_);
    for (const auto & idx : target_indices) {
      target_label_ids_.emplace_back(label_ids.at(idx));
      for (const auto & v : histories.at(idx).get_latest_state().as_array()) {
        current_target_data_.push_back(v);
      }
    }

    ego_data_.reserve(time_length_ * state_dim());
    for (const auto & v : histories.at(ego_index).as_array()) {
      ego_data_.push_back(v);
    }
  }

  // Return the number of classes `C`.
  static size_t num_class() { return 3; }  // TODO(ktro2828): Do not use magic number.

  // Return the number of target agents `B`.
  size_t num_target() const { return num_target_; }

  // Return the number of agents `N`.
  size_t num_agent() const { return num_agent_; }

  // Return the timestamp length `T`.
  size_t time_length() const { return time_length_; }

  // Return the number of agent state dimensions `D`.
  static size_t state_dim() { return AgentStateDim; }

  // Return the index of ego.
  size_t ego_index() const { return ego_index_; }

  // Return the vector of indices of target agents, in shape `[B]`.
  const std::vector<size_t> & target_indices() const { return target_indices_; }

  // Return the vector of label ids of all agents, in shape `[N]`.
  const std::vector<size_t> & label_ids() const { return label_ids_; }

  // Return the vector of label ids of target agents, in shape `[B]`.
  const std::vector<size_t> & target_label_ids() const { return target_label_ids_; }

  // Return the vector of timestamps in shape `[T]`.
  const std::vector<float> & timestamps() const { return timestamps_; }

  // Return the number of all elements `N*T*D`.
  size_t size() const { return num_agent_ * time_length_ * state_dim(); }

  // Return the number of state dimensions of MTR input `T+C+D+3`.
  size_t input_dim() const { return time_length_ + state_dim() + num_class() + 3; }

  // Return the data shape ordering in (N, T, D).
  std::tuple<size_t, size_t, size_t> shape() const
  {
    return {num_agent_, time_length_, state_dim()};
  }

  // Return the address pointer of data array.
  const float * data_ptr() const noexcept { return data_.data(); }

  // Return the address pointer of data array for target agents.
  const float * current_target_data_ptr() const noexcept { return current_target_data_.data(); }

  // Return the address pointer of data array for ego vehicle.
  const float * ego_data_ptr() const noexcept { return ego_data_.data(); }

private:
  size_t num_target_;
  size_t num_agent_;
  size_t time_length_;
  size_t ego_index_;
  std::vector<size_t> target_indices_;
  std::vector<size_t> label_ids_;
  std::vector<size_t> target_label_ids_;
  std::vector<float> timestamps_;
  std::vector<float> data_;
  std::vector<float> current_target_data_;
  std::vector<float> ego_data_;
};

// Get label names from label indices.
inline std::vector<std::string> getLabelNames(const std::vector<size_t> & label_index)
{
  std::vector<std::string> label_names;
  label_names.reserve(label_index.size());
  for (const auto & idx : label_index) {
    switch (idx) {
      case 0:
        label_names.emplace_back("VEHICLE");
        break;
      case 1:
        label_names.emplace_back("PEDESTRIAN");
        break;
      case 2:
        label_names.emplace_back("CYCLIST");
        break;
      default:
        std::ostringstream msg;
        msg << "Error invalid label index: " << idx;
        throw std::runtime_error(msg.str());
        break;
    }
  }
  return label_names;
}

}  // namespace autoware::mtr
#endif  // AUTOWARE__MTR__AGENT_HPP_

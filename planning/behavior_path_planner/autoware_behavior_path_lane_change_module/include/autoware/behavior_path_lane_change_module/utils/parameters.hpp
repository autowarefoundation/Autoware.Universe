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

#ifndef AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__PARAMETERS_HPP_
#define AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__PARAMETERS_HPP_

#include "autoware/behavior_path_planner_common/utils/path_safety_checker/path_safety_checker_parameters.hpp"

#include <autoware/interpolation/linear_interpolation.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>

#include <utility>
#include <vector>

namespace autoware::behavior_path_planner::lane_change
{
using utils::path_safety_checker::ObjectTypesToCheck;
using utils::path_safety_checker::RSSparams;

struct LateralAccelerationMap
{
  std::vector<double> base_vel;
  std::vector<double> base_min_acc;
  std::vector<double> base_max_acc;

  void add(const double velocity, const double min_acc, const double max_acc)
  {
    if (base_vel.size() != base_min_acc.size() || base_vel.size() != base_max_acc.size()) {
      return;
    }

    size_t idx = 0;
    for (size_t i = 0; i < base_vel.size(); ++i) {
      if (velocity < base_vel.at(i)) {
        break;
      }
      idx = i + 1;
    }

    base_vel.insert(base_vel.begin() + idx, velocity);
    base_min_acc.insert(base_min_acc.begin() + idx, min_acc);
    base_max_acc.insert(base_max_acc.begin() + idx, max_acc);
  }

  std::pair<double, double> find(const double velocity) const
  {
    if (!base_vel.empty() && velocity < base_vel.front()) {
      return std::make_pair(base_min_acc.front(), base_max_acc.front());
    }
    if (!base_vel.empty() && velocity > base_vel.back()) {
      return std::make_pair(base_min_acc.back(), base_max_acc.back());
    }

    const double min_acc = autoware::interpolation::lerp(base_vel, base_min_acc, velocity);
    const double max_acc = autoware::interpolation::lerp(base_vel, base_max_acc, velocity);

    return std::make_pair(min_acc, max_acc);
  }
};

struct CancelParameters
{
  bool enable_on_prepare_phase{true};
  bool enable_on_lane_changing_phase{false};
  double delta_time{1.0};
  double duration{5.0};
  double max_lateral_jerk{10.0};
  double overhang_tolerance{0.0};

  // th_unsafe_hysteresis will be compare with the number of detected unsafe instance. If the
  // number of unsafe exceeds th_unsafe_hysteresis, the lane change will be cancelled or
  // aborted.
  int th_unsafe_hysteresis{2};

  int deceleration_sampling_num{5};
};

struct CollisionCheckParameters
{
  bool enable_for_prepare_phase_in_general_lanes{false};
  bool enable_for_prepare_phase_in_intersection{true};
  bool enable_for_prepare_phase_in_turns{true};
  bool check_current_lane{true};
  bool check_other_lanes{true};
  bool use_all_predicted_paths{false};
  double th_yaw_diff{3.1416};
  double prediction_time_resolution{0.5};
};

struct SafetyParameters
{
  bool enable_loose_check_for_cancel{true};
  bool enable_target_lane_bound_check{true};
  double th_stopped_object_velocity{0.1};
  double lane_expansion_left_offset{0.0};
  double lane_expansion_right_offset{0.0};
  RSSparams rss_params{};
  RSSparams rss_params_for_parked{};
  RSSparams rss_params_for_abort{};
  RSSparams rss_params_for_stuck{};
  ObjectTypesToCheck target_object_types{};
  CollisionCheckParameters collision_check{};
};

struct TrajectoryParameters
{
  double max_prepare_duration{4.0};
  double min_prepare_duration{1.0};
  double lateral_jerk{0.5};
  double min_longitudinal_acc{-1.0};
  double max_longitudinal_acc{1.0};
  double th_prepare_length_diff{0.5};
  double th_lane_changing_length_diff{0.5};
  double min_lane_changing_velocity{5.6};
  double lane_changing_decel_factor{0.5};
  int lon_acc_sampling_num{10};
  int lat_acc_sampling_num{10};
  LateralAccelerationMap lat_acc_map{};
};

struct DelayParameters
{
  bool enable{true};
  bool check_only_parked_vehicle{false};
  double min_road_shoulder_width{0.5};
  double th_parked_vehicle_shift_ratio{0.6};
};

struct Parameters
{
  TrajectoryParameters trajectory{};
  SafetyParameters safety{};
  CancelParameters cancel{};
  DelayParameters delay{};

  // lane change parameters
  double backward_lane_length{200.0};
  double backward_length_buffer_for_end_of_lane{0.0};
  double backward_length_buffer_for_blocking_object{0.0};
  double backward_length_from_intersection{5.0};

  // parked vehicle
  double object_check_min_road_shoulder_width{0.5};
  double th_object_shiftable_ratio{0.6};

  // turn signal
  double min_length_for_turn_signal_activation{10.0};

  // regulatory elements
  bool regulate_on_crosswalk{false};
  bool regulate_on_intersection{false};
  bool regulate_on_traffic_light{false};

  // ego vehicle stuck detection
  double th_stop_velocity{0.1};
  double th_stop_time{3.0};

  // finish judge parameter
  double lane_change_finish_judge_buffer{3.0};
  double th_finish_judge_lateral_diff{0.2};
  double th_finish_judge_yaw_diff{autoware::universe_utils::deg2rad(3.0)};

  // debug marker
  bool publish_debug_marker{false};
};

}  // namespace autoware::behavior_path_planner::lane_change

#endif  // AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__PARAMETERS_HPP_

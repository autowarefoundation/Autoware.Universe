// Copyright 2023 TIER IV, Inc.
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

#include "autoware/behavior_path_lane_change_module/manager.hpp"

#include "autoware/behavior_path_lane_change_module/interface.hpp"
#include "autoware/universe_utils/ros/parameter.hpp"
#include "autoware/universe_utils/ros/update_param.hpp"

#include <rclcpp/rclcpp.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace autoware::behavior_path_planner
{

void LaneChangeModuleManager::init(rclcpp::Node * node)
{
  // init manager interface
  initInterface(node, {""});
  initParams(node);
}

LCParamPtr LaneChangeModuleManager::set_params(rclcpp::Node * node, const std::string & node_name)
{
  using autoware::universe_utils::getOrDeclareParameter;

  LaneChangeParameters p{};

  const auto parameter = [](std::string && name) { return "lane_change." + name; };

  // trajectory generation
  {
    p.trajectory.max_prepare_duration =
      getOrDeclareParameter<double>(*node, parameter("trajectory.max_prepare_duration"));
    p.trajectory.min_prepare_duration =
      getOrDeclareParameter<double>(*node, parameter("trajectory.min_prepare_duration"));
    p.trajectory.lateral_jerk =
      getOrDeclareParameter<double>(*node, parameter("trajectory.lateral_jerk"));
    p.trajectory.min_longitudinal_acc =
      getOrDeclareParameter<double>(*node, parameter("trajectory.min_longitudinal_acc"));
    p.trajectory.max_longitudinal_acc =
      getOrDeclareParameter<double>(*node, parameter("trajectory.max_longitudinal_acc"));
    p.trajectory.th_prepare_length_diff =
      getOrDeclareParameter<double>(*node, parameter("trajectory.th_prepare_length_diff"));
    p.trajectory.th_lane_changing_length_diff =
      getOrDeclareParameter<double>(*node, parameter("trajectory.th_lane_changing_length_diff"));
    p.trajectory.min_lane_changing_velocity =
      getOrDeclareParameter<double>(*node, parameter("trajectory.min_lane_changing_velocity"));
    p.trajectory.lane_changing_decel_factor =
      getOrDeclareParameter<double>(*node, parameter("trajectory.lane_changing_decel_factor"));
    p.trajectory.th_prepare_curvature =
      getOrDeclareParameter<double>(*node, parameter("trajectory.th_prepare_curvature"));
    p.trajectory.lon_acc_sampling_num =
      getOrDeclareParameter<int>(*node, parameter("trajectory.lon_acc_sampling_num"));
    p.trajectory.lat_acc_sampling_num =
      getOrDeclareParameter<int>(*node, parameter("trajectory.lat_acc_sampling_num"));

    const auto max_acc = getOrDeclareParameter<double>(*node, "normal.max_acc");
    p.trajectory.min_lane_changing_velocity = std::min(
      p.trajectory.min_lane_changing_velocity, max_acc * p.trajectory.max_prepare_duration);

    // validation of trajectory parameters
    if (p.trajectory.lon_acc_sampling_num < 1 || p.trajectory.lat_acc_sampling_num < 1) {
      RCLCPP_FATAL_STREAM(
        node->get_logger().get_child(node_name),
        "lane_change_sampling_num must be positive integer. Given longitudinal parameter: "
          << p.trajectory.lon_acc_sampling_num
          << "Given lateral parameter: " << p.trajectory.lat_acc_sampling_num << std::endl
          << "Terminating the program...");
      exit(EXIT_FAILURE);
    }

    // lateral acceleration map
    const auto lateral_acc_velocity =
      getOrDeclareParameter<std::vector<double>>(*node, parameter("lateral_acceleration.velocity"));
    const auto min_lateral_acc = getOrDeclareParameter<std::vector<double>>(
      *node, parameter("lateral_acceleration.min_values"));
    const auto max_lateral_acc = getOrDeclareParameter<std::vector<double>>(
      *node, parameter("lateral_acceleration.max_values"));
    if (
      lateral_acc_velocity.size() != min_lateral_acc.size() ||
      lateral_acc_velocity.size() != max_lateral_acc.size()) {
      RCLCPP_ERROR(
        node->get_logger().get_child(node_name),
        "Lane change lateral acceleration map has invalid size.");
      exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < lateral_acc_velocity.size(); ++i) {
      p.trajectory.lat_acc_map.add(
        lateral_acc_velocity.at(i), min_lateral_acc.at(i), max_lateral_acc.at(i));
    }
  }

  // turn signal
  p.min_length_for_turn_signal_activation =
    getOrDeclareParameter<double>(*node, parameter("min_length_for_turn_signal_activation"));

  // lane change regulations
  p.regulate_on_crosswalk = getOrDeclareParameter<bool>(*node, parameter("regulation.crosswalk"));
  p.regulate_on_intersection =
    getOrDeclareParameter<bool>(*node, parameter("regulation.intersection"));
  p.regulate_on_traffic_light =
    getOrDeclareParameter<bool>(*node, parameter("regulation.traffic_light"));

  // ego vehicle stuck detection
  p.th_stop_velocity = getOrDeclareParameter<double>(*node, parameter("stuck_detection.velocity"));
  p.th_stop_time = getOrDeclareParameter<double>(*node, parameter("stuck_detection.stop_time"));

  // safety
  {
    p.safety.enable_loose_check_for_cancel =
      getOrDeclareParameter<bool>(*node, parameter("safety_check.allow_loose_check_for_cancel"));
    p.safety.enable_target_lane_bound_check =
      getOrDeclareParameter<bool>(*node, parameter("safety_check.enable_target_lane_bound_check"));
    p.safety.th_stopped_object_velocity = getOrDeclareParameter<double>(
      *node, parameter("safety_check.stopped_object_velocity_threshold"));
    p.safety.lane_expansion_left_offset =
      getOrDeclareParameter<double>(*node, parameter("safety_check.lane_expansion.left_offset"));
    p.safety.lane_expansion_right_offset =
      getOrDeclareParameter<double>(*node, parameter("safety_check.lane_expansion.right_offset"));

    // collision check
    p.safety.collision_check.enable_for_prepare_phase_in_general_lanes =
      getOrDeclareParameter<bool>(
        *node, parameter("collision_check.enable_for_prepare_phase.general_lanes"));
    p.safety.collision_check.enable_for_prepare_phase_in_intersection = getOrDeclareParameter<bool>(
      *node, parameter("collision_check.enable_for_prepare_phase.intersection"));
    p.safety.collision_check.enable_for_prepare_phase_in_turns = getOrDeclareParameter<bool>(
      *node, parameter("collision_check.enable_for_prepare_phase.turns"));
    p.safety.collision_check.check_current_lane =
      getOrDeclareParameter<bool>(*node, parameter("collision_check.check_current_lanes"));
    p.safety.collision_check.check_other_lanes =
      getOrDeclareParameter<bool>(*node, parameter("collision_check.check_other_lanes"));
    p.safety.collision_check.use_all_predicted_paths =
      getOrDeclareParameter<bool>(*node, parameter("collision_check.use_all_predicted_paths"));
    p.safety.collision_check.prediction_time_resolution =
      getOrDeclareParameter<double>(*node, parameter("collision_check.prediction_time_resolution"));
    p.safety.collision_check.th_yaw_diff =
      getOrDeclareParameter<double>(*node, parameter("collision_check.yaw_diff_threshold"));

    // rss check
    auto set_rss_params = [&](auto & params, const std::string & prefix) {
      params.longitudinal_distance_min_threshold = getOrDeclareParameter<double>(
        *node, parameter(prefix + ".longitudinal_distance_min_threshold"));
      params.longitudinal_velocity_delta_time = getOrDeclareParameter<double>(
        *node, parameter(prefix + ".longitudinal_velocity_delta_time"));
      params.front_vehicle_deceleration =
        getOrDeclareParameter<double>(*node, parameter(prefix + ".expected_front_deceleration"));
      params.rear_vehicle_deceleration =
        getOrDeclareParameter<double>(*node, parameter(prefix + ".expected_rear_deceleration"));
      params.rear_vehicle_reaction_time =
        getOrDeclareParameter<double>(*node, parameter(prefix + ".rear_vehicle_reaction_time"));
      params.rear_vehicle_safety_time_margin = getOrDeclareParameter<double>(
        *node, parameter(prefix + ".rear_vehicle_safety_time_margin"));
      params.lateral_distance_max_threshold =
        getOrDeclareParameter<double>(*node, parameter(prefix + ".lateral_distance_max_threshold"));
    };
    set_rss_params(p.safety.rss_params, "safety_check.execution");
    set_rss_params(p.safety.rss_params_for_parked, "safety_check.parked");
    set_rss_params(p.safety.rss_params_for_abort, "safety_check.cancel");
    set_rss_params(p.safety.rss_params_for_stuck, "safety_check.stuck");

    // target object types
    const std::string ns = "lane_change.target_object.";
    p.safety.target_object_types.check_car = getOrDeclareParameter<bool>(*node, ns + "car");
    p.safety.target_object_types.check_truck = getOrDeclareParameter<bool>(*node, ns + "truck");
    p.safety.target_object_types.check_bus = getOrDeclareParameter<bool>(*node, ns + "bus");
    p.safety.target_object_types.check_trailer = getOrDeclareParameter<bool>(*node, ns + "trailer");
    p.safety.target_object_types.check_unknown = getOrDeclareParameter<bool>(*node, ns + "unknown");
    p.safety.target_object_types.check_bicycle = getOrDeclareParameter<bool>(*node, ns + "bicycle");
    p.safety.target_object_types.check_motorcycle =
      getOrDeclareParameter<bool>(*node, ns + "motorcycle");
    p.safety.target_object_types.check_pedestrian =
      getOrDeclareParameter<bool>(*node, ns + "pedestrian");
  }

  // lane change parameters
  p.backward_lane_length = getOrDeclareParameter<double>(*node, parameter("backward_lane_length"));
  p.backward_length_buffer_for_end_of_lane =
    getOrDeclareParameter<double>(*node, parameter("backward_length_buffer_for_end_of_lane"));
  p.backward_length_buffer_for_blocking_object =
    getOrDeclareParameter<double>(*node, parameter("backward_length_buffer_for_blocking_object"));
  p.backward_length_from_intersection =
    getOrDeclareParameter<double>(*node, parameter("backward_length_from_intersection"));
  p.enable_stopped_vehicle_buffer =
    getOrDeclareParameter<bool>(*node, parameter("enable_stopped_vehicle_buffer"));

  if (p.backward_length_buffer_for_end_of_lane < 1.0) {
    RCLCPP_WARN_STREAM(
      node->get_logger().get_child(node_name),
      "Lane change buffer must be more than 1 meter. Modifying the buffer.");
  }

  // lane change delay
  p.delay.enable = getOrDeclareParameter<bool>(*node, parameter("delay_lane_change.enable"));
  p.delay.check_only_parked_vehicle =
    getOrDeclareParameter<bool>(*node, parameter("delay_lane_change.check_only_parked_vehicle"));
  p.delay.min_road_shoulder_width =
    getOrDeclareParameter<double>(*node, parameter("delay_lane_change.min_road_shoulder_width"));
  p.delay.th_parked_vehicle_shift_ratio = getOrDeclareParameter<double>(
    *node, parameter("delay_lane_change.th_parked_vehicle_shift_ratio"));

  // trajectory generation near terminal using frenet planner
  p.frenet.enable = getOrDeclareParameter<bool>(*node, parameter("frenet.enable"));
  p.frenet.th_yaw_diff_deg = getOrDeclareParameter<double>(*node, parameter("frenet.th_yaw_diff"));
  p.frenet.th_curvature_smoothing =
    getOrDeclareParameter<double>(*node, parameter("frenet.th_curvature_smoothing"));

  // lane change cancel
  p.cancel.enable_on_prepare_phase =
    getOrDeclareParameter<bool>(*node, parameter("cancel.enable_on_prepare_phase"));
  p.cancel.enable_on_lane_changing_phase =
    getOrDeclareParameter<bool>(*node, parameter("cancel.enable_on_lane_changing_phase"));
  p.cancel.delta_time = getOrDeclareParameter<double>(*node, parameter("cancel.delta_time"));
  p.cancel.duration = getOrDeclareParameter<double>(*node, parameter("cancel.duration"));
  p.cancel.max_lateral_jerk =
    getOrDeclareParameter<double>(*node, parameter("cancel.max_lateral_jerk"));
  p.cancel.overhang_tolerance =
    getOrDeclareParameter<double>(*node, parameter("cancel.overhang_tolerance"));
  p.cancel.th_unsafe_hysteresis =
    getOrDeclareParameter<int>(*node, parameter("cancel.unsafe_hysteresis_threshold"));
  p.cancel.deceleration_sampling_num =
    getOrDeclareParameter<int>(*node, parameter("cancel.deceleration_sampling_num"));

  // finish judge parameters
  p.lane_change_finish_judge_buffer =
    getOrDeclareParameter<double>(*node, parameter("lane_change_finish_judge_buffer"));
  p.th_finish_judge_lateral_diff =
    getOrDeclareParameter<double>(*node, parameter("finish_judge_lateral_threshold"));
  const auto finish_judge_lateral_angle_deviation =
    getOrDeclareParameter<double>(*node, parameter("finish_judge_lateral_angle_deviation"));
  p.th_finish_judge_yaw_diff =
    autoware::universe_utils::deg2rad(finish_judge_lateral_angle_deviation);

  // debug marker
  p.publish_debug_marker = getOrDeclareParameter<bool>(*node, parameter("publish_debug_marker"));

  // terminal lane change path
  p.terminal_path.enable = getOrDeclareParameter<bool>(*node, parameter("terminal_path.enable"));
  p.terminal_path.disable_near_goal =
    getOrDeclareParameter<bool>(*node, parameter("terminal_path.disable_near_goal"));
  p.terminal_path.stop_at_boundary =
    getOrDeclareParameter<bool>(*node, parameter("terminal_path.stop_at_boundary"));

  // validation of safety check parameters
  // if loose check is not enabled, lane change module will keep on chattering and canceling, and
  // false positive situation might  occur
  if (!p.safety.enable_loose_check_for_cancel) {
    if (
      p.safety.rss_params.front_vehicle_deceleration >
        p.safety.rss_params_for_abort.front_vehicle_deceleration ||
      p.safety.rss_params.rear_vehicle_deceleration >
        p.safety.rss_params_for_abort.rear_vehicle_deceleration ||
      p.safety.rss_params.rear_vehicle_reaction_time >
        p.safety.rss_params_for_abort.rear_vehicle_reaction_time ||
      p.safety.rss_params.rear_vehicle_safety_time_margin >
        p.safety.rss_params_for_abort.rear_vehicle_safety_time_margin ||
      p.safety.rss_params.lateral_distance_max_threshold >
        p.safety.rss_params_for_abort.lateral_distance_max_threshold ||
      p.safety.rss_params.longitudinal_distance_min_threshold >
        p.safety.rss_params_for_abort.longitudinal_distance_min_threshold ||
      p.safety.rss_params.longitudinal_velocity_delta_time >
        p.safety.rss_params_for_abort.longitudinal_velocity_delta_time) {
      RCLCPP_FATAL_STREAM(
        node->get_logger().get_child(node_name),
        "abort parameter might be loose... Terminating the program...");
      exit(EXIT_FAILURE);
    }
  }
  if (p.cancel.delta_time < 1.0) {
    RCLCPP_WARN_STREAM(
      node->get_logger().get_child(node_name),
      "cancel.delta_time: " << p.cancel.delta_time
                            << ", is too short. This could cause a danger behavior.");
  }

  return std::make_shared<lane_change::Parameters>(p);
}

void LaneChangeModuleManager::initParams(rclcpp::Node * node)
{
  parameters_ = set_params(node, name());
}

std::unique_ptr<SceneModuleInterface> LaneChangeModuleManager::createNewSceneModuleInstance()
{
  return std::make_unique<LaneChangeInterface>(
    name_, *node_, parameters_, rtc_interface_ptr_map_,
    objects_of_interest_marker_interface_ptr_map_,
    std::make_unique<NormalLaneChange>(parameters_, LaneChangeModuleType::NORMAL, direction_));
}

void LaneChangeModuleManager::updateModuleParams(const std::vector<rclcpp::Parameter> & parameters)
{
  using autoware::universe_utils::updateParam;

  auto p = parameters_;

  {
    const std::string ns = "lane_change.";
    updateParam<double>(parameters, ns + "backward_lane_length", p->backward_lane_length);
    updateParam<double>(
      parameters, ns + "backward_length_buffer_for_end_of_lane",
      p->backward_length_buffer_for_end_of_lane);
    updateParam<double>(
      parameters, ns + "backward_length_buffer_for_blocking_object",
      p->backward_length_buffer_for_blocking_object);
    updateParam<double>(
      parameters, ns + "lane_change_finish_judge_buffer", p->lane_change_finish_judge_buffer);
    updateParam<bool>(
      parameters, ns + "enable_stopped_vehicle_buffer", p->enable_stopped_vehicle_buffer);
    updateParam<double>(
      parameters, ns + "finish_judge_lateral_threshold", p->th_finish_judge_lateral_diff);
    updateParam<bool>(parameters, ns + "publish_debug_marker", p->publish_debug_marker);
    updateParam<double>(
      parameters, ns + "min_length_for_turn_signal_activation",
      p->min_length_for_turn_signal_activation);
  }

  {
    const std::string ns = "lane_change.trajectory.";
    updateParam<double>(
      parameters, ns + "max_prepare_duration", p->trajectory.max_prepare_duration);
    updateParam<double>(
      parameters, ns + "min_prepare_duration", p->trajectory.min_prepare_duration);
    updateParam<double>(parameters, ns + "lateral_jerk", p->trajectory.lateral_jerk);
    updateParam<double>(
      parameters, ns + "min_lane_changing_velocity", p->trajectory.min_lane_changing_velocity);
    updateParam<double>(
      parameters, ns + "min_longitudinal_acc", p->trajectory.min_longitudinal_acc);
    updateParam<double>(
      parameters, ns + "max_longitudinal_acc", p->trajectory.max_longitudinal_acc);
    updateParam<double>(
      parameters, ns + "lane_changing_decel_factor", p->trajectory.lane_changing_decel_factor);
    updateParam<double>(
      parameters, ns + "th_prepare_curvature", p->trajectory.th_prepare_curvature);
    int longitudinal_acc_sampling_num = 0;
    updateParam<int>(parameters, ns + "lon_acc_sampling_num", longitudinal_acc_sampling_num);
    if (longitudinal_acc_sampling_num > 0) {
      p->trajectory.lon_acc_sampling_num = longitudinal_acc_sampling_num;
    } else {
      RCLCPP_WARN_ONCE(
        node_->get_logger(),
        "Parameter 'lon_acc_sampling_num' is not updated because the value (%d) is not positive",
        longitudinal_acc_sampling_num);
    }

    int lateral_acc_sampling_num = 0;
    updateParam<int>(parameters, ns + "lat_acc_sampling_num", lateral_acc_sampling_num);
    if (lateral_acc_sampling_num > 0) {
      p->trajectory.lat_acc_sampling_num = lateral_acc_sampling_num;
    } else {
      RCLCPP_WARN_ONCE(
        node_->get_logger(),
        "Parameter 'lat_acc_sampling_num' is not updated because the value (%d) is not positive",
        lateral_acc_sampling_num);
    }

    updateParam<double>(
      parameters, ns + "th_prepare_length_diff", p->trajectory.th_prepare_length_diff);
    updateParam<double>(
      parameters, ns + "th_lane_changing_length_diff", p->trajectory.th_lane_changing_length_diff);
  }

  {
    const std::string ns = "lane_change.frenet.";
    updateParam<bool>(parameters, ns + "enable", p->frenet.enable);
    updateParam<double>(parameters, ns + "th_yaw_diff", p->frenet.th_yaw_diff_deg);
    updateParam<double>(
      parameters, ns + "th_curvature_smoothing", p->frenet.th_curvature_smoothing);
  }

  {
    const std::string ns = "lane_change.safety_check.lane_expansion.";
    updateParam<double>(parameters, ns + "left_offset", p->safety.lane_expansion_left_offset);
    updateParam<double>(parameters, ns + "right_offset", p->safety.lane_expansion_right_offset);
  }

  {
    const std::string ns = "lane_change.lateral_acceleration.";
    std::vector<double> velocity = p->trajectory.lat_acc_map.base_vel;
    std::vector<double> min_values = p->trajectory.lat_acc_map.base_min_acc;
    std::vector<double> max_values = p->trajectory.lat_acc_map.base_max_acc;

    updateParam<std::vector<double>>(parameters, ns + "velocity", velocity);
    updateParam<std::vector<double>>(parameters, ns + "min_values", min_values);
    updateParam<std::vector<double>>(parameters, ns + "max_values", max_values);
    if (
      velocity.size() >= 2 && velocity.size() == min_values.size() &&
      velocity.size() == max_values.size()) {
      LateralAccelerationMap lat_acc_map;
      for (size_t i = 0; i < velocity.size(); ++i) {
        lat_acc_map.add(velocity.at(i), min_values.at(i), max_values.at(i));
      }
      p->trajectory.lat_acc_map = lat_acc_map;
    } else {
      RCLCPP_WARN_ONCE(
        node_->get_logger(),
        "Mismatched size for lateral acceleration. Expected size: %lu, but velocity: %lu, "
        "min_values: %lu, max_values: %lu",
        std::max(2ul, velocity.size()), velocity.size(), min_values.size(), max_values.size());
    }
  }

  {
    const std::string ns = "lane_change.collision_check.";
    updateParam<bool>(
      parameters, ns + "enable_for_prepare_phase.general_lanes",
      p->safety.collision_check.enable_for_prepare_phase_in_general_lanes);
    updateParam<bool>(
      parameters, ns + "enable_for_prepare_phase.intersection",
      p->safety.collision_check.enable_for_prepare_phase_in_intersection);
    updateParam<bool>(
      parameters, ns + "enable_for_prepare_phase.turns",
      p->safety.collision_check.enable_for_prepare_phase_in_turns);
    updateParam<bool>(
      parameters, ns + "check_current_lanes", p->safety.collision_check.check_current_lane);
    updateParam<bool>(
      parameters, ns + "check_other_lanes", p->safety.collision_check.check_other_lanes);
    updateParam<bool>(
      parameters, ns + "use_all_predicted_paths",
      p->safety.collision_check.use_all_predicted_paths);
    updateParam<double>(
      parameters, ns + "prediction_time_resolution",
      p->safety.collision_check.prediction_time_resolution);
    updateParam<double>(
      parameters, ns + "yaw_diff_threshold", p->safety.collision_check.th_yaw_diff);
  }

  {
    const std::string ns = "lane_change.target_object.";
    updateParam<bool>(parameters, ns + "car", p->safety.target_object_types.check_car);
    updateParam<bool>(parameters, ns + "truck", p->safety.target_object_types.check_truck);
    updateParam<bool>(parameters, ns + "bus", p->safety.target_object_types.check_bus);
    updateParam<bool>(parameters, ns + "trailer", p->safety.target_object_types.check_trailer);
    updateParam<bool>(parameters, ns + "unknown", p->safety.target_object_types.check_unknown);
    updateParam<bool>(parameters, ns + "bicycle", p->safety.target_object_types.check_bicycle);
    updateParam<bool>(
      parameters, ns + "motorcycle", p->safety.target_object_types.check_motorcycle);
    updateParam<bool>(
      parameters, ns + "pedestrian", p->safety.target_object_types.check_pedestrian);
  }

  {
    const std::string ns = "lane_change.regulation.";
    updateParam<bool>(parameters, ns + "crosswalk", p->regulate_on_crosswalk);
    updateParam<bool>(parameters, ns + "intersection", p->regulate_on_intersection);
    updateParam<bool>(parameters, ns + "traffic_light", p->regulate_on_traffic_light);
  }

  {
    const std::string ns = "lane_change.stuck_detection.";
    updateParam<double>(parameters, ns + "velocity", p->th_stop_velocity);
    updateParam<double>(parameters, ns + "stop_time", p->th_stop_time);
  }

  auto update_rss_params = [&parameters](const std::string & prefix, auto & params) {
    using autoware::universe_utils::updateParam;
    updateParam<double>(
      parameters, prefix + "longitudinal_distance_min_threshold",
      params.longitudinal_distance_min_threshold);
    updateParam<double>(
      parameters, prefix + "longitudinal_velocity_delta_time",
      params.longitudinal_velocity_delta_time);
    updateParam<double>(
      parameters, prefix + "expected_front_deceleration", params.front_vehicle_deceleration);
    updateParam<double>(
      parameters, prefix + "expected_rear_deceleration", params.rear_vehicle_deceleration);
    updateParam<double>(
      parameters, prefix + "rear_vehicle_reaction_time", params.rear_vehicle_reaction_time);
    updateParam<double>(
      parameters, prefix + "rear_vehicle_safety_time_margin",
      params.rear_vehicle_safety_time_margin);
    updateParam<double>(
      parameters, prefix + "lateral_distance_max_threshold", params.lateral_distance_max_threshold);
  };

  update_rss_params("lane_change.safety_check.execution.", p->safety.rss_params);
  update_rss_params("lane_change.safety_check.parked.", p->safety.rss_params_for_parked);
  update_rss_params("lane_change.safety_check.cancel.", p->safety.rss_params_for_abort);
  update_rss_params("lane_change.safety_check.stuck.", p->safety.rss_params_for_stuck);

  {
    const std::string ns = "lane_change.delay_lane_change.";
    updateParam<bool>(parameters, ns + "enable", p->delay.enable);
    updateParam<bool>(
      parameters, ns + "check_only_parked_vehicle", p->delay.check_only_parked_vehicle);
    updateParam<double>(
      parameters, ns + "min_road_shoulder_width", p->delay.min_road_shoulder_width);
    updateParam<double>(
      parameters, ns + "th_parked_vehicle_shift_ratio", p->delay.th_parked_vehicle_shift_ratio);
  }

  {
    const std::string ns = "lane_change.terminal_path.";
    updateParam<bool>(parameters, ns + "enable", p->terminal_path.enable);
    updateParam<bool>(parameters, ns + "disable_near_goal", p->terminal_path.disable_near_goal);
    updateParam<bool>(parameters, ns + "stop_at_boundary", p->terminal_path.stop_at_boundary);
  }

  {
    const std::string ns = "lane_change.cancel.";
    bool enable_on_prepare_phase = true;
    updateParam<bool>(parameters, ns + "enable_on_prepare_phase", enable_on_prepare_phase);
    if (!enable_on_prepare_phase) {
      RCLCPP_WARN_ONCE(node_->get_logger(), "WARNING! Lane Change cancel function is disabled.");
      p->cancel.enable_on_prepare_phase = enable_on_prepare_phase;
    }

    bool enable_on_lane_changing_phase = true;
    updateParam<bool>(
      parameters, ns + "enable_on_lane_changing_phase", enable_on_lane_changing_phase);
    if (!enable_on_lane_changing_phase) {
      RCLCPP_WARN_ONCE(node_->get_logger(), "WARNING! Lane Change abort function is disabled.");
      p->cancel.enable_on_lane_changing_phase = enable_on_lane_changing_phase;
    }

    int deceleration_sampling_num = 0;
    updateParam<int>(parameters, ns + "deceleration_sampling_num", deceleration_sampling_num);
    if (deceleration_sampling_num > 0) {
      p->cancel.deceleration_sampling_num = deceleration_sampling_num;
    } else {
      RCLCPP_WARN_ONCE(
        node_->get_logger(),
        "Parameter 'deceleration_sampling_num' is not updated because the value (%d) is not "
        "positive",
        deceleration_sampling_num);
    }

    updateParam<double>(parameters, ns + "delta_time", p->cancel.delta_time);
    updateParam<double>(parameters, ns + "duration", p->cancel.duration);
    updateParam<double>(parameters, ns + "max_lateral_jerk", p->cancel.max_lateral_jerk);
    updateParam<double>(parameters, ns + "overhang_tolerance", p->cancel.overhang_tolerance);
    updateParam<int>(
      parameters, ns + "unsafe_hysteresis_threshold", p->cancel.th_unsafe_hysteresis);
  }

  std::for_each(observers_.begin(), observers_.end(), [&p](const auto & observer) {
    if (!observer.expired()) observer.lock()->updateModuleParams(p);
  });
}

}  // namespace autoware::behavior_path_planner

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(
  autoware::behavior_path_planner::LaneChangeRightModuleManager,
  autoware::behavior_path_planner::SceneModuleManagerInterface)
PLUGINLIB_EXPORT_CLASS(
  autoware::behavior_path_planner::LaneChangeLeftModuleManager,
  autoware::behavior_path_planner::SceneModuleManagerInterface)

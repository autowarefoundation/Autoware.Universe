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

#include "autoware/behavior_path_start_planner_module/data_structs.hpp"

#include "autoware/behavior_path_start_planner_module/manager.hpp"

#include <autoware/universe_utils/ros/parameter.hpp>
#include <rclcpp/rclcpp.hpp>

#include <string>

namespace autoware::behavior_path_planner
{

StartPlannerParameters StartPlannerParameters::init(rclcpp::Node & node)
{
  using autoware::universe_utils::getOrDeclareParameter;
  StartPlannerParameters p;
  {
    const std::string ns = "start_planner.";

    p.th_arrived_distance = getOrDeclareParameter<double>(node, ns + "th_arrived_distance");
    p.th_stopped_velocity = getOrDeclareParameter<double>(node, ns + "th_stopped_velocity");
    p.th_stopped_time = getOrDeclareParameter<double>(node, ns + "th_stopped_time");
    p.prepare_time_before_start =
      getOrDeclareParameter<double>(node, ns + "prepare_time_before_start");
    p.th_distance_to_middle_of_the_road =
      getOrDeclareParameter<double>(node, ns + "th_distance_to_middle_of_the_road");
    p.skip_rear_vehicle_check = getOrDeclareParameter<bool>(node, ns + "skip_rear_vehicle_check");
    p.extra_width_margin_for_rear_obstacle =
      getOrDeclareParameter<double>(node, ns + "extra_width_margin_for_rear_obstacle");
    p.collision_check_margins =
      getOrDeclareParameter<std::vector<double>>(node, ns + "collision_check_margins");
    p.collision_check_margin_from_front_object =
      getOrDeclareParameter<double>(node, ns + "collision_check_margin_from_front_object");
    p.th_moving_object_velocity =
      getOrDeclareParameter<double>(node, ns + "th_moving_object_velocity");
    p.center_line_path_interval =
      getOrDeclareParameter<double>(node, ns + "center_line_path_interval");
    // shift pull out
    p.enable_shift_pull_out = getOrDeclareParameter<bool>(node, ns + "enable_shift_pull_out");
    p.check_shift_path_lane_departure =
      getOrDeclareParameter<bool>(node, ns + "check_shift_path_lane_departure");
    p.allow_check_shift_path_lane_departure_override =
      getOrDeclareParameter<bool>(node, ns + "allow_check_shift_path_lane_departure_override");
    p.shift_collision_check_distance_from_end =
      getOrDeclareParameter<double>(node, ns + "shift_collision_check_distance_from_end");
    p.minimum_shift_pull_out_distance =
      getOrDeclareParameter<double>(node, ns + "minimum_shift_pull_out_distance");
    p.lateral_acceleration_sampling_num =
      getOrDeclareParameter<int>(node, ns + "lateral_acceleration_sampling_num");
    p.lateral_jerk = getOrDeclareParameter<double>(node, ns + "lateral_jerk");
    p.maximum_lateral_acc = getOrDeclareParameter<double>(node, ns + "maximum_lateral_acc");
    p.minimum_lateral_acc = getOrDeclareParameter<double>(node, ns + "minimum_lateral_acc");
    p.maximum_curvature = getOrDeclareParameter<double>(node, ns + "maximum_curvature");
    p.end_pose_curvature_threshold =
      getOrDeclareParameter<double>(node, ns + "end_pose_curvature_threshold");
    p.maximum_longitudinal_deviation =
      getOrDeclareParameter<double>(node, ns + "maximum_longitudinal_deviation");
    // geometric pull out
    p.enable_geometric_pull_out =
      getOrDeclareParameter<bool>(node, ns + "enable_geometric_pull_out");
    p.geometric_collision_check_distance_from_end =
      getOrDeclareParameter<double>(node, ns + "geometric_collision_check_distance_from_end");
    p.divide_pull_out_path = getOrDeclareParameter<bool>(node, ns + "divide_pull_out_path");
    p.parallel_parking_parameters.pull_out_velocity =
      getOrDeclareParameter<double>(node, ns + "geometric_pull_out_velocity");
    p.parallel_parking_parameters.pull_out_arc_path_interval =
      getOrDeclareParameter<double>(node, ns + "arc_path_interval");
    p.parallel_parking_parameters.pull_out_lane_departure_margin =
      getOrDeclareParameter<double>(node, ns + "lane_departure_margin");
    p.lane_departure_check_expansion_margin =
      getOrDeclareParameter<double>(node, ns + "lane_departure_check_expansion_margin");
    p.parallel_parking_parameters.pull_out_max_steer_angle =
      getOrDeclareParameter<double>(node, ns + "pull_out_max_steer_angle");  // 15deg
    p.parallel_parking_parameters.center_line_path_interval =
      p.center_line_path_interval;  // for geometric parallel parking
    // search start pose backward
    p.search_priority = getOrDeclareParameter<std::string>(
      node,
      ns + "search_priority");  // "efficient_path" or "short_back_distance"
    p.enable_back = getOrDeclareParameter<bool>(node, ns + "enable_back");
    p.backward_velocity = getOrDeclareParameter<double>(node, ns + "backward_velocity");
    p.max_back_distance = getOrDeclareParameter<double>(node, ns + "max_back_distance");
    p.backward_search_resolution =
      getOrDeclareParameter<double>(node, ns + "backward_search_resolution");
    p.backward_path_update_duration =
      getOrDeclareParameter<double>(node, ns + "backward_path_update_duration");
    p.ignore_distance_from_lane_end =
      getOrDeclareParameter<double>(node, ns + "ignore_distance_from_lane_end");
    // stop condition
    p.maximum_deceleration_for_stop =
      getOrDeclareParameter<double>(node, ns + "stop_condition.maximum_deceleration_for_stop");
    p.maximum_jerk_for_stop =
      getOrDeclareParameter<double>(node, ns + "stop_condition.maximum_jerk_for_stop");
  }
  {
    const std::string ns = "start_planner.object_types_to_check_for_path_generation.";
    p.object_types_to_check_for_path_generation.check_car =
      getOrDeclareParameter<bool>(node, ns + "check_car");
    p.object_types_to_check_for_path_generation.check_truck =
      getOrDeclareParameter<bool>(node, ns + "check_truck");
    p.object_types_to_check_for_path_generation.check_bus =
      getOrDeclareParameter<bool>(node, ns + "check_bus");
    p.object_types_to_check_for_path_generation.check_trailer =
      getOrDeclareParameter<bool>(node, ns + "check_trailer");
    p.object_types_to_check_for_path_generation.check_unknown =
      getOrDeclareParameter<bool>(node, ns + "check_unknown");
    p.object_types_to_check_for_path_generation.check_bicycle =
      getOrDeclareParameter<bool>(node, ns + "check_bicycle");
    p.object_types_to_check_for_path_generation.check_motorcycle =
      getOrDeclareParameter<bool>(node, ns + "check_motorcycle");
    p.object_types_to_check_for_path_generation.check_pedestrian =
      getOrDeclareParameter<bool>(node, ns + "check_pedestrian");
  }
  // freespace planner general params
  {
    const std::string ns = "start_planner.freespace_planner.";
    p.enable_freespace_planner = getOrDeclareParameter<bool>(node, ns + "enable_freespace_planner");
    p.freespace_planner_algorithm =
      getOrDeclareParameter<std::string>(node, ns + "freespace_planner_algorithm");
    p.end_pose_search_start_distance =
      getOrDeclareParameter<double>(node, ns + "end_pose_search_start_distance");
    p.end_pose_search_end_distance =
      getOrDeclareParameter<double>(node, ns + "end_pose_search_end_distance");
    p.end_pose_search_interval =
      getOrDeclareParameter<double>(node, ns + "end_pose_search_interval");
    p.freespace_planner_velocity = getOrDeclareParameter<double>(node, ns + "velocity");
    p.vehicle_shape_margin = getOrDeclareParameter<double>(node, ns + "vehicle_shape_margin");
    p.freespace_planner_common_parameters.time_limit =
      getOrDeclareParameter<double>(node, ns + "time_limit");
    p.freespace_planner_common_parameters.max_turning_ratio =
      getOrDeclareParameter<double>(node, ns + "max_turning_ratio");
    p.freespace_planner_common_parameters.turning_steps =
      getOrDeclareParameter<int>(node, ns + "turning_steps");
  }
  //  freespace planner search config
  {
    const std::string ns = "start_planner.freespace_planner.search_configs.";
    p.freespace_planner_common_parameters.theta_size =
      getOrDeclareParameter<int>(node, ns + "theta_size");
    p.freespace_planner_common_parameters.angle_goal_range =
      getOrDeclareParameter<double>(node, ns + "angle_goal_range");
    p.freespace_planner_common_parameters.curve_weight =
      getOrDeclareParameter<double>(node, ns + "curve_weight");
    p.freespace_planner_common_parameters.reverse_weight =
      getOrDeclareParameter<double>(node, ns + "reverse_weight");
    p.freespace_planner_common_parameters.lateral_goal_range =
      getOrDeclareParameter<double>(node, ns + "lateral_goal_range");
    p.freespace_planner_common_parameters.longitudinal_goal_range =
      getOrDeclareParameter<double>(node, ns + "longitudinal_goal_range");
  }
  //  freespace planner costmap configs
  {
    const std::string ns = "start_planner.freespace_planner.costmap_configs.";
    p.freespace_planner_common_parameters.obstacle_threshold =
      getOrDeclareParameter<int>(node, ns + "obstacle_threshold");
  }
  //  freespace planner astar
  {
    const std::string ns = "start_planner.freespace_planner.astar.";
    p.astar_parameters.search_method =
      getOrDeclareParameter<std::string>(node, ns + "search_method");
    p.astar_parameters.only_behind_solutions =
      getOrDeclareParameter<bool>(node, ns + "only_behind_solutions");
    p.astar_parameters.use_back = getOrDeclareParameter<bool>(node, ns + "use_back");
    p.astar_parameters.distance_heuristic_weight =
      getOrDeclareParameter<double>(node, ns + "distance_heuristic_weight");
  }
  //   freespace planner rrtstar
  {
    const std::string ns = "start_planner.freespace_planner.rrtstar.";
    p.rrt_star_parameters.enable_update = getOrDeclareParameter<bool>(node, ns + "enable_update");
    p.rrt_star_parameters.use_informed_sampling =
      getOrDeclareParameter<bool>(node, ns + "use_informed_sampling");
    p.rrt_star_parameters.max_planning_time =
      getOrDeclareParameter<double>(node, ns + "max_planning_time");
    p.rrt_star_parameters.neighbor_radius =
      getOrDeclareParameter<double>(node, ns + "neighbor_radius");
    p.rrt_star_parameters.margin = getOrDeclareParameter<double>(node, ns + "margin");
  }

  const std::string base_ns = "start_planner.path_safety_check.";
  // EgoPredictedPath
  {
    const std::string ego_path_ns = base_ns + "ego_predicted_path.";
    p.ego_predicted_path_params.min_velocity =
      getOrDeclareParameter<double>(node, ego_path_ns + "min_velocity");
    p.ego_predicted_path_params.acceleration =
      getOrDeclareParameter<double>(node, ego_path_ns + "min_acceleration");
    p.ego_predicted_path_params.time_horizon_for_front_object =
      getOrDeclareParameter<double>(node, ego_path_ns + "time_horizon_for_front_object");
    p.ego_predicted_path_params.time_horizon_for_rear_object =
      getOrDeclareParameter<double>(node, ego_path_ns + "time_horizon_for_rear_object");
    p.ego_predicted_path_params.time_resolution =
      getOrDeclareParameter<double>(node, ego_path_ns + "time_resolution");
    p.ego_predicted_path_params.delay_until_departure =
      getOrDeclareParameter<double>(node, ego_path_ns + "delay_until_departure");
  }
  // ObjectFilteringParams
  const std::string obj_filter_ns = base_ns + "target_filtering.";
  {
    p.objects_filtering_params.safety_check_time_horizon =
      getOrDeclareParameter<double>(node, obj_filter_ns + "safety_check_time_horizon");
    p.objects_filtering_params.safety_check_time_resolution =
      getOrDeclareParameter<double>(node, obj_filter_ns + "safety_check_time_resolution");
    p.objects_filtering_params.object_check_forward_distance =
      getOrDeclareParameter<double>(node, obj_filter_ns + "object_check_forward_distance");
    p.objects_filtering_params.object_check_backward_distance =
      getOrDeclareParameter<double>(node, obj_filter_ns + "object_check_backward_distance");
    p.objects_filtering_params.ignore_object_velocity_threshold =
      getOrDeclareParameter<double>(node, obj_filter_ns + "ignore_object_velocity_threshold");
    p.objects_filtering_params.include_opposite_lane =
      getOrDeclareParameter<bool>(node, obj_filter_ns + "include_opposite_lane");
    p.objects_filtering_params.invert_opposite_lane =
      getOrDeclareParameter<bool>(node, obj_filter_ns + "invert_opposite_lane");
    p.objects_filtering_params.check_all_predicted_path =
      getOrDeclareParameter<bool>(node, obj_filter_ns + "check_all_predicted_path");
    p.objects_filtering_params.use_all_predicted_path =
      getOrDeclareParameter<bool>(node, obj_filter_ns + "use_all_predicted_path");
    p.objects_filtering_params.use_predicted_path_outside_lanelet =
      getOrDeclareParameter<bool>(node, obj_filter_ns + "use_predicted_path_outside_lanelet");
  }
  // ObjectTypesToCheck
  {
    const std::string obj_types_ns = obj_filter_ns + "object_types_to_check.";
    p.objects_filtering_params.object_types_to_check.check_car =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_car");
    p.objects_filtering_params.object_types_to_check.check_truck =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_truck");
    p.objects_filtering_params.object_types_to_check.check_bus =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_bus");
    p.objects_filtering_params.object_types_to_check.check_trailer =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_trailer");
    p.objects_filtering_params.object_types_to_check.check_unknown =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_unknown");
    p.objects_filtering_params.object_types_to_check.check_bicycle =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_bicycle");
    p.objects_filtering_params.object_types_to_check.check_motorcycle =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_motorcycle");
    p.objects_filtering_params.object_types_to_check.check_pedestrian =
      getOrDeclareParameter<bool>(node, obj_types_ns + "check_pedestrian");
  }
  // ObjectLaneConfiguration
  {
    const std::string obj_lane_ns = obj_filter_ns + "object_lane_configuration.";
    p.objects_filtering_params.object_lane_configuration.check_current_lane =
      getOrDeclareParameter<bool>(node, obj_lane_ns + "check_current_lane");
    p.objects_filtering_params.object_lane_configuration.check_right_lane =
      getOrDeclareParameter<bool>(node, obj_lane_ns + "check_right_side_lane");
    p.objects_filtering_params.object_lane_configuration.check_left_lane =
      getOrDeclareParameter<bool>(node, obj_lane_ns + "check_left_side_lane");
    p.objects_filtering_params.object_lane_configuration.check_shoulder_lane =
      getOrDeclareParameter<bool>(node, obj_lane_ns + "check_shoulder_lane");
    p.objects_filtering_params.object_lane_configuration.check_other_lane =
      getOrDeclareParameter<bool>(node, obj_lane_ns + "check_other_lane");
  }
  // SafetyCheckParams
  const std::string safety_check_ns = base_ns + "safety_check_params.";
  {
    p.safety_check_params.enable_safety_check =
      getOrDeclareParameter<bool>(node, safety_check_ns + "enable_safety_check");
    p.safety_check_params.hysteresis_factor_expand_rate =
      getOrDeclareParameter<double>(node, safety_check_ns + "hysteresis_factor_expand_rate");
    p.safety_check_params.backward_path_length =
      getOrDeclareParameter<double>(node, safety_check_ns + "backward_path_length");
    p.safety_check_params.forward_path_length =
      getOrDeclareParameter<double>(node, safety_check_ns + "forward_path_length");
    p.safety_check_params.publish_debug_marker =
      getOrDeclareParameter<bool>(node, safety_check_ns + "publish_debug_marker");
    p.safety_check_params.collision_check_yaw_diff_threshold =
      getOrDeclareParameter<double>(node, safety_check_ns + "collision_check_yaw_diff_threshold");
  }
  // RSSparams
  {
    const std::string rss_ns = safety_check_ns + "rss_params.";
    p.safety_check_params.rss_params.rear_vehicle_reaction_time =
      getOrDeclareParameter<double>(node, rss_ns + "rear_vehicle_reaction_time");
    p.safety_check_params.rss_params.rear_vehicle_safety_time_margin =
      getOrDeclareParameter<double>(node, rss_ns + "rear_vehicle_safety_time_margin");
    p.safety_check_params.rss_params.lateral_distance_max_threshold =
      getOrDeclareParameter<double>(node, rss_ns + "lateral_distance_max_threshold");
    p.safety_check_params.rss_params.longitudinal_distance_min_threshold =
      getOrDeclareParameter<double>(node, rss_ns + "longitudinal_distance_min_threshold");
    p.safety_check_params.rss_params.longitudinal_velocity_delta_time =
      getOrDeclareParameter<double>(node, rss_ns + "longitudinal_velocity_delta_time");
    p.safety_check_params.rss_params.extended_polygon_policy =
      getOrDeclareParameter<std::string>(node, rss_ns + "extended_polygon_policy");
  }
  // surround moving obstacle check
  {
    const std::string surround_moving_obstacle_check_ns =
      "start_planner.surround_moving_obstacle_check.";
    p.search_radius =
      getOrDeclareParameter<double>(node, surround_moving_obstacle_check_ns + "search_radius");
    p.th_moving_obstacle_velocity = getOrDeclareParameter<double>(
      node, surround_moving_obstacle_check_ns + "th_moving_obstacle_velocity");
    // ObjectTypesToCheck
    {
      const std::string obj_types_ns = surround_moving_obstacle_check_ns + "object_types_to_check.";
      p.surround_moving_obstacles_type_to_check.check_car =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_car");
      p.surround_moving_obstacles_type_to_check.check_truck =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_truck");
      p.surround_moving_obstacles_type_to_check.check_bus =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_bus");
      p.surround_moving_obstacles_type_to_check.check_trailer =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_trailer");
      p.surround_moving_obstacles_type_to_check.check_unknown =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_unknown");
      p.surround_moving_obstacles_type_to_check.check_bicycle =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_bicycle");
      p.surround_moving_obstacles_type_to_check.check_motorcycle =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_motorcycle");
      p.surround_moving_obstacles_type_to_check.check_pedestrian =
        getOrDeclareParameter<bool>(node, obj_types_ns + "check_pedestrian");
    }
  }

  // debug
  {
    const std::string debug_ns = "start_planner.debug.";
    p.print_debug_info = getOrDeclareParameter<bool>(node, debug_ns + "print_debug_info");
  }

  return p;
}
}  // namespace autoware::behavior_path_planner

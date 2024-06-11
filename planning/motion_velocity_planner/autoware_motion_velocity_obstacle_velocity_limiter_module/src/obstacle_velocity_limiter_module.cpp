// Copyright 2022 TIER IV, Inc.
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

#include "obstacle_velocity_limiter_module.hpp"

#include "debug.hpp"
#include "forward_projection.hpp"
#include "map_utils.hpp"
#include "obstacle_velocity_limiter.hpp"
#include "parameters.hpp"
#include "trajectory_preprocessing.hpp"

#include <autoware_vehicle_info_utils/vehicle_info_utils.hpp>
#include <lanelet2_extension/utility/message_conversion.hpp>
#include <motion_utils/marker/virtual_wall_marker_creator.hpp>
#include <motion_utils/trajectory/trajectory.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/logging.hpp>
#include <tier4_autoware_utils/ros/update_param.hpp>

#include <boost/geometry.hpp>

#include <algorithm>
#include <chrono>

namespace autoware::motion_velocity_planner
{
void ObstacleVelocityLimiterModule::init(rclcpp::Node & node, const std::string & module_name)
{
  module_name_ = module_name;
  logger_ = node.get_logger();
  clock_ = node.get_clock();
  preprocessing_params_ = obstacle_velocity_limiter::PreprocessingParameters(node);
  projection_params_ = obstacle_velocity_limiter::ProjectionParameters(node);
  obstacle_params_ = obstacle_velocity_limiter::ObstacleParameters(node);
  velocity_params_ = obstacle_velocity_limiter::VelocityParameters(node);
  velocity_factor_interface_.init(motion_utils::PlanningBehavior::ROUTE_OBSTACLE);

  debug_publisher_ =
    node.create_publisher<visualization_msgs::msg::MarkerArray>("~/" + ns_ + "/debug_markers", 1);
  virtual_wall_publisher_ =
    node.create_publisher<visualization_msgs::msg::MarkerArray>("~/" + ns_ + "/virtual_walls", 1);

  const auto vehicle_info = vehicle_info_utils::VehicleInfoUtils(node).getVehicleInfo();
  vehicle_lateral_offset_ = static_cast<double>(vehicle_info.max_lateral_offset_m);
  vehicle_front_offset_ = static_cast<double>(vehicle_info.max_longitudinal_offset_m);
  distance_buffer_ = node.declare_parameter<double>("distance_buffer");

  projection_params_.wheel_base = vehicle_info.wheel_base_m;
  projection_params_.extra_length = vehicle_front_offset_ + distance_buffer_;
}

void ObstacleVelocityLimiterModule::update_parameters(
  const std::vector<rclcpp::Parameter> & parameters)
{
  using obstacle_velocity_limiter::ObstacleParameters;
  using obstacle_velocity_limiter::PreprocessingParameters;
  using obstacle_velocity_limiter::ProjectionParameters;
  using obstacle_velocity_limiter::VelocityParameters;
  for (const auto & parameter : parameters) {
    if (parameter.get_name() == "distance_buffer") {
      distance_buffer_ = static_cast<double>(parameter.as_double());
      projection_params_.extra_length = vehicle_front_offset_ + distance_buffer_;
      // Preprocessing parameters
    } else if (parameter.get_name() == PreprocessingParameters::START_DIST_PARAM) {
      preprocessing_params_.start_distance = static_cast<double>(parameter.as_double());
    } else if (parameter.get_name() == PreprocessingParameters::DOWNSAMPLING_PARAM) {
      preprocessing_params_.updateDownsampleFactor(parameter.as_int());
    } else if (parameter.get_name() == PreprocessingParameters::CALC_STEER_PARAM) {
      preprocessing_params_.calculate_steering_angles = parameter.as_bool();
    } else if (parameter.get_name() == PreprocessingParameters::MAX_LENGTH_PARAM) {
      preprocessing_params_.max_length = static_cast<double>(parameter.as_double());
    } else if (parameter.get_name() == PreprocessingParameters::MAX_DURATION_PARAM) {
      preprocessing_params_.max_duration = static_cast<double>(parameter.as_double());
      // Velocity parameters
    } else if (parameter.get_name() == VelocityParameters::MIN_VEL_PARAM) {
      velocity_params_.min_velocity = static_cast<double>(parameter.as_double());
    } else if (parameter.get_name() == VelocityParameters::MAX_DECEL_PARAM) {
      velocity_params_.max_deceleration = static_cast<double>(parameter.as_double());
      // Obstacle parameters
    } else if (parameter.get_name() == ProjectionParameters::DURATION_PARAM) {
      const auto min_ttc = static_cast<double>(parameter.as_double());
      if (min_ttc > 0.0) projection_params_.duration = min_ttc;
    } else if (parameter.get_name() == ObstacleParameters::DYN_SOURCE_PARAM) {
      obstacle_params_.updateType(logger_, parameter.as_string());
    } else if (parameter.get_name() == ObstacleParameters::OCC_GRID_THRESH_PARAM) {
      obstacle_params_.occupancy_grid_threshold = static_cast<int8_t>(parameter.as_int());
    } else if (parameter.get_name() == ObstacleParameters::BUFFER_PARAM) {
      obstacle_params_.dynamic_obstacles_buffer = static_cast<double>(parameter.as_double());
    } else if (parameter.get_name() == ObstacleParameters::MIN_VEL_PARAM) {
      obstacle_params_.dynamic_obstacles_min_vel = static_cast<double>(parameter.as_double());
    } else if (parameter.get_name() == ObstacleParameters::MAP_TAGS_PARAM) {
      // TODO(Maxime): implement in the node
      // obstacle_params_.static_map_tags = parameter.as_string_array();
      // if (lanelet_map_ptr_)
      //   static_map_obstacles_ =
      //     extractStaticObstacles(*lanelet_map_ptr_, obstacle_params_.static_map_tags);
    } else if (parameter.get_name() == ObstacleParameters::FILTERING_PARAM) {
      obstacle_params_.filter_envelope = parameter.as_bool();
    } else if (parameter.get_name() == ObstacleParameters::IGNORE_ON_PATH_PARAM) {
      obstacle_params_.ignore_on_path = parameter.as_bool();
    } else if (parameter.get_name() == ObstacleParameters::IGNORE_DIST_PARAM) {
      obstacle_params_.ignore_extra_distance = static_cast<double>(parameter.as_double());
    } else if (parameter.get_name() == ObstacleParameters::RTREE_POINTS_PARAM) {
      obstacle_params_.updateRtreeMinPoints(logger_, static_cast<int>(parameter.as_int()));
    } else if (parameter.get_name() == ObstacleParameters::RTREE_SEGMENTS_PARAM) {
      obstacle_params_.updateRtreeMinSegments(logger_, static_cast<int>(parameter.as_int()));
      // Projection parameters
    } else if (parameter.get_name() == ProjectionParameters::MODEL_PARAM) {
      projection_params_.updateModel(logger_, parameter.as_string());
    } else if (parameter.get_name() == ProjectionParameters::NB_POINTS_PARAM) {
      projection_params_.updateNbPoints(logger_, static_cast<int>(parameter.as_int()));
    } else if (parameter.get_name() == ProjectionParameters::STEER_OFFSET_PARAM) {
      projection_params_.steering_angle_offset = parameter.as_double();
    } else if (parameter.get_name() == ProjectionParameters::DISTANCE_METHOD_PARAM) {
      projection_params_.updateDistanceMethod(logger_, parameter.as_string());
    } else {
      RCLCPP_WARN(logger_, "Unknown parameter %s", parameter.get_name().c_str());
    }
  }
}

VelocityPlanningResult ObstacleVelocityLimiterModule::plan(
  const std::vector<autoware_planning_msgs::msg::TrajectoryPoint> & ego_trajectory_points,
  const std::shared_ptr<const PlannerData> planner_data)
{
  VelocityPlanningResult result;
  // if (!validInputs()) return result;
  // const auto t_start = std::chrono::system_clock::now();
  const auto ego_idx =
    motion_utils::findNearestIndex(ego_trajectory_points, planner_data->current_odometry.pose.pose);
  if (!ego_idx) {
    RCLCPP_WARN_THROTTLE(
      logger_, *clock_, rcutils_duration_value_t(1000),
      "Cannot calculate ego index on the trajectory");
    return result;
  }
  auto original_traj_points = ego_trajectory_points;
  // if (preprocessing_params_.calculate_steering_angles)
  //   calculateSteeringAngles(original_traj, projection_params_.wheel_base);
  // velocity_params_.current_ego_velocity = current_odometry_ptr_->twist.twist.linear.x;
  // const auto start_idx =
  //   calculateStartIndex(original_traj, *ego_idx, preprocessing_params_.start_distance);
  // const auto end_idx = calculateEndIndex(
  //   original_traj, start_idx, preprocessing_params_.max_length,
  //   preprocessing_params_.max_duration);
  // Trajectory downsampled_traj = downsampleTrajectory(
  //   original_traj, start_idx, end_idx, preprocessing_params_.downsample_factor);
  // ObstacleMasks obstacle_masks;
  // obstacle_masks.negative_masks = createPolygonMasks(
  //   *dynamic_obstacles_ptr_, obstacle_params_.dynamic_obstacles_buffer,
  //   obstacle_params_.dynamic_obstacles_min_vel);
  // if (obstacle_params_.ignore_on_path)
  //   obstacle_masks.negative_masks.push_back(createTrajectoryFootprint(
  //     *msg, vehicle_lateral_offset_ + obstacle_params_.ignore_extra_distance));
  // const auto projected_linestrings = createProjectedLines(downsampled_traj, projection_params_);
  // const auto footprint_polygons =
  //   createFootprintPolygons(projected_linestrings, vehicle_lateral_offset_);
  // Obstacles obstacles;
  // obstacles.lines = static_map_obstacles_;
  // if (obstacle_params_.dynamic_source != ObstacleParameters::STATIC_ONLY) {
  //   if (obstacle_params_.filter_envelope)
  //     obstacle_masks.positive_mask = createEnvelopePolygon(footprint_polygons);
  //   addSensorObstacles(
  //     obstacles, *occupancy_grid_ptr_, *pointcloud_ptr_, obstacle_masks, transform_listener_,
  //     original_traj.header.frame_id, obstacle_params_);
  // }
  // limitVelocity(
  //   downsampled_traj,
  //   CollisionChecker(
  //     obstacles, obstacle_params_.rtree_min_points, obstacle_params_.rtree_min_segments),
  //   projected_linestrings, footprint_polygons, projection_params_, velocity_params_);
  // auto safe_trajectory = copyDownsampledVelocity(
  //   downsampled_traj, original_traj, start_idx, preprocessing_params_.downsample_factor);

  // if (debug_publisher_->get_subscription_count() > 0) {
  //   const auto safe_projected_linestrings =
  //     createProjectedLines(downsampled_traj, projection_params_);
  //   const auto safe_footprint_polygons =
  //     createFootprintPolygons(safe_projected_linestrings, vehicle_lateral_offset_);
  //   debug_publisher_->publish(makeDebugMarkers(
  //     obstacles, projected_linestrings, safe_projected_linestrings, footprint_polygons,
  //     safe_footprint_polygons, obstacle_masks, occupancy_grid_ptr_->info.origin.position.z));
  // }
  return result;
}

// bool ObstacleVelocityLimiterModule::validInputs()
// {
//   constexpr auto one_sec = rcutils_duration_value_t(1000);
//   if (!occupancy_grid_ptr_)
//     RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), one_sec, "Occupancy grid not yet received");
//   if (!dynamic_obstacles_ptr_)
//     RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), one_sec, "Dynamic obstacles not yet
//     received");
//   if (!current_odometry_ptr_)
//     RCLCPP_WARN_THROTTLE(
//       get_logger(), *get_clock(), one_sec, "Current ego velocity not yet received");

//   return occupancy_grid_ptr_ && dynamic_obstacles_ptr_ && current_odometry_ptr_;
// }
}  // namespace autoware::motion_velocity_planner

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(
  autoware::motion_velocity_planner::ObstacleVelocityLimiterModule,
  autoware::motion_velocity_planner::PluginModuleInterface)

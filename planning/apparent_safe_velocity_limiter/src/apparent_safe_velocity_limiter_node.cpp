// Copyright 2022 Tier IV, Inc.
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

#include "apparent_safe_velocity_limiter/apparent_safe_velocity_limiter_node.hpp"

#include "apparent_safe_velocity_limiter/apparent_safe_velocity_limiter.hpp"
#include "apparent_safe_velocity_limiter/debug.hpp"
#include "apparent_safe_velocity_limiter/map_utils.hpp"
#include "apparent_safe_velocity_limiter/parameters.hpp"
#include "apparent_safe_velocity_limiter/types.hpp"

#include <lanelet2_extension/utility/message_conversion.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/qos.hpp>
#include <tier4_autoware_utils/trajectory/trajectory.hpp>
#include <vehicle_info_util/vehicle_info_util.hpp>

#include <algorithm>

namespace apparent_safe_velocity_limiter
{
ApparentSafeVelocityLimiterNode::ApparentSafeVelocityLimiterNode(
  const rclcpp::NodeOptions & node_options)
: rclcpp::Node("apparent_safe_velocity_limiter", node_options),
  preprocessing_params_(*this),
  projection_params_(*this),
  obstacle_params_(*this)
{
  sub_trajectory_ = create_subscription<Trajectory>(
    "~/input/trajectory", 1, [this](const Trajectory::ConstSharedPtr msg) { onTrajectory(msg); });
  sub_occupancy_grid_ = create_subscription<nav_msgs::msg::OccupancyGrid>(
    "~/input/occupancy_grid", 1,
    [this](const OccupancyGrid::ConstSharedPtr msg) { occupancy_grid_ptr_ = msg; });
  sub_pointcloud_ = create_subscription<PointCloud>(
    "~/input/obstacle_pointcloud", rclcpp::QoS(1).best_effort(),
    [this](const PointCloud::ConstSharedPtr msg) { pointcloud_ptr_ = msg; });
  sub_objects_ = create_subscription<PredictedObjects>(
    "~/input/dynamic_obstacles", 1,
    [this](const PredictedObjects::ConstSharedPtr msg) { dynamic_obstacles_ptr_ = msg; });
  sub_odom_ = create_subscription<nav_msgs::msg::Odometry>(
    "~/input/odometry", rclcpp::QoS{1}, [this](const nav_msgs::msg::Odometry::ConstSharedPtr msg) {
      current_ego_velocity_ = static_cast<Float>(msg->twist.twist.linear.x);
    });
  map_sub_ = create_subscription<autoware_auto_mapping_msgs::msg::HADMapBin>(
    "~/input/map", rclcpp::QoS{1}.transient_local(),
    [this](const autoware_auto_mapping_msgs::msg::HADMapBin::ConstSharedPtr msg) {
      lanelet::utils::conversion::fromBinMsg(*msg, lanelet_map_ptr_);
    });
  route_sub_ = create_subscription<autoware_auto_planning_msgs::msg::HADMapRoute>(
    "~/input/route", rclcpp::QoS{1}.transient_local(),
    std::bind(&ApparentSafeVelocityLimiterNode::onRoute, this, std::placeholders::_1));

  pub_trajectory_ = create_publisher<Trajectory>("~/output/trajectory", 1);
  pub_debug_markers_ =
    create_publisher<visualization_msgs::msg::MarkerArray>("~/output/debug_markers", 1);
  pub_debug_pointcloud_ = create_publisher<PointCloud>("~/output/debug_pointcloud", 1);

  const auto vehicle_info = vehicle_info_util::VehicleInfoUtil(*this).getVehicleInfo();
  vehicle_lateral_offset_ = static_cast<Float>(vehicle_info.max_lateral_offset_m);
  obstacle_params_.pcd_cluster_max_dist = 2 * vehicle_lateral_offset_;
  vehicle_front_offset_ = static_cast<Float>(vehicle_info.max_longitudinal_offset_m);

  projection_params_.wheel_base = vehicle_info.wheel_base_m;
  projection_params_.extra_length = vehicle_front_offset_ + distance_buffer_;

  set_param_res_ =
    add_on_set_parameters_callback([this](const auto & params) { return onParameter(params); });
  self_pose_listener_.waitForFirstPose();
}

rcl_interfaces::msg::SetParametersResult ApparentSafeVelocityLimiterNode::onParameter(
  const std::vector<rclcpp::Parameter> & parameters)
{
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;
  for (const auto & parameter : parameters) {
    if (parameter.get_name() == "distance_buffer") {
      distance_buffer_ = static_cast<Float>(parameter.as_double());
      projection_params_.extra_length = vehicle_front_offset_ + distance_buffer_;
      // Preprocessing parameters
    } else if (parameter.get_name() == PreprocessingParameters::START_DIST_PARAM) {
      preprocessing_params_.start_distance = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == PreprocessingParameters::DOWNSAMPLING_PARAM) {
      if (!preprocessing_params_.updateDownsampleFactor(parameter.as_int())) {
        result.successful = false;
        result.reason = "downsample_factor must be positive";
      }
    } else if (parameter.get_name() == PreprocessingParameters::CALC_STEER_PARAM) {
      preprocessing_params_.calculate_steering_angles = parameter.as_bool();
      // Velocity parameters
    } else if (parameter.get_name() == VelocityParameters::MIN_VEL_PARAM) {
      velocity_params_.min_velocity = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == VelocityParameters::MAX_DECEL_PARAM) {
      velocity_params_.max_deceleration = static_cast<Float>(parameter.as_double());
      // Obstacle parameters
    } else if (parameter.get_name() == ProjectionParameters::DURATION_PARAM) {
      const auto min_ttc = static_cast<Float>(parameter.as_double());
      if (min_ttc > 0.0) {
        projection_params_.duration = min_ttc;
      } else {
        result.successful = false;
        result.reason = "duration of forward projection must be positive";
      }
    } else if (parameter.get_name() == ObstacleParameters::DYN_SOURCE_PARAM) {
      if (!obstacle_params_.updateType(*this, parameter.as_string())) {
        result.successful = false;
        result.reason = "dynamic_source value must be 'pointcloud' or 'occupancy_grid'";
      }
    } else if (parameter.get_name() == ObstacleParameters::OCC_GRID_THRESH_PARAM) {
      obstacle_params_.occupancy_grid_threshold = static_cast<int8_t>(parameter.as_int());
    } else if (parameter.get_name() == ObstacleParameters::BUFFER_PARAM) {
      obstacle_params_.dynamic_obstacles_buffer = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == ObstacleParameters::MIN_VEL_PARAM) {
      obstacle_params_.dynamic_obstacles_min_vel = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == ObstacleParameters::MAP_TAGS_PARAM) {
      obstacle_params_.static_map_tags = parameter.as_string_array();
    } else if (parameter.get_name() == ObstacleParameters::FILTERING_PARAM) {
      obstacle_params_.filter_envelope = parameter.as_bool();
      // Projection parameters
    } else if (parameter.get_name() == ProjectionParameters::MODEL_PARAM) {
      if (!projection_params_.updateModel(*this, parameter.as_string())) {
        result.successful = false;
        result.reason = "Unknown forward projection model";
      }
    } else if (parameter.get_name() == ProjectionParameters::NBPOINTS_PARAM) {
      if (!projection_params_.updateNbPoints(*this, static_cast<int>(parameter.as_int()))) {
        result.successful = false;
        result.reason = "number of points for projections must be at least 2";
      }
    } else if (parameter.get_name() == ProjectionParameters::STEER_OFFSET_PARAM) {
      projection_params_.steering_angle_offset = parameter.as_double();
    } else if (parameter.get_name() == ProjectionParameters::DISTANCE_METHOD_PARAM) {
      projection_params_.updateDistanceMethod(*this, parameter.as_string());
    } else if (parameter.get_name() == "obstacles.static_map_ids") {
      static_map_obstacle_ids_ = parameter.as_integer_array();
    } else {
      RCLCPP_WARN(get_logger(), "Unknown parameter %s", parameter.get_name().c_str());
      result.successful = false;
    }
  }
  return result;
}

void ApparentSafeVelocityLimiterNode::onTrajectory(const Trajectory::ConstSharedPtr msg)
{
  const auto t_start = std::chrono::system_clock::now();
  const auto ego_idx =
    tier4_autoware_utils::findNearestIndex(msg->points, self_pose_listener_.getCurrentPose()->pose);
  if (!validInputs(ego_idx)) return;

  velocity_params_.current_ego_velocity = *current_ego_velocity_;
  const auto start_idx = calculateStartIndex(*msg, *ego_idx, preprocessing_params_.start_distance);
  Trajectory downsampled_traj =
    downsampleTrajectory(*msg, start_idx, preprocessing_params_.downsample_factor);
  const auto polygon_masks = createPolygonMasks(
    *dynamic_obstacles_ptr_, obstacle_params_.dynamic_obstacles_buffer,
    obstacle_params_.dynamic_obstacles_min_vel);
  const auto projected_linestrings = createProjectedLines(downsampled_traj, projection_params_);
  const auto footprint_polygons =
    createFootprintPolygons(projected_linestrings, vehicle_lateral_offset_);
  polygon_t envelope_polygon;
  if (obstacle_params_.filter_envelope)
    envelope_polygon = createEnvelopePolygon(footprint_polygons);
  auto obstacles = static_map_obstacles_;
  if (obstacle_params_.dynamic_source != ObstacleParameters::STATIC_ONLY) {
    PointCloud debug_pointcloud;
    const auto dynamic_obstacles = createObstacles(
      *occupancy_grid_ptr_, *pointcloud_ptr_, polygon_masks, envelope_polygon, transform_listener_,
      msg->header.frame_id, obstacle_params_, debug_pointcloud);
    debug_pointcloud.header.stamp = now();
    debug_pointcloud.header.frame_id = msg->header.frame_id;
    pub_debug_pointcloud_->publish(debug_pointcloud);
    obstacles.insert(obstacles.end(), dynamic_obstacles.begin(), dynamic_obstacles.end());
  }
  limitVelocity(
    downsampled_traj, obstacles, projected_linestrings, footprint_polygons, projection_params_,
    velocity_params_, obstacle_params_.filter_envelope);
  auto safe_trajectory = copyDownsampledVelocity(
    downsampled_traj, *msg, start_idx, preprocessing_params_.downsample_factor);
  safe_trajectory.header.stamp = now();

  pub_trajectory_->publish(safe_trajectory);

  const auto t_end = std::chrono::system_clock::now();
  const auto runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
  RCLCPP_WARN(get_logger(), "onTrajectory() runtime: %d ms", runtime);

  const auto safe_projected_linestrings =
    createProjectedLines(downsampled_traj, projection_params_);
  const auto safe_footprint_polygons =
    createFootprintPolygons(safe_projected_linestrings, vehicle_lateral_offset_);
  pub_debug_markers_->publish(makeDebugMarkers(
    obstacles, projected_linestrings, safe_projected_linestrings, footprint_polygons,
    safe_footprint_polygons, occupancy_grid_ptr_->info.origin.position.z));
}

bool ApparentSafeVelocityLimiterNode::validInputs(const boost::optional<size_t> & ego_idx)
{
  constexpr auto one_sec = rcutils_duration_value_t(1000);
  if (!occupancy_grid_ptr_)
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), one_sec, "Occupancy grid not yet received");
  if (!dynamic_obstacles_ptr_)
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), one_sec, "Dynamic obstacles not yet received");
  if (!ego_idx)
    RCLCPP_WARN_THROTTLE(
      get_logger(), *get_clock(), one_sec, "Cannot calculate ego index on the trajectory");
  if (!current_ego_velocity_)
    RCLCPP_WARN_THROTTLE(
      get_logger(), *get_clock(), one_sec, "Current ego velocity not yet received");
  return occupancy_grid_ptr_ && dynamic_obstacles_ptr_ && ego_idx && current_ego_velocity_;
}

void ApparentSafeVelocityLimiterNode::onRoute(
  const autoware_auto_planning_msgs::msg::HADMapRoute::ConstSharedPtr msg)
{
  if (!lanelet_map_ptr_) return;
  static_map_obstacles_ = extractStaticObstacles(
    *lanelet_map_ptr_, *msg, obstacle_params_.static_map_tags, static_map_obstacle_ids_);
}
}  // namespace apparent_safe_velocity_limiter

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(apparent_safe_velocity_limiter::ApparentSafeVelocityLimiterNode)

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

#include "apparent_safe_velocity_limiter/collision.hpp"
#include "apparent_safe_velocity_limiter/debug.hpp"
#include "apparent_safe_velocity_limiter/occupancy_grid_utils.hpp"
#include "apparent_safe_velocity_limiter/pointcloud_utils.hpp"
#include "apparent_safe_velocity_limiter/types.hpp"
#include "tier4_autoware_utils/geometry/geometry.hpp"

#include <pcl_ros/transforms.hpp>

#include <nav_msgs/msg/detail/occupancy_grid__struct.hpp>
#include <sensor_msgs/msg/detail/point_cloud2__struct.hpp>

#include <boost/geometry/algorithms/correct.hpp>

#include <pcl_conversions/pcl_conversions.h>

namespace apparent_safe_velocity_limiter
{
ApparentSafeVelocityLimiterNode::ApparentSafeVelocityLimiterNode(
  const rclcpp::NodeOptions & node_options)
: rclcpp::Node("apparent_safe_velocity_limiter", node_options)
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
    "~/input/odometry", rclcpp::QoS{1}, [&](const nav_msgs::msg::Odometry::ConstSharedPtr msg) {
      current_ego_velocity_ = static_cast<Float>(msg->twist.twist.linear.x);
    });

  pub_trajectory_ = create_publisher<Trajectory>("~/output/trajectory", 1);
  pub_debug_markers_ =
    create_publisher<visualization_msgs::msg::MarkerArray>("~/output/debug_markers", 1);
  pub_debug_pointcloud_ = create_publisher<PointCloud>("~/output/debug_pointcloud", 1);

  const auto vehicle_info = vehicle_info_util::VehicleInfoUtil(*this).getVehicleInfo();
  vehicle_lateral_offset_ = static_cast<Float>(vehicle_info.max_lateral_offset_m);
  vehicle_front_offset_ = static_cast<Float>(vehicle_info.max_longitudinal_offset_m);

  auto obstacle_type = declare_parameter<std::string>("obstacle_type");
  if (obstacle_type == "pointcloud")
    obstacle_type_ = POINTCLOUD;
  else if (obstacle_type == "occupancy_grid")
    obstacle_type_ = OCCUPANCYGRID;
  else
    RCLCPP_WARN(
      get_logger(), "Unknown obstacle_type value: '%s'. Using default POINTCLOUD type.",
      obstacle_type.c_str());

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
    if (parameter.get_name() == "time_buffer") {
      const auto new_time_buffer = static_cast<Float>(parameter.as_double());
      if (new_time_buffer > 0.0) {
        time_buffer_ = new_time_buffer;
      } else {
        result.successful = false;
        result.reason = "time_buffer must be positive";
      }
    } else if (parameter.get_name() == "distance_buffer") {
      distance_buffer_ = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == "start_distance") {
      start_distance_ = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == "downsample_factor") {
      const auto new_downsample_factor = static_cast<int>(parameter.as_int());
      if (new_downsample_factor > 0) {
        downsample_factor_ = new_downsample_factor;
      } else {
        result.successful = false;
        result.reason = "downsample_factor must be positive";
      }
    } else if (parameter.get_name() == "occupancy_grid_obstacle_threshold") {
      occupancy_grid_obstacle_threshold_ = static_cast<int8_t>(parameter.as_int());
    } else if (parameter.get_name() == "dynamic_obstacles_buffer") {
      dynamic_obstacles_buffer_ = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == "dynamic_obstacles_min_vel") {
      dynamic_obstacles_min_vel_ = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == "min_adjusted_velocity") {
      min_adjusted_velocity_ = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == "max_deceleration") {
      max_deceleration_ = static_cast<Float>(parameter.as_double());
    } else if (parameter.get_name() == "obstacle_type") {
      if (parameter.as_string() == "pointcloud") {
        obstacle_type_ = POINTCLOUD;
      } else if (parameter.as_string() == "occupancy_grid") {
        obstacle_type_ = OCCUPANCYGRID;
      } else {
        result.successful = false;
        result.reason = "obstacle_type value must be 'pointcloud' or 'occupancy_grid'";
      }
    } else {
      RCLCPP_WARN(get_logger(), "Unknown parameter %s", parameter.get_name().c_str());
      result.successful = false;
    }
  }
  return result;
}

void ApparentSafeVelocityLimiterNode::onTrajectory(const Trajectory::ConstSharedPtr msg)
{
  const auto ego_idx =
    tier4_autoware_utils::findNearestIndex(msg->points, self_pose_listener_.getCurrentPose()->pose);
  if (!validInputs(ego_idx)) return;

  const auto extra_vehicle_length = vehicle_front_offset_ + distance_buffer_;
  const auto start_idx = calculateStartIndex(*msg, *ego_idx, start_distance_);
  Trajectory downsampled_traj = downsampleTrajectory(*msg, start_idx);
  // TODO(Maxime CLEMENT): used for debugging, remove before merging
  double obs_mask_duration{};
  double obs_envelope_duration{};
  double obs_filter_duration{};
  double footprint_duration{};
  double dist_poly_duration{};
  tier4_autoware_utils::StopWatch<std::chrono::milliseconds> stopwatch;
  stopwatch.tic("obs_mask_duration");
  const auto polygon_masks = createPolygonMasks(*msg, start_idx);
  obs_mask_duration += stopwatch.toc("obs_mask_duration");
  stopwatch.tic("obs_envelope_duration");
  const auto envelope_polygon = createEnvelopePolygon(*msg, extra_vehicle_length);
  obs_envelope_duration += stopwatch.toc("obs_envelope_duration");
  stopwatch.tic("obs_filter_duration");
  const auto obstacles = createObstacleLines(
    *occupancy_grid_ptr_, *pointcloud_ptr_, polygon_masks, envelope_polygon, msg->header.frame_id);
  obs_filter_duration += stopwatch.toc("obs_filter_duration");
  limitVelocity(downsampled_traj, extra_vehicle_length, obstacles);
  const auto safe_trajectory = copyDownsampledVelocity(downsampled_traj, *msg, start_idx);

  pub_trajectory_->publish(safe_trajectory);

  ForwardProjectionFunction fn = [&](const TrajectoryPoint & p) {
    return forwardSimulatedSegment(p, time_buffer_, distance_buffer_ + vehicle_front_offset_);
  };
  pub_debug_markers_->publish(makeDebugMarkers(
    *msg, safe_trajectory, obstacles, fn, occupancy_grid_ptr_->info.origin.position.z));
  // TODO(Maxime CLEMENT): removes or change to RCLCPP_DEBUG before merging
  RCLCPP_WARN(get_logger(), "Runtimes");
  RCLCPP_WARN(get_logger(), "  obstacles masks      = %2.2fms", obs_mask_duration);
  RCLCPP_WARN(get_logger(), "  obstacles envelope   = %2.2fms", obs_envelope_duration);
  RCLCPP_WARN(get_logger(), "  obstacles filter     = %2.2fms", obs_filter_duration);
  RCLCPP_WARN(get_logger(), "  footprint generation = %2.2fms", footprint_duration);
  RCLCPP_WARN(get_logger(), "  distance to obstacle = %2.2fms", dist_poly_duration);
  const auto runtime = stopwatch.toc();
  runtimes.insert(runtime);
  const auto sum = std::accumulate(runtimes.begin(), runtimes.end(), 0.0);
  RCLCPP_WARN(get_logger(), "**************** Total = %2.2fms", runtime);
  RCLCPP_WARN(get_logger(), "**************** Total (max) = %2.2fms", *std::prev(runtimes.end()));
  RCLCPP_WARN(
    get_logger(), "**************** Total (med) = %2.2fms",
    *std::next(runtimes.begin(), runtimes.size() / 2));
  RCLCPP_WARN(get_logger(), "**************** Total (avg) = %2.2fms", sum / runtimes.size());
}

Float ApparentSafeVelocityLimiterNode::calculateSafeVelocity(
  const TrajectoryPoint & trajectory_point, const Float dist_to_collision) const
{
  return std::min(
    trajectory_point.longitudinal_velocity_mps,
    std::max(min_adjusted_velocity_, static_cast<Float>(dist_to_collision / time_buffer_)));
}

size_t ApparentSafeVelocityLimiterNode::calculateStartIndex(
  const Trajectory & trajectory, const size_t ego_idx, const Float start_distance)
{
  auto dist = 0.0;
  auto idx = ego_idx;
  while (idx + 1 < trajectory.points.size() && dist < start_distance) {
    dist +=
      tier4_autoware_utils::calcDistance2d(trajectory.points[idx], trajectory.points[idx + 1]);
    ++idx;
  }
  return idx;
}

Trajectory ApparentSafeVelocityLimiterNode::downsampleTrajectory(
  const Trajectory & trajectory, const size_t start_idx) const
{
  Trajectory downsampled_traj;
  downsampled_traj.header = trajectory.header;
  downsampled_traj.points.reserve(trajectory.points.size() / downsample_factor_);
  for (size_t i = start_idx; i < trajectory.points.size(); i += downsample_factor_)
    downsampled_traj.points.push_back(trajectory.points[i]);
  return downsampled_traj;
}

multipolygon_t ApparentSafeVelocityLimiterNode::createPolygonMasks(
  const Trajectory & trajectory, const size_t start_idx) const
{
  auto polygon_masks = createObjectPolygons(
    *dynamic_obstacles_ptr_, dynamic_obstacles_buffer_, dynamic_obstacles_min_vel_);
  linestring_t trajectory_linestring;
  for (size_t i = start_idx; i < trajectory.points.size(); ++i)
    trajectory_linestring.emplace_back(
      trajectory.points[i].pose.position.x, trajectory.points[i].pose.position.y);
  return polygon_masks;
}

polygon_t ApparentSafeVelocityLimiterNode::createEnvelopePolygon(
  const Trajectory & trajectory, const double extra_vehicle_length) const
{
  polygon_t envelope_polygon;
  for (const auto & point : trajectory.points)
    envelope_polygon.outer().emplace_back(point.pose.position.x, point.pose.position.y);
  for (auto it = trajectory.points.rbegin(); it != trajectory.points.rend(); ++it) {
    const auto forward_simulated_vector =
      forwardSimulatedSegment(*it, time_buffer_, extra_vehicle_length);
    envelope_polygon.outer().push_back(forward_simulated_vector.second);
  }
  boost::geometry::correct(envelope_polygon);
  return envelope_polygon;
}

bool ApparentSafeVelocityLimiterNode::validInputs(const boost::optional<size_t> & ego_idx)
{
  constexpr auto one_sec = rcutils_duration_value_t(1000);
  if (!occupancy_grid_ptr_)
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), one_sec, "Occupancy grid not yet received");
  if (!dynamic_obstacles_ptr_)
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), one_sec, "Dynamic obstable not yet received");
  if (!ego_idx)
    RCLCPP_WARN_THROTTLE(
      get_logger(), *get_clock(), one_sec, "Cannot calculate ego index on the trajectory");
  if (!current_ego_velocity_)
    RCLCPP_WARN_THROTTLE(
      get_logger(), *get_clock(), one_sec, "Current ego velocity not yet received");
  return occupancy_grid_ptr_ && dynamic_obstacles_ptr_ && ego_idx && current_ego_velocity_;
}

multilinestring_t ApparentSafeVelocityLimiterNode::createObstacleLines(
  const nav_msgs::msg::OccupancyGrid & occupancy_grid,
  const sensor_msgs::msg::PointCloud2 & pointcloud, const multipolygon_t & polygon_masks,
  const polygon_t & envelope_polygon, const std::string & target_frame)
{
  if (obstacle_type_ == OCCUPANCYGRID) {
    return extractObstacleLines(
      occupancy_grid, polygon_masks, envelope_polygon, occupancy_grid_obstacle_threshold_);
  } else {
    const auto filtered_pcd = transformAndFilterPointCloud(
      pointcloud, polygon_masks, envelope_polygon, transform_listener_, target_frame);
    // TODO(Maxime CLEMENT): remove before merging
    PointCloud ros_pointcloud;
    pcl::toROSMsg(*filtered_pcd, ros_pointcloud);
    ros_pointcloud.header.stamp = now();
    ros_pointcloud.header.frame_id = target_frame;
    pub_debug_pointcloud_->publish(ros_pointcloud);
    return extractObstacleLines(filtered_pcd, vehicle_lateral_offset_ * 2);
  }
}

void ApparentSafeVelocityLimiterNode::limitVelocity(
  Trajectory & trajectory, const double extra_length, const multilinestring_t & obstacles) const
{
  Float time = 0.0;
  for (size_t i = 0; i < trajectory.points.size(); ++i) {
    auto & trajectory_point = trajectory.points[i];
    if (i > 0) {
      const auto & prev_point = trajectory.points[i - 1];
      time += static_cast<Float>(
        tier4_autoware_utils::calcDistance2d(prev_point, trajectory_point) /
        prev_point.longitudinal_velocity_mps);
    }
    const auto forward_simulated_vector =
      forwardSimulatedSegment(trajectory_point, time_buffer_, extra_length);
    const auto footprint = generateFootprint(forward_simulated_vector, vehicle_lateral_offset_);
    const auto dist_to_collision =
      distanceToClosestCollision(forward_simulated_vector, footprint, obstacles);
    if (dist_to_collision) {
      const auto min_feasible_velocity = *current_ego_velocity_ - max_deceleration_ * time;
      trajectory_point.longitudinal_velocity_mps = std::max(
        min_feasible_velocity,
        calculateSafeVelocity(
          trajectory_point, static_cast<Float>(*dist_to_collision - extra_length)));
    }
  }
}

Trajectory ApparentSafeVelocityLimiterNode::copyDownsampledVelocity(
  const Trajectory & downsampled_traj, Trajectory trajectory, const size_t start_idx) const
{
  for (size_t i = 0; i < downsampled_traj.points.size(); ++i) {
    trajectory.points[start_idx + i * downsample_factor_].longitudinal_velocity_mps =
      downsampled_traj.points[i].longitudinal_velocity_mps;
  }
  trajectory.header.stamp = now();
  return trajectory;
}
}  // namespace apparent_safe_velocity_limiter

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(apparent_safe_velocity_limiter::ApparentSafeVelocityLimiterNode)

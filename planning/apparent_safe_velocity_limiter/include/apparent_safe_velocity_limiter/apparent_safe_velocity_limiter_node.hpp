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

#ifndef APPARENT_SAFE_VELOCITY_LIMITER__APPARENT_SAFE_VELOCITY_LIMITER_NODE_HPP_
#define APPARENT_SAFE_VELOCITY_LIMITER__APPARENT_SAFE_VELOCITY_LIMITER_NODE_HPP_

#include "apparent_safe_velocity_limiter/types.hpp"
#include "tier4_autoware_utils/geometry/geometry.hpp"

#include <grid_map_ros/GridMapRosConverter.hpp>
#include <rclcpp/duration.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/qos.hpp>
#include <rclcpp/rclcpp.hpp>
#include <tier4_autoware_utils/ros/self_pose_listener.hpp>
#include <tier4_autoware_utils/ros/transform_listener.hpp>
#include <tier4_autoware_utils/system/stop_watch.hpp>
#include <tier4_autoware_utils/trajectory/trajectory.hpp>
#include <vehicle_info_util/vehicle_info_util.hpp>

#include <autoware_auto_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_auto_planning_msgs/msg/trajectory.hpp>
#include <autoware_auto_planning_msgs/msg/trajectory_point.hpp>
#include <grid_map_msgs/msg/grid_map.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <boost/optional.hpp>

#include <rcutils/time.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace apparent_safe_velocity_limiter
{

class ApparentSafeVelocityLimiterNode : public rclcpp::Node
{
public:
  explicit ApparentSafeVelocityLimiterNode(const rclcpp::NodeOptions & node_options);

private:
  tier4_autoware_utils::TransformListener transform_listener_{this};
  tier4_autoware_utils::SelfPoseListener self_pose_listener_{this};
  rclcpp::Publisher<Trajectory>::SharedPtr
    pub_trajectory_;  //!< @brief publisher for output trajectory
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr
    pub_debug_markers_;  //!< @brief publisher for debug markers
  rclcpp::Publisher<PointCloud>::SharedPtr
    pub_debug_pointcloud_;  //!< @brief publisher for filtered pointcloud
  rclcpp::Subscription<Trajectory>::SharedPtr
    sub_trajectory_;  //!< @brief subscriber for reference trajectory
  rclcpp::Subscription<PredictedObjects>::SharedPtr
    sub_objects_;  //!< @brief subscribe for dynamic objects
  rclcpp::Subscription<OccupancyGrid>::SharedPtr
    sub_occupancy_grid_;  //!< @brief subscriber for occupancy grid
  rclcpp::Subscription<PointCloud>::SharedPtr
    sub_pointcloud_;  //!< @brief subscriber for obstacle pointcloud
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr
    sub_odom_;  //!< @brief subscriber for the current velocity

  // cached inputs
  PredictedObjects::ConstSharedPtr dynamic_obstacles_ptr_;
  OccupancyGrid::ConstSharedPtr occupancy_grid_ptr_;
  PointCloud::ConstSharedPtr pointcloud_ptr_;

  // Benchmarking & Debugging
  std::multiset<double> runtimes;

  // parameters
  ProjectionParameters projection_params_;
  Float time_buffer_ = static_cast<Float>(declare_parameter<Float>("time_buffer"));
  Float distance_buffer_ = static_cast<Float>(declare_parameter<Float>("distance_buffer"));
  Float start_distance_ = static_cast<Float>(declare_parameter<Float>("start_distance"));
  Float min_adjusted_velocity_ =
    static_cast<Float>(declare_parameter<Float>("min_adjusted_velocity"));
  int downsample_factor_ = static_cast<int>(declare_parameter<int>("downsample_factor"));
  int8_t occupancy_grid_obstacle_threshold_ =
    static_cast<int8_t>(declare_parameter<int>("occupancy_grid_obstacle_threshold"));
  Float dynamic_obstacles_buffer_ =
    static_cast<Float>(declare_parameter<Float>("dynamic_obstacles_buffer"));
  Float dynamic_obstacles_min_vel_ =
    static_cast<Float>(declare_parameter<Float>("dynamic_obstacles_min_vel"));
  Float max_deceleration_ = static_cast<Float>(declare_parameter<Float>("max_deceleration"));
  ObstacleType obstacle_type_ = POINTCLOUD;
  Float vehicle_lateral_offset_;
  Float vehicle_front_offset_;
  std::optional<Float> current_ego_velocity_;

  OnSetParametersCallbackHandle::SharedPtr set_param_res_;

  /// @brief callback for parameter updates
  /// @param[in] parameters updated parameters and their new values
  rcl_interfaces::msg::SetParametersResult onParameter(
    const std::vector<rclcpp::Parameter> & parameters);

  /// @brief callback for input trajectories. Publishes a trajectory with updated velocities
  /// @param[in] msg input trajectory message
  void onTrajectory(const Trajectory::ConstSharedPtr msg);

  /// @brief calculate the apparent safe velocity
  /// @param[in] trajectory_point trajectory point for which to calculate the apparent safe velocity
  /// @param[in] dist_to_collision distance from the trajectory point to the apparent collision
  /// @return apparent safe velocity
  Float calculateSafeVelocity(
    const TrajectoryPoint & trajectory_point, const Float dist_to_collision) const;

  /// @brief create and publish debug markers for the given trajectories and polygons
  /// @param[in] original_trajectory original input trajectory
  /// @param[in] adjusted_trajectory trajectory adjusted for apparent safety
  /// @param[in] polygons polygons to publish as markers
  void publishDebugMarkers(
    const Trajectory & original_trajectory, const Trajectory & adjusted_trajectory,
    const multilinestring_t & polygons) const;

  /// @brief calculate trajectory index that is ahead of the given index by the given distance
  /// @param[in] trajectory trajectory
  /// @param[in] ego_idx index closest to the current ego position in the trajectory
  /// @param[in] start_distance desired distance ahead of the ego_idx
  /// @return trajectory index ahead of ego_idx by the start_distance
  static size_t calculateStartIndex(
    const Trajectory & trajectory, const size_t ego_idx, const Float start_distance);

  Trajectory downsampleTrajectory(const Trajectory & trajectory, const size_t start_idx) const;
  multipolygon_t createPolygonMasks() const;
  polygon_t createEnvelopePolygon(
    const Trajectory & trajectory, const size_t start_idx,
    ProjectionParameters & projections_params) const;
  bool validInputs(const boost::optional<size_t> & ego_idx);
  multilinestring_t createObstacleLines(
    const nav_msgs::msg::OccupancyGrid & occupancy_grid,
    const sensor_msgs::msg::PointCloud2 & pointcloud, const multipolygon_t & polygon_masks,
    const polygon_t & envelope_polygon, const std::string & target_frame);
  multipolygon_t limitVelocity(
    Trajectory & trajectory, ProjectionParameters & projection_params,
    const multilinestring_t & obstacles) const;
  Trajectory copyDownsampledVelocity(
    const Trajectory & downsampled_traj, Trajectory trajectory, const size_t start_idx) const;
};
}  // namespace apparent_safe_velocity_limiter

#endif  // APPARENT_SAFE_VELOCITY_LIMITER__APPARENT_SAFE_VELOCITY_LIMITER_NODE_HPP_

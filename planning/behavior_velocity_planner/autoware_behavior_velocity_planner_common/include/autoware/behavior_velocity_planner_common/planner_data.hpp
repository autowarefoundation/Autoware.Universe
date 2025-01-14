// Copyright 2019 Autoware Foundation
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

#ifndef AUTOWARE__BEHAVIOR_VELOCITY_PLANNER_COMMON__PLANNER_DATA_HPP_
#define AUTOWARE__BEHAVIOR_VELOCITY_PLANNER_COMMON__PLANNER_DATA_HPP_

#include "autoware/behavior_velocity_planner_common/utilization/util.hpp"
#include "autoware/route_handler/route_handler.hpp"
#include "autoware/velocity_smoother/smoother/smoother_base.hpp"
#include "autoware_vehicle_info_utils/vehicle_info_utils.hpp"

#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group_array.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <std_msgs/msg/header.hpp>
#include <tier4_planning_msgs/msg/velocity_limit.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <deque>
#include <map>
#include <memory>
#include <optional>

namespace autoware::behavior_velocity_planner
{
struct PlannerData
{
  explicit PlannerData(rclcpp::Node & node);

  rclcpp::Clock::SharedPtr clock_;

  geometry_msgs::msg::PoseStamped::ConstSharedPtr current_odometry;
  geometry_msgs::msg::TwistStamped::ConstSharedPtr current_velocity;
  geometry_msgs::msg::AccelWithCovarianceStamped::ConstSharedPtr current_acceleration;
  static constexpr double velocity_buffer_time_sec = 10.0;
  std::deque<geometry_msgs::msg::TwistStamped> velocity_buffer;
  autoware_perception_msgs::msg::PredictedObjects::ConstSharedPtr predicted_objects;
  pcl::PointCloud<pcl::PointXYZ>::ConstPtr no_ground_pointcloud;

  nav_msgs::msg::OccupancyGrid::ConstSharedPtr occupancy_grid;

  double ego_nearest_dist_threshold;
  double ego_nearest_yaw_threshold;

  std::map<lanelet::Id, TrafficSignalStamped> traffic_light_id_map_raw_;
  std::map<lanelet::Id, TrafficSignalStamped> traffic_light_id_map_last_observed_;
  std::optional<tier4_planning_msgs::msg::VelocityLimit> external_velocity_limit;

  bool is_simulation = false;

  std::shared_ptr<autoware::velocity_smoother::SmootherBase> velocity_smoother_;
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler_;
  autoware::vehicle_info_utils::VehicleInfo vehicle_info_;

  double max_stop_acceleration_threshold;
  double max_stop_jerk_threshold;
  double system_delay;
  double delay_response_time;
  double stop_line_extend_length;

  bool isVehicleStopped(const double stop_duration = 0.0) const;

  std::optional<TrafficSignalStamped> getTrafficSignal(
    const lanelet::Id id, const bool keep_last_observation = false) const;
};
}  // namespace autoware::behavior_velocity_planner

#endif  // AUTOWARE__BEHAVIOR_VELOCITY_PLANNER_COMMON__PLANNER_DATA_HPP_

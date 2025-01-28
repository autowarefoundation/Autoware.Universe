// Copyright 2024 Autoware Foundation
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

#ifndef AUTOWARE__MOTION_VELOCITY_PLANNER_COMMON_UNIVERSE__PLANNER_DATA_HPP_
#define AUTOWARE__MOTION_VELOCITY_PLANNER_COMMON_UNIVERSE__PLANNER_DATA_HPP_

#include <autoware/motion_utils/distance/distance.hpp>
#include <autoware/motion_velocity_planner_common_universe/collision_checker.hpp>
#include <autoware/route_handler/route_handler.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware/velocity_smoother/smoother/smoother_base.hpp>
#include <autoware_vehicle_info_utils/vehicle_info_utils.hpp>

#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group_array.hpp>
#include <autoware_planning_msgs/msg/trajectory_point.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <std_msgs/msg/header.hpp>

#include <lanelet2_core/Forward.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace autoware::motion_velocity_planner
{
struct TrafficSignalStamped
{
  builtin_interfaces::msg::Time stamp;
  autoware_perception_msgs::msg::TrafficLightGroup signal;
};

struct StopPoint
{
  double ego_trajectory_arc_length{};  // [m] arc length along the ego trajectory
  geometry_msgs::msg::Pose
    ego_stop_pose;                       // intersection between the trajectory and a map stop line
  lanelet::BasicLineString2d stop_line;  // stop line from the map
};
struct PlannerData
{
  explicit PlannerData(rclcpp::Node & node)
  : vehicle_info_(autoware::vehicle_info_utils::VehicleInfoUtils(node).getVehicleInfo())
  {
  }

  struct Object
  {
  public:
    Object() = default;
    explicit Object(const autoware_perception_msgs::msg::PredictedObject & arg_predicted_object)
    : predicted_object(arg_predicted_object)
    {
    }

    autoware_perception_msgs::msg::PredictedObject predicted_object;
    // double get_lon_vel_relative_to_traj()
    // {
    //   if (!lon_vel_relative_to_traj) {
    //     lon_vel_relative_to_traj = 0.0;
    //   }
    //   return *lon_vel_relative_to_traj;
    // }
    // double get_lat_vel_relative_to_traj()
    // {
    //   if (!lat_vel_relative_to_traj) {
    //     lat_vel_relative_to_traj = 0.0;
    //   }
    //   return *lat_vel_relative_to_traj;
    // }

  private:
    // TODO(murooka) implement the following variables and their functions.
    // std::optional<double> dist_to_traj_poly{std::nullopt};
    // std::optional<double> dist_to_traj_lateral{std::nullopt};
    // std::optional<double> dist_from_ego_longitudinal{std::nullopt};
    // std::optional<double> lon_vel_relative_to_traj{std::nullopt};
    // std::optional<double> lat_vel_relative_to_traj{std::nullopt};
  };

  struct Pointcloud
  {
  public:
    Pointcloud() = default;
    explicit Pointcloud(const pcl::PointCloud<pcl::PointXYZ> & arg_pointcloud)
    : pointcloud(arg_pointcloud)
    {
    }

    pcl::PointCloud<pcl::PointXYZ> pointcloud;

  private:
    // NOTE: clustered result will be added.
  };

  void process_predicted_objects(
    const autoware_perception_msgs::msg::PredictedObjects & predicted_objects)
  {
    predicted_objects_header = predicted_objects.header;

    objects.clear();
    for (const auto & predicted_object : predicted_objects.objects) {
      objects.push_back(Object(predicted_object));
    }
  }

  // msgs from callbacks that are used for data-ready
  nav_msgs::msg::Odometry current_odometry;
  geometry_msgs::msg::AccelWithCovarianceStamped current_acceleration;
  std_msgs::msg::Header predicted_objects_header;
  std::vector<Object> objects;
  Pointcloud no_ground_pointcloud;
  nav_msgs::msg::OccupancyGrid occupancy_grid;
  std::shared_ptr<route_handler::RouteHandler> route_handler;

  // nearest search
  double ego_nearest_dist_threshold{};
  double ego_nearest_yaw_threshold{};

  // other internal data
  // traffic_light_id_map_raw is the raw observation, while traffic_light_id_map_keep_last keeps the
  // last observed infomation for UNKNOWN
  std::map<lanelet::Id, TrafficSignalStamped> traffic_light_id_map_raw_;
  std::map<lanelet::Id, TrafficSignalStamped> traffic_light_id_map_last_observed_;

  // velocity smoother
  std::shared_ptr<autoware::velocity_smoother::SmootherBase> velocity_smoother_;
  // parameters
  autoware::vehicle_info_utils::VehicleInfo vehicle_info_;

  /// @brief queries the traffic signal information of given Id. if keep_last_observation = true,
  /// recent UNKNOWN observation is overwritten as the last non-UNKNOWN observation
  [[nodiscard]] std::optional<TrafficSignalStamped> get_traffic_signal(
    const lanelet::Id id, const bool keep_last_observation = false) const;

  /// @brief calculate the minimum distance needed by ego to decelerate to the given velocity
  /// @param [in] target_velocity [m/s] target velocity
  /// @return [m] distance needed to reach the target velocity
  [[nodiscard]] std::optional<double> calculate_min_deceleration_distance(
    const double target_velocity) const;

  /// @brief calculate possible stop points along the current trajectory where it intersects with
  /// stop lines
  /// @param [in] trajectory ego trajectory
  /// @return stop points taken from the map
  [[nodiscard]] std::vector<StopPoint> calculate_map_stop_points(
    const std::vector<autoware_planning_msgs::msg::TrajectoryPoint> & trajectory) const;
};
}  // namespace autoware::motion_velocity_planner

#endif  // AUTOWARE__MOTION_VELOCITY_PLANNER_COMMON_UNIVERSE__PLANNER_DATA_HPP_

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

#ifndef AUTOWARE__COLLISION_DETECTOR__NODE_HPP_
#define AUTOWARE__COLLISION_DETECTOR__NODE_HPP_

#include <autoware/universe_utils/ros/polling_subscriber.hpp>
#include <autoware_vehicle_info_utils/vehicle_info_utils.hpp>
#include <diagnostic_updater/diagnostic_updater.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_adapi_v1_msgs/msg/operation_mode_state.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <boost/optional.hpp>

#include <tf2/utils.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::collision_detector
{
using autoware::vehicle_info_utils::VehicleInfo;
using autoware_adapi_v1_msgs::msg::OperationModeState;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_perception_msgs::msg::Shape;

using Obstacle = std::pair<double /* distance */, geometry_msgs::msg::Point>;

class CollisionDetectorNode : public rclcpp::Node
{
public:
  explicit CollisionDetectorNode(const rclcpp::NodeOptions & node_options);

  struct NearbyObjectTypeFilters
  {
    bool filter_car{false};
    bool filter_truck{false};
    bool filter_bus{false};
    bool filter_trailer{false};
    bool filter_unknown{false};
    bool filter_bicycle{false};
    bool filter_motorcycle{false};
    bool filter_pedestrian{false};
  };

  struct NodeParam
  {
    bool use_pointcloud;
    bool use_dynamic_object;
    double collision_distance;
    double nearby_filter_radius;
    double keep_ignoring_time;
    NearbyObjectTypeFilters nearby_object_type_filters;
  };

  struct TimestampedObject
  {
    unique_identifier_msgs::msg::UUID object_id;
    rclcpp::Time timestamp;
  };

private:
  PredictedObjects filterObjects(const PredictedObjects & objects);

  void removeOldObjects(
    std::vector<TimestampedObject> & container, const rclcpp::Time & current_time,
    const rclcpp::Duration & duration_sec);

  bool shouldBeExcluded(
    const autoware_perception_msgs::msg::ObjectClassification::_label_type & classification) const;

  void checkCollision(diagnostic_updater::DiagnosticStatusWrapper & stat);

  boost::optional<Obstacle> getNearestObstacle() const;

  boost::optional<Obstacle> getNearestObstacleByPointCloud() const;

  boost::optional<Obstacle> getNearestObstacleByDynamicObject() const;

  boost::optional<geometry_msgs::msg::TransformStamped> getTransform(
    const std::string & source, const std::string & target, const rclcpp::Time & stamp,
    double duration_sec) const;

  // ros
  mutable tf2_ros::Buffer tf_buffer_{get_clock()};
  mutable tf2_ros::TransformListener tf_listener_{tf_buffer_};
  rclcpp::TimerBase::SharedPtr timer_;

  // publisher and subscriber
  autoware::universe_utils::InterProcessPollingSubscriber<nav_msgs::msg::Odometry> sub_odometry_{
    this, "~/input/odometry"};
  autoware::universe_utils::InterProcessPollingSubscriber<sensor_msgs::msg::PointCloud2>
    sub_pointcloud_{this, "~/input/pointcloud", autoware::universe_utils::SingleDepthSensorQoS()};
  autoware::universe_utils::InterProcessPollingSubscriber<PredictedObjects> sub_dynamic_objects_{
    this, "~/input/objects"};
  autoware::universe_utils::InterProcessPollingSubscriber<
    autoware_adapi_v1_msgs::msg::OperationModeState>
    sub_operation_mode_{this, "/api/operation_mode/state", rclcpp::QoS{1}.transient_local()};

  // parameter
  NodeParam node_param_;
  autoware::vehicle_info_utils::VehicleInfo vehicle_info_;

  // data
  nav_msgs::msg::Odometry::ConstSharedPtr odometry_ptr_;
  sensor_msgs::msg::PointCloud2::ConstSharedPtr pointcloud_ptr_;
  PredictedObjects::ConstSharedPtr object_ptr_;
  OperationModeState::ConstSharedPtr operation_mode_ptr_;
  rclcpp::Time last_obstacle_found_stamp_;
  std::shared_ptr<PredictedObjects> filtered_object_ptr_;
  std::vector<TimestampedObject> observed_objects_;
  std::vector<TimestampedObject> ignored_objects_;

  // Diagnostic Updater
  diagnostic_updater::Updater updater_;
};
}  // namespace autoware::collision_detector

#endif  // AUTOWARE__COLLISION_DETECTOR__NODE_HPP_

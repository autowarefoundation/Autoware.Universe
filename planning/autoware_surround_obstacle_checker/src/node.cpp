// Copyright 2020 Tier IV, Inc.
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

#include "node.hpp"

#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/ros/update_param.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <autoware/universe_utils/transform/transforms.hpp>

#include <boost/assert.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <pcl/common/transforms.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>

#include <optional>
#include <utility>
#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>
#endif

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>
#include <string>

#define EIGEN_MPL2_ONLY
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace autoware::surround_obstacle_checker
{
namespace bg = boost::geometry;
using Point2d = bg::model::d2::point_xy<double>;
using Polygon2d = bg::model::polygon<Point2d>;
using autoware::universe_utils::createPoint;
using autoware::universe_utils::pose2transform;
using autoware_perception_msgs::msg::ObjectClassification;

SurroundObstacleCheckerNode::SurroundObstacleCheckerNode(const rclcpp::NodeOptions & node_options)
: Node("surround_obstacle_checker_node", node_options)
{
  label_map_ = {
    {ObjectClassification::UNKNOWN, "unknown"}, {ObjectClassification::CAR, "car"},
    {ObjectClassification::TRUCK, "truck"},     {ObjectClassification::BUS, "bus"},
    {ObjectClassification::TRAILER, "trailer"}, {ObjectClassification::MOTORCYCLE, "motorcycle"},
    {ObjectClassification::BICYCLE, "bicycle"}, {ObjectClassification::PEDESTRIAN, "pedestrian"}};
  // Parameters
  {
    param_listener_ = std::make_shared<surround_obstacle_checker_node::ParamListener>(
      this->get_node_parameters_interface());

    logger_configure_ = std::make_unique<autoware::universe_utils::LoggerLevelConfigure>(this);
  }

  vehicle_info_ = autoware::vehicle_info_utils::VehicleInfoUtils(*this).getVehicleInfo();

  // Publishers
  pub_clear_velocity_limit_ = this->create_publisher<VelocityLimitClearCommand>(
    "~/output/velocity_limit_clear_command", rclcpp::QoS{1}.transient_local());
  pub_velocity_limit_ = this->create_publisher<VelocityLimit>(
    "~/output/max_velocity", rclcpp::QoS{1}.transient_local());
  pub_processing_time_ =
    this->create_publisher<tier4_debug_msgs::msg::Float64Stamped>("~/debug/processing_time_ms", 1);

  using std::chrono_literals::operator""ms;
  timer_ = rclcpp::create_timer(
    this, get_clock(), 100ms, std::bind(&SurroundObstacleCheckerNode::onTimer, this));

  // Stop Checker
  vehicle_stop_checker_ = std::make_unique<VehicleStopChecker>(this);

  // Debug
  odometry_ptr_ = std::make_shared<nav_msgs::msg::Odometry>();
  {
    const auto param = param_listener_->get_params();
    const auto check_distances = getCheckDistances(param.debug_footprint_label);
    debug_ptr_ = std::make_shared<SurroundObstacleCheckerDebugNode>(
      vehicle_info_, param.debug_footprint_label, check_distances.at(0), check_distances.at(1),
      check_distances.at(2), param.surround_check_hysteresis_distance, odometry_ptr_->pose.pose,
      this->get_clock(), *this);
  }
}

std::array<double, 3> SurroundObstacleCheckerNode::getCheckDistances(
  const std::string & str_label) const
{
  const auto param = param_listener_->get_params();
  const auto & obstacle_param = param.obstacle_types_map.at(str_label);
  return {
    obstacle_param.surround_check_front_distance, obstacle_param.surround_check_side_distance,
    obstacle_param.surround_check_back_distance};
}

bool SurroundObstacleCheckerNode::getUseDynamicObject() const
{
  const auto param = param_listener_->get_params();
  bool use_dynamic_object = false;
  for (const auto & label_pair : label_map_) {
    use_dynamic_object |= param.object_types_map.at(label_pair.second).enable_check;
  }
  return use_dynamic_object;
}

void SurroundObstacleCheckerNode::onTimer()
{
  autoware::universe_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  stop_watch.tic();

  odometry_ptr_ = sub_odometry_.takeData();
  pointcloud_ptr_ = sub_pointcloud_.takeData();
  object_ptr_ = sub_dynamic_objects_.takeData();

  if (!odometry_ptr_) {
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 5000 /* ms */, "waiting for current velocity...");
    return;
  }

  const auto param = param_listener_->get_params();
  const auto use_dynamic_object = getUseDynamicObject();

  if (param.publish_debug_footprints) {
    debug_ptr_->publishFootprints();
  }

  if (param.pointcloud.enable_check && !pointcloud_ptr_) {
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 5000 /* ms */, "waiting for pointcloud info...");
  }

  if (use_dynamic_object && !object_ptr_) {
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 5000 /* ms */, "waiting for dynamic object info...");
  }

  if (!param.pointcloud.enable_check && !use_dynamic_object) {
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 5000 /* ms */,
      "Surround obstacle check is disabled for all dynamic object types and for pointcloud check.");
  }

  const auto nearest_obstacle = getNearestObstacle();
  const auto is_vehicle_stopped = vehicle_stop_checker_->isVehicleStopped();

  constexpr double epsilon = 1e-3;
  switch (state_) {
    case State::PASS: {
      const auto is_obstacle_found =
        !nearest_obstacle ? false : nearest_obstacle.value().first < epsilon;

      bool is_stop_required = false;
      std::tie(is_stop_required, last_obstacle_found_time_) = isStopRequired(
        is_obstacle_found, is_vehicle_stopped, state_, last_obstacle_found_time_,
        param.state_clear_time);
      if (!is_stop_required) {
        break;
      }

      state_ = State::STOP;

      auto velocity_limit = std::make_shared<VelocityLimit>();
      velocity_limit->stamp = this->now();
      velocity_limit->max_velocity = 0.0;
      velocity_limit->use_constraints = false;
      velocity_limit->sender = "surround_obstacle_checker";

      pub_velocity_limit_->publish(*velocity_limit);

      // do not start when there is a obstacle near the ego vehicle.
      RCLCPP_WARN(get_logger(), "do not start because there is obstacle near the ego vehicle.");

      break;
    }

    case State::STOP: {
      const auto is_obstacle_found = !nearest_obstacle ? false
                                                       : nearest_obstacle.value().first <
                                                           param.surround_check_hysteresis_distance;

      bool is_stop_required = false;
      std::tie(is_stop_required, last_obstacle_found_time_) = isStopRequired(
        is_obstacle_found, is_vehicle_stopped, state_, last_obstacle_found_time_,
        param.state_clear_time);
      if (is_stop_required) {
        break;
      }

      state_ = State::PASS;

      auto velocity_limit_clear_command = std::make_shared<VelocityLimitClearCommand>();
      velocity_limit_clear_command->stamp = this->now();
      velocity_limit_clear_command->command = true;
      velocity_limit_clear_command->sender = "surround_obstacle_checker";

      pub_clear_velocity_limit_->publish(*velocity_limit_clear_command);

      break;
    }

    default:
      break;
  }

  if (nearest_obstacle) {
    debug_ptr_->pushObstaclePoint(nearest_obstacle.value().second, PointType::NoStart);
  }

  if (state_ == State::STOP) {
    debug_ptr_->pushPose(odometry_ptr_->pose.pose, PoseType::NoStart);
  }

  tier4_debug_msgs::msg::Float64Stamped processing_time_msg;
  processing_time_msg.stamp = get_clock()->now();
  processing_time_msg.data = stop_watch.toc();
  pub_processing_time_->publish(processing_time_msg);

  debug_ptr_->publish();
}

std::optional<Obstacle> SurroundObstacleCheckerNode::getNearestObstacle() const
{
  const auto nearest_pointcloud = getNearestObstacleByPointCloud();
  const auto nearest_object = getNearestObstacleByDynamicObject();
  if (!nearest_pointcloud && !nearest_object) {
    return {};
  }

  if (!nearest_pointcloud) {
    return nearest_object;
  }

  if (!nearest_object) {
    return nearest_pointcloud;
  }

  return nearest_pointcloud.value().first < nearest_object.value().first ? nearest_pointcloud
                                                                         : nearest_object;
}

std::optional<Obstacle> SurroundObstacleCheckerNode::getNearestObstacleByPointCloud() const
{
  const auto param = param_listener_->get_params();

  if (!param.pointcloud.enable_check || !pointcloud_ptr_) {
    return std::nullopt;
  }

  if (pointcloud_ptr_->data.empty()) {
    return std::nullopt;
  }

  const auto transform_stamped =
    getTransform("base_link", pointcloud_ptr_->header.frame_id, pointcloud_ptr_->header.stamp, 0.5);

  if (!transform_stamped) {
    return std::nullopt;
  }

  Eigen::Affine3f isometry =
    tf2::transformToEigen(transform_stamped.value().transform).cast<float>();
  pcl::PointCloud<pcl::PointXYZ> transformed_pointcloud;
  pcl::fromROSMsg(*pointcloud_ptr_, transformed_pointcloud);
  autoware::universe_utils::transformPointCloud(
    transformed_pointcloud, transformed_pointcloud, isometry);

  const auto & pointcloud_param = param.obstacle_types_map.at("pointcloud");
  const double front_margin = pointcloud_param.surround_check_front_distance;
  const double side_margin = pointcloud_param.surround_check_side_distance;
  const double back_margin = pointcloud_param.surround_check_back_distance;
  const double base_to_front = vehicle_info_.max_longitudinal_offset_m + front_margin;
  const double base_to_rear = vehicle_info_.rear_overhang_m + back_margin;
  const double width = vehicle_info_.vehicle_width_m + side_margin * 2;
  const auto ego_polygon = autoware::universe_utils::toFootprint(
    odometry_ptr_->pose.pose, base_to_front, base_to_rear, width);

  geometry_msgs::msg::Point nearest_point;
  double minimum_distance = std::numeric_limits<double>::max();
  bool was_minimum_distance_updated = false;
  for (const auto & p : transformed_pointcloud) {
    Point2d boost_point(p.x, p.y);

    const auto distance_to_object = bg::distance(ego_polygon, boost_point);

    if (distance_to_object < minimum_distance) {
      nearest_point = createPoint(p.x, p.y, p.z);
      minimum_distance = distance_to_object;
      was_minimum_distance_updated = true;
    }
  }

  if (was_minimum_distance_updated) {
    return std::make_pair(minimum_distance, nearest_point);
  }
  return std::nullopt;
}

std::optional<Obstacle> SurroundObstacleCheckerNode::getNearestObstacleByDynamicObject() const
{
  if (!object_ptr_ || !getUseDynamicObject()) return std::nullopt;

  const auto param = param_listener_->get_params();

  // TODO(murooka) check computation cost
  geometry_msgs::msg::Point nearest_point;
  double minimum_distance = std::numeric_limits<double>::max();
  bool was_minimum_distance_updated = false;
  for (const auto & object : object_ptr_->objects) {
    const auto & object_pose = object.kinematics.initial_pose_with_covariance.pose;
    const int label = object.classification.front().label;
    const auto & str_label = label_map_.at(label);

    if (!param.object_types_map.at(str_label).enable_check) {
      continue;
    }
    const auto & object_param = param.obstacle_types_map.at(str_label);
    const double front_margin = object_param.surround_check_front_distance;
    const double side_margin = object_param.surround_check_side_distance;
    const double back_margin = object_param.surround_check_back_distance;
    const double base_to_front = vehicle_info_.max_longitudinal_offset_m + front_margin;
    const double base_to_rear = vehicle_info_.rear_overhang_m + back_margin;
    const double width = vehicle_info_.vehicle_width_m + side_margin * 2;
    const auto ego_polygon = autoware::universe_utils::toFootprint(
      odometry_ptr_->pose.pose, base_to_front, base_to_rear, width);

    const auto object_polygon = autoware::universe_utils::toPolygon2d(object);

    const auto distance_to_object = bg::distance(ego_polygon, object_polygon);

    if (distance_to_object < minimum_distance) {
      nearest_point = object_pose.position;
      minimum_distance = distance_to_object;
      was_minimum_distance_updated = true;
    }
  }

  if (was_minimum_distance_updated) {
    return std::make_pair(minimum_distance, nearest_point);
  }
  return std::nullopt;
}

std::optional<geometry_msgs::msg::TransformStamped> SurroundObstacleCheckerNode::getTransform(
  const std::string & source, const std::string & target, const rclcpp::Time & stamp,
  double duration_sec) const
{
  geometry_msgs::msg::TransformStamped transform_stamped;

  try {
    transform_stamped =
      tf_buffer_.lookupTransform(source, target, stamp, tf2::durationFromSec(duration_sec));
  } catch (const tf2::TransformException & ex) {
    return {};
  }

  return transform_stamped;
}

auto SurroundObstacleCheckerNode::isStopRequired(
  const bool is_obstacle_found, const bool is_vehicle_stopped, const State & state,
  const std::optional<rclcpp::Time> & last_obstacle_found_time, const double time_threshold) const
  -> std::pair<bool, std::optional<rclcpp::Time>>
{
  if (!is_vehicle_stopped) {
    return std::make_pair(false, std::nullopt);
  }

  if (is_obstacle_found) {
    return std::make_pair(true, this->now());
  }

  if (state != State::STOP) {
    return std::make_pair(false, std::nullopt);
  }

  // Keep stop state
  if (last_obstacle_found_time.has_value()) {
    const auto elapsed_time = this->now() - last_obstacle_found_time.value();
    if (elapsed_time.seconds() <= time_threshold) {
      return std::make_pair(true, last_obstacle_found_time.value());
    }
  }

  return std::make_pair(false, std::nullopt);
}

}  // namespace autoware::surround_obstacle_checker

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::surround_obstacle_checker::SurroundObstacleCheckerNode)

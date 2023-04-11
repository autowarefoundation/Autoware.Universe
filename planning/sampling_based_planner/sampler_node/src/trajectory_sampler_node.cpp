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

#include "sampler_node/trajectory_sampler_node.hpp"

#include "Eigen/src/Core/Matrix.h"
#include "frenet_planner/structures.hpp"
#include "lanelet2_core/LaneletMap.h"
#include "sampler_common/constraints/footprint.hpp"
#include "sampler_common/constraints/soft_constraint.hpp"
#include "sampler_common/structures.hpp"
#include "sampler_common/trajectory_reuse.hpp"
#include "sampler_common/transform/spline_transform.hpp"
#include "sampler_node/prepare_inputs.hpp"
#include "sampler_node/trajectory_generation.hpp"
#include "sampler_node/utils/occupancy_grid_to_polygons.hpp"

#include <lanelet2_extension/utility/message_conversion.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/time.hpp>
#include <rclcpp/utilities.hpp>
#include <vehicle_info_util/vehicle_info_util.hpp>

#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <boost/geometry/algorithms/distance.hpp>

#include <lanelet2_routing/RoutingGraph.h>
#include <lanelet2_traffic_rules/TrafficRules.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

namespace sampler_node
{
TrajectorySamplerNode::TrajectorySamplerNode(const rclcpp::NodeOptions & node_options)
: Node("trajectory_sampler_node", node_options),
  tf_buffer_(this->get_clock()),
  tf_listener_(tf_buffer_),
  vehicle_info_(vehicle_info_util::VehicleInfoUtil(*this).getVehicleInfo())
{
  trajectory_pub_ =
    create_publisher<autoware_auto_planning_msgs::msg::Trajectory>("~/output/trajectory", 1);

  path_sub_ = create_subscription<autoware_auto_planning_msgs::msg::Path>(
    "~/input/path", rclcpp::QoS{1},
    std::bind(&TrajectorySamplerNode::pathCallback, this, std::placeholders::_1));
  // steer_sub_ = create_subscription<autoware_auto_vehicle_msgs::msg::SteeringReport>(
  //   "~/input/steer", rclcpp::QoS{1},
  //   std::bind(&TrajectorySamplerNode::steerCallback, this, std::placeholders::_1));
  objects_sub_ = create_subscription<autoware_auto_perception_msgs::msg::PredictedObjects>(
    "~/input/objects", rclcpp::QoS{10},
    std::bind(&TrajectorySamplerNode::objectsCallback, this, std::placeholders::_1));
  map_sub_ = create_subscription<autoware_auto_mapping_msgs::msg::HADMapBin>(
    "~/input/vector_map", rclcpp::QoS{1}.transient_local(),
    std::bind(&TrajectorySamplerNode::mapCallback, this, std::placeholders::_1));
  route_sub_ = create_subscription<autoware_auto_planning_msgs::msg::HADMapRoute>(
    "~/input/route", rclcpp::QoS{1}.transient_local(),
    std::bind(&TrajectorySamplerNode::routeCallback, this, std::placeholders::_1));
  fallback_sub_ = create_subscription<autoware_auto_planning_msgs::msg::Trajectory>(
    "~/input/fallback", rclcpp::QoS{1},
    std::bind(&TrajectorySamplerNode::fallbackCallback, this, std::placeholders::_1));
  vel_sub_ = create_subscription<nav_msgs::msg::Odometry>(
    "~/input/velocity", rclcpp::QoS{1}, [&](nav_msgs::msg::Odometry::ConstSharedPtr msg) {
      velocities_.push_back(msg->twist.twist.linear.x);
    });
  acc_sub_ = create_subscription<geometry_msgs::msg::AccelWithCovarianceStamped>(
    "~/input/acceleration", rclcpp::QoS{1},
    [&](geometry_msgs::msg::AccelWithCovarianceStamped::ConstSharedPtr msg) {
      accelerations_.push_back(msg->accel.accel.linear.x);
    });

  in_objects_ptr_ = std::make_unique<autoware_auto_perception_msgs::msg::PredictedObjects>();

  fallback_timeout_ = declare_parameter<double>("fallback_trajectory_timeout");
  params_.constraints.hard.max_curvature =
    declare_parameter<double>("constraints.hard.max_curvature");
  params_.constraints.hard.min_curvature =
    declare_parameter<double>("constraints.hard.min_curvature");
  params_.constraints.hard.max_acceleration =
    declare_parameter<double>("constraints.hard.max_acceleration");
  params_.constraints.hard.min_acceleration =
    declare_parameter<double>("constraints.hard.min_acceleration");
  params_.constraints.hard.max_velocity =
    declare_parameter<double>("constraints.hard.max_velocity");
  params_.constraints.hard.max_yaw_rate =
    declare_parameter<double>("constraints.hard.max_yaw_rate");
  params_.constraints.hard.min_velocity =
    declare_parameter<double>("constraints.hard.min_velocity");
  params_.constraints.collision_distance_buffer =
    declare_parameter<double>("constraints.collision_distance_buffer");
  params_.constraints.static_dynamic_obstacle_velocity_threshold =
    declare_parameter<double>("constraints.static_dynamic_obstacle_velocity_threshold");
  params_.constraints.soft.lateral_deviation_weight =
    declare_parameter<double>("constraints.soft.lateral_deviation_weight");
  params_.constraints.soft.longitudinal_deviation_weight =
    declare_parameter<double>("constraints.soft.longitudinal_deviation_weight");
  params_.constraints.soft.jerk_weight = declare_parameter<double>("constraints.soft.jerk_weight");
  params_.constraints.soft.length_weight =
    declare_parameter<double>("constraints.soft.length_weight");
  params_.constraints.soft.curvature_weight =
    declare_parameter<double>("constraints.soft.curvature_weight");
  params_.constraints.soft.velocity_weight =
    declare_parameter<double>("constraints.soft.velocity_weight");
  params_.constraints.soft.yaw_rate_weight =
    declare_parameter<double>("constraints.soft.yaw_rate_weight");
  params_.sampling.enable_frenet = declare_parameter<bool>("sampling.enable_frenet");
  params_.sampling.enable_bezier = declare_parameter<bool>("sampling.enable_bezier");
  params_.sampling.resolution = declare_parameter<double>("sampling.resolution");
  params_.preprocessing.reuse_times =
    declare_parameter<std::vector<double>>("preprocessing.reuse_times");
  params_.preprocessing.reuse_max_deviation =
    declare_parameter<double>("preprocessing.reuse_max_deviation");
  params_.sampling.frenet.manual.enable = declare_parameter<bool>("sampling.frenet.manual.enable");
  params_.sampling.frenet.manual.target_durations =
    declare_parameter<std::vector<double>>("sampling.frenet.manual.target_durations");
  params_.sampling.frenet.manual.target_longitudinal_position_offsets =
    declare_parameter<std::vector<double>>(
      "sampling.frenet.manual.target_longitudinal_position_offsets");
  params_.sampling.frenet.manual.target_longitudinal_velocities =
    declare_parameter<std::vector<double>>("sampling.frenet.manual.target_longitudinal_velocities");
  params_.sampling.frenet.manual.target_longitudinal_accelerations =
    declare_parameter<std::vector<double>>(
      "sampling.frenet.manual.target_longitudinal_accelerations");
  params_.sampling.frenet.manual.target_lateral_positions =
    declare_parameter<std::vector<double>>("sampling.frenet.manual.target_lateral_positions");
  params_.sampling.frenet.manual.target_lateral_velocities =
    declare_parameter<std::vector<double>>("sampling.frenet.manual.target_lateral_velocities");
  params_.sampling.frenet.manual.target_lateral_accelerations =
    declare_parameter<std::vector<double>>("sampling.frenet.manual.target_lateral_accelerations");
  params_.sampling.frenet.calc.enable = declare_parameter<bool>("sampling.frenet.calc.enable");
  params_.sampling.frenet.calc.target_longitudinal_velocity_samples =
    declare_parameter<int>("sampling.frenet.calc.target_longitudinal_velocity_samples");
  params_.sampling.bezier.nb_k = declare_parameter<int>("sampling.bezier.nb_k");
  params_.sampling.bezier.mk_min = declare_parameter<double>("sampling.bezier.mk_min");
  params_.sampling.bezier.mk_max = declare_parameter<double>("sampling.bezier.mk_max");
  params_.sampling.bezier.nb_t = declare_parameter<int>("sampling.bezier.nb_t");
  params_.sampling.bezier.mt_min = declare_parameter<double>("sampling.bezier.mt_min");
  params_.sampling.bezier.mt_max = declare_parameter<double>("sampling.bezier.mt_max");
  params_.preprocessing.force_zero_deviation =
    declare_parameter<bool>("preprocessing.force_zero_initial_deviation");
  params_.preprocessing.force_zero_heading =
    declare_parameter<bool>("preprocessing.force_zero_initial_heading");
  params_.preprocessing.smooth_reference =
    declare_parameter<bool>("preprocessing.smooth_reference_trajectory.enable");
  params_.preprocessing.control_points_ratio =
    declare_parameter<double>("preprocessing.smooth_reference_trajectory.control_points_ratio");
  params_.preprocessing.smooth_weight =
    declare_parameter<double>("preprocessing.smooth_reference_trajectory.smoothing_weight");
  params_.preprocessing.desired_traj_behind_length =
    declare_parameter<double>("preprocessing.desired_traj_behind_length");

  // const auto half_wheel_tread = vehicle_info_.wheel_tread_m / 2.0;
  const auto left_offset = vehicle_info_.vehicle_width_m / 2.0;
  const auto right_offset = -vehicle_info_.vehicle_width_m / 2.0;
  // const auto right_offset = -(half_wheel_tread + vehicle_info_.right_overhang_m);
  const auto rear_offset = -vehicle_info_.rear_overhang_m;
  const auto front_offset = vehicle_info_.wheel_base_m + vehicle_info_.front_overhang_m;
  params_.constraints.vehicle_offsets.left_rear = Eigen::Vector2d(rear_offset, left_offset);
  params_.constraints.vehicle_offsets.left_front = Eigen::Vector2d(front_offset, left_offset);
  params_.constraints.vehicle_offsets.right_rear = Eigen::Vector2d(rear_offset, right_offset);
  params_.constraints.vehicle_offsets.right_front = Eigen::Vector2d(front_offset, right_offset);

  set_param_res_ =
    add_on_set_parameters_callback([this](const auto & params) { return onParameter(params); });
  // This is necessary to interact with the GUI even when we are not generating trajectories
  gui_process_timer_ =
    create_wall_timer(std::chrono::milliseconds(100), [this]() { gui_.update(); });
}

rcl_interfaces::msg::SetParametersResult TrajectorySamplerNode::onParameter(
  const std::vector<rclcpp::Parameter> & parameters)
{
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;
  for (const auto & parameter : parameters) {
    if (parameter.get_name() == "fallback_trajectory_timeout") {
      fallback_timeout_ = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.max_curvature") {
      params_.constraints.hard.max_curvature = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.min_curvature") {
      params_.constraints.hard.min_curvature = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.max_acceleration") {
      params_.constraints.hard.max_acceleration = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.min_acceleration") {
      params_.constraints.hard.min_acceleration = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.max_velocity") {
      params_.constraints.hard.max_velocity = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.max_yaw_rate") {
      params_.constraints.hard.max_yaw_rate = parameter.as_double();
    } else if (parameter.get_name() == "constraints.collision_distance_buffer") {
      params_.constraints.collision_distance_buffer = parameter.as_double();
    } else if (parameter.get_name() == "constraints.static_dynamic_obstacle_velocity_threshold") {
      params_.constraints.static_dynamic_obstacle_velocity_threshold = parameter.as_double();
    } else if (parameter.get_name() == "constraints.hard.min_velocity") {
      params_.constraints.hard.min_velocity = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.lateral_deviation_weight") {
      params_.constraints.soft.lateral_deviation_weight = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.longitudinal_deviation_weight") {
      params_.constraints.soft.longitudinal_deviation_weight = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.jerk_weight") {
      params_.constraints.soft.jerk_weight = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.length_weight") {
      params_.constraints.soft.length_weight = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.velocity_weight") {
      params_.constraints.soft.velocity_weight = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.yaw_rate_weight") {
      params_.constraints.soft.yaw_rate_weight = parameter.as_double();
    } else if (parameter.get_name() == "constraints.soft.curvature_weight") {
      params_.constraints.soft.curvature_weight = parameter.as_double();
    } else if (parameter.get_name() == "sampling.enable_frenet") {
      params_.sampling.enable_frenet = parameter.as_bool();
    } else if (parameter.get_name() == "sampling.enable_bezier") {
      params_.sampling.enable_bezier = parameter.as_bool();
    } else if (parameter.get_name() == "sampling.resolution") {
      params_.sampling.resolution = parameter.as_double();
    } else if (parameter.get_name() == "preprocessing.reuse_times") {
      params_.preprocessing.reuse_times = parameter.as_double_array();
    } else if (parameter.get_name() == "preprocessing.reuse_max_deviation") {
      params_.preprocessing.reuse_max_deviation = parameter.as_double();
    } else if (parameter.get_name() == "sampling.frenet.manual.enable") {
      params_.sampling.frenet.manual.enable = parameter.as_bool();
    } else if (parameter.get_name() == "sampling.frenet.manual.target_durations") {
      params_.sampling.frenet.manual.target_durations = parameter.as_double_array();
    } else if (
      parameter.get_name() == "sampling.frenet.manual.target_longitudinal_position_offsets") {
      params_.sampling.frenet.manual.target_longitudinal_position_offsets =
        parameter.as_double_array();
    } else if (parameter.get_name() == "sampling.frenet.target_longitudinal_velocities") {
      params_.sampling.frenet.manual.target_longitudinal_velocities = parameter.as_double_array();
    } else if (parameter.get_name() == "sampling.frenet.manual.target_longitudinal_accelerations") {
      params_.sampling.frenet.manual.target_longitudinal_accelerations =
        parameter.as_double_array();
    } else if (parameter.get_name() == "sampling.frenet.manual.target_lateral_positions") {
      params_.sampling.frenet.manual.target_lateral_positions = parameter.as_double_array();
    } else if (parameter.get_name() == "sampling.frenet.manual.target_lateral_velocities") {
      params_.sampling.frenet.manual.target_lateral_velocities = parameter.as_double_array();
    } else if (parameter.get_name() == "sampling.frenet.manual.target_lateral_accelerations") {
      params_.sampling.frenet.manual.target_lateral_accelerations = parameter.as_double_array();
    } else if (parameter.get_name() == "sampling.frenet.calc.enable") {
      params_.sampling.frenet.calc.enable = parameter.as_bool();
    } else if (
      parameter.get_name() == "sampling.frenet.calc.target_longitudinal_velocity_samples") {
      params_.sampling.frenet.calc.target_longitudinal_velocity_samples = parameter.as_int();
    } else if (parameter.get_name() == "sampling.bezier.nb_k") {
      params_.sampling.bezier.nb_k = parameter.as_int();
    } else if (parameter.get_name() == "sampling.bezier.mk_min") {
      params_.sampling.bezier.mk_min = parameter.as_double();
    } else if (parameter.get_name() == "sampling.bezier.mk_max") {
      params_.sampling.bezier.mk_max = parameter.as_double();
    } else if (parameter.get_name() == "sampling.bezier.nb_t") {
      params_.sampling.bezier.nb_t = parameter.as_int();
    } else if (parameter.get_name() == "sampling.bezier.mt_min") {
      params_.sampling.bezier.mt_min = parameter.as_double();
    } else if (parameter.get_name() == "sampling.bezier.mt_max") {
      params_.sampling.bezier.mt_max = parameter.as_double();
    } else if (parameter.get_name() == "preprocessing.force_zero_initial_deviation") {
      params_.preprocessing.force_zero_deviation = parameter.as_bool();
    } else if (parameter.get_name() == "preprocessing.force_zero_initial_heading") {
      params_.preprocessing.force_zero_heading = parameter.as_bool();
    } else if (parameter.get_name() == "preprocessing.smooth_reference_trajectory.enable") {
      params_.preprocessing.smooth_reference = parameter.as_bool();
    } else if (
      parameter.get_name() == "preprocessing.smooth_reference_trajectory.control_points_ratio") {
      params_.preprocessing.control_points_ratio = parameter.as_double();
    } else if (
      parameter.get_name() == "preprocessing.smooth_reference_trajectory.smoothing_weight") {
      params_.preprocessing.smooth_weight = parameter.as_double();
    } else if (parameter.get_name() == "preprocessing.desired_traj_behind_length") {
      params_.preprocessing.desired_traj_behind_length = parameter.as_double();
    } else {
      RCLCPP_WARN(get_logger(), "Unknown parameter %s", parameter.get_name().c_str());
      result.successful = false;
    }
  }
  return result;
}

void TrajectorySamplerNode::pathCallback(
  const autoware_auto_planning_msgs::msg::Path::ConstSharedPtr msg)
{
  const auto calc_begin = std::chrono::steady_clock::now();
  const auto current_state = getCurrentEgoConfiguration();
  // TODO(Maxime CLEMENT): move to "validInputs(current_state, msg)"
  if (msg->points.size() < 2 || msg->drivable_area.data.empty() || !current_state) {
    RCLCPP_INFO(
      get_logger(),
      "[pathCallback] incomplete inputs: current_state: %d | drivable_area: %d | path points: %ld",
      current_state.has_value(), !msg->drivable_area.data.empty(), msg->points.size());
    return;
  }

  const auto path_spline = preparePathSpline(*msg, params_);
  const auto planning_configuration = getPlanningConfiguration(*current_state, path_spline);
  prepareConstraints(
    params_.constraints, *in_objects_ptr_, *lanelet_map_ptr_, drivable_ids_, prefered_ids_,
    msg->drivable_area);
  params_.constraints.distance_to_end = path_spline.lastS() - planning_configuration.frenet.s;
  gui_.setConstraints(params_.constraints);

  const auto updated_prev_traj = updatePreviousTrajectory(
    prev_traj_, planning_configuration, params_.preprocessing.reuse_max_deviation,
    params_.preprocessing.desired_traj_behind_length);
  auto reusable_trajectories = sampler_common::calculateReusableTrajectories(
    updated_prev_traj, params_.preprocessing.reuse_times);
  gui_.setReusableTrajectories(reusable_trajectories);
  auto trajectories =
    generateCandidateTrajectories(planning_configuration, {}, path_spline, *msg, gui_, params_);
  for (auto & reusable_traj : reusable_trajectories) {
    auto trajs = generateCandidateTrajectories(
      reusable_traj.planning_configuration, reusable_traj.trajectory, path_spline, *msg, gui_,
      params_);
    trajectories.insert(
      trajectories.end(), std::make_move_iterator(trajs.begin()),
      std::make_move_iterator(trajs.end()));
  }
  for (auto & trajectory : trajectories) {
    auto resampled_traj = trajectory.resampleTimeFromZero(0.1);
    sampler_common::constraints::checkHardConstraints(resampled_traj, params_.constraints);
    trajectory.constraint_results = resampled_traj.constraint_results;
    sampler_common::constraints::calculateCost(resampled_traj, params_.constraints, path_spline);
    trajectory.cost = resampled_traj.cost;
  }
  const auto selected_trajectory_idx = selectBestTrajectory(trajectories);
  if (selected_trajectory_idx) {
    const auto & selected_trajectory = trajectories[*selected_trajectory_idx];
    auto output_trajectory = selected_trajectory;
    // Make the trajectory nicer for the controller
    // TODO(Maxime CLEMENT): 0.25m/s is the min engage velocity. Should be a parameter.
    constexpr auto min_engage_vel = 0.25;
    if (
      output_trajectory.longitudinal_velocities.size() > 1lu &&
      *std::max_element(
        output_trajectory.longitudinal_velocities.begin(),
        output_trajectory.longitudinal_velocities.end()) > min_engage_vel) {
      for (auto i = 0lu; i < output_trajectory.longitudinal_velocities.size() &&
                         output_trajectory.longitudinal_velocities[i] < min_engage_vel;
           ++i)
        output_trajectory.longitudinal_velocities[i] = min_engage_vel;
      // std::cout << "[prependTrajectory] updated first velocity points to "
      //          << output_trajectory.longitudinal_velocities.front() << "m/s\n";
    }
    if (!output_trajectory.points.empty())
      publishTrajectory(output_trajectory, msg->header.frame_id);
    prev_traj_ = selected_trajectory;
  } else {
    if (
      fallback_traj_ptr_ &&
      (now() - fallback_traj_ptr_->header.stamp).seconds() < fallback_timeout_) {
      RCLCPP_WARN(get_logger(), "Using fallback trajectory");
      trajectory_pub_->publish(*fallback_traj_ptr_);
      prev_traj_.clear();
    } else {
      if (!prev_traj_.points.empty()) {
        std::fill(
          prev_traj_.longitudinal_velocities.begin(), prev_traj_.longitudinal_velocities.end(),
          0.0);
        publishTrajectory(prev_traj_, msg->header.frame_id);
      }
    }
  }
  const auto calc_end = std::chrono::steady_clock::now();
  const auto gui_begin = std::chrono::steady_clock::now();
  gui_.setInputs(*msg, path_spline, *current_state);
  gui_.setOutputs(trajectories, selected_trajectory_idx, prev_traj_);
  gui_.update();
  const auto gui_end = std::chrono::steady_clock::now();
  const auto calc_time_ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(calc_end - calc_begin).count();
  const auto gui_time_ms =
    std::chrono::duration_cast<std::chrono::milliseconds>(gui_end - gui_begin).count();
  std::printf(" runtime = %ldms | gui time = %ldms", calc_time_ms, gui_time_ms);
  std::cout << std::endl;
  gui_.setPerformances(trajectories.size(), calc_time_ms, gui_time_ms);
}

std::optional<size_t> TrajectorySamplerNode::selectBestTrajectory(
  const std::vector<sampler_common::Trajectory> & trajectories)
{
  auto min_cost = std::numeric_limits<double>::max();
  std::optional<size_t> best_trajectory_idx;
  for (size_t i = 0; i < trajectories.size(); ++i) {
    const auto & traj = trajectories[i];
    if (traj.constraint_results.isValid() && traj.cost < min_cost) {
      min_cost = traj.cost;
      best_trajectory_idx = i;
    }
  }
  return best_trajectory_idx;
}

std::optional<sampler_common::Configuration> TrajectorySamplerNode::getCurrentEgoConfiguration()
{
  geometry_msgs::msg::TransformStamped tf_current_pose;

  try {
    tf_current_pose = tf_buffer_.lookupTransform(
      "map", "base_link", rclcpp::Time(0), rclcpp::Duration::from_seconds(1.0));
  } catch (tf2::TransformException & ex) {
    RCLCPP_ERROR(get_logger(), "[TrajectorySamplerNode] %s", ex.what());
    return {};
  }

  auto config = sampler_common::Configuration();
  config.pose = {tf_current_pose.transform.translation.x, tf_current_pose.transform.translation.y};
  config.heading = tf2::getYaw(tf_current_pose.transform.rotation);
  if (!points_.empty()) {
    const auto dist = boost::geometry::distance(config.pose, points_.back());
    if (dist > 2.0) {
      points_.clear();
      yaws_.clear();
    } else if (dist > 0.1) {
      points_.push_back(config.pose);
      yaws_.push_back(config.heading);
    }
  } else {
    points_.push_back(config.pose);
    yaws_.push_back(config.heading);
  }
  // TODO(Maxime CLEMENT): use a proper filter from the signal_processing lib
  config.velocity =
    std::accumulate(velocities_.begin(), velocities_.end(), 0.0) / velocities_.size();
  config.acceleration =
    std::accumulate(accelerations_.begin(), accelerations_.end(), 0.0) / accelerations_.size();
  return config;
}

void TrajectorySamplerNode::objectsCallback(
  const autoware_auto_perception_msgs::msg::PredictedObjects::ConstSharedPtr msg)
{
  in_objects_ptr_ = std::make_unique<autoware_auto_perception_msgs::msg::PredictedObjects>(*msg);
}

void TrajectorySamplerNode::publishTrajectory(
  const sampler_common::Trajectory & trajectory, const std::string & frame_id)
{
  if (trajectory.points.size() < 2) return;
  const auto t = trajectory.resample(0.1);
  tf2::Quaternion q;  // to convert yaw angle to Quaternion orientation
  autoware_auto_planning_msgs::msg::Trajectory traj_msg;
  traj_msg.header.frame_id = frame_id;
  traj_msg.header.stamp = now();
  autoware_auto_planning_msgs::msg::TrajectoryPoint point;
  for (size_t i = 0; i < t.points.size(); ++i) {
    point.pose.position.x = t.points[i].x();
    point.pose.position.y = t.points[i].y();
    q.setRPY(0, 0, t.yaws[i]);
    point.pose.orientation = tf2::toMsg(q);
    point.longitudinal_velocity_mps = static_cast<float>(t.longitudinal_velocities[i]);
    point.acceleration_mps2 = static_cast<float>(t.longitudinal_accelerations[i]);
    point.lateral_velocity_mps = static_cast<float>(t.lateral_velocities[i]);
    point.heading_rate_rps = static_cast<float>(point.longitudinal_velocity_mps * t.curvatures[i]);
    point.front_wheel_angle_rad = 0.0f;
    point.rear_wheel_angle_rad = 0.0f;
    traj_msg.points.push_back(point);
  }
  trajectory_pub_->publish(traj_msg);
}

// TODO(Maxime CLEMENT): unused in favor of the Path's drivable area
void TrajectorySamplerNode::mapCallback(
  const autoware_auto_mapping_msgs::msg::HADMapBin::ConstSharedPtr map_msg)
{
  lanelet_map_ptr_ = std::make_shared<lanelet::LaneletMap>();
  lanelet::utils::conversion::fromBinMsg(*map_msg, lanelet_map_ptr_);
}

// TODO(Maxime CLEMENT): unused in favor of the Path's drivable area
void TrajectorySamplerNode::routeCallback(
  const autoware_auto_planning_msgs::msg::HADMapRoute::ConstSharedPtr route_msg)
{
  prefered_ids_.clear();
  drivable_ids_.clear();
  if (lanelet_map_ptr_) {
    for (const auto & segment : route_msg->segments) {
      prefered_ids_.push_back(segment.preferred_primitive_id);
      for (const auto & primitive : segment.primitives) {
        drivable_ids_.push_back(primitive.id);
      }
    }
  }
}

void TrajectorySamplerNode::fallbackCallback(
  const autoware_auto_planning_msgs::msg::Trajectory::ConstSharedPtr fallback_msg)
{
  fallback_traj_ptr_ = fallback_msg;
}

sampler_common::Configuration TrajectorySamplerNode::getPlanningConfiguration(
  sampler_common::Configuration state,
  const sampler_common::transform::Spline2D & path_spline) const
{
  state.frenet = path_spline.frenet(state.pose);
  if (params_.preprocessing.force_zero_deviation) {
    state.pose = path_spline.cartesian(state.frenet.s);
  }
  if (params_.preprocessing.force_zero_heading) {
    state.heading = path_spline.yaw(state.frenet.s);
  }
  state.curvature = path_spline.curvature(state.frenet.s);
  return state;
}

sampler_common::Trajectory TrajectorySamplerNode::prependTrajectory(
  const sampler_common::Trajectory & trajectory,
  const sampler_common::transform::Spline2D & reference,
  const sampler_common::Configuration & current_state) const
{
  if (
    params_.preprocessing.desired_traj_behind_length == 0.0 && !trajectory.times.empty() &&
    trajectory.times.front() > 0.0) {
    sampler_common::Trajectory t;
    t.points = {current_state.pose};
    t.times = {0.0};
    t.lengths = {0.0};
    const auto & vels = trajectory.longitudinal_velocities;
    t.longitudinal_velocities = {vels.empty() ? current_state.velocity : vels.front()};
    t.longitudinal_accelerations = {current_state.acceleration};
    t.curvatures = {current_state.curvature};
    t.yaws = {current_state.heading};
    t.lateral_velocities = {0.0};
    t.lateral_accelerations = {0.0};
    t.jerks = {0.0};
    return t.extend(trajectory);
  }
  // TODO(Maxime): fix reuse/prepend logic. Following code is broken
  if (trajectory.points.empty()) return trajectory;
  // only prepend if the trajectory starts from the current pose
  if (
    !params_.preprocessing.force_zero_deviation &&
    (trajectory.points.front().x() != current_state.pose.x() ||
     trajectory.points.front().y() != current_state.pose.y())) {
    std::cout << "[prependTrajectory] no prepend when 1st traj point is not current pose\n";
    return trajectory;
  }
  const auto current_frenet = reference.frenet(trajectory.points.front());
  const auto resolution = params_.sampling.resolution;
  sampler_common::Trajectory trajectory_to_prepend;
  const auto first_s = current_frenet.s - params_.preprocessing.desired_traj_behind_length;
  const auto min_s =
    std::max(resolution, first_s);  // avoid s=0 where the reference spline may diverge
  std::vector<double> ss;
  if (!params_.preprocessing.force_zero_deviation) {
    auto s = current_frenet.s;
    for (auto i = 1lu; i + 1 < points_.size() && s >= min_s; ++i) {
      const auto & curr = points_[points_.size() - i];
      const auto & prev = points_[points_.size() - i - 1];
      trajectory_to_prepend.points.push_back(curr);
      if (!params_.preprocessing.force_zero_heading)
        trajectory_to_prepend.yaws.push_back(yaws_[points_.size() - i]);
      s -= boost::geometry::distance(curr, prev);
      ss.push_back(s);
    }
    std::reverse(trajectory_to_prepend.points.begin(), trajectory_to_prepend.points.end());
    if (!params_.preprocessing.force_zero_heading)
      std::reverse(trajectory_to_prepend.yaws.begin(), trajectory_to_prepend.yaws.end());
    std::reverse(ss.begin(), ss.end());
  } else {
    for (auto s = min_s; s < current_frenet.s; s += resolution) {
      ss.push_back(s);
    }
  }
  const auto first_vel = trajectory.longitudinal_velocities.front();
  for (const auto s : ss) {
    if (params_.preprocessing.force_zero_deviation)
      trajectory_to_prepend.points.push_back(reference.cartesian(s));
    if (params_.preprocessing.force_zero_heading)
      trajectory_to_prepend.yaws.push_back(reference.yaw(s));
    trajectory_to_prepend.curvatures.push_back(reference.curvature(s));
    trajectory_to_prepend.lengths.push_back(s - ss.front());
    trajectory_to_prepend.longitudinal_velocities.push_back(first_vel);
    trajectory_to_prepend.lateral_velocities.push_back(trajectory.lateral_velocities.front());
    trajectory_to_prepend.longitudinal_accelerations.push_back(
      trajectory.longitudinal_accelerations.front());
    trajectory_to_prepend.lateral_accelerations.push_back(trajectory.lateral_accelerations.front());
    if (first_vel == 0.0)
      trajectory_to_prepend.times.push_back(-(current_frenet.s - s));
    else
      trajectory_to_prepend.times.push_back(-(current_frenet.s - s) / first_vel);
  }
  return trajectory_to_prepend.extend(trajectory);
}
}  // namespace sampler_node

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(sampler_node::TrajectorySamplerNode)

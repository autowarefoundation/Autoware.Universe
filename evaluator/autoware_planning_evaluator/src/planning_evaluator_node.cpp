// Copyright 2021 Tier IV, Inc.
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

#include "autoware/planning_evaluator/planning_evaluator_node.hpp"

#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <nlohmann/json.hpp>

#include <diagnostic_msgs/msg/detail/diagnostic_status__struct.hpp>

#include "boost/lexical_cast.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace planning_diagnostics
{
PlanningEvaluatorNode::PlanningEvaluatorNode(const rclcpp::NodeOptions & node_options)
: Node("planning_evaluator", node_options)
{
  using std::placeholders::_1;
  tf_buffer_ = std::make_unique<tf2_ros::Buffer>(this->get_clock());
  transform_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

  // Timer callback to publish evaluator diagnostics
  using namespace std::literals::chrono_literals;
  timer_ = rclcpp::create_timer(
    this, get_clock(), 100ms, std::bind(&PlanningEvaluatorNode::onTimer, this));
  // Parameters
  metrics_calculator_.parameters.trajectory.min_point_dist_m =
    declare_parameter<double>("trajectory.min_point_dist_m");
  metrics_calculator_.parameters.trajectory.lookahead.max_dist_m =
    declare_parameter<double>("trajectory.lookahead.max_dist_m");
  metrics_calculator_.parameters.trajectory.lookahead.max_time_s =
    declare_parameter<double>("trajectory.lookahead.max_time_s");
  metrics_calculator_.parameters.trajectory.evaluation_time_s =
    declare_parameter<double>("trajectory.evaluation_time_s");
  metrics_calculator_.parameters.obstacle.dist_thr_m =
    declare_parameter<double>("obstacle.dist_thr_m");

  output_metrics_ = declare_parameter<bool>("output_metrics");
  ego_frame_str_ = declare_parameter<std::string>("ego_frame");

  processing_time_pub_ = this->create_publisher<autoware_internal_debug_msgs::msg::Float64Stamped>(
    "~/debug/processing_time_ms", 1);

  // List of metrics to calculate and publish
  metrics_pub_ = create_publisher<MetricArrayMsg>("~/metrics", 1);
  for (const std::string & selected_metric :
       declare_parameter<std::vector<std::string>>("selected_metrics")) {
    Metric metric = str_to_metric.at(selected_metric);
    metrics_.push_back(metric);
  }
}

PlanningEvaluatorNode::~PlanningEvaluatorNode()
{
  if (!output_metrics_) {
    return;
  }

  try {
    // generate json data
    using json = nlohmann::json;
    json j;
    for (Metric metric : metrics_) {
      const std::string base_name = metric_to_str.at(metric) + "/";
      j[base_name + "min"] = metric_accumulators_[static_cast<size_t>(metric)][0].min();
      j[base_name + "max"] = metric_accumulators_[static_cast<size_t>(metric)][1].max();
      j[base_name + "mean"] = metric_accumulators_[static_cast<size_t>(metric)][2].mean();
      j[base_name + "count"] = metric_accumulators_[static_cast<size_t>(metric)][2].count();
      j[base_name + "description"] = metric_descriptions.at(metric);
    }

    // get output folder
    const std::string output_folder_str =
      rclcpp::get_logging_directory().string() + "/autoware_metrics";
    if (!std::filesystem::exists(output_folder_str)) {
      if (!std::filesystem::create_directories(output_folder_str)) {
        RCLCPP_ERROR(
          this->get_logger(), "Failed to create directories: %s", output_folder_str.c_str());
        return;
      }
    }

    // get time stamp
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm * local_time = std::localtime(&now_time_t);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d-%H-%M-%S");
    std::string cur_time_str = oss.str();

    // Write metrics .json to file
    const std::string output_file_str =
      output_folder_str + "/autoware_planning_evaluator-" + cur_time_str + ".json";
    std::ofstream f(output_file_str);
    if (f.is_open()) {
      f << j.dump(4);
      f.close();
    } else {
      RCLCPP_ERROR(this->get_logger(), "Failed to open file: %s", output_file_str.c_str());
    }
  } catch (const std::exception & e) {
    std::cerr << "Exception in MotionEvaluatorNode destructor: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception in MotionEvaluatorNode destructor" << std::endl;
  }
}

void PlanningEvaluatorNode::getRouteData()
{
  // route
  {
    const auto msg = route_subscriber_.takeData();
    if (msg) {
      if (msg->segments.empty()) {
        RCLCPP_ERROR(get_logger(), "input route is empty. ignored");
      } else {
        route_handler_.setRoute(*msg);
      }
    }
  }

  // map
  {
    const auto msg = vector_map_subscriber_.takeData();
    if (msg) {
      route_handler_.setMap(*msg);
    }
  }
}

void PlanningEvaluatorNode::AddLaneletMetricMsg(const Odometry::ConstSharedPtr ego_state_ptr)
{
  const auto & ego_pose = ego_state_ptr->pose.pose;
  const auto current_lanelets = [&]() {
    lanelet::ConstLanelet closest_route_lanelet;
    route_handler_.getClosestLaneletWithinRoute(ego_pose, &closest_route_lanelet);
    const auto shoulder_lanelets = route_handler_.getShoulderLaneletsAtPose(ego_pose);
    lanelet::ConstLanelets closest_lanelets{closest_route_lanelet};
    closest_lanelets.insert(
      closest_lanelets.end(), shoulder_lanelets.begin(), shoulder_lanelets.end());
    return closest_lanelets;
  }();
  const auto arc_coordinates = lanelet::utils::getArcCoordinates(current_lanelets, ego_pose);
  lanelet::ConstLanelet current_lane;
  lanelet::utils::query::getClosestLanelet(current_lanelets, ego_pose, &current_lane);

  // push_back lanelet info to MetricArrayMsg
  const std::string base_name = "ego_lane_info/";
  MetricMsg metric_msg;

  {
    metric_msg.name = base_name + "lane_id";
    metric_msg.value = std::to_string(current_lane.id());
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  {
    metric_msg.name = base_name + "s";
    metric_msg.value = std::to_string(arc_coordinates.length);
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  {
    metric_msg.name = base_name + "t";
    metric_msg.value = std::to_string(arc_coordinates.distance);
    metrics_msg_.metric_array.push_back(metric_msg);
  }
  return;
}

void PlanningEvaluatorNode::AddKinematicStateMetricMsg(
  const AccelWithCovarianceStamped & accel_stamped, const Odometry::ConstSharedPtr ego_state_ptr)
{
  const std::string base_name = "kinematic_state/";
  MetricMsg metric_msg;

  metric_msg.name = base_name + "vel";
  metric_msg.value = std::to_string(ego_state_ptr->twist.twist.linear.x);
  metrics_msg_.metric_array.push_back(metric_msg);

  metric_msg.name = base_name + "acc";
  const auto & acc = accel_stamped.accel.accel.linear.x;
  metric_msg.value = std::to_string(acc);
  metrics_msg_.metric_array.push_back(metric_msg);

  const auto jerk = [&]() {
    if (!prev_acc_stamped_.has_value()) {
      prev_acc_stamped_ = accel_stamped;
      return 0.0;
    }
    const auto t = static_cast<double>(accel_stamped.header.stamp.sec) +
                   static_cast<double>(accel_stamped.header.stamp.nanosec) * 1e-9;
    const auto prev_t = static_cast<double>(prev_acc_stamped_.value().header.stamp.sec) +
                        static_cast<double>(prev_acc_stamped_.value().header.stamp.nanosec) * 1e-9;
    const auto dt = t - prev_t;
    if (dt < std::numeric_limits<double>::epsilon()) return 0.0;

    const auto prev_acc = prev_acc_stamped_.value().accel.accel.linear.x;
    prev_acc_stamped_ = accel_stamped;
    return (acc - prev_acc) / dt;
  }();
  metric_msg.name = base_name + "jerk";
  metric_msg.value = std::to_string(jerk);
  metrics_msg_.metric_array.push_back(metric_msg);
  return;
}

void PlanningEvaluatorNode::AddMetricMsg(
  const Metric & metric, const Accumulator<double> & metric_stat)
{
  const std::string base_name = metric_to_str.at(metric) + "/";
  MetricMsg metric_msg;
  {
    metric_msg.name = base_name + "min";
    metric_msg.value = boost::lexical_cast<decltype(metric_msg.value)>(metric_stat.min());
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  {
    metric_msg.name = base_name + "max";
    metric_msg.value = boost::lexical_cast<decltype(metric_msg.value)>(metric_stat.max());
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  {
    metric_msg.name = base_name + "mean";
    metric_msg.value = boost::lexical_cast<decltype(metric_msg.value)>(metric_stat.mean());
    metrics_msg_.metric_array.push_back(metric_msg);
  }

  return;
}

void PlanningEvaluatorNode::onTimer()
{
  autoware::universe_utils::StopWatch<std::chrono::milliseconds> stop_watch;

  const auto ego_state_ptr = odometry_sub_.takeData();
  onOdometry(ego_state_ptr);
  {
    const auto objects_msg = objects_sub_.takeData();
    onObjects(objects_msg);
  }

  {
    const auto ref_traj_msg = ref_sub_.takeData();
    onReferenceTrajectory(ref_traj_msg);
  }

  {
    const auto traj_msg = traj_sub_.takeData();
    onTrajectory(traj_msg, ego_state_ptr);
  }
  {
    const auto modified_goal_msg = modified_goal_sub_.takeData();
    onModifiedGoal(modified_goal_msg, ego_state_ptr);
  }

  // Publish metrics
  metrics_msg_.stamp = now();
  metrics_pub_->publish(metrics_msg_);
  metrics_msg_ = MetricArrayMsg{};

  // Publish ProcessingTime
  autoware_internal_debug_msgs::msg::Float64Stamped processing_time_msg;
  processing_time_msg.stamp = get_clock()->now();
  processing_time_msg.data = stop_watch.toc();
  processing_time_pub_->publish(processing_time_msg);
}

void PlanningEvaluatorNode::onTrajectory(
  const Trajectory::ConstSharedPtr traj_msg, const Odometry::ConstSharedPtr ego_state_ptr)
{
  if (!ego_state_ptr || !traj_msg) {
    return;
  }

  auto start = now();

  for (Metric metric : metrics_) {
    const auto metric_stat = metrics_calculator_.calculate(Metric(metric), *traj_msg);
    if (!metric_stat) {
      continue;
    }

    if (output_metrics_) {
      metric_accumulators_[static_cast<size_t>(metric)][0].add(metric_stat->min());
      metric_accumulators_[static_cast<size_t>(metric)][1].add(metric_stat->max());
      metric_accumulators_[static_cast<size_t>(metric)][2].add(metric_stat->mean());
    }

    if (metric_stat->count() > 0) {
      AddMetricMsg(metric, *metric_stat);
    }
  }

  metrics_calculator_.setPreviousTrajectory(*traj_msg);
  auto runtime = (now() - start).seconds();
  RCLCPP_DEBUG(get_logger(), "Planning evaluation calculation time: %2.2f ms", runtime * 1e3);
}

void PlanningEvaluatorNode::onModifiedGoal(
  const PoseWithUuidStamped::ConstSharedPtr modified_goal_msg,
  const Odometry::ConstSharedPtr ego_state_ptr)
{
  if (!modified_goal_msg || !ego_state_ptr) {
    return;
  }
  auto start = now();

  for (Metric metric : metrics_) {
    const auto metric_stat = metrics_calculator_.calculate(
      Metric(metric), modified_goal_msg->pose, ego_state_ptr->pose.pose);
    if (!metric_stat) {
      continue;
    }
    if (output_metrics_) {
      metric_accumulators_[static_cast<size_t>(metric)][0].add(metric_stat->min());
      metric_accumulators_[static_cast<size_t>(metric)][1].add(metric_stat->max());
      metric_accumulators_[static_cast<size_t>(metric)][2].add(metric_stat->mean());
    }
    if (metric_stat->count() > 0) {
      AddMetricMsg(metric, *metric_stat);
    }
  }
  auto runtime = (now() - start).seconds();
  RCLCPP_DEBUG(
    get_logger(), "Planning evaluation modified goal deviation calculation time: %2.2f ms",
    runtime * 1e3);
}

void PlanningEvaluatorNode::onOdometry(const Odometry::ConstSharedPtr odometry_msg)
{
  if (!odometry_msg) return;
  metrics_calculator_.setEgoPose(*odometry_msg);
  {
    getRouteData();
    if (route_handler_.isHandlerReady() && odometry_msg) {
      AddLaneletMetricMsg(odometry_msg);
    }

    const auto acc_msg = accel_sub_.takeData();
    if (acc_msg && odometry_msg) {
      AddKinematicStateMetricMsg(*acc_msg, odometry_msg);
    }
  }
}

void PlanningEvaluatorNode::onReferenceTrajectory(const Trajectory::ConstSharedPtr traj_msg)
{
  if (!traj_msg) {
    return;
  }
  metrics_calculator_.setReferenceTrajectory(*traj_msg);
}

void PlanningEvaluatorNode::onObjects(const PredictedObjects::ConstSharedPtr objects_msg)
{
  if (!objects_msg) {
    return;
  }
  metrics_calculator_.setPredictedObjects(*objects_msg);
}

bool PlanningEvaluatorNode::isFinite(const TrajectoryPoint & point)
{
  const auto & o = point.pose.orientation;
  const auto & p = point.pose.position;
  const auto & v = point.longitudinal_velocity_mps;
  const auto & w = point.lateral_velocity_mps;
  const auto & a = point.acceleration_mps2;
  const auto & z = point.heading_rate_rps;
  const auto & f = point.front_wheel_angle_rad;
  const auto & r = point.rear_wheel_angle_rad;

  return std::isfinite(o.x) && std::isfinite(o.y) && std::isfinite(o.z) && std::isfinite(o.w) &&
         std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z) && std::isfinite(v) &&
         std::isfinite(w) && std::isfinite(a) && std::isfinite(z) && std::isfinite(f) &&
         std::isfinite(r);
}
}  // namespace planning_diagnostics

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(planning_diagnostics::PlanningEvaluatorNode)

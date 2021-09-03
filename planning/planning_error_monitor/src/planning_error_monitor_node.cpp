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

#include <memory>
#include <string>
#include <utility>

#include "autoware_utils/autoware_utils.hpp"

#include "planning_error_monitor/planning_error_monitor_node.hpp"

namespace planning_diagnostics
{
using autoware_utils::calcCurvature;
using autoware_utils::calcDistance2d;
using diagnostic_msgs::msg::DiagnosticStatus;

PlanningErrorMonitorNode::PlanningErrorMonitorNode(const rclcpp::NodeOptions & node_options)
: Node("planning_error_monitor", node_options)
{
  using std::placeholders::_1;
  using std::chrono_literals::operator""ms;

  traj_sub_ =
    create_subscription<Trajectory>(
    "~/input/trajectory", 1,
    std::bind(&PlanningErrorMonitorNode::onCurrentTrajectory, this, _1));

  updater_.setHardwareID("planning_error_monitor");

  updater_.add(
    "trajectory_point_validation", this, &PlanningErrorMonitorNode::onTrajectoryPointValueChecker);
  updater_.add(
    "trajectory_interval_validation", this, &PlanningErrorMonitorNode::onTrajectoryIntervalChecker);
  updater_.add(
    "trajectory_curvature_validation", this,
    &PlanningErrorMonitorNode::onTrajectoryCurvatureChecker);

  auto on_timer_ = std::bind(&PlanningErrorMonitorNode::onTimer, this);
  timer_ = std::make_shared<rclcpp::GenericTimer<decltype(on_timer_)>>(
    this->get_clock(), 100ms, std::move(on_timer_),
    this->get_node_base_interface()->get_context());
  this->get_node_timers_interface()->add_timer(timer_, nullptr);

  // Parameter
  error_interval_ = declare_parameter("error_interval", 100.0);
  error_curvature_ = declare_parameter("error_curvature", 1.0);
}

void PlanningErrorMonitorNode::onTimer() {updater_.force_update();}

void PlanningErrorMonitorNode::onCurrentTrajectory(const Trajectory::ConstSharedPtr msg)
{
  current_trajectory_ = msg;
}

void PlanningErrorMonitorNode::onTrajectoryPointValueChecker(DiagnosticStatusWrapper & stat)
{
  if (!current_trajectory_) {return;}

  std::string error_msg;
  const auto diag_level = checkTrajectoryPointValue(*current_trajectory_, error_msg) ?
    DiagnosticStatus::OK :
    DiagnosticStatus::ERROR;
  stat.summary(diag_level, error_msg);
}

void PlanningErrorMonitorNode::onTrajectoryIntervalChecker(DiagnosticStatusWrapper & stat)
{
  if (!current_trajectory_) {return;}

  std::string error_msg;
  const auto diag_level =
    checkTrajectoryInterval(*current_trajectory_, error_interval_, error_msg) ?
    DiagnosticStatus::OK :
    DiagnosticStatus::ERROR;
  stat.summary(diag_level, error_msg);
}

void PlanningErrorMonitorNode::onTrajectoryCurvatureChecker(DiagnosticStatusWrapper & stat)
{
  if (!current_trajectory_) {return;}

  std::string error_msg;
  const auto diag_level =
    checkTrajectoryCurvature(*current_trajectory_, error_curvature_, error_msg) ?
    DiagnosticStatus::OK :
    DiagnosticStatus::ERROR;
  stat.summary(diag_level, error_msg);
}

bool PlanningErrorMonitorNode::checkTrajectoryPointValue(
  const Trajectory & traj,
  std::string & error_msg)
{
  error_msg = "This Trajectory doesn't have any invalid values";
  for (const auto & p : traj.points) {
    if (!checkFinite(p)) {
      error_msg = "This trajectory has an infinite value";
      return false;
    }
  }
  return true;
}

bool PlanningErrorMonitorNode::checkFinite(const TrajectoryPoint & point)
{
  const auto & o = point.pose.orientation;
  const auto & p = point.pose.position;
  const auto & v = point.twist.linear;
  const auto & w = point.twist.angular;
  const auto & a = point.accel.linear;
  const auto & z = point.accel.angular;

  const bool quat_result =
    std::isfinite(o.x) && std::isfinite(o.y) && std::isfinite(o.z) && std::isfinite(o.w);
  const bool p_result = std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z);
  const bool v_result = std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
  const bool w_result = std::isfinite(w.x) && std::isfinite(w.y) && std::isfinite(w.z);
  const bool a_result = std::isfinite(a.x) && std::isfinite(a.y) && std::isfinite(a.z);
  const bool z_result = std::isfinite(z.x) && std::isfinite(z.y) && std::isfinite(z.z);

  return quat_result && p_result && v_result && w_result && a_result && z_result;
}

bool PlanningErrorMonitorNode::checkTrajectoryInterval(
  const Trajectory & traj, const double & interval_threshold, std::string & error_msg)
{
  error_msg = "Trajectory Interval Length is within the expected range";
  for (size_t i = 1; i < traj.points.size(); ++i) {
    double ds = calcDistance2d(traj.points.at(i), traj.points.at(i - 1));

    if (ds > interval_threshold) {
      error_msg = "Trajectory Interval Length is longer than the expected range";
      return false;
    }
  }
  return true;
}

bool PlanningErrorMonitorNode::checkTrajectoryCurvature(
  const Trajectory & traj, const double & curvature_threshold, std::string & error_msg)
{
  error_msg = "This trajectory's curvature is within the expected range";

  // We need at least three points to compute curvature
  if (traj.points.size() < 3) {return true;}

  constexpr double points_distance = 1.0;

  for (size_t p1_id = 0; p1_id < traj.points.size() - 2; ++p1_id) {
    // Get Point1
    const auto p1 = traj.points.at(p1_id).pose.position;

    // Get Point2
    const auto p2_id = getIndexAfterDistance(traj, p1_id, points_distance);
    const auto p2 = traj.points.at(p2_id).pose.position;

    // Get Point3
    const auto p3_id = getIndexAfterDistance(traj, p2_id, points_distance);
    const auto p3 = traj.points.at(p3_id).pose.position;

    // no need to check for pi, since there is no point with "points_distance" from p1.
    if (p1_id == p2_id || p1_id == p3_id || p2_id == p3_id) {break;}

    const double curvature = calcCurvature(p1, p2, p3);

    if (std::fabs(curvature) > curvature_threshold) {
      error_msg = "This Trajectory's curvature has larger value than the expected value";
      return false;
    }
  }
  return true;
}

size_t PlanningErrorMonitorNode::getIndexAfterDistance(
  const Trajectory & traj, const size_t curr_id, const double distance)
{
  // Get Current Trajectory Point
  const TrajectoryPoint & curr_p = traj.points.at(curr_id);

  size_t target_id = curr_id;
  double current_distance = 0.0;
  for (size_t traj_id = curr_id + 1; traj_id < traj.points.size(); ++traj_id) {
    current_distance = calcDistance2d(traj.points.at(traj_id), curr_p);
    if (current_distance >= distance) {
      target_id = traj_id;
      break;
    }
  }

  return target_id;
}
}  // namespace planning_diagnostics

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(planning_diagnostics::PlanningErrorMonitorNode)

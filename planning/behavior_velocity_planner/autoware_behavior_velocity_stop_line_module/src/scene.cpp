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

#include "scene.hpp"

#include "autoware/behavior_velocity_planner_common/utilization/util.hpp"

#include <rclcpp/logging.hpp>

#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <optional>
#include <utility>

namespace autoware::behavior_velocity_planner
{

geometry_msgs::msg::Point getCenterOfStopLine(const lanelet::ConstLineString3d & stop_line)
{
  geometry_msgs::msg::Point center_point;
  center_point.x = (stop_line[0].x() + stop_line[1].x()) / 2.0;
  center_point.y = (stop_line[0].y() + stop_line[1].y()) / 2.0;
  center_point.z = (stop_line[0].z() + stop_line[1].z()) / 2.0;
  return center_point;
}

StopLineModule::StopLineModule(
  const int64_t module_id, lanelet::ConstLineString3d stop_line, const PlannerParam & planner_param,
  const rclcpp::Logger & logger, const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  stop_line_(std::move(stop_line)),
  planner_param_(planner_param),
  state_(State::APPROACH),
  debug_data_()
{
  velocity_factor_.init(PlanningBehavior::STOP_SIGN);
}

bool StopLineModule::modifyPathVelocity(PathWithLaneId * path)
{
  auto trajectory =
    trajectory::Trajectory<tier4_planning_msgs::msg::PathPointWithLaneId>::Builder{}.build(
      path->points);

  if (!trajectory) {
    return true;
  }

  auto [ego_s, stop_point] =
    getEgoAndStopPoint(*trajectory, planner_data_->current_odometry->pose, state_);

  if (!stop_point) {
    return true;
  }

  trajectory->longitudinal_velocity_mps.range(*stop_point, trajectory->length()).set(0.0);

  path->points = trajectory->restore();

  updateVelocityFactor(&velocity_factor_, state_, *stop_point - ego_s);

  updateStateAndStoppedTime(
    &state_, &stopped_time_, clock_->now(), *stop_point - ego_s, planner_data_->isVehicleStopped());

  geometry_msgs::msg::Pose stop_pose = trajectory->compute(*stop_point).point.pose;

  updateDebugData(&debug_data_, stop_pose, state_);

  return true;
}

std::pair<double, std::optional<double>> StopLineModule::getEgoAndStopPoint(
  const Trajectory & trajectory, const geometry_msgs::msg::Pose & ego_pose,
  const State & state) const
{
  const double ego_s = trajectory.closest(ego_pose.position);
  std::optional<double> stop_point_s;

  switch (state) {
    case State::APPROACH: {
      const double base_link2front = planner_data_->vehicle_info_.max_longitudinal_offset_m;
      const LineString2d stop_line = planning_utils::extendLine(
        stop_line_[0], stop_line_[1], planner_data_->stop_line_extend_length);

      // Calculate intersection with stop line
      const auto trajectory_stop_line_intersection =
        trajectory.crossed(stop_line.front(), stop_line.back());

      // If no collision found, do nothing
      if (!trajectory_stop_line_intersection) {
        stop_point_s = std::nullopt;
        break;
      }

      stop_point_s =
        *trajectory_stop_line_intersection -
        (base_link2front + planner_param_.stop_margin);  // consider vehicle length and stop margin

      if (*stop_point_s < 0.0) {
        stop_point_s = std::nullopt;
      }
      break;
    }

    case State::STOPPED: {
      stop_point_s = ego_s;
      break;
    }

    case State::START: {
      stop_point_s = std::nullopt;
      break;
    }
  }
  return {ego_s, stop_point_s};
}

void StopLineModule::updateStateAndStoppedTime(
  State * state, std::optional<rclcpp::Time> * stopped_time, const rclcpp::Time & now,
  const double & distance_to_stop_point, const bool & is_vehicle_stopped) const
{
  switch (*state) {
    case State::APPROACH: {
      if (distance_to_stop_point < planner_param_.hold_stop_margin_distance && is_vehicle_stopped) {
        *state = State::STOPPED;
        *stopped_time = now;
        RCLCPP_INFO(logger_, "APPROACH -> STOPPED");

        if (distance_to_stop_point < 0.0) {
          RCLCPP_WARN(logger_, "Vehicle cannot stop before stop line");
        }
      }
      break;
    }
    case State::STOPPED: {
      double stop_duration = (now - **stopped_time).seconds();
      if (stop_duration > planner_param_.stop_duration_sec) {
        *state = State::START;
        stopped_time->reset();
        RCLCPP_INFO(logger_, "STOPPED -> START");
      }
      break;
    }
    case State::START: {
      break;
    }
  }
}

void StopLineModule::updateVelocityFactor(
  autoware::motion_utils::VelocityFactorInterface * velocity_factor, const State & state,
  const double & distance_to_stop_point)
{
  switch (state) {
    case State::APPROACH: {
      velocity_factor->set(distance_to_stop_point, VelocityFactor::APPROACHING);
      break;
    }
    case State::STOPPED: {
      velocity_factor->set(distance_to_stop_point, VelocityFactor::STOPPED);
      break;
    }
    case State::START:
      break;
  }
}

void StopLineModule::updateDebugData(
  DebugData * debug_data, const geometry_msgs::msg::Pose & stop_pose, const State & state) const
{
  debug_data->base_link2front = planner_data_->vehicle_info_.max_longitudinal_offset_m;
  debug_data->stop_pose = stop_pose;
  if (state == State::START) {
    debug_data->stop_pose = std::nullopt;
  }
}

}  // namespace autoware::behavior_velocity_planner

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
#include "autoware/trajectory/path_point_with_lane_id.hpp"

#include <tier4_planning_msgs/msg/detail/path_with_lane_id__struct.hpp>

namespace autoware::behavior_velocity_planner
{

StopLineModule::StopLineModule(
  const int64_t module_id, const size_t lane_id, lanelet::ConstLineString3d stop_line,
  const PlannerParam & planner_param, const rclcpp::Logger & logger,
  const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  lane_id_(lane_id),
  stop_line_(std::move(stop_line)),
  state_(State::APPROACH),
  planner_param_(planner_param),
  debug_data_()
{
  velocity_factor_.init(PlanningBehavior::STOP_SIGN);
}

bool StopLineModule::modifyPathVelocity(PathWithLaneId * path, StopReason * stop_reason)
{
  auto trajectory =
    trajectory::Trajectory<tier4_planning_msgs::msg::PathPointWithLaneId>::Builder{}.build(
      path->points);

  if (!trajectory) {
    return true;
  }

  debug_data_ = DebugData();
  const double base_link2front = planner_data_->vehicle_info_.max_longitudinal_offset_m;
  debug_data_.base_link2front = base_link2front;
  first_stop_path_point_distance_ = trajectory->length();
  *stop_reason = planning_utils::initializeStopReason(StopReason::STOP_LINE);

  const LineString2d stop_line = planning_utils::extendLine(
    stop_line_[0], stop_line_[1], planner_data_->stop_line_extend_length);

  // Calculate intersection with stop line
  const auto trajectory_stop_line_intersection =
    trajectory->crossed(stop_line.front(), stop_line.back());

  // If no collision found, do nothing
  if (!trajectory_stop_line_intersection) {
    RCLCPP_DEBUG_THROTTLE(logger_, *clock_, 5000 /* ms */, "is no collision");
    return true;
  }

  const double stop_point_s =
    *trajectory_stop_line_intersection -
    (base_link2front + planner_param_.stop_margin);  // consider vehicle length and stop margin

  if (stop_point_s < 0.0) {
    return true;
  }

  const auto stop_pose = trajectory->compute(stop_point_s);

  /**
   * @brief : calculate signed arc length consider stop margin from stop line
   *
   * |----------------------------|
   * s---ego----------x--|--------g
   */
  const double ego_on_trajectory_s =
    trajectory->closest(planner_data_->current_odometry->pose.position);
  const double signed_arc_dist_to_stop_point = stop_point_s - ego_on_trajectory_s;

  switch (state_) {
    case State::APPROACH: {
      // Insert stop pose
      trajectory->longitudinal_velocity_mps.range(stop_point_s, trajectory->length()).set(0.0);

      // Update first stop path point distance
      first_stop_path_point_distance_ = stop_point_s;
      debug_data_.stop_pose = stop_pose.point.pose;

      // Get stop point and stop factor
      {
        tier4_planning_msgs::msg::StopFactor stop_factor;
        stop_factor.stop_pose = stop_pose.point.pose;
        stop_factor.stop_factor_points.push_back(getCenterOfStopLine(stop_line_));
        planning_utils::appendStopReason(stop_factor, stop_reason);
        velocity_factor_.set(signed_arc_dist_to_stop_point, VelocityFactor::APPROACHING);
      }

      // Move to stopped state if stopped
      if (
        signed_arc_dist_to_stop_point < planner_param_.hold_stop_margin_distance &&
        planner_data_->isVehicleStopped()) {
        RCLCPP_INFO(logger_, "APPROACH -> STOPPED");

        state_ = State::STOPPED;
        stopped_time_ = std::make_shared<const rclcpp::Time>(clock_->now());

        if (signed_arc_dist_to_stop_point < -planner_param_.hold_stop_margin_distance) {
          RCLCPP_ERROR(
            logger_, "Failed to stop near stop line but ego stopped. Change state to STOPPED");
        }
      }

      break;
    }

    case State::STOPPED: {
      // Insert stop pose
      trajectory->longitudinal_velocity_mps.range(ego_on_trajectory_s, trajectory->length())
        .set(0.0);
      const auto ego_pos_on_path = trajectory->compute(ego_on_trajectory_s).point.pose;
      debug_data_.stop_pose = ego_pos_on_path;

      // Get stop point and stop factor
      {
        tier4_planning_msgs::msg::StopFactor stop_factor;
        stop_factor.stop_pose = ego_pos_on_path;
        stop_factor.stop_factor_points.push_back(getCenterOfStopLine(stop_line_));
        planning_utils::appendStopReason(stop_factor, stop_reason);
        velocity_factor_.set(signed_arc_dist_to_stop_point, VelocityFactor::STOPPED);
      }

      const double elapsed_time = (clock_->now() - *stopped_time_).seconds();

      if (planner_param_.stop_duration_sec < elapsed_time) {
        RCLCPP_INFO(logger_, "STOPPED -> START");
        state_ = State::START;
      }

      break;
    }

    case State::START: {
      // Initialize if vehicle is far from stop_line
      if (planner_param_.use_initialization_stop_line_state) {
        if (signed_arc_dist_to_stop_point > planner_param_.hold_stop_margin_distance) {
          RCLCPP_INFO(logger_, "START -> APPROACH");
          state_ = State::APPROACH;
        }
      }

      break;
    }
  }

  path->points = trajectory->restore();

  return true;
}

geometry_msgs::msg::Point StopLineModule::getCenterOfStopLine(
  const lanelet::ConstLineString3d & stop_line)
{
  geometry_msgs::msg::Point center_point;
  center_point.x = (stop_line[0].x() + stop_line[1].x()) / 2.0;
  center_point.y = (stop_line[0].y() + stop_line[1].y()) / 2.0;
  center_point.z = (stop_line[0].z() + stop_line[1].z()) / 2.0;
  return center_point;
}
}  // namespace autoware::behavior_velocity_planner

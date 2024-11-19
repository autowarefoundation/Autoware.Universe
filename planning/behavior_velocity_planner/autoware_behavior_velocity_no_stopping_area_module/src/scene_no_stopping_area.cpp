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

#include "scene_no_stopping_area.hpp"

#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/planner_data.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>
#include <autoware/interpolation/spline_interpolation.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/logger.hpp>

#include <lanelet2_core/geometry/Polygon.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_core/utility/Optional.h>

#include <cmath>
#include <vector>

namespace autoware::behavior_velocity_planner
{
namespace bg = boost::geometry;

NoStoppingAreaModule::NoStoppingAreaModule(
  const int64_t module_id, const int64_t lane_id,
  const lanelet::autoware::NoStoppingArea & no_stopping_area_reg_elem,
  const PlannerParam & planner_param, const rclcpp::Logger & logger,
  const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  lane_id_(lane_id),
  no_stopping_area_reg_elem_(no_stopping_area_reg_elem),
  planner_param_(planner_param),
  debug_data_()
{
  velocity_factor_.init(PlanningBehavior::NO_STOPPING_AREA);
  state_machine_.setState(StateMachine::State::GO);
  state_machine_.setMarginTime(planner_param_.state_clear_time);
}

bool NoStoppingAreaModule::modifyPathVelocity(PathWithLaneId * path, StopReason * stop_reason)
{
  // Store original path
  const auto original_path = *path;
  const auto & predicted_obj_arr_ptr = planner_data_->predicted_objects;
  const auto & current_pose = planner_data_->current_odometry;
  if (path->points.size() <= 2) {
    return true;
  }
  // Reset data
  debug_data_ = no_stopping_area::DebugData();
  debug_data_.base_link2front = planner_data_->vehicle_info_.max_longitudinal_offset_m;
  *stop_reason = planning_utils::initializeStopReason(StopReason::NO_STOPPING_AREA);

  const no_stopping_area::EgoData ego_data(*planner_data_);

  // Get stop line geometry
  const auto stop_line = no_stopping_area::get_stop_line_geometry2d(
    original_path, no_stopping_area_reg_elem_, planner_param_.stop_line_margin,
    planner_data_->stop_line_extend_length, planner_data_->vehicle_info_.vehicle_width_m);
  if (!stop_line) {
    setSafe(true);
    return true;
  }
  const auto stop_point = arc_lane_utils::createTargetPoint(
    original_path, stop_line.value(), planner_param_.stop_margin,
    planner_data_->vehicle_info_.max_longitudinal_offset_m);
  if (!stop_point) {
    setSafe(true);
    return true;
  }
  const auto & stop_pose = stop_point->second;
  setDistance(autoware::motion_utils::calcSignedArcLength(
    original_path.points, current_pose->pose.position, stop_pose.position));
  if (planning_utils::isOverLine(
        original_path, current_pose->pose, stop_pose, planner_param_.dead_line_margin)) {
    // ego can't stop in front of no stopping area -> GO or OR
    state_machine_.setState(StateMachine::State::GO);
    setSafe(true);
    return true;
  }
  const auto & vi = planner_data_->vehicle_info_;
  const double margin = planner_param_.stop_line_margin;
  const double ego_space_in_front_of_stuck_vehicle =
    margin + vi.vehicle_length_m + planner_param_.stuck_vehicle_front_margin;
  const Polygon2d stuck_vehicle_detect_area =
    no_stopping_area::generate_ego_no_stopping_area_lane_polygon(
      *path, current_pose->pose, no_stopping_area_reg_elem_, ego_space_in_front_of_stuck_vehicle,
      planner_param_.detection_area_length, planner_param_.path_expand_width, logger_, *clock_);
  const double ego_space_in_front_of_stop_line =
    margin + planner_param_.stop_margin + vi.rear_overhang_m;
  const Polygon2d stop_line_detect_area =
    no_stopping_area::generate_ego_no_stopping_area_lane_polygon(
      *path, current_pose->pose, no_stopping_area_reg_elem_, ego_space_in_front_of_stop_line,
      planner_param_.detection_area_length, planner_param_.path_expand_width, logger_, *clock_);
  if (stuck_vehicle_detect_area.outer().empty() && stop_line_detect_area.outer().empty()) {
    setSafe(true);
    return true;
  }
  debug_data_.stuck_vehicle_detect_area = toGeomPoly(stuck_vehicle_detect_area);
  debug_data_.stop_line_detect_area = toGeomPoly(stop_line_detect_area);
  // Find stuck vehicle in no stopping area
  const bool is_entry_prohibited_by_stuck_vehicle =
    check_stuck_vehicles_in_no_stopping_area(stuck_vehicle_detect_area, predicted_obj_arr_ptr);
  // Find stop line in no stopping area
  const bool is_entry_prohibited_by_stop_line =
    no_stopping_area::check_stop_lines_in_no_stopping_area(
      *path, stop_line_detect_area, debug_data_);
  const bool is_entry_prohibited =
    is_entry_prohibited_by_stuck_vehicle || is_entry_prohibited_by_stop_line;
  if (!no_stopping_area::is_stoppable(
        pass_judge_, current_pose->pose, stop_point->second, ego_data, logger_, *clock_)) {
    state_machine_.setState(StateMachine::State::GO);
    setSafe(true);
    return false;
  }

  state_machine_.setStateWithMarginTime(
    is_entry_prohibited ? StateMachine::State::STOP : StateMachine::State::GO,
    logger_.get_child("state_machine"), *clock_);

  setSafe(state_machine_.getState() != StateMachine::State::STOP);
  if (!isActivated()) {
    // ----------------stop reason and stop point--------------------------
    no_stopping_area::insert_stop_point(*path, *stop_point);
    // For virtual wall
    debug_data_.stop_poses.push_back(stop_pose);

    // Create StopReason
    {
      tier4_planning_msgs::msg::StopFactor stop_factor;
      stop_factor.stop_pose = stop_point->second;
      stop_factor.stop_factor_points = debug_data_.stuck_points;
      planning_utils::appendStopReason(stop_factor, stop_reason);
      velocity_factor_.set(
        path->points, planner_data_->current_odometry->pose, stop_point->second,
        VelocityFactor::UNKNOWN);
    }

    // Create legacy StopReason
    {
      const double stop_path_point_distance = autoware::motion_utils::calcSignedArcLength(
        path->points, 0, stop_pose.position, stop_point->first);

      if (
        !first_stop_path_point_distance_ ||
        stop_path_point_distance < first_stop_path_point_distance_.value()) {
        debug_data_.first_stop_pose = stop_point->second;
        first_stop_path_point_distance_ = stop_path_point_distance;
      }
    }
  } else if (state_machine_.getState() == StateMachine::State::GO) {
    // reset pass judge if current state is go
    pass_judge_.is_stoppable = true;
    pass_judge_.pass_judged = false;
  }
  return true;
}

bool NoStoppingAreaModule::check_stuck_vehicles_in_no_stopping_area(
  const Polygon2d & poly,
  const autoware_perception_msgs::msg::PredictedObjects::ConstSharedPtr & predicted_obj_arr_ptr)
{
  // stuck points by predicted objects
  for (const auto & object : predicted_obj_arr_ptr->objects) {
    if (!no_stopping_area::is_vehicle_type(object)) {
      continue;  // not target vehicle type
    }
    const auto obj_v = std::fabs(object.kinematics.initial_twist_with_covariance.twist.linear.x);
    if (obj_v > planner_param_.stuck_vehicle_vel_thr) {
      setObjectsOfInterestData(
        object.kinematics.initial_pose_with_covariance.pose, object.shape, ColorName::GREEN);
      continue;  // not stop vehicle
    }
    // check if the footprint is in the stuck detect area
    const Polygon2d obj_footprint = autoware::universe_utils::toPolygon2d(object);
    const bool is_in_stuck_area = !bg::disjoint(obj_footprint, poly);
    if (is_in_stuck_area) {
      RCLCPP_DEBUG(logger_, "stuck vehicle found.");
      setObjectsOfInterestData(
        object.kinematics.initial_pose_with_covariance.pose, object.shape, ColorName::RED);
      for (const auto & p : obj_footprint.outer()) {
        geometry_msgs::msg::Point point;
        point.x = p.x();
        point.y = p.y();
        point.z = 0.0;
        debug_data_.stuck_points.emplace_back(point);
      }
      return true;
    }
  }
  return false;
}
}  // namespace autoware::behavior_velocity_planner

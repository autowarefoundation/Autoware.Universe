// Copyright 2022 TIER IV, Inc.
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
#ifndef AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__STRUCTS__DATA_HPP_
#define AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__STRUCTS__DATA_HPP_

#include "autoware/behavior_path_lane_change_module/structs/parameters.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/path_safety_checker_parameters.hpp"
#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"

#include <autoware/behavior_path_planner_common/parameters.hpp>
#include <autoware/interpolation/linear_interpolation.hpp>
#include <autoware/route_handler/route_handler.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>

#include <nav_msgs/msg/odometry.hpp>

#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/Polygon.h>

#include <limits>
#include <memory>
#include <utility>
#include <vector>

namespace autoware::behavior_path_planner::lane_change
{
using geometry_msgs::msg::Pose;
using geometry_msgs::msg::Twist;
using nav_msgs::msg::Odometry;
using route_handler::Direction;
using route_handler::RouteHandler;
using universe_utils::Polygon2d;
using utils::path_safety_checker::ExtendedPredictedObjects;

enum class States {
  Normal = 0,
  Cancel,
  Abort,
  Stop,
  Warning,
};

struct PhaseInfo
{
  double prepare{0.0};
  double lane_changing{0.0};

  [[nodiscard]] double sum() const { return prepare + lane_changing; }

  PhaseInfo(const double _prepare, const double _lane_changing)
  : prepare(_prepare), lane_changing(_lane_changing)
  {
  }
};

struct PhaseMetrics
{
  double duration{0.0};
  double length{0.0};
  double velocity{0.0};
  double sampled_lon_accel{0.0};
  double actual_lon_accel{0.0};
  double lat_accel{0.0};

  PhaseMetrics() = default;
  PhaseMetrics(
    const double _duration, const double _length, const double _velocity,
    const double _sampled_lon_accel, const double _actual_lon_accel, const double _lat_accel)
  : duration(_duration),
    length(_length),
    velocity(_velocity),
    sampled_lon_accel(_sampled_lon_accel),
    actual_lon_accel(_actual_lon_accel),
    lat_accel(_lat_accel)
  {
  }
};

struct Lanes
{
  bool current_lane_in_goal_section{false};
  bool target_lane_in_goal_section{false};
  lanelet::ConstLanelet ego_lane;
  lanelet::ConstLanelets current;
  lanelet::ConstLanelets target_neighbor;
  lanelet::ConstLanelets target;
  std::vector<lanelet::ConstLanelets> preceding_target;
};

struct Info
{
  PhaseInfo longitudinal_acceleration{0.0, 0.0};
  PhaseInfo velocity{0.0, 0.0};
  PhaseInfo duration{0.0, 0.0};
  PhaseInfo length{0.0, 0.0};

  Pose lane_changing_start;
  Pose lane_changing_end;

  ShiftLine shift_line;

  double lateral_acceleration{0.0};
  double terminal_lane_changing_velocity{0.0};

  Info() = default;
  Info(
    const PhaseMetrics & _prep_metrics, const PhaseMetrics & _lc_metrics,
    const Pose & _lc_start_pose, const Pose & _lc_end_pose, const ShiftLine & _shift_line)
  {
    longitudinal_acceleration =
      PhaseInfo{_prep_metrics.actual_lon_accel, _lc_metrics.actual_lon_accel};
    duration = PhaseInfo{_prep_metrics.duration, _lc_metrics.duration};
    velocity = PhaseInfo{_prep_metrics.velocity, _prep_metrics.velocity};
    length = PhaseInfo{_prep_metrics.length, _lc_metrics.length};
    lane_changing_start = _lc_start_pose;
    lane_changing_end = _lc_end_pose;
    lateral_acceleration = _lc_metrics.lat_accel;
    terminal_lane_changing_velocity = _lc_metrics.velocity;
    shift_line = _shift_line;
  }

  void set_prepare(const PhaseMetrics & _prep_metrics)
  {
    longitudinal_acceleration.prepare = _prep_metrics.actual_lon_accel;
    velocity.prepare = _prep_metrics.velocity;
    duration.prepare = _prep_metrics.duration;
    length.prepare = _prep_metrics.length;
  }

  void set_lane_changing(const PhaseMetrics & _lc_metrics)
  {
    longitudinal_acceleration.lane_changing = _lc_metrics.actual_lon_accel;
    velocity.lane_changing = _lc_metrics.velocity;
    duration.lane_changing = _lc_metrics.duration;
    length.lane_changing = _lc_metrics.length;
  }
};

struct TargetLaneLeadingObjects
{
  ExtendedPredictedObjects moving;
  ExtendedPredictedObjects stopped;

  // for objects outside of target lanes, but close to its boundaries
  ExtendedPredictedObjects stopped_at_bound;

  [[nodiscard]] size_t size() const
  {
    return moving.size() + stopped.size() + stopped_at_bound.size();
  }
};

struct FilteredLanesObjects
{
  ExtendedPredictedObjects others;
  ExtendedPredictedObjects current_lane;
  ExtendedPredictedObjects target_lane_trailing;
  TargetLaneLeadingObjects target_lane_leading;
};

struct TargetObjects
{
  ExtendedPredictedObjects leading;
  ExtendedPredictedObjects trailing;
  TargetObjects(ExtendedPredictedObjects leading, ExtendedPredictedObjects trailing)
  : leading(std::move(leading)), trailing(std::move(trailing))
  {
  }

  [[nodiscard]] bool empty() const { return leading.empty() && trailing.empty(); }
};

enum class ModuleType {
  NORMAL = 0,
  EXTERNAL_REQUEST,
  AVOIDANCE_BY_LANE_CHANGE,
};

struct PathSafetyStatus
{
  bool is_safe{true};
  bool is_trailing_object{false};

  PathSafetyStatus() = default;
  PathSafetyStatus(const bool is_safe, const bool is_trailing_object)
  : is_safe(is_safe), is_trailing_object(is_trailing_object)
  {
  }
};

struct LanesPolygon
{
  lanelet::BasicPolygon2d current;
  lanelet::BasicPolygon2d target;
  lanelet::BasicPolygon2d expanded_target;
  lanelet::BasicPolygon2d target_neighbor;
  std::vector<lanelet::BasicPolygon2d> preceding_target;
};

struct MinMaxValue
{
  double min{std::numeric_limits<double>::infinity()};
  double max{std::numeric_limits<double>::infinity()};
};

struct TransientData
{
  Polygon2d current_footprint;  // ego's polygon at current pose

  MinMaxValue lane_changing_length;  // lane changing length for a single lane change
  MinMaxValue
    current_dist_buffer;  // distance buffer computed backward from current lanes' terminal end
  MinMaxValue
    next_dist_buffer;  // distance buffer computed backward  from target lanes' terminal end
  double dist_to_terminal_end{
    std::numeric_limits<double>::min()};  // distance from ego base link to the current lanes'
                                          // terminal end
  double dist_from_prev_intersection{std::numeric_limits<double>::max()};
  // terminal end
  double dist_to_terminal_start{
    std::numeric_limits<double>::min()};  // distance from ego base link to the current lanes'
                                          // terminal start
  double max_prepare_length{
    std::numeric_limits<double>::max()};  // maximum prepare length, starting from ego's base link

  double target_lane_length{std::numeric_limits<double>::min()};

  double dist_to_target_end{std::numeric_limits<double>::max()};

  lanelet::ArcCoordinates current_lanes_ego_arc;  // arc coordinates of ego pose along current lanes
  lanelet::ArcCoordinates target_lanes_ego_arc;   // arc coordinates of ego pose along target lanes

  size_t current_path_seg_idx;   // index of nearest segment to ego along current path
  double current_path_velocity;  // velocity of the current path at the ego position along the path

  double lane_change_prepare_duration{0.0};

  bool is_ego_near_current_terminal_start{false};
  bool is_ego_stuck{false};

  bool in_turn_direction_lane{false};
  bool in_intersection{false};
};

using RouteHandlerPtr = std::shared_ptr<RouteHandler>;
using BppParamPtr = std::shared_ptr<BehaviorPathPlannerParameters>;
using LCParamPtr = std::shared_ptr<Parameters>;
using LanesPtr = std::shared_ptr<Lanes>;
using LanesPolygonPtr = std::shared_ptr<LanesPolygon>;

struct CommonData
{
  RouteHandlerPtr route_handler_ptr;
  Odometry::ConstSharedPtr self_odometry_ptr;
  BppParamPtr bpp_param_ptr;
  LCParamPtr lc_param_ptr;
  LanesPtr lanes_ptr;
  LanesPolygonPtr lanes_polygon_ptr;
  TransientData transient_data;
  PathWithLaneId current_lanes_path;
  PathWithLaneId target_lanes_path;
  ModuleType lc_type;
  Direction direction;

  [[nodiscard]] const Pose & get_ego_pose() const { return self_odometry_ptr->pose.pose; }

  [[nodiscard]] const Twist & get_ego_twist() const { return self_odometry_ptr->twist.twist; }

  [[nodiscard]] double get_ego_speed(bool use_norm = false) const
  {
    if (!use_norm) {
      return get_ego_twist().linear.x;
    }

    const auto x = get_ego_twist().linear.x;
    const auto y = get_ego_twist().linear.y;
    return std::hypot(x, y);
  }

  [[nodiscard]] bool is_data_available() const
  {
    return route_handler_ptr && self_odometry_ptr && bpp_param_ptr && lc_param_ptr && lanes_ptr &&
           lanes_polygon_ptr;
  }

  [[nodiscard]] bool is_lanes_available() const
  {
    return lanes_ptr && !lanes_ptr->current.empty() && !lanes_ptr->target.empty() &&
           !lanes_ptr->target_neighbor.empty();
  }
};
using CommonDataPtr = std::shared_ptr<CommonData>;
}  // namespace autoware::behavior_path_planner::lane_change

namespace autoware::behavior_path_planner
{
using autoware_perception_msgs::msg::PredictedObject;
using utils::path_safety_checker::ExtendedPredictedObjects;
using LaneChangeModuleType = lane_change::ModuleType;
using LaneChangeParameters = lane_change::Parameters;
using LaneChangeStates = lane_change::States;
using LaneChangePhaseInfo = lane_change::PhaseInfo;
using LaneChangePhaseMetrics = lane_change::PhaseMetrics;
using LaneChangeInfo = lane_change::Info;
using FilteredLanesObjects = lane_change::FilteredLanesObjects;
using LateralAccelerationMap = lane_change::LateralAccelerationMap;
}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__STRUCTS__DATA_HPP_

// Copyright 2024 Tier IV, Inc.
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

#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/utilization/path_utilization.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <rclcpp/clock.hpp>

#include <boost/geometry/geometry.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_core/geometry/Polygon.h>

#include <limits>
#include <vector>

namespace autoware::behavior_velocity_planner::no_stopping_area
{

bool is_vehicle_type(const autoware_perception_msgs::msg::PredictedObject & object)
{
  // TODO(anyone): should we switch to using functions from the common object_recognition_utils ?
  return !object.classification.empty() &&
         (object.classification.front().label ==
            autoware_perception_msgs::msg::ObjectClassification::CAR ||
          object.classification.front().label ==
            autoware_perception_msgs::msg::ObjectClassification::BUS ||
          object.classification.front().label ==
            autoware_perception_msgs::msg::ObjectClassification::TRUCK ||
          object.classification.front().label ==
            autoware_perception_msgs::msg::ObjectClassification::TRAILER ||
          object.classification.front().label ==
            autoware_perception_msgs::msg::ObjectClassification::MOTORCYCLE);
}

void insert_stop_point(
  tier4_planning_msgs::msg::PathWithLaneId & path, const PathIndexWithPose & stop_point)
{
  auto insert_idx = stop_point.first + 1UL;
  const auto & stop_pose = stop_point.second;

  // To PathPointWithLaneId
  tier4_planning_msgs::msg::PathPointWithLaneId stop_point_with_lane_id;
  stop_point_with_lane_id = path.points.at(insert_idx);
  stop_point_with_lane_id.point.pose = stop_pose;
  stop_point_with_lane_id.point.longitudinal_velocity_mps = 0.0;

  // Insert stop point or replace with zero velocity
  planning_utils::insertVelocity(path, stop_point_with_lane_id, 0.0, insert_idx);
}

std::optional<LineString2d> generate_stop_line(
  const tier4_planning_msgs::msg::PathWithLaneId & path,
  const lanelet::ConstPolygons3d & no_stopping_areas, const double ego_width,
  const double stop_line_margin)
{
  LineString2d stop_line;
  for (const auto & no_stopping_area : no_stopping_areas) {
    const auto & area_poly = lanelet::utils::to2D(no_stopping_area).basicPolygon();
    for (size_t i = 0; i < path.points.size() - 1; ++i) {
      const auto p0 = path.points.at(i).point.pose.position;
      const auto p1 = path.points.at(i + 1).point.pose.position;
      const LineString2d line{{p0.x, p0.y}, {p1.x, p1.y}};
      std::vector<Point2d> collision_points;
      boost::geometry::intersection(area_poly, line, collision_points);
      if (!collision_points.empty()) {
        const double yaw = autoware::universe_utils::calcAzimuthAngle(p0, p1);
        const double w = ego_width;
        const double l = stop_line_margin;
        stop_line.emplace_back(
          -l * std::cos(yaw) + collision_points.front().x() + w * std::cos(yaw + M_PI_2),
          collision_points.front().y() + w * std::sin(yaw + M_PI_2));
        stop_line.emplace_back(
          -l * std::cos(yaw) + collision_points.front().x() + w * std::cos(yaw - M_PI_2),
          collision_points.front().y() + w * std::sin(yaw - M_PI_2));
        return stop_line;
      }
    }
  }
  return {};
}

bool is_stoppable(
  PassJudge & pass_judge, const geometry_msgs::msg::Pose & self_pose,
  const geometry_msgs::msg::Pose & line_pose, const EgoData & ego_data,
  const rclcpp::Logger & logger, rclcpp::Clock & clock)
{
  // compute pass_judge_line_distance
  const double stoppable_distance = planning_utils::calcJudgeLineDistWithJerkLimit(
    ego_data.current_velocity, ego_data.current_acceleration, ego_data.max_stop_acc,
    ego_data.max_stop_jerk, ego_data.delay_response_time);
  const double signed_arc_length =
    arc_lane_utils::calcSignedDistance(self_pose, line_pose.position);
  const bool distance_stoppable = stoppable_distance < signed_arc_length;
  const bool slow_velocity = ego_data.current_velocity < 2.0;
  // ego vehicle is high speed and can't stop before stop line -> GO
  // stoppable or not is judged only once
  RCLCPP_DEBUG(
    logger, "stoppable_dist: %lf signed_arc_length: %lf", stoppable_distance, signed_arc_length);
  if (!distance_stoppable && !pass_judge.pass_judged) {
    pass_judge.pass_judged = true;
    // can't stop using maximum brake consider jerk limit
    if (!slow_velocity) {
      // pass through
      pass_judge.is_stoppable = false;
      RCLCPP_WARN_THROTTLE(
        logger, clock, 1000, "[NoStoppingArea] can't stop in front of no stopping area");
    } else {
      pass_judge.is_stoppable = true;
    }
  }
  return pass_judge.is_stoppable;
}

Polygon2d generate_ego_no_stopping_area_lane_polygon(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const geometry_msgs::msg::Pose & ego_pose,
  const lanelet::autoware::NoStoppingArea & no_stopping_area_reg_elem, const double margin,
  const double max_polygon_length, const double path_expand_width, const rclcpp::Logger & logger,
  rclcpp::Clock & clock)
{
  Polygon2d ego_area;  // open polygon
  double dist_from_start_sum = 0.0;
  constexpr double interpolation_interval = 0.5;
  bool is_in_area = false;
  tier4_planning_msgs::msg::PathWithLaneId interpolated_path;
  if (!splineInterpolate(path, interpolation_interval, interpolated_path, logger)) {
    return ego_area;
  }
  const auto & pp = interpolated_path.points;
  /* calc closest index */
  const auto closest_idx_opt = autoware::motion_utils::findNearestIndex(pp, ego_pose, 3.0, M_PI_4);
  if (!closest_idx_opt) {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(
      logger, clock, 1000 /* ms */, "autoware::motion_utils::findNearestIndex fail");
    return ego_area;
  }
  const size_t closest_idx = closest_idx_opt.value();

  const int num_ignore_nearest = 1;  // Do not consider nearest lane polygon
  size_t ego_area_start_idx = closest_idx + num_ignore_nearest;
  // return if area size is not intentional
  if (no_stopping_area_reg_elem.noStoppingAreas().size() != 1) {
    return ego_area;
  }
  const auto no_stopping_area = no_stopping_area_reg_elem.noStoppingAreas().front();
  for (size_t i = ego_area_start_idx; i < pp.size() - 1; ++i) {
    dist_from_start_sum += autoware::universe_utils::calcDistance2d(pp.at(i), pp.at(i - 1));
    const auto & p = pp.at(i).point.pose.position;
    // TODO(someone): within will skip points on the edge of polygons so some points can be skipped
    // depending on the interpolation
    if (bg::within(Point2d{p.x, p.y}, lanelet::utils::to2D(no_stopping_area).basicPolygon())) {
      is_in_area = true;
      break;
    }
    if (dist_from_start_sum > max_polygon_length) {
      return ego_area;
    }
    ++ego_area_start_idx;
  }
  if (ego_area_start_idx > num_ignore_nearest) {
    // TODO(someone): check if this is a bug
    // this -1 causes pp[ego_area_start_idx] to be outside the no_stopping_area
    // it causes "dist_from_area" to count the first point in the next loop
    // moreover it causes the "dist_from_start_sum" to count the same point twice
    ego_area_start_idx--;
  }
  if (!is_in_area) {
    return ego_area;
  }
  double dist_from_area_sum = 0.0;
  // decide end idx with extract distance
  size_t ego_area_end_idx = ego_area_start_idx;
  for (size_t i = ego_area_start_idx; i < pp.size() - 1; ++i) {
    dist_from_start_sum += autoware::universe_utils::calcDistance2d(pp.at(i), pp.at(i - 1));
    const auto & p = pp.at(i).point.pose.position;
    // TODO(someone): within will skip points on the edge of polygons so some points can be skipped
    // depending on the interpolation
    if (!bg::within(Point2d{p.x, p.y}, lanelet::utils::to2D(no_stopping_area).basicPolygon())) {
      dist_from_area_sum += autoware::universe_utils::calcDistance2d(pp.at(i), pp.at(i - 1));
    }
    if (dist_from_start_sum > max_polygon_length || dist_from_area_sum > margin) {
      break;
    }
    ++ego_area_end_idx;
  }

  ego_area = planning_utils::generatePathPolygon(
    interpolated_path, ego_area_start_idx, ego_area_end_idx, path_expand_width);
  return ego_area;
}

bool check_stop_lines_in_no_stopping_area(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const Polygon2d & poly,
  no_stopping_area::DebugData & debug_data)
{
  const double stop_vel = std::numeric_limits<float>::min();

  // if the detected stop point is near goal, it's ignored.
  static constexpr double close_to_goal_distance = 1.0;

  // stuck points by stop line
  for (size_t i = 0; i + 1 < path.points.size(); ++i) {
    const auto p0 = path.points.at(i).point.pose.position;
    const auto p1 = path.points.at(i + 1).point.pose.position;
    const auto v0 = path.points.at(i).point.longitudinal_velocity_mps;
    const auto v1 = path.points.at(i + 1).point.longitudinal_velocity_mps;
    if (v0 > stop_vel && v1 > stop_vel) {
      continue;
    }
    // judge if stop point p0 is near goal, by its distance to the path end.
    const double dist_to_path_end =
      autoware::motion_utils::calcSignedArcLength(path.points, i, path.points.size() - 1);
    if (dist_to_path_end < close_to_goal_distance) {
      // exit with false, cause position is near goal.
      return false;
    }

    const LineString2d line{{p0.x, p0.y}, {p1.x, p1.y}};
    std::vector<Point2d> collision_points;
    bg::intersection(poly, line, collision_points);
    if (!collision_points.empty()) {
      geometry_msgs::msg::Point point;
      point.x = collision_points.front().x();
      point.y = collision_points.front().y();
      point.z = 0.0;
      debug_data.stuck_points.emplace_back(point);
      return true;
    }
  }
  return false;
}

std::optional<LineString2d> get_stop_line_geometry2d(
  const tier4_planning_msgs::msg::PathWithLaneId & path,
  const lanelet::autoware::NoStoppingArea & no_stopping_area_reg_elem,
  const double stop_line_margin, const double stop_line_extend_length, const double vehicle_width)
{
  const auto & stop_line = no_stopping_area_reg_elem.stopLine();
  if (stop_line && stop_line->size() >= 2) {
    // get stop line from map
    return planning_utils::extendLine(
      stop_line.value()[0], stop_line.value()[1], stop_line_extend_length);
  }
  return generate_stop_line(
    path, no_stopping_area_reg_elem.noStoppingAreas(), vehicle_width, stop_line_margin);
}

}  // namespace autoware::behavior_velocity_planner::no_stopping_area

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

#include <motion_utils/trajectory/trajectory.hpp>
#include <scene_module/detection_area/scene.hpp>
#include <utilization/util.hpp>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>
#endif

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace behavior_velocity_planner
{
namespace bg = boost::geometry;
using motion_utils::calcSignedArcLength;
using motion_utils::findNearestSegmentIndex;
using motion_utils::insertTargetPoint;

namespace
{
double calcSignedDistance(const geometry_msgs::msg::Pose & p1, const geometry_msgs::msg::Point & p2)
{
  Eigen::Affine3d map2p1;
  tf2::fromMsg(p1, map2p1);
  const auto basecoords_p2 = map2p1.inverse() * Eigen::Vector3d(p2.x, p2.y, p2.z);
  return basecoords_p2.x() >= 0 ? basecoords_p2.norm() : -basecoords_p2.norm();
}

double calcYawFromPoints(
  const geometry_msgs::msg::Point & p_front, const geometry_msgs::msg::Point & p_back)
{
  return std::atan2(p_back.y - p_front.y, p_back.x - p_front.x);
}

boost::optional<Point2d> getNearestCollisionPoint(
  const LineString2d & stop_line, const LineString2d & path_segment)
{
  // Find all collision points
  std::vector<Point2d> collision_points;
  bg::intersection(stop_line, path_segment, collision_points);
  if (collision_points.empty()) {
    return {};
  }

  // To dist list
  std::vector<double> dist_list;
  dist_list.reserve(collision_points.size());
  std::transform(
    collision_points.cbegin(), collision_points.cend(), std::back_inserter(dist_list),
    [&path_segment](const Point2d & collision_point) {
      return bg::distance(path_segment.front(), collision_point);
    });

  // Find nearest collision point
  const auto min_itr = std::min_element(dist_list.cbegin(), dist_list.cend());
  const auto min_idx = std::distance(dist_list.cbegin(), min_itr);

  return collision_points.at(min_idx);
}

boost::optional<PathIndexWithPoint2d> findCollisionSegment(
  const PathWithLaneId & path, const LineString2d & stop_line)
{
  for (size_t i = 0; i < path.points.size() - 1; ++i) {
    const auto & p1 = path.points.at(i).point.pose.position;      // Point before collision point
    const auto & p2 = path.points.at(i + 1).point.pose.position;  // Point after collision point

    const LineString2d path_segment = {{p1.x, p1.y}, {p2.x, p2.y}};

    const auto nearest_collision_point = getNearestCollisionPoint(stop_line, path_segment);
    if (nearest_collision_point) {
      return std::make_pair(i, *nearest_collision_point);
    }
  }

  return {};
}

boost::optional<PathIndexWithOffset> findForwardOffsetSegment(
  const PathWithLaneId & path, const size_t base_idx, const double offset_length)
{
  double sum_length = 0.0;
  for (size_t i = base_idx; i < path.points.size() - 1; ++i) {
    const auto p_front = to_bg2d(path.points.at(i).point.pose.position);
    const auto p_back = to_bg2d(path.points.at(i + 1).point.pose.position);

    const auto segment_length = bg::distance(p_front, p_back);
    sum_length += segment_length;

    // If it's over offset point, return front index and remain offset length
    if (sum_length >= offset_length) {
      const auto remain_length = sum_length - offset_length;
      return std::make_pair(i, segment_length - remain_length);
    }
  }

  // No enough path length
  return {};
}

boost::optional<PathIndexWithOffset> findBackwardOffsetSegment(
  const PathWithLaneId & path, const size_t base_idx, const double offset_length)
{
  double sum_length = 0.0;
  const auto start = static_cast<std::int32_t>(base_idx) - 1;
  for (std::int32_t i = start; i >= 0; --i) {
    const auto p_front = to_bg2d(path.points.at(i).point.pose.position);
    const auto p_back = to_bg2d(path.points.at(i + 1).point.pose.position);

    sum_length += bg::distance(p_front, p_back);

    // If it's over offset point, return front index and remain offset length
    if (sum_length >= offset_length) {
      const auto k = static_cast<std::size_t>(i);
      return std::make_pair(k, sum_length - offset_length);
    }
  }

  // No enough path length
  return {};
}

boost::optional<PathIndexWithOffset> findOffsetSegment(
  const PathWithLaneId & path, const PathIndexWithPoint2d & collision_segment,
  const double offset_length)
{
  const size_t & collision_idx = collision_segment.first;
  const Point2d & collision_point = collision_segment.second;
  const auto p_front = to_bg2d(path.points.at(collision_idx).point.pose.position);
  const auto p_back = to_bg2d(path.points.at(collision_idx + 1).point.pose.position);

  if (offset_length >= 0) {
    return findForwardOffsetSegment(
      path, collision_idx, offset_length + bg::distance(p_front, collision_point));
  } else {
    return findBackwardOffsetSegment(
      path, collision_idx + 1, -offset_length + bg::distance(p_back, collision_point));
  }
}

geometry_msgs::msg::Pose calcTargetPose(
  const PathWithLaneId & path, const PathIndexWithOffset & offset_segment)
{
  const size_t offset_idx = offset_segment.first;
  const double remain_offset_length = offset_segment.second;
  const auto & p_front = path.points.at(offset_idx).point.pose.position;
  const auto & p_back = path.points.at(offset_idx + 1).point.pose.position;

  // To Eigen point
  const auto p_eigen_front = Eigen::Vector2d(p_front.x, p_front.y);
  const auto p_eigen_back = Eigen::Vector2d(p_back.x, p_back.y);

  // Calculate interpolation ratio
  const auto interpolate_ratio = remain_offset_length / (p_eigen_back - p_eigen_front).norm();

  // Add offset to front point
  const auto target_point_2d = p_eigen_front + interpolate_ratio * (p_eigen_back - p_eigen_front);
  const double interpolated_z = p_front.z + interpolate_ratio * (p_back.z - p_front.z);

  // Calculate orientation so that X-axis would be along the trajectory
  tf2::Quaternion quat;
  quat.setRPY(0, 0, calcYawFromPoints(p_front, p_back));

  // To Pose
  geometry_msgs::msg::Pose target_pose;
  target_pose.position.x = target_point_2d.x();
  target_pose.position.y = target_point_2d.y();
  target_pose.position.z = interpolated_z;
  target_pose.orientation = tf2::toMsg(quat);

  return target_pose;
}
}  // namespace

DetectionAreaModule::DetectionAreaModule(
  const int64_t module_id, const lanelet::autoware::DetectionArea & detection_area_reg_elem,
  const PlannerParam & planner_param, const rclcpp::Logger logger,
  const rclcpp::Clock::SharedPtr clock)
: SceneModuleInterface(module_id, logger, clock),
  detection_area_reg_elem_(detection_area_reg_elem),
  state_(State::GO),
  planner_param_(planner_param)
{
}

LineString2d DetectionAreaModule::getStopLineGeometry2d() const
{
  const lanelet::ConstLineString3d stop_line = detection_area_reg_elem_.stopLine();
  return planning_utils::extendLine(
    stop_line[0], stop_line[1], planner_data_->stop_line_extend_length);
}

bool DetectionAreaModule::modifyPathVelocity(PathWithLaneId * path, StopReason * stop_reason)
{
  // Store original path
  const auto original_path = *path;

  // Reset data
  debug_data_ = DebugData();
  debug_data_.base_link2front = planner_data_->vehicle_info_.max_longitudinal_offset_m;
  *stop_reason = planning_utils::initializeStopReason(StopReason::DETECTION_AREA);

  // Find obstacles in detection area
  const auto obstacle_points = getObstaclePoints();
  debug_data_.obstacle_points = obstacle_points;
  if (!obstacle_points.empty()) {
    last_obstacle_found_time_ = std::make_shared<const rclcpp::Time>(clock_->now());
  }

  // Get stop line geometry
  const auto stop_line = getStopLineGeometry2d();

  // Get self pose
  const auto & self_pose = planner_data_->current_pose.pose;

  // Get stop point
  const auto stop_point = createTargetPoint(original_path, stop_line, planner_param_.stop_margin);
  if (!stop_point) {
    return true;
  }

  const auto is_stopped = planner_data_->isVehicleStopped(0.0);

  auto stop_pose = stop_point->second;
  const auto stop_dist = calcSignedArcLength(path->points, self_pose.position, stop_pose.position);

  // Don't re-approach when the ego stops closer to the stop point than hold_stop_margin_distance
  if (is_stopped && stop_dist < planner_param_.hold_stop_margin_distance) {
    stop_pose = self_pose;
  }

  setDistance(stop_dist);

  // Check state
  setSafe(canClearStopState());
  if (isActivated()) {
    state_ = State::GO;
    last_obstacle_found_time_ = {};
    return true;
  }

  // Force ignore objects after dead_line
  if (planner_param_.use_dead_line) {
    // Use '-' for margin because it's the backward distance from stop line
    const auto dead_line_point =
      createTargetPoint(original_path, stop_line, -planner_param_.dead_line_margin);

    if (dead_line_point) {
      const auto & dead_line_pose = dead_line_point->second;
      debug_data_.dead_line_poses.push_back(dead_line_pose);

      if (isOverLine(original_path, self_pose, dead_line_pose)) {
        RCLCPP_WARN(logger_, "[detection_area] vehicle is over dead line");
        setSafe(true);
        return true;
      }
    }
  }

  // Ignore objects detected after stop_line if not in STOP state
  if (state_ != State::STOP && isOverLine(original_path, self_pose, stop_point->second)) {
    setSafe(true);
    return true;
  }

  // Ignore objects if braking distance is not enough
  if (planner_param_.use_pass_judge_line) {
    if (state_ != State::STOP && !hasEnoughBrakingDistance(self_pose, stop_point->second)) {
      RCLCPP_WARN_THROTTLE(
        logger_, *clock_, std::chrono::milliseconds(1000).count(),
        "[detection_area] vehicle is over stop border");
      setSafe(true);
      return true;
    }
  }

  // Insert stop point
  state_ = State::STOP;
  insertStopPoint(stop_pose, *path);

  // For virtual wall
  debug_data_.stop_poses.push_back(stop_point->second);

  // Create StopReason
  {
    StopFactor stop_factor{};
    stop_factor.stop_pose = stop_point->second;
    stop_factor.stop_factor_points = obstacle_points;
    planning_utils::appendStopReason(stop_factor, stop_reason);
  }

  // Create legacy StopReason
  {
    const auto insert_idx = stop_point->first + 1;

    if (
      !first_stop_path_point_index_ ||
      static_cast<int>(insert_idx) < first_stop_path_point_index_) {
      debug_data_.first_stop_pose = stop_point->second;
      first_stop_path_point_index_ = static_cast<int>(insert_idx);
    }
  }

  return true;
}

std::vector<geometry_msgs::msg::Point> DetectionAreaModule::getObstaclePoints() const
{
  std::vector<geometry_msgs::msg::Point> obstacle_points;

  const auto detection_areas = detection_area_reg_elem_.detectionAreas();
  const auto & points = *(planner_data_->no_ground_pointcloud);

  for (const auto & detection_area : detection_areas) {
    for (const auto p : points) {
      if (bg::within(Point2d{p.x, p.y}, lanelet::utils::to2D(detection_area).basicPolygon())) {
        obstacle_points.push_back(planning_utils::toRosPoint(p));
        // get all obstacle point becomes high computation cost so skip if any point is found
        break;
      }
    }
  }

  return obstacle_points;
}

bool DetectionAreaModule::canClearStopState() const
{
  // vehicle can clear stop state if the obstacle has never appeared in detection area
  if (!last_obstacle_found_time_) {
    return true;
  }

  // vehicle can clear stop state if the certain time has passed since the obstacle disappeared
  const auto elapsed_time = clock_->now() - *last_obstacle_found_time_;
  if (elapsed_time.seconds() >= planner_param_.state_clear_time) {
    return true;
  }

  // rollback in simulation mode
  if (elapsed_time.seconds() < 0.0) {
    return true;
  }

  return false;
}

bool DetectionAreaModule::isOverLine(
  const PathWithLaneId & path, const geometry_msgs::msg::Pose & self_pose,
  const geometry_msgs::msg::Pose & line_pose) const
{
  return calcSignedArcLength(path.points, self_pose.position, line_pose.position) < 0;
}

bool DetectionAreaModule::hasEnoughBrakingDistance(
  const geometry_msgs::msg::Pose & self_pose, const geometry_msgs::msg::Pose & line_pose) const
{
  // get vehicle info and compute pass_judge_line_distance
  const auto current_velocity = planner_data_->current_velocity->twist.linear.x;
  const double max_acc = planner_data_->max_stop_acceleration_threshold;
  const double delay_response_time = planner_data_->delay_response_time;
  const double pass_judge_line_distance =
    planning_utils::calcJudgeLineDistWithAccLimit(current_velocity, max_acc, delay_response_time);

  return calcSignedDistance(self_pose, line_pose.position) > pass_judge_line_distance;
}

void DetectionAreaModule::insertStopPoint(
  const geometry_msgs::msg::Pose & stop_pose, PathWithLaneId & path) const
{
  const size_t base_idx = findNearestSegmentIndex(path.points, stop_pose.position);
  const auto insert_idx = insertTargetPoint(base_idx, stop_pose.position, path.points);

  if (!insert_idx) {
    return;
  }

  for (size_t i = insert_idx.get(); i < path.points.size(); ++i) {
    path.points.at(i).point.longitudinal_velocity_mps = 0.0;
  }
}

boost::optional<PathIndexWithPose> DetectionAreaModule::createTargetPoint(
  const PathWithLaneId & path, const LineString2d & stop_line, const double margin) const
{
  // Find collision segment
  const auto collision_segment = findCollisionSegment(path, stop_line);
  if (!collision_segment) {
    // No collision
    return {};
  }

  // Calculate offset length from stop line
  // Use '-' to make the positive direction is forward
  const double offset_length = -(margin + planner_data_->vehicle_info_.max_longitudinal_offset_m);

  // Find offset segment
  const auto offset_segment = findOffsetSegment(path, *collision_segment, offset_length);
  if (!offset_segment) {
    // No enough path length
    return {};
  }

  const auto front_idx = offset_segment->first;
  const auto target_pose = calcTargetPose(path, *offset_segment);

  return std::make_pair(front_idx, target_pose);
}
}  // namespace behavior_velocity_planner

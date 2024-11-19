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

#include "autoware/behavior_path_lane_change_module/utils/utils.hpp"

#include "autoware/behavior_path_lane_change_module/utils/data_structs.hpp"
#include "autoware/behavior_path_lane_change_module/utils/path.hpp"
#include "autoware/behavior_path_planner_common/parameters.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/safety_check.hpp"
#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"
#include "autoware/behavior_path_planner_common/utils/path_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/traffic_light_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"
#include "autoware/object_recognition_utils/predicted_path_utils.hpp"
#include "autoware/universe_utils/math/unit_conversion.hpp"

#include <autoware/motion_utils/trajectory/interpolation.hpp>
#include <autoware/motion_utils/trajectory/path_with_lane_id.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <autoware_vehicle_info_utils/vehicle_info.hpp>
#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/detail/pose__struct.hpp>

#include <boost/geometry/algorithms/detail/disjoint/interface.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/LineString.h>
#include <lanelet2_core/geometry/Point.h>
#include <lanelet2_core/geometry/Polygon.h>
#include <tf2/utils.h>
#include <tf2_ros/transform_listener.h>

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace autoware::behavior_path_planner::utils::lane_change
{
using autoware::route_handler::RouteHandler;
using autoware::universe_utils::LineString2d;
using autoware::universe_utils::Polygon2d;
using autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObjects;
using geometry_msgs::msg::Pose;
using tier4_planning_msgs::msg::PathWithLaneId;

using lanelet::ArcCoordinates;
using tier4_planning_msgs::msg::PathPointWithLaneId;

rclcpp::Logger get_logger()
{
  constexpr const char * name{"lane_change.utils"};
  static rclcpp::Logger logger = rclcpp::get_logger(name);
  return logger;
}

bool is_mandatory_lane_change(const ModuleType lc_type)
{
  return lc_type == LaneChangeModuleType::NORMAL ||
         lc_type == LaneChangeModuleType::AVOIDANCE_BY_LANE_CHANGE;
}

double calcLaneChangeResampleInterval(
  const double lane_changing_length, const double lane_changing_velocity)
{
  constexpr auto min_resampling_points{30.0};
  constexpr auto resampling_dt{0.2};
  return std::max(
    lane_changing_length / min_resampling_points, lane_changing_velocity * resampling_dt);
}

void setPrepareVelocity(
  PathWithLaneId & prepare_segment, const double current_velocity, const double prepare_velocity)
{
  if (current_velocity < prepare_velocity) {
    // acceleration
    for (auto & point : prepare_segment.points) {
      point.point.longitudinal_velocity_mps =
        std::min(point.point.longitudinal_velocity_mps, static_cast<float>(prepare_velocity));
    }
  } else {
    // deceleration
    prepare_segment.points.back().point.longitudinal_velocity_mps = std::min(
      prepare_segment.points.back().point.longitudinal_velocity_mps,
      static_cast<float>(prepare_velocity));
  }
}

lanelet::ConstLanelets get_target_neighbor_lanes(
  const RouteHandler & route_handler, const lanelet::ConstLanelets & current_lanes,
  const LaneChangeModuleType & type)
{
  lanelet::ConstLanelets neighbor_lanes;

  for (const auto & current_lane : current_lanes) {
    const auto mandatory_lane_change = is_mandatory_lane_change(type);
    if (route_handler.getNumLaneToPreferredLane(current_lane) != 0) {
      if (mandatory_lane_change) {
        neighbor_lanes.push_back(current_lane);
      }
    } else {
      if (!mandatory_lane_change) {
        neighbor_lanes.push_back(current_lane);
      }
    }
  }

  return neighbor_lanes;
}

bool isPathInLanelets(
  const PathWithLaneId & path, const lanelet::ConstLanelets & current_lanes,
  const lanelet::ConstLanelets & target_lanes)
{
  const auto current_lane_poly =
    lanelet::utils::getPolygonFromArcLength(current_lanes, 0, std::numeric_limits<double>::max());
  const auto target_lane_poly =
    lanelet::utils::getPolygonFromArcLength(target_lanes, 0, std::numeric_limits<double>::max());
  const auto current_lane_poly_2d = lanelet::utils::to2D(current_lane_poly).basicPolygon();
  const auto target_lane_poly_2d = lanelet::utils::to2D(target_lane_poly).basicPolygon();
  for (const auto & pt : path.points) {
    const lanelet::BasicPoint2d ll_pt(pt.point.pose.position.x, pt.point.pose.position.y);
    const auto is_in_current = boost::geometry::covered_by(ll_pt, current_lane_poly_2d);
    if (is_in_current) {
      continue;
    }
    const auto is_in_target = boost::geometry::covered_by(ll_pt, target_lane_poly_2d);
    if (!is_in_target) {
      return false;
    }
  }
  return true;
}

bool path_footprint_exceeds_target_lane_bound(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & path, const VehicleInfo & ego_info,
  const double margin)
{
  if (common_data_ptr->direction == Direction::NONE || path.points.empty()) {
    return false;
  }

  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const bool is_left = common_data_ptr->direction == Direction::LEFT;

  const auto combined_target_lane = lanelet::utils::combineLaneletsShape(target_lanes);

  for (const auto & path_point : path.points) {
    const auto & pose = path_point.point.pose;
    const auto front_vertex = getEgoFrontVertex(pose, ego_info, is_left);

    const auto sign = is_left ? -1.0 : 1.0;
    const auto dist_to_boundary =
      sign * utils::getSignedDistanceFromLaneBoundary(combined_target_lane, front_vertex, is_left);

    if (dist_to_boundary < margin) {
      RCLCPP_DEBUG(get_logger(), "Path footprint exceeds target lane boundary");
      return true;
    }
  }

  return false;
}

std::optional<LaneChangePath> construct_candidate_path(
  const CommonDataPtr & common_data_ptr, const LaneChangeInfo & lane_change_info,
  const PathWithLaneId & prepare_segment, const PathWithLaneId & target_lane_reference_path,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids)
{
  const auto & shift_line = lane_change_info.shift_line;
  const auto terminal_lane_changing_velocity = lane_change_info.terminal_lane_changing_velocity;
  const auto longitudinal_acceleration = lane_change_info.longitudinal_acceleration;
  const auto lane_change_velocity = lane_change_info.velocity;

  PathShifter path_shifter;
  path_shifter.setPath(target_lane_reference_path);
  path_shifter.addShiftLine(shift_line);
  path_shifter.setLongitudinalAcceleration(longitudinal_acceleration.lane_changing);
  ShiftedPath shifted_path;

  // offset front side
  bool offset_back = false;

  const auto initial_lane_changing_velocity = lane_change_velocity.lane_changing;
  path_shifter.setVelocity(initial_lane_changing_velocity);
  path_shifter.setLateralAccelerationLimit(std::abs(lane_change_info.lateral_acceleration));

  if (!path_shifter.generate(&shifted_path, offset_back)) {
    RCLCPP_DEBUG(get_logger(), "Failed to generate shifted path.");
  }

  // TODO(Zulfaqar Azmi): have to think of a more feasible solution for points being remove by path
  // shifter.
  if (shifted_path.path.points.size() < shift_line.end_idx + 1) {
    RCLCPP_DEBUG(get_logger(), "Path points are removed by PathShifter.");
    return std::nullopt;
  }

  LaneChangePath candidate_path;
  candidate_path.info = lane_change_info;

  const auto lane_change_end_idx = autoware::motion_utils::findNearestIndex(
    shifted_path.path.points, candidate_path.info.lane_changing_end);

  if (!lane_change_end_idx) {
    RCLCPP_DEBUG(get_logger(), "Lane change end idx not found on target path.");
    return std::nullopt;
  }

  for (size_t i = 0; i < shifted_path.path.points.size(); ++i) {
    auto & point = shifted_path.path.points.at(i);
    if (i < *lane_change_end_idx) {
      point.lane_ids = replaceWithSortedIds(point.lane_ids, sorted_lane_ids);
      point.point.longitudinal_velocity_mps = std::min(
        point.point.longitudinal_velocity_mps, static_cast<float>(terminal_lane_changing_velocity));
      continue;
    }
    const auto nearest_idx =
      autoware::motion_utils::findNearestIndex(target_lane_reference_path.points, point.point.pose);
    point.lane_ids = target_lane_reference_path.points.at(*nearest_idx).lane_ids;
  }

  // TODO(Yutaka Shimizu): remove this flag after make the isPathInLanelets faster
  const bool enable_path_check_in_lanelet = false;

  // check candidate path is in lanelet
  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  if (
    enable_path_check_in_lanelet &&
    !isPathInLanelets(shifted_path.path, current_lanes, target_lanes)) {
    return std::nullopt;
  }

  if (prepare_segment.points.size() > 1 && shifted_path.path.points.size() > 1) {
    const auto & prepare_segment_second_last_point =
      std::prev(prepare_segment.points.end() - 1)->point.pose;
    const auto & lane_change_start_from_shifted =
      std::next(shifted_path.path.points.begin())->point.pose;
    const auto yaw_diff2 = std::abs(autoware::universe_utils::normalizeRadian(
      tf2::getYaw(prepare_segment_second_last_point.orientation) -
      tf2::getYaw(lane_change_start_from_shifted.orientation)));
    if (yaw_diff2 > autoware::universe_utils::deg2rad(5.0)) {
      RCLCPP_DEBUG(
        get_logger(), "Excessive yaw difference %.3f which exceeds the 5 degrees threshold.",
        autoware::universe_utils::rad2deg(yaw_diff2));
      return std::nullopt;
    }
  }

  candidate_path.path = utils::combinePath(prepare_segment, shifted_path.path);
  candidate_path.shifted_path = shifted_path;

  return std::optional<LaneChangePath>{candidate_path};
}

PathWithLaneId get_reference_path_from_target_Lane(
  const CommonDataPtr & common_data_ptr, const Pose & lane_changing_start_pose,
  const double lane_changing_length, const double resample_interval)
{
  const auto & route_handler = *common_data_ptr->route_handler_ptr;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const auto target_lane_length = common_data_ptr->transient_data.target_lane_length;
  const auto is_goal_in_route = common_data_ptr->lanes_ptr->target_lane_in_goal_section;
  const auto next_lc_buffer = common_data_ptr->transient_data.next_dist_buffer.min;
  const auto forward_path_length = common_data_ptr->bpp_param_ptr->forward_path_length;

  const ArcCoordinates lane_change_start_arc_position =
    lanelet::utils::getArcCoordinates(target_lanes, lane_changing_start_pose);

  const double s_start = lane_change_start_arc_position.length;
  const double s_end = std::invoke([&]() {
    const auto dist_from_lc_start = s_start + lane_changing_length + forward_path_length;
    if (is_goal_in_route) {
      const double s_goal =
        lanelet::utils::getArcCoordinates(target_lanes, route_handler.getGoalPose()).length -
        next_lc_buffer;
      return std::min(dist_from_lc_start, s_goal);
    }
    return std::min(dist_from_lc_start, target_lane_length - next_lc_buffer);
  });

  constexpr double epsilon = 1e-4;
  if (s_end - s_start + epsilon < lane_changing_length) {
    return PathWithLaneId();
  }

  const auto lane_changing_reference_path =
    route_handler.getCenterLinePath(target_lanes, s_start, s_end);

  return utils::resamplePathWithSpline(
    lane_changing_reference_path, resample_interval, true, {0.0, lane_changing_length});
}

ShiftLine get_lane_changing_shift_line(
  const Pose & lane_changing_start_pose, const Pose & lane_changing_end_pose,
  const PathWithLaneId & reference_path, const double shift_length)
{
  ShiftLine shift_line;
  shift_line.end_shift_length = shift_length;
  shift_line.start = lane_changing_start_pose;
  shift_line.end = lane_changing_end_pose;
  shift_line.start_idx = autoware::motion_utils::findNearestIndex(
    reference_path.points, lane_changing_start_pose.position);
  shift_line.end_idx = autoware::motion_utils::findNearestIndex(
    reference_path.points, lane_changing_end_pose.position);

  return shift_line;
}

std::vector<DrivableLanes> generateDrivableLanes(
  const RouteHandler & route_handler, const lanelet::ConstLanelets & current_lanes,
  const lanelet::ConstLanelets & lane_change_lanes)
{
  size_t current_lc_idx = 0;
  std::vector<DrivableLanes> drivable_lanes(current_lanes.size());
  for (size_t i = 0; i < current_lanes.size(); ++i) {
    const auto & current_lane = current_lanes.at(i);
    drivable_lanes.at(i).left_lane = current_lane;
    drivable_lanes.at(i).right_lane = current_lane;

    const auto left_lane = route_handler.getLeftLanelet(current_lane, false, false);
    const auto right_lane = route_handler.getRightLanelet(current_lane, false, false);
    if (!left_lane && !right_lane) {
      continue;
    }

    for (size_t lc_idx = current_lc_idx; lc_idx < lane_change_lanes.size(); ++lc_idx) {
      const auto & lc_lane = lane_change_lanes.at(lc_idx);
      if (left_lane && lc_lane.id() == left_lane->id()) {
        drivable_lanes.at(i).left_lane = lc_lane;
        current_lc_idx = lc_idx;
        break;
      }

      if (right_lane && lc_lane.id() == right_lane->id()) {
        drivable_lanes.at(i).right_lane = lc_lane;
        current_lc_idx = lc_idx;
        break;
      }
    }
  }

  for (size_t i = current_lc_idx + 1; i < lane_change_lanes.size(); ++i) {
    const auto & lc_lane = lane_change_lanes.at(i);
    DrivableLanes drivable_lane;
    drivable_lane.left_lane = lc_lane;
    drivable_lane.right_lane = lc_lane;
    drivable_lanes.push_back(drivable_lane);
  }

  return drivable_lanes;
}

std::vector<DrivableLanes> generateDrivableLanes(
  const std::vector<DrivableLanes> & original_drivable_lanes, const RouteHandler & route_handler,
  const lanelet::ConstLanelets & current_lanes, const lanelet::ConstLanelets & lane_change_lanes)
{
  const auto has_same_lane =
    [](const lanelet::ConstLanelets & lanes, const lanelet::ConstLanelet & lane) {
      if (lanes.empty()) return false;
      const auto has_same = [&](const auto & ll) { return ll.id() == lane.id(); };
      return std::find_if(lanes.begin(), lanes.end(), has_same) != lanes.end();
    };

  const auto check_middle = [&](const auto & lane) -> std::optional<DrivableLanes> {
    for (const auto & drivable_lane : original_drivable_lanes) {
      if (has_same_lane(drivable_lane.middle_lanes, lane)) {
        return drivable_lane;
      }
    }
    return std::nullopt;
  };

  const auto check_left = [&](const auto & lane) -> std::optional<DrivableLanes> {
    for (const auto & drivable_lane : original_drivable_lanes) {
      if (drivable_lane.left_lane.id() == lane.id()) {
        return drivable_lane;
      }
    }
    return std::nullopt;
  };

  const auto check_right = [&](const auto & lane) -> std::optional<DrivableLanes> {
    for (const auto & drivable_lane : original_drivable_lanes) {
      if (drivable_lane.right_lane.id() == lane.id()) {
        return drivable_lane;
      }
    }
    return std::nullopt;
  };

  size_t current_lc_idx = 0;
  std::vector<DrivableLanes> drivable_lanes(current_lanes.size());
  for (size_t i = 0; i < current_lanes.size(); ++i) {
    const auto & current_lane = current_lanes.at(i);

    const auto middle_drivable_lane = check_middle(current_lane);
    if (middle_drivable_lane) {
      drivable_lanes.at(i) = *middle_drivable_lane;
    }

    const auto left_drivable_lane = check_left(current_lane);
    if (left_drivable_lane) {
      drivable_lanes.at(i) = *left_drivable_lane;
    }

    const auto right_drivable_lane = check_right(current_lane);
    if (right_drivable_lane) {
      drivable_lanes.at(i) = *right_drivable_lane;
    }

    if (!middle_drivable_lane && !left_drivable_lane && !right_drivable_lane) {
      drivable_lanes.at(i).left_lane = current_lane;
      drivable_lanes.at(i).right_lane = current_lane;
    }

    const auto left_lane = route_handler.getLeftLanelet(current_lane);
    const auto right_lane = route_handler.getRightLanelet(current_lane);
    if (!left_lane && !right_lane) {
      continue;
    }

    for (size_t lc_idx = current_lc_idx; lc_idx < lane_change_lanes.size(); ++lc_idx) {
      const auto & lc_lane = lane_change_lanes.at(lc_idx);
      if (left_lane && lc_lane.id() == left_lane->id()) {
        if (left_drivable_lane) {
          drivable_lanes.at(i).left_lane = lc_lane;
        }
        current_lc_idx = lc_idx;
        break;
      }

      if (right_lane && lc_lane.id() == right_lane->id()) {
        if (right_drivable_lane) {
          drivable_lanes.at(i).right_lane = lc_lane;
        }
        current_lc_idx = lc_idx;
        break;
      }
    }
  }

  for (size_t i = current_lc_idx + 1; i < lane_change_lanes.size(); ++i) {
    const auto & lc_lane = lane_change_lanes.at(i);
    DrivableLanes drivable_lane;

    const auto middle_drivable_lane = check_middle(lc_lane);
    if (middle_drivable_lane) {
      drivable_lane = *middle_drivable_lane;
    }

    const auto left_drivable_lane = check_left(lc_lane);
    if (left_drivable_lane) {
      drivable_lane = *left_drivable_lane;
    }

    const auto right_drivable_lane = check_right(lc_lane);
    if (right_drivable_lane) {
      drivable_lane = *right_drivable_lane;
    }

    if (!middle_drivable_lane && !left_drivable_lane && !right_drivable_lane) {
      drivable_lane.left_lane = lc_lane;
      drivable_lane.right_lane = lc_lane;
    }

    drivable_lanes.push_back(drivable_lane);
  }

  return drivable_lanes;
}

double getLateralShift(const LaneChangePath & path)
{
  if (path.shifted_path.shift_length.empty()) {
    return 0.0;
  }

  const auto start_idx =
    std::min(path.info.shift_line.start_idx, path.shifted_path.shift_length.size() - 1);
  const auto end_idx =
    std::min(path.info.shift_line.end_idx, path.shifted_path.shift_length.size() - 1);

  return path.shifted_path.shift_length.at(end_idx) - path.shifted_path.shift_length.at(start_idx);
}

std::vector<std::vector<int64_t>> get_sorted_lane_ids(const CommonDataPtr & common_data_ptr)
{
  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const auto & route_handler = *common_data_ptr->route_handler_ptr;
  const auto & current_pose = common_data_ptr->get_ego_pose();

  const auto rough_shift_length =
    lanelet::utils::getArcCoordinates(target_lanes, current_pose).distance;

  std::vector<std::vector<int64_t>> sorted_lane_ids{};
  sorted_lane_ids.reserve(target_lanes.size());
  const auto get_sorted_lane_ids = [&](const lanelet::ConstLanelet & target_lane) {
    const auto routing_graph_ptr = route_handler.getRoutingGraphPtr();
    lanelet::ConstLanelet lane;
    if (rough_shift_length < 0.0) {
      // lane change to the left, so i wan to take the lane right to target
      const auto has_target_right = routing_graph_ptr->right(target_lane);
      if (has_target_right) {
        lane = *has_target_right;
      }
    } else if (rough_shift_length > 0.0) {
      const auto has_target_left = routing_graph_ptr->left(target_lane);
      if (has_target_left) {
        lane = *has_target_left;
      }
    } else {
      lane = target_lane;
    }

    const auto find_same_id = std::find_if(
      current_lanes.cbegin(), current_lanes.cend(),
      [&lane](const lanelet::ConstLanelet & orig) { return orig.id() == lane.id(); });

    if (find_same_id == current_lanes.cend()) {
      return std::vector{target_lane.id()};
    }

    if (target_lane.id() > find_same_id->id()) {
      return std::vector{find_same_id->id(), target_lane.id()};
    }

    return std::vector{target_lane.id(), find_same_id->id()};
  };

  std::transform(
    target_lanes.cbegin(), target_lanes.cend(), std::back_inserter(sorted_lane_ids),
    get_sorted_lane_ids);

  return sorted_lane_ids;
}

std::vector<int64_t> replaceWithSortedIds(
  const std::vector<int64_t> & original_lane_ids,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids)
{
  for (const auto original_id : original_lane_ids) {
    for (const auto & sorted_id : sorted_lane_ids) {
      if (std::find(sorted_id.cbegin(), sorted_id.cend(), original_id) != sorted_id.cend()) {
        return sorted_id;
      }
    }
  }
  return original_lane_ids;
}

CandidateOutput assignToCandidate(
  const LaneChangePath & lane_change_path, const Point & ego_position)
{
  CandidateOutput candidate_output;
  candidate_output.path_candidate = lane_change_path.path;
  candidate_output.lateral_shift = utils::lane_change::getLateralShift(lane_change_path);
  candidate_output.start_distance_to_path_change = autoware::motion_utils::calcSignedArcLength(
    lane_change_path.path.points, ego_position, lane_change_path.info.shift_line.start.position);
  candidate_output.finish_distance_to_path_change = autoware::motion_utils::calcSignedArcLength(
    lane_change_path.path.points, ego_position, lane_change_path.info.shift_line.end.position);

  return candidate_output;
}

std::optional<lanelet::ConstLanelet> get_lane_change_target_lane(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & current_lanes)
{
  const auto direction = common_data_ptr->direction;
  const auto route_handler_ptr = common_data_ptr->route_handler_ptr;
  if (is_mandatory_lane_change(common_data_ptr->lc_type)) {
    return route_handler_ptr->getLaneChangeTarget(current_lanes, direction);
  }

  return route_handler_ptr->getLaneChangeTargetExceptPreferredLane(current_lanes, direction);
}

std::vector<PoseWithVelocityStamped> convertToPredictedPath(
  const LaneChangePath & lane_change_path, const Twist & vehicle_twist, const Pose & vehicle_pose,
  const double lane_changing_acceleration, const BehaviorPathPlannerParameters & common_parameters,
  const LaneChangeParameters & lane_change_parameters, const double resolution)
{
  if (lane_change_path.path.points.empty()) {
    return {};
  }

  const auto & path = lane_change_path.path;
  const auto prepare_acc = lane_change_path.info.longitudinal_acceleration.prepare;
  const auto duration = lane_change_path.info.duration.sum();
  const auto prepare_time = lane_change_path.info.duration.prepare;
  const auto & minimum_lane_changing_velocity =
    lane_change_parameters.minimum_lane_changing_velocity;

  const auto nearest_seg_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      path.points, vehicle_pose, common_parameters.ego_nearest_dist_threshold,
      common_parameters.ego_nearest_yaw_threshold);

  std::vector<PoseWithVelocityStamped> predicted_path;
  const auto vehicle_pose_frenet =
    convertToFrenetPoint(path.points, vehicle_pose.position, nearest_seg_idx);
  const double initial_velocity = std::abs(vehicle_twist.linear.x);

  // prepare segment
  for (double t = 0.0; t < prepare_time; t += resolution) {
    const double velocity =
      std::max(initial_velocity + prepare_acc * t, minimum_lane_changing_velocity);
    const double length = initial_velocity * t + 0.5 * prepare_acc * t * t;
    const auto pose = autoware::motion_utils::calcInterpolatedPose(
      path.points, vehicle_pose_frenet.length + length);
    predicted_path.emplace_back(t, pose, velocity);
  }

  // lane changing segment
  const double lane_changing_velocity =
    std::max(initial_velocity + prepare_acc * prepare_time, minimum_lane_changing_velocity);
  const double offset =
    initial_velocity * prepare_time + 0.5 * prepare_acc * prepare_time * prepare_time;
  for (double t = prepare_time; t < duration; t += resolution) {
    const double delta_t = t - prepare_time;
    const double velocity = lane_changing_velocity + lane_changing_acceleration * delta_t;
    const double length = lane_changing_velocity * delta_t +
                          0.5 * lane_changing_acceleration * delta_t * delta_t + offset;
    const auto pose = autoware::motion_utils::calcInterpolatedPose(
      path.points, vehicle_pose_frenet.length + length);
    predicted_path.emplace_back(t, pose, velocity);
  }

  return predicted_path;
}

bool isParkedObject(
  const PathWithLaneId & path, const RouteHandler & route_handler,
  const ExtendedPredictedObject & object, const double object_check_min_road_shoulder_width,
  const double object_shiftable_ratio_threshold, const double static_object_velocity_threshold)
{
  // ============================================ <- most_left_lanelet.leftBound()
  // y              road shoulder
  // ^ ------------------------------------------
  // |   x                                +
  // +---> --- object closest lanelet --- o ----- <- object_closest_lanelet.centerline()
  //
  // --------------------------------------------
  // +: object position
  // o: nearest point on centerline

  using lanelet::geometry::distance2d;
  using lanelet::geometry::toArcCoordinates;

  const double object_vel_norm =
    std::hypot(object.initial_twist.linear.x, object.initial_twist.linear.y);
  if (object_vel_norm > static_object_velocity_threshold) {
    return false;
  }

  const auto & object_pose = object.initial_pose;
  const auto object_closest_index =
    autoware::motion_utils::findNearestIndex(path.points, object_pose.position);
  const auto object_closest_pose = path.points.at(object_closest_index).point.pose;

  lanelet::ConstLanelet closest_lanelet;
  if (!route_handler.getClosestLaneletWithinRoute(object_closest_pose, &closest_lanelet)) {
    return false;
  }

  const double lat_dist =
    autoware::motion_utils::calcLateralOffset(path.points, object_pose.position);
  lanelet::BasicLineString2d bound;
  double center_to_bound_buffer = 0.0;
  if (lat_dist > 0.0) {
    // left side vehicle
    const auto most_left_road_lanelet = route_handler.getMostLeftLanelet(closest_lanelet);
    const auto most_left_lanelet_candidates =
      route_handler.getLaneletMapPtr()->laneletLayer.findUsages(most_left_road_lanelet.leftBound());
    lanelet::ConstLanelet most_left_lanelet = most_left_road_lanelet;
    const lanelet::Attribute most_left_sub_type =
      most_left_lanelet.attribute(lanelet::AttributeName::Subtype);

    for (const auto & ll : most_left_lanelet_candidates) {
      const auto & sub_type = ll.attribute(lanelet::AttributeName::Subtype);
      if (sub_type.value() == "road_shoulder") {
        most_left_lanelet = ll;
      }
    }
    bound = most_left_lanelet.leftBound2d().basicLineString();
    if (most_left_sub_type.value() != "road_shoulder") {
      center_to_bound_buffer = object_check_min_road_shoulder_width;
    }
  } else {
    // right side vehicle
    const auto most_right_road_lanelet = route_handler.getMostRightLanelet(closest_lanelet);
    const auto most_right_lanelet_candidates =
      route_handler.getLaneletMapPtr()->laneletLayer.findUsages(
        most_right_road_lanelet.rightBound());

    lanelet::ConstLanelet most_right_lanelet = most_right_road_lanelet;
    const lanelet::Attribute most_right_sub_type =
      most_right_lanelet.attribute(lanelet::AttributeName::Subtype);

    for (const auto & ll : most_right_lanelet_candidates) {
      const auto & sub_type = ll.attribute(lanelet::AttributeName::Subtype);
      if (sub_type.value() == "road_shoulder") {
        most_right_lanelet = ll;
      }
    }
    bound = most_right_lanelet.rightBound2d().basicLineString();
    if (most_right_sub_type.value() != "road_shoulder") {
      center_to_bound_buffer = object_check_min_road_shoulder_width;
    }
  }

  return isParkedObject(
    closest_lanelet, bound, object, center_to_bound_buffer, object_shiftable_ratio_threshold);
}

bool isParkedObject(
  const lanelet::ConstLanelet & closest_lanelet, const lanelet::BasicLineString2d & boundary,
  const ExtendedPredictedObject & object, const double buffer_to_bound,
  const double ratio_threshold)
{
  using lanelet::geometry::distance2d;

  const auto & obj_pose = object.initial_pose;
  const auto & obj_shape = object.shape;
  const auto obj_poly = autoware::universe_utils::toPolygon2d(obj_pose, obj_shape);
  const auto obj_point = obj_pose.position;

  double max_dist_to_bound = std::numeric_limits<double>::lowest();
  double min_dist_to_bound = std::numeric_limits<double>::max();
  for (const auto & edge : obj_poly.outer()) {
    const auto ll_edge = lanelet::Point2d(lanelet::InvalId, edge.x(), edge.y());
    const auto dist = distance2d(boundary, ll_edge);
    max_dist_to_bound = std::max(dist, max_dist_to_bound);
    min_dist_to_bound = std::min(dist, min_dist_to_bound);
  }
  const double obj_width = std::max(max_dist_to_bound - min_dist_to_bound, 0.0);

  // distance from centerline to the boundary line with object width
  const auto centerline_pose = lanelet::utils::getClosestCenterPose(closest_lanelet, obj_point);
  const lanelet::BasicPoint3d centerline_point(
    centerline_pose.position.x, centerline_pose.position.y, centerline_pose.position.z);
  const double dist_bound_to_centerline =
    std::abs(distance2d(boundary, centerline_point)) - 0.5 * obj_width + buffer_to_bound;

  // distance from object point to centerline
  const auto centerline = closest_lanelet.centerline();
  const auto ll_obj_point = lanelet::Point2d(lanelet::InvalId, obj_point.x, obj_point.y);
  const double dist_obj_to_centerline = std::abs(distance2d(centerline, ll_obj_point));

  const double ratio = dist_obj_to_centerline / std::max(dist_bound_to_centerline, 1e-6);
  const double clamped_ratio = std::clamp(ratio, 0.0, 1.0);
  return clamped_ratio > ratio_threshold;
}

bool passed_parked_objects(
  const CommonDataPtr & common_data_ptr, const LaneChangePath & lane_change_path,
  const std::vector<ExtendedPredictedObject> & objects, CollisionCheckDebugMap & object_debug)
{
  const auto route_handler = *common_data_ptr->route_handler_ptr;
  const auto lane_change_parameters = *common_data_ptr->lc_param_ptr;
  const auto & object_check_min_road_shoulder_width =
    lane_change_parameters.object_check_min_road_shoulder_width;
  const auto & object_shiftable_ratio_threshold =
    lane_change_parameters.object_shiftable_ratio_threshold;
  const auto & current_lane_path = common_data_ptr->current_lanes_path;

  if (objects.empty() || lane_change_path.path.points.empty() || current_lane_path.points.empty()) {
    return true;
  }

  const auto leading_obj_idx = getLeadingStaticObjectIdx(
    route_handler, lane_change_path, objects, object_check_min_road_shoulder_width,
    object_shiftable_ratio_threshold);
  if (!leading_obj_idx) {
    return true;
  }

  const auto & leading_obj = objects.at(*leading_obj_idx);
  auto debug = utils::path_safety_checker::createObjectDebug(leading_obj);
  const auto leading_obj_poly =
    autoware::universe_utils::toPolygon2d(leading_obj.initial_pose, leading_obj.shape);
  if (leading_obj_poly.outer().empty()) {
    return true;
  }

  const auto & current_path_end = current_lane_path.points.back().point.pose.position;
  const auto dist_to_path_end = [&](const auto & src_point) {
    if (common_data_ptr->lanes_ptr->current_lane_in_goal_section) {
      const auto goal_pose = route_handler.getGoalPose();
      return motion_utils::calcSignedArcLength(
        current_lane_path.points, src_point, goal_pose.position);
    }
    return motion_utils::calcSignedArcLength(current_lane_path.points, src_point, current_path_end);
  };

  const auto min_dist_to_end_of_current_lane = std::invoke([&]() {
    auto min_dist_to_end_of_current_lane = std::numeric_limits<double>::max();
    for (const auto & point : leading_obj_poly.outer()) {
      const auto obj_p = universe_utils::createPoint(point.x(), point.y(), 0.0);
      const auto dist = dist_to_path_end(obj_p);
      min_dist_to_end_of_current_lane = std::min(dist, min_dist_to_end_of_current_lane);
    }
    return min_dist_to_end_of_current_lane;
  });

  // If there are still enough length after the target object, we delay the lane change
  if (min_dist_to_end_of_current_lane <= common_data_ptr->transient_data.current_dist_buffer.min) {
    return true;
  }

  const auto current_pose = common_data_ptr->get_ego_pose();
  const auto dist_ego_to_obj = motion_utils::calcSignedArcLength(
    current_lane_path.points, current_pose.position, leading_obj.initial_pose.position);

  if (dist_ego_to_obj < lane_change_path.info.length.lane_changing) {
    return true;
  }

  debug.second.unsafe_reason = "delay lane change";
  utils::path_safety_checker::updateCollisionCheckDebugMap(object_debug, debug, false);
  return false;
}

std::optional<size_t> getLeadingStaticObjectIdx(
  const RouteHandler & route_handler, const LaneChangePath & lane_change_path,
  const std::vector<ExtendedPredictedObject> & objects,
  const double object_check_min_road_shoulder_width, const double object_shiftable_ratio_threshold)
{
  const auto & path = lane_change_path.path;

  if (path.points.empty() || objects.empty()) {
    return {};
  }

  const auto & lane_change_start = lane_change_path.info.lane_changing_start;
  const auto & path_end = path.points.back();

  double dist_lc_start_to_leading_obj = 0.0;
  std::optional<size_t> leading_obj_idx = std::nullopt;
  for (size_t obj_idx = 0; obj_idx < objects.size(); ++obj_idx) {
    const auto & obj = objects.at(obj_idx);
    const auto & obj_pose = obj.initial_pose;

    // ignore non-static object
    // TODO(shimizu): parametrize threshold
    const double obj_vel_norm = std::hypot(obj.initial_twist.linear.x, obj.initial_twist.linear.y);
    if (obj_vel_norm > 1.0) {
      continue;
    }

    // ignore non-parked object
    if (!isParkedObject(
          path, route_handler, obj, object_check_min_road_shoulder_width,
          object_shiftable_ratio_threshold)) {
      continue;
    }

    const double dist_back_to_obj = autoware::motion_utils::calcSignedArcLength(
      path.points, path_end.point.pose.position, obj_pose.position);
    if (dist_back_to_obj > 0.0) {
      // object is not on the lane change path
      continue;
    }

    const double dist_lc_start_to_obj = autoware::motion_utils::calcSignedArcLength(
      path.points, lane_change_start.position, obj_pose.position);
    if (dist_lc_start_to_obj < 0.0) {
      // object is on the lane changing path or behind it. It will be detected in safety check
      continue;
    }

    if (dist_lc_start_to_obj > dist_lc_start_to_leading_obj) {
      dist_lc_start_to_leading_obj = dist_lc_start_to_obj;
      leading_obj_idx = obj_idx;
    }
  }

  return leading_obj_idx;
}

lanelet::BasicPolygon2d create_polygon(
  const lanelet::ConstLanelets & lanes, const double start_dist, const double end_dist)
{
  if (lanes.empty()) {
    return {};
  }

  const auto polygon_3d = lanelet::utils::getPolygonFromArcLength(lanes, start_dist, end_dist);
  return lanelet::utils::to2D(polygon_3d).basicPolygon();
}

ExtendedPredictedObject transform(
  const PredictedObject & object,
  [[maybe_unused]] const BehaviorPathPlannerParameters & common_parameters,
  const LaneChangeParameters & lane_change_parameters)
{
  ExtendedPredictedObject extended_object(object);

  const auto & time_resolution = lane_change_parameters.prediction_time_resolution;
  const double obj_vel_norm =
    std::hypot(extended_object.initial_twist.linear.x, extended_object.initial_twist.linear.y);

  extended_object.predicted_paths.resize(object.kinematics.predicted_paths.size());
  for (size_t i = 0; i < object.kinematics.predicted_paths.size(); ++i) {
    const auto & path = object.kinematics.predicted_paths.at(i);
    const double end_time =
      rclcpp::Duration(path.time_step).seconds() * static_cast<double>(path.path.size() - 1);
    extended_object.predicted_paths.at(i).confidence = path.confidence;

    // create path
    for (double t = 0.0; t < end_time + std::numeric_limits<double>::epsilon();
         t += time_resolution) {
      const auto obj_pose = autoware::object_recognition_utils::calcInterpolatedPose(path, t);
      if (obj_pose) {
        const auto obj_polygon = autoware::universe_utils::toPolygon2d(*obj_pose, object.shape);
        extended_object.predicted_paths.at(i).path.emplace_back(
          t, *obj_pose, obj_vel_norm, obj_polygon);
      }
    }
  }

  return extended_object;
}

bool is_collided_polygons_in_lanelet(
  const std::vector<Polygon2d> & collided_polygons, const lanelet::BasicPolygon2d & lanes_polygon)
{
  const auto is_in_lanes = [&](const auto & collided_polygon) {
    return !lanes_polygon.empty() && !boost::geometry::disjoint(collided_polygon, lanes_polygon);
  };

  return std::any_of(collided_polygons.begin(), collided_polygons.end(), is_in_lanes);
}

lanelet::ConstLanelets generateExpandedLanelets(
  const lanelet::ConstLanelets & lanes, const Direction direction, const double left_offset,
  const double right_offset)
{
  const auto left_extend_offset = (direction == Direction::LEFT) ? left_offset : 0.0;
  const auto right_extend_offset = (direction == Direction::RIGHT) ? -right_offset : 0.0;
  return lanelet::utils::getExpandedLanelets(lanes, left_extend_offset, right_extend_offset);
}

rclcpp::Logger getLogger(const std::string & type)
{
  return rclcpp::get_logger("lane_change").get_child(type);
}

Polygon2d get_ego_footprint(const Pose & ego_pose, const VehicleInfo & ego_info)
{
  const auto base_to_front = ego_info.max_longitudinal_offset_m;
  const auto base_to_rear = ego_info.rear_overhang_m;
  const auto width = ego_info.vehicle_width_m;

  return autoware::universe_utils::toFootprint(ego_pose, base_to_front, base_to_rear, width);
}

Point getEgoFrontVertex(
  const Pose & ego_pose, const autoware::vehicle_info_utils::VehicleInfo & ego_info, bool left)
{
  const double lon_offset = ego_info.wheel_base_m + ego_info.front_overhang_m;
  const double lat_offset = 0.5 * (left ? ego_info.vehicle_width_m : -ego_info.vehicle_width_m);
  return autoware::universe_utils::calcOffsetPose(ego_pose, lon_offset, lat_offset, 0.0).position;
}

bool is_within_intersection(
  const std::shared_ptr<RouteHandler> & route_handler, const lanelet::ConstLanelet & lanelet,
  const Polygon2d & polygon)
{
  const std::string id = lanelet.attributeOr("intersection_area", "else");
  if (id == "else" || !std::atoi(id.c_str())) {
    return false;
  }

  if (!route_handler || !route_handler->getLaneletMapPtr()) {
    return false;
  }

  const auto & polygon_layer = route_handler->getLaneletMapPtr()->polygonLayer;
  const auto lanelet_polygon_opt = polygon_layer.find(std::atoi(id.c_str()));
  if (lanelet_polygon_opt == polygon_layer.end()) {
    return false;
  }
  const auto & lanelet_polygon = *lanelet_polygon_opt;

  return boost::geometry::within(
    polygon, utils::toPolygon2d(lanelet::utils::to2D(lanelet_polygon.basicPolygon())));
}

bool is_within_turn_direction_lanes(
  const lanelet::ConstLanelet & lanelet, const Polygon2d & polygon)
{
  const std::string turn_direction = lanelet.attributeOr("turn_direction", "else");
  if (turn_direction == "else" || turn_direction == "straight") {
    return false;
  }

  return !boost::geometry::disjoint(
    polygon, utils::toPolygon2d(lanelet::utils::to2D(lanelet.polygon2d().basicPolygon())));
}

LanesPolygon create_lanes_polygon(const CommonDataPtr & common_data_ptr)
{
  const auto & lanes = common_data_ptr->lanes_ptr;
  LanesPolygon lanes_polygon;

  lanes_polygon.current =
    utils::lane_change::create_polygon(lanes->current, 0.0, std::numeric_limits<double>::max());

  lanes_polygon.target =
    utils::lane_change::create_polygon(lanes->target, 0.0, std::numeric_limits<double>::max());

  const auto & lc_param_ptr = common_data_ptr->lc_param_ptr;
  const auto expanded_target_lanes = utils::lane_change::generateExpandedLanelets(
    lanes->target, common_data_ptr->direction, lc_param_ptr->lane_expansion_left_offset,
    lc_param_ptr->lane_expansion_right_offset);
  lanes_polygon.expanded_target = utils::lane_change::create_polygon(
    expanded_target_lanes, 0.0, std::numeric_limits<double>::max());

  lanes_polygon.target_neighbor = utils::lane_change::create_polygon(
    lanes->target_neighbor, 0.0, std::numeric_limits<double>::max());

  lanes_polygon.preceding_target.reserve(lanes->preceding_target.size());
  for (const auto & preceding_lane : lanes->preceding_target) {
    auto lane_polygon =
      utils::lane_change::create_polygon(preceding_lane, 0.0, std::numeric_limits<double>::max());

    if (!lane_polygon.empty()) {
      lanes_polygon.preceding_target.push_back(lane_polygon);
    }
  }
  return lanes_polygon;
}

bool is_same_lane_with_prev_iteration(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & current_lanes,
  const lanelet::ConstLanelets & target_lanes)
{
  if (current_lanes.empty() || target_lanes.empty()) {
    return false;
  }
  const auto & prev_current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & prev_target_lanes = common_data_ptr->lanes_ptr->target;
  if (prev_current_lanes.empty() || prev_target_lanes.empty()) {
    return false;
  }

  if (
    (prev_current_lanes.front().id() != current_lanes.front().id()) ||
    (prev_current_lanes.back().id() != prev_current_lanes.back().id())) {
    return false;
  }
  return (prev_target_lanes.front().id() == target_lanes.front().id()) &&
         (prev_target_lanes.back().id() == prev_target_lanes.back().id());
}

bool is_ahead_of_ego(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & path,
  const PredictedObject & object)
{
  const auto & current_ego_pose = common_data_ptr->get_ego_pose();

  const auto & obj_position = object.kinematics.initial_pose_with_covariance.pose.position;

  const auto dist_to_base_link = autoware::motion_utils::calcSignedArcLength(
    path.points, current_ego_pose.position, obj_position);
  const auto & ego_info = common_data_ptr->bpp_param_ptr->vehicle_info;
  const auto lon_dev = std::max(
    ego_info.max_longitudinal_offset_m + ego_info.rear_overhang_m, object.shape.dimensions.x);

  // we don't always have to check the distance accurately.
  if (std::abs(dist_to_base_link) > lon_dev) {
    return dist_to_base_link >= 0.0;
  }

  const auto & current_footprint = common_data_ptr->transient_data.current_footprint.outer();
  auto ego_min_dist_to_end = std::numeric_limits<double>::max();
  for (const auto & ego_edge_point : current_footprint) {
    const auto ego_edge =
      autoware::universe_utils::createPoint(ego_edge_point.x(), ego_edge_point.y(), 0.0);
    const auto dist_to_end = autoware::motion_utils::calcSignedArcLength(
      path.points, ego_edge, path.points.back().point.pose.position);
    ego_min_dist_to_end = std::min(dist_to_end, ego_min_dist_to_end);
  }

  const auto obj_polygon = autoware::universe_utils::toPolygon2d(object).outer();
  auto current_min_dist_to_end = std::numeric_limits<double>::max();
  for (const auto & polygon_p : obj_polygon) {
    const auto obj_p = autoware::universe_utils::createPoint(polygon_p.x(), polygon_p.y(), 0.0);
    const auto dist_ego_to_obj = autoware::motion_utils::calcSignedArcLength(
      path.points, obj_p, path.points.back().point.pose.position);
    current_min_dist_to_end = std::min(dist_ego_to_obj, current_min_dist_to_end);
  }
  return ego_min_dist_to_end - current_min_dist_to_end >= 0.0;
}

bool is_before_terminal(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & path,
  const PredictedObject & object)
{
  const auto & route_handler_ptr = common_data_ptr->route_handler_ptr;
  const auto & lanes_ptr = common_data_ptr->lanes_ptr;
  const auto terminal_position = (lanes_ptr->current_lane_in_goal_section)
                                   ? route_handler_ptr->getGoalPose().position
                                   : path.points.back().point.pose.position;
  double current_max_dist = std::numeric_limits<double>::lowest();

  const auto & obj_position = object.kinematics.initial_pose_with_covariance.pose.position;
  const auto dist_to_base_link =
    autoware::motion_utils::calcSignedArcLength(path.points, obj_position, terminal_position);
  // we don't always have to check the distance accurately.
  if (std::abs(dist_to_base_link) > object.shape.dimensions.x) {
    return dist_to_base_link >= 0.0;
  }

  const auto obj_polygon = autoware::universe_utils::toPolygon2d(object).outer();
  for (const auto & polygon_p : obj_polygon) {
    const auto obj_p = autoware::universe_utils::createPoint(polygon_p.x(), polygon_p.y(), 0.0);
    const auto dist_obj_to_terminal =
      autoware::motion_utils::calcSignedArcLength(path.points, obj_p, terminal_position);
    current_max_dist = std::max(dist_obj_to_terminal, current_max_dist);
  }
  return current_max_dist >= 0.0;
}

double calc_angle_to_lanelet_segment(const lanelet::ConstLanelets & lanelets, const Pose & pose)
{
  lanelet::ConstLanelet closest_lanelet;

  if (!lanelet::utils::query::getClosestLanelet(lanelets, pose, &closest_lanelet)) {
    return autoware::universe_utils::deg2rad(180);
  }
  const auto closest_pose = lanelet::utils::getClosestCenterPose(closest_lanelet, pose.position);
  return std::abs(autoware::universe_utils::calcYawDeviation(closest_pose, pose));
}

ExtendedPredictedObjects transform_to_extended_objects(
  const CommonDataPtr & common_data_ptr, const std::vector<PredictedObject> & objects)
{
  ExtendedPredictedObjects extended_objects;
  extended_objects.reserve(objects.size());

  const auto & bpp_param = *common_data_ptr->bpp_param_ptr;
  const auto & lc_param = *common_data_ptr->lc_param_ptr;
  std::transform(
    objects.begin(), objects.end(), std::back_inserter(extended_objects), [&](const auto & object) {
      return utils::lane_change::transform(object, bpp_param, lc_param);
    });

  return extended_objects;
}

double get_distance_to_next_regulatory_element(
  const CommonDataPtr & common_data_ptr, const bool ignore_crosswalk,
  const bool ignore_intersection)
{
  double distance = std::numeric_limits<double>::max();

  const auto current_pose = common_data_ptr->get_ego_pose();
  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & route_handler = *common_data_ptr->route_handler_ptr;
  const auto overall_graphs_ptr = route_handler.getOverallGraphPtr();

  if (!ignore_intersection && common_data_ptr->lc_param_ptr->regulate_on_intersection) {
    distance =
      std::min(distance, utils::getDistanceToNextIntersection(current_pose, current_lanes));
  }
  if (!ignore_crosswalk && common_data_ptr->lc_param_ptr->regulate_on_crosswalk) {
    distance = std::min(
      distance, utils::getDistanceToCrosswalk(current_pose, current_lanes, *overall_graphs_ptr));
  }
  if (common_data_ptr->lc_param_ptr->regulate_on_traffic_light) {
    distance = std::min(
      distance, utils::traffic_light::getDistanceToNextTrafficLight(current_pose, current_lanes));
  }

  return distance;
}

double get_min_dist_to_current_lanes_obj(
  const CommonDataPtr & common_data_ptr, const FilteredByLanesExtendedObjects & filtered_objects,
  const double dist_to_target_lane_start, const PathWithLaneId & path)
{
  const auto & path_points = path.points;
  auto min_dist_to_obj = std::numeric_limits<double>::max();
  for (const auto & object : filtered_objects.current_lane) {
    // check if stationary
    const auto obj_v = std::abs(object.initial_twist.linear.x);
    if (obj_v > common_data_ptr->lc_param_ptr->stop_velocity_threshold) {
      continue;
    }

    // provide "estimation" based on size of object
    const auto dist_to_obj =
      motion_utils::calcSignedArcLength(
        path_points, path_points.front().point.pose.position, object.initial_pose.position) -
      (object.shape.dimensions.x / 2);

    if (dist_to_obj < dist_to_target_lane_start) {
      continue;
    }

    // calculate distance from path front to the stationary object polygon on the ego lane.
    for (const auto & polygon_p : object.initial_polygon.outer()) {
      const auto p_fp = autoware::universe_utils::toMsg(polygon_p.to_3d());
      const auto lateral_fp = motion_utils::calcLateralOffset(path_points, p_fp);

      // ignore if the point is not on ego path
      if (std::abs(lateral_fp) > (common_data_ptr->bpp_param_ptr->vehicle_width / 2)) {
        continue;
      }

      const auto current_distance_to_obj = motion_utils::calcSignedArcLength(path_points, 0, p_fp);
      min_dist_to_obj = std::min(min_dist_to_obj, current_distance_to_obj);
    }
  }
  return min_dist_to_obj;
}

bool has_blocking_target_object(
  const CommonDataPtr & common_data_ptr, const FilteredByLanesExtendedObjects & filtered_objects,
  const double stop_arc_length, const PathWithLaneId & path)
{
  return std::any_of(
    filtered_objects.target_lane_leading.begin(), filtered_objects.target_lane_leading.end(),
    [&](const auto & object) {
      const auto v = std::abs(object.initial_twist.linear.x);
      if (v > common_data_ptr->lc_param_ptr->stop_velocity_threshold) {
        return false;
      }

      // filtered_objects includes objects out of target lanes, so filter them out
      if (boost::geometry::disjoint(
            object.initial_polygon, common_data_ptr->lanes_polygon_ptr->target)) {
        return false;
      }

      const auto arc_length_to_target_lane_obj = motion_utils::calcSignedArcLength(
        path.points, path.points.front().point.pose.position, object.initial_pose.position);
      const auto width_margin = object.shape.dimensions.x / 2;
      return (arc_length_to_target_lane_obj - width_margin) >= stop_arc_length;
    });
}

bool has_passed_intersection_turn_direction(const CommonDataPtr & common_data_ptr)
{
  const auto & transient_data = common_data_ptr->transient_data;
  if (transient_data.in_intersection && transient_data.in_turn_direction_lane) {
    return false;
  }

  return transient_data.dist_from_prev_intersection >
         common_data_ptr->lc_param_ptr->backward_length_from_intersection;
}

std::vector<LineString2d> get_line_string_paths(const ExtendedPredictedObject & object)
{
  const auto to_linestring_2d = [](const auto & predicted_path) -> LineString2d {
    LineString2d line_string;
    const auto & path = predicted_path.path;
    line_string.reserve(path.size());
    for (const auto & path_point : path) {
      const auto point = universe_utils::fromMsg(path_point.pose.position).to_2d();
      line_string.push_back(point);
    }

    return line_string;
  };

  const auto paths = object.predicted_paths;
  std::vector<LineString2d> line_strings;
  std::transform(paths.begin(), paths.end(), std::back_inserter(line_strings), to_linestring_2d);

  return line_strings;
}

bool has_overtaking_turn_lane_object(
  const CommonDataPtr & common_data_ptr, const ExtendedPredictedObjects & trailing_objects)
{
  // Note: This situation is only applicable if the ego is in a turn lane.
  if (has_passed_intersection_turn_direction(common_data_ptr)) {
    return false;
  }

  const auto is_path_overlap_with_target = [&](const LineString2d & path) {
    return !boost::geometry::disjoint(path, common_data_ptr->lanes_polygon_ptr->target);
  };

  const auto is_object_overlap_with_target = [&](const auto & object) {
    // to compensate for perception issue, or if object is from behind ego, and tries to overtake,
    // but stop all of sudden
    if (!boost::geometry::disjoint(
          object.initial_polygon, common_data_ptr->lanes_polygon_ptr->current)) {
      return true;
    }

    const auto paths = get_line_string_paths(object);
    return std::any_of(paths.begin(), paths.end(), is_path_overlap_with_target);
  };

  return std::any_of(
    trailing_objects.begin(), trailing_objects.end(), is_object_overlap_with_target);
}
}  // namespace autoware::behavior_path_planner::utils::lane_change

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

#include "autoware/behavior_path_lane_change_module/structs/data.hpp"
#include "autoware/behavior_path_lane_change_module/structs/path.hpp"
#include "autoware/behavior_path_planner_common/parameters.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/safety_check.hpp"
#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"
#include "autoware/behavior_path_planner_common/utils/traffic_light_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"
#include "autoware/object_recognition_utils/predicted_path_utils.hpp"
#include "autoware/universe_utils/math/unit_conversion.hpp"

// for the geometry types
#include <autoware/motion_utils/trajectory/path_shift.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
// for the svg mapper
#include <autoware/behavior_path_planner_common/utils/path_safety_checker/objects_filtering.hpp>
#include <autoware/motion_utils/trajectory/interpolation.hpp>
#include <autoware/motion_utils/trajectory/path_with_lane_id.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <autoware_frenet_planner/frenet_planner.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <autoware_vehicle_info_utils/vehicle_info.hpp>
#include <range/v3/action/remove_if.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/numeric.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view.hpp>
#include <rclcpp/rclcpp.hpp>

#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/detail/disjoint/interface.hpp>
#include <boost/geometry/io/svg/svg_mapper.hpp>
#include <boost/geometry/io/svg/write.hpp>

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
#include <unordered_set>
#include <vector>

namespace autoware::behavior_path_planner::utils::lane_change
{
using autoware::route_handler::RouteHandler;
using autoware::universe_utils::LineString2d;
using autoware::universe_utils::Point2d;
using autoware::universe_utils::Polygon2d;
using autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObjects;
using behavior_path_planner::lane_change::PathType;
using geometry_msgs::msg::Pose;
using tier4_planning_msgs::msg::PathWithLaneId;

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

void set_prepare_velocity(
  PathWithLaneId & prepare_segment, const double current_velocity, const double prepare_velocity)
{
  if (current_velocity >= prepare_velocity) {
    // deceleration
    prepare_segment.points.back().point.longitudinal_velocity_mps = std::min(
      prepare_segment.points.back().point.longitudinal_velocity_mps,
      static_cast<float>(prepare_velocity));
    return;
  }
  // acceleration
  for (auto & point : prepare_segment.points) {
    point.point.longitudinal_velocity_mps =
      std::min(point.point.longitudinal_velocity_mps, static_cast<float>(prepare_velocity));
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

std::vector<int64_t> replace_with_sorted_ids(
  const std::vector<int64_t> & current_lane_ids,
  const std::vector<std::vector<int64_t>> & sorted_lane_ids, std::vector<int64_t> & prev_lane_ids,
  std::vector<int64_t> & prev_sorted_lane_ids)
{
  if (current_lane_ids == prev_lane_ids) {
    return prev_sorted_lane_ids;
  }

  for (const auto original_id : current_lane_ids) {
    for (const auto & sorted_id : sorted_lane_ids) {
      if (std::find(sorted_id.cbegin(), sorted_id.cend(), original_id) != sorted_id.cend()) {
        prev_lane_ids = current_lane_ids;
        prev_sorted_lane_ids = sorted_id;
        return prev_sorted_lane_ids;
      }
    }
  }

  return current_lane_ids;
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

std::vector<PoseWithVelocityStamped> convert_to_predicted_path(
  const CommonDataPtr & common_data_ptr, const LaneChangePath & lane_change_path,
  const double lane_changing_acceleration)
{
  if (lane_change_path.path.points.empty()) {
    return {};
  }

  const auto & path = lane_change_path.path;
  const auto & vehicle_pose = common_data_ptr->get_ego_pose();
  const auto & bpp_param_ptr = common_data_ptr->bpp_param_ptr;
  const auto nearest_seg_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      path.points, vehicle_pose, bpp_param_ptr->ego_nearest_dist_threshold,
      bpp_param_ptr->ego_nearest_yaw_threshold);

  const auto vehicle_pose_frenet =
    convertToFrenetPoint(path.points, vehicle_pose.position, nearest_seg_idx);

  const auto initial_velocity = common_data_ptr->get_ego_speed();
  const auto prepare_acc = lane_change_path.info.longitudinal_acceleration.prepare;
  const auto duration = lane_change_path.info.duration.sum();
  const auto prepare_time = lane_change_path.info.duration.prepare;
  const auto & lc_param_ptr = common_data_ptr->lc_param_ptr;
  const auto resolution = lc_param_ptr->safety.collision_check.prediction_time_resolution;
  std::vector<PoseWithVelocityStamped> predicted_path;
  predicted_path.reserve(static_cast<size_t>(std::ceil(duration / resolution)));

  // prepare segment
  for (double t = 0.0; t < prepare_time; t += resolution) {
    const auto velocity =
      std::clamp(initial_velocity + prepare_acc * t, 0.0, lane_change_path.info.velocity.prepare);
    const auto length = initial_velocity * t + 0.5 * prepare_acc * t * t;
    const auto pose = autoware::motion_utils::calcInterpolatedPose(
      path.points, vehicle_pose_frenet.length + length);
    predicted_path.emplace_back(t, pose, velocity);
  }

  // lane changing segment
  const auto lane_changing_velocity = std::clamp(
    initial_velocity + prepare_acc * prepare_time, 0.0, lane_change_path.info.velocity.prepare);
  const auto offset =
    initial_velocity * prepare_time + 0.5 * prepare_acc * prepare_time * prepare_time;

  for (double t = prepare_time; t < duration; t += resolution) {
    const auto delta_t = t - prepare_time;
    const auto velocity = std::clamp(
      lane_changing_velocity + lane_changing_acceleration * delta_t, 0.0,
      lane_change_path.info.velocity.lane_changing);
    const auto length = lane_changing_velocity * delta_t +
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
  const auto most_side_lanelet =
    lat_dist > 0.0 ? route_handler.getMostLeftLanelet(closest_lanelet, false, true)
                   : route_handler.getMostRightLanelet(closest_lanelet, false, true);
  const auto bound = lat_dist > 0.0 ? most_side_lanelet.leftBound2d().basicLineString()
                                    : most_side_lanelet.rightBound2d().basicLineString();
  const lanelet::Attribute lanelet_sub_type =
    most_side_lanelet.attribute(lanelet::AttributeName::Subtype);
  const auto center_to_bound_buffer =
    lanelet_sub_type.value() == "road_shoulder" ? 0.0 : object_check_min_road_shoulder_width;

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

bool is_delay_lane_change(
  const CommonDataPtr & common_data_ptr, const LaneChangePath & lane_change_path,
  const ExtendedPredictedObjects & target_objects, CollisionCheckDebugMap & object_debug)
{
  const auto & current_lane_path = common_data_ptr->current_lanes_path;
  const auto & delay_lc_param = common_data_ptr->lc_param_ptr->delay;

  if (
    !delay_lc_param.enable || target_objects.empty() || lane_change_path.path.points.empty() ||
    current_lane_path.points.empty()) {
    return false;
  }

  const auto dist_to_end = common_data_ptr->transient_data.dist_to_terminal_end;
  const auto dist_buffer = common_data_ptr->transient_data.current_dist_buffer.min;
  auto is_near_end = [&dist_to_end, &dist_buffer](const ExtendedPredictedObject & obj) {
    const auto dist_obj_to_end = dist_to_end - obj.dist_from_ego;
    return dist_obj_to_end <= dist_buffer;
  };

  const auto ego_vel = common_data_ptr->get_ego_speed();
  const auto min_lon_acc = common_data_ptr->lc_param_ptr->trajectory.min_longitudinal_acc;
  const auto gap_threshold = std::abs((ego_vel * ego_vel) / (2 * min_lon_acc));
  auto is_sufficient_gap = [&gap_threshold](const auto & current_obj, const auto & next_obj) {
    const auto curr_obj_half_length = current_obj.shape.dimensions.x;
    const auto next_obj_half_length = next_obj.shape.dimensions.x;
    const auto dist_current_to_next = next_obj.dist_from_ego - current_obj.dist_from_ego;
    const auto gap_length = dist_current_to_next - curr_obj_half_length - next_obj_half_length;
    return gap_length > gap_threshold;
  };

  for (auto it = target_objects.begin(); it < target_objects.end(); ++it) {
    if (is_near_end(*it)) break;

    if (it->dist_from_ego < lane_change_path.info.length.lane_changing) continue;

    if (
      delay_lc_param.check_only_parked_vehicle &&
      !isParkedObject(
        lane_change_path.path, *common_data_ptr->route_handler_ptr, *it,
        delay_lc_param.min_road_shoulder_width, delay_lc_param.th_parked_vehicle_shift_ratio)) {
      continue;
    }

    auto next_it = std::next(it);
    if (next_it == target_objects.end() || is_sufficient_gap(*it, *next_it)) {
      auto debug = utils::path_safety_checker::createObjectDebug(*it);
      debug.second.unsafe_reason = "delay lane change";
      utils::path_safety_checker::updateCollisionCheckDebugMap(object_debug, debug, false);
      return true;
    }
  }

  return false;
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
  const PredictedObject & object, const LaneChangeParameters & lane_change_parameters)
{
  ExtendedPredictedObject extended_object(object);

  const auto & time_resolution =
    lane_change_parameters.safety.collision_check.prediction_time_resolution;
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

  const auto & params = common_data_ptr->lc_param_ptr->safety;
  const auto expanded_target_lanes = utils::lane_change::generateExpandedLanelets(
    lanes->target, common_data_ptr->direction, params.lane_expansion_left_offset,
    params.lane_expansion_right_offset);
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
  const ExtendedPredictedObject & object)
{
  const auto & ego_info = common_data_ptr->bpp_param_ptr->vehicle_info;
  const auto lon_dev = std::max(
    ego_info.max_longitudinal_offset_m + ego_info.rear_overhang_m, object.shape.dimensions.x);

  // we don't always have to check the distance accurately.
  if (std::abs(object.dist_from_ego) > lon_dev) {
    return object.dist_from_ego >= 0.0;
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

  auto current_min_dist_to_end = std::numeric_limits<double>::max();
  for (const auto & polygon_p : object.initial_polygon.outer()) {
    const auto obj_p = autoware::universe_utils::createPoint(polygon_p.x(), polygon_p.y(), 0.0);
    const auto dist_ego_to_obj = autoware::motion_utils::calcSignedArcLength(
      path.points, obj_p, path.points.back().point.pose.position);
    current_min_dist_to_end = std::min(dist_ego_to_obj, current_min_dist_to_end);
  }
  return ego_min_dist_to_end - current_min_dist_to_end >= 0.0;
}

bool is_before_terminal(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & path,
  const ExtendedPredictedObject & object)
{
  const auto & route_handler_ptr = common_data_ptr->route_handler_ptr;
  const auto & lanes_ptr = common_data_ptr->lanes_ptr;
  const auto terminal_position = (lanes_ptr->current_lane_in_goal_section)
                                   ? route_handler_ptr->getGoalPose().position
                                   : path.points.back().point.pose.position;
  double current_max_dist = std::numeric_limits<double>::lowest();

  const auto & obj_position = object.initial_pose.position;
  const auto dist_to_base_link =
    autoware::motion_utils::calcSignedArcLength(path.points, obj_position, terminal_position);
  // we don't always have to check the distance accurately.
  if (std::abs(dist_to_base_link) > object.shape.dimensions.x) {
    return dist_to_base_link >= 0.0;
  }

  for (const auto & polygon_p : object.initial_polygon.outer()) {
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
  const CommonDataPtr & common_data_ptr, const FilteredLanesObjects & filtered_objects,
  const double dist_to_target_lane_start, const PathWithLaneId & path)
{
  const auto & path_points = path.points;
  auto min_dist_to_obj = std::numeric_limits<double>::max();
  for (const auto & object : filtered_objects.current_lane) {
    // check if stationary
    const auto obj_v = std::abs(object.initial_twist.linear.x);
    if (obj_v > common_data_ptr->lc_param_ptr->th_stop_velocity) {
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

    // check if object is on ego path
    const auto obj_half_width = object.shape.dimensions.y / 2;
    const auto obj_lat_dist_to_path =
      std::abs(motion_utils::calcLateralOffset(path_points, object.initial_pose.position)) -
      obj_half_width;
    if (obj_lat_dist_to_path > (common_data_ptr->bpp_param_ptr->vehicle_width / 2)) {
      continue;
    }

    min_dist_to_obj = std::min(min_dist_to_obj, dist_to_obj);
    break;
  }
  return min_dist_to_obj;
}

bool has_blocking_target_object(
  const TargetLaneLeadingObjects & target_leading_objects, const double stop_arc_length,
  const PathWithLaneId & path)
{
  return ranges::any_of(target_leading_objects.stopped, [&](const auto & object) {
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

  return object.predicted_paths | ranges::views::transform(to_linestring_2d) |
         ranges::to<std::vector>();
}

bool has_overtaking_turn_lane_object(
  const CommonDataPtr & common_data_ptr, const ExtendedPredictedObjects & trailing_objects)
{
  // Note: This situation is only applicable if the ego is in a turn lane.
  if (has_passed_intersection_turn_direction(common_data_ptr)) {
    return false;
  }

  const auto is_object_overlap_with_target = [&](const auto & object) {
    // to compensate for perception issue, or if object is from behind ego, and tries to overtake,
    // but stop all of sudden
    if (!boost::geometry::disjoint(
          object.initial_polygon, common_data_ptr->lanes_polygon_ptr->current)) {
      return true;
    }

    return object_path_overlaps_lanes(object, common_data_ptr->lanes_polygon_ptr->target);
  };

  return std::any_of(
    trailing_objects.begin(), trailing_objects.end(), is_object_overlap_with_target);
}

bool filter_target_lane_objects(
  const CommonDataPtr & common_data_ptr, const ExtendedPredictedObject & object,
  const double dist_ego_to_current_lanes_center, const bool ahead_of_ego,
  const bool before_terminal, TargetLaneLeadingObjects & leading_objects,
  ExtendedPredictedObjects & trailing_objects)
{
  using behavior_path_planner::utils::path_safety_checker::filter::is_vehicle;
  using behavior_path_planner::utils::path_safety_checker::filter::velocity_filter;
  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & vehicle_width = common_data_ptr->bpp_param_ptr->vehicle_info.vehicle_width_m;
  const auto & lanes_polygon = *common_data_ptr->lanes_polygon_ptr;
  const auto stopped_obj_vel_th = common_data_ptr->lc_param_ptr->safety.th_stopped_object_velocity;

  const auto is_lateral_far = std::invoke([&]() -> bool {
    const auto dist_object_to_current_lanes_center =
      lanelet::utils::getLateralDistanceToClosestLanelet(current_lanes, object.initial_pose);
    const auto lateral = dist_object_to_current_lanes_center - dist_ego_to_current_lanes_center;
    return std::abs(lateral) > (vehicle_width / 2);
  });

  const auto is_stopped = velocity_filter(
    object.initial_twist, -std::numeric_limits<double>::epsilon(), stopped_obj_vel_th);
  if (is_lateral_far && before_terminal) {
    const auto overlapping_with_target_lanes =
      !boost::geometry::disjoint(object.initial_polygon, lanes_polygon.target) ||
      (!is_stopped && is_vehicle(object.classification) &&
       object_path_overlaps_lanes(object, lanes_polygon.target));

    if (overlapping_with_target_lanes) {
      if (!ahead_of_ego && !is_stopped) {
        trailing_objects.push_back(object);
        return true;
      }

      if (ahead_of_ego) {
        if (is_stopped) {
          leading_objects.stopped.push_back(object);
        } else {
          leading_objects.moving.push_back(object);
        }
        return true;
      }
    }

    // Check if the object is positioned outside the lane boundary but still close to its edge.
    const auto in_expanded_target_lanes =
      !boost::geometry::disjoint(object.initial_polygon, lanes_polygon.expanded_target);

    if (in_expanded_target_lanes && is_stopped && ahead_of_ego) {
      leading_objects.stopped_at_bound.push_back(object);
      return true;
    }
  }

  const auto is_overlap_target_backward =
    ranges::any_of(lanes_polygon.preceding_target, [&](const auto & target_backward_polygon) {
      return !boost::geometry::disjoint(object.initial_polygon, target_backward_polygon);
    });

  // check if the object intersects with target backward lanes
  if (is_overlap_target_backward && !is_stopped) {
    trailing_objects.push_back(object);
    return true;
  }

  return false;
}

std::vector<lanelet::ConstLanelets> get_preceding_lanes(const CommonDataPtr & common_data_ptr)
{
  const auto & route_handler_ptr = common_data_ptr->route_handler_ptr;
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const auto & ego_pose = common_data_ptr->get_ego_pose();
  const auto backward_lane_length = common_data_ptr->lc_param_ptr->backward_lane_length;

  const auto preceding_lanes_list =
    utils::getPrecedingLanelets(*route_handler_ptr, target_lanes, ego_pose, backward_lane_length);

  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  std::unordered_set<lanelet::Id> current_lanes_id;
  for (const auto & lane : current_lanes) {
    current_lanes_id.insert(lane.id());
  }
  const auto is_overlapping = [&](const lanelet::ConstLanelet & lane) {
    return current_lanes_id.find(lane.id()) != current_lanes_id.end();
  };

  std::vector<lanelet::ConstLanelets> non_overlapping_lanes_vec;
  for (const auto & lanes : preceding_lanes_list) {
    auto lanes_reversed = lanes | ranges::views::reverse;
    auto overlapped_itr = ranges::find_if(lanes_reversed, is_overlapping);

    if (overlapped_itr == lanes_reversed.begin()) {
      continue;
    }

    // Lanes are not reversed by default. Avoid returning reversed lanes to prevent undefined
    // behavior.
    lanelet::ConstLanelets non_overlapping_lanes(overlapped_itr.base(), lanes.end());
    non_overlapping_lanes_vec.push_back(non_overlapping_lanes);
  }

  return non_overlapping_lanes_vec;
}

bool object_path_overlaps_lanes(
  const ExtendedPredictedObject & object, const lanelet::BasicPolygon2d & lanes_polygon)
{
  return ranges::any_of(get_line_string_paths(object), [&](const auto & path) {
    return !boost::geometry::disjoint(path, lanes_polygon);
  });
}

std::vector<std::vector<PoseWithVelocityStamped>> convert_to_predicted_paths(
  const CommonDataPtr & common_data_ptr, const LaneChangePath & lane_change_path,
  const size_t deceleration_sampling_num)
{
  static constexpr double floating_err_th{1e-3};
  const auto bpp_param = *common_data_ptr->bpp_param_ptr;
  const auto global_min_acc = bpp_param.min_acc;
  const auto lane_changing_acc = lane_change_path.info.longitudinal_acceleration.lane_changing;

  const auto min_acc = std::min(lane_changing_acc, global_min_acc);
  const auto sampling_num =
    std::abs(min_acc - lane_changing_acc) > floating_err_th ? deceleration_sampling_num : 1;
  const auto acc_resolution = (min_acc - lane_changing_acc) / static_cast<double>(sampling_num);

  const auto ego_predicted_path = [&](size_t n) {
    if (lane_change_path.type == PathType::FrenetPlanner) {
      return convert_to_predicted_path(
        common_data_ptr, lane_change_path.frenet_path, deceleration_sampling_num);
    }
    auto acc = lane_changing_acc + static_cast<double>(n) * acc_resolution;
    return utils::lane_change::convert_to_predicted_path(common_data_ptr, lane_change_path, acc);
  };

  return ranges::views::iota(0UL, sampling_num) | ranges::views::transform(ego_predicted_path) |
         ranges::to<std::vector>();
}

std::vector<PoseWithVelocityStamped> convert_to_predicted_path(
  const CommonDataPtr & common_data_ptr, const lane_change::TrajectoryGroup & frenet_candidate,
  [[maybe_unused]] const size_t deceleration_sampling_num)
{
  const auto initial_velocity = common_data_ptr->get_ego_speed();
  const auto prepare_time = frenet_candidate.prepare_metric.duration;
  const auto resolution =
    common_data_ptr->lc_param_ptr->safety.collision_check.prediction_time_resolution;
  const auto prepare_acc = frenet_candidate.prepare_metric.sampled_lon_accel;
  std::vector<PoseWithVelocityStamped> predicted_path;
  const auto & path = frenet_candidate.prepare.points;
  const auto & vehicle_pose = common_data_ptr->get_ego_pose();
  const auto & bpp_param_ptr = common_data_ptr->bpp_param_ptr;
  const auto nearest_seg_idx =
    autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints(
      path, vehicle_pose, bpp_param_ptr->ego_nearest_dist_threshold,
      bpp_param_ptr->ego_nearest_yaw_threshold);

  const auto vehicle_pose_frenet =
    convertToFrenetPoint(path, vehicle_pose.position, nearest_seg_idx);

  for (double t = 0.0; t < prepare_time; t += resolution) {
    const auto velocity =
      std::clamp(initial_velocity + prepare_acc * t, 0.0, frenet_candidate.prepare_metric.velocity);
    const auto length = initial_velocity * t + 0.5 * prepare_acc * t * t;
    const auto pose =
      autoware::motion_utils::calcInterpolatedPose(path, vehicle_pose_frenet.length + length);
    predicted_path.emplace_back(t, pose, velocity);
  }

  const auto & poses = frenet_candidate.lane_changing.poses;
  const auto & velocities = frenet_candidate.lane_changing.longitudinal_velocities;
  const auto & times = frenet_candidate.lane_changing.times;

  for (const auto [t, pose, velocity] :
       ranges::views::zip(times, poses, velocities) | ranges::views::drop(1)) {
    predicted_path.emplace_back(prepare_time + t, pose, velocity);
  }

  return predicted_path;
}

bool is_valid_start_point(const lane_change::CommonDataPtr & common_data_ptr, const Pose & pose)
{
  const lanelet::BasicPoint2d lc_start_point(pose.position.x, pose.position.y);

  const auto & target_neighbor_poly = common_data_ptr->lanes_polygon_ptr->target_neighbor;
  const auto & target_lane_poly = common_data_ptr->lanes_polygon_ptr->target;

  // Check the target lane because the previous approved path might be shifted by avoidance module
  return boost::geometry::covered_by(lc_start_point, target_neighbor_poly) ||
         boost::geometry::covered_by(lc_start_point, target_lane_poly);
}
}  // namespace autoware::behavior_path_planner::utils::lane_change

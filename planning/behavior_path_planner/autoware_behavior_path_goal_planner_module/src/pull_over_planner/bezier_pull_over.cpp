// Copyright 2024 TIER IV, Inc.
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

#include "autoware/behavior_path_goal_planner_module/pull_over_planner/bezier_pull_over.hpp"

#include "autoware/behavior_path_goal_planner_module/util.hpp"
#include "autoware/behavior_path_planner_common/utils/drivable_area_expansion/static_drivable_area.hpp"
#include "autoware/behavior_path_planner_common/utils/path_utils.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"

#include <autoware/motion_utils/trajectory/path_shift.hpp>
#include <autoware_bezier_sampler/bezier_sampling.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
namespace autoware::behavior_path_planner
{
BezierPullOver::BezierPullOver(rclcpp::Node & node, const GoalPlannerParameters & parameters)
: PullOverPlannerBase(node, parameters),
  lane_departure_checker_(
    lane_departure_checker::Param{parameters.lane_departure_check_expansion_margin}, vehicle_info_),
  left_side_parking_(parameters.parking_policy == ParkingPolicy::LEFT_SIDE)
{
}
std::optional<PullOverPath> BezierPullOver::plan(
  [[maybe_unused]] const GoalCandidate & modified_goal_pose, [[maybe_unused]] const size_t id,
  [[maybe_unused]] const std::shared_ptr<const PlannerData> planner_data,
  [[maybe_unused]] const BehaviorModuleOutput & upstream_module_output)
{
  const auto & route_handler = planner_data->route_handler;
  const double min_jerk = parameters_.minimum_lateral_jerk;
  const double max_jerk = parameters_.maximum_lateral_jerk;
  const double backward_search_length = parameters_.backward_goal_search_length;
  const double forward_search_length = parameters_.forward_goal_search_length;
  const int shift_sampling_num = parameters_.shift_sampling_num;
  [[maybe_unused]] const double jerk_resolution =
    std::abs(max_jerk - min_jerk) / shift_sampling_num;

  const auto road_lanes = utils::getExtendedCurrentLanesFromPath(
    upstream_module_output.path, planner_data, backward_search_length, forward_search_length,
    /*forward_only_in_route*/ false);

  const auto pull_over_lanes = goal_planner_utils::getPullOverLanes(
    *route_handler, left_side_parking_, backward_search_length, forward_search_length);
  if (road_lanes.empty() || pull_over_lanes.empty()) {
    return {};
  }

  // find safe one from paths with different jerk
  for (double lateral_jerk = min_jerk; lateral_jerk <= max_jerk; lateral_jerk += jerk_resolution) {
    const auto pull_over_path = generateBezierPath(
      modified_goal_pose, id, planner_data, upstream_module_output, road_lanes, pull_over_lanes,
      lateral_jerk);
    if (!pull_over_path.empty()) {
      return pull_over_path.at(0);
    }
  }

  return std::nullopt;
}

std::vector<PullOverPath> BezierPullOver::plans(
  [[maybe_unused]] const GoalCandidate & modified_goal_pose, [[maybe_unused]] const size_t id,
  [[maybe_unused]] const std::shared_ptr<const PlannerData> planner_data,
  [[maybe_unused]] const BehaviorModuleOutput & upstream_module_output)
{
  const auto & route_handler = planner_data->route_handler;
  const double min_jerk = parameters_.minimum_lateral_jerk;
  const double max_jerk = parameters_.maximum_lateral_jerk;
  const double backward_search_length = parameters_.backward_goal_search_length;
  const double forward_search_length = parameters_.forward_goal_search_length;
  const int shift_sampling_num = parameters_.shift_sampling_num;
  [[maybe_unused]] const double jerk_resolution =
    std::abs(max_jerk - min_jerk) / shift_sampling_num;

  const auto road_lanes = utils::getExtendedCurrentLanesFromPath(
    upstream_module_output.path, planner_data, backward_search_length, forward_search_length,
    /*forward_only_in_route*/ false);

  const auto pull_over_lanes = goal_planner_utils::getPullOverLanes(
    *route_handler, left_side_parking_, backward_search_length, forward_search_length);
  if (road_lanes.empty() || pull_over_lanes.empty()) {
    return {};
  }

  // find safe one from paths with different jerk
  std::vector<PullOverPath> paths;
  for (double lateral_jerk = min_jerk; lateral_jerk <= max_jerk; lateral_jerk += jerk_resolution) {
    auto pull_over_paths = generateBezierPath(
      modified_goal_pose, id, planner_data, upstream_module_output, road_lanes, pull_over_lanes,
      lateral_jerk);
    std::copy(
      std::make_move_iterator(pull_over_paths.begin()),
      std::make_move_iterator(pull_over_paths.end()), std::back_inserter(paths));
  }

  return paths;
}

std::vector<PullOverPath> BezierPullOver::generateBezierPath(
  const GoalCandidate & goal_candidate, const size_t id,
  const std::shared_ptr<const PlannerData> planner_data,
  const BehaviorModuleOutput & upstream_module_output, const lanelet::ConstLanelets & road_lanes,
  const lanelet::ConstLanelets & pull_over_lanes, const double lateral_jerk) const
{
  const double pull_over_velocity = parameters_.pull_over_velocity;

  const auto & goal_pose = goal_candidate.goal_pose;

  // shift end pose is longitudinal offset from goal pose to improve parking angle accuracy
  const Pose shift_end_pose = goal_pose;

  // calculate lateral shift of previous module path terminal pose from road lane reference path
  const auto road_lane_reference_path_to_shift_end = utils::resamplePathWithSpline(
    generateReferencePath(planner_data, road_lanes, shift_end_pose),
    parameters_.center_line_path_interval);
  const auto prev_module_path = utils::resamplePathWithSpline(
    upstream_module_output.path, parameters_.center_line_path_interval);
  const auto prev_module_path_terminal_pose = prev_module_path.points.back().point.pose;

  // process previous module path for path shifter input path
  // case1) extend path if shift end pose is behind of previous module path terminal pose
  // case2) crop path if shift end pose is ahead of previous module path terminal pose
  const auto processed_prev_module_path = std::invoke([&]() -> std::optional<PathWithLaneId> {
    const bool extend_previous_module_path =
      lanelet::utils::getArcCoordinates(road_lanes, shift_end_pose).length >
      lanelet::utils::getArcCoordinates(road_lanes, prev_module_path_terminal_pose).length;
    if (extend_previous_module_path) {  // case1
      // NOTE: The previous module may insert a zero velocity at the end of the path, so remove it
      // by setting remove_connected_zero_velocity=true. Inserting a velocity of 0 into the goal is
      // the role of the goal planner, and the intermediate zero velocity after extension is
      // unnecessary.
      return goal_planner_utils::extendPath(
        prev_module_path, road_lane_reference_path_to_shift_end, shift_end_pose, true);
    } else {  // case2
      return goal_planner_utils::cropPath(prev_module_path, shift_end_pose);
    }
  });
  if (!processed_prev_module_path || processed_prev_module_path->points.empty()) {
    return {};
  }

  // calculate shift length
  const Pose & shift_end_pose_prev_module_path =
    processed_prev_module_path->points.back().point.pose;
  const double shift_end_road_to_target_distance =
    autoware::universe_utils::inverseTransformPoint(
      shift_end_pose.position, shift_end_pose_prev_module_path)
      .y;

  // calculate shift start pose on road lane
  const double pull_over_distance = autoware::motion_utils::calc_longitudinal_dist_from_jerk(
    shift_end_road_to_target_distance, lateral_jerk, pull_over_velocity);
  const double before_shifted_pull_over_distance = calcBeforeShiftedArcLength(
    processed_prev_module_path.value(), pull_over_distance, shift_end_road_to_target_distance);
  const auto shift_start_pose = autoware::motion_utils::calcLongitudinalOffsetPose(
    processed_prev_module_path->points, shift_end_pose_prev_module_path.position,
    -before_shifted_pull_over_distance);

  std::vector<std::tuple<double, double, double>> params;
  const size_t n_sample_v_init = 4;
  const size_t n_sample_v_final = 4;
  const size_t n_sample_acc = 3;
  for (unsigned i = 0; i <= n_sample_v_init; ++i) {
    for (unsigned j = 0; j <= n_sample_v_final; j++) {
      for (unsigned k = 0; k <= n_sample_acc; k++) {
        const double v_init_coeff = i * 0.25;
        const double v_final_coeff = j * 0.25;
        const double acc_coeff = k * (10.0 / 3);
        params.emplace_back(v_init_coeff, v_final_coeff, acc_coeff);
      }
    }
  }

  std::vector<PullOverPath> bezier_paths;
  for (const auto & [v_init_coeff, v_final_coeff, acc_coeff] : params) {
    // set path shifter and generate shifted path
    PathShifter path_shifter{};
    path_shifter.setPath(processed_prev_module_path.value());
    ShiftLine shift_line{};
    shift_line.start = *shift_start_pose;
    shift_line.end = shift_end_pose;
    shift_line.end_shift_length = shift_end_road_to_target_distance;
    path_shifter.addShiftLine(shift_line);
    ShiftedPath shifted_path{};
    const bool offset_back = true;  // offset front side from reference path
    if (!path_shifter.generate(&shifted_path, offset_back)) {
      continue;
    }
    const auto from_idx_opt =
      autoware::motion_utils::findNearestIndex(shifted_path.path.points, *shift_start_pose);
    const auto to_idx_opt =
      autoware::motion_utils::findNearestIndex(shifted_path.path.points, shift_end_pose);
    if (!from_idx_opt || !to_idx_opt) {
      continue;
    }
    const auto from_idx = from_idx_opt.value();
    const auto to_idx = to_idx_opt.value();
    const auto span = static_cast<unsigned>(
      std::max<int>(static_cast<int>(to_idx) - static_cast<int>(from_idx), 0));
    const auto reference_curvature =
      autoware::motion_utils::calcCurvature(processed_prev_module_path->points);
    const auto & from_pose = shifted_path.path.points[from_idx].point.pose;
    const auto & to_pose = shifted_path.path.points[to_idx].point.pose;
    const autoware::sampler_common::State initial{
      {from_pose.position.x, from_pose.position.y},
      {0.0, 0.0},
      reference_curvature.at(from_idx),
      tf2::getYaw(from_pose.orientation)};
    const autoware::sampler_common::State final{
      {to_pose.position.x, to_pose.position.y}, {0.0, 0.0}, 0.0, tf2::getYaw(to_pose.orientation)};
    // setting the initial velocity to higher gives straight forward path (the steering does not
    // change)
    const auto bezier_path =
      bezier_sampler::sample(initial, final, v_init_coeff, v_final_coeff, acc_coeff);
    const auto bezier_points = bezier_path.cartesianWithHeading(span);
    for (unsigned i = 0; i + 1 < span; ++i) {
      auto & p = shifted_path.path.points[from_idx + i];
      p.point.pose.position.x = bezier_points[i].x();
      p.point.pose.position.y = bezier_points[i].y();
      p.point.pose.orientation =
        universe_utils::createQuaternionFromRPY(0.0, 0.0, bezier_points[i].z());
    }
    shifted_path.path.points =
      autoware::motion_utils::removeOverlapPoints(shifted_path.path.points);

    autoware::motion_utils::insertOrientation(shifted_path.path.points, true);

    // combine road lanes and shoulder lanes to find closest lanelet id
    const auto lanes = std::invoke([&]() -> lanelet::ConstLanelets {
      auto lanes = road_lanes;
      lanes.insert(lanes.end(), pull_over_lanes.begin(), pull_over_lanes.end());
      return lanes;  // not copy the value (Return Value Optimization)
    });

    // set goal pose with velocity 0
    {
      PathPointWithLaneId p{};
      p.point.longitudinal_velocity_mps = 0.0;
      p.point.pose = goal_pose;
      lanelet::Lanelet goal_lanelet{};
      if (lanelet::utils::query::getClosestLanelet(lanes, goal_pose, &goal_lanelet)) {
        p.lane_ids = {goal_lanelet.id()};
      } else {
        p.lane_ids = shifted_path.path.points.back().lane_ids;
      }
      shifted_path.path.points.push_back(p);
    }

    // set lane_id and velocity to shifted_path
    for (size_t i = path_shifter.getShiftLines().front().start_idx;
         i < shifted_path.path.points.size() - 1; ++i) {
      auto & point = shifted_path.path.points.at(i);
      point.point.longitudinal_velocity_mps =
        std::min(point.point.longitudinal_velocity_mps, static_cast<float>(pull_over_velocity));
      lanelet::Lanelet lanelet{};
      if (lanelet::utils::query::getClosestLanelet(lanes, point.point.pose, &lanelet)) {
        point.lane_ids = {lanelet.id()};  // overwrite lane_ids
      } else {
        point.lane_ids = shifted_path.path.points.at(i - 1).lane_ids;
      }
    }

    // set pull over path
    auto pull_over_path_opt = PullOverPath::create(
      getPlannerType(), id, {shifted_path.path}, path_shifter.getShiftLines().front().start,
      goal_candidate, {std::make_pair(pull_over_velocity, 0)});

    if (!pull_over_path_opt) {
      continue;
    }
    auto & pull_over_path = pull_over_path_opt.value();

    // check if the parking path will leave drivable area and lanes
    const bool is_in_parking_lots = std::invoke([&]() -> bool {
      const auto & p = planner_data->parameters;
      const auto parking_lot_polygons =
        lanelet::utils::query::getAllParkingLots(planner_data->route_handler->getLaneletMapPtr());
      const auto path_footprints = goal_planner_utils::createPathFootPrints(
        pull_over_path.parking_path(), p.base_link2front, p.base_link2rear, p.vehicle_width);
      const auto is_footprint_in_any_polygon = [&parking_lot_polygons](const auto & footprint) {
        return std::any_of(
          parking_lot_polygons.begin(), parking_lot_polygons.end(),
          [&footprint](const auto & polygon) {
            return lanelet::geometry::within(
              footprint, lanelet::utils::to2D(polygon).basicPolygon());
          });
      };
      return std::all_of(
        path_footprints.begin(), path_footprints.end(),
        [&is_footprint_in_any_polygon](const auto & footprint) {
          return is_footprint_in_any_polygon(footprint);
        });
    });
    const bool is_in_lanes = std::invoke([&]() -> bool {
      const auto drivable_lanes =
        utils::generateDrivableLanesWithShoulderLanes(road_lanes, pull_over_lanes);
      const auto & dp = planner_data->drivable_area_expansion_parameters;
      const auto expanded_lanes = utils::expandLanelets(
        drivable_lanes, dp.drivable_area_left_bound_offset, dp.drivable_area_right_bound_offset,
        dp.drivable_area_types_to_skip);
      const auto combined_drivable = utils::combineDrivableLanes(
        expanded_lanes, upstream_module_output.drivable_area_info.drivable_lanes);
      return !lane_departure_checker_.checkPathWillLeaveLane(
        utils::transformToLanelets(combined_drivable), pull_over_path.parking_path());
    });
    if (!is_in_parking_lots && !is_in_lanes) {
      continue;
    }
    bezier_paths.push_back(std::move(pull_over_path));
  }

  return bezier_paths;
}

PathWithLaneId BezierPullOver::generateReferencePath(
  const std::shared_ptr<const PlannerData> planner_data, const lanelet::ConstLanelets & road_lanes,
  const Pose & end_pose) const
{
  const auto & route_handler = planner_data->route_handler;
  const Pose & current_pose = planner_data->self_odometry->pose.pose;
  const double backward_path_length = planner_data->parameters.backward_path_length;
  const double pull_over_velocity = parameters_.pull_over_velocity;
  const double deceleration_interval = parameters_.deceleration_interval;

  const auto current_road_arc_coords = lanelet::utils::getArcCoordinates(road_lanes, current_pose);
  const double s_start = current_road_arc_coords.length - backward_path_length;
  const double s_end = std::max(
    lanelet::utils::getArcCoordinates(road_lanes, end_pose).length,
    s_start + std::numeric_limits<double>::epsilon());
  auto road_lane_reference_path = route_handler->getCenterLinePath(road_lanes, s_start, s_end);

  // decelerate velocity linearly to minimum pull over velocity
  // (or keep original velocity if it is lower than pull over velocity)
  for (auto & point : road_lane_reference_path.points) {
    const auto arclength = lanelet::utils::getArcCoordinates(road_lanes, point.point.pose).length;
    const double distance_to_pull_over_start =
      std::clamp(s_end - arclength, 0.0, deceleration_interval);
    const auto decelerated_velocity = static_cast<float>(
      distance_to_pull_over_start / deceleration_interval *
        (point.point.longitudinal_velocity_mps - pull_over_velocity) +
      pull_over_velocity);
    point.point.longitudinal_velocity_mps =
      std::min(point.point.longitudinal_velocity_mps, decelerated_velocity);
  }
  return road_lane_reference_path;
}

double BezierPullOver::calcBeforeShiftedArcLength(
  const PathWithLaneId & path, const double target_after_arc_length, const double dr)
{
  // reverse path for checking from the end point
  // note that the sign of curvature is also reversed
  PathWithLaneId reversed_path{};
  std::reverse_copy(
    path.points.begin(), path.points.end(), std::back_inserter(reversed_path.points));

  double after_arc_length{0.0};

  const auto curvature_and_segment_length =
    autoware::motion_utils::calcCurvatureAndSegmentLength(reversed_path.points);

  for (size_t i = 0; i < curvature_and_segment_length.size(); ++i) {
    const auto & [k, segment_length_pair] = curvature_and_segment_length[i];

    // If it is the last point, add the lengths of the previous and next segments.
    // For other points, only add the length of the previous segment.
    const double segment_length = i == curvature_and_segment_length.size() - 1
                                    ? segment_length_pair.first
                                    : segment_length_pair.first + segment_length_pair.second;

    // after shifted segment length
    const double after_segment_length =
      k > 0 ? segment_length * (1 + k * dr) : segment_length / (1 - k * dr);
    if (after_arc_length + after_segment_length > target_after_arc_length) {
      const double offset = target_after_arc_length - after_arc_length;
      after_arc_length += k > 0 ? offset * (1 + k * dr) : offset / (1 - k * dr);
      return after_arc_length;
    }
    after_arc_length += after_segment_length;
  }

  return after_arc_length;
}

}  // namespace autoware::behavior_path_planner

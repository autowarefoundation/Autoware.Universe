
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

#include <autoware/behavior_path_lane_change_module/utils/calculation.hpp>
#include <autoware/behavior_path_planner_common/utils/utils.hpp>
#include <autoware/motion_utils/trajectory/path_shift.hpp>

#include <boost/geometry/algorithms/buffer.hpp>

namespace autoware::behavior_path_planner::utils::lane_change::calculation
{

rclcpp::Logger get_logger()
{
  constexpr const char * name{"lane_change.utils"};
  static rclcpp::Logger logger = rclcpp::get_logger(name);
  return logger;
}

double calc_dist_from_pose_to_terminal_end(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & lanes,
  const Pose & src_pose)
{
  if (lanes.empty()) {
    return 0.0;
  }

  const auto in_goal_route_section =
    common_data_ptr->route_handler_ptr->isInGoalRouteSection(lanes.back());
  if (in_goal_route_section) {
    const auto & goal_pose = common_data_ptr->route_handler_ptr->getGoalPose();
    return utils::getSignedDistance(src_pose, goal_pose, lanes);
  }
  return utils::getDistanceToEndOfLane(src_pose, lanes);
}

double calc_stopping_distance(const LCParamPtr & lc_param_ptr)
{
  // v^2 = u^2 + 2ad
  const auto min_lc_vel = lc_param_ptr->trajectory.min_lane_changing_velocity;
  const auto min_lon_acc = lc_param_ptr->trajectory.min_longitudinal_acc;
  const auto min_back_dist = std::abs((min_lc_vel * min_lc_vel) / (2 * min_lon_acc));

  const auto param_back_dist = lc_param_ptr->backward_length_buffer_for_end_of_lane;
  return std::max(min_back_dist, param_back_dist);
}

double calc_dist_to_last_fit_width(
  const lanelet::ConstLanelets & lanelets, const lanelet::BasicPolygon2d & lanelet_polygon,
  const universe_utils::LineString2d & line_string, const Pose & src_pose,
  const BehaviorPathPlannerParameters & bpp_param, const double margin)
{
  const double buffer_distance = 0.5 * bpp_param.vehicle_width + margin;
  universe_utils::MultiPolygon2d center_line_polygon;
  namespace strategy = boost::geometry::strategy::buffer;
  boost::geometry::buffer(
    line_string, center_line_polygon, strategy::distance_symmetric<double>(buffer_distance),
    strategy::side_straight(), strategy::join_miter(), strategy::end_flat(),
    strategy::point_square());

  if (center_line_polygon.empty()) return 0.0;

  std::vector<universe_utils::Point2d> intersection_points;
  boost::geometry::intersection(lanelet_polygon, center_line_polygon, intersection_points);

  if (intersection_points.empty()) {
    return utils::getDistanceToEndOfLane(src_pose, lanelets);
  }

  Pose pose;
  double distance = std::numeric_limits<double>::max();
  for (const auto & point : intersection_points) {
    pose.position.x = boost::geometry::get<0>(point);
    pose.position.y = boost::geometry::get<1>(point);
    distance = std::min(distance, utils::getSignedDistance(src_pose, pose, lanelets));
  }

  return std::max(distance - (bpp_param.base_link2front + margin), 0.0);
}

double calc_dist_to_last_fit_width(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & path, const double margin)
{
  const auto & current_lanes = common_data_ptr->lanes_ptr->current;
  const auto & current_lanes_polygon = common_data_ptr->lanes_polygon_ptr->current;
  const auto & bpp_param = *common_data_ptr->bpp_param_ptr;

  universe_utils::LineString2d line_string;
  line_string.reserve(path.points.size() - 1);
  std::for_each(path.points.begin() + 1, path.points.end(), [&line_string](const auto & point) {
    const auto & position = point.point.pose.position;
    boost::geometry::append(line_string, universe_utils::Point2d(position.x, position.y));
  });

  const auto & src_pose = path.points.front().point.pose;
  return calc_dist_to_last_fit_width(
    current_lanes, current_lanes_polygon, line_string, src_pose, bpp_param, margin);
}

double calc_dist_to_last_fit_width(
  const lanelet::ConstLanelets & lanelets, const Pose & src_pose,
  const BehaviorPathPlannerParameters & bpp_param, const double margin)
{
  if (lanelets.empty()) return 0.0;

  const auto lane_polygon = lanelets.back().polygon2d().basicPolygon();
  const auto center_line = lanelet::utils::generateFineCenterline(lanelets.back(), 1.0);

  if (center_line.size() <= 1) return 0.0;

  universe_utils::LineString2d line_string;
  line_string.reserve(center_line.size() - 1);
  std::for_each(center_line.begin() + 1, center_line.end(), [&line_string](const auto & point) {
    boost::geometry::append(line_string, universe_utils::Point2d(point.x(), point.y()));
  });

  return calc_dist_to_last_fit_width(
    lanelets, lane_polygon, line_string, src_pose, bpp_param, margin);
}

double calc_maximum_prepare_length(const CommonDataPtr & common_data_ptr)
{
  const auto max_prepare_duration = common_data_ptr->lc_param_ptr->trajectory.max_prepare_duration;
  const auto ego_max_speed = common_data_ptr->bpp_param_ptr->max_vel;

  return max_prepare_duration * ego_max_speed;
}

double calc_ego_dist_to_lanes_start(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & current_lanes,
  const lanelet::ConstLanelets & target_lanes)
{
  const auto & route_handler_ptr = common_data_ptr->route_handler_ptr;

  if (!route_handler_ptr || target_lanes.empty() || current_lanes.empty()) {
    return std::numeric_limits<double>::max();
  }

  const auto & target_bound =
    common_data_ptr->direction == autoware::route_handler::Direction::RIGHT
      ? target_lanes.front().leftBound()
      : target_lanes.front().rightBound();

  if (target_bound.empty()) {
    return std::numeric_limits<double>::max();
  }

  const auto & path = common_data_ptr->current_lanes_path;

  if (path.points.empty()) {
    return std::numeric_limits<double>::max();
  }

  const auto target_front_pt = lanelet::utils::conversion::toGeomMsgPt(target_bound.front());
  const auto ego_position = common_data_ptr->get_ego_pose().position;

  return motion_utils::calcSignedArcLength(path.points, ego_position, target_front_pt);
}

double calc_minimum_acceleration(
  const LaneChangeParameters & lane_change_parameters, const double current_velocity,
  const double min_acc_threshold, const double prepare_duration)
{
  if (prepare_duration < eps) return -std::abs(min_acc_threshold);
  const double min_lc_velocity = lane_change_parameters.trajectory.min_lane_changing_velocity;
  const double acc = (min_lc_velocity - current_velocity) / prepare_duration;
  return std::clamp(acc, -std::abs(min_acc_threshold), -eps);
}

double calc_maximum_acceleration(
  const double prepare_duration, const double current_velocity, const double current_max_velocity,
  const double max_acc_threshold)
{
  if (prepare_duration < eps) return max_acc_threshold;
  const double acc = (current_max_velocity - current_velocity) / prepare_duration;
  return std::clamp(acc, 0.0, max_acc_threshold);
}

std::vector<double> calc_min_lane_change_lengths(
  const LCParamPtr & lc_param_ptr, const std::vector<double> & shift_intervals)
{
  if (shift_intervals.empty()) {
    return {};
  }

  const auto min_vel = lc_param_ptr->trajectory.min_lane_changing_velocity;
  const auto min_max_lat_acc = lc_param_ptr->trajectory.lat_acc_map.find(min_vel);
  const auto max_lat_acc = std::get<1>(min_max_lat_acc);
  const auto lat_jerk = lc_param_ptr->trajectory.lateral_jerk;

  std::vector<double> min_lc_lengths{};
  min_lc_lengths.reserve(shift_intervals.size());

  const auto min_lc_length = [&](const auto shift_interval) {
    const auto t =
      autoware::motion_utils::calc_shift_time_from_jerk(shift_interval, lat_jerk, max_lat_acc);
    return min_vel * t;
  };

  std::transform(
    shift_intervals.cbegin(), shift_intervals.cend(), std::back_inserter(min_lc_lengths),
    min_lc_length);

  return min_lc_lengths;
}

std::vector<double> calc_max_lane_change_lengths(
  const CommonDataPtr & common_data_ptr, const std::vector<double> & shift_intervals)
{
  if (shift_intervals.empty()) {
    return {};
  }

  const auto & params = common_data_ptr->lc_param_ptr->trajectory;
  const auto lat_jerk = params.lateral_jerk;
  const auto t_prepare = params.max_prepare_duration;
  const auto current_velocity = common_data_ptr->get_ego_speed();
  const auto path_velocity = common_data_ptr->transient_data.current_path_velocity;

  const auto max_acc = calc_maximum_acceleration(
    t_prepare, current_velocity, path_velocity, params.max_longitudinal_acc);

  // TODO(Quda, Azu): should probably limit upper bound of velocity as well, but
  // disabled due failing evaluation tests.
  // const auto limit_vel = [&](const auto vel) {
  //   const auto max_global_vel = common_data_ptr->bpp_param_ptr->max_vel;
  //   return std::clamp(vel, params.min_lane_changing_velocity, max_global_vel);
  // };

  auto vel = std::max(common_data_ptr->get_ego_speed(), params.min_lane_changing_velocity);

  std::vector<double> max_lc_lengths{};

  const auto max_lc_length = [&](const auto shift_interval) {
    // prepare section
    const auto prepare_length = vel * t_prepare + 0.5 * max_acc * t_prepare * t_prepare;
    vel = vel + max_acc * t_prepare;

    // lane changing section
    const auto [min_lat_acc, max_lat_acc] = params.lat_acc_map.find(vel);
    const auto t_lane_changing =
      autoware::motion_utils::calc_shift_time_from_jerk(shift_interval, lat_jerk, max_lat_acc);
    const auto lane_changing_length =
      vel * t_lane_changing + 0.5 * max_acc * t_lane_changing * t_lane_changing;

    vel = vel + max_acc * t_lane_changing;
    return prepare_length + lane_changing_length;
  };

  std::transform(
    shift_intervals.cbegin(), shift_intervals.cend(), std::back_inserter(max_lc_lengths),
    max_lc_length);
  return max_lc_lengths;
}

double calc_distance_buffer(const LCParamPtr & lc_param_ptr, const std::vector<double> & lc_lengths)
{
  if (lc_lengths.empty()) {
    return std::numeric_limits<double>::max();
  }

  const auto finish_judge_buffer = lc_param_ptr->lane_change_finish_judge_buffer;
  const auto backward_buffer = calc_stopping_distance(lc_param_ptr);
  const auto lengths_sum = std::accumulate(lc_lengths.begin(), lc_lengths.end(), 0.0);
  const auto num_of_lane_changes = static_cast<double>(lc_lengths.size());
  return lengths_sum + (num_of_lane_changes * finish_judge_buffer) +
         ((num_of_lane_changes - 1.0) * backward_buffer);
}

std::vector<double> calc_shift_intervals(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & lanes)
{
  if (!common_data_ptr || !common_data_ptr->is_data_available() || lanes.empty()) {
    return {};
  }

  const auto & route_handler_ptr = common_data_ptr->route_handler_ptr;
  const auto direction = common_data_ptr->direction;

  return route_handler_ptr->getLateralIntervalsToPreferredLane(lanes.back(), direction);
}

std::pair<MinMaxValue, MinMaxValue> calc_lc_length_and_dist_buffer(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & lanes)
{
  if (!common_data_ptr || !common_data_ptr->is_data_available() || lanes.empty()) {
    return {};
  }
  const auto shift_intervals = calculation::calc_shift_intervals(common_data_ptr, lanes);
  const auto min_lc_lengths =
    calculation::calc_min_lane_change_lengths(common_data_ptr->lc_param_ptr, shift_intervals);
  const auto min_lc_length =
    !min_lc_lengths.empty() ? min_lc_lengths.front() : std::numeric_limits<double>::max();
  const auto min_dist_buffer =
    calculation::calc_distance_buffer(common_data_ptr->lc_param_ptr, min_lc_lengths);

  const auto max_lc_lengths =
    calculation::calc_max_lane_change_lengths(common_data_ptr, shift_intervals);
  const auto max_lc_length =
    !max_lc_lengths.empty() ? max_lc_lengths.front() : std::numeric_limits<double>::max();
  const auto max_dist_buffer =
    calculation::calc_distance_buffer(common_data_ptr->lc_param_ptr, max_lc_lengths);

  return {{min_lc_length, max_lc_length}, {min_dist_buffer, max_dist_buffer}};
}

double calc_phase_length(
  const double velocity, const double maximum_velocity, const double acceleration,
  const double duration)
{
  const auto length_with_acceleration =
    velocity * duration + 0.5 * acceleration * std::pow(duration, 2);
  const auto length_with_max_velocity = maximum_velocity * duration;
  return std::min(length_with_acceleration, length_with_max_velocity);
}

std::pair<double, double> calc_min_max_acceleration(
  const CommonDataPtr & common_data_ptr, const double max_path_velocity,
  const double prepare_duration)
{
  const auto & lc_params = *common_data_ptr->lc_param_ptr;
  const auto & bpp_params = *common_data_ptr->bpp_param_ptr;
  const auto current_ego_velocity = common_data_ptr->get_ego_speed();

  const auto min_accel_threshold =
    std::max(bpp_params.min_acc, lc_params.trajectory.min_longitudinal_acc);
  const auto max_accel_threshold =
    std::min(bpp_params.max_acc, lc_params.trajectory.max_longitudinal_acc);

  // calculate minimum and maximum acceleration
  const auto min_acc = calc_minimum_acceleration(
    lc_params, current_ego_velocity, min_accel_threshold, prepare_duration);
  const auto max_acc = calc_maximum_acceleration(
    prepare_duration, current_ego_velocity, max_path_velocity, max_accel_threshold);

  return {min_acc, max_acc};
}

std::vector<double> calc_acceleration_values(
  const double min_accel, const double max_accel, const double sampling_num)
{
  if (min_accel > max_accel) return {};

  if (max_accel - min_accel < eps) {
    return {min_accel};
  }

  const auto resolution = std::abs(max_accel - min_accel) / sampling_num;

  std::vector<double> sampled_values{min_accel};
  for (double accel = min_accel + resolution; accel < max_accel + eps; accel += resolution) {
    // check whether if we need to add 0.0
    if (sampled_values.back() < -eps && accel > eps) {
      sampled_values.push_back(0.0);
    }

    sampled_values.push_back(accel);
  }
  std::reverse(sampled_values.begin(), sampled_values.end());

  return sampled_values;
}

std::vector<double> calc_lon_acceleration_samples(
  const CommonDataPtr & common_data_ptr, const double max_path_velocity,
  const double prepare_duration)
{
  const auto & transient_data = common_data_ptr->transient_data;
  const auto & current_pose = common_data_ptr->get_ego_pose();
  const auto & target_lanes = common_data_ptr->lanes_ptr->target;
  const auto goal_pose = common_data_ptr->route_handler_ptr->getGoalPose();
  const auto sampling_num = common_data_ptr->lc_param_ptr->trajectory.lon_acc_sampling_num;

  const auto min_max_accel =
    calc_min_max_acceleration(common_data_ptr, max_path_velocity, prepare_duration);
  const auto & min_accel = min_max_accel.first;
  const auto & max_accel = min_max_accel.second;

  const auto is_sampling_required = std::invoke([&]() -> bool {
    if (max_accel < 0.0 || transient_data.is_ego_stuck) return true;

    const auto max_dist_buffer = transient_data.current_dist_buffer.max;
    if (max_dist_buffer > transient_data.dist_to_terminal_end) return true;

    const auto dist_to_target_lane_end =
      common_data_ptr->lanes_ptr->target_lane_in_goal_section
        ? utils::getSignedDistance(current_pose, goal_pose, target_lanes)
        : utils::getDistanceToEndOfLane(current_pose, target_lanes);

    return max_dist_buffer >= dist_to_target_lane_end;
  });

  if (is_sampling_required) {
    return calc_acceleration_values(min_accel, max_accel, sampling_num);
  }

  return {max_accel};
}

double calc_lane_changing_acceleration(
  const CommonDataPtr & common_data_ptr, const double initial_lane_changing_velocity,
  const double max_path_velocity, const double lane_changing_time,
  const double prepare_longitudinal_acc)
{
  if (prepare_longitudinal_acc <= 0.0) {
    const auto & params = common_data_ptr->lc_param_ptr->trajectory;
    const auto lane_changing_acc =
      common_data_ptr->transient_data.is_ego_near_current_terminal_start
        ? prepare_longitudinal_acc * params.lane_changing_decel_factor
        : 0.0;
    return lane_changing_acc;
  }

  return std::clamp(
    (max_path_velocity - initial_lane_changing_velocity) / lane_changing_time, 0.0,
    prepare_longitudinal_acc);
}

double calc_actual_prepare_duration(
  const CommonDataPtr & common_data_ptr, const double current_velocity,
  const double active_signal_duration)
{
  const auto & params = common_data_ptr->lc_param_ptr->trajectory;
  const auto min_lc_velocity = params.min_lane_changing_velocity;

  // need to ensure min prep duration is sufficient to reach minimum lane changing velocity
  const auto min_prepare_duration = std::invoke([&]() -> double {
    if (current_velocity >= min_lc_velocity) {
      return params.min_prepare_duration;
    }
    const auto max_acc =
      std::min(common_data_ptr->bpp_param_ptr->max_acc, params.max_longitudinal_acc);
    if (max_acc < eps) {
      return params.max_prepare_duration;
    }
    return std::max((min_lc_velocity - current_velocity) / max_acc, params.min_prepare_duration);
  });

  return std::max(params.max_prepare_duration - active_signal_duration, min_prepare_duration);
}

std::vector<double> calc_prepare_durations(const CommonDataPtr & common_data_ptr)
{
  const auto & lc_param_ptr = common_data_ptr->lc_param_ptr;
  const auto threshold = common_data_ptr->bpp_param_ptr->base_link2front +
                         lc_param_ptr->min_length_for_turn_signal_activation;

  // TODO(Azu) this check seems to cause scenario failures.
  if (common_data_ptr->transient_data.dist_to_terminal_start >= threshold) {
    return {common_data_ptr->transient_data.lane_change_prepare_duration};
  }

  const auto max_prepare_duration = lc_param_ptr->trajectory.max_prepare_duration;
  std::vector<double> prepare_durations;
  constexpr double step = 0.5;

  for (double duration = max_prepare_duration; duration >= 0.0; duration -= step) {
    prepare_durations.push_back(duration);
  }

  return prepare_durations;
}

std::vector<PhaseMetrics> calc_prepare_phase_metrics(
  const CommonDataPtr & common_data_ptr, const double current_velocity,
  const double max_path_velocity, const double min_length_threshold,
  const double max_length_threshold)
{
  const auto & min_lc_vel = common_data_ptr->lc_param_ptr->trajectory.min_lane_changing_velocity;
  const auto & max_vel = common_data_ptr->bpp_param_ptr->max_vel;

  std::vector<PhaseMetrics> metrics;

  auto is_skip = [&](const double prepare_length) {
    if (prepare_length > max_length_threshold || prepare_length < min_length_threshold) {
      RCLCPP_DEBUG(
        get_logger(),
        "Skip: prepare length out of expected range. length: %.5f, threshold min: %.5f, max: %.5f",
        prepare_length, min_length_threshold, max_length_threshold);
      return true;
    }
    return false;
  };

  const auto prepare_durations = calc_prepare_durations(common_data_ptr);

  for (const auto & prepare_duration : prepare_durations) {
    const auto lon_accel_samples =
      calc_lon_acceleration_samples(common_data_ptr, max_path_velocity, prepare_duration);
    for (const auto & lon_accel : lon_accel_samples) {
      const auto prepare_velocity =
        std::clamp(current_velocity + lon_accel * prepare_duration, min_lc_vel, max_vel);

      // compute actual longitudinal acceleration
      const double prepare_accel = (prepare_duration < 1e-3)
                                     ? 0.0
                                     : ((prepare_velocity - current_velocity) / prepare_duration);

      const auto prepare_length =
        calc_phase_length(current_velocity, max_vel, prepare_accel, prepare_duration);

      if (is_skip(prepare_length)) continue;

      metrics.emplace_back(
        prepare_duration, prepare_length, prepare_velocity, lon_accel, prepare_accel, 0.0);
    }
  }

  return metrics;
}

std::vector<PhaseMetrics> calc_shift_phase_metrics(
  const CommonDataPtr & common_data_ptr, const double shift_length, const double initial_velocity,
  const double max_path_velocity, const double lon_accel, const double max_length_threshold)
{
  const auto & min_lc_vel = common_data_ptr->lc_param_ptr->trajectory.min_lane_changing_velocity;
  const auto & max_vel = common_data_ptr->bpp_param_ptr->max_vel;

  // get lateral acceleration range
  const auto [min_lateral_acc, max_lateral_acc] =
    common_data_ptr->lc_param_ptr->trajectory.lat_acc_map.find(initial_velocity);
  const auto lateral_acc_resolution =
    std::abs(max_lateral_acc - min_lateral_acc) /
    common_data_ptr->lc_param_ptr->trajectory.lat_acc_sampling_num;

  std::vector<PhaseMetrics> metrics;

  auto is_skip = [&](const double lane_changing_length) {
    if (lane_changing_length > max_length_threshold) {
      RCLCPP_DEBUG(
        get_logger(),
        "Skip: lane changing length exceeds maximum threshold. length: %.5f, threshold: %.5f",
        lane_changing_length, max_length_threshold);
      return true;
    }
    return false;
  };

  for (double lat_acc = min_lateral_acc; lat_acc < max_lateral_acc + eps;
       lat_acc += lateral_acc_resolution) {
    const auto lane_changing_duration = autoware::motion_utils::calc_shift_time_from_jerk(
      shift_length, common_data_ptr->lc_param_ptr->trajectory.lateral_jerk, lat_acc);

    const double lane_changing_accel = calc_lane_changing_acceleration(
      common_data_ptr, initial_velocity, max_path_velocity, lane_changing_duration, lon_accel);

    const auto lane_changing_length = calculation::calc_phase_length(
      initial_velocity, max_vel, lane_changing_accel, lane_changing_duration);

    if (is_skip(lane_changing_length)) continue;

    const auto lane_changing_velocity = std::clamp(
      initial_velocity + lane_changing_accel * lane_changing_duration, min_lc_vel, max_vel);

    metrics.emplace_back(
      lane_changing_duration, lane_changing_length, lane_changing_velocity, lon_accel,
      lane_changing_accel, lat_acc);
  }

  return metrics;
}
}  // namespace autoware::behavior_path_planner::utils::lane_change::calculation

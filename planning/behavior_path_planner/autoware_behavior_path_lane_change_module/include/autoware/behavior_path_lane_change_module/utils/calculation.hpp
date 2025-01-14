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
#ifndef AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__CALCULATION_HPP_
#define AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__CALCULATION_HPP_

#include "autoware/behavior_path_lane_change_module/structs/data.hpp"

#include <autoware/route_handler/route_handler.hpp>

namespace autoware::behavior_path_planner::utils::lane_change::calculation
{
using autoware::route_handler::Direction;
using autoware::route_handler::RouteHandler;
using behavior_path_planner::lane_change::CommonDataPtr;
using behavior_path_planner::lane_change::LCParamPtr;
using behavior_path_planner::lane_change::MinMaxValue;
using behavior_path_planner::lane_change::PhaseMetrics;

static constexpr double eps = 0.001;

double calc_dist_from_pose_to_terminal_end(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & lanes,
  const Pose & src_pose);

/**
 * @brief Calculates the minimum stopping distance to terminal start.
 *
 * This function computes the minimum stopping distance to terminal start based on the
 * minimum lane changing velocity and the minimum longitudinal acceleration. It then
 * compares this calculated distance with a pre-defined backward length buffer parameter
 * and returns the larger of the two values to ensure safe lane changing.
 *
 * @param lc_param_ptr Shared pointer to an LCParam structure, which should include:
 *  - `minimum_lane_changing_velocity`: The minimum velocity required for lane changing.
 *  - `min_longitudinal_acc`: The minimum longitudinal acceleration used for deceleration.
 *  - `backward_length_buffer_for_end_of_lane`: A predefined backward buffer length parameter.
 *
 * @return The required backward buffer distance in meters.
 */
double calc_stopping_distance(const LCParamPtr & lc_param_ptr);

/**
 * @brief Calculates the distance to last fit width position along the lane
 *
 * This function computes the distance from the current ego position to the last
 * position along the lane where the ego foot prints stays within the lane
 * boundaries.
 *
 * @param lanelets current ego lanelets
 * @param src_pose source pose to calculate distance from
 * @param bpp_param common parameters used in behavior path planner.
 * @param margin additional margin for checking lane width
 * @return distance to last fit width position along the lane
 */
double calc_dist_to_last_fit_width(
  const lanelet::ConstLanelets & lanelets, const Pose & src_pose,
  const BehaviorPathPlannerParameters & bpp_param, const double margin = 0.1);

double calc_dist_to_last_fit_width(
  const CommonDataPtr & common_data_ptr, const PathWithLaneId & path, const double margin = 0.1);

/**
 * @brief Calculates the maximum preparation longitudinal distance for lane change.
 *
 * This function computes the maximum distance that the ego vehicle can travel during
 * the preparation phase of a lane change. The distance is calculated as the product
 * of the maximum lane change preparation duration and the maximum velocity of the ego vehicle.
 *
 * @param common_data_ptr Shared pointer to a CommonData structure, which should include:
 *  - `lc_param_ptr->maximum_prepare_duration`: The maximum duration allowed for lane change
 * preparation.
 *  - `bpp_param_ptr->max_vel`: The maximum velocity of the ego vehicle.
 *
 * @return The maximum preparation longitudinal distance in meters.
 */
double calc_maximum_prepare_length(const CommonDataPtr & common_data_ptr);

/**
 * @brief Calculates the distance from the ego vehicle to the start of the target lanes.
 *
 * This function computes the shortest distance from the current position of the ego vehicle
 * to the start of the target lanes by measuring the arc length to the front points of
 * the left and right boundaries of the target lane. If the target lanes are empty or other
 * required data is unavailable, the function returns numeric_limits<double>::max() preventing lane
 * change being executed.
 *
 * @param common_data_ptr Shared pointer to a CommonData structure, which should include:
 *  - `route_handler_ptr`: Pointer to the route handler that manages the route.
 *  - Other necessary parameters for ego vehicle positioning.
 * @param current_lanes The set of lanelets representing the current lanes of the ego vehicle.
 * @param target_lanes The set of lanelets representing the target lanes for lane changing.
 *
 * @return The distance from the ego vehicle to the start of the target lanes in meters,
 * or numeric_limits<double>::max() if the target lanes are empty or data is unavailable.
 */
double calc_ego_dist_to_lanes_start(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & current_lanes,
  const lanelet::ConstLanelets & target_lanes);

double calc_lane_changing_acceleration(
  const CommonDataPtr & common_data_ptr, const double initial_lane_changing_velocity,
  const double max_path_velocity, const double lane_changing_time,
  const double prepare_longitudinal_acc);

/**
 * @brief Calculates the distance required during a lane change operation.
 *
 * Used for computing prepare or lane change length based on current and maximum velocity,
 * acceleration, and duration, returning the lesser of accelerated distance or distance at max
 * velocity.
 *
 * @param velocity The current velocity of the vehicle in meters per second (m/s).
 * @param maximum_velocity The maximum velocity the vehicle can reach in meters per second (m/s).
 * @param acceleration The acceleration of the vehicle in meters per second squared (m/s^2).
 * @param duration The duration of the lane change in seconds (s).
 * @return The calculated minimum distance in meters (m).
 */
double calc_phase_length(
  const double velocity, const double maximum_velocity, const double acceleration,
  const double duration);

std::vector<double> calc_lon_acceleration_samples(
  const CommonDataPtr & common_data_ptr, const double max_path_velocity,
  const double prepare_duration);

/**
 * @brief calculates the actual prepare duration that will be used for path generation
 *
 * @details computes the required prepare duration to be used for candidate path
 * generation based on the elapsed time of turn signal activation, the minimum
 * and maximum parameterized values for the prepare duration,
 * and the minimum lane changing velocity.
 *
 * @param common_data_ptr Shared pointer to a CommonData structure, which includes
 * lane change parameters and bpp common parameters.
 * @param current_velocity current ego vehicle velocity.
 * @param active_signal_duration elapsed time since turn signal activation.
 * @return The calculated prepare duration value in seconds (s)
 */
double calc_actual_prepare_duration(
  const CommonDataPtr & common_data_ptr, const double current_velocity,
  const double active_signal_duration);

std::vector<PhaseMetrics> calc_prepare_phase_metrics(
  const CommonDataPtr & common_data_ptr, const double current_velocity,
  const double max_path_velocity, const double min_length_threshold = 0.0,
  const double max_length_threshold = std::numeric_limits<double>::max());

std::vector<PhaseMetrics> calc_shift_phase_metrics(
  const CommonDataPtr & common_data_ptr, const double shift_length, const double initial_velocity,
  const double max_path_velocity, const double lon_accel,
  const double max_length_threshold = std::numeric_limits<double>::max());

/**
 * @brief Calculates the minimum and maximum lane changing lengths, along with the corresponding
 * distance buffers.
 *
 * This function computes the minimum and maximum lane change lengths based on shift intervals and
 * vehicle parameters. It only accounts for the lane changing phase itself, excluding the prepare
 * distance, which should be handled separately during path generation.
 *
 * Additionally, the function calculates the distance buffer to ensure that the ego vehicle has
 * enough space to complete the lane change, including cases where multiple lane changes may be
 * necessary.
 *
 * @param common_data_ptr Shared pointer to a CommonData structure, which includes:
 *  - `lc_param_ptr`: Parameters related to lane changing, such as velocity, lateral acceleration,
 * and jerk.
 *  - `route_handler_ptr`: Pointer to the route handler for managing routes.
 *  - `direction`: Lane change direction (e.g., left or right).
 *  - `transient_data.acc.max`: Maximum acceleration of the vehicle.
 *  - Other relevant data for the ego vehicle's dynamics and state.
 * @param lanes The set of lanelets representing the lanes for the lane change.
 *
 * @return A pair of `MinMaxValue` structures where:
 *  - The first element contains the minimum and maximum lane changing lengths in meters.
 *  - The second element contains the minimum and maximum distance buffers in meters.
 * If the lanes or necessary data are unavailable, returns `std::numeric_limits<double>::max()` for
 * both values.
 */
std::pair<MinMaxValue, MinMaxValue> calc_lc_length_and_dist_buffer(
  const CommonDataPtr & common_data_ptr, const lanelet::ConstLanelets & lanes);
}  // namespace autoware::behavior_path_planner::utils::lane_change::calculation

#endif  // AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__UTILS__CALCULATION_HPP_

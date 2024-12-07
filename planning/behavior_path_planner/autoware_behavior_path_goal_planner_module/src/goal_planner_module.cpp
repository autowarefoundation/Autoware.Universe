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

#include "autoware/behavior_path_goal_planner_module/goal_planner_module.hpp"

#include "autoware/behavior_path_goal_planner_module/default_fixed_goal_planner.hpp"
#include "autoware/behavior_path_goal_planner_module/pull_over_planner/freespace_pull_over.hpp"
#include "autoware/behavior_path_goal_planner_module/pull_over_planner/geometric_pull_over.hpp"
#include "autoware/behavior_path_goal_planner_module/pull_over_planner/pull_over_planner_base.hpp"
#include "autoware/behavior_path_goal_planner_module/pull_over_planner/shift_pull_over.hpp"
#include "autoware/behavior_path_goal_planner_module/util.hpp"
#include "autoware/behavior_path_planner_common/utils/drivable_area_expansion/static_drivable_area.hpp"
#include "autoware/behavior_path_planner_common/utils/parking_departure/geometric_parallel_parking.hpp"
#include "autoware/behavior_path_planner_common/utils/parking_departure/utils.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/objects_filtering.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/safety_check.hpp"
#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"
#include "autoware/behavior_path_planner_common/utils/utils.hpp"
#include "autoware/universe_utils/geometry/boost_polygon_utils.hpp"

#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <magic_enum.hpp>
#include <rclcpp/rclcpp.hpp>

#include <algorithm>
#include <cstddef>
#include <deque>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

using autoware::behavior_path_planner::utils::parking_departure::calcFeasibleDecelDistance;
using autoware::motion_utils::calcLongitudinalOffsetPose;
using autoware::motion_utils::calcSignedArcLength;
using autoware::motion_utils::findFirstNearestSegmentIndexWithSoftConstraints;
using autoware::motion_utils::insertDecelPoint;
using autoware::universe_utils::calcDistance2d;
using autoware::universe_utils::calcOffsetPose;
using autoware::universe_utils::createMarkerColor;
using nav_msgs::msg::OccupancyGrid;

namespace autoware::behavior_path_planner
{
GoalPlannerModule::GoalPlannerModule(
  const std::string & name, rclcpp::Node & node,
  const std::shared_ptr<GoalPlannerParameters> & parameters,
  const std::unordered_map<std::string, std::shared_ptr<RTCInterface>> & rtc_interface_ptr_map,
  std::unordered_map<std::string, std::shared_ptr<ObjectsOfInterestMarkerInterface>> &
    objects_of_interest_marker_interface_ptr_map)
: SceneModuleInterface{name, node, rtc_interface_ptr_map, objects_of_interest_marker_interface_ptr_map},  // NOLINT
  parameters_{parameters},
  vehicle_info_{autoware::vehicle_info_utils::VehicleInfoUtils(node).getVehicleInfo()},
  thread_safe_data_{mutex_, clock_},
  is_lane_parking_cb_running_{false},
  is_freespace_parking_cb_running_{false},
  debug_stop_pose_with_info_{&stop_pose_}
{
  LaneDepartureChecker lane_departure_checker{};
  lane_departure_checker.setVehicleInfo(vehicle_info_);
  lane_departure_checker::Param lane_departure_checker_params;
  lane_departure_checker_params.footprint_extra_margin =
    parameters->lane_departure_check_expansion_margin;
  lane_departure_checker.setParam(lane_departure_checker_params);

  occupancy_grid_map_ = std::make_shared<OccupancyGridBasedCollisionDetector>();

  left_side_parking_ = parameters_->parking_policy == ParkingPolicy::LEFT_SIDE;

  // planner when goal modification is not allowed
  fixed_goal_planner_ = std::make_unique<DefaultFixedGoalPlanner>();

  for (const std::string & planner_type : parameters_->efficient_path_order) {
    if (planner_type == "SHIFT" && parameters_->enable_shift_parking) {
      pull_over_planners_.push_back(
        std::make_shared<ShiftPullOver>(node, *parameters, lane_departure_checker));
    } else if (planner_type == "ARC_FORWARD" && parameters_->enable_arc_forward_parking) {
      pull_over_planners_.push_back(std::make_shared<GeometricPullOver>(
        node, *parameters, lane_departure_checker, /*is_forward*/ true));
    } else if (planner_type == "ARC_BACKWARD" && parameters_->enable_arc_backward_parking) {
      pull_over_planners_.push_back(std::make_shared<GeometricPullOver>(
        node, *parameters, lane_departure_checker, /*is_forward*/ false));
    }
  }

  if (pull_over_planners_.empty()) {
    RCLCPP_WARN(
      getLogger(),
      "No enabled planner found. The vehicle will stop in the road lane without pull over. Please "
      "check the parameters if this is the intended behavior.");
  }

  // set selected goal searcher
  // currently there is only one goal_searcher_type
  const auto vehicle_info = autoware::vehicle_info_utils::VehicleInfoUtils(node).getVehicleInfo();
  vehicle_footprint_ = vehicle_info.createFootprint();
  goal_searcher_ = std::make_shared<GoalSearcher>(*parameters, vehicle_footprint_);

  // timer callback for generating lane parking candidate paths
  const auto lane_parking_period_ns = rclcpp::Rate(1.0).period();
  lane_parking_timer_cb_group_ =
    node.create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  lane_parking_timer_ = rclcpp::create_timer(
    &node, clock_, lane_parking_period_ns, std::bind(&GoalPlannerModule::onTimer, this),
    lane_parking_timer_cb_group_);

  // freespace parking
  if (parameters_->enable_freespace_parking) {
    freespace_planner_ = std::make_unique<FreespacePullOver>(node, *parameters, vehicle_info);
    const auto freespace_parking_period_ns = rclcpp::Rate(1.0).period();
    freespace_parking_timer_cb_group_ =
      node.create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    freespace_parking_timer_ = rclcpp::create_timer(
      &node, clock_, freespace_parking_period_ns,
      std::bind(&GoalPlannerModule::onFreespaceParkingTimer, this),
      freespace_parking_timer_cb_group_);
  }

  // Initialize safety checker
  if (parameters_->safety_check_params.enable_safety_check) {
    initializeSafetyCheckParameters();
  }

  steering_factor_interface_.init(PlanningBehavior::GOAL_PLANNER);
  velocity_factor_interface_.init(PlanningBehavior::GOAL_PLANNER);

  /**
   * NOTE: Add `universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);` to functions called
   * from the main thread only.
   * If you want to see the processing time tree in console, uncomment the following line
   */
  // time_keeper_->add_reporter(&std::cerr);
}

bool isOnModifiedGoal(
  const Pose & current_pose, const GoalCandidate & modified_goal,
  const GoalPlannerParameters & parameters)
{
  return calcDistance2d(current_pose, modified_goal.goal_pose) < parameters.th_arrived_distance;
}

bool isOnModifiedGoal(
  const Pose & current_pose, const std::optional<GoalCandidate> & modified_goal_opt,
  const GoalPlannerParameters & parameters)
{
  if (!modified_goal_opt) {
    return false;
  }

  return isOnModifiedGoal(current_pose, modified_goal_opt.value(), parameters);
}

bool hasPreviousModulePathShapeChanged(
  const BehaviorModuleOutput & previous_module_output,
  const BehaviorModuleOutput & last_previous_module_output)
{
  // Calculate the lateral distance between each point of the current path and the nearest point of
  // the last path
  constexpr double LATERAL_DEVIATION_THRESH = 0.3;
  for (const auto & p : previous_module_output.path.points) {
    const size_t nearest_seg_idx = autoware::motion_utils::findNearestSegmentIndex(
      last_previous_module_output.path.points, p.point.pose.position);
    const auto seg_front = last_previous_module_output.path.points.at(nearest_seg_idx);
    const auto seg_back = last_previous_module_output.path.points.at(nearest_seg_idx + 1);
    // Check if the target point is within the segment
    const Eigen::Vector3d segment_vec{
      seg_back.point.pose.position.x - seg_front.point.pose.position.x,
      seg_back.point.pose.position.y - seg_front.point.pose.position.y, 0.0};
    const Eigen::Vector3d target_vec{
      p.point.pose.position.x - seg_front.point.pose.position.x,
      p.point.pose.position.y - seg_front.point.pose.position.y, 0.0};
    const double dot_product = segment_vec.x() * target_vec.x() + segment_vec.y() * target_vec.y();
    const double segment_length_squared =
      segment_vec.x() * segment_vec.x() + segment_vec.y() * segment_vec.y();
    if (dot_product < 0 || dot_product > segment_length_squared) {
      // p.point.pose.position is not within the segment, skip lateral distance check
      continue;
    }
    const double lateral_distance = std::abs(autoware::motion_utils::calcLateralOffset(
      last_previous_module_output.path.points, p.point.pose.position, nearest_seg_idx));
    if (lateral_distance > LATERAL_DEVIATION_THRESH) {
      return true;
    }
  }
  return false;
}

bool hasDeviatedFromLastPreviousModulePath(
  const PlannerData & planner_data, const BehaviorModuleOutput & last_previous_module_output)
{
  return std::abs(autoware::motion_utils::calcLateralOffset(
           last_previous_module_output.path.points,
           planner_data.self_odometry->pose.pose.position)) > 0.3;
}

bool hasDeviatedFromCurrentPreviousModulePath(
  const PlannerData & planner_data, const BehaviorModuleOutput & previous_module_output)
{
  constexpr double LATERAL_DEVIATION_THRESH = 0.3;
  return std::abs(autoware::motion_utils::calcLateralOffset(
           previous_module_output.path.points, planner_data.self_odometry->pose.pose.position)) >
         LATERAL_DEVIATION_THRESH;
}

bool needPathUpdate(
  const Pose & current_pose, const double path_update_duration, const rclcpp::Time & now,
  const std::optional<GoalCandidate> & modified_goal,
  const std::optional<rclcpp::Time> & selected_time, const GoalPlannerParameters & parameters)
{
  const bool has_enough_time_passed =
    selected_time ? (now - selected_time.value()).seconds() > path_update_duration : true;
  return !isOnModifiedGoal(current_pose, modified_goal, parameters) && has_enough_time_passed;
}

bool checkOccupancyGridCollision(
  const PathWithLaneId & path,
  const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map)
{
  if (!occupancy_grid_map) {
    return false;
  }
  const bool check_out_of_range = false;
  return occupancy_grid_map->hasObstacleOnPath(path, check_out_of_range);
}

bool isStopped(
  std::deque<nav_msgs::msg::Odometry::ConstSharedPtr> & odometry_buffer,
  const nav_msgs::msg::Odometry::ConstSharedPtr self_odometry, const double duration_lower,
  const double velocity_upper)
{
  odometry_buffer.push_back(self_odometry);
  // Delete old data in buffer_stuck_
  while (rclcpp::ok()) {
    const auto time_diff = rclcpp::Time(odometry_buffer.back()->header.stamp) -
                           rclcpp::Time(odometry_buffer.front()->header.stamp);
    if (time_diff.seconds() < duration_lower) {
      break;
    }
    odometry_buffer.pop_front();
  }
  bool is_stopped = true;
  for (const auto & odometry : odometry_buffer) {
    const double ego_vel = utils::l2Norm(odometry->twist.twist.linear);
    if (ego_vel > velocity_upper) {
      is_stopped = false;
      break;
    }
  }
  return is_stopped;
}

// generate pull over candidate paths
void GoalPlannerModule::onTimer()
{
  const ScopedFlag flag(is_lane_parking_cb_running_);

  std::shared_ptr<const PlannerData> local_planner_data{nullptr};
  std::optional<ModuleStatus> current_status_opt{std::nullopt};
  std::optional<BehaviorModuleOutput> previous_module_output_opt{std::nullopt};
  std::optional<BehaviorModuleOutput> last_previous_module_output_opt{std::nullopt};
  std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map{nullptr};
  std::optional<GoalPlannerParameters> parameters_opt{std::nullopt};
  std::optional<GoalCandidates> goal_candidates_opt{std::nullopt};

  // begin of critical section
  {
    std::lock_guard<std::mutex> guard(gp_planner_data_mutex_);
    if (gp_planner_data_) {
      const auto & gp_planner_data = gp_planner_data_.value();
      local_planner_data = std::make_shared<const PlannerData>(gp_planner_data.planner_data);
      current_status_opt = gp_planner_data.current_status;
      previous_module_output_opt = gp_planner_data.previous_module_output;
      last_previous_module_output_opt = gp_planner_data.last_previous_module_output;
      occupancy_grid_map = gp_planner_data.occupancy_grid_map;
      parameters_opt = gp_planner_data.parameters;
      goal_candidates_opt = gp_planner_data.goal_candidates;
    }
  }
  // end of critical section
  if (
    !local_planner_data || !current_status_opt || !previous_module_output_opt ||
    !last_previous_module_output_opt || !occupancy_grid_map || !parameters_opt ||
    !goal_candidates_opt) {
    RCLCPP_INFO_THROTTLE(
      getLogger(), *clock_, 5000,
      "failed to get valid "
      "local_planner_data/current_status/previous_module_output/occupancy_grid_map/parameters_opt "
      "in onTimer");
    return;
  }
  const auto & current_status = current_status_opt.value();
  const auto & previous_module_output = previous_module_output_opt.value();
  const auto & last_previous_module_output = last_previous_module_output_opt.value();
  const auto & parameters = parameters_opt.value();
  const auto & goal_candidates = goal_candidates_opt.value();

  if (current_status == ModuleStatus::IDLE) {
    return;
  }

  // goals are not yet available.
  if (goal_candidates.empty()) {
    return;
  }

  if (!utils::isAllowedGoalModification(local_planner_data->route_handler)) {
    return;
  }

  // check if new pull over path candidates are needed to be generated
  const auto current_state = thread_safe_data_.get_prev_data().state;
  const bool need_update = std::invoke([&]() {
    const bool found_pull_over_path = thread_safe_data_.foundPullOverPath();
    const std::optional<PullOverPath> pull_over_path_opt =
      found_pull_over_path
        ? std::make_optional<PullOverPath>(*thread_safe_data_.get_pull_over_path())
        : std::nullopt;
    const std::optional<GoalCandidate> modified_goal_opt =
      pull_over_path_opt
        ? std::make_optional<GoalCandidate>(pull_over_path_opt.value().modified_goal())
        : std::nullopt;
    if (isOnModifiedGoal(
          local_planner_data->self_odometry->pose.pose, modified_goal_opt, parameters)) {
      return false;
    }
    if (hasDeviatedFromCurrentPreviousModulePath(*local_planner_data, previous_module_output)) {
      RCLCPP_DEBUG(getLogger(), "has deviated from current previous module path");
      return false;
    }
    if (thread_safe_data_.get_pull_over_path_candidates().empty()) {
      return true;
    }
    if (hasPreviousModulePathShapeChanged(previous_module_output, last_previous_module_output)) {
      RCLCPP_DEBUG(getLogger(), "has previous module path shape changed");
      return true;
    }
    if (
      hasDeviatedFromLastPreviousModulePath(*local_planner_data, last_previous_module_output) &&
      current_state != PathDecisionState::DecisionKind::DECIDED) {
      RCLCPP_DEBUG(getLogger(), "has deviated from last previous module path");
      return true;
    }
    // TODO(someone): The generated path inherits the velocity of the path of the previous module.
    // Therefore, if the velocity of the path of the previous module changes (e.g. stop points are
    // inserted, deleted), the path should be regenerated.

    return false;
  });
  if (!need_update) {
    return;
  }

  // generate valid pull over path candidates and calculate closest start pose
  const auto current_lanes = utils::getExtendedCurrentLanes(
    local_planner_data, parameters.backward_goal_search_length,
    parameters.forward_goal_search_length,
    /*forward_only_in_route*/ false);
  std::vector<PullOverPath> path_candidates{};
  std::optional<Pose> closest_start_pose{};
  double min_start_arc_length = std::numeric_limits<double>::max();
  const auto planCandidatePaths = [&](
                                    const std::shared_ptr<PullOverPlannerBase> & planner,
                                    const GoalCandidate & goal_candidate) {
    const auto pull_over_path = planner->plan(
      goal_candidate, path_candidates.size(), local_planner_data, previous_module_output);
    if (pull_over_path) {
      // calculate absolute maximum curvature of parking path(start pose to end pose) for path
      // priority
      path_candidates.push_back(*pull_over_path);
      // calculate closest pull over start pose for stop path
      const double start_arc_length =
        lanelet::utils::getArcCoordinates(current_lanes, pull_over_path->start_pose()).length;
      if (start_arc_length < min_start_arc_length) {
        min_start_arc_length = start_arc_length;
        // closest start pose is stop point when not finding safe path
        closest_start_pose = pull_over_path->start_pose();
      }
    }
  };

  // todo: currently non centerline input path is supported only by shift pull over
  const bool is_center_line_input_path = goal_planner_utils::isReferencePath(
    previous_module_output.reference_path, previous_module_output.path, 0.1);
  RCLCPP_DEBUG(
    getLogger(), "the input path of pull over planner is center line: %d",
    is_center_line_input_path);

  // plan candidate paths and set them to the member variable
  if (parameters.path_priority == "efficient_path") {
    for (const auto & planner : pull_over_planners_) {
      // todo: temporary skip NON SHIFT planner when input path is not center line
      if (!is_center_line_input_path && planner->getPlannerType() != PullOverPlannerType::SHIFT) {
        continue;
      }
      for (const auto & goal_candidate : goal_candidates) {
        planCandidatePaths(planner, goal_candidate);
      }
    }
  } else if (parameters.path_priority == "close_goal") {
    for (const auto & goal_candidate : goal_candidates) {
      for (const auto & planner : pull_over_planners_) {
        // todo: temporary skip NON SHIFT planner when input path is not center line
        if (!is_center_line_input_path && planner->getPlannerType() != PullOverPlannerType::SHIFT) {
          continue;
        }
        planCandidatePaths(planner, goal_candidate);
      }
    }
  } else {
    RCLCPP_ERROR(
      getLogger(), "path_priority should be efficient_path or close_goal, but %s is given.",
      parameters.path_priority.c_str());
    throw std::domain_error("[pull_over] invalid path_priority");
  }

  // set member variables
  thread_safe_data_.set_pull_over_path_candidates(path_candidates);
  thread_safe_data_.set_closest_start_pose(closest_start_pose);
  RCLCPP_INFO(getLogger(), "generated %lu pull over path candidates", path_candidates.size());
}

void GoalPlannerModule::onFreespaceParkingTimer()
{
  const ScopedFlag flag(is_freespace_parking_cb_running_);

  std::shared_ptr<const PlannerData> local_planner_data{nullptr};
  std::optional<ModuleStatus> current_status_opt{std::nullopt};
  std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map{nullptr};
  std::optional<GoalPlannerParameters> parameters_opt{std::nullopt};
  std::optional<autoware::universe_utils::LinearRing2d> vehicle_footprint_opt{std::nullopt};
  std::optional<GoalCandidates> goal_candidates_opt{std::nullopt};

  // begin of critical section
  {
    std::lock_guard<std::mutex> guard(gp_planner_data_mutex_);
    if (gp_planner_data_) {
      const auto & gp_planner_data = gp_planner_data_.value();
      local_planner_data = std::make_shared<const PlannerData>(gp_planner_data.planner_data);
      current_status_opt = gp_planner_data.current_status;
      occupancy_grid_map = gp_planner_data.occupancy_grid_map;
      parameters_opt = gp_planner_data.parameters;
      vehicle_footprint_opt = gp_planner_data.vehicle_footprint;
      goal_candidates_opt = gp_planner_data.goal_candidates;
    }
  }
  // end of critical section
  if (
    !local_planner_data || !current_status_opt || !occupancy_grid_map || !parameters_opt ||
    !vehicle_footprint_opt || !goal_candidates_opt) {
    RCLCPP_WARN_THROTTLE(
      getLogger(), *clock_, 5000,
      "failed to get valid planner_data/current_status/parameters in "
      "onFreespaceParkingTimer");
    return;
  }

  const auto & current_status = current_status_opt.value();
  const auto & parameters = parameters_opt.value();
  const auto & vehicle_footprint = vehicle_footprint_opt.value();
  const auto & goal_candidates = goal_candidates_opt.value();

  if (current_status == ModuleStatus::IDLE) {
    return;
  }

  if (!local_planner_data->costmap) {
    return;
  }
  // fixed goal planner do not use freespace planner
  if (!utils::isAllowedGoalModification(local_planner_data->route_handler)) {
    return;
  }

  const bool found_pull_over_path = thread_safe_data_.foundPullOverPath();
  const std::optional<PullOverPath> pull_over_path_opt =
    found_pull_over_path ? std::make_optional<PullOverPath>(*thread_safe_data_.get_pull_over_path())
                         : std::nullopt;
  const std::optional<GoalCandidate> modified_goal_opt =
    pull_over_path_opt
      ? std::make_optional<GoalCandidate>(pull_over_path_opt.value().modified_goal())
      : std::nullopt;
  if (isOnModifiedGoal(
        local_planner_data->self_odometry->pose.pose, modified_goal_opt, parameters)) {
    return;
  }

  const double vehicle_width = planner_data_->parameters.vehicle_width;
  const bool left_side_parking = parameters.parking_policy == ParkingPolicy::LEFT_SIDE;
  const auto pull_over_lanes = goal_planner_utils::getPullOverLanes(
    *(local_planner_data->route_handler), left_side_parking, parameters.backward_goal_search_length,
    parameters.forward_goal_search_length);
  const auto objects_extraction_polygon = goal_planner_utils::generateObjectExtractionPolygon(
    pull_over_lanes, left_side_parking, parameters.detection_bound_offset,
    parameters.margin_from_boundary + parameters.max_lateral_offset + vehicle_width);

  PredictedObjects dynamic_target_objects{};
  for (const auto & object : local_planner_data->dynamic_object->objects) {
    const auto object_polygon = universe_utils::toPolygon2d(object);
    if (
      objects_extraction_polygon.has_value() &&
      boost::geometry::intersects(object_polygon, objects_extraction_polygon.value())) {
      dynamic_target_objects.objects.push_back(object);
    }
  }
  const auto static_target_objects = utils::path_safety_checker::filterObjectsByVelocity(
    dynamic_target_objects, parameters.th_moving_object_velocity);

  const bool is_new_costmap =
    (clock_->now() - local_planner_data->costmap->header.stamp).seconds() < 1.0;
  constexpr double path_update_duration = 1.0;
  if (
    isStuck(
      static_target_objects, dynamic_target_objects, local_planner_data, occupancy_grid_map,
      parameters) &&
    is_new_costmap &&
    needPathUpdate(
      local_planner_data->self_odometry->pose.pose, path_update_duration, clock_->now(),
      modified_goal_opt, thread_safe_data_.get_last_path_update_time(), parameters)) {
    auto goal_searcher = std::make_shared<GoalSearcher>(parameters, vehicle_footprint);

    planFreespacePath(
      local_planner_data, goal_searcher, goal_candidates, occupancy_grid_map,
      static_target_objects);
  }
}

void GoalPlannerModule::updateData()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  // extract static and dynamic objects in extraction polygon for path collision check
  const auto & p = parameters_;
  const auto & rh = *(planner_data_->route_handler);
  const auto objects = *(planner_data_->dynamic_object);
  const double vehicle_width = planner_data_->parameters.vehicle_width;
  const auto pull_over_lanes = goal_planner_utils::getPullOverLanes(
    rh, left_side_parking_, p->backward_goal_search_length, p->forward_goal_search_length);
  const auto objects_extraction_polygon = goal_planner_utils::generateObjectExtractionPolygon(
    pull_over_lanes, left_side_parking_, p->detection_bound_offset,
    p->margin_from_boundary + p->max_lateral_offset + vehicle_width);
  if (objects_extraction_polygon.has_value()) {
    debug_data_.objects_extraction_polygon = objects_extraction_polygon.value();
  }
  PredictedObjects dynamic_target_objects{};
  for (const auto & object : objects.objects) {
    const auto object_polygon = universe_utils::toPolygon2d(object);
    if (
      objects_extraction_polygon.has_value() &&
      boost::geometry::intersects(object_polygon, objects_extraction_polygon.value())) {
      dynamic_target_objects.objects.push_back(object);
    }
  }
  const auto static_target_objects = utils::path_safety_checker::filterObjectsByVelocity(
    dynamic_target_objects, p->th_moving_object_velocity);

  // update goal searcher and generate goal candidates
  if (goal_candidates_.empty()) {
    goal_candidates_ = generateGoalCandidates();
  }

  // In PlannerManager::run(), it calls SceneModuleInterface::setData and
  // SceneModuleInterface::setPreviousModuleOutput before module_ptr->run().
  // Then module_ptr->run() invokes GoalPlannerModule::updateData and then
  // planWaitingApproval()/plan(), so we can copy latest current_status/previous_module_output to
  // gp_planner_data_ here

  // NOTE: onTimer/onFreespaceParkingTimer copies gp_planner_data_ to their local clone, so we need
  // to lock gp_planner_data_ here to avoid data race. But the following clone process is
  // lightweight because most of the member variables of PlannerData/RouteHandler is
  // shared_ptrs/bool
  // begin of critical section
  {
    std::lock_guard<std::mutex> guard(gp_planner_data_mutex_);
    if (!gp_planner_data_) {
      gp_planner_data_ = GoalPlannerData(*planner_data_, *parameters_, getPreviousModuleOutput());
    }
    auto & gp_planner_data = gp_planner_data_.value();
    // NOTE: for the above reasons, PlannerManager/behavior_path_planner_node ensure that
    // planner_data_ is not nullptr, so it is OK to copy as value
    // By copying PlannerData as value, the internal shared member variables are also copied
    // (reference count is incremented), so `gp_planner_data_.foo` is now thread-safe from the
    // **re-pointing** by `planner_data_->foo = msg` in behavior_path_planner::onCallbackFor(msg)
    // and if these two coincided, only the reference count is affected
    gp_planner_data.update(
      *parameters_, *planner_data_, getCurrentStatus(), getPreviousModuleOutput(),
      vehicle_footprint_, goal_candidates_);
    // NOTE: RouteHandler holds several shared pointers in it, so just copying PlannerData as
    // value does not adds the reference counts of RouteHandler.lanelet_map_ptr_ and others. Since
    // behavior_path_planner::run() updates
    // planner_data_->route_handler->lanelet_map_ptr_/routing_graph_ptr_ especially, we also have
    // to copy route_handler as value to use lanelet_map_ptr_/routing_graph_ptr_ thread-safely in
    // onTimer/onFreespaceParkingTimer
    // TODO(Mamoru Sobue): If the copy of RouteHandler.road_lanelets/shoulder_lanelets is not
    // lightweight, we should update gp_planner_data_.route_handler only when
    // `planner_data_.is_route_handler_updated` variable is set true by behavior_path_planner
    // (although this flag is not implemented yet). In that case, gp_planner_data members except
    // for route_handler should be copied from planner_data_

    // GoalPlannerModule::occupancy_grid_map_ and gp_planner_data.occupancy_grid_map share the
    // ownership, and gp_planner_data.occupancy_grid_map maybe also shared by the local
    // planner_data on onFreespaceParkingTimer thread local memory space. So following operation
    // is thread-safe because gp_planner_data.occupancy_grid_map is only re-pointed here and its
    // prior resource is still owned by the onFreespaceParkingTimer thread locally.
    occupancy_grid_map_ = gp_planner_data.occupancy_grid_map;
  }
  // end of critical section

  if (getCurrentStatus() == ModuleStatus::IDLE && !isExecutionRequested()) {
    return;
  }

  resetPathCandidate();
  resetPathReference();
  path_reference_ = std::make_shared<PathWithLaneId>(getPreviousModuleOutput().reference_path);

  const bool found_pull_over_path = thread_safe_data_.foundPullOverPath();
  std::optional<PullOverPath> pull_over_path_recv =
    found_pull_over_path ? std::make_optional<PullOverPath>(*thread_safe_data_.get_pull_over_path())
                         : std::nullopt;

  const auto modified_goal_pose = [&]() -> std::optional<GoalCandidate> {
    if (!pull_over_path_recv) {
      return std::nullopt;
    }
    const auto & pull_over_path = pull_over_path_recv.value();
    return pull_over_path.modified_goal();
  }();

  // save "old" state
  const auto prev_decision_state = path_decision_controller_.get_current_state();
  const auto [is_current_safe, collision_check_map] = isSafePath(
    planner_data_, found_pull_over_path, pull_over_path_recv, prev_decision_state, *parameters_,
    ego_predicted_path_params_, objects_filtering_params_, safety_check_params_);
  debug_data_.collision_check = collision_check_map;
  // update to latest state
  path_decision_controller_.transit_state(
    found_pull_over_path, clock_->now(), static_target_objects, dynamic_target_objects,
    modified_goal_pose, planner_data_, occupancy_grid_map_, is_current_safe, *parameters_,
    goal_searcher_, isActivated(), pull_over_path_recv, debug_data_.ego_polygons_expanded);

  context_data_.emplace(
    path_decision_controller_.get_current_state().is_stable_safe, static_target_objects,
    dynamic_target_objects, std::move(pull_over_path_recv),
    thread_safe_data_.get_pull_over_path_candidates(), prev_decision_state);
  auto & ctx_data = context_data_.value();

  thread_safe_data_.set_prev_data(path_decision_controller_.get_current_state());

  if (!isActivated()) {
    return;
  }

  if (hasFinishedCurrentPath(ctx_data)) {
    if (thread_safe_data_.incrementPathIndex()) {
      ctx_data.last_path_idx_increment_time = clock_->now();
    }
  }

  if (!last_approval_data_) {
    last_approval_data_ =
      std::make_unique<LastApprovalData>(clock_->now(), planner_data_->self_odometry->pose.pose);
    // TODO(soblin): do not "plan" in updateData
    decideVelocity();
  }
}

void GoalPlannerModule::initializeSafetyCheckParameters()
{
  ego_predicted_path_params_ =
    std::make_shared<EgoPredictedPathParams>(parameters_->ego_predicted_path_params);
  safety_check_params_ = std::make_shared<SafetyCheckParams>(parameters_->safety_check_params);
  objects_filtering_params_ =
    std::make_shared<ObjectsFilteringParams>(parameters_->objects_filtering_params);
}

void GoalPlannerModule::processOnExit()
{
  resetPathCandidate();
  resetPathReference();
  debug_marker_.markers.clear();
  thread_safe_data_.reset();
  last_approval_data_.reset();
}

bool GoalPlannerModule::isExecutionRequested() const
{
  if (getCurrentStatus() == ModuleStatus::RUNNING) {
    return true;
  }

  // TODO(someone): if goal is behind of ego, do not execute goal_planner

  const auto & route_handler = planner_data_->route_handler;
  const Pose & current_pose = planner_data_->self_odometry->pose.pose;
  const Pose goal_pose = route_handler->getOriginalGoalPose();

  // check if goal_pose is in current_lanes or neighboring road lanes
  const lanelet::ConstLanelets current_lanes =
    utils::getCurrentLanesFromPath(getPreviousModuleOutput().reference_path, planner_data_);
  const auto getNeighboringLane =
    [&](const lanelet::ConstLanelet & lane) -> std::optional<lanelet::ConstLanelet> {
    return left_side_parking_ ? route_handler->getLeftLanelet(lane, false, true)
                              : route_handler->getRightLanelet(lane, false, true);
  };
  lanelet::ConstLanelets goal_check_lanes = current_lanes;
  for (const auto & lane : current_lanes) {
    auto neighboring_lane = getNeighboringLane(lane);
    while (neighboring_lane) {
      goal_check_lanes.push_back(neighboring_lane.value());
      neighboring_lane = getNeighboringLane(neighboring_lane.value());
    }
  }
  const bool goal_is_in_current_segment_lanes = std::any_of(
    goal_check_lanes.begin(), goal_check_lanes.end(), [&](const lanelet::ConstLanelet & lane) {
      return lanelet::utils::isInLanelet(goal_pose, lane);
    });

  // check that goal is in current neighbor shoulder lane
  const bool goal_is_in_current_shoulder_lanes = std::invoke([&]() {
    for (const auto & lane : current_lanes) {
      const auto shoulder_lane = left_side_parking_ ? route_handler->getLeftShoulderLanelet(lane)
                                                    : route_handler->getRightShoulderLanelet(lane);
      if (shoulder_lane && lanelet::utils::isInLanelet(goal_pose, *shoulder_lane)) {
        return true;
      }
    }
    return false;
  });

  // if goal is not in current_lanes and current_shoulder_lanes, do not execute goal_planner,
  // because goal arc coordinates cannot be calculated.
  if (!goal_is_in_current_segment_lanes && !goal_is_in_current_shoulder_lanes) {
    return false;
  }

  // if goal modification is not allowed
  // 1) goal_pose is in current_lanes, plan path to the original fixed goal
  // 2) goal_pose is NOT in current_lanes, do not execute goal_planner
  if (!utils::isAllowedGoalModification(route_handler)) {
    return goal_is_in_current_segment_lanes;
  }

  // if goal arc coordinates can be calculated, check if goal is in request_length
  const double self_to_goal_arc_length =
    utils::getSignedDistance(current_pose, goal_pose, current_lanes);
  const double request_length = utils::isAllowedGoalModification(route_handler)
                                  ? calcModuleRequestLength()
                                  : parameters_->pull_over_minimum_request_length;
  if (self_to_goal_arc_length < 0.0 || self_to_goal_arc_length > request_length) {
    // if current position is far from goal or behind goal, do not execute goal_planner
    return false;
  }

  // if (A) or (B) is met execute pull over
  // (A) target lane is `road` and same to the current lanes
  // (B) target lane is `road_shoulder` and neighboring to the current lanes
  const lanelet::ConstLanelets pull_over_lanes = goal_planner_utils::getPullOverLanes(
    *(route_handler), left_side_parking_, parameters_->backward_goal_search_length,
    parameters_->forward_goal_search_length);
  lanelet::ConstLanelet target_lane{};
  lanelet::utils::query::getClosestLanelet(pull_over_lanes, goal_pose, &target_lane);

  // Get the lanelet of the ego vehicle's side edge at the terminal pose of the previous module's
  // path. Check if the lane is the target lane or the neighboring lane. NOTE: This is because in
  // the case of avoidance, there is a possibility of base_link entering the neighboring lane, and
  // we want to activate the pull over at this time as well.
  const Pose previous_terminal_pose = getPreviousModuleOutput().path.points.back().point.pose;
  const double vehicle_half_width = planner_data_->parameters.vehicle_width / 2.0;
  const Pose previous_terminal_vehicle_edge_pose = calcOffsetPose(
    previous_terminal_pose, 0, left_side_parking_ ? vehicle_half_width : -vehicle_half_width, 0);
  lanelet::ConstLanelet previous_module_terminal_vehicle_edge_lane{};
  route_handler->getClosestLaneletWithinRoute(
    previous_terminal_vehicle_edge_pose, &previous_module_terminal_vehicle_edge_lane);

  if (!isCrossingPossible(previous_module_terminal_vehicle_edge_lane, target_lane)) {
    return false;
  }

  return true;
}

bool GoalPlannerModule::isExecutionReady() const
{
  // NOTE(soblin): at least in goal_planner, isExecutionReady is called via super::updateRTCStatus
  // from self::postProcess, so returning cached member variable like
  // path_decision_controller.get_current_state() is valid
  if (parameters_->safety_check_params.enable_safety_check && isWaitingApproval()) {
    if (!path_decision_controller_.get_current_state().is_stable_safe) {
      RCLCPP_INFO_THROTTLE(
        getLogger(), *clock_, 5000,
        "Path is not safe against dynamic objects, so the candidate path is not approved.");
      return false;
    }
  }
  return true;
}

double GoalPlannerModule::calcModuleRequestLength() const
{
  const auto min_stop_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk, 0.0);
  if (!min_stop_distance) {
    return parameters_->pull_over_minimum_request_length;
  }

  //  The module is requested at a distance such that the ego can stop for the pull over start point
  //  closest to ego. When path planning, each start point is checked to see if it is possible to
  //  stop again. At that time, if the speed has changed over time, the path will be rejected if
  //  min_stop_distance is used as is, so scale is applied to provide a buffer.
  constexpr double scale_factor_for_buffer = 1.2;
  const double minimum_request_length = *min_stop_distance * scale_factor_for_buffer +
                                        parameters_->backward_goal_search_length +
                                        approximate_pull_over_distance_;

  return std::max(minimum_request_length, parameters_->pull_over_minimum_request_length);
}

bool GoalPlannerModule::planFreespacePath(
  std::shared_ptr<const PlannerData> planner_data,
  const std::shared_ptr<GoalSearcherBase> goal_searcher, const GoalCandidates & goal_candidates_arg,
  const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map,
  const PredictedObjects & static_target_objects)
{
  auto goal_candidates = goal_candidates_arg;
  goal_searcher->update(goal_candidates, occupancy_grid_map, planner_data, static_target_objects);
  debug_data_.freespace_planner.num_goal_candidates = goal_candidates.size();
  debug_data_.freespace_planner.is_planning = true;

  for (size_t i = 0; i < goal_candidates.size(); i++) {
    const auto goal_candidate = goal_candidates.at(i);
    {
      const std::lock_guard<std::recursive_mutex> lock(mutex_);
      debug_data_.freespace_planner.current_goal_idx = i;
    }

    if (!goal_candidate.is_safe) {
      continue;
    }
    const auto freespace_path = freespace_planner_->plan(
      goal_candidate, 0, planner_data, BehaviorModuleOutput{}  // NOTE: not used so passing {} is OK
    );
    if (!freespace_path) {
      continue;
    }

    {
      const std::lock_guard<std::recursive_mutex> lock(mutex_);
      thread_safe_data_.set_pull_over_path(*freespace_path);
      debug_data_.freespace_planner.is_planning = false;
    }

    return true;
  }

  const std::lock_guard<std::recursive_mutex> lock(mutex_);
  debug_data_.freespace_planner.is_planning = false;
  return false;
}

bool GoalPlannerModule::canReturnToLaneParking(const PullOverContextData & context_data)
{
  // return only before starting free space parking
  if (!isStopped(
        odometry_buffer_stopped_, planner_data_->self_odometry, parameters_->th_stopped_time,
        parameters_->th_stopped_velocity)) {
    return false;
  }

  if (!context_data.pull_over_path_opt) {
    return false;
  }
  if (context_data.pull_over_path_opt.value().type() == PullOverPlannerType::FREESPACE) {
    return false;
  }
  const auto & lane_parking_path = context_data.pull_over_path_opt.value();

  const auto & path = lane_parking_path.full_path();
  const auto & curvatures = lane_parking_path.full_path_curvatures();
  if (
    parameters_->use_object_recognition &&
    goal_planner_utils::checkObjectsCollision(
      path, curvatures, context_data.static_target_objects, context_data.dynamic_target_objects,
      planner_data_->parameters,
      parameters_->object_recognition_collision_check_hard_margins.back(),
      /*extract_static_objects=*/false, parameters_->maximum_deceleration,
      parameters_->object_recognition_collision_check_max_extra_stopping_margin,
      debug_data_.ego_polygons_expanded)) {
    return false;
  }

  if (
    parameters_->use_occupancy_grid_for_path_collision_check &&
    checkOccupancyGridCollision(path, occupancy_grid_map_)) {
    return false;
  }

  const Point & current_point = planner_data_->self_odometry->pose.pose.position;
  constexpr double th_distance = 0.5;
  const bool is_close_to_path =
    std::abs(autoware::motion_utils::calcLateralOffset(path.points, current_point)) < th_distance;
  if (!is_close_to_path) {
    return false;
  }

  return true;
}

GoalCandidates GoalPlannerModule::generateGoalCandidates() const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  // calculate goal candidates
  const auto & route_handler = planner_data_->route_handler;
  if (utils::isAllowedGoalModification(route_handler)) {
    return goal_searcher_->search(planner_data_);
  }

  // NOTE:
  // currently since pull over is performed only when isAllowedGoalModification is true,
  // never be in the following process.
  GoalCandidate goal_candidate{};
  goal_candidate.goal_pose = route_handler->getOriginalGoalPose();
  goal_candidate.distance_from_original_goal = 0.0;
  GoalCandidates goal_candidates{};
  goal_candidates.push_back(goal_candidate);

  return goal_candidates;
}

BehaviorModuleOutput GoalPlannerModule::plan()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (utils::isAllowedGoalModification(planner_data_->route_handler)) {
    if (!context_data_) {
      RCLCPP_WARN_THROTTLE(
        getLogger(), *clock_, 5000, " [pull_over] plan() is called without valid context_data");
    } else {
      const auto & context_data = context_data_.value();
      return planPullOver(context_data);
    }
  }

  fixed_goal_planner_->setPreviousModuleOutput(getPreviousModuleOutput());
  return fixed_goal_planner_->plan(planner_data_);
}

std::optional<PullOverPath> GoalPlannerModule::selectPullOverPath(
  const PullOverContextData & context_data,
  const std::vector<PullOverPath> & pull_over_path_candidates,
  const GoalCandidates & goal_candidates) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  const auto & goal_pose = planner_data_->route_handler->getOriginalGoalPose();
  const double backward_length =
    parameters_->backward_goal_search_length + parameters_->decide_path_distance;
  const auto & prev_module_output_path = getPreviousModuleOutput().path;
  const auto & soft_margins = parameters_->object_recognition_collision_check_soft_margins;
  const auto & hard_margins = parameters_->object_recognition_collision_check_hard_margins;

  // STEP1: Filter valid paths before sorting
  // NOTE: Since copying pull over path takes time, it is handled by indices
  std::vector<size_t> sorted_path_indices;
  sorted_path_indices.reserve(pull_over_path_candidates.size());

  // STEP1-1: Extract paths which have safe goal
  // Create a map of goal_id to GoalCandidate for quick access
  std::unordered_map<int, GoalCandidate> goal_candidate_map;
  for (const auto & goal_candidate : goal_candidates) {
    goal_candidate_map[goal_candidate.id] = goal_candidate;
  }
  for (size_t i = 0; i < pull_over_path_candidates.size(); ++i) {
    const auto & path = pull_over_path_candidates[i];
    const auto goal_candidate_it = goal_candidate_map.find(path.goal_id());
    if (goal_candidate_it != goal_candidate_map.end() && goal_candidate_it->second.is_safe) {
      sorted_path_indices.push_back(i);
    }
  }

  // STEP1-2: Remove paths which do not have enough distance
  const double prev_path_front_to_goal_dist = calcSignedArcLength(
    prev_module_output_path.points, prev_module_output_path.points.front().point.pose.position,
    goal_pose.position);
  const auto & long_tail_reference_path = [&]() {
    if (prev_path_front_to_goal_dist > backward_length) {
      return prev_module_output_path;
    }
    // get road lanes which is at least backward_length[m] behind the goal
    const auto road_lanes = utils::getExtendedCurrentLanesFromPath(
      prev_module_output_path, planner_data_, backward_length, 0.0, false);
    const auto goal_pose_length = lanelet::utils::getArcCoordinates(road_lanes, goal_pose).length;
    return planner_data_->route_handler->getCenterLinePath(
      road_lanes, std::max(0.0, goal_pose_length - backward_length),
      goal_pose_length + parameters_->forward_goal_search_length);
  }();

  sorted_path_indices.erase(
    std::remove_if(
      sorted_path_indices.begin(), sorted_path_indices.end(),
      [&](const size_t i) {
        return !hasEnoughDistance(pull_over_path_candidates[i], long_tail_reference_path);
      }),
    sorted_path_indices.end());

  // STEP2: Sort pull over path candidates
  // STEP2-1: Sort pull_over_path_candidates based on the order in goal_candidates
  // Create a map of goal_id to its index in goal_candidates
  std::map<size_t, size_t> goal_id_to_index;
  for (size_t i = 0; i < goal_candidates.size(); ++i) {
    goal_id_to_index[goal_candidates[i].id] = i;
  }

  // Sort pull_over_path_candidates based on the order in goal_candidates
  std::stable_sort(
    sorted_path_indices.begin(), sorted_path_indices.end(),
    [&](const size_t a_i, const size_t b_i) {
      const auto & a = pull_over_path_candidates[a_i];
      const auto & b = pull_over_path_candidates[b_i];
      return goal_id_to_index[a.goal_id()] < goal_id_to_index[b.goal_id()];
    });

  // Sort the path only when the number of objects to avoid is the same to minimize the
  // num_objects_to_avoid.
  const auto isSameNumObjectsToAvoid = [&](const PullOverPath & a, const PullOverPath & b) -> bool {
    return goal_candidates[goal_id_to_index[a.goal_id()]].num_objects_to_avoid ==
           goal_candidates[goal_id_to_index[b.goal_id()]].num_objects_to_avoid;
  };

  // compare to sort pull_over_path_candidates based on the order in efficient_path_order
  const auto comparePathTypePriority = [&](const PullOverPath & a, const PullOverPath & b) -> bool {
    const auto & order = parameters_->efficient_path_order;
    const auto a_pos = std::find(order.begin(), order.end(), magic_enum::enum_name(a.type()));
    const auto b_pos = std::find(order.begin(), order.end(), magic_enum::enum_name(b.type()));
    return a_pos < b_pos;
  };

  // if object recognition is enabled, sort by collision check margin
  if (parameters_->use_object_recognition) {
    // STEP2-2: Sort by collision check margins
    const auto [margins, margins_with_zero] =
      std::invoke([&]() -> std::tuple<std::vector<double>, std::vector<double>> {
        std::vector<double> margins = soft_margins;
        margins.insert(margins.end(), hard_margins.begin(), hard_margins.end());
        std::vector<double> margins_with_zero = margins;
        margins_with_zero.push_back(0.0);
        return std::make_tuple(margins, margins_with_zero);
      });

    // Create a map of PullOverPath pointer to largest collision check margin
    std::map<size_t, double> path_id_to_rough_margin_map;
    const auto & target_objects = context_data.static_target_objects;
    for (const size_t i : sorted_path_indices) {
      const auto & path = pull_over_path_candidates[i];
      const double distance = utils::path_safety_checker::calculateRoughDistanceToObjects(
        path.parking_path(), target_objects, planner_data_->parameters, false, "max");
      auto it = std::lower_bound(
        margins_with_zero.begin(), margins_with_zero.end(), distance, std::greater<double>());
      if (it == margins_with_zero.end()) {
        path_id_to_rough_margin_map[path.id()] = margins_with_zero.back();
      } else {
        path_id_to_rough_margin_map[path.id()] = *it;
      }
    }

    // sorts in descending order so the item with larger margin comes first
    std::stable_sort(
      sorted_path_indices.begin(), sorted_path_indices.end(),
      [&](const size_t a_i, const size_t b_i) {
        const auto & a = pull_over_path_candidates[a_i];
        const auto & b = pull_over_path_candidates[b_i];
        if (!isSameNumObjectsToAvoid(a, b)) {
          return false;
        }
        if (
          std::abs(path_id_to_rough_margin_map[a.id()] - path_id_to_rough_margin_map[b.id()]) <
          0.01) {
          return false;
        }
        return path_id_to_rough_margin_map[a.id()] > path_id_to_rough_margin_map[b.id()];
      });

    // STEP2-3: Sort by curvature
    // If the curvature is less than the threshold, prioritize the path.
    const auto isHighCurvature = [&](const PullOverPath & path) -> bool {
      return path.parking_path_max_curvature() >= parameters_->high_curvature_threshold;
    };

    const auto isSoftMargin = [&](const PullOverPath & path) -> bool {
      const double margin = path_id_to_rough_margin_map[path.id()];
      return std::any_of(
        soft_margins.begin(), soft_margins.end(),
        [margin](const double soft_margin) { return std::abs(margin - soft_margin) < 0.01; });
    };
    const auto isSameHardMargin = [&](const PullOverPath & a, const PullOverPath & b) -> bool {
      return !isSoftMargin(a) && !isSoftMargin(b) &&
             std::abs(path_id_to_rough_margin_map[a.id()] - path_id_to_rough_margin_map[b.id()]) <
               0.01;
    };

    // NOTE: this is just partition sort based on curvature threshold within each sub partitions
    std::stable_sort(
      sorted_path_indices.begin(), sorted_path_indices.end(),
      [&](const size_t a_i, const size_t b_i) {
        const auto & a = pull_over_path_candidates[a_i];
        const auto & b = pull_over_path_candidates[b_i];
        if (!isSameNumObjectsToAvoid(a, b)) {
          return false;
        }

        // if both are soft margin or both are same hard margin, prioritize the path with lower
        // curvature.
        if ((isSoftMargin(a) && isSoftMargin(b)) || isSameHardMargin(a, b)) {
          return !isHighCurvature(a) && isHighCurvature(b);
        }
        // otherwise, keep the order based on the margin.
        return false;
      });

    // STEP2-4: Sort pull_over_path_candidates based on the order in efficient_path_order keeping
    // the collision check margin and curvature priority.
    if (parameters_->path_priority == "efficient_path") {
      std::stable_sort(
        sorted_path_indices.begin(), sorted_path_indices.end(),
        [&](const size_t a_i, const size_t b_i) {
          // if any of following conditions are met, sort by path type priority
          // - both are soft margin
          // - both are same hard margin
          const auto & a = pull_over_path_candidates[a_i];
          const auto & b = pull_over_path_candidates[b_i];
          if (!isSameNumObjectsToAvoid(a, b)) {
            return false;
          }
          if ((isSoftMargin(a) && isSoftMargin(b)) || isSameHardMargin(a, b)) {
            return comparePathTypePriority(a, b);
          }
          // otherwise, keep the order.
          return false;
        });
    }

    // debug print path priority sorted by
    // - efficient_path_order
    // - collision check margin
    // - curvature
    const std::string path_priority_info_str = goal_planner_utils::makePathPriorityDebugMessage(
      sorted_path_indices, pull_over_path_candidates, goal_id_to_index, goal_candidates,
      path_id_to_rough_margin_map, isSoftMargin, isHighCurvature);
    RCLCPP_DEBUG_STREAM(getLogger(), path_priority_info_str);
  } else {
    /**
     * NOTE: use_object_recognition=false is not recommended. This option will be deprecated in the
     * future. sort by curvature is not implemented yet.
     * Sort pull_over_path_candidates based on the order in efficient_path_order
     */
    if (parameters_->path_priority == "efficient_path") {
      std::stable_sort(
        sorted_path_indices.begin(), sorted_path_indices.end(),
        [&](const size_t a_i, const size_t b_i) {
          const auto & a = pull_over_path_candidates[a_i];
          const auto & b = pull_over_path_candidates[b_i];
          if (!isSameNumObjectsToAvoid(a, b)) {
            return false;
          }
          return comparePathTypePriority(a, b);
        });
    }
  }

  // STEP3: Select the final pull over path by checking collision to make it as high priority as
  // possible
  const double collision_check_margin =
    parameters_->object_recognition_collision_check_hard_margins.back();
  for (const size_t i : sorted_path_indices) {
    const auto & path = pull_over_path_candidates[i];
    const PathWithLaneId & parking_path = path.parking_path();
    const auto & parking_path_curvatures = path.parking_path_curvatures();
    if (
      parameters_->use_object_recognition &&
      goal_planner_utils::checkObjectsCollision(
        parking_path, parking_path_curvatures, context_data.static_target_objects,
        context_data.dynamic_target_objects, planner_data_->parameters, collision_check_margin,
        true, parameters_->maximum_deceleration,
        parameters_->object_recognition_collision_check_max_extra_stopping_margin,
        debug_data_.ego_polygons_expanded, true)) {
      continue;
    }
    if (
      parameters_->use_occupancy_grid_for_path_collision_check &&
      checkOccupancyGridCollision(parking_path, occupancy_grid_map_)) {
      continue;
    }
    return path;
  }
  return {};
}

std::vector<DrivableLanes> GoalPlannerModule::generateDrivableLanes() const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  const lanelet::ConstLanelets current_lanes = utils::getExtendedCurrentLanes(
    planner_data_, parameters_->backward_goal_search_length,
    parameters_->forward_goal_search_length,
    /*forward_only_in_route*/ false);
  const lanelet::ConstLanelets pull_over_lanes = goal_planner_utils::getPullOverLanes(
    *(planner_data_->route_handler), left_side_parking_, parameters_->backward_goal_search_length,
    parameters_->forward_goal_search_length);
  return utils::generateDrivableLanesWithShoulderLanes(current_lanes, pull_over_lanes);
}

void GoalPlannerModule::setOutput(
  const PullOverContextData & context_data, BehaviorModuleOutput & output)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  output.reference_path = getPreviousModuleOutput().reference_path;

  if (!context_data.pull_over_path_opt) {
    // situation : not safe against static objects use stop_path
    output.path = generateStopPath(context_data);
    RCLCPP_INFO_THROTTLE(
      getLogger(), *clock_, 5000, "Not found safe pull_over path, generate stop path");
    setDrivableAreaInfo(context_data, output);
    return;
  }

  const auto & pull_over_path = context_data.pull_over_path_opt.value();
  if (
    parameters_->safety_check_params.enable_safety_check && !context_data.is_stable_safe_path &&
    isActivated()) {
    // situation : not safe against dynamic objects after approval
    // insert stop point in current path if ego is able to stop with acceleration and jerk
    // constraints
    output.path = generateFeasibleStopPath(pull_over_path.getCurrentPath());
    RCLCPP_INFO_THROTTLE(
      getLogger(), *clock_, 5000, "Not safe against dynamic objects, generate stop path");
    debug_stop_pose_with_info_.set(std::string("feasible stop after approval"));
  } else {
    // situation : (safe against static and dynamic objects) or (safe against static objects and
    // before approval) don't stop
    // keep stop if not enough time passed,
    // because it takes time for the trajectory to be reflected
    auto current_path = pull_over_path.getCurrentPath();
    keepStoppedWithCurrentPath(context_data, current_path);
    output.path = current_path;
  }

  setModifiedGoal(context_data, output);
  setDrivableAreaInfo(context_data, output);

  // set hazard and turn signal
  if (
    path_decision_controller_.get_current_state().state ==
      PathDecisionState::DecisionKind::DECIDED &&
    isActivated()) {
    setTurnSignalInfo(context_data, output);
  }
}

void GoalPlannerModule::setDrivableAreaInfo(
  const PullOverContextData & context_data, BehaviorModuleOutput & output) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (
    context_data.pull_over_path_opt &&
    context_data.pull_over_path_opt.value().type() == PullOverPlannerType::FREESPACE) {
    const double drivable_area_margin = planner_data_->parameters.vehicle_width;
    output.drivable_area_info.drivable_margin =
      planner_data_->parameters.vehicle_width / 2.0 + drivable_area_margin;
  } else {
    const auto target_drivable_lanes = utils::getNonOverlappingExpandedLanes(
      output.path, generateDrivableLanes(), planner_data_->drivable_area_expansion_parameters);

    DrivableAreaInfo current_drivable_area_info;
    current_drivable_area_info.drivable_lanes = target_drivable_lanes;
    output.drivable_area_info = utils::combineDrivableAreaInfo(
      current_drivable_area_info, getPreviousModuleOutput().drivable_area_info);
  }
}

void GoalPlannerModule::setModifiedGoal(
  const PullOverContextData & context_data, BehaviorModuleOutput & output) const
{
  const auto & route_handler = planner_data_->route_handler;
  if (context_data.pull_over_path_opt) {
    PoseWithUuidStamped modified_goal{};
    modified_goal.uuid = route_handler->getRouteUuid();
    modified_goal.pose = context_data.pull_over_path_opt.value().modified_goal_pose();
    modified_goal.header = route_handler->getRouteHeader();
    output.modified_goal = modified_goal;
  } else {
    output.modified_goal = {};
  }
}

void GoalPlannerModule::setTurnSignalInfo(
  const PullOverContextData & context_data, BehaviorModuleOutput & output)
{
  const auto original_signal = getPreviousModuleOutput().turn_signal_info;
  const auto new_signal = calcTurnSignalInfo(context_data);
  const auto current_seg_idx = planner_data_->findEgoSegmentIndex(output.path.points);
  output.turn_signal_info = planner_data_->turn_signal_decider.overwrite_turn_signal(
    output.path, getEgoPose(), current_seg_idx, original_signal, new_signal,
    planner_data_->parameters.ego_nearest_dist_threshold,
    planner_data_->parameters.ego_nearest_yaw_threshold);
}

void GoalPlannerModule::updateSteeringFactor(
  const PullOverContextData & context_data, const std::array<Pose, 2> & pose,
  const std::array<double, 2> distance, const uint16_t type)
{
  const uint16_t steering_factor_direction = std::invoke([&]() {
    const auto turn_signal = calcTurnSignalInfo(context_data);
    if (turn_signal.turn_signal.command == TurnIndicatorsCommand::ENABLE_LEFT) {
      return SteeringFactor::LEFT;
    } else if (turn_signal.turn_signal.command == TurnIndicatorsCommand::ENABLE_RIGHT) {
      return SteeringFactor::RIGHT;
    }
    return SteeringFactor::STRAIGHT;
  });

  steering_factor_interface_.set(pose, distance, steering_factor_direction, type, "");
}

void GoalPlannerModule::decideVelocity()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  const double current_vel = planner_data_->self_odometry->twist.twist.linear.x;

  // partial_paths
  // TODO(soblin): only update velocity on main thread side, use that on main thread side
  auto & first_path = thread_safe_data_.get_pull_over_path()->partial_paths().front();
  const auto vel =
    static_cast<float>(std::max(current_vel, parameters_->pull_over_minimum_velocity));
  for (auto & p : first_path.points) {
    p.point.longitudinal_velocity_mps = std::min(p.point.longitudinal_velocity_mps, vel);
  }
}

BehaviorModuleOutput GoalPlannerModule::planPullOver(const PullOverContextData & context_data)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (
    path_decision_controller_.get_current_state().state !=
    PathDecisionState::DecisionKind::DECIDED) {
    return planPullOverAsCandidate(context_data);
  }

  return planPullOverAsOutput(context_data);
}

BehaviorModuleOutput GoalPlannerModule::planPullOverAsCandidate(
  const PullOverContextData & context_data)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  // if pull over path candidates generation is not finished, use previous module output
  if (context_data.pull_over_path_candidates.empty()) {
    return getPreviousModuleOutput();
  }

  BehaviorModuleOutput output{};
  const BehaviorModuleOutput pull_over_output = planPullOverAsOutput(context_data);
  output.modified_goal = pull_over_output.modified_goal;
  output.path = generateStopPath(context_data);
  output.reference_path = getPreviousModuleOutput().reference_path;

  const auto target_drivable_lanes = utils::getNonOverlappingExpandedLanes(
    output.path, generateDrivableLanes(), planner_data_->drivable_area_expansion_parameters);

  DrivableAreaInfo current_drivable_area_info{};
  current_drivable_area_info.drivable_lanes = target_drivable_lanes;
  output.drivable_area_info = utils::combineDrivableAreaInfo(
    current_drivable_area_info, getPreviousModuleOutput().drivable_area_info);

  if (!context_data.pull_over_path_opt) {
    return output;
  }

  setDebugData(context_data);

  return output;
}

BehaviorModuleOutput GoalPlannerModule::planPullOverAsOutput(
  const PullOverContextData & context_data)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  std::chrono::system_clock::time_point start;
  start = std::chrono::system_clock::now();

  // if pull over path candidates generation is not finished, use previous module output
  if (context_data.pull_over_path_candidates.empty()) {
    return getPreviousModuleOutput();
  }

  auto context_data_with_velocity = context_data;
  /**
     NOTE(soblin): this path originates from the previously selected(by main thread) pull_over_path
     which was originally generated by either road_parking or freespace thread
   */
  auto & pull_over_path_with_velocity_opt = context_data_with_velocity.pull_over_path_opt;
  const bool is_freespace =
    pull_over_path_with_velocity_opt &&
    pull_over_path_with_velocity_opt.value().type() == PullOverPlannerType::FREESPACE;
  const std::optional<GoalCandidate> modified_goal_opt =
    pull_over_path_with_velocity_opt
      ? std::make_optional<GoalCandidate>(pull_over_path_with_velocity_opt.value().modified_goal())
      : std::nullopt;
  if (
    path_decision_controller_.get_current_state().state ==
      PathDecisionState::DecisionKind::NOT_DECIDED &&
    !is_freespace &&
    needPathUpdate(
      planner_data_->self_odometry->pose.pose, 1.0 /*path_update_duration*/, clock_->now(),
      modified_goal_opt, thread_safe_data_.get_last_path_update_time(), *parameters_)) {
    // if the final path is not decided and enough time has passed since last path update,
    // select safe path from lane parking pull over path candidates
    // and set it to thread_safe_data_
    RCLCPP_DEBUG(getLogger(), "Update pull over path candidates");

    thread_safe_data_.clearPullOverPath();

    // update goal candidates
    auto goal_candidates = goal_candidates_;
    goal_searcher_->update(
      goal_candidates, occupancy_grid_map_, planner_data_, context_data.static_target_objects);

    // Select a path that is as safe as possible and has a high priority.
    const auto & pull_over_path_candidates = context_data.pull_over_path_candidates;
    auto path_and_goal_opt =
      selectPullOverPath(context_data, pull_over_path_candidates, goal_candidates);

    // update thread_safe_data_
    if (path_and_goal_opt) {
      const auto & pull_over_path = path_and_goal_opt.value();
      /** TODO(soblin): since thread_safe_data::pull_over_path was used as a global variable, old
       * code was setting deceleration to thread_safe_data::pull_over_path and setOutput() accessed
       * to the velocity profile in thread_safe_data::pull_over_path, which is a very bad usage of
       * member variable
       *
       * set this selected pull_over_path to ThreadSafeData, but actually RoadParking thread does
       * not use pull_over_path, but only FreespaceParking thread use this selected pull_over_path.
       * As the next action item, only set this selected pull_over_path to only
       * FreespaceThreadSafeData.
       */
      thread_safe_data_.set(pull_over_path);
      if (pull_over_path_with_velocity_opt) {
        auto & pull_over_path_with_velocity = pull_over_path_with_velocity_opt.value();
        // copy the path for later setOutput()
        pull_over_path_with_velocity = pull_over_path;
        // modify the velocity for latest setOutput()
        deceleratePath(pull_over_path_with_velocity);
      }
      RCLCPP_DEBUG(
        getLogger(), "selected pull over path: path_id: %ld, goal_id: %ld", pull_over_path.id(),
        pull_over_path.modified_goal().id);
    }
  }

  // set output and status
  BehaviorModuleOutput output{};
  setOutput(context_data_with_velocity, output);

  // return to lane parking if it is possible
  if (is_freespace && canReturnToLaneParking(context_data_with_velocity)) {
    // TODO(soblin): return from freespace to lane is disabled temporarily, because if
    // context_data_with_velocity contained freespace path, since lane_parking_pull_over_path is
    // deleted, freespace path is set again
    if (context_data_with_velocity.pull_over_path_opt) {
      thread_safe_data_.set_pull_over_path(context_data_with_velocity.pull_over_path_opt.value());
    }
  }

  // For debug
  setDebugData(context_data_with_velocity);

  if (!pull_over_path_with_velocity_opt) {
    return output;
  }

  path_candidate_ =
    std::make_shared<PathWithLaneId>(pull_over_path_with_velocity_opt.value().full_path());

  return output;
}

void GoalPlannerModule::postProcess()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (!context_data_) {
    RCLCPP_WARN_THROTTLE(
      getLogger(), *clock_, 5000,
      " [pull_over] postProcess() is called without valid context_data. use dummy context data.");
  }
  const auto context_data_dummy =
    PullOverContextData(true, PredictedObjects{}, PredictedObjects{}, std::nullopt, {}, {});
  const auto & context_data =
    context_data_.has_value() ? context_data_.value() : context_data_dummy;

  const bool has_decided_path =
    path_decision_controller_.get_current_state().state == PathDecisionState::DecisionKind::DECIDED;

  if (!context_data.pull_over_path_opt) {
    context_data_ = std::nullopt;
    return;
  }
  const auto & pull_over_path = context_data.pull_over_path_opt.value();

  const auto distance_to_path_change = calcDistanceToPathChange(context_data);

  if (has_decided_path) {
    updateRTCStatus(distance_to_path_change.first, distance_to_path_change.second);
  }

  updateSteeringFactor(
    context_data, {pull_over_path.start_pose(), pull_over_path.modified_goal_pose()},
    {distance_to_path_change.first, distance_to_path_change.second},
    has_decided_path ? SteeringFactor::TURNING : SteeringFactor::APPROACHING);

  setVelocityFactor(pull_over_path.full_path());

  context_data_ = std::nullopt;
}

BehaviorModuleOutput GoalPlannerModule::planWaitingApproval()
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (utils::isAllowedGoalModification(planner_data_->route_handler)) {
    if (!context_data_) {
      RCLCPP_WARN_THROTTLE(
        getLogger(), *clock_, 5000,
        " [pull_over] planWaitingApproval() is called without valid context_data. use fixed goal "
        "planner");
    } else {
      const auto & context_data = context_data_.value();
      return planPullOverAsCandidate(context_data);
    }
  }

  fixed_goal_planner_->setPreviousModuleOutput(getPreviousModuleOutput());
  return fixed_goal_planner_->plan(planner_data_);
}

std::pair<double, double> GoalPlannerModule::calcDistanceToPathChange(
  const PullOverContextData & context_data) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (!context_data.pull_over_path_opt) {
    return {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
  }
  const auto & pull_over_path = context_data.pull_over_path_opt.value();

  const auto & full_path = context_data.pull_over_path_opt.value().full_path();

  const auto ego_segment_idx = autoware::motion_utils::findNearestSegmentIndex(
    full_path.points, planner_data_->self_odometry->pose.pose, std::numeric_limits<double>::max(),
    M_PI_2);
  if (!ego_segment_idx) {
    return {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
  }

  const size_t start_pose_segment_idx = autoware::motion_utils::findNearestSegmentIndex(
    full_path.points, pull_over_path.start_pose().position);
  const double dist_to_parking_start_pose = calcSignedArcLength(
    full_path.points, planner_data_->self_odometry->pose.pose.position, *ego_segment_idx,
    pull_over_path.start_pose().position, start_pose_segment_idx);
  const size_t goal_pose_segment_idx = autoware::motion_utils::findNearestSegmentIndex(
    full_path.points, pull_over_path.modified_goal_pose().position);
  const double dist_to_parking_finish_pose = calcSignedArcLength(
    full_path.points, planner_data_->self_odometry->pose.pose.position, *ego_segment_idx,
    pull_over_path.modified_goal_pose().position, goal_pose_segment_idx);

  return {dist_to_parking_start_pose, dist_to_parking_finish_pose};
}

void GoalPlannerModule::setParameters(const std::shared_ptr<GoalPlannerParameters> & parameters)
{
  parameters_ = parameters;
}

PathWithLaneId GoalPlannerModule::generateStopPath(const PullOverContextData & context_data) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  const auto & route_handler = planner_data_->route_handler;
  const auto & current_pose = planner_data_->self_odometry->pose.pose;
  const auto & common_parameters = planner_data_->parameters;
  const double current_vel = planner_data_->self_odometry->twist.twist.linear.x;
  const double pull_over_velocity = parameters_->pull_over_velocity;

  const lanelet::ConstLanelets current_lanes = utils::getExtendedCurrentLanes(
    planner_data_, parameters_->backward_goal_search_length,
    parameters_->forward_goal_search_length,
    /*forward_only_in_route*/ false);

  if (current_lanes.empty()) {
    return PathWithLaneId{};
  }

  // extend previous module path to generate reference path for stop path
  const auto reference_path = std::invoke([&]() -> PathWithLaneId {
    const auto s_current = lanelet::utils::getArcCoordinates(current_lanes, current_pose).length;
    const double s_start = std::max(0.0, s_current - common_parameters.backward_path_length);
    const double s_end = s_current + common_parameters.forward_path_length;
    return route_handler->getCenterLinePath(current_lanes, s_start, s_end, true);
  });

  // NOTE: The previous module may insert a zero velocity at the end of the path, so remove it by
  // setting remove_connected_zero_velocity=true. Inserting a velocity of 0 into the goal is the
  // role of the goal planner, and the intermediate zero velocity after extension is unnecessary.
  const auto extended_prev_path = goal_planner_utils::extendPath(
    getPreviousModuleOutput().path, reference_path, common_parameters.forward_path_length, true);

  // calculate search start offset pose from the closest goal candidate pose with
  // approximate_pull_over_distance_ ego vehicle decelerates to this position. or if no feasible
  // stop point is found, stop at this position.
  const auto closest_goal_candidate =
    goal_searcher_->getClosetGoalCandidateAlongLanes(goal_candidates_, planner_data_);
  const auto decel_pose = calcLongitudinalOffsetPose(
    extended_prev_path.points, closest_goal_candidate.goal_pose.position,
    -approximate_pull_over_distance_);

  // if not approved stop road lane.
  // stop point priority is
  // 1. actual start pose
  // 2. closest candidate start pose
  // 3. pose offset by approximate_pull_over_distance_ from search start pose.
  //     (In the case of the curve lane, the position is not aligned due to the
  //     difference between the outer and inner sides)
  // 4. feasible stop
  const auto stop_pose_with_info =
    std::invoke([&]() -> std::optional<std::pair<Pose, std::string>> {
      if (context_data.pull_over_path_opt) {
        return std::make_pair(
          context_data.pull_over_path_opt.value().start_pose(), "stop at selected start pose");
      }
      if (thread_safe_data_.get_closest_start_pose()) {
        return std::make_pair(
          thread_safe_data_.get_closest_start_pose().value(), "stop at closest start pose");
      }
      if (!decel_pose) {
        return std::nullopt;
      }
      return std::make_pair(decel_pose.value(), "stop at search start pose");
    });
  if (!stop_pose_with_info) {
    const auto feasible_stop_path = generateFeasibleStopPath(getPreviousModuleOutput().path);
    // override stop pose info debug string
    debug_stop_pose_with_info_.set(std::string("feasible stop: not calculate stop pose"));
    return feasible_stop_path;
  }
  const Pose stop_pose = stop_pose_with_info->first;

  // if stop pose is closer than min_stop_distance, stop as soon as possible
  const double ego_to_stop_distance = calcSignedArcLengthFromEgo(extended_prev_path, stop_pose);
  const auto min_stop_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk, 0.0);
  const double eps_vel = 0.01;
  const bool is_stopped = std::abs(current_vel) < eps_vel;
  const double buffer = is_stopped ? stop_distance_buffer_ : 0.0;
  if (min_stop_distance && ego_to_stop_distance + buffer < *min_stop_distance) {
    const auto feasible_stop_path = generateFeasibleStopPath(getPreviousModuleOutput().path);
    debug_stop_pose_with_info_.set(
      std::string("feasible stop: stop pose is closer than min_stop_distance"));
    return feasible_stop_path;
  }

  // slow down for turn signal, insert stop point to stop_pose
  auto stop_path = extended_prev_path;
  decelerateForTurnSignal(stop_pose, stop_path);
  debug_stop_pose_with_info_.set(stop_pose, stop_pose_with_info->second);

  // slow down before the search area.
  if (decel_pose) {
    decelerateBeforeSearchStart(*decel_pose, stop_path);
    return stop_path;
  }

  // if already passed the decel pose, set pull_over_velocity to stop_path.
  const auto min_decel_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk,
    pull_over_velocity);
  for (auto & p : stop_path.points) {
    const double distance_from_ego = calcSignedArcLengthFromEgo(stop_path, p.point.pose);
    if (min_decel_distance && distance_from_ego < *min_decel_distance) {
      continue;
    }
    p.point.longitudinal_velocity_mps =
      std::min(p.point.longitudinal_velocity_mps, static_cast<float>(pull_over_velocity));
  }
  return stop_path;
}

PathWithLaneId GoalPlannerModule::generateFeasibleStopPath(const PathWithLaneId & path) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  // calc minimum stop distance under maximum deceleration
  const auto min_stop_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk, 0.0);
  if (!min_stop_distance) {
    return path;
  }

  // set stop point
  auto stop_path = path;
  const auto & current_pose = planner_data_->self_odometry->pose.pose;
  const auto stop_idx =
    autoware::motion_utils::insertStopPoint(current_pose, *min_stop_distance, stop_path.points);
  if (stop_idx) {
    debug_stop_pose_with_info_.set(stop_path.points.at(*stop_idx).point.pose, "feasible stop");
  }

  return stop_path;
}

bool GoalPlannerModule::isStuck(
  const PredictedObjects & static_target_objects, const PredictedObjects & dynamic_target_objects,
  const std::shared_ptr<const PlannerData> planner_data,
  const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map,
  const GoalPlannerParameters & parameters)
{
  const bool found_pull_over_path = thread_safe_data_.foundPullOverPath();
  const std::optional<PullOverPath> pull_over_path_opt =
    found_pull_over_path ? std::make_optional<PullOverPath>(*thread_safe_data_.get_pull_over_path())
                         : std::nullopt;
  const std::optional<GoalCandidate> modified_goal_opt =
    pull_over_path_opt
      ? std::make_optional<GoalCandidate>(pull_over_path_opt.value().modified_goal())
      : std::nullopt;
  const std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (isOnModifiedGoal(planner_data->self_odometry->pose.pose, modified_goal_opt, parameters)) {
    return false;
  }

  constexpr double stuck_time = 5.0;
  if (!isStopped(
        odometry_buffer_stuck_, planner_data->self_odometry, stuck_time,
        parameters_->th_stopped_velocity)) {
    return false;
  }

  if (!found_pull_over_path) {
    return true;
  }

  const auto & pull_over_path = pull_over_path_opt.value();
  if (parameters.use_object_recognition) {
    const auto & path = pull_over_path.getCurrentPath();
    const auto curvatures = autoware::motion_utils::calcCurvature(path.points);
    if (goal_planner_utils::checkObjectsCollision(
          path, curvatures, static_target_objects, dynamic_target_objects, planner_data->parameters,
          parameters.object_recognition_collision_check_hard_margins.back(),
          /*extract_static_objects=*/false, parameters.maximum_deceleration,
          parameters.object_recognition_collision_check_max_extra_stopping_margin,
          debug_data_.ego_polygons_expanded)) {
      return true;
    }
  }

  if (
    parameters.use_occupancy_grid_for_path_collision_check &&
    checkOccupancyGridCollision(
      thread_safe_data_.get_pull_over_path()->getCurrentPath(), occupancy_grid_map)) {
    return true;
  }

  return false;
}

bool GoalPlannerModule::hasFinishedCurrentPath(const PullOverContextData & ctx_data)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (!last_approval_data_) {
    return false;
  }

  if (!isStopped(
        odometry_buffer_stopped_, planner_data_->self_odometry, parameters_->th_stopped_time,
        parameters_->th_stopped_velocity)) {
    return false;
  }

  // check if enough time has passed since last approval
  // this is necessary to give turn signal for enough time
  const bool has_passed_enough_time_from_approval =
    (clock_->now() - last_approval_data_->time).seconds() >
    planner_data_->parameters.turn_signal_search_time;
  if (!has_passed_enough_time_from_approval) {
    return false;
  }

  // require increment only when the time passed is enough
  // to prevent increment before driving
  // when the end of the current path is close to the current pose
  // this value should be `keep_stop_time` in keepStoppedWithCurrentPath
  constexpr double keep_current_idx_time = 4.0;
  const bool has_passed_enough_time_from_increment =
    (clock_->now() - *thread_safe_data_.get_last_path_update_time()).seconds() >
    keep_current_idx_time;
  if (!has_passed_enough_time_from_increment) {
    return false;
  }

  // check if self pose is near the end of current path
  if (!ctx_data.pull_over_path_opt) {
    return false;
  }
  const auto & current_path_end =
    ctx_data.pull_over_path_opt.value().getCurrentPath().points.back();
  const auto & self_pose = planner_data_->self_odometry->pose.pose;
  return autoware::universe_utils::calcDistance2d(current_path_end, self_pose) <
         parameters_->th_arrived_distance;
}

TurnSignalInfo GoalPlannerModule::calcTurnSignalInfo(const PullOverContextData & context_data)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (!context_data.pull_over_path_opt) {
    return {};
  }
  const auto & pull_over_path = context_data.pull_over_path_opt.value();

  const auto & path = pull_over_path.full_path();
  if (path.points.empty()) return getPreviousModuleOutput().turn_signal_info;

  const auto & current_pose = planner_data_->self_odometry->pose.pose;
  const auto & start_pose = pull_over_path.start_pose();
  const auto & end_pose = pull_over_path.modified_goal_pose();

  const auto shift_start_idx =
    autoware::motion_utils::findNearestIndex(path.points, start_pose.position);
  const auto shift_end_idx =
    autoware::motion_utils::findNearestIndex(path.points, end_pose.position);

  const auto is_ignore_signal = [this](const lanelet::Id & id) {
    if (!ignore_signal_.has_value()) {
      return false;
    }
    return ignore_signal_.value() == id;
  };

  const auto update_ignore_signal = [](const lanelet::Id & id, const bool is_ignore) {
    return is_ignore ? std::make_optional(id) : std::nullopt;
  };

  const lanelet::ConstLanelets current_lanes = utils::getExtendedCurrentLanes(
    planner_data_, parameters_->backward_goal_search_length,
    parameters_->forward_goal_search_length,
    /*forward_only_in_route*/ false);

  if (current_lanes.empty()) {
    return {};
  }

  lanelet::Lanelet closest_lanelet;
  lanelet::utils::query::getClosestLanelet(current_lanes, current_pose, &closest_lanelet);

  if (is_ignore_signal(closest_lanelet.id())) {
    return getPreviousModuleOutput().turn_signal_info;
  }

  const double current_shift_length =
    lanelet::utils::getArcCoordinates(current_lanes, current_pose).distance;

  constexpr bool egos_lane_is_shifted = true;
  constexpr bool is_driving_forward = true;

  constexpr bool is_pull_out = false;
  constexpr bool is_lane_change = false;
  constexpr bool is_pull_over = true;
  const bool override_ego_stopped_check = std::invoke([&]() {
    if (pull_over_path.type() == PullOverPlannerType::SHIFT) {
      return false;
    }
    constexpr double distance_threshold = 1.0;
    const auto stop_point = pull_over_path.partial_paths().front().points.back();
    const double distance_from_ego_to_stop_point =
      std::abs(autoware::motion_utils::calcSignedArcLength(
        path.points, stop_point.point.pose.position, current_pose.position));
    return distance_from_ego_to_stop_point < distance_threshold;
  });

  const auto [new_signal, is_ignore] = planner_data_->getBehaviorTurnSignalInfo(
    path, shift_start_idx, shift_end_idx, current_lanes, current_shift_length, is_driving_forward,
    egos_lane_is_shifted, override_ego_stopped_check, is_pull_out, is_lane_change, is_pull_over);
  ignore_signal_ = update_ignore_signal(closest_lanelet.id(), is_ignore);

  return new_signal;
}

bool GoalPlannerModule::hasEnoughDistance(
  const PullOverPath & pull_over_path, const PathWithLaneId & long_tail_reference_path) const
{
  const Pose & current_pose = planner_data_->self_odometry->pose.pose;
  const double current_vel = planner_data_->self_odometry->twist.twist.linear.x;

  // when the path is separated and start_pose is close,
  // once stopped, the vehicle cannot start again.
  // so need enough distance to restart.
  // distance to restart should be less than decide_path_distance.
  // otherwise, the goal would change immediately after departure.
  const bool is_separated_path = pull_over_path.partial_paths().size() > 1;
  const double distance_to_start = calcSignedArcLength(
    long_tail_reference_path.points, current_pose.position, pull_over_path.start_pose().position);
  const double distance_to_restart = parameters_->decide_path_distance / 2;
  const double eps_vel = 0.01;
  const bool is_stopped = std::abs(current_vel) < eps_vel;
  if (is_separated_path && is_stopped && distance_to_start < distance_to_restart) {
    return false;
  }

  const auto current_to_stop_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk, 0.0);
  if (!current_to_stop_distance) {
    return false;
  }

  // If the stop line is subtly exceeded, it is assumed that there is not enough distance to the
  // starting point of parking, so to prevent this, once the vehicle has stopped, it also has a
  // stop_distance_buffer to allow for the amount exceeded.
  const double buffer = is_stopped ? stop_distance_buffer_ : 0.0;
  if (distance_to_start + buffer < *current_to_stop_distance) {
    return false;
  }

  return true;
}

void GoalPlannerModule::keepStoppedWithCurrentPath(
  const PullOverContextData & ctx_data, PathWithLaneId & path) const
{
  const auto last_path_idx_increment_time = ctx_data.last_path_idx_increment_time;
  constexpr double keep_stop_time = 2.0;
  if (!last_path_idx_increment_time) {
    return;
  }

  const auto time_diff = (clock_->now() - last_path_idx_increment_time.value()).seconds();
  if (time_diff > keep_stop_time) {
    return;
  }

  for (auto & p : path.points) {
    p.point.longitudinal_velocity_mps = 0.0;
  }
}

double GoalPlannerModule::calcSignedArcLengthFromEgo(
  const PathWithLaneId & path, const Pose & pose) const
{
  const auto & current_pose = planner_data_->self_odometry->pose.pose;
  const auto & p = planner_data_->parameters;

  const size_t ego_idx = planner_data_->findEgoIndex(path.points);
  const size_t target_idx = findFirstNearestSegmentIndexWithSoftConstraints(
    path.points, pose, p.ego_nearest_dist_threshold, p.ego_nearest_yaw_threshold);
  return calcSignedArcLength(
    path.points, current_pose.position, ego_idx, pose.position, target_idx);
}

void GoalPlannerModule::deceleratePath(PullOverPath & pull_over_path) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  // decelerate before the search area start
  const auto closest_goal_candidate =
    goal_searcher_->getClosetGoalCandidateAlongLanes(goal_candidates_, planner_data_);
  const auto decel_pose = calcLongitudinalOffsetPose(
    pull_over_path.full_path().points, closest_goal_candidate.goal_pose.position,
    -approximate_pull_over_distance_);
  auto & first_path = pull_over_path.partial_paths().front();
  if (decel_pose) {
    decelerateBeforeSearchStart(*decel_pose, first_path);
    return;
  }

  // if already passed the search start offset pose, set pull_over_velocity to first_path.
  const auto min_decel_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk,
    parameters_->pull_over_velocity);
  for (auto & p : first_path.points) {
    const double distance_from_ego = calcSignedArcLengthFromEgo(first_path, p.point.pose);
    if (min_decel_distance && distance_from_ego < *min_decel_distance) {
      continue;
    }
    p.point.longitudinal_velocity_mps = std::min(
      p.point.longitudinal_velocity_mps, static_cast<float>(parameters_->pull_over_velocity));
  }
}

void GoalPlannerModule::decelerateForTurnSignal(const Pose & stop_pose, PathWithLaneId & path) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  const double time = planner_data_->parameters.turn_signal_search_time;
  const Pose & current_pose = planner_data_->self_odometry->pose.pose;

  for (auto & point : path.points) {
    const double distance_to_stop = std::max(
      0.0, calcSignedArcLength(path.points, point.point.pose.position, stop_pose.position));
    const float decel_vel =
      std::min(point.point.longitudinal_velocity_mps, static_cast<float>(distance_to_stop / time));
    const double distance_from_ego = calcSignedArcLengthFromEgo(path, point.point.pose);
    const auto min_decel_distance = calcFeasibleDecelDistance(
      planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk, decel_vel);

    // when current velocity already lower than decel_vel, min_decel_distance will be 0.0,
    // and do not need to decelerate.
    // skip next process to avoid inserting decel point at the same current position.
    constexpr double eps_distance = 0.1;
    if (!min_decel_distance || *min_decel_distance < eps_distance) {
      continue;
    }

    if (*min_decel_distance < distance_from_ego) {
      point.point.longitudinal_velocity_mps = decel_vel;
    } else {
      insertDecelPoint(current_pose.position, *min_decel_distance, decel_vel, path.points);
    }
  }

  const double stop_point_length = calcSignedArcLength(path.points, 0, stop_pose.position);
  const auto min_stop_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk, 0.0);

  if (min_stop_distance && *min_stop_distance < stop_point_length) {
    utils::insertStopPoint(stop_point_length, path);
  }
}

void GoalPlannerModule::decelerateBeforeSearchStart(
  const Pose & search_start_offset_pose, PathWithLaneId & path) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  const double pull_over_velocity = parameters_->pull_over_velocity;
  const Pose & current_pose = planner_data_->self_odometry->pose.pose;

  // slow down before the search area.
  const auto min_decel_distance = calcFeasibleDecelDistance(
    planner_data_, parameters_->maximum_deceleration, parameters_->maximum_jerk,
    pull_over_velocity);
  if (min_decel_distance) {
    const double distance_to_search_start =
      calcSignedArcLengthFromEgo(path, search_start_offset_pose);
    const double distance_to_decel =
      std::max(*min_decel_distance, distance_to_search_start - approximate_pull_over_distance_);
    insertDecelPoint(current_pose.position, distance_to_decel, pull_over_velocity, path.points);
  }
}

bool GoalPlannerModule::isCrossingPossible(
  const lanelet::ConstLanelet & start_lane, const lanelet::ConstLanelet & end_lane) const
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  if (start_lane.centerline().empty() || end_lane.centerline().empty()) {
    return false;
  }

  if (start_lane == end_lane) {
    return true;
  }

  const auto & route_handler = planner_data_->route_handler;

  lanelet::ConstLanelets start_lane_sequence = route_handler->getLaneletSequence(start_lane);

  // get end lane sequence based on whether it is shoulder lanelet or not
  lanelet::ConstLanelets end_lane_sequence{};
  const bool is_shoulder_lane = route_handler->isShoulderLanelet(end_lane);
  if (is_shoulder_lane) {
    Pose end_lane_pose{};
    end_lane_pose.orientation.w = 1.0;
    end_lane_pose.position = lanelet::utils::conversion::toGeomMsgPt(end_lane.centerline().front());
    // NOTE: this line does not specify the /forward/backward length, so if the shoulders form a
    // loop, this returns all shoulder lanes in the loop
    end_lane_sequence = route_handler->getShoulderLaneletSequence(end_lane, end_lane_pose);
  } else {
    const double dist = std::numeric_limits<double>::max();
    end_lane_sequence = route_handler->getLaneletSequence(end_lane, dist, dist, false);
  }

  const auto getNeighboringLane =
    [&](const lanelet::ConstLanelet & lane) -> std::optional<lanelet::ConstLanelet> {
    return left_side_parking_ ? route_handler->getLeftLanelet(lane, false, true)
                              : route_handler->getRightLanelet(lane, false, true);
  };

  // Iterate through start_lane_sequence to find a path to end_lane_sequence
  for (auto it = start_lane_sequence.rbegin(); it != start_lane_sequence.rend(); ++it) {
    lanelet::ConstLanelet current_lane = *it;

    // Check if the current lane is in the end_lane_sequence
    auto end_it = std::find(end_lane_sequence.rbegin(), end_lane_sequence.rend(), current_lane);
    if (end_it != end_lane_sequence.rend()) {
      return true;
    }

    // Traversing is not allowed between road lanes
    if (!is_shoulder_lane) {
      continue;
    }

    // Traverse the lanes horizontally until the end_lane_sequence is reached
    std::optional<lanelet::ConstLanelet> neighboring_lane = getNeighboringLane(current_lane);
    if (neighboring_lane) {
      // Check if the neighboring lane is in the end_lane_sequence
      end_it =
        std::find(end_lane_sequence.rbegin(), end_lane_sequence.rend(), neighboring_lane.value());
      if (end_it != end_lane_sequence.rend()) {
        return true;
      }
    }
  }

  return false;
}

bool GoalPlannerModule::isCrossingPossible(
  const Pose & start_pose, const Pose & end_pose, const lanelet::ConstLanelets lanes) const
{
  lanelet::ConstLanelet start_lane{};
  lanelet::utils::query::getClosestLanelet(lanes, start_pose, &start_lane);

  lanelet::ConstLanelet end_lane{};
  lanelet::utils::query::getClosestLanelet(lanes, end_pose, &end_lane);

  return isCrossingPossible(start_lane, end_lane);
}

bool GoalPlannerModule::isCrossingPossible(const PullOverPath & pull_over_path) const
{
  const lanelet::ConstLanelets lanes = utils::transformToLanelets(generateDrivableLanes());
  const Pose & start_pose = pull_over_path.start_pose();
  const Pose & end_pose = pull_over_path.modified_goal_pose();

  return isCrossingPossible(start_pose, end_pose, lanes);
}

/*
void GoalPlannerModule::updateSafetyCheckTargetObjectsData(
  const PredictedObjects & filtered_objects, const TargetObjectsOnLane & target_objects_on_lane,
  const std::vector<PoseWithVelocityStamped> & ego_predicted_path) const
{
  goal_planner_data_.filtered_objects = filtered_objects;
  goal_planner_data_.target_objects_on_lane = target_objects_on_lane;
  goal_planner_data_.ego_predicted_path = ego_predicted_path;
}
*/

static std::vector<utils::path_safety_checker::ExtendedPredictedObject> filterObjectsByWithinPolicy(
  const std::shared_ptr<const PredictedObjects> & objects,
  const lanelet::ConstLanelets & target_lanes,
  const std::shared_ptr<
    autoware::behavior_path_planner::utils::path_safety_checker::ObjectsFilteringParams> & params)
{
  // implanted part of behavior_path_planner::utils::path_safety_checker::filterObjects() and
  // createTargetObjectsOnLane()

  // Guard
  if (objects->objects.empty()) {
    return {};
  }

  const double ignore_object_velocity_threshold = params->ignore_object_velocity_threshold;
  const auto & target_object_types = params->object_types_to_check;

  PredictedObjects filtered_objects = utils::path_safety_checker::filterObjectsByVelocity(
    *objects, ignore_object_velocity_threshold, true);

  utils::path_safety_checker::filterObjectsByClass(filtered_objects, target_object_types);

  std::vector<PredictedObject> within_filtered_objects;
  for (const auto & target_lane : target_lanes) {
    const auto lane_poly = target_lane.polygon2d().basicPolygon();
    for (const auto & filtered_object : filtered_objects.objects) {
      const auto object_bbox = autoware::universe_utils::toPolygon2d(filtered_object);
      if (boost::geometry::within(object_bbox, lane_poly)) {
        within_filtered_objects.push_back(filtered_object);
      }
    }
  }

  const double safety_check_time_horizon = params->safety_check_time_horizon;
  const double safety_check_time_resolution = params->safety_check_time_resolution;

  std::vector<utils::path_safety_checker::ExtendedPredictedObject> refined_filtered_objects;
  for (const auto & within_filtered_object : within_filtered_objects) {
    refined_filtered_objects.push_back(utils::path_safety_checker::transform(
      within_filtered_object, safety_check_time_horizon, safety_check_time_resolution));
  }
  return refined_filtered_objects;
}

std::pair<bool, utils::path_safety_checker::CollisionCheckDebugMap> GoalPlannerModule::isSafePath(
  const std::shared_ptr<const PlannerData> planner_data, const bool found_pull_over_path,
  const std::optional<PullOverPath> & pull_over_path_opt, const PathDecisionState & prev_data,
  const GoalPlannerParameters & parameters,
  const std::shared_ptr<EgoPredictedPathParams> & ego_predicted_path_params,
  const std::shared_ptr<ObjectsFilteringParams> & objects_filtering_params,
  const std::shared_ptr<SafetyCheckParams> & safety_check_params) const
{
  using autoware::behavior_path_planner::utils::path_safety_checker::createPredictedPath;
  using autoware::behavior_path_planner::utils::path_safety_checker::
    filterPredictedPathAfterTargetPose;
  CollisionCheckDebugMap collision_check{};

  if (!found_pull_over_path || !pull_over_path_opt) {
    return {false, collision_check};
  }
  const auto & pull_over_path = pull_over_path_opt.value();
  const auto & current_pull_over_path = pull_over_path.getCurrentPath();
  const auto & current_pose = planner_data->self_odometry->pose.pose;
  const double current_velocity = std::hypot(
    planner_data->self_odometry->twist.twist.linear.x,
    planner_data->self_odometry->twist.twist.linear.y);
  const auto & dynamic_object = planner_data->dynamic_object;
  const auto & route_handler = planner_data->route_handler;
  const lanelet::ConstLanelets current_lanes = utils::getExtendedCurrentLanes(
    planner_data, parameters.backward_goal_search_length, parameters.forward_goal_search_length,
    /*forward_only_in_route*/ false);
  const auto pull_over_lanes = goal_planner_utils::getPullOverLanes(
    *route_handler, left_side_parking_, parameters.backward_goal_search_length,
    parameters.forward_goal_search_length);
  const std::pair<double, double> terminal_velocity_and_accel =
    pull_over_path.getPairsTerminalVelocityAndAccel();
  RCLCPP_DEBUG(
    getLogger(), "pairs_terminal_velocity_and_accel for goal_planner: %f, %f",
    terminal_velocity_and_accel.first, terminal_velocity_and_accel.second);
  auto temp_param = std::make_shared<EgoPredictedPathParams>(*ego_predicted_path_params);
  utils::parking_departure::updatePathProperty(temp_param, terminal_velocity_and_accel);
  // TODO(Sugahara): shoule judge is_object_front properly
  const bool is_object_front = true;
  const bool limit_to_max_velocity = true;
  const auto ego_seg_idx = planner_data->findEgoIndex(current_pull_over_path.points);
  const auto ego_predicted_path_from_current_pose = createPredictedPath(
    ego_predicted_path_params, current_pull_over_path.points, current_pose, current_velocity,
    ego_seg_idx, is_object_front, limit_to_max_velocity);
  const auto ego_predicted_path = filterPredictedPathAfterTargetPose(
    ego_predicted_path_from_current_pose, pull_over_path.start_pose());

  // ==========================================================================================
  // if ego is before the entry of pull_over_lanes, the beginning of the safety check area
  // should be from the entry of pull_over_lanes
  // ==========================================================================================
  const Pose ego_pose_for_expand = std::invoke([&]() {
    // get first road lane in pull over lanes segment
    const auto fist_road_lane = std::invoke([&]() {
      const auto first_pull_over_lane = pull_over_lanes.front();
      if (!route_handler->isShoulderLanelet(first_pull_over_lane)) {
        return first_pull_over_lane;
      }
      const auto road_lane_opt = left_side_parking_
                                   ? route_handler->getRightLanelet(first_pull_over_lane)
                                   : route_handler->getLeftLanelet(first_pull_over_lane);
      if (road_lane_opt) {
        return road_lane_opt.value();
      }
      return first_pull_over_lane;
    });
    // generate first road lane pose
    Pose first_road_pose{};
    const auto first_road_point =
      lanelet::utils::conversion::toGeomMsgPt(fist_road_lane.centerline().front());
    const double lane_yaw = lanelet::utils::getLaneletAngle(fist_road_lane, first_road_point);
    first_road_pose.position = first_road_point;
    first_road_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(lane_yaw);
    // if current ego pose is before pull over lanes segment, use first road lanelet center pose
    if (
      calcSignedArcLength(
        current_pull_over_path.points, first_road_pose.position, current_pose.position) < 0.0) {
      return first_road_pose;
    }
    // if current ego pose is in pull over lanes segment, use current ego pose
    return current_pose;
  });

  // filtering objects based on the current position's lane
  const auto expanded_pull_over_lanes_between_ego =
    goal_planner_utils::generateBetweenEgoAndExpandedPullOverLanes(
      pull_over_lanes, left_side_parking_, ego_pose_for_expand,
      planner_data->parameters.vehicle_info, parameters.outer_road_detection_offset,
      parameters.inner_road_detection_offset);
  const auto merged_expanded_pull_over_lanes =
    lanelet::utils::combineLaneletsShape(expanded_pull_over_lanes_between_ego);
  debug_data_.expanded_pull_over_lane_between_ego = merged_expanded_pull_over_lanes;

  const auto filtered_objects = filterObjectsByWithinPolicy(
    dynamic_object, {merged_expanded_pull_over_lanes}, objects_filtering_params);

  const double hysteresis_factor =
    prev_data.is_stable_safe ? 1.0 : parameters.hysteresis_factor_expand_rate;

  const bool current_is_safe = std::invoke([&]() {
    if (parameters.safety_check_params.method == "RSS") {
      return autoware::behavior_path_planner::utils::path_safety_checker::checkSafetyWithRSS(
        current_pull_over_path, ego_predicted_path, filtered_objects, collision_check,
        planner_data->parameters, safety_check_params->rss_params,
        objects_filtering_params->use_all_predicted_path, hysteresis_factor,
        safety_check_params->collision_check_yaw_diff_threshold);
    }
    if (parameters.safety_check_params.method == "integral_predicted_polygon") {
      return utils::path_safety_checker::checkSafetyWithIntegralPredictedPolygon(
        ego_predicted_path, vehicle_info_, filtered_objects,
        objects_filtering_params->check_all_predicted_path,
        parameters.safety_check_params.integral_predicted_polygon_params, collision_check);
    }
    RCLCPP_ERROR(
      getLogger(), " [pull_over] invalid safety check method: %s",
      parameters.safety_check_params.method.c_str());
    throw std::domain_error("[pull_over] invalid safety check method");
  });

  /*
   *                      ==== is_safe
   *                      ---- current_is_safe
   *    is_safe
   *     |
   *     |                   time
   *   1 +--+    +---+       +---=========   +--+
   *     |  |    |   |       |           |   |  |
   *     |  |    |   |       |           |   |  |
   *     |  |    |   |       |           |   |  |
   *     |  |    |   |       |           |   |  |
   *   0 =========================-------==========-- t
   */
  return {current_is_safe, collision_check};
}

void GoalPlannerModule::setDebugData(const PullOverContextData & context_data)
{
  universe_utils::ScopedTimeTrack st(__func__, *time_keeper_);

  debug_marker_.markers.clear();

  using autoware::motion_utils::createStopVirtualWallMarker;
  using autoware::universe_utils::createDefaultMarker;
  using autoware::universe_utils::createMarkerColor;
  using autoware::universe_utils::createMarkerScale;
  using marker_utils::createObjectsMarkerArray;
  using marker_utils::createPathMarkerArray;
  using marker_utils::createPoseMarkerArray;
  using marker_utils::createPredictedPathMarkerArray;
  using marker_utils::showPolygon;
  using marker_utils::showPredictedPath;
  using marker_utils::showSafetyCheckInfo;

  const auto header = planner_data_->route_handler->getRouteHeader();

  const auto add = [this](MarkerArray added) {
    for (auto & marker : added.markers) {
      marker.lifetime = rclcpp::Duration::from_seconds(1.5);
    }
    autoware::universe_utils::appendMarkerArray(added, &debug_marker_);
  };
  if (utils::isAllowedGoalModification(planner_data_->route_handler)) {
    // Visualize pull over areas
    const auto color = path_decision_controller_.get_current_state().state ==
                           PathDecisionState::DecisionKind::DECIDED
                         ? createMarkerColor(1.0, 1.0, 0.0, 0.999)   // yellow
                         : createMarkerColor(0.0, 1.0, 0.0, 0.999);  // green
    const double z = planner_data_->route_handler->getGoalPose().position.z;
    add(goal_planner_utils::createPullOverAreaMarkerArray(
      goal_searcher_->getAreaPolygons(), header, color, z));

    // Visualize goal candidates
    add(goal_planner_utils::createGoalCandidatesMarkerArray(goal_candidates_, color));

    // Visualize objects extraction polygon
    auto marker = autoware::universe_utils::createDefaultMarker(
      "map", rclcpp::Clock{RCL_ROS_TIME}.now(), "objects_extraction_polygon", 0, Marker::LINE_LIST,
      autoware::universe_utils::createMarkerScale(0.1, 0.0, 0.0),
      autoware::universe_utils::createMarkerColor(0.0, 1.0, 1.0, 0.999));
    const double ego_z = planner_data_->self_odometry->pose.pose.position.z;
    for (size_t i = 0; i < debug_data_.objects_extraction_polygon.outer().size(); ++i) {
      const auto & current_point = debug_data_.objects_extraction_polygon.outer().at(i);
      const auto & next_point = debug_data_.objects_extraction_polygon.outer().at(
        (i + 1) % debug_data_.objects_extraction_polygon.outer().size());
      marker.points.push_back(
        autoware::universe_utils::createPoint(current_point.x(), current_point.y(), ego_z));
      marker.points.push_back(
        autoware::universe_utils::createPoint(next_point.x(), next_point.y(), ego_z));
    }

    debug_marker_.markers.push_back(marker);
  }

  // Visualize previous module output
  add(createPathMarkerArray(
    getPreviousModuleOutput().path, "previous_module_path", 0, 1.0, 0.0, 0.0));

  // Visualize path and related pose
  if (context_data.pull_over_path_opt) {
    const auto & pull_over_path = context_data.pull_over_path_opt.value();
    add(
      createPoseMarkerArray(pull_over_path.start_pose(), "pull_over_start_pose", 0, 0.3, 0.3, 0.9));
    add(createPoseMarkerArray(
      pull_over_path.modified_goal_pose(), "pull_over_end_pose", 0, 0.3, 0.3, 0.9));
    add(createPathMarkerArray(pull_over_path.full_path(), "full_path", 0, 0.0, 0.5, 0.9));
    add(createPathMarkerArray(pull_over_path.getCurrentPath(), "current_path", 0, 0.9, 0.5, 0.0));

    // visualize each partial path
    for (size_t i = 0; i < context_data.pull_over_path_opt.value().partial_paths().size(); ++i) {
      const auto & partial_path = context_data.pull_over_path_opt.value().partial_paths().at(i);
      add(
        createPathMarkerArray(partial_path, "partial_path_" + std::to_string(i), 0, 0.9, 0.5, 0.9));
    }

    auto marker = autoware::universe_utils::createDefaultMarker(
      "map", rclcpp::Clock{RCL_ROS_TIME}.now(), "detection_polygons", 0, Marker::LINE_LIST,
      autoware::universe_utils::createMarkerScale(0.01, 0.0, 0.0),
      autoware::universe_utils::createMarkerColor(0.0, 0.0, 1.0, 0.999));
    const double ego_z = planner_data_->self_odometry->pose.pose.position.z;
    for (const auto & ego_polygon : debug_data_.ego_polygons_expanded) {
      for (size_t ep_idx = 0; ep_idx < ego_polygon.outer().size(); ++ep_idx) {
        const auto & current_point = ego_polygon.outer().at(ep_idx);
        const auto & next_point = ego_polygon.outer().at((ep_idx + 1) % ego_polygon.outer().size());

        marker.points.push_back(
          autoware::universe_utils::createPoint(current_point.x(), current_point.y(), ego_z));
        marker.points.push_back(
          autoware::universe_utils::createPoint(next_point.x(), next_point.y(), ego_z));
      }
    }
    debug_marker_.markers.push_back(marker);

    if (parameters_->safety_check_params.enable_safety_check) {
      autoware::universe_utils::appendMarkerArray(
        goal_planner_utils::createLaneletPolygonMarkerArray(
          debug_data_.expanded_pull_over_lane_between_ego.polygon3d(), header,
          "expanded_pull_over_lane_between_ego",
          autoware::universe_utils::createMarkerColor(1.0, 0.7, 0.0, 0.999)),
        &debug_marker_);
    }

    // Visualize debug poses
    const auto & debug_poses = pull_over_path.debug_poses;
    for (size_t i = 0; i < debug_poses.size(); ++i) {
      add(createPoseMarkerArray(
        debug_poses.at(i), "debug_pose_" + std::to_string(i), 0, 0.3, 0.3, 0.3));
    }
  }

  // safety check
  if (parameters_->safety_check_params.enable_safety_check) {
    if (goal_planner_data_.ego_predicted_path.size() > 0) {
      const auto & ego_predicted_path = utils::path_safety_checker::convertToPredictedPath(
        goal_planner_data_.ego_predicted_path, ego_predicted_path_params_->time_resolution);
      add(createPredictedPathMarkerArray(
        ego_predicted_path, vehicle_info_, "ego_predicted_path_goal_planner", 0, 0.0, 0.5, 0.9));
    }
    if (goal_planner_data_.filtered_objects.objects.size() > 0) {
      add(createObjectsMarkerArray(
        goal_planner_data_.filtered_objects, "filtered_objects", 0, 0.0, 0.5, 0.9));
    }

    auto collision_check = debug_data_.collision_check;
    if (parameters_->safety_check_params.method == "RSS") {
      add(showSafetyCheckInfo(collision_check, "object_debug_info"));
    }
    add(showPredictedPath(collision_check, "ego_predicted_path"));
    add(showPolygon(collision_check, "ego_and_target_polygon_relation"));

    // set objects of interest
    for (const auto & [uuid, data] : collision_check) {
      const auto color = data.is_safe ? ColorName::GREEN : ColorName::RED;
      setObjectsOfInterestData(data.current_obj_pose, data.obj_shape, color);
    }

    // TODO(Mamoru Sobue): it is not clear where ThreadSafeData::collision_check should be cleared
    utils::parking_departure::initializeCollisionCheckDebugMap(collision_check);

    // visualize safety status maker
    {
      const auto & prev_data = context_data.prev_state_for_debug;
      visualization_msgs::msg::MarkerArray marker_array{};
      const auto color = prev_data.is_stable_safe ? createMarkerColor(1.0, 1.0, 1.0, 0.99)
                                                  : createMarkerColor(1.0, 0.0, 0.0, 0.99);
      auto marker = createDefaultMarker(
        header.frame_id, header.stamp, "safety_status", 0,
        visualization_msgs::msg::Marker::TEXT_VIEW_FACING, createMarkerScale(0.0, 0.0, 1.0), color);

      marker.pose = planner_data_->self_odometry->pose.pose;
      marker.text += "is_safe: " + std::to_string(prev_data.is_stable_safe) + "\n";
      if (prev_data.safe_start_time) {
        const double elapsed_time_from_safe_start =
          (clock_->now() - prev_data.safe_start_time.value()).seconds();
        marker.text +=
          "elapsed_time_from_safe_start: " + std::to_string(elapsed_time_from_safe_start) + "\n";
      }
      marker_array.markers.push_back(marker);
      add(marker_array);
    }
  }

  // Visualize planner type text
  {
    visualization_msgs::msg::MarkerArray planner_type_marker_array{};
    const auto color = context_data.pull_over_path_opt ? createMarkerColor(1.0, 1.0, 1.0, 0.99)
                                                       : createMarkerColor(1.0, 0.0, 0.0, 0.99);
    auto marker = createDefaultMarker(
      header.frame_id, header.stamp, "planner_type", 0,
      visualization_msgs::msg::Marker::TEXT_VIEW_FACING, createMarkerScale(0.0, 0.0, 1.0), color);
    marker.pose = context_data.pull_over_path_opt
                    ? context_data.pull_over_path_opt.value().modified_goal_pose()
                    : planner_data_->self_odometry->pose.pose;
    if (context_data.pull_over_path_opt) {
      const auto & pull_over_path = context_data.pull_over_path_opt.value();
      marker.text = magic_enum::enum_name(pull_over_path.type());
      marker.text += " " + std::to_string(pull_over_path.path_idx()) + "/" +
                     std::to_string(pull_over_path.partial_paths().size() - 1);
    }

    /*
      TODO(soblin): disable until thread safe design is done
    if (isStuck(
          context_data.static_target_objects, context_data.dynamic_target_objects, planner_data_,
          occupancy_grid_map_, *parameters_)) {
      marker.text += " stuck";
    }
    if (isStopped()) {
      marker.text += " stopped";
    }
    */

    if (debug_data_.freespace_planner.is_planning) {
      marker.text +=
        " freespace: " + std::to_string(debug_data_.freespace_planner.current_goal_idx) + "/" +
        std::to_string(debug_data_.freespace_planner.num_goal_candidates);
    }

    planner_type_marker_array.markers.push_back(marker);
    add(planner_type_marker_array);
  }

  // Visualize stop pose info
  if (debug_stop_pose_with_info_.pose->has_value()) {
    visualization_msgs::msg::MarkerArray stop_pose_marker_array{};
    const auto color = createMarkerColor(1.0, 1.0, 1.0, 0.99);
    auto marker = createDefaultMarker(
      header.frame_id, header.stamp, "stop_pose_info", 0,
      visualization_msgs::msg::Marker::TEXT_VIEW_FACING, createMarkerScale(0.0, 0.0, 0.5), color);
    marker.pose = debug_stop_pose_with_info_.pose->value();
    marker.text = debug_stop_pose_with_info_.string;
    stop_pose_marker_array.markers.push_back(marker);
    add(stop_pose_marker_array);
    add(createPoseMarkerArray(marker.pose, "stop_pose", 1.0, 1.0, 1.0, 0.9));
  }
}

void GoalPlannerModule::GoalPlannerData::initializeOccupancyGridMap(
  const PlannerData & planner_data, const GoalPlannerParameters & parameters)
{
  OccupancyGridMapParam occupancy_grid_map_param{};
  const double margin = parameters.occupancy_grid_collision_check_margin;
  occupancy_grid_map_param.vehicle_shape.length =
    planner_data.parameters.vehicle_length + 2 * margin;
  occupancy_grid_map_param.vehicle_shape.width = planner_data.parameters.vehicle_width + 2 * margin;
  occupancy_grid_map_param.vehicle_shape.base2back =
    planner_data.parameters.base_link2rear + margin;
  occupancy_grid_map_param.theta_size = parameters.theta_size;
  occupancy_grid_map_param.obstacle_threshold = parameters.obstacle_threshold;
  occupancy_grid_map = std::make_shared<OccupancyGridBasedCollisionDetector>();
  occupancy_grid_map->setParam(occupancy_grid_map_param);
}

GoalPlannerModule::GoalPlannerData GoalPlannerModule::GoalPlannerData::clone() const
{
  GoalPlannerModule::GoalPlannerData gp_planner_data(
    planner_data, parameters, last_previous_module_output);
  gp_planner_data.update(
    parameters, planner_data, current_status, previous_module_output, vehicle_footprint,
    goal_candidates);
  return gp_planner_data;
}

void GoalPlannerModule::GoalPlannerData::update(
  const GoalPlannerParameters & parameters_, const PlannerData & planner_data_,
  const ModuleStatus & current_status_, const BehaviorModuleOutput & previous_module_output_,
  const autoware::universe_utils::LinearRing2d & vehicle_footprint_,
  const GoalCandidates & goal_candidates_)
{
  parameters = parameters_;
  vehicle_footprint = vehicle_footprint_;

  planner_data = planner_data_;
  planner_data.route_handler = std::make_shared<RouteHandler>(*(planner_data_.route_handler));
  current_status = current_status_;
  last_previous_module_output = previous_module_output;
  previous_module_output = previous_module_output_;
  occupancy_grid_map->setMap(*(planner_data.occupancy_grid));
  goal_candidates = goal_candidates_;
}

}  // namespace autoware::behavior_path_planner

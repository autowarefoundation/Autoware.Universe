// Copyright 2023 TIER IV, Inc.
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

#include "autoware/behavior_path_lane_change_module/interface.hpp"

#include "autoware/behavior_path_lane_change_module/utils/markers.hpp"
#include "autoware/behavior_path_lane_change_module/utils/utils.hpp"
#include "autoware/behavior_path_planner_common/interface/scene_module_interface.hpp"
#include "autoware/behavior_path_planner_common/interface/scene_module_visitor.hpp"
#include "autoware/behavior_path_planner_common/marker_utils/utils.hpp"

#include <autoware/universe_utils/ros/marker_helper.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>

#include <algorithm>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace autoware::behavior_path_planner
{
using autoware::universe_utils::appendMarkerArray;
using utils::lane_change::assignToCandidate;

LaneChangeInterface::LaneChangeInterface(
  const std::string & name, rclcpp::Node & node, std::shared_ptr<LaneChangeParameters> parameters,
  const std::unordered_map<std::string, std::shared_ptr<RTCInterface>> & rtc_interface_ptr_map,
  std::unordered_map<std::string, std::shared_ptr<ObjectsOfInterestMarkerInterface>> &
    objects_of_interest_marker_interface_ptr_map,
  std::unique_ptr<LaneChangeBase> && module_type)
: SceneModuleInterface{name, node, rtc_interface_ptr_map, objects_of_interest_marker_interface_ptr_map},  // NOLINT
  parameters_{std::move(parameters)},
  module_type_{std::move(module_type)}
{
  module_type_->setTimeKeeper(getTimeKeeper());
  logger_ = utils::lane_change::getLogger(module_type_->getModuleTypeStr());
  steering_factor_interface_.init(PlanningBehavior::LANE_CHANGE);
  velocity_factor_interface_.init(PlanningBehavior::LANE_CHANGE);
}

void LaneChangeInterface::processOnExit()
{
  module_type_->resetParameters();
  debug_marker_.markers.clear();
  post_process_safety_status_ = {};
  resetPathCandidate();
}

bool LaneChangeInterface::isExecutionRequested() const
{
  if (getCurrentStatus() == ModuleStatus::RUNNING) {
    return true;
  }

  return module_type_->isLaneChangeRequired();
}

bool LaneChangeInterface::isExecutionReady() const
{
  return module_type_->isSafe() && !module_type_->isAbortState();
}

void LaneChangeInterface::updateData()
{
  universe_utils::ScopedTimeTrack st(__func__, *getTimeKeeper());
  module_type_->setPreviousModuleOutput(getPreviousModuleOutput());
  module_type_->update_lanes(getCurrentStatus() == ModuleStatus::RUNNING);
  module_type_->update_filtered_objects();
  module_type_->update_transient_data(getCurrentStatus() == ModuleStatus::RUNNING);
  module_type_->updateSpecialData();

  if (isWaitingApproval() || module_type_->isAbortState()) {
    module_type_->updateLaneChangeStatus();
  }

  module_type_->resetStopPose();
  updateDebugMarker();
}

void LaneChangeInterface::postProcess()
{
  if (getCurrentStatus() == ModuleStatus::RUNNING) {
    const auto safety_status = module_type_->isApprovedPathSafe();
    post_process_safety_status_ =
      module_type_->evaluateApprovedPathWithUnsafeHysteresis(safety_status);
  }
  updateDebugMarker();
}

BehaviorModuleOutput LaneChangeInterface::plan()
{
  universe_utils::ScopedTimeTrack st(__func__, *getTimeKeeper());
  resetPathCandidate();
  resetPathReference();

  auto output = module_type_->generateOutput();
  path_reference_ = std::make_shared<PathWithLaneId>(output.reference_path);

  stop_pose_ = module_type_->getStopPose();

  const auto & lane_change_debug = module_type_->getDebugData();
  for (const auto & [uuid, data] : lane_change_debug.collision_check_objects_after_approval) {
    const auto color = data.is_safe ? ColorName::GREEN : ColorName::RED;
    setObjectsOfInterestData(data.current_obj_pose, data.obj_shape, color);
  }

  updateSteeringFactorPtr(output);
  if (module_type_->isAbortState()) {
    const auto candidate = planCandidate();
    path_candidate_ = std::make_shared<PathWithLaneId>(candidate.path_candidate);
    updateRTCStatus(
      std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), true,
      State::ABORTING);
  } else {
    const auto path =
      assignToCandidate(module_type_->getLaneChangePath(), module_type_->getEgoPosition());
    const auto is_registered = std::any_of(
      rtc_interface_ptr_map_.begin(), rtc_interface_ptr_map_.end(),
      [&](const auto & rtc) { return rtc.second->isRegistered(uuid_map_.at(rtc.first)); });

    if (!is_registered) {
      updateRTCStatus(
        path.start_distance_to_path_change, path.finish_distance_to_path_change, true,
        State::WAITING_FOR_EXECUTION);
    } else {
      const auto force_activated = std::any_of(
        rtc_interface_ptr_map_.begin(), rtc_interface_ptr_map_.end(),
        [&](const auto & rtc) { return rtc.second->isForceActivated(uuid_map_.at(rtc.first)); });
      updateRTCStatus(
        path.start_distance_to_path_change, path.finish_distance_to_path_change, !force_activated,
        State::RUNNING);
    }
  }

  setVelocityFactor(output.path);

  return output;
}

BehaviorModuleOutput LaneChangeInterface::planWaitingApproval()
{
  BehaviorModuleOutput out = module_type_->getTerminalLaneChangePath();

  module_type_->insert_stop_point(module_type_->get_current_lanes(), out.path, true);
  out.turn_signal_info = module_type_->get_current_turn_signal_info();

  const auto & lane_change_debug = module_type_->getDebugData();
  for (const auto & [uuid, data] : lane_change_debug.collision_check_objects) {
    const auto color = data.is_safe ? ColorName::GREEN : ColorName::RED;
    setObjectsOfInterestData(data.current_obj_pose, data.obj_shape, color);
  }

  path_reference_ = std::make_shared<PathWithLaneId>(out.reference_path);
  stop_pose_ = module_type_->getStopPose();

  if (!module_type_->isValidPath()) {
    path_candidate_ = std::make_shared<PathWithLaneId>();
    return out;
  }

  const auto candidate = planCandidate();
  path_candidate_ = std::make_shared<PathWithLaneId>(candidate.path_candidate);

  updateRTCStatus(
    candidate.start_distance_to_path_change, candidate.finish_distance_to_path_change,
    isExecutionReady(), State::WAITING_FOR_EXECUTION);
  is_abort_path_approved_ = false;

  setVelocityFactor(out.path);

  return out;
}

CandidateOutput LaneChangeInterface::planCandidate() const
{
  universe_utils::ScopedTimeTrack st(__func__, *getTimeKeeper());
  const auto selected_path = module_type_->getLaneChangePath();

  if (selected_path.path.points.empty()) {
    return {};
  }

  CandidateOutput output = assignToCandidate(selected_path, module_type_->getEgoPosition());

  updateSteeringFactorPtr(output, selected_path);
  return output;
}

void LaneChangeInterface::updateModuleParams(const std::any & parameters)
{
  parameters_ = std::any_cast<std::shared_ptr<LaneChangeParameters>>(parameters);
}

void LaneChangeInterface::setData(const std::shared_ptr<const PlannerData> & data)
{
  planner_data_ = data;
  module_type_->setData(data);
}

bool LaneChangeInterface::canTransitSuccessState()
{
  auto log_debug_throttled = [&](std::string_view message) -> void {
    RCLCPP_DEBUG(getLogger(), "%s", message.data());
  };
  updateDebugMarker();

  if (module_type_->specialExpiredCheck() && isWaitingApproval()) {
    log_debug_throttled("Run specialExpiredCheck.");
    return true;
  }

  if (module_type_->hasFinishedLaneChange()) {
    module_type_->resetParameters();
    log_debug_throttled("Lane change process has completed.");
    return true;
  }

  log_debug_throttled("Lane changing process is ongoing");
  return false;
}

bool LaneChangeInterface::canTransitFailureState()
{
  const auto force_activated = std::any_of(
    rtc_interface_ptr_map_.begin(), rtc_interface_ptr_map_.end(),
    [&](const auto & rtc) { return rtc.second->isForceActivated(uuid_map_.at(rtc.first)); });

  if (force_activated) {
    RCLCPP_WARN_THROTTLE(getLogger(), *clock_, 5000, "unsafe but force executed");
    return false;
  }

  const auto [state, reason] = check_transit_failure();

  interface_debug_.failing_reason = reason;
  interface_debug_.lc_state = state;

  updateDebugMarker();

  if (state == LaneChangeStates::Cancel) {
    updateRTCStatus(
      std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), true,
      State::FAILED);
    module_type_->toCancelState();
    return true;
  }

  if (state == LaneChangeStates::Abort) {
    module_type_->toAbortState();
    return false;
  }

  // Note: Ideally, if it is unsafe, but for some reason, we can't abort or cancel, then we should
  // stop. Note: This feature is not working properly for now.
  const auto [is_safe, unsafe_trailing_obj] = post_process_safety_status_;
  if (!is_safe && module_type_->isRequiredStop(unsafe_trailing_obj)) {
    module_type_->toStopState();
    return false;
  }

  module_type_->toNormalState();
  return false;
}

std::pair<LaneChangeStates, std::string_view> LaneChangeInterface::check_transit_failure()
{
  if (module_type_->isAbortState()) {
    if (module_type_->hasFinishedAbort()) {
      return {LaneChangeStates::Cancel, "Aborted"};
    }
    return {LaneChangeStates::Abort, "Aborting"};
  }

  if (isWaitingApproval()) {
    if (module_type_->is_near_regulatory_element()) {
      return {LaneChangeStates::Cancel, "CloseToRegElement"};
    }
    return {LaneChangeStates::Normal, "WaitingForApproval"};
  }

  if (!module_type_->isValidPath()) {
    return {LaneChangeStates::Cancel, "InvalidPath"};
  }

  const auto is_preparing = module_type_->isEgoOnPreparePhase();
  const auto can_return_to_current = module_type_->isAbleToReturnCurrentLane();

  // regardless of safe and unsafe, we want to cancel lane change.
  if (is_preparing) {
    const auto force_deactivated = std::any_of(
      rtc_interface_ptr_map_.begin(), rtc_interface_ptr_map_.end(),
      [&](const auto & rtc) { return rtc.second->isForceDeactivated(uuid_map_.at(rtc.first)); });

    if (force_deactivated && can_return_to_current) {
      return {LaneChangeStates::Cancel, "ForceDeactivation"};
    }
  }

  if (post_process_safety_status_.is_safe) {
    return {LaneChangeStates::Normal, "SafeToLaneChange"};
  }

  if (!module_type_->isCancelEnabled()) {
    return {LaneChangeStates::Warning, "CancelDisabled"};
  }

  // We also check if the ego can return to the current lane, as prepare segment might be out of the
  // lane, for example, during an evasive maneuver around a static object.
  if (is_preparing && can_return_to_current) {
    return {LaneChangeStates::Cancel, "SafeToCancel"};
  }

  if (module_type_->is_near_terminal()) {
    return {LaneChangeStates::Warning, "TooNearTerminal"};
  }

  if (!module_type_->isAbortEnabled()) {
    return {LaneChangeStates::Warning, "AbortDisabled"};
  }

  // To prevent the lane module from having to check rear objects in the current lane, we limit the
  // abort maneuver to cases where the ego vehicle is still in the current lane.
  if (!can_return_to_current) {
    return {LaneChangeStates::Warning, "TooLateToAbort"};
  }

  const auto found_abort_path = module_type_->calcAbortPath();
  if (!found_abort_path) {
    return {LaneChangeStates::Warning, "AbortPathNotFound"};
  }

  return {LaneChangeStates::Abort, "SafeToAbort"};
}

void LaneChangeInterface::updateDebugMarker() const
{
  debug_marker_.markers.clear();
  if (!parameters_->publish_debug_marker) {
    return;
  }
  using marker_utils::lane_change_markers::createDebugMarkerArray;
  debug_marker_ = createDebugMarkerArray(
    interface_debug_, module_type_->getDebugData(), module_type_->getEgoPose());
}

MarkerArray LaneChangeInterface::getModuleVirtualWall()
{
  using marker_utils::lane_change_markers::createLaneChangingVirtualWallMarker;
  MarkerArray marker;

  if (!parameters_->publish_debug_marker) {
    return marker;
  }

  if (isWaitingApproval() || getCurrentStatus() != ModuleStatus::RUNNING) {
    return marker;
  }
  const auto & start_pose = module_type_->getLaneChangePath().info.lane_changing_start;
  const auto start_marker =
    createLaneChangingVirtualWallMarker(start_pose, name(), clock_->now(), "lane_change_start");

  const auto & end_pose = module_type_->getLaneChangePath().info.lane_changing_end;
  const auto end_marker =
    createLaneChangingVirtualWallMarker(end_pose, name(), clock_->now(), "lane_change_end");
  marker.markers.reserve(start_marker.markers.size() + end_marker.markers.size());
  appendMarkerArray(start_marker, &marker);
  appendMarkerArray(end_marker, &marker);
  return marker;
}

void LaneChangeInterface::updateSteeringFactorPtr(const BehaviorModuleOutput & output)
{
  universe_utils::ScopedTimeTrack st(__func__, *getTimeKeeper());
  const auto steering_factor_direction = std::invoke([&]() {
    if (module_type_->getDirection() == Direction::LEFT) {
      return SteeringFactor::LEFT;
    }
    if (module_type_->getDirection() == Direction::RIGHT) {
      return SteeringFactor::RIGHT;
    }
    return SteeringFactor::UNKNOWN;
  });

  const auto current_position = module_type_->getEgoPosition();
  const auto status = module_type_->getLaneChangeStatus();
  const auto start_distance = autoware::motion_utils::calcSignedArcLength(
    output.path.points, current_position, status.lane_change_path.info.shift_line.start.position);
  const auto finish_distance = autoware::motion_utils::calcSignedArcLength(
    output.path.points, current_position, status.lane_change_path.info.shift_line.end.position);

  steering_factor_interface_.set(
    {status.lane_change_path.info.shift_line.start, status.lane_change_path.info.shift_line.end},
    {start_distance, finish_distance}, steering_factor_direction, SteeringFactor::TURNING, "");
}

void LaneChangeInterface::updateSteeringFactorPtr(
  const CandidateOutput & output, const LaneChangePath & selected_path) const
{
  const uint16_t steering_factor_direction = std::invoke([&output]() {
    if (output.lateral_shift > 0.0) {
      return SteeringFactor::LEFT;
    }
    return SteeringFactor::RIGHT;
  });

  steering_factor_interface_.set(
    {selected_path.info.shift_line.start, selected_path.info.shift_line.end},
    {output.start_distance_to_path_change, output.finish_distance_to_path_change},
    steering_factor_direction, SteeringFactor::APPROACHING, "");
}
}  // namespace autoware::behavior_path_planner

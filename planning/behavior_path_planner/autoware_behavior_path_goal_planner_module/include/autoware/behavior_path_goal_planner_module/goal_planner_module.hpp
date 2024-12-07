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

#ifndef AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__GOAL_PLANNER_MODULE_HPP_
#define AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__GOAL_PLANNER_MODULE_HPP_

#include "autoware/behavior_path_goal_planner_module/decision_state.hpp"
#include "autoware/behavior_path_goal_planner_module/fixed_goal_planner_base.hpp"
#include "autoware/behavior_path_goal_planner_module/goal_planner_parameters.hpp"
#include "autoware/behavior_path_goal_planner_module/goal_searcher.hpp"
#include "autoware/behavior_path_goal_planner_module/thread_data.hpp"
#include "autoware/behavior_path_planner_common/interface/scene_module_interface.hpp"
#include "autoware/behavior_path_planner_common/utils/parking_departure/common_module_data.hpp"
#include "autoware/behavior_path_planner_common/utils/path_safety_checker/path_safety_checker_parameters.hpp"

#include <autoware/lane_departure_checker/lane_departure_checker.hpp>

#include <autoware_vehicle_msgs/msg/hazard_lights_command.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <lanelet2_core/Forward.h>

#include <atomic>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware::behavior_path_planner
{
using autoware::lane_departure_checker::LaneDepartureChecker;
using autoware_vehicle_msgs::msg::HazardLightsCommand;
using geometry_msgs::msg::PoseArray;
using nav_msgs::msg::OccupancyGrid;
using visualization_msgs::msg::Marker;
using visualization_msgs::msg::MarkerArray;

using autoware::freespace_planning_algorithms::AbstractPlanningAlgorithm;
using autoware::freespace_planning_algorithms::AstarParam;
using autoware::freespace_planning_algorithms::AstarSearch;
using autoware::freespace_planning_algorithms::PlannerCommonParam;

using autoware::behavior_path_planner::utils::path_safety_checker::EgoPredictedPathParams;
using autoware::behavior_path_planner::utils::path_safety_checker::ObjectsFilteringParams;
using autoware::behavior_path_planner::utils::path_safety_checker::PoseWithVelocityStamped;
using autoware::behavior_path_planner::utils::path_safety_checker::SafetyCheckParams;
using autoware::behavior_path_planner::utils::path_safety_checker::TargetObjectsOnLane;
using autoware::universe_utils::Polygon2d;

struct FreespacePlannerDebugData
{
  bool is_planning{false};
  size_t current_goal_idx{0};
  size_t num_goal_candidates{0};
};

struct GoalPlannerDebugData
{
  FreespacePlannerDebugData freespace_planner{};
  std::vector<Polygon2d> ego_polygons_expanded{};
  lanelet::ConstLanelet expanded_pull_over_lane_between_ego{};
  Polygon2d objects_extraction_polygon{};
  utils::path_safety_checker::CollisionCheckDebugMap collision_check{};
};

struct LastApprovalData
{
  LastApprovalData(rclcpp::Time time, Pose pose) : time(time), pose(pose) {}

  rclcpp::Time time{};
  Pose pose{};
};

// store stop_pose_ pointer with reason string
struct PoseWithString
{
  std::optional<Pose> * pose;
  std::string string;

  explicit PoseWithString(std::optional<Pose> * shared_pose) : pose(shared_pose), string("") {}

  void set(const Pose & new_pose, const std::string & new_string)
  {
    *pose = new_pose;
    string = new_string;
  }

  void set(const std::string & new_string) { string = new_string; }

  void clear()
  {
    pose->reset();
    string = "";
  }
};

struct PullOverContextData
{
  PullOverContextData() = delete;
  explicit PullOverContextData(
    const bool is_stable_safe_path, const PredictedObjects & static_objects,
    const PredictedObjects & dynamic_objects, std::optional<PullOverPath> && pull_over_path_opt,
    const std::vector<PullOverPath> & pull_over_path_candidates,
    const PathDecisionState & prev_state)
  : is_stable_safe_path(is_stable_safe_path),
    static_target_objects(static_objects),
    dynamic_target_objects(dynamic_objects),
    pull_over_path_opt(pull_over_path_opt),
    pull_over_path_candidates(pull_over_path_candidates),
    prev_state_for_debug(prev_state),
    last_path_idx_increment_time(std::nullopt)
  {
  }
  const bool is_stable_safe_path;
  const PredictedObjects static_target_objects;
  const PredictedObjects dynamic_target_objects;
  // TODO(soblin): due to deceleratePath(), this cannot be const member(velocity is modified by it)
  std::optional<PullOverPath> pull_over_path_opt;
  const std::vector<PullOverPath> pull_over_path_candidates;
  // TODO(soblin): goal_candidate_from_thread, modifed_goal_pose_from_thread, closest_start_pose
  const PathDecisionState prev_state_for_debug;
  std::optional<rclcpp::Time> last_path_idx_increment_time;
};

bool isOnModifiedGoal(
  const Pose & current_pose, const std::optional<GoalCandidate> & modified_goal_opt,
  const GoalPlannerParameters & parameters);

bool hasPreviousModulePathShapeChanged(
  const BehaviorModuleOutput & previous_module_output,
  const BehaviorModuleOutput & last_previous_module_output);
bool hasDeviatedFromLastPreviousModulePath(
  const PlannerData & planner_data, const BehaviorModuleOutput & last_previous_module_output);
bool hasDeviatedFromCurrentPreviousModulePath(
  const PlannerData & planner_data, const BehaviorModuleOutput & previous_module_output);

bool needPathUpdate(
  const Pose & current_pose, const double path_update_duration, const rclcpp::Time & now,
  const std::optional<GoalCandidate> & modified_goal,
  const std::optional<rclcpp::Time> & selected_time, const GoalPlannerParameters & parameters);

bool checkOccupancyGridCollision(
  const PathWithLaneId & path,
  const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map);

bool isStopped(
  std::deque<nav_msgs::msg::Odometry::ConstSharedPtr> & odometry_buffer,
  const nav_msgs::msg::Odometry::ConstSharedPtr self_odometry, const double duration_lower,
  const double velocity_upper);

// Flag class for managing whether a certain callback is running in multi-threading
class ScopedFlag
{
public:
  explicit ScopedFlag(std::atomic<bool> & flag) : flag_(flag) { flag_.store(true); }

  ~ScopedFlag() { flag_.store(false); }

private:
  std::atomic<bool> & flag_;
};

class GoalPlannerModule : public SceneModuleInterface
{
public:
  GoalPlannerModule(
    const std::string & name, rclcpp::Node & node,
    const std::shared_ptr<GoalPlannerParameters> & parameters,
    const std::unordered_map<std::string, std::shared_ptr<RTCInterface>> & rtc_interface_ptr_map,
    std::unordered_map<std::string, std::shared_ptr<ObjectsOfInterestMarkerInterface>> &
      objects_of_interest_marker_interface_ptr_map);

  ~GoalPlannerModule()
  {
    if (lane_parking_timer_) {
      lane_parking_timer_->cancel();
    }
    if (freespace_parking_timer_) {
      freespace_parking_timer_->cancel();
    }

    while (is_lane_parking_cb_running_.load() || is_freespace_parking_cb_running_.load()) {
      const std::string running_callbacks = std::invoke([&]() {
        if (is_lane_parking_cb_running_ && is_freespace_parking_cb_running_) {
          return "lane parking and freespace parking";
        }
        if (is_lane_parking_cb_running_) {
          return "lane parking";
        }
        return "freespace parking";
      });
      RCLCPP_INFO_THROTTLE(
        getLogger(), *clock_, 1000, "Waiting for %s callback to finish...",
        running_callbacks.c_str());
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    RCLCPP_INFO_THROTTLE(
      getLogger(), *clock_, 1000, "lane parking and freespace parking callbacks finished");
  }

  void updateModuleParams(const std::any & parameters) override
  {
    parameters_ = std::any_cast<std::shared_ptr<GoalPlannerParameters>>(parameters);
    if (parameters_->safety_check_params.enable_safety_check) {
      ego_predicted_path_params_ =
        std::make_shared<EgoPredictedPathParams>(parameters_->ego_predicted_path_params);
      objects_filtering_params_ =
        std::make_shared<ObjectsFilteringParams>(parameters_->objects_filtering_params);
      safety_check_params_ = std::make_shared<SafetyCheckParams>(parameters_->safety_check_params);
    }
  }

  BehaviorModuleOutput plan() override;
  BehaviorModuleOutput planWaitingApproval() override;
  bool isExecutionRequested() const override;
  bool isExecutionReady() const override;
  void processOnExit() override;
  void updateData() override;

  void postProcess() override;
  void setParameters(const std::shared_ptr<GoalPlannerParameters> & parameters);
  void acceptVisitor(
    [[maybe_unused]] const std::shared_ptr<SceneModuleVisitor> & visitor) const override
  {
  }
  CandidateOutput planCandidate() const override { return CandidateOutput{}; }

private:
  /**
   * @brief shared data for onTimer(onTimer/onFreespaceParkingTimer just read this)
   */
  struct GoalPlannerData
  {
    GoalPlannerData(
      const PlannerData & planner_data, const GoalPlannerParameters & parameters,
      const BehaviorModuleOutput & previous_module_output_)
    {
      initializeOccupancyGridMap(planner_data, parameters);
      previous_module_output = previous_module_output_;
      last_previous_module_output = previous_module_output_;
    };
    GoalPlannerParameters parameters;
    autoware::universe_utils::LinearRing2d vehicle_footprint;

    PlannerData planner_data;
    ModuleStatus current_status;
    BehaviorModuleOutput previous_module_output;
    BehaviorModuleOutput last_previous_module_output;  //<! previous "previous_module_output"
    GoalCandidates goal_candidates;  //<! only the positional information of goal_candidates

    // collision detector
    // need to be shared_ptr to be used in planner and goal searcher
    std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map;

    const BehaviorModuleOutput & getPreviousModuleOutput() const { return previous_module_output; }
    const ModuleStatus & getCurrentStatus() const { return current_status; }
    void updateOccupancyGrid();
    GoalPlannerData clone() const;
    void update(
      const GoalPlannerParameters & parameters, const PlannerData & planner_data,
      const ModuleStatus & current_status, const BehaviorModuleOutput & previous_module_output,
      const autoware::universe_utils::LinearRing2d & vehicle_footprint,
      const GoalCandidates & goal_candidates);

  private:
    void initializeOccupancyGridMap(
      const PlannerData & planner_data, const GoalPlannerParameters & parameters);
  };
  std::optional<GoalPlannerData> gp_planner_data_{std::nullopt};
  std::mutex gp_planner_data_mutex_;

  /*
   * state transitions and plan function used in each state
   *
   * +--------------------------+
   * | RUNNING                  |
   * | planPullOverAsCandidate()|
   * +------------+-------------+
   *              | hasDecidedPath()
   *  2           v
   * +--------------------------+
   * | WAITING_APPROVAL         |
   * | planPullOverAsCandidate()|
   * +------------+-------------+
   *              | isActivated()
   *  3           v
   * +--------------------------+
   * | RUNNING                  |
   * | planPullOverAsOutput()   |
   * +--------------------------+
   */

  // The start_planner activates when it receives a new route,
  // so there is no need to terminate the goal planner.
  // If terminating it, it may switch to lane following and could generate an inappropriate path.
  bool canTransitSuccessState() override { return false; }
  bool canTransitFailureState() override { return false; }

  mutable StartGoalPlannerData goal_planner_data_;

  std::shared_ptr<GoalPlannerParameters> parameters_;

  mutable std::shared_ptr<EgoPredictedPathParams> ego_predicted_path_params_;
  mutable std::shared_ptr<ObjectsFilteringParams> objects_filtering_params_;
  mutable std::shared_ptr<SafetyCheckParams> safety_check_params_;

  autoware::vehicle_info_utils::VehicleInfo vehicle_info_{};

  // planner
  std::vector<std::shared_ptr<PullOverPlannerBase>> pull_over_planners_;
  std::unique_ptr<PullOverPlannerBase> freespace_planner_;
  std::unique_ptr<FixedGoalPlannerBase> fixed_goal_planner_;

  // goal searcher
  std::shared_ptr<GoalSearcherBase> goal_searcher_;
  GoalCandidates goal_candidates_{};

  // NOTE: this is latest occupancy_grid_map pointer which the local planner_data on
  // onFreespaceParkingTimer thread storage may point to while calculation.
  // onTimer/onFreespaceParkingTimer and their callees MUST NOT use this
  std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map_{nullptr};

  // check stopped and stuck state
  std::deque<nav_msgs::msg::Odometry::ConstSharedPtr> odometry_buffer_stopped_;
  std::deque<nav_msgs::msg::Odometry::ConstSharedPtr> odometry_buffer_stuck_;

  autoware::universe_utils::LinearRing2d vehicle_footprint_;

  std::recursive_mutex mutex_;
  mutable ThreadSafeData thread_safe_data_;

  // TODO(soblin): organize part of thread_safe_data and previous data to PullOverContextData
  // context_data_ is initialized in updateData(), used in plan() and refreshed in postProcess()
  std::optional<PullOverContextData> context_data_{std::nullopt};
  // path_decision_controller is updated in updateData(), and used in plan()
  PathDecisionStateController path_decision_controller_{getLogger()};
  std::unique_ptr<LastApprovalData> last_approval_data_{nullptr};

  // approximate distance from the start point to the end point of pull_over.
  // this is used as an assumed value to decelerate, etc., before generating the actual path.
  const double approximate_pull_over_distance_{20.0};
  // ego may exceed the stop distance, so add a buffer
  const double stop_distance_buffer_{2.0};

  // for parking policy
  bool left_side_parking_{true};

  // pre-generate lane parking paths in a separate thread
  rclcpp::TimerBase::SharedPtr lane_parking_timer_;
  rclcpp::CallbackGroup::SharedPtr lane_parking_timer_cb_group_;
  std::atomic<bool> is_lane_parking_cb_running_;

  // generate freespace parking paths in a separate thread
  rclcpp::TimerBase::SharedPtr freespace_parking_timer_;
  rclcpp::CallbackGroup::SharedPtr freespace_parking_timer_cb_group_;
  std::atomic<bool> is_freespace_parking_cb_running_;

  // debug
  mutable GoalPlannerDebugData debug_data_;
  mutable PoseWithString debug_stop_pose_with_info_;

  // goal seach
  GoalCandidates generateGoalCandidates() const;

  // stop or decelerate
  void deceleratePath(PullOverPath & pull_over_path) const;
  void decelerateForTurnSignal(const Pose & stop_pose, PathWithLaneId & path) const;
  void decelerateBeforeSearchStart(
    const Pose & search_start_offset_pose, PathWithLaneId & path) const;
  PathWithLaneId generateStopPath(const PullOverContextData & context_data) const;
  PathWithLaneId generateFeasibleStopPath(const PathWithLaneId & path) const;

  void keepStoppedWithCurrentPath(
    const PullOverContextData & ctx_data, PathWithLaneId & path) const;
  double calcSignedArcLengthFromEgo(const PathWithLaneId & path, const Pose & pose) const;

  // status
  bool hasFinishedCurrentPath(const PullOverContextData & ctx_data);
  double calcModuleRequestLength() const;
  bool isStuck(
    const PredictedObjects & static_target_objects, const PredictedObjects & dynamic_target_objects,
    const std::shared_ptr<const PlannerData> planner_data,
    const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map,
    const GoalPlannerParameters & parameters);
  void decideVelocity();
  void updateStatus(const BehaviorModuleOutput & output);

  // validation
  bool hasEnoughDistance(
    const PullOverPath & pull_over_path, const PathWithLaneId & long_tail_reference_path) const;
  bool isCrossingPossible(
    const lanelet::ConstLanelet & start_lane, const lanelet::ConstLanelet & end_lane) const;
  bool isCrossingPossible(
    const Pose & start_pose, const Pose & end_pose, const lanelet::ConstLanelets lanes) const;
  bool isCrossingPossible(const PullOverPath & pull_over_path) const;
  bool hasEnoughTimePassedSincePathUpdate(const double duration) const;

  // freespace parking
  bool planFreespacePath(
    std::shared_ptr<const PlannerData> planner_data,
    const std::shared_ptr<GoalSearcherBase> goal_searcher, const GoalCandidates & goal_candidates,
    const std::shared_ptr<OccupancyGridBasedCollisionDetector> occupancy_grid_map,
    const PredictedObjects & static_target_objects);
  bool canReturnToLaneParking(const PullOverContextData & context_data);

  // plan pull over path
  BehaviorModuleOutput planPullOver(const PullOverContextData & context_data);
  BehaviorModuleOutput planPullOverAsOutput(const PullOverContextData & context_data);
  BehaviorModuleOutput planPullOverAsCandidate(const PullOverContextData & context_data);
  std::optional<PullOverPath> selectPullOverPath(
    const PullOverContextData & context_data,
    const std::vector<PullOverPath> & pull_over_path_candidates,
    const GoalCandidates & goal_candidates) const;

  // lanes and drivable area
  std::vector<DrivableLanes> generateDrivableLanes() const;
  void setDrivableAreaInfo(
    const PullOverContextData & context_data, BehaviorModuleOutput & output) const;

  // output setter
  void setOutput(const PullOverContextData & context_data, BehaviorModuleOutput & output);

  void setModifiedGoal(
    const PullOverContextData & context_data, BehaviorModuleOutput & output) const;
  void setTurnSignalInfo(const PullOverContextData & context_data, BehaviorModuleOutput & output);

  // new turn signal
  TurnSignalInfo calcTurnSignalInfo(const PullOverContextData & context_data);
  std::optional<lanelet::Id> ignore_signal_{std::nullopt};

  // timer for generating pull over path candidates in a separate thread
  void onTimer();
  void onFreespaceParkingTimer();

  // steering factor
  void updateSteeringFactor(
    const PullOverContextData & context_data, const std::array<Pose, 2> & pose,
    const std::array<double, 2> distance, const uint16_t type);

  // rtc
  std::pair<double, double> calcDistanceToPathChange(
    const PullOverContextData & context_data) const;

  // safety check
  void initializeSafetyCheckParameters();
  SafetyCheckParams createSafetyCheckParams() const;
  /*
  void updateSafetyCheckTargetObjectsData(
    const PredictedObjects & filtered_objects, const TargetObjectsOnLane & target_objects_on_lane,
    const std::vector<PoseWithVelocityStamped> & ego_predicted_path) const;
  */
  /**
   * @brief Checks if the current path is safe.
   * @return If the path is safe in the current state, true.
   */
  std::pair<bool, utils::path_safety_checker::CollisionCheckDebugMap> isSafePath(
    const std::shared_ptr<const PlannerData> planner_data, const bool found_pull_over_path,
    const std::optional<PullOverPath> & pull_over_path_opt, const PathDecisionState & prev_data,
    const GoalPlannerParameters & parameters,
    const std::shared_ptr<EgoPredictedPathParams> & ego_predicted_path_params,
    const std::shared_ptr<ObjectsFilteringParams> & objects_filtering_params,
    const std::shared_ptr<SafetyCheckParams> & safety_check_params) const;

  // debug
  void setDebugData(const PullOverContextData & context_data);
  void printParkingPositionError() const;
};
}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_GOAL_PLANNER_MODULE__GOAL_PLANNER_MODULE_HPP_

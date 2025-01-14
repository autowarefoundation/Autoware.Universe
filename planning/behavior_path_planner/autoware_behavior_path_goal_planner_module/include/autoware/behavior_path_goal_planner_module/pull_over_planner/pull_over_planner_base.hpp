// Copyright 2022 TIER IV, Inc.
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

#pragma once

#include "autoware/behavior_path_goal_planner_module/goal_planner_parameters.hpp"
#include "autoware/behavior_path_goal_planner_module/goal_searcher_base.hpp"
#include "autoware/behavior_path_planner_common/data_manager.hpp"

#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <memory>
#include <utility>
#include <vector>

using autoware::universe_utils::LinearRing2d;
using geometry_msgs::msg::Pose;
using tier4_planning_msgs::msg::PathWithLaneId;

namespace autoware::behavior_path_planner
{
enum class PullOverPlannerType {
  SHIFT,
  BEZIER,
  ARC_FORWARD,
  ARC_BACKWARD,
  FREESPACE,
};

struct PullOverPath
{
public:
  static std::optional<PullOverPath> create(
    const PullOverPlannerType & type, const size_t id,
    const std::vector<PathWithLaneId> & partial_paths, const Pose & start_pose,
    const GoalCandidate & modified_goal_pose,
    const std::vector<std::pair<double, double>> & pairs_terminal_velocity_and_accel);

  PullOverPath(const PullOverPath & other);
  PullOverPath & operator=(const PullOverPath & other) = default;

  PullOverPlannerType type() const { return type_; }
  size_t goal_id() const { return modified_goal_pose_.id; }
  size_t id() const { return id_; }
  Pose start_pose() const { return start_pose_; }
  Pose modified_goal_pose() const { return modified_goal_pose_.goal_pose; }
  const GoalCandidate & modified_goal() const { return modified_goal_pose_; }

  std::vector<PathWithLaneId> & partial_paths() { return partial_paths_; }
  const std::vector<PathWithLaneId> & partial_paths() const { return partial_paths_; }

  // TODO(soblin): use reference to avoid copy once thread-safe design is finished
  const PathWithLaneId & full_path() const { return full_path_; }
  PathWithLaneId parking_path() const { return parking_path_; }
  const std::vector<double> & full_path_curvatures() const { return full_path_curvatures_; }
  const std::vector<double> & parking_path_curvatures() const { return parking_path_curvatures_; }
  double full_path_max_curvature() const { return full_path_max_curvature_; }
  double parking_path_max_curvature() const { return parking_path_max_curvature_; }
  size_t path_idx() const { return path_idx_; }

  bool incrementPathIndex();

  // TODO(soblin): this cannot be const due to decelerateBeforeSearchStart
  PathWithLaneId & getCurrentPath();

  const PathWithLaneId & getCurrentPath() const;

  std::pair<double, double> getPairsTerminalVelocityAndAccel() const
  {
    if (pairs_terminal_velocity_and_accel_.size() <= path_idx_) {
      return std::make_pair(0.0, 0.0);
    }
    return pairs_terminal_velocity_and_accel_.at(path_idx_);
  }

  std::vector<Pose> debug_poses{};

private:
  PullOverPath(
    const PullOverPlannerType & type, const size_t id, const Pose & start_pose,
    const GoalCandidate & modified_goal_pose, const std::vector<PathWithLaneId> & partial_paths,
    const PathWithLaneId & full_path, const PathWithLaneId & parking_path,
    const std::vector<double> & full_path_curvatures,
    const std::vector<double> & parking_path_curvatures, const double full_path_max_curvature,
    const double parking_path_max_curvature,
    const std::vector<std::pair<double, double>> & pairs_terminal_velocity_and_accel);

  PullOverPlannerType type_;
  GoalCandidate modified_goal_pose_;
  size_t id_;
  Pose start_pose_;

  std::vector<PathWithLaneId> partial_paths_;
  PathWithLaneId full_path_;
  PathWithLaneId parking_path_;
  std::vector<double> full_path_curvatures_;
  std::vector<double> parking_path_curvatures_;
  double full_path_max_curvature_;
  double parking_path_max_curvature_;

  // accelerate with constant acceleration to the target velocity
  size_t path_idx_;
  std::vector<std::pair<double, double>> pairs_terminal_velocity_and_accel_;
};

class PullOverPlannerBase
{
public:
  PullOverPlannerBase(rclcpp::Node & node, const GoalPlannerParameters & parameters)
  : vehicle_info_{autoware::vehicle_info_utils::VehicleInfoUtils(node).getVehicleInfo()},
    vehicle_footprint_{vehicle_info_.createFootprint()},
    parameters_{parameters}
  {
  }
  virtual ~PullOverPlannerBase() = default;

  virtual PullOverPlannerType getPlannerType() const = 0;
  virtual std::optional<PullOverPath> plan(
    const GoalCandidate & modified_goal_pose, const size_t id,
    const std::shared_ptr<const PlannerData> planner_data,
    const BehaviorModuleOutput & upstream_module_output) = 0;

protected:
  const autoware::vehicle_info_utils::VehicleInfo vehicle_info_;
  const LinearRing2d vehicle_footprint_;
  const GoalPlannerParameters parameters_;
};
}  // namespace autoware::behavior_path_planner

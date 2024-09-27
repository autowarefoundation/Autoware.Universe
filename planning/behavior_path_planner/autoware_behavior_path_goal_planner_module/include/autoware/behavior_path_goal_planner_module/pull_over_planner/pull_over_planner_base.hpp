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
  ARC_FORWARD,
  ARC_BACKWARD,
  FREESPACE,
};

struct PullOverPath
{
public:
  static std::optional<PullOverPath> create(
    const PullOverPlannerType & type, const size_t goal_id, const size_t id,
    const std::vector<PathWithLaneId> & partial_paths, const Pose & start_pose,
    const Pose & end_pose);

  PullOverPath(const PullOverPath & other);

  // ThreadSafeDataのsetterで変更するから全てconstにできない
  PullOverPlannerType type() const { return type_; }
  size_t goal_id() const { return goal_id_; }
  size_t id() const { return id_; }
  Pose start_pose() const { return start_pose_; }
  Pose end_pose() const { return end_pose_; }

  // todo: multithreadでこれを参照するとアウト
  // けどそれだとmultithread以外の所で毎回コピーが発生する
  std::vector<PathWithLaneId> & partial_paths() { return partial_paths_; }
  const std::vector<PathWithLaneId> & partial_paths() const { return partial_paths_; }

  // TODO(soblin): use reference to avoid copy once thread-safe design is finished
  PathWithLaneId full_path() const { return full_path_; }
  PathWithLaneId parking_path() const { return parking_path_; }
  std::vector<double> full_path_curvatures() { return full_path_curvatures_; }
  std::vector<double> parking_path_curvatures() const { return parking_path_curvatures_; }
  double full_path_max_curvature() const { return full_path_max_curvature_; }
  double parking_path_max_curvature() const { return parking_path_max_curvature_; }

  bool incrementPathIndex();

  // todo: decelerateBeforeSearchStartのせいでconstにできない
  PathWithLaneId & getCurrentPath();

  const PathWithLaneId & getCurrentPath() const;

  // accelerate with constant acceleration to the target velocity
  // TODO(soblin): 後でprivatenにする
  size_t path_idx{0};
  std::vector<std::pair<double, double>> pairs_terminal_velocity_and_accel{};
  std::vector<Pose> debug_poses{};

private:
  PullOverPath(
    const PullOverPlannerType & type, const size_t goal_id, const size_t id,
    const Pose & start_pose, const Pose & end_pose,
    const std::vector<PathWithLaneId> & partial_paths, const PathWithLaneId & full_path,
    const PathWithLaneId & parking_path, const std::vector<double> & full_path_curvatures,
    const std::vector<double> & parking_path_curvatures, const double full_path_max_curvature,
    const double parking_path_max_curvature);

  PullOverPlannerType type_;
  size_t goal_id_;
  size_t id_;
  Pose start_pose_;
  Pose end_pose_;

  std::vector<PathWithLaneId> partial_paths_;
  PathWithLaneId full_path_;
  PathWithLaneId parking_path_;
  std::vector<double> full_path_curvatures_;
  std::vector<double> parking_path_curvatures_;
  double full_path_max_curvature_;
  double parking_path_max_curvature_;
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
    const size_t goal_id, const size_t id, const std::shared_ptr<const PlannerData> planner_data,
    const BehaviorModuleOutput & previous_module_output, const Pose & goal_pose) = 0;

protected:
  const autoware::vehicle_info_utils::VehicleInfo vehicle_info_;
  const LinearRing2d vehicle_footprint_;
  const GoalPlannerParameters parameters_;
};
}  // namespace autoware::behavior_path_planner

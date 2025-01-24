// Copyright 2021 TIER IV, Inc.
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

#ifndef AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__STRUCTS__PATH_HPP_
#define AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__STRUCTS__PATH_HPP_

#include "autoware/behavior_path_lane_change_module/structs/data.hpp"
#include "autoware/behavior_path_planner_common/turn_signal_decider.hpp"
#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"

#include <autoware_frenet_planner/structures.hpp>

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>

#include <utility>
#include <vector>

namespace autoware::behavior_path_planner::lane_change
{
enum class PathType { ConstantJerk = 0, FrenetPlanner };

using autoware::behavior_path_planner::TurnSignalInfo;
using autoware_internal_planning_msgs::msg::PathWithLaneId;
struct TrajectoryGroup
{
  PathWithLaneId prepare;
  PathWithLaneId target_lane_ref_path;
  std::vector<double> target_lane_ref_path_dist;
  LaneChangePhaseMetrics prepare_metric;
  frenet_planner::Trajectory lane_changing;
  frenet_planner::FrenetState initial_state;
  double max_lane_changing_length{0.0};

  TrajectoryGroup() = default;
  TrajectoryGroup(
    PathWithLaneId prepare, PathWithLaneId target_lane_ref_path,
    std::vector<double> target_lane_ref_path_dist, LaneChangePhaseMetrics prepare_metric,
    frenet_planner::Trajectory lane_changing, frenet_planner::FrenetState initial_state,
    const double max_lane_changing_length)
  : prepare(std::move(prepare)),
    target_lane_ref_path(std::move(target_lane_ref_path)),
    target_lane_ref_path_dist(std::move(target_lane_ref_path_dist)),
    prepare_metric(prepare_metric),
    lane_changing(std::move(lane_changing)),
    initial_state(initial_state),
    max_lane_changing_length(max_lane_changing_length)
  {
  }
};

struct Path
{
  Info info;
  PathWithLaneId path;
  ShiftedPath shifted_path;
  TrajectoryGroup frenet_path;
  PathType type = PathType::ConstantJerk;
};

struct Status
{
  Path lane_change_path;
  bool is_safe{false};
  bool is_valid_path{false};
};
}  // namespace autoware::behavior_path_planner::lane_change

namespace autoware::behavior_path_planner
{
using LaneChangePath = lane_change::Path;
using LaneChangePaths = std::vector<lane_change::Path>;
using LaneChangeStatus = lane_change::Status;
}  // namespace autoware::behavior_path_planner
#endif  // AUTOWARE__BEHAVIOR_PATH_LANE_CHANGE_MODULE__STRUCTS__PATH_HPP_

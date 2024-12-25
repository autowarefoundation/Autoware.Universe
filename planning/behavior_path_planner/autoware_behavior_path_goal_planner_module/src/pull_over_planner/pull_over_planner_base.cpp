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

#include <autoware/behavior_path_goal_planner_module/pull_over_planner/pull_over_planner_base.hpp>

#include <utility>
#include <vector>

namespace autoware::behavior_path_planner
{

std::optional<PullOverPath> PullOverPath::create(
  const PullOverPlannerType & type, const size_t id,
  const std::vector<PathWithLaneId> & partial_paths, const Pose & start_pose,
  const GoalCandidate & modified_goal_pose,
  const std::vector<std::pair<double, double>> & pairs_terminal_velocity_and_accel)
{
  if (partial_paths.empty()) {
    return std::nullopt;
  }
  PathWithLaneId path{};
  for (size_t i = 0; i < partial_paths.size(); ++i) {
    if (i == 0) {
      path.points.insert(
        path.points.end(), partial_paths.at(i).points.begin(), partial_paths.at(i).points.end());
    } else {
      // skip overlapping point
      path.points.insert(
        path.points.end(), next(partial_paths.at(i).points.begin()),
        partial_paths.at(i).points.end());
    }
  }
  PathWithLaneId full_path{};
  full_path.points = autoware::motion_utils::removeOverlapPoints(path.points);
  if (full_path.points.size() < 3) {
    return std::nullopt;
  }

  const size_t start_idx =
    autoware::motion_utils::findNearestIndex(full_path.points, start_pose.position);

  PathWithLaneId parking_path{};
  std::copy(
    full_path.points.begin() + start_idx, full_path.points.end(),
    std::back_inserter(parking_path.points));

  if (parking_path.points.size() < 3) {
    return std::nullopt;
  }

  const auto calculateCurvaturesAndMax =
    [](const auto & path) -> std::pair<std::vector<double>, double> {
    std::vector<double> curvatures = autoware::motion_utils::calcCurvature(path.points);
    double max_curvature = 0.0;
    if (!curvatures.empty()) {
      max_curvature = std::abs(*std::max_element(
        curvatures.begin(), curvatures.end(),
        [](const double & a, const double & b) { return std::abs(a) < std::abs(b); }));
    }
    return std::make_pair(curvatures, max_curvature);
  };

  std::vector<double> full_path_curvatures{};
  std::vector<double> parking_path_curvatures{};
  double full_path_max_curvature{};
  double parking_path_max_curvature{};
  std::tie(full_path_curvatures, full_path_max_curvature) = calculateCurvaturesAndMax(full_path);
  std::tie(parking_path_curvatures, parking_path_max_curvature) =
    calculateCurvaturesAndMax(parking_path);

  return PullOverPath(
    type, id, start_pose, modified_goal_pose, partial_paths, full_path, parking_path,
    full_path_curvatures, parking_path_curvatures, full_path_max_curvature,
    parking_path_max_curvature, pairs_terminal_velocity_and_accel);
}

PullOverPath::PullOverPath(const PullOverPath & other)
: type_(other.type_),
  modified_goal_pose_(other.modified_goal_pose_),
  id_(other.id_),
  start_pose_(other.start_pose_),
  partial_paths_(other.partial_paths_),
  full_path_(other.full_path_),
  parking_path_(other.parking_path_),
  full_path_curvatures_(other.full_path_curvatures_),
  parking_path_curvatures_(other.parking_path_curvatures_),
  full_path_max_curvature_(other.full_path_max_curvature_),
  parking_path_max_curvature_(other.parking_path_max_curvature_),
  path_idx_(other.path_idx_),
  pairs_terminal_velocity_and_accel_(other.pairs_terminal_velocity_and_accel_)
{
}

PullOverPath::PullOverPath(
  const PullOverPlannerType & type, const size_t id, const Pose & start_pose,
  const GoalCandidate & modified_goal_pose, const std::vector<PathWithLaneId> & partial_paths,
  const PathWithLaneId & full_path, const PathWithLaneId & parking_path,
  const std::vector<double> & full_path_curvatures,
  const std::vector<double> & parking_path_curvatures, const double full_path_max_curvature,
  const double parking_path_max_curvature,
  const std::vector<std::pair<double, double>> & pairs_terminal_velocity_and_accel)
: type_(type),
  modified_goal_pose_(modified_goal_pose),
  id_(id),
  start_pose_(start_pose),
  partial_paths_(partial_paths),
  full_path_(full_path),
  parking_path_(parking_path),
  full_path_curvatures_(full_path_curvatures),
  parking_path_curvatures_(parking_path_curvatures),
  full_path_max_curvature_(full_path_max_curvature),
  parking_path_max_curvature_(parking_path_max_curvature),
  path_idx_(0),
  pairs_terminal_velocity_and_accel_(pairs_terminal_velocity_and_accel)
{
}

bool PullOverPath::incrementPathIndex()
{
  {
    if (partial_paths_.size() - 1 <= path_idx_) {
      return false;
    }
    path_idx_ += 1;
    return true;
  }
}

PathWithLaneId & PullOverPath::getCurrentPath()
{
  if (partial_paths_.size() <= path_idx_) {
    return partial_paths_.back();
  }
  return partial_paths_.at(path_idx_);
}

const PathWithLaneId & PullOverPath::getCurrentPath() const
{
  if (partial_paths_.size() <= path_idx_) {
    return partial_paths_.back();
  }
  return partial_paths_.at(path_idx_);
}

}  // namespace autoware::behavior_path_planner

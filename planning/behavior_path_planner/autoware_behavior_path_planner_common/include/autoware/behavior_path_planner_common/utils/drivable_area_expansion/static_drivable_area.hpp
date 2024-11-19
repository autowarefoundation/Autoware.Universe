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

#ifndef AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__DRIVABLE_AREA_EXPANSION__STATIC_DRIVABLE_AREA_HPP_  // NOLINT
#define AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__DRIVABLE_AREA_EXPANSION__STATIC_DRIVABLE_AREA_HPP_  // NOLINT

#include <autoware/behavior_path_planner_common/utils/utils.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>
namespace autoware::behavior_path_planner::utils
{
using autoware::behavior_path_planner::drivable_area_expansion::DrivableAreaExpansionParameters;

/**
 * @brief finds the index of the first lane in the provided vector that overlaps with a preceding
 * lane (excluding the immediate predecessor in the vector)
 * @param [ink] lanes vector of DrivableLanes
 * @return the index of the first overlapping lane (if any)
 */
std::optional<size_t> getOverlappedLaneletId(const std::vector<DrivableLanes> & lanes);

/**
 * @brief modify a path to only keep points inside the given lanes (before any lane overlap)
 * @details the path point lanelet_ids are used to determine if they are inside a lanelet
 * @param [inout] path path to be cut
 * @param [in] lanes lanes used to cut the path
 * @return the shortened lanes without overlaps
 */
std::vector<DrivableLanes> cutOverlappedLanes(
  PathWithLaneId & path, const std::vector<DrivableLanes> & lanes);

/**
 * @brief generate DrivableLanes objects from a sequence of lanelets
 * @param [in] lanelets sequence of lanelets
 * @return a vector of DrivableLanes constructed from the given lanelets
 */
std::vector<DrivableLanes> generateDrivableLanes(const lanelet::ConstLanelets & lanelets);

std::vector<DrivableLanes> generateDrivableLanesWithShoulderLanes(
  const lanelet::ConstLanelets & current_lanes, const lanelet::ConstLanelets & shoulder_lanes);

std::vector<DrivableLanes> getNonOverlappingExpandedLanes(
  PathWithLaneId & path, const std::vector<DrivableLanes> & lanes,
  const DrivableAreaExpansionParameters & parameters);

/**
 * @brief generate the drivable area of the given path
 * @param [inout] path path whose center line is used to calculate the drivable area and whose
 * left/right bound are generated
 * @param [in] lanes lanes driven by the ego vehicle
 * @param [in] enable_expanding_hatched_road_markings if true, expand the drivable area into hatched
 * road markings
 * @param [in] enable_expanding_intersection_areas if true, expand the drivable area into
 * intersection areas
 * @param [in] enable_expanding_freespace_areas if true, expand the drivable area into freespace
 * areas
 * @param [in] planner_data planner data with the route handler (and map), the parameters, the ego
 * pose, etc
 * @param [in] is_driving_forward true if the ego vehicle drives in the forward direction
 */
void generateDrivableArea(
  PathWithLaneId & path, const std::vector<DrivableLanes> & lanes,
  const bool enable_expanding_hatched_road_markings, const bool enable_expanding_intersection_areas,
  const bool enable_expanding_freespace_areas,
  const std::shared_ptr<const PlannerData> planner_data, const bool is_driving_forward = true);

/**
 * @brief generate the drivable area of the given path by applying the given offsets to its points
 * @param [inout] path path whose center line is used to calculate the drivable area and whose
 * left/right bound are generated
 * @param [in] vehicle_length [m] length of the ego vehicle
 * @param [in] offset [m] lateral offset between the path points and the drivable area
 * @param [in] is_driving_forward true if the ego vehicle drives in the forward direction
 */
void generateDrivableArea(
  PathWithLaneId & path, const double vehicle_length, const double offset,
  const bool is_driving_forward = true);

/**
 * @brief Expand the borders of the given lanelets
 * @param [in] drivable_lanes lanelets to expand
 * @param [in] left_bound_offset [m] expansion distance of the left bound
 * @param [in] right_bound_offset [m] expansion distance of the right bound
 * @param [in] types_to_skip linestring types that will not be expanded
 * @return expanded lanelets
 */
std::vector<DrivableLanes> expandLanelets(
  const std::vector<DrivableLanes> & drivable_lanes, const double left_bound_offset,
  const double right_bound_offset, const std::vector<std::string> & types_to_skip = {});

void extractObstaclesFromDrivableArea(
  PathWithLaneId & path, const std::vector<DrivableAreaInfo::Obstacle> & obstacles);

std::pair<std::vector<lanelet::ConstPoint3d>, bool> getBoundWithFreeSpaceAreas(
  const std::vector<lanelet::ConstPoint3d> & original_bound,
  const std::vector<lanelet::ConstPoint3d> & other_side_bound,
  const std::shared_ptr<const PlannerData> planner_data, const bool is_left);

std::vector<lanelet::ConstPoint3d> getBoundWithHatchedRoadMarkings(
  const std::vector<lanelet::ConstPoint3d> & original_bound,
  const std::shared_ptr<RouteHandler> & route_handler);

/**
 * @brief Expand the given bound to include intersection areas from the map
 * @param [in] original_bound original bound to expand
 * @param [in] drivable_lanes lanelets to consider
 * @param [in] route_handler route handler with the map information
 * @param [in] is_left whether the bound to calculate is on the left or not
 * @return the bound including intersection areas
 */
std::vector<lanelet::ConstPoint3d> getBoundWithIntersectionAreas(
  const std::vector<lanelet::ConstPoint3d> & original_bound,
  const std::shared_ptr<RouteHandler> & route_handler,
  const std::vector<DrivableLanes> & drivable_lanes, const bool is_left);

std::vector<geometry_msgs::msg::Point> calcBound(
  const PathWithLaneId & path, const std::shared_ptr<const PlannerData> planner_data,
  const std::vector<DrivableLanes> & drivable_lanes,
  const bool enable_expanding_hatched_road_markings, const bool enable_expanding_intersection_areas,
  const bool enable_expanding_freespace_areas, const bool is_left,
  const bool is_driving_forward = true);

std::vector<geometry_msgs::msg::Point> postProcess(
  const std::vector<geometry_msgs::msg::Point> & original_bound, const PathWithLaneId & path,
  const std::shared_ptr<const PlannerData> planner_data,
  const std::vector<DrivableLanes> & drivable_lanes, const bool is_left,
  const bool is_driving_forward = true);

/**
 * @brief combine two drivable area info objects
 * @param [in] drivable_area_Info1 first drivable area info
 * @param [in] drivable_area_Info2 second drivable area info
 * @return the combined drivable area info
 */
DrivableAreaInfo combineDrivableAreaInfo(
  const DrivableAreaInfo & drivable_area_info1, const DrivableAreaInfo & drivable_area_info2);

lanelet::ConstLanelets combineLanelets(
  const lanelet::ConstLanelets & base_lanes, const lanelet::ConstLanelets & added_lanes);

std::vector<DrivableLanes> combineDrivableLanes(
  const std::vector<DrivableLanes> & original_drivable_lanes_vec,
  const std::vector<DrivableLanes> & new_drivable_lanes_vec);

}  // namespace autoware::behavior_path_planner::utils

// clang-format off
#endif  // AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__DRIVABLE_AREA_EXPANSION__STATIC_DRIVABLE_AREA_HPP_  // NOLINT
// clang-format on

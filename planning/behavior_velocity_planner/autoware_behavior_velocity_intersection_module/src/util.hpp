// Copyright 2020 Tier IV, Inc.
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

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include "interpolated_path_info.hpp"

#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <rclcpp/logger.hpp>

#include <autoware_perception_msgs/msg/predicted_object_kinematics.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_routing/Forward.h>

#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace autoware::behavior_velocity_planner::util
{

/**
 * @brief insert a new pose to the path and return its index
 * @return if insertion was successful return the inserted point index
 */
std::optional<size_t> insertPointIndex(
  const geometry_msgs::msg::Pose & in_pose, tier4_planning_msgs::msg::PathWithLaneId * inout_path,
  const double ego_nearest_dist_threshold, const double ego_nearest_yaw_threshold);

/**
 * @brief check if a PathPointWithLaneId contains any of the given lane ids
 */
bool hasLaneIds(
  const tier4_planning_msgs::msg::PathPointWithLaneId & p, const std::set<lanelet::Id> & ids);

/**
 * @brief find the first contiguous interval of the path points that contains the specified lane ids
 * @return if no interval is found, return null. if the interval [start, end] (inclusive range) is
 * found, returns the pair (start-1, end)
 */
std::optional<std::pair<size_t, size_t>> findLaneIdsInterval(
  const tier4_planning_msgs::msg::PathWithLaneId & p, const std::set<lanelet::Id> & ids);

/**
 * @brief return the index of the first point which is inside the given polygon
 * @param[in] lane_interval the interval of the path points on the intersection
 * @param[in] search_forward flag for search direction
 */
std::optional<size_t> getFirstPointInsidePolygon(
  const tier4_planning_msgs::msg::PathWithLaneId & path,
  const std::pair<size_t, size_t> lane_interval, const lanelet::CompoundPolygon3d & polygon,
  const bool search_forward = true);

/**
 * @brief check if ego is over the target_idx. If the index is same, compare the exact pose
 * @param[in] path path
 * @param[in] closest_idx ego's closest index on the path
 * @param[in] current_pose ego's exact pose
 * @return true if ego is over the target_idx
 */
bool isOverTargetIndex(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const size_t closest_idx,
  const geometry_msgs::msg::Pose & current_pose, const size_t target_idx);

/**
 * @brief check if ego is before the target_idx. If the index is same, compare the exact pose
 * @param[in] path path
 * @param[in] closest_idx ego's closest index on the path
 * @param[in] current_pose ego's exact pose
 * @return true if ego is over the target_idx
 */
bool isBeforeTargetIndex(
  const tier4_planning_msgs::msg::PathWithLaneId & path, const size_t closest_idx,
  const geometry_msgs::msg::Pose & current_pose, const size_t target_idx);

std::optional<autoware::universe_utils::Polygon2d> getIntersectionArea(
  lanelet::ConstLanelet assigned_lane, lanelet::LaneletMapConstPtr lanelet_map_ptr);

/**
 * @brief check if the given lane has related traffic light
 */
bool hasAssociatedTrafficLight(lanelet::ConstLanelet lane);

/**
 * @brief interpolate PathWithLaneId
 */
std::optional<InterpolatedPathInfo> generateInterpolatedPath(
  const lanelet::Id lane_id, const std::set<lanelet::Id> & associative_lane_ids,
  const tier4_planning_msgs::msg::PathWithLaneId & input_path, const double ds,
  const rclcpp::Logger logger);

geometry_msgs::msg::Pose getObjectPoseWithVelocityDirection(
  const autoware_perception_msgs::msg::PredictedObjectKinematics & obj_state);

/**
 * @brief this function sorts the set of lanelets topologically using topological sort and merges
 * the lanelets from each root to each end. each branch is merged and returned with the original
 * lanelets
 *
 * @param lanelets The set of input lanelets to be processed
 * @param terminal_lanelets The set of lanelets considered as terminal
 * @param routing_graph_ptr Pointer to the routing graph used for determining lanelet
 * connections
 *
 * @return A pair containing:
 *         - First: A vector of merged lanelets, where each element represents a merged lanelet from
 * a branch
 *         - Second: A vector of vectors, where each inner vector contains the original lanelets
 * that were merged to form the corresponding merged lanelet
 */
std::pair<lanelet::ConstLanelets, std::vector<lanelet::ConstLanelets>>
mergeLaneletsByTopologicalSort(
  const lanelet::ConstLanelets & lanelets, const lanelet::ConstLanelets & terminal_lanelets,
  const lanelet::routing::RoutingGraphPtr routing_graph_ptr);

/**
 * @brief Retrieves all paths from the given source to terminal nodes on the tree
 *
 * @param adjacency A 2D vector representing the adjacency matrix of the graph
 * @param src_ind The index of the current source node being processed
 *
 * @return A vector of vectors, where each inner vector represents a path from the source node to a
 * terminal node.
 */
std::vector<std::vector<size_t>> retrievePathsBackward(
  const std::vector<std::vector<bool>> & adjacency, size_t start_node);

/**
 * @brief find the index of the first point where vehicle footprint intersects with the given
 * polygon
 */
std::optional<size_t> getFirstPointInsidePolygonByFootprint(
  const lanelet::CompoundPolygon3d & polygon, const InterpolatedPathInfo & interpolated_path_info,
  const autoware::universe_utils::LinearRing2d & footprint, const double vehicle_length);

/**
 * @brief find the index of the first point where vehicle footprint intersects with the given
 * polygons
 */
std::optional<std::pair<
  size_t /* the index of interpolated PathPoint*/, size_t /* the index of corresponding Polygon */>>
getFirstPointInsidePolygonsByFootprint(
  const std::vector<lanelet::CompoundPolygon3d> & polygons,
  const InterpolatedPathInfo & interpolated_path_info,
  const autoware::universe_utils::LinearRing2d & footprint, const double vehicle_length);

std::vector<lanelet::CompoundPolygon3d> getPolygon3dFromLanelets(
  const lanelet::ConstLanelets & ll_vec);

}  // namespace autoware::behavior_velocity_planner::util

#endif  // UTIL_HPP_

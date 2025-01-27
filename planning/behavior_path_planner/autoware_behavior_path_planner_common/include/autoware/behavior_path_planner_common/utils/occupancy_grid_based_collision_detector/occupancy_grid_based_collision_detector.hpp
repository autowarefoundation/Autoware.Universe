// Copyright 2022 TIER IV, Inc. All rights reserved.
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

#ifndef AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__OCCUPANCY_GRID_BASED_COLLISION_DETECTOR__OCCUPANCY_GRID_BASED_COLLISION_DETECTOR_HPP_
#define AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__OCCUPANCY_GRID_BASED_COLLISION_DETECTOR__OCCUPANCY_GRID_BASED_COLLISION_DETECTOR_HPP_

#include <autoware/universe_utils/geometry/geometry.hpp>

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <geometry_msgs/msg/pose_array.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>

#include <vector>

namespace autoware::behavior_path_planner
{
/**
 * @brief Discretizes a given angle into an index within a specified range.
 * @param theta The angle in radians to discretize.
 * @param theta_size The number of discrete bins or angular intervals.
 * @return The discretized angle as an integer index.
 */
int discretize_angle(const double theta, const int theta_size);

struct IndexXYT
{
  int x;
  int y;
  int theta;
};

struct IndexXY
{
  int x;
  int y;
};

/**
 * @brief Converts a given local pose into a 3D grid index (x, y, theta).
 * @param costmap The occupancy grid used for indexing.
 * @param pose_local The local pose.
 * @param theta_size The number of discrete angular intervals for yaw.
 * @return IndexXYT The grid index representing the position and discretized orientation.
 */
IndexXYT pose2index(
  const nav_msgs::msg::OccupancyGrid & costmap, const geometry_msgs::msg::Pose & pose_local,
  const int theta_size);

/**
 * @brief Converts a grid index (x, y, theta) back into a pose in the local frame.
 * @param costmap The occupancy grid used for indexing.
 * @param index The grid index containing x, y, and theta.
 * @param theta_size The number of discrete angular intervals for yaw.
 * @return geometry_msgs::msg::Pose The corresponding local pose.
 */
geometry_msgs::msg::Pose index2pose(
  const nav_msgs::msg::OccupancyGrid & costmap, const IndexXYT & index, const int theta_size);

/**
 * @brief Transforms a global pose into a local pose relative to the costmap's origin.
 * @param costmap The occupancy grid that defines the local frame.
 * @param pose_global The global pose to transform.
 * @return geometry_msgs::msg::Pose The transformed pose in the local frame.
 */
geometry_msgs::msg::Pose global2local(
  const nav_msgs::msg::OccupancyGrid & costmap, const geometry_msgs::msg::Pose & pose_global);

struct VehicleShape
{
  double length;     // X [m]
  double width;      // Y [m]
  double base2back;  // base_link to rear [m]
};

struct OccupancyGridMapParam
{
  // robot configs
  VehicleShape vehicle_shape;

  // costmap configs
  int theta_size{0};          // discretized angle table size [-]
  int obstacle_threshold{0};  // obstacle threshold on grid [-]
};

struct PlannerWaypoint
{
  geometry_msgs::msg::PoseStamped pose;
  bool is_back = false;
};

class OccupancyGridBasedCollisionDetector
{
public:
  OccupancyGridBasedCollisionDetector() = default;
  OccupancyGridBasedCollisionDetector(const OccupancyGridBasedCollisionDetector &) = default;
  OccupancyGridBasedCollisionDetector(OccupancyGridBasedCollisionDetector &&) = delete;
  OccupancyGridBasedCollisionDetector & operator=(const OccupancyGridBasedCollisionDetector &) =
    default;
  OccupancyGridBasedCollisionDetector & operator=(OccupancyGridBasedCollisionDetector &&) = delete;
  void setParam(const OccupancyGridMapParam & param) { param_ = param; };
  [[nodiscard]] OccupancyGridMapParam getParam() const { return param_; };
  void setMap(const nav_msgs::msg::OccupancyGrid & costmap);
  [[nodiscard]] nav_msgs::msg::OccupancyGrid getMap() const { return costmap_; };

  /**
   * @brief Detects if a collision occurs with given path.
   * @param path The path to check collision defined in a global frame
   * @param check_out_of_range A boolean flag indicating whether out-of-range conditions is
   * collision
   * @return true if a collision is detected, false if no collision is detected.
   */
  [[nodiscard]] bool hasObstacleOnPath(
    const autoware_internal_planning_msgs::msg::PathWithLaneId & path,
    const bool check_out_of_range) const;

  /**
   * @brief Detects if a collision occurs at the specified base index in the occupancy grid map.
   * @param base_index The 3D index (x, y, theta) of the base position in the occupancy grid.
   * @param check_out_of_range A boolean flag indicating whether out-of-range conditions is
   * collision
   * @return true if a collision is detected, false if no collision is detected.
   */
  [[nodiscard]] bool detectCollision(
    const IndexXYT & base_index, const bool check_out_of_range) const;
  virtual ~OccupancyGridBasedCollisionDetector() = default;

protected:
  /**
   * @brief Computes the 2D grid indexes where collision might occur for a given theta index.
   * @param theta_index The discretized orientation index for yaw.
   * @param indexes_2d The output vector of 2D grid indexes where collisions could happen.
   */
  void compute_collision_indexes(int theta_index, std::vector<IndexXY> & indexes);

  [[nodiscard]] inline bool is_out_of_range(const IndexXYT & index) const
  {
    if (index.x < 0 || static_cast<int>(costmap_.info.width) <= index.x) {
      return true;
    }
    if (index.y < 0 || static_cast<int>(costmap_.info.height) <= index.y) {
      return true;
    }
    return false;
  }
  [[nodiscard]] inline bool is_obs(const IndexXYT & index) const
  {
    // NOTE: Accessing by .at() instead makes 1.2 times slower here.
    // Also, boundary check is already done in isOutOfRange before calling this function.
    // So, basically .at() is not necessary.
    return is_obstacle_table_[index.y][index.x];
  }

  OccupancyGridMapParam param_;

  // costmap as occupancy grid
  nav_msgs::msg::OccupancyGrid costmap_;

  // collision indexes cache
  std::vector<std::vector<IndexXY>> coll_indexes_table_;

  // is_obstacle's table
  std::vector<std::vector<bool>> is_obstacle_table_;
};
}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__OCCUPANCY_GRID_BASED_COLLISION_DETECTOR__OCCUPANCY_GRID_BASED_COLLISION_DETECTOR_HPP_

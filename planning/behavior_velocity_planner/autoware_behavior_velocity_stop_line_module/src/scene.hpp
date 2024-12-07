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

#ifndef SCENE_HPP_
#define SCENE_HPP_
#define EIGEN_MPL2_ONLY

#include "autoware/behavior_velocity_planner_common/scene_module_interface.hpp"
#include "autoware/behavior_velocity_planner_common/utilization/util.hpp"
#include "autoware/motion_utils/factor/velocity_factor_interface.hpp"
#include "autoware/trajectory/path_point_with_lane_id.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/time.hpp>

#include <geometry_msgs/msg/pose.hpp>

#include <lanelet2_core/LaneletMap.h>

#include <optional>
#include <utility>

namespace autoware::behavior_velocity_planner
{
class StopLineModule : public SceneModuleInterface
{
public:
  using StopLineWithLaneId = std::pair<lanelet::ConstLineString3d, int64_t>;
  using Trajectory =
    autoware::trajectory::Trajectory<tier4_planning_msgs::msg::PathPointWithLaneId>;
  enum class State { APPROACH, STOPPED, START };

  struct DebugData
  {
    double base_link2front;  ///< Distance from the base link to the vehicle front.
    std::optional<geometry_msgs::msg::Pose> stop_pose;  ///< Pose of the stop position.
  };

  struct PlannerParam
  {
    double stop_margin;        ///< Margin to the stop line.
    double stop_duration_sec;  ///< Required stop duration at the stop line.
    double
      hold_stop_margin_distance;  ///< Distance threshold for transitioning to the STOPPED state
  };

  /**
   * @brief Constructor for StopLineModule.
   * @param module_id Unique ID for the module.
   * @param stop_line Stop line data.
   * @param planner_param Planning parameters.
   * @param logger Logger for output messages.
   * @param clock Shared clock instance.
   */
  StopLineModule(
    const int64_t module_id, lanelet::ConstLineString3d stop_line,
    const PlannerParam & planner_param, const rclcpp::Logger & logger,
    const rclcpp::Clock::SharedPtr clock);

  bool modifyPathVelocity(PathWithLaneId * path) override;

  /**
   * @brief Calculate ego position and stop point.
   * @param trajectory Current trajectory.
   * @param ego_pose Current pose of the vehicle.
   * @param state Current state of the stop line module.
   * @return Pair of ego position and optional stop point.
   */
  std::pair<double, std::optional<double>> getEgoAndStopPoint(
    const Trajectory & trajectory, const geometry_msgs::msg::Pose & ego_pose,
    const State & state) const;

  /**
   * @brief Update the state and stopped time of the module.
   * @param state Pointer to the current state.
   * @param stopped_time Pointer to the stopped time.
   * @param now Current time.
   * @param distance_to_stop_point Distance to the stop point.
   * @param is_vehicle_stopped Flag indicating if the vehicle is stopped.
   */
  void updateStateAndStoppedTime(
    State * state, std::optional<rclcpp::Time> * stopped_time, const rclcpp::Time & now,
    const double & distance_to_stop_point, const bool & is_vehicle_stopped) const;

  static void updateVelocityFactor(
    autoware::motion_utils::VelocityFactorInterface * velocity_factor, const State & state,
    const double & distance_to_stop_point);

  void updateDebugData(
    DebugData * debug_data, const geometry_msgs::msg::Pose & stop_pose, const State & state) const;

  visualization_msgs::msg::MarkerArray createDebugMarkerArray() override
  {
    return visualization_msgs::msg::MarkerArray{};
  }
  autoware::motion_utils::VirtualWalls createVirtualWalls() override;

private:
  const lanelet::ConstLineString3d stop_line_;  ///< Stop line geometry.
  const PlannerParam planner_param_;            ///< Parameters for the planner.
  State state_;                                 ///< Current state of the module.
  std::optional<rclcpp::Time> stopped_time_;    ///< Time when the vehicle stopped.
  DebugData debug_data_;                        ///< Debug information.
};
}  // namespace autoware::behavior_velocity_planner

#endif  // SCENE_HPP_

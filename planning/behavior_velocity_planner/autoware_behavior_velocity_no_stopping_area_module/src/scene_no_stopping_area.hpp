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

#ifndef SCENE_NO_STOPPING_AREA_HPP_
#define SCENE_NO_STOPPING_AREA_HPP_

#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/scene_module_interface.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/boost_geometry_helper.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/state_machine.hpp>
#include <autoware_lanelet2_extension/regulatory_elements/no_stopping_area.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/predicted_object.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <memory>

namespace autoware::behavior_velocity_planner
{
class NoStoppingAreaModule : public SceneModuleInterface
{
public:
  struct PlannerParam
  {
    /**
     * @brief representation of a parameter for no stopping area
     *
     * Ego --|<--stop line margin-->|NoStoppingArea|<--front margin-->|stuck vehicle|---> path
     * Ego --|<--stop line margin-->|NoStoppingArea|<-rear_overhang-->| stop point  |---> path
     *
     */
    double state_clear_time;       //! [s] time to clear stop state
    double stuck_vehicle_vel_thr;  //! [m/s] Threshold of the speed to be recognized as stopped
    double stop_margin;            //! [m] margin to stop line at no stopping area
    double dead_line_margin;       //! [m] dead line to go at no stopping area
    double stop_line_margin;       //! [m] distance from auto-generated stopline to no_stopping_area
    double detection_area_length;  //! [m] used to create detection area polygon
    double stuck_vehicle_front_margin;  //! [m] margin from area end to forward lane
    double path_expand_width;           //! [m] path width to calculate the edge line for both side
  };

  NoStoppingAreaModule(
    const int64_t module_id, const int64_t lane_id,
    const lanelet::autoware::NoStoppingArea & no_stopping_area_reg_elem,
    const PlannerParam & planner_param, const rclcpp::Logger & logger,
    const rclcpp::Clock::SharedPtr clock);

  bool modifyPathVelocity(PathWithLaneId * path) override;

  visualization_msgs::msg::MarkerArray createDebugMarkerArray() override;
  autoware::motion_utils::VirtualWalls createVirtualWalls() override;

private:
  const int64_t lane_id_;

  no_stopping_area::PassJudge pass_judge_;
  StateMachine state_machine_;  //! for state

  /**
   * @brief Check if there is a stopped vehicle in stuck vehicle detect area.
   * @param poly            ego focusing area polygon
   * @param objects_ptr     target objects
   * @return true if exists
   */
  bool check_stuck_vehicles_in_no_stopping_area(
    const Polygon2d & poly,
    const autoware_perception_msgs::msg::PredictedObjects::ConstSharedPtr & predicted_obj_arr_ptr);

  // Key Feature
  const lanelet::autoware::NoStoppingArea & no_stopping_area_reg_elem_;
  std::shared_ptr<const rclcpp::Time> last_obstacle_found_time_;

  // Parameter
  PlannerParam planner_param_;

  // Debug
  no_stopping_area::DebugData debug_data_;
};
}  // namespace autoware::behavior_velocity_planner

#endif  // SCENE_NO_STOPPING_AREA_HPP_

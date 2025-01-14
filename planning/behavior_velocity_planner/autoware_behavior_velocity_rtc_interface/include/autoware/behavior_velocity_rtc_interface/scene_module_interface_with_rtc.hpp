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

#ifndef AUTOWARE__BEHAVIOR_VELOCITY_RTC_INTERFACE__SCENE_MODULE_INTERFACE_WITH_RTC_HPP_
#define AUTOWARE__BEHAVIOR_VELOCITY_RTC_INTERFACE__SCENE_MODULE_INTERFACE_WITH_RTC_HPP_

#include <autoware/behavior_velocity_planner_common/planner_data.hpp>
#include <autoware/behavior_velocity_planner_common/scene_module_interface.hpp>
#include <autoware/rtc_interface/rtc_interface.hpp>
#include <autoware/universe_utils/ros/parameter.hpp>
#include <builtin_interfaces/msg/time.hpp>

#include <autoware_planning_msgs/msg/path.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>
#include <tier4_rtc_msgs/msg/state.hpp>
#include <unique_identifier_msgs/msg/uuid.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Debug
#include <rclcpp/publisher.hpp>
#include <rclcpp/rclcpp.hpp>

namespace autoware::behavior_velocity_planner
{

using autoware::rtc_interface::RTCInterface;
using autoware::universe_utils::getOrDeclareParameter;
using builtin_interfaces::msg::Time;
using tier4_planning_msgs::msg::PathWithLaneId;
using tier4_rtc_msgs::msg::Module;
using tier4_rtc_msgs::msg::State;
using unique_identifier_msgs::msg::UUID;

class SceneModuleInterfaceWithRTC : public SceneModuleInterface
{
public:
  explicit SceneModuleInterfaceWithRTC(
    const int64_t module_id, rclcpp::Logger logger, rclcpp::Clock::SharedPtr clock);
  virtual ~SceneModuleInterfaceWithRTC() = default;

  void setActivation(const bool activated) { activated_ = activated; }
  void setRTCEnabled(const bool enable_rtc) { rtc_enabled_ = enable_rtc; }
  bool isActivated() const { return activated_; }
  bool isSafe() const { return safe_; }
  double getDistance() const { return distance_; }

protected:
  bool activated_;
  bool safe_;
  bool rtc_enabled_;
  double distance_;

  void setSafe(const bool safe)
  {
    safe_ = safe;
    if (!rtc_enabled_) {
      syncActivation();
    }
  }
  void setDistance(const double distance) { distance_ = distance; }
  void syncActivation() { setActivation(isSafe()); }
};

class SceneModuleManagerInterfaceWithRTC
: public SceneModuleManagerInterface<SceneModuleInterfaceWithRTC>
{
public:
  SceneModuleManagerInterfaceWithRTC(
    rclcpp::Node & node, const char * module_name, const bool enable_rtc = true);

  void plan(tier4_planning_msgs::msg::PathWithLaneId * path) override;

protected:
  RTCInterface rtc_interface_;
  std::unordered_map<int64_t, UUID> map_uuid_;

  ObjectsOfInterestMarkerInterface objects_of_interest_marker_interface_;

  virtual void sendRTC(const Time & stamp);

  virtual void setActivation();

  void updateRTCStatus(
    const UUID & uuid, const bool safe, const uint8_t state, const double distance,
    const Time & stamp)
  {
    rtc_interface_.updateCooperateStatus(uuid, safe, state, distance, distance, stamp);
  }

  void removeRTCStatus(const UUID & uuid) { rtc_interface_.removeCooperateStatus(uuid); }

  void publishRTCStatus(const Time & stamp)
  {
    rtc_interface_.removeExpiredCooperateStatus();
    rtc_interface_.publishCooperateStatus(stamp);
  }

  UUID getUUID(const int64_t & module_id) const;

  void generateUUID(const int64_t & module_id);

  void removeUUID(const int64_t & module_id);

  void publishObjectsOfInterestMarker();

  void deleteExpiredModules(const tier4_planning_msgs::msg::PathWithLaneId & path) override;

  static bool getEnableRTC(rclcpp::Node & node, const std::string & param_name)
  {
    bool enable_rtc = true;

    try {
      enable_rtc = getOrDeclareParameter<bool>(node, "enable_all_modules_auto_mode")
                     ? false
                     : getOrDeclareParameter<bool>(node, param_name);
    } catch (const std::exception & e) {
      enable_rtc = getOrDeclareParameter<bool>(node, param_name);
    }

    return enable_rtc;
  }
};

extern template size_t
SceneModuleManagerInterface<SceneModuleInterfaceWithRTC>::findEgoSegmentIndex(
  const std::vector<tier4_planning_msgs::msg::PathPointWithLaneId> & points) const;
extern template void
SceneModuleManagerInterface<SceneModuleInterfaceWithRTC>::updateSceneModuleInstances(
  const std::shared_ptr<const PlannerData> & planner_data,
  const tier4_planning_msgs::msg::PathWithLaneId & path);
extern template void SceneModuleManagerInterface<SceneModuleInterfaceWithRTC>::modifyPathVelocity(
  tier4_planning_msgs::msg::PathWithLaneId * path);
extern template void SceneModuleManagerInterface<SceneModuleInterfaceWithRTC>::registerModule(
  const std::shared_ptr<SceneModuleInterfaceWithRTC> & scene_module);
}  // namespace autoware::behavior_velocity_planner

#endif  // AUTOWARE__BEHAVIOR_VELOCITY_RTC_INTERFACE__SCENE_MODULE_INTERFACE_WITH_RTC_HPP_

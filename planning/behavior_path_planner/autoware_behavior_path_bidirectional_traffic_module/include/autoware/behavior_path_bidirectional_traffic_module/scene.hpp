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

#ifndef AUTOWARE__BEHAVIOR_PATH_BIDIRECTIONAL_TRAFFIC_MODULE__SCENE_HPP_
#define AUTOWARE__BEHAVIOR_PATH_BIDIRECTIONAL_TRAFFIC_MODULE__SCENE_HPP_

#include "autoware/behavior_path_bidirectional_traffic_module/utils.hpp"
#include "autoware/behavior_path_planner_common/interface/scene_module_interface.hpp"

#include <string_view>

namespace autoware::behavior_path_planner
{

class BidirectionalTrafficModule : public SceneModuleInterface
{
public:
  BidirectionalTrafficModule(
    std::string_view name, rclcpp::Node & node,
    const std::unordered_map<std::string, std::shared_ptr<RTCInterface>> & rtc_interface_ptr_map,
    std::unordered_map<std::string, std::shared_ptr<ObjectsOfInterestMarkerInterface>> &
      objects_of_interest_marker_interface_ptr_map);
  CandidateOutput planCandidate() const override;
  BehaviorModuleOutput plan() override;
  BehaviorModuleOutput planWaitingApproval() override;

  bool isExecutionRequested() const override;
  bool isExecutionReady() const override;
  void processOnEntry() override;
  void processOnExit() override;
  void updateData() override;
  void acceptVisitor(const std::shared_ptr<SceneModuleVisitor> & visitor) const override;
  void updateModuleParams(const std::any & parameters) override;
  bool canTransitSuccessState() override;
  bool canTransitFailureState() override;

private:
  bool bidirectional_lane_searched_{
    false};  //!< flag to check if bidirectional lane is searched in the map

  BidirectionalLanes bidirectional_lanes_;  //!< bidirectional lane pairs in the map

  std::vector<std::pair<double, double>>
    bidirectional_lane_intervals_in_trajectory_;  //!< bidirectional lane intervals in the
                                                  //!< trajectory
};

}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_BIDIRECTIONAL_TRAFFIC_MODULE__SCENE_HPP_

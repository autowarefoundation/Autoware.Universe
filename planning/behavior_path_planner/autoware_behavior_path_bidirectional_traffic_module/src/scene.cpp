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
#include "autoware/behavior_path_bidirectional_traffic_module/scene.hpp"

#include "autoware/trajectory/path_point_with_lane_id.hpp"

#include <autoware/trajectory/point.hpp>
#include <rclcpp/logging.hpp>

#include <tier4_planning_msgs/msg/path_point_with_lane_id.hpp>

#include <lanelet2_core/LaneletMap.h>

#include <optional>
#include <set>
#include <vector>

namespace autoware::behavior_path_planner
{

BidirectionalTrafficModule::BidirectionalTrafficModule(
  std::string_view name, rclcpp::Node & node,
  const std::unordered_map<std::string, std::shared_ptr<RTCInterface>> & rtc_interface_ptr_map,
  std::unordered_map<std::string, std::shared_ptr<ObjectsOfInterestMarkerInterface>> &
    objects_of_interest_marker_interface_ptr_map)
: SceneModuleInterface{
    name.data(), node, rtc_interface_ptr_map, objects_of_interest_marker_interface_ptr_map}
{
  RCLCPP_INFO(getLogger(), "BidirectionalTrafficModule::BidirectionalTrafficModule()");
}

CandidateOutput BidirectionalTrafficModule::planCandidate() const
{
  // Implementation will be added here in the future.
  return CandidateOutput{};
}

BehaviorModuleOutput BidirectionalTrafficModule::plan()
{
  using tier4_planning_msgs::msg::PathWithLaneId;

  PathWithLaneId previous_path = getPreviousModuleOutput().path;
  auto trajectory =
    trajectory::Trajectory<tier4_planning_msgs::msg::PathPointWithLaneId>::Builder{}.build(
      previous_path.points);
  if (!trajectory) {
    RCLCPP_ERROR(getLogger(), "Failed to build trajectory");
    return getPreviousModuleOutput();
  }

  // std::optional<double> s = trajectory->find_point(
  //   [](const PathPointWithLaneId & point) { return point.lane_ids == std::vector({1L}); });
  // std::optional<double> s = trajectory::find_point(
  //   trajectory,
  //   [](const PathPointWithLaneId & point) { return point.lane_ids == std::vector({1L}); });

  return getPreviousModuleOutput();
}

BehaviorModuleOutput BidirectionalTrafficModule::planWaitingApproval()
{
  // Implementation will be added here in the future.
  return BehaviorModuleOutput{};
}

bool BidirectionalTrafficModule::isExecutionRequested() const
{
  return !bidirectional_lane_intervals_in_trajectory_.empty();
}

bool BidirectionalTrafficModule::isExecutionReady() const
{
  return bidirectional_lane_searched_;
}

void BidirectionalTrafficModule::processOnEntry()
{
  // Implementation will be added here in the future.
}

void BidirectionalTrafficModule::processOnExit()
{
  // Implementation will be added here in the future.
}

void BidirectionalTrafficModule::updateData()
{
  if (!bidirectional_lane_searched_) {
    const lanelet::LaneletMap & map = *planner_data_->route_handler->getLaneletMapPtr();
    bidirectional_lanes_ = search_bidirectional_lane_from_map(map);
    bidirectional_lane_searched_ = true;
  }

  PathWithLaneId previous_path = getPreviousModuleOutput().path;
  auto trajectory =
    trajectory::Trajectory<tier4_planning_msgs::msg::PathPointWithLaneId>::Builder{}.build(
      previous_path.points);

  if (!trajectory) {
    RCLCPP_ERROR(getLogger(), "Failed to build trajectory");
    return;
  }

  std::set<int64_t> bidirectional_lane_ids_flatten;
  for (const auto & lane_pair : bidirectional_lanes_) {
    bidirectional_lane_ids_flatten.insert(lane_pair.first.id());
    bidirectional_lane_ids_flatten.insert(lane_pair.second.id());
  }

  bidirectional_lane_intervals_in_trajectory_ = trajectory->get_segments([&](double s) {
    auto point = trajectory->compute(s);
    for (const auto & lane_id : point.lane_ids) {
      if (bidirectional_lane_ids_flatten.find(lane_id) != bidirectional_lane_ids_flatten.end()) {
        return true;
      }
    }
    return false;
  });
}

void BidirectionalTrafficModule::acceptVisitor(
  const std::shared_ptr<SceneModuleVisitor> & /* visitor */) const
{
  // Implementation will be added here in the future.
}

void BidirectionalTrafficModule::updateModuleParams(const std::any & /* parameters */)
{
  // Implementation will be added here in the future.
}

bool BidirectionalTrafficModule::canTransitSuccessState()
{
  // Implementation will be added here in the
  return false;
}

bool BidirectionalTrafficModule::canTransitFailureState()
{
  // Implementation will be added here in the
  return false;
}

}  // namespace autoware::behavior_path_planner

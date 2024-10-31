// Copyright 2024 TIER IV, inc.
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

#ifndef MAP_BASED_PREDICTION__PREDICTOR_VRU_HPP_
#define MAP_BASED_PREDICTION__PREDICTOR_VRU_HPP_

#include "map_based_prediction/data_structure.hpp"
#include "map_based_prediction/path_generator.hpp"

#include <autoware/universe_utils/system/time_keeper.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/tracked_objects.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group_array.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_traffic_rules/TrafficRules.h>

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace autoware::map_based_prediction
{
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjectKinematics;
using autoware_perception_msgs::msg::TrackedObject;
using autoware_perception_msgs::msg::TrackedObjectKinematics;
using autoware_perception_msgs::msg::TrafficLightElement;
using autoware_perception_msgs::msg::TrafficLightGroup;
using autoware_perception_msgs::msg::TrafficLightGroupArray;

class PredictorVru
{
public:
  explicit PredictorVru(rclcpp::Node & node) : node_(node) {}
  ~PredictorVru() = default;

  void setParameters(
    bool match_lost_and_appeared_crosswalk_users, double min_crosswalk_user_velocity,
    double max_crosswalk_user_delta_yaw_threshold_for_lanelet, bool use_crosswalk_signal,
    double threshold_velocity_assumed_as_stopping,
    const std::vector<double> & distance_set_for_no_intention_to_walk,
    const std::vector<double> & timeout_set_for_no_intention_to_walk,
    double prediction_sampling_time_interval, double prediction_time_horizon)
  {
    match_lost_and_appeared_crosswalk_users_ = match_lost_and_appeared_crosswalk_users;
    min_crosswalk_user_velocity_ = min_crosswalk_user_velocity;
    max_crosswalk_user_delta_yaw_threshold_for_lanelet_ =
      max_crosswalk_user_delta_yaw_threshold_for_lanelet;
    use_crosswalk_signal_ = use_crosswalk_signal;
    threshold_velocity_assumed_as_stopping_ = threshold_velocity_assumed_as_stopping;
    distance_set_for_no_intention_to_walk_ = distance_set_for_no_intention_to_walk;
    timeout_set_for_no_intention_to_walk_ = timeout_set_for_no_intention_to_walk;
    prediction_time_horizon_ = prediction_time_horizon;

    path_generator_ = std::make_shared<PathGenerator>(
      prediction_sampling_time_interval, min_crosswalk_user_velocity);
  }

  void setLaneletMap(std::shared_ptr<lanelet::LaneletMap> lanelet_map_ptr);

  void setTimeKeeper(std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_ptr)
  {
    time_keeper_ = std::move(time_keeper_ptr);
  }

  void setTrafficSignal(const TrafficLightGroupArray & traffic_signal_groups);

  void initialize();

  void loadCurrentCrosswalkUsers(const TrackedObjects & objects);
  void removeOldKnownMatches(const double current_time, const double buffer_time);

  PredictedObject predict(const std_msgs::msg::Header & header, const TrackedObject & object);
  PredictedObjects retrieveUndetectedObjects();

private:
  rclcpp::Node & node_;
  std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_;

  // Map data
  std::shared_ptr<lanelet::LaneletMap> lanelet_map_ptr_;
  lanelet::ConstLanelets crosswalks_;
  lanelet::LaneletMapUPtr fence_layer_{nullptr};
  std::shared_ptr<PathGenerator> path_generator_;

  // Data
  std::unordered_map<std::string, TrackedObject> current_crosswalk_users_;
  std::unordered_set<std::string> predicted_crosswalk_users_ids_;
  std::unordered_map<std::string, std::deque<CrosswalkUserData>> crosswalk_users_history_;
  std::map<std::pair<std::string, lanelet::Id>, rclcpp::Time> stopped_times_against_green_;
  std::unordered_map<std::string, std::string> known_matches_;
  std::unordered_map<lanelet::Id, TrafficLightGroup> traffic_signal_id_map_;

  // Parameters
  double prediction_time_horizon_;
  bool match_lost_and_appeared_crosswalk_users_;
  double min_crosswalk_user_velocity_;
  double max_crosswalk_user_delta_yaw_threshold_for_lanelet_;
  bool use_crosswalk_signal_;
  double threshold_velocity_assumed_as_stopping_;
  std::vector<double> distance_set_for_no_intention_to_walk_;
  std::vector<double> timeout_set_for_no_intention_to_walk_;

  //// process
  std::optional<lanelet::Id> getTrafficSignalId(const lanelet::ConstLanelet & way_lanelet);
  PredictedObject getPredictedObjectAsCrosswalkUser(const TrackedObject & object);
  void updateCrosswalkUserHistory(
    const std_msgs::msg::Header & header, const TrackedObject & object,
    const std::string & object_id);
  std::string tryMatchNewObjectToDisappeared(
    const std::string & object_id, std::unordered_map<std::string, TrackedObject> & current_users);
  bool calcIntentionToCrossWithTrafficSignal(
    const TrackedObject & object, const lanelet::ConstLanelet & crosswalk,
    const lanelet::Id & signal_id);
  bool doesPathCrossAnyFence(const PredictedPath & predicted_path);
  std::optional<TrafficLightElement> getTrafficSignalElement(const lanelet::Id & id);
};

}  // namespace autoware::map_based_prediction

#endif  // MAP_BASED_PREDICTION__PREDICTOR_VRU_HPP_

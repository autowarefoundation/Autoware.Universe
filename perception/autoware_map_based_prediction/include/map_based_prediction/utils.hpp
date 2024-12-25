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

#ifndef MAP_BASED_PREDICTION__UTILS_HPP_
#define MAP_BASED_PREDICTION__UTILS_HPP_

#include "map_based_prediction/data_structure.hpp"

#include <autoware/universe_utils/math/normalization.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/tracked_objects.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/LaneletMap.h>
#include <tf2/utils.h>

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace autoware::map_based_prediction
{

namespace utils
{

using autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjectKinematics;
using autoware_perception_msgs::msg::TrackedObject;
using autoware_perception_msgs::msg::TrackedObjectKinematics;

/**
 * @brief calc absolute normalized yaw difference between lanelet and object
 *
 * @param object
 * @param lanelet
 * @return double
 */
double calcAbsYawDiffBetweenLaneletAndObject(
  const TrackedObject & object, const lanelet::ConstLanelet & lanelet);

bool withinRoadLanelet(
  const TrackedObject & object,
  const std::vector<std::pair<double, lanelet::Lanelet>> & surrounding_lanelets_with_dist,
  const bool use_yaw_information = false);

bool withinRoadLanelet(
  const TrackedObject & object, const lanelet::LaneletMapPtr & lanelet_map_ptr,
  const bool use_yaw_information = false);

/**
 * @brief change label for prediction
 *
 * @param label
 * @return ObjectClassification::_label_type
 */
ObjectClassification::_label_type changeLabelForPrediction(
  const ObjectClassification::_label_type & label, const TrackedObject & object,
  const lanelet::LaneletMapPtr & lanelet_map_ptr_);

template <typename T>
std::unordered_set<std::string> removeOldObjectsHistory(
  const double current_time, const double buffer_time,
  std::unordered_map<std::string, std::deque<T>> & target_objects);

extern template std::unordered_set<std::string> removeOldObjectsHistory<ObjectData>(
  const double current_time, const double buffer_time,
  std::unordered_map<std::string, std::deque<ObjectData>> & target_objects);
extern template std::unordered_set<std::string> removeOldObjectsHistory<CrosswalkUserData>(
  const double current_time, const double buffer_time,
  std::unordered_map<std::string, std::deque<CrosswalkUserData>> & target_objects);

PredictedObjectKinematics convertToPredictedKinematics(
  const TrackedObjectKinematics & tracked_object);

PredictedObject convertToPredictedObject(const TrackedObject & tracked_object);

double calculateLocalLikelihood(
  const lanelet::Lanelet & current_lanelet, const TrackedObject & object,
  const double sigma_lateral_offset, const double sigma_yaw_angle_deg);

bool isDuplicated(
  const std::pair<double, lanelet::ConstLanelet> & target_lanelet,
  const LaneletsData & lanelets_data);

bool isDuplicated(
  const PredictedPath & predicted_path, const std::vector<PredictedPath> & predicted_paths);

bool checkCloseLaneletCondition(
  const std::pair<double, lanelet::Lanelet> & lanelet, const TrackedObject & object,
  const std::unordered_map<std::string, std::deque<ObjectData>> & road_users_history,
  const double dist_threshold_for_searching_lanelet,
  const double delta_yaw_threshold_for_searching_lanelet);

// NOTE: These two functions are copied from the route_handler package.
lanelet::Lanelets getRightOppositeLanelets(
  const std::shared_ptr<lanelet::LaneletMap> & lanelet_map_ptr,
  const lanelet::ConstLanelet & lanelet);

lanelet::Lanelets getLeftOppositeLanelets(
  const std::shared_ptr<lanelet::LaneletMap> & lanelet_map_ptr,
  const lanelet::ConstLanelet & lanelet);

LaneletsData getCurrentLanelets(
  const TrackedObject & object, lanelet::LaneletMapPtr lanelet_map_ptr,
  const std::unordered_map<std::string, std::deque<ObjectData>> & road_users_history,
  const double dist_threshold_for_searching_lanelet,
  const double delta_yaw_threshold_for_searching_lanelet, const double sigma_lateral_offset,
  const double sigma_yaw_angle_deg);

}  // namespace utils

}  // namespace autoware::map_based_prediction

#endif  // MAP_BASED_PREDICTION__UTILS_HPP_

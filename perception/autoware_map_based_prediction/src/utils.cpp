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

#include "map_based_prediction/utils.hpp"

#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/ros/uuid_helper.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_core/LaneletMap.h>

#include <algorithm>
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

/**
 * @brief calc absolute normalized yaw difference between lanelet and object
 *
 * @param object
 * @param lanelet
 * @return double
 */
double calcAbsYawDiffBetweenLaneletAndObject(
  const TrackedObject & object, const lanelet::ConstLanelet & lanelet)
{
  const double object_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
  const double lane_yaw =
    lanelet::utils::getLaneletAngle(lanelet, object.kinematics.pose_with_covariance.pose.position);
  const double delta_yaw = object_yaw - lane_yaw;
  const double normalized_delta_yaw = autoware::universe_utils::normalizeRadian(delta_yaw);
  const double abs_norm_delta = std::fabs(normalized_delta_yaw);
  return abs_norm_delta;
}

bool withinRoadLanelet(
  const TrackedObject & object,
  const std::vector<std::pair<double, lanelet::Lanelet>> & surrounding_lanelets_with_dist,
  const bool use_yaw_information)
{
  for (const auto & [dist, lanelet] : surrounding_lanelets_with_dist) {
    if (lanelet.hasAttribute(lanelet::AttributeName::Subtype)) {
      lanelet::Attribute attr = lanelet.attribute(lanelet::AttributeName::Subtype);
      if (
        attr.value() == lanelet::AttributeValueString::Crosswalk ||
        attr.value() == lanelet::AttributeValueString::Walkway) {
        continue;
      }
    }

    constexpr float yaw_threshold = 0.6;
    bool within_lanelet = std::abs(dist) < 1e-5;
    if (use_yaw_information) {
      within_lanelet =
        within_lanelet && calcAbsYawDiffBetweenLaneletAndObject(object, lanelet) < yaw_threshold;
    }
    if (within_lanelet) {
      return true;
    }
  }

  return false;
}

bool withinRoadLanelet(
  const TrackedObject & object, const lanelet::LaneletMapPtr & lanelet_map_ptr,
  const bool use_yaw_information)
{
  const auto & obj_pos = object.kinematics.pose_with_covariance.pose.position;
  lanelet::BasicPoint2d search_point(obj_pos.x, obj_pos.y);
  // nearest lanelet
  constexpr double search_radius = 10.0;  // [m]
  const auto surrounding_lanelets_with_dist =
    lanelet::geometry::findWithin2d(lanelet_map_ptr->laneletLayer, search_point, search_radius);

  return withinRoadLanelet(object, surrounding_lanelets_with_dist, use_yaw_information);
}

/**
 * @brief change label for prediction
 *
 * @param label
 * @return ObjectClassification::_label_type
 */
ObjectClassification::_label_type changeLabelForPrediction(
  const ObjectClassification::_label_type & label, const TrackedObject & object,
  const lanelet::LaneletMapPtr & lanelet_map_ptr_)
{
  // for car like vehicle do not change labels
  switch (label) {
    case ObjectClassification::CAR:
    case ObjectClassification::BUS:
    case ObjectClassification::TRUCK:
    case ObjectClassification::TRAILER:
    case ObjectClassification::UNKNOWN:
      return label;

    case ObjectClassification::MOTORCYCLE:
    case ObjectClassification::BICYCLE: {  // if object is within road lanelet and satisfies yaw
                                           // constraints
      const bool within_road_lanelet = withinRoadLanelet(object, lanelet_map_ptr_, true);
      // if the object is within lanelet, do the same estimation with vehicle
      if (within_road_lanelet) return ObjectClassification::MOTORCYCLE;

      constexpr float high_speed_threshold =
        autoware::universe_utils::kmph2mps(25.0);  // High speed bicycle 25 km/h
      // calc abs speed from x and y velocity
      const double abs_speed = std::hypot(
        object.kinematics.twist_with_covariance.twist.linear.x,
        object.kinematics.twist_with_covariance.twist.linear.y);
      const bool high_speed_object = abs_speed > high_speed_threshold;
      // high speed object outside road lanelet will move like unknown object
      // return ObjectClassification::UNKNOWN; // temporary disabled
      if (high_speed_object) return label;  // Do nothing for now
      return ObjectClassification::BICYCLE;
    }

    case ObjectClassification::PEDESTRIAN: {
      const float max_velocity_for_human_mps =
        autoware::universe_utils::kmph2mps(25.0);  // Max human being motion speed is 25km/h
      const double abs_speed = std::hypot(
        object.kinematics.twist_with_covariance.twist.linear.x,
        object.kinematics.twist_with_covariance.twist.linear.y);
      const bool high_speed_object = abs_speed > max_velocity_for_human_mps;
      // fast, human-like object: like segway
      // return ObjectClassification::MOTORCYCLE;
      if (high_speed_object) return label;  // currently do nothing
      // fast human outside road lanelet will move like unknown object
      // return ObjectClassification::UNKNOWN;
      return label;
    }

    default:
      return label;
  }
}

template <typename T>
std::unordered_set<std::string> removeOldObjectsHistory(
  const double current_time, const double buffer_time,
  std::unordered_map<std::string, std::deque<T>> & target_objects)
{
  std::unordered_set<std::string> invalid_object_id;
  for (auto iter = target_objects.begin(); iter != target_objects.end(); ++iter) {
    const std::string object_id = iter->first;
    std::deque<T> & object_data = iter->second;

    // If object data is empty, we are going to delete the buffer for the obstacle
    if (object_data.empty()) {
      invalid_object_id.insert(object_id);
      continue;
    }

    const double latest_object_time = rclcpp::Time(object_data.back().header.stamp).seconds();

    // Delete Old Objects
    if (current_time - latest_object_time > buffer_time) {
      invalid_object_id.insert(object_id);
      continue;
    }

    // Delete old information
    while (!object_data.empty()) {
      const double post_object_time = rclcpp::Time(object_data.front().header.stamp).seconds();
      if (current_time - post_object_time <= buffer_time) {
        break;
      }
      object_data.pop_front();
    }

    if (object_data.empty()) {
      invalid_object_id.insert(object_id);
      continue;
    }
  }

  for (const auto & key : invalid_object_id) {
    target_objects.erase(key);
  }

  return invalid_object_id;
}

// Explicit instantiation definitions
template std::unordered_set<std::string> removeOldObjectsHistory<ObjectData>(
  const double current_time, const double buffer_time,
  std::unordered_map<std::string, std::deque<ObjectData>> & target_objects);
template std::unordered_set<std::string> removeOldObjectsHistory<CrosswalkUserData>(
  const double current_time, const double buffer_time,
  std::unordered_map<std::string, std::deque<CrosswalkUserData>> & target_objects);

PredictedObjectKinematics convertToPredictedKinematics(
  const TrackedObjectKinematics & tracked_object)
{
  PredictedObjectKinematics output;
  output.initial_pose_with_covariance = tracked_object.pose_with_covariance;
  output.initial_twist_with_covariance = tracked_object.twist_with_covariance;
  output.initial_acceleration_with_covariance = tracked_object.acceleration_with_covariance;
  return output;
}

PredictedObject convertToPredictedObject(const TrackedObject & tracked_object)
{
  PredictedObject predicted_object;
  predicted_object.kinematics = convertToPredictedKinematics(tracked_object.kinematics);
  predicted_object.classification = tracked_object.classification;
  predicted_object.object_id = tracked_object.object_id;
  predicted_object.shape.type = tracked_object.shape.type;
  predicted_object.shape.footprint = tracked_object.shape.footprint;
  predicted_object.shape.dimensions = tracked_object.shape.dimensions;
  predicted_object.existence_probability = tracked_object.existence_probability;

  return predicted_object;
}

double calculateLocalLikelihood(
  const lanelet::Lanelet & current_lanelet, const TrackedObject & object,
  const double sigma_lateral_offset, const double sigma_yaw_angle_deg)
{
  const auto & obj_point = object.kinematics.pose_with_covariance.pose.position;

  // compute yaw difference between the object and lane
  const double obj_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
  const double lane_yaw = lanelet::utils::getLaneletAngle(current_lanelet, obj_point);
  const double delta_yaw = obj_yaw - lane_yaw;
  const double abs_norm_delta_yaw = std::fabs(autoware::universe_utils::normalizeRadian(delta_yaw));

  // compute lateral distance
  const auto centerline = current_lanelet.centerline();
  std::vector<geometry_msgs::msg::Point> converted_centerline;
  for (const auto & p : centerline) {
    const auto converted_p = lanelet::utils::conversion::toGeomMsgPt(p);
    converted_centerline.push_back(converted_p);
  }
  const double lat_dist =
    std::fabs(autoware::motion_utils::calcLateralOffset(converted_centerline, obj_point));

  // Compute Chi-squared distributed (Equation (8) in the paper)
  const double sigma_d = sigma_lateral_offset;  // Standard Deviation for lateral position
  const double sigma_yaw = M_PI * sigma_yaw_angle_deg / 180.0;  // Standard Deviation for yaw angle
  const Eigen::Vector2d delta(lat_dist, abs_norm_delta_yaw);
  const Eigen::Matrix2d P_inv =
    (Eigen::Matrix2d() << 1.0 / (sigma_d * sigma_d), 0.0, 0.0, 1.0 / (sigma_yaw * sigma_yaw))
      .finished();
  const double MINIMUM_DISTANCE = 1e-6;
  const double dist = std::max(delta.dot(P_inv * delta), MINIMUM_DISTANCE);

  return 1.0 / dist;
}

bool isDuplicated(
  const std::pair<double, lanelet::ConstLanelet> & target_lanelet,
  const LaneletsData & lanelets_data)
{
  const double CLOSE_LANELET_THRESHOLD = 0.1;
  for (const auto & lanelet_data : lanelets_data) {
    const auto target_lanelet_end_p = target_lanelet.second.centerline2d().back();
    const auto lanelet_end_p = lanelet_data.lanelet.centerline2d().back();
    const double dist = std::hypot(
      target_lanelet_end_p.x() - lanelet_end_p.x(), target_lanelet_end_p.y() - lanelet_end_p.y());
    if (dist < CLOSE_LANELET_THRESHOLD) {
      return true;
    }
  }

  return false;
}

bool isDuplicated(
  const PredictedPath & predicted_path, const std::vector<PredictedPath> & predicted_paths)
{
  const double CLOSE_PATH_THRESHOLD = 0.1;
  for (const auto & prev_predicted_path : predicted_paths) {
    const auto prev_path_end = prev_predicted_path.path.back().position;
    const auto current_path_end = predicted_path.path.back().position;
    const double dist = autoware::universe_utils::calcDistance2d(prev_path_end, current_path_end);
    if (dist < CLOSE_PATH_THRESHOLD) {
      return true;
    }
  }

  return false;
}

bool checkCloseLaneletCondition(
  const std::pair<double, lanelet::Lanelet> & lanelet, const TrackedObject & object,
  const std::unordered_map<std::string, std::deque<ObjectData>> & road_users_history,
  const double dist_threshold_for_searching_lanelet,
  const double delta_yaw_threshold_for_searching_lanelet)
{
  // Step1. If we only have one point in the centerline, we will ignore the lanelet
  if (lanelet.second.centerline().size() <= 1) {
    return false;
  }

  // If the object is in the objects history, we check if the target lanelet is
  // inside the current lanelets id or following lanelets
  const std::string object_id = autoware::universe_utils::toHexString(object.object_id);
  if (road_users_history.count(object_id) != 0) {
    const std::vector<lanelet::ConstLanelet> & possible_lanelet =
      road_users_history.at(object_id).back().future_possible_lanelets;

    bool not_in_possible_lanelet =
      std::find(possible_lanelet.begin(), possible_lanelet.end(), lanelet.second) ==
      possible_lanelet.end();
    if (!possible_lanelet.empty() && not_in_possible_lanelet) {
      return false;
    }
  }

  // Step2. Calculate the angle difference between the lane angle and obstacle angle
  const double object_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
  const double lane_yaw = lanelet::utils::getLaneletAngle(
    lanelet.second, object.kinematics.pose_with_covariance.pose.position);
  const double delta_yaw = object_yaw - lane_yaw;
  const double normalized_delta_yaw = autoware::universe_utils::normalizeRadian(delta_yaw);
  const double abs_norm_delta = std::fabs(normalized_delta_yaw);

  // Step3. Check if the closest lanelet is valid, and add all
  // of the lanelets that are below max_dist and max_delta_yaw
  const double object_vel = object.kinematics.twist_with_covariance.twist.linear.x;
  const bool is_yaw_reversed =
    M_PI - delta_yaw_threshold_for_searching_lanelet < abs_norm_delta && object_vel < 0.0;
  if (dist_threshold_for_searching_lanelet < lanelet.first) {
    return false;
  }
  if (!is_yaw_reversed && delta_yaw_threshold_for_searching_lanelet < abs_norm_delta) {
    return false;
  }

  return true;
}

// NOTE: These two functions are copied from the route_handler package.
lanelet::Lanelets getRightOppositeLanelets(
  const std::shared_ptr<lanelet::LaneletMap> & lanelet_map_ptr,
  const lanelet::ConstLanelet & lanelet)
{
  const auto opposite_candidate_lanelets =
    lanelet_map_ptr->laneletLayer.findUsages(lanelet.rightBound().invert());

  lanelet::Lanelets opposite_lanelets;
  for (const auto & candidate_lanelet : opposite_candidate_lanelets) {
    if (candidate_lanelet.leftBound().id() == lanelet.rightBound().id()) {
      continue;
    }

    opposite_lanelets.push_back(candidate_lanelet);
  }

  return opposite_lanelets;
}

lanelet::Lanelets getLeftOppositeLanelets(
  const std::shared_ptr<lanelet::LaneletMap> & lanelet_map_ptr,
  const lanelet::ConstLanelet & lanelet)
{
  const auto opposite_candidate_lanelets =
    lanelet_map_ptr->laneletLayer.findUsages(lanelet.leftBound().invert());

  lanelet::Lanelets opposite_lanelets;
  for (const auto & candidate_lanelet : opposite_candidate_lanelets) {
    if (candidate_lanelet.rightBound().id() == lanelet.leftBound().id()) {
      continue;
    }

    opposite_lanelets.push_back(candidate_lanelet);
  }

  return opposite_lanelets;
}

LaneletsData getCurrentLanelets(
  const TrackedObject & object, lanelet::LaneletMapPtr lanelet_map_ptr,
  const std::unordered_map<std::string, std::deque<ObjectData>> & road_users_history,
  const double dist_threshold_for_searching_lanelet,
  const double delta_yaw_threshold_for_searching_lanelet, const double sigma_lateral_offset,
  const double sigma_yaw_angle_deg)
{
  // obstacle point
  lanelet::BasicPoint2d search_point(
    object.kinematics.pose_with_covariance.pose.position.x,
    object.kinematics.pose_with_covariance.pose.position.y);

  // nearest lanelet
  std::vector<std::pair<double, lanelet::Lanelet>> surrounding_lanelets =
    lanelet::geometry::findNearest(lanelet_map_ptr->laneletLayer, search_point, 10);

  {  // Step 1. Search same directional lanelets
    // No Closest Lanelets
    if (surrounding_lanelets.empty()) {
      return {};
    }

    LaneletsData object_lanelets;
    std::optional<std::pair<double, lanelet::Lanelet>> closest_lanelet{std::nullopt};
    for (const auto & lanelet : surrounding_lanelets) {
      // Check if the close lanelets meet the necessary condition for start lanelets and
      // Check if similar lanelet is inside the object lanelet
      if (
        !checkCloseLaneletCondition(
          lanelet, object, road_users_history, dist_threshold_for_searching_lanelet,
          delta_yaw_threshold_for_searching_lanelet) ||
        isDuplicated(lanelet, object_lanelets)) {
        continue;
      }

      // Memorize closest lanelet
      // NOTE: The object may be outside the lanelet.
      if (!closest_lanelet || lanelet.first < closest_lanelet->first) {
        closest_lanelet = lanelet;
      }

      // Check if the obstacle is inside of this lanelet
      constexpr double epsilon = 1e-3;
      if (lanelet.first < epsilon) {
        const auto object_lanelet = LaneletData{
          lanelet.second, utils::calculateLocalLikelihood(
                            lanelet.second, object, sigma_lateral_offset, sigma_yaw_angle_deg)};
        object_lanelets.push_back(object_lanelet);
      }
    }

    if (!object_lanelets.empty()) {
      return object_lanelets;
    }
    if (closest_lanelet) {
      return LaneletsData{LaneletData{
        closest_lanelet->second,
        utils::calculateLocalLikelihood(
          closest_lanelet->second, object, sigma_lateral_offset, sigma_yaw_angle_deg)}};
    }
  }

  {  // Step 2. Search opposite directional lanelets
    // Get opposite lanelets and calculate distance to search point.
    std::vector<std::pair<double, lanelet::Lanelet>> surrounding_opposite_lanelets;
    for (const auto & surrounding_lanelet : surrounding_lanelets) {
      for (const auto & left_opposite_lanelet :
           getLeftOppositeLanelets(lanelet_map_ptr, surrounding_lanelet.second)) {
        const double distance = lanelet::geometry::distance2d(left_opposite_lanelet, search_point);
        surrounding_opposite_lanelets.push_back(std::make_pair(distance, left_opposite_lanelet));
      }
      for (const auto & right_opposite_lanelet :
           getRightOppositeLanelets(lanelet_map_ptr, surrounding_lanelet.second)) {
        const double distance = lanelet::geometry::distance2d(right_opposite_lanelet, search_point);
        surrounding_opposite_lanelets.push_back(std::make_pair(distance, right_opposite_lanelet));
      }
    }

    std::optional<std::pair<double, lanelet::Lanelet>> opposite_closest_lanelet{std::nullopt};
    for (const auto & lanelet : surrounding_opposite_lanelets) {
      // Check if the close lanelets meet the necessary condition for start lanelets
      // except for distance checking
      if (!checkCloseLaneletCondition(
            lanelet, object, road_users_history, dist_threshold_for_searching_lanelet,
            delta_yaw_threshold_for_searching_lanelet)) {
        continue;
      }

      // Memorize closest lanelet
      if (!opposite_closest_lanelet || lanelet.first < opposite_closest_lanelet->first) {
        opposite_closest_lanelet = lanelet;
      }
    }
    if (opposite_closest_lanelet) {
      return LaneletsData{LaneletData{
        opposite_closest_lanelet->second,
        utils::calculateLocalLikelihood(
          opposite_closest_lanelet->second, object, sigma_lateral_offset, sigma_yaw_angle_deg)}};
    }
  }

  return LaneletsData{};
}

}  // namespace utils
}  // namespace autoware::map_based_prediction

// Copyright 2024 Tier IV, Inc.
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

#ifndef OBJECT_MANAGER_HPP_
#define OBJECT_MANAGER_HPP_

#include <rclcpp/time.hpp>

#include <autoware_auto_perception_msgs/msg/predicted_object.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <unique_identifier_msgs/msg/uuid.hpp>

#include <boost/unordered_map.hpp>
#include <boost/uuid/uuid.hpp>

#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>

#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace std
{
template <>
struct hash<unique_identifier_msgs::msg::UUID>
{
  size_t operator()(const unique_identifier_msgs::msg::UUID & uid) const
  {
    const auto & ids = uid.uuid;
    boost::uuids::uuid u = {ids[0], ids[1], ids[2],  ids[3],  ids[4],  ids[5],  ids[6],  ids[7],
                            ids[8], ids[9], ids[10], ids[11], ids[12], ids[13], ids[14], ids[15]};
    return boost::hash<boost::uuids::uuid>()(u);
  }
};
}  // namespace std

namespace behavior_velocity_planner::intersection
{

/**
 * @brief store collision information
 */
struct CollisionInterval
{
  enum LanePosition {
    FIRST,
    SECOND,
    ELSE,
  } expected_lane_position;
  lanelet::Id lane_id;

  //! original predicted path
  std::vector<geometry_msgs::msg::Pose> path;

  //! possible collision interval position index on path
  std::pair<size_t, size_t> interval_position;

  //! possible collision interval time(without TTC margin)
  std::pair<double, double> interval_time;
};

struct CollisionKnowledge
{
  rclcpp::Time stamp;
  CollisionInterval interval;
  lanelet::ArcCoordinates observed_position_arc_coords;
  double observed_velocity;
};

/**
 * @brief store collision information of object on the attention area
 */
class ObjectInfo
{
public:
  explicit ObjectInfo(const unique_identifier_msgs::msg::UUID & uuid);

  const autoware_auto_perception_msgs::msg::PredictedObject & predicted_object() const
  {
    return predicted_object_;
  };

  const std::optional<CollisionInterval> & unsafe_decision_knowledge() const
  {
    return unsafe_decision_knowledge_;
  }

  /**
   * @brief update observed_position, observed_position_arc_coords, predicted_path,
   * observed_velocity, attention_lanelet, stopline, dist_to_stopline
   */
  void initialize(
    const autoware_auto_perception_msgs::msg::PredictedObject & predicted_object,
    std::optional<lanelet::ConstLanelet> attention_lanelet_opt,
    std::optional<lanelet::ConstLineString3d> stopline_opt);

  /**
   * @brief update observed_position, observed_position_arc_coords, predicted_path,
   * observed_velocity, attention_lanelet, stopline, dist_to_stopline
   */
  void update(const std::optional<CollisionInterval> & unsafe_knowledge_opt);

  /**
   * @brief find the estimated position of the object in the past
   */
  geometry_msgs::msg::Pose estimated_past_pose(const double past_duration) const;

  /**
   * @brief check if object can stop before stopline under the deceleration. return false if
   * stopline is null for conservative collision  checking
   */
  bool can_stop_before_stopline(const double brake_deceleration) const;

  /**
   * @brief check if object can stop before stopline within the overshoot margin. return false if
   * stopline is null for conservative collision checking
   */
  bool can_stop_before_ego_lane(
    const double brake_deceleration, const double tolerable_overshoot,
    lanelet::ConstLanelet ego_lane) const;

  /**
   * @brief check if the object is before the stopline within the specified margin
   */
  bool before_stopline_by(const double margin) const;

private:
  const std::string uuid_str;
  autoware_auto_perception_msgs::msg::PredictedObject predicted_object_;

  //! arc coordinate on the attention lanelet
  lanelet::ArcCoordinates observed_position_arc_coords;

  double observed_velocity;

  //! null if the object in intersection_area but not in attention_area
  std::optional<lanelet::ConstLanelet> attention_lanelet_opt{std::nullopt};

  //! null if the object in intersection_area but not in attention_area
  std::optional<lanelet::ConstLineString3d> stopline_opt{std::nullopt};

  //! null if the object in intersection_area but not in attention_area
  std::optional<double> dist_to_stopline_opt{std::nullopt};

  //! store the information of if judged as UNSAFE
  std::optional<CollisionInterval> unsafe_decision_knowledge_{std::nullopt};

  std::optional<std::tuple<rclcpp::Time, CollisionKnowledge, bool>>
    decision_at_1st_pass_judge_line_passage{std::nullopt};
  std::optional<std::tuple<rclcpp::Time, CollisionKnowledge, bool>>
    decision_at_2nd_pass_judge_line_passage{std::nullopt};

  /**
   * @brief calculate/update the distance to corresponding stopline
   */
  void calc_dist_to_stopline();
};

/**
 * @brief store predicted objects for intersection
 */
class ObjectInfoManager
{
public:
  std::shared_ptr<ObjectInfo> registerObject(
    const unique_identifier_msgs::msg::UUID & uuid, const bool belong_attention_area,
    const bool belong_intersection_area, const bool is_parked);

  void registerExistingObject(
    const unique_identifier_msgs::msg::UUID & uuid, const bool belong_attention_area,
    const bool belong_intersection_area, const bool is_parked,
    std::shared_ptr<intersection::ObjectInfo> object);

  void clearObjects();

  const std::vector<std::shared_ptr<ObjectInfo>> & attentionObjects() const
  {
    return attention_area_objects_;
  }

  const std::vector<std::shared_ptr<ObjectInfo>> & parkedObjects() const { return parked_objects_; }

  std::vector<std::shared_ptr<ObjectInfo>> allObjects();

  const std::unordered_map<unique_identifier_msgs::msg::UUID, std::shared_ptr<ObjectInfo>> &
  getObjectsMap()
  {
    return objects_info_;
  }

private:
  std::unordered_map<unique_identifier_msgs::msg::UUID, std::shared_ptr<ObjectInfo>> objects_info_;

  //! belong to attention area
  std::vector<std::shared_ptr<ObjectInfo>> attention_area_objects_;

  //! does not belong to attention area but to intersection area
  std::vector<std::shared_ptr<ObjectInfo>> intersection_area_objects_;

  //! parked objects on attention_area/intersection_area
  std::vector<std::shared_ptr<ObjectInfo>> parked_objects_;
};

/**
 * @brief return the CollisionInterval struct if the predicted path collides ego path geometrically
 */
std::optional<intersection::CollisionInterval> findPassageInterval(
  const autoware_auto_perception_msgs::msg::PredictedPath & predicted_path,
  const autoware_auto_perception_msgs::msg::Shape & shape,
  const lanelet::BasicPolygon2d & ego_lane_poly,
  const std::optional<lanelet::ConstLanelet> & first_attention_lane_opt,
  const std::optional<lanelet::ConstLanelet> & second_attention_lane_opt);

}  // namespace behavior_velocity_planner::intersection

#endif  // OBJECT_MANAGER_HPP_

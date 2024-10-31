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

#include "map_based_prediction/predictor_vru.hpp"

#include "map_based_prediction/utils.hpp"

#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/ros/uuid_helper.hpp>

namespace autoware::map_based_prediction
{
using autoware::universe_utils::ScopedTimeTrack;

namespace
{
boost::optional<CrosswalkEdgePoints> isReachableCrosswalkEdgePoints(
  const TrackedObject & object, const lanelet::ConstLanelets & surrounding_lanelets,
  const lanelet::ConstLanelets & surrounding_crosswalks, const CrosswalkEdgePoints & edge_points,
  const double time_horizon, const double min_object_vel)
{
  using Point = boost::geometry::model::d2::point_xy<double>;

  const auto & obj_pos = object.kinematics.pose_with_covariance.pose.position;
  const auto & obj_vel = object.kinematics.twist_with_covariance.twist.linear;
  const auto yaw = autoware::universe_utils::getRPY(object.kinematics.pose_with_covariance.pose).z;

  lanelet::BasicPoint2d obj_pos_as_lanelet(obj_pos.x, obj_pos.y);

  const auto & p1 = edge_points.front_center_point;
  const auto & p2 = edge_points.back_center_point;

  CrosswalkEdgePoints ret{p1, {}, {}, p2, {}, {}};
  auto distance_pedestrian_to_p1 = std::hypot(p1.x() - obj_pos.x, p1.y() - obj_pos.y);
  auto distance_pedestrian_to_p2 = std::hypot(p2.x() - obj_pos.x, p2.y() - obj_pos.y);

  if (distance_pedestrian_to_p2 < distance_pedestrian_to_p1) {
    ret.swap();
    std::swap(distance_pedestrian_to_p1, distance_pedestrian_to_p2);
  }

  constexpr double stop_velocity_th = 0.14;  // [m/s]
  const auto estimated_velocity = std::hypot(obj_vel.x, obj_vel.y);
  const auto is_stop_object = estimated_velocity < stop_velocity_th;
  const auto velocity = std::max(min_object_vel, estimated_velocity);

  const auto isAcrossAnyRoad = [&surrounding_lanelets, &surrounding_crosswalks](
                                 const Point & p_src, const Point & p_dst) {
    const auto withinAnyCrosswalk = [&surrounding_crosswalks](const Point & p) {
      for (const auto & crosswalk : surrounding_crosswalks) {
        if (boost::geometry::within(p, crosswalk.polygon2d().basicPolygon())) {
          return true;
        }
      }
      return false;
    };

    const auto isExist = [](const Point & p, const std::vector<Point> & points) {
      for (const auto & existingPoint : points) {
        if (boost::geometry::distance(p, existingPoint) < 1e-1) {
          return true;
        }
      }
      return false;
    };

    std::vector<Point> points_of_intersect;
    const boost::geometry::model::linestring<Point> line{p_src, p_dst};
    for (const auto & lanelet : surrounding_lanelets) {
      const lanelet::Attribute attr = lanelet.attribute(lanelet::AttributeName::Subtype);
      if (attr.value() != lanelet::AttributeValueString::Road) {
        continue;
      }

      std::vector<Point> tmp_intersects;
      boost::geometry::intersection(line, lanelet.polygon2d().basicPolygon(), tmp_intersects);
      for (const auto & p : tmp_intersects) {
        if (isExist(p, points_of_intersect) || withinAnyCrosswalk(p)) {
          continue;
        }
        points_of_intersect.push_back(p);
        if (points_of_intersect.size() >= 2) {
          return true;
        }
      }
    }
    return false;
  };

  const bool first_intersects_road = isAcrossAnyRoad(
    {obj_pos.x, obj_pos.y}, {ret.front_center_point.x(), ret.front_center_point.y()});
  const bool second_intersects_road =
    isAcrossAnyRoad({obj_pos.x, obj_pos.y}, {ret.back_center_point.x(), ret.back_center_point.y()});

  if (first_intersects_road && second_intersects_road) {
    return {};
  }

  if (first_intersects_road && !second_intersects_road) {
    ret.swap();
  }

  const Eigen::Vector2d pedestrian_to_crosswalk(
    (ret.front_center_point.x() + ret.back_center_point.x()) / 2.0 - obj_pos.x,
    (ret.front_center_point.y() + ret.back_center_point.y()) / 2.0 - obj_pos.y);
  const Eigen::Vector2d pedestrian_heading_direction(
    obj_vel.x * std::cos(yaw), obj_vel.x * std::sin(yaw));
  const auto reachable =
    std::min(distance_pedestrian_to_p1, distance_pedestrian_to_p2) < velocity * time_horizon;
  const auto heading_for_crosswalk =
    pedestrian_to_crosswalk.dot(pedestrian_heading_direction) > 0.0;

  if ((reachable && heading_for_crosswalk) || (reachable && is_stop_object)) {
    return ret;
  }

  return {};
}

bool hasPotentialToReach(
  const TrackedObject & object, const Eigen::Vector2d & center_point,
  const Eigen::Vector2d & right_point, const Eigen::Vector2d & left_point,
  const double time_horizon, const double min_object_vel,
  const double max_crosswalk_user_delta_yaw_threshold_for_lanelet)
{
  const auto & obj_pos = object.kinematics.pose_with_covariance.pose.position;
  const auto & obj_vel = object.kinematics.twist_with_covariance.twist.linear;
  const auto yaw = autoware::universe_utils::getRPY(object.kinematics.pose_with_covariance.pose).z;

  constexpr double stop_velocity_th = 0.14;  // [m/s]
  const auto estimated_velocity = std::hypot(obj_vel.x, obj_vel.y);
  const auto is_stop_object = estimated_velocity < stop_velocity_th;
  const auto velocity = std::max(min_object_vel, estimated_velocity);

  const double pedestrian_to_crosswalk_center_direction =
    std::atan2(center_point.y() - obj_pos.y, center_point.x() - obj_pos.x);

  const auto
    [pedestrian_to_crosswalk_right_rel_direction, pedestrian_to_crosswalk_left_rel_direction] =
      [&]() {
        const double pedestrian_to_crosswalk_right_direction =
          std::atan2(right_point.y() - obj_pos.y, right_point.x() - obj_pos.x);
        const double pedestrian_to_crosswalk_left_direction =
          std::atan2(left_point.y() - obj_pos.y, left_point.x() - obj_pos.x);
        return std::make_pair(
          autoware::universe_utils::normalizeRadian(
            pedestrian_to_crosswalk_right_direction - pedestrian_to_crosswalk_center_direction),
          autoware::universe_utils::normalizeRadian(
            pedestrian_to_crosswalk_left_direction - pedestrian_to_crosswalk_center_direction));
      }();

  const double pedestrian_heading_rel_direction = [&]() {
    const double pedestrian_heading_direction =
      std::atan2(obj_vel.x * std::sin(yaw), obj_vel.x * std::cos(yaw));
    return autoware::universe_utils::normalizeRadian(
      pedestrian_heading_direction - pedestrian_to_crosswalk_center_direction);
  }();

  const double pedestrian_to_crosswalk_min_rel_direction = std::min(
    pedestrian_to_crosswalk_right_rel_direction, pedestrian_to_crosswalk_left_rel_direction);
  const double pedestrian_to_crosswalk_max_rel_direction = std::max(
    pedestrian_to_crosswalk_right_rel_direction, pedestrian_to_crosswalk_left_rel_direction);
  const double pedestrian_vel_angle_against_crosswalk = [&]() {
    if (pedestrian_heading_rel_direction < pedestrian_to_crosswalk_min_rel_direction) {
      return pedestrian_to_crosswalk_min_rel_direction - pedestrian_heading_rel_direction;
    }
    if (pedestrian_to_crosswalk_max_rel_direction < pedestrian_heading_rel_direction) {
      return pedestrian_to_crosswalk_max_rel_direction - pedestrian_heading_rel_direction;
    }
    return 0.0;
  }();
  const auto heading_for_crosswalk = std::abs(pedestrian_vel_angle_against_crosswalk) <
                                     max_crosswalk_user_delta_yaw_threshold_for_lanelet;
  const auto reachable = std::hypot(center_point.x() - obj_pos.x, center_point.y() - obj_pos.y) <
                         velocity * time_horizon;

  if (reachable && (heading_for_crosswalk || is_stop_object)) {
    return true;
  }

  return false;
}

CrosswalkEdgePoints getCrosswalkEdgePoints(const lanelet::ConstLanelet & crosswalk)
{
  const Eigen::Vector2d r_p_front = crosswalk.rightBound().front().basicPoint2d();
  const Eigen::Vector2d l_p_front = crosswalk.leftBound().front().basicPoint2d();
  const Eigen::Vector2d front_center_point = (r_p_front + l_p_front) / 2.0;

  const Eigen::Vector2d r_p_back = crosswalk.rightBound().back().basicPoint2d();
  const Eigen::Vector2d l_p_back = crosswalk.leftBound().back().basicPoint2d();
  const Eigen::Vector2d back_center_point = (r_p_back + l_p_back) / 2.0;

  return CrosswalkEdgePoints{front_center_point, r_p_front, l_p_front,
                             back_center_point,  r_p_back,  l_p_back};
}

bool isIntersecting(
  const geometry_msgs::msg::Point & point1, const geometry_msgs::msg::Point & point2,
  const lanelet::ConstPoint3d & point3, const lanelet::ConstPoint3d & point4)
{
  const auto p1 = autoware::universe_utils::createPoint(point1.x, point1.y, 0.0);
  const auto p2 = autoware::universe_utils::createPoint(point2.x, point2.y, 0.0);
  const auto p3 = autoware::universe_utils::createPoint(point3.x(), point3.y(), 0.0);
  const auto p4 = autoware::universe_utils::createPoint(point4.x(), point4.y(), 0.0);
  const auto intersection = autoware::universe_utils::intersect(p1, p2, p3, p4);
  return intersection.has_value();
}

bool doesPathCrossFence(
  const PredictedPath & predicted_path, const lanelet::ConstLineString3d & fence_line)
{
  // check whether the predicted path cross with fence
  for (size_t i = 0; i < predicted_path.path.size() - 1; ++i) {
    for (size_t j = 0; j < fence_line.size() - 1; ++j) {
      if (isIntersecting(
            predicted_path.path[i].position, predicted_path.path[i + 1].position, fence_line[j],
            fence_line[j + 1])) {
        return true;
      }
    }
  }
  return false;
}

}  // namespace

void PredictorVru::initialize()
{
  current_crosswalk_users_.clear();
  predicted_crosswalk_users_ids_.clear();
}

void PredictorVru::setTrafficSignal(const TrafficLightGroupArray & traffic_signal)
{
  traffic_signal_id_map_.clear();
  for (const auto & signal : traffic_signal.traffic_light_groups) {
    traffic_signal_id_map_[signal.traffic_light_group_id] = signal;
  }
}

void PredictorVru::setLaneletMap(std::shared_ptr<lanelet::LaneletMap> lanelet_map_ptr)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  lanelet_map_ptr_ = std::move(lanelet_map_ptr);

  const auto all_lanelets = lanelet::utils::query::laneletLayer(lanelet_map_ptr_);
  const auto crosswalks = lanelet::utils::query::crosswalkLanelets(all_lanelets);
  const auto walkways = lanelet::utils::query::walkwayLanelets(all_lanelets);
  crosswalks_.insert(crosswalks_.end(), crosswalks.begin(), crosswalks.end());
  crosswalks_.insert(crosswalks_.end(), walkways.begin(), walkways.end());

  lanelet::LineStrings3d fences;
  for (const auto & linestring : lanelet_map_ptr_->lineStringLayer) {
    if (const std::string type = linestring.attributeOr(lanelet::AttributeName::Type, "none");
        type == "fence") {
      fences.push_back(lanelet::LineString3d(
        std::const_pointer_cast<lanelet::LineStringData>(linestring.constData())));
    }
  }
  fence_layer_ = lanelet::utils::createMap(fences);
}

bool PredictorVru::doesPathCrossAnyFence(const PredictedPath & predicted_path)
{
  lanelet::BasicLineString2d predicted_path_ls;
  for (const auto & p : predicted_path.path)
    predicted_path_ls.emplace_back(p.position.x, p.position.y);
  const auto candidates =
    fence_layer_->lineStringLayer.search(lanelet::geometry::boundingBox2d(predicted_path_ls));
  for (const auto & candidate : candidates) {
    if (doesPathCrossFence(predicted_path, candidate)) {
      return true;
    }
  }
  return false;
}

void PredictorVru::loadCurrentCrosswalkUsers(const TrackedObjects & objects)
{
  if (!lanelet_map_ptr_) {
    return;
  }

  // removeStaleTrafficLightInfo
  for (auto it = stopped_times_against_green_.begin(); it != stopped_times_against_green_.end();) {
    const bool isDisappeared = std::none_of(
      objects.objects.begin(), objects.objects.end(),
      [&it](autoware_perception_msgs::msg::TrackedObject obj) {
        return autoware::universe_utils::toHexString(obj.object_id) == it->first.first;
      });
    if (isDisappeared) {
      it = stopped_times_against_green_.erase(it);
    } else {
      ++it;
    }
  }

  //
  initialize();

  // load current crosswalk users
  for (const auto & object : objects.objects) {
    const auto label_for_prediction = utils::changeLabelForPrediction(
      object.classification.front().label, object, lanelet_map_ptr_);
    if (
      label_for_prediction == ObjectClassification::PEDESTRIAN ||
      label_for_prediction == ObjectClassification::BICYCLE) {
      const std::string object_id = autoware::universe_utils::toHexString(object.object_id);
      current_crosswalk_users_.emplace(object_id, object);
    }
  }
}

void PredictorVru::removeOldKnownMatches(const double current_time, const double buffer_time)
{
  auto invalidated_crosswalk_users =
    utils::removeOldObjectsHistory(current_time, buffer_time, crosswalk_users_history_);
  // delete matches that point to invalid object
  for (auto it = known_matches_.begin(); it != known_matches_.end();) {
    if (invalidated_crosswalk_users.count(it->second)) {
      it = known_matches_.erase(it);
    } else {
      ++it;
    }
  }
}

PredictedObject PredictorVru::predict(
  const std_msgs::msg::Header & header, const TrackedObject & object)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  std::string object_id = autoware::universe_utils::toHexString(object.object_id);
  if (match_lost_and_appeared_crosswalk_users_) {
    object_id = tryMatchNewObjectToDisappeared(object_id, current_crosswalk_users_);
  }
  predicted_crosswalk_users_ids_.insert(object_id);
  updateCrosswalkUserHistory(header, object, object_id);
  return getPredictedObjectAsCrosswalkUser(object);
}

PredictedObjects PredictorVru::retrieveUndetectedObjects()
{
  PredictedObjects output;
  for (const auto & [id, crosswalk_user] : crosswalk_users_history_) {
    // get a predicted path for crosswalk users in history who didn't get path yet using latest
    // message
    if (predicted_crosswalk_users_ids_.count(id) == 0) {
      const auto predicted_object =
        getPredictedObjectAsCrosswalkUser(crosswalk_user.back().tracked_object);
      output.objects.push_back(predicted_object);
    }
  }
  return output;
}

PredictedObject PredictorVru::getPredictedObjectAsCrosswalkUser(const TrackedObject & object)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  auto predicted_object = utils::convertToPredictedObject(object);
  {
    PredictedPath predicted_path =
      path_generator_->generatePathForNonVehicleObject(object, prediction_time_horizon_);
    predicted_path.confidence = 1.0;

    predicted_object.kinematics.predicted_paths.push_back(predicted_path);
  }

  boost::optional<lanelet::ConstLanelet> crossing_crosswalk{boost::none};
  const auto & obj_pos = object.kinematics.pose_with_covariance.pose.position;
  const auto & obj_vel = object.kinematics.twist_with_covariance.twist.linear;
  const auto estimated_velocity = std::hypot(obj_vel.x, obj_vel.y);
  const auto velocity = std::max(min_crosswalk_user_velocity_, estimated_velocity);
  const auto surrounding_lanelets_with_dist = lanelet::geometry::findWithin2d(
    lanelet_map_ptr_->laneletLayer, lanelet::BasicPoint2d{obj_pos.x, obj_pos.y},
    prediction_time_horizon_ * velocity);
  lanelet::ConstLanelets surrounding_lanelets;
  lanelet::ConstLanelets surrounding_crosswalks;
  for (const auto & [dist, lanelet] : surrounding_lanelets_with_dist) {
    surrounding_lanelets.push_back(lanelet);
    const auto attr = lanelet.attribute(lanelet::AttributeName::Subtype);
    if (
      attr.value() == lanelet::AttributeValueString::Crosswalk ||
      attr.value() == lanelet::AttributeValueString::Walkway) {
      const auto & crosswalk = lanelet;
      surrounding_crosswalks.push_back(crosswalk);
      if (std::abs(dist) < 1e-5) {
        crossing_crosswalk = crosswalk;
      }
    }
  }

  // If the object is in the crosswalk, generate path to the crosswalk edge
  if (crossing_crosswalk) {
    const auto edge_points = getCrosswalkEdgePoints(crossing_crosswalk.get());

    if (hasPotentialToReach(
          object, edge_points.front_center_point, edge_points.front_right_point,
          edge_points.front_left_point, std::numeric_limits<double>::max(),
          min_crosswalk_user_velocity_, max_crosswalk_user_delta_yaw_threshold_for_lanelet_)) {
      PredictedPath predicted_path =
        path_generator_->generatePathToTargetPoint(object, edge_points.front_center_point);
      predicted_path.confidence = 1.0;
      predicted_object.kinematics.predicted_paths.push_back(predicted_path);
    }

    if (hasPotentialToReach(
          object, edge_points.back_center_point, edge_points.back_right_point,
          edge_points.back_left_point, std::numeric_limits<double>::max(),
          min_crosswalk_user_velocity_, max_crosswalk_user_delta_yaw_threshold_for_lanelet_)) {
      PredictedPath predicted_path =
        path_generator_->generatePathToTargetPoint(object, edge_points.back_center_point);
      predicted_path.confidence = 1.0;
      predicted_object.kinematics.predicted_paths.push_back(predicted_path);
    }

    // If the object is not crossing the crosswalk, in the road lanelets, try to find the closest
    // crosswalk and generate path to the crosswalk edge
  } else if (utils::withinRoadLanelet(object, surrounding_lanelets_with_dist)) {
    lanelet::ConstLanelet closest_crosswalk{};
    const auto & obj_pose = object.kinematics.pose_with_covariance.pose;
    const auto found_closest_crosswalk =
      lanelet::utils::query::getClosestLanelet(crosswalks_, obj_pose, &closest_crosswalk);

    if (found_closest_crosswalk) {
      const auto edge_points = getCrosswalkEdgePoints(closest_crosswalk);

      if (hasPotentialToReach(
            object, edge_points.front_center_point, edge_points.front_right_point,
            edge_points.front_left_point, prediction_time_horizon_ * 2.0,
            min_crosswalk_user_velocity_, max_crosswalk_user_delta_yaw_threshold_for_lanelet_)) {
        PredictedPath predicted_path =
          path_generator_->generatePathToTargetPoint(object, edge_points.front_center_point);
        predicted_path.confidence = 1.0;
        predicted_object.kinematics.predicted_paths.push_back(predicted_path);
      }

      if (hasPotentialToReach(
            object, edge_points.back_center_point, edge_points.back_right_point,
            edge_points.back_left_point, prediction_time_horizon_ * 2.0,
            min_crosswalk_user_velocity_, max_crosswalk_user_delta_yaw_threshold_for_lanelet_)) {
        PredictedPath predicted_path =
          path_generator_->generatePathToTargetPoint(object, edge_points.back_center_point);
        predicted_path.confidence = 1.0;
        predicted_object.kinematics.predicted_paths.push_back(predicted_path);
      }
    }
  }

  // try to find the edge points for other surrounding crosswalks and generate path to the crosswalk
  // edge
  for (const auto & crosswalk : surrounding_crosswalks) {
    if (crossing_crosswalk && crossing_crosswalk.get() == crosswalk) {
      continue;
    }
    const auto crosswalk_signal_id_opt = getTrafficSignalId(crosswalk);
    if (crosswalk_signal_id_opt.has_value() && use_crosswalk_signal_) {
      if (!calcIntentionToCrossWithTrafficSignal(
            object, crosswalk, crosswalk_signal_id_opt.value())) {
        continue;
      }
    }

    const auto edge_points = getCrosswalkEdgePoints(crosswalk);

    const auto reachable_first = hasPotentialToReach(
      object, edge_points.front_center_point, edge_points.front_right_point,
      edge_points.front_left_point, prediction_time_horizon_, min_crosswalk_user_velocity_,
      max_crosswalk_user_delta_yaw_threshold_for_lanelet_);
    const auto reachable_second = hasPotentialToReach(
      object, edge_points.back_center_point, edge_points.back_right_point,
      edge_points.back_left_point, prediction_time_horizon_, min_crosswalk_user_velocity_,
      max_crosswalk_user_delta_yaw_threshold_for_lanelet_);

    if (!reachable_first && !reachable_second) {
      continue;
    }

    const auto reachable_crosswalk = isReachableCrosswalkEdgePoints(
      object, surrounding_lanelets, surrounding_crosswalks, edge_points, prediction_time_horizon_,
      min_crosswalk_user_velocity_);

    if (!reachable_crosswalk) {
      continue;
    }

    PredictedPath predicted_path = path_generator_->generatePathForCrosswalkUser(
      object, reachable_crosswalk.get(), prediction_time_horizon_);
    predicted_path.confidence = 1.0;

    if (predicted_path.path.empty()) {
      continue;
    }
    // If the predicted path to the crosswalk is crossing the fence, don't use it
    if (doesPathCrossAnyFence(predicted_path)) {
      continue;
    }

    predicted_object.kinematics.predicted_paths.push_back(predicted_path);
  }

  const auto n_path = predicted_object.kinematics.predicted_paths.size();
  for (auto & predicted_path : predicted_object.kinematics.predicted_paths) {
    predicted_path.confidence = 1.0 / n_path;
  }

  return predicted_object;
}

std::optional<lanelet::Id> PredictorVru::getTrafficSignalId(
  const lanelet::ConstLanelet & way_lanelet)
{
  const auto traffic_light_reg_elems =
    way_lanelet.regulatoryElementsAs<const lanelet::TrafficLight>();
  if (traffic_light_reg_elems.empty()) {
    return std::nullopt;
  } else if (traffic_light_reg_elems.size() > 1) {
    RCLCPP_ERROR(
      node_.get_logger(),
      "[Map Based Prediction]: "
      "Multiple regulatory elements as TrafficLight are defined to one lanelet object.");
  }
  return traffic_light_reg_elems.front()->id();
}

void PredictorVru::updateCrosswalkUserHistory(
  const std_msgs::msg::Header & header, const TrackedObject & object, const std::string & object_id)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  CrosswalkUserData crosswalk_user_data;
  crosswalk_user_data.header = header;
  crosswalk_user_data.tracked_object = object;

  if (crosswalk_users_history_.count(object_id) == 0) {
    crosswalk_users_history_.emplace(object_id, std::deque<CrosswalkUserData>{crosswalk_user_data});
    return;
  }

  crosswalk_users_history_.at(object_id).push_back(crosswalk_user_data);
}

std::string PredictorVru::tryMatchNewObjectToDisappeared(
  const std::string & object_id, std::unordered_map<std::string, TrackedObject> & current_users)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  const auto known_match_opt = [&]() -> std::optional<std::string> {
    if (!known_matches_.count(object_id)) {
      return std::nullopt;
    }

    std::string match_id = known_matches_[object_id];
    // object in the history is already matched to something (possibly itself)
    if (crosswalk_users_history_.count(match_id)) {
      // avoid matching two appeared users to one user in history
      current_users[match_id] = crosswalk_users_history_[match_id].back().tracked_object;
      return match_id;
    } else {
      RCLCPP_WARN_STREAM(
        node_.get_logger(), "Crosswalk user was "
                              << object_id << "was matched to " << match_id
                              << " but history for the crosswalk user was deleted. Rematching");
    }
    return std::nullopt;
  }();
  //  early return if the match is already known
  if (known_match_opt.has_value()) {
    return known_match_opt.value();
  }

  std::string match_id = object_id;
  double best_score = std::numeric_limits<double>::max();
  const auto object_pos = current_users[object_id].kinematics.pose_with_covariance.pose.position;
  for (const auto & [user_id, user_history] : crosswalk_users_history_) {
    // user present in current_users and will be matched to itself
    if (current_users.count(user_id)) {
      continue;
    }
    // TODO(dkoldaev): implement more sophisticated scoring, for now simply dst to last position in
    // history
    const auto match_candidate_pos =
      user_history.back().tracked_object.kinematics.pose_with_covariance.pose.position;
    const double score =
      std::hypot(match_candidate_pos.x - object_pos.x, match_candidate_pos.y - object_pos.y);
    if (score < best_score) {
      best_score = score;
      match_id = user_id;
    }
  }

  if (object_id != match_id) {
    RCLCPP_INFO_STREAM(
      node_.get_logger(), "[Map Based Prediction]: Matched " << object_id << " to " << match_id);
    // avoid matching two appeared users to one user in history
    current_users[match_id] = crosswalk_users_history_[match_id].back().tracked_object;
  }

  known_matches_[object_id] = match_id;
  return match_id;
}

bool PredictorVru::calcIntentionToCrossWithTrafficSignal(
  const TrackedObject & object, const lanelet::ConstLanelet & crosswalk,
  const lanelet::Id & signal_id)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  const auto signal_color = [&] {
    const auto elem_opt = getTrafficSignalElement(signal_id);
    return elem_opt ? elem_opt.value().color : TrafficLightElement::UNKNOWN;
  }();

  const auto key =
    std::make_pair(autoware::universe_utils::toHexString(object.object_id), signal_id);
  if (
    signal_color == TrafficLightElement::GREEN &&
    autoware::universe_utils::calcNorm(object.kinematics.twist_with_covariance.twist.linear) <
      threshold_velocity_assumed_as_stopping_) {
    stopped_times_against_green_.try_emplace(key, node_.get_clock()->now());

    const auto timeout_no_intention_to_walk = [&]() {
      auto InterpolateMap = [](
                              const std::vector<double> & key_set,
                              const std::vector<double> & value_set, const double query) {
        if (query <= key_set.front()) {
          return value_set.front();
        } else if (query >= key_set.back()) {
          return value_set.back();
        }
        for (size_t i = 0; i < key_set.size() - 1; ++i) {
          if (key_set.at(i) <= query && query <= key_set.at(i + 1)) {
            auto ratio =
              (query - key_set.at(i)) / std::max(key_set.at(i + 1) - key_set.at(i), 1.0e-5);
            ratio = std::clamp(ratio, 0.0, 1.0);
            return value_set.at(i) + ratio * (value_set.at(i + 1) - value_set.at(i));
          }
        }
        return value_set.back();
      };

      const auto obj_position = object.kinematics.pose_with_covariance.pose.position;
      const double distance_to_crosswalk = boost::geometry::distance(
        crosswalk.polygon2d().basicPolygon(),
        lanelet::BasicPoint2d(obj_position.x, obj_position.y));
      return InterpolateMap(
        distance_set_for_no_intention_to_walk_, timeout_set_for_no_intention_to_walk_,
        distance_to_crosswalk);
    }();

    if (
      (node_.get_clock()->now() - stopped_times_against_green_.at(key)).seconds() >
      timeout_no_intention_to_walk) {
      return false;
    }

  } else {
    stopped_times_against_green_.erase(key);
    // If the pedestrian disappears, another function erases the old data.
  }

  if (signal_color == TrafficLightElement::RED) {
    return false;
  }

  return true;
}

std::optional<TrafficLightElement> PredictorVru::getTrafficSignalElement(const lanelet::Id & id)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  if (traffic_signal_id_map_.count(id) != 0) {
    const auto & signal_elements = traffic_signal_id_map_.at(id).elements;
    if (signal_elements.size() > 1) {
      RCLCPP_ERROR(
        node_.get_logger(), "[Map Based Prediction]: Multiple TrafficSignalElement_ are received.");
    } else if (!signal_elements.empty()) {
      return signal_elements.front();
    }
  }
  return std::nullopt;
}

}  // namespace autoware::map_based_prediction

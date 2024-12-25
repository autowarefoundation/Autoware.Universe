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

#ifndef MAP_BASED_PREDICTION__DATA_STRUCTURE_HPP_
#define MAP_BASED_PREDICTION__DATA_STRUCTURE_HPP_

#include <autoware/universe_utils/system/stop_watch.hpp>

#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/tracked_object.hpp>
#include <autoware_perception_msgs/msg/tracked_objects.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group_array.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/header.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_routing/LaneletPath.h>

#include <unordered_map>
#include <utility>
#include <vector>
namespace autoware::map_based_prediction
{
using PosePath = std::vector<geometry_msgs::msg::Pose>;

struct PredictionTimeHorizon
{
  // NOTE(Mamoru Sobue): motorcycle belongs to "vehicle" and bicycle to "pedestrian"
  double vehicle;
  double pedestrian;
  double unknown;
};

struct LateralKinematicsToLanelet
{
  double dist_from_left_boundary;
  double dist_from_right_boundary;
  double left_lateral_velocity;
  double right_lateral_velocity;
  double filtered_left_lateral_velocity;
  double filtered_right_lateral_velocity;
};

enum class Maneuver {
  UNINITIALIZED = 0,
  LANE_FOLLOW = 1,
  LEFT_LANE_CHANGE = 2,
  RIGHT_LANE_CHANGE = 3,
};

struct LaneletData
{
  lanelet::Lanelet lanelet;
  double probability;
};

struct PredictedRefPath
{
  float probability;
  double speed_limit;
  double width;
  PosePath path;
  Maneuver maneuver;
};

struct ObjectData
{
  std_msgs::msg::Header header;
  lanelet::ConstLanelets current_lanelets;
  lanelet::ConstLanelets future_possible_lanelets;
  geometry_msgs::msg::Pose pose;
  geometry_msgs::msg::Twist twist;
  double time_delay;
  // for lane change prediction
  std::unordered_map<lanelet::ConstLanelet, LateralKinematicsToLanelet> lateral_kinematics_set;
  Maneuver one_shot_maneuver{Maneuver::UNINITIALIZED};
  Maneuver output_maneuver{
    Maneuver::UNINITIALIZED};  // output maneuver considering previous one shot maneuvers
};
struct CrosswalkUserData
{
  std_msgs::msg::Header header;
  autoware_perception_msgs::msg::TrackedObject tracked_object;
};

using LaneletsData = std::vector<LaneletData>;
using ManeuverProbability = std::unordered_map<Maneuver, float>;
using autoware::universe_utils::StopWatch;
using autoware_map_msgs::msg::LaneletMapBin;
using autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjectKinematics;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_perception_msgs::msg::PredictedPath;
using autoware_perception_msgs::msg::TrackedObject;
using autoware_perception_msgs::msg::TrackedObjectKinematics;
using autoware_perception_msgs::msg::TrackedObjects;
using autoware_perception_msgs::msg::TrafficLightElement;
using autoware_perception_msgs::msg::TrafficLightGroup;
using autoware_perception_msgs::msg::TrafficLightGroupArray;
using LaneletPathWithPathInfo = std::pair<lanelet::routing::LaneletPath, PredictedRefPath>;

}  // namespace autoware::map_based_prediction

#endif  // MAP_BASED_PREDICTION__DATA_STRUCTURE_HPP_

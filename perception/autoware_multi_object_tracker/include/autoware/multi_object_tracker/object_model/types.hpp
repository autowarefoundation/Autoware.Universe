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
//
//
// Author: v1.0 Taekjin Lee

#ifndef AUTOWARE__MULTI_OBJECT_TRACKER__OBJECT_MODEL__TYPES_HPP_
#define AUTOWARE__MULTI_OBJECT_TRACKER__OBJECT_MODEL__TYPES_HPP_

#include <autoware_perception_msgs/msg/detected_object.hpp>
#include <autoware_perception_msgs/msg/detected_object_kinematics.hpp>
#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <autoware_perception_msgs/msg/object_classification.hpp>
#include <autoware_perception_msgs/msg/shape.hpp>
#include <autoware_perception_msgs/msg/tracked_object.hpp>
#include <autoware_perception_msgs/msg/tracked_object_kinematics.hpp>
#include <geometry_msgs/msg/polygon.hpp>
#include <geometry_msgs/msg/pose_with_covariance.hpp>
#include <geometry_msgs/msg/twist_with_covariance.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <std_msgs/msg/header.hpp>
#include <unique_identifier_msgs/msg/uuid.hpp>

#include <boost/optional.hpp>

#include <vector>

namespace autoware::multi_object_tracker
{
namespace types
{
enum OrientationAvailability : uint8_t {
  UNAVAILABLE = 0,
  SIGN_UNKNOWN = 1,
  AVAILABLE = 2,
};

struct ObjectKinematics
{
  geometry_msgs::msg::PoseWithCovariance pose_with_covariance;
  geometry_msgs::msg::TwistWithCovariance twist_with_covariance;
  bool has_position_covariance = false;
  OrientationAvailability orientation_availability;
  bool has_twist = false;
  bool has_twist_covariance = false;
};

struct DynamicObject
{
  unique_identifier_msgs::msg::UUID object_id = unique_identifier_msgs::msg::UUID();
  uint channel_index;
  float existence_probability;
  std::vector<autoware_perception_msgs::msg::ObjectClassification> classification;
  ObjectKinematics kinematics;
  autoware_perception_msgs::msg::Shape shape;
};

struct DynamicObjectList
{
  std_msgs::msg::Header header;
  uint channel_index;
  std::vector<DynamicObject> objects;
};

DynamicObject toDynamicObject(
  const autoware_perception_msgs::msg::DetectedObject & det_object, const uint channel_index = 0);

DynamicObjectList toDynamicObjectList(
  const autoware_perception_msgs::msg::DetectedObjects & det_objects, const uint channel_index = 0);

autoware_perception_msgs::msg::TrackedObject toTrackedObjectMsg(const DynamicObject & dyn_object);

}  // namespace types

}  // namespace autoware::multi_object_tracker

#endif  // AUTOWARE__MULTI_OBJECT_TRACKER__OBJECT_MODEL__TYPES_HPP_

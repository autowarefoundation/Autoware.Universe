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
//
//
// Author: v1.0 Taekjin Lee

#include "autoware/multi_object_tracker/object_model/types.hpp"

#include <vector>

namespace autoware::multi_object_tracker
{

namespace types
{

DynamicObject toDynamicObject(
  const autoware_perception_msgs::msg::DetectedObject & det_object, const uint channel_index)
{
  DynamicObject dynamic_object;
  dynamic_object.channel_index = channel_index;
  dynamic_object.existence_probability = det_object.existence_probability;
  dynamic_object.classification = det_object.classification;

  dynamic_object.kinematics.pose_with_covariance = det_object.kinematics.pose_with_covariance;
  dynamic_object.kinematics.twist_with_covariance = det_object.kinematics.twist_with_covariance;
  dynamic_object.kinematics.has_position_covariance = det_object.kinematics.has_position_covariance;
  if (
    det_object.kinematics.orientation_availability ==
    autoware_perception_msgs::msg::DetectedObjectKinematics::UNAVAILABLE) {
    dynamic_object.kinematics.orientation_availability = OrientationAvailability::UNAVAILABLE;
  } else if (
    det_object.kinematics.orientation_availability ==
    autoware_perception_msgs::msg::DetectedObjectKinematics::SIGN_UNKNOWN) {
    dynamic_object.kinematics.orientation_availability = OrientationAvailability::SIGN_UNKNOWN;
  } else if (
    det_object.kinematics.orientation_availability ==
    autoware_perception_msgs::msg::DetectedObjectKinematics::AVAILABLE) {
    dynamic_object.kinematics.orientation_availability = OrientationAvailability::AVAILABLE;
  }
  dynamic_object.kinematics.has_twist = det_object.kinematics.has_twist;
  dynamic_object.kinematics.has_twist_covariance = det_object.kinematics.has_twist_covariance;

  dynamic_object.shape = det_object.shape;

  return dynamic_object;
}

DynamicObjectList toDynamicObjectList(
  const autoware_perception_msgs::msg::DetectedObjects & det_objects, const uint channel_index)
{
  DynamicObjectList dynamic_objects;
  dynamic_objects.header = det_objects.header;
  dynamic_objects.channel_index = channel_index;
  dynamic_objects.objects.reserve(det_objects.objects.size());
  for (const auto & det_object : det_objects.objects) {
    dynamic_objects.objects.emplace_back(toDynamicObject(det_object, channel_index));
  }
  return dynamic_objects;
}

autoware_perception_msgs::msg::TrackedObject toTrackedObjectMsg(const DynamicObject & dyn_object)
{
  autoware_perception_msgs::msg::TrackedObject tracked_object;
  tracked_object.object_id = dyn_object.object_id;
  tracked_object.existence_probability = dyn_object.existence_probability;
  tracked_object.classification = dyn_object.classification;

  tracked_object.kinematics.pose_with_covariance = dyn_object.kinematics.pose_with_covariance;
  tracked_object.kinematics.twist_with_covariance = dyn_object.kinematics.twist_with_covariance;
  if (dyn_object.kinematics.orientation_availability == OrientationAvailability::UNAVAILABLE) {
    tracked_object.kinematics.orientation_availability =
      autoware_perception_msgs::msg::TrackedObjectKinematics::UNAVAILABLE;
  } else if (
    dyn_object.kinematics.orientation_availability == OrientationAvailability::SIGN_UNKNOWN) {
    tracked_object.kinematics.orientation_availability =
      autoware_perception_msgs::msg::TrackedObjectKinematics::SIGN_UNKNOWN;
  } else if (dyn_object.kinematics.orientation_availability == OrientationAvailability::AVAILABLE) {
    tracked_object.kinematics.orientation_availability =
      autoware_perception_msgs::msg::TrackedObjectKinematics::AVAILABLE;
  }
  tracked_object.kinematics.is_stationary = false;

  tracked_object.shape = dyn_object.shape;

  return tracked_object;
}
}  // namespace types

}  // namespace autoware::multi_object_tracker

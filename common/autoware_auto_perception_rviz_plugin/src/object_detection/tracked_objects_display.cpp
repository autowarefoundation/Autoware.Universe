// Copyright 2021 Apex.AI, Inc.
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
// Co-developed by Tier IV, Inc. and Apex.AI, Inc.

#include "autoware_auto_perception_rviz_plugin/object_detection/tracked_objects_display.hpp"

#include <memory>

namespace autoware
{
namespace rviz_plugins
{
namespace object_detection
{
TrackedObjectsDisplay::TrackedObjectsDisplay() : ObjectPolygonDisplayBase("tracks")
{
  // Option for selective visualization by object dynamics
  m_select_object_dynamics_property = new rviz_common::properties::EnumProperty(
    "Dynamic Status", "All", "Selectively visualize objects by its dynamic status.", this);
  m_select_object_dynamics_property->addOption("Dynamic", 0);
  m_select_object_dynamics_property->addOption("Static", 1);
  m_select_object_dynamics_property->addOption("All", 2);
}

bool TrackedObjectsDisplay::is_object_to_show(
  const uint showing_dynamic_status, const TrackedObject & object)
{
  if (showing_dynamic_status == 0 && object.kinematics.is_stationary) {
    return false;  // Show only moving objects
  }
  if (showing_dynamic_status == 1 && !object.kinematics.is_stationary) {
    return false;  // Show only stationary objects
  }
  return true;
}

void TrackedObjectsDisplay::processMessage(TrackedObjects::ConstSharedPtr msg)
{
  clear_markers();
  update_id_map(msg);

  const auto showing_dynamic_status = get_object_dynamics_to_visualize();
  for (const auto & object : msg->objects) {
    // Filter by object dynamic status
    if (!is_object_to_show(showing_dynamic_status, object)) continue;
    // Get marker for shape
    auto shape_marker = get_shape_marker_ptr(
      object.shape, object.kinematics.pose_with_covariance.pose.position,
      object.kinematics.pose_with_covariance.pose.orientation, object.classification,
      get_line_width(),
      object.kinematics.orientation_availability ==
        autoware_auto_perception_msgs::msg::DetectedObjectKinematics::AVAILABLE);
    if (shape_marker) {
      auto shape_marker_ptr = shape_marker.value();
      shape_marker_ptr->header = msg->header;
      shape_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(shape_marker_ptr);
    }

    // Get marker for label
    auto label_marker = get_label_marker_ptr(
      object.kinematics.pose_with_covariance.pose.position,
      object.kinematics.pose_with_covariance.pose.orientation, object.classification);
    if (label_marker) {
      auto label_marker_ptr = label_marker.value();
      label_marker_ptr->header = msg->header;
      label_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(label_marker_ptr);
    }

    // Get marker for id
    geometry_msgs::msg::Point uuid_vis_position;
    uuid_vis_position.x = object.kinematics.pose_with_covariance.pose.position.x - 0.5;
    uuid_vis_position.y = object.kinematics.pose_with_covariance.pose.position.y;
    uuid_vis_position.z = object.kinematics.pose_with_covariance.pose.position.z - 0.5;

    auto id_marker =
      get_uuid_marker_ptr(object.object_id, uuid_vis_position, object.classification);
    if (id_marker) {
      auto id_marker_ptr = id_marker.value();
      id_marker_ptr->header = msg->header;
      id_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(id_marker_ptr);
    }

    // Get marker for pose with covariance
    auto pose_with_covariance_marker = get_pose_with_covariance_marker_ptr(
      object.kinematics.pose_with_covariance, get_line_width() * 0.5);
    if (pose_with_covariance_marker) {
      auto marker_ptr = pose_with_covariance_marker.value();
      marker_ptr->header = msg->header;
      marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(marker_ptr);
    }

    // Get marker for yaw covariance
    auto yaw_covariance_marker =
      get_yaw_covariance_marker_ptr(object.kinematics.pose_with_covariance, object.shape.dimensions.x * 0.65, get_line_width() * 0.5);
    if (yaw_covariance_marker) {
      auto marker_ptr = yaw_covariance_marker.value();
      marker_ptr->header = msg->header;
      marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(marker_ptr);
    }

    // Get marker for existence probability
    geometry_msgs::msg::Point existence_probability_position;
    existence_probability_position.x = object.kinematics.pose_with_covariance.pose.position.x + 0.5;
    existence_probability_position.y = object.kinematics.pose_with_covariance.pose.position.y;
    existence_probability_position.z = object.kinematics.pose_with_covariance.pose.position.z + 0.5;
    const float existence_probability = object.existence_probability;
    auto existence_prob_marker = get_existence_probability_marker_ptr(
      existence_probability_position, object.kinematics.pose_with_covariance.pose.orientation,
      existence_probability, object.classification);
    if (existence_prob_marker) {
      auto existence_prob_marker_ptr = existence_prob_marker.value();
      existence_prob_marker_ptr->header = msg->header;
      existence_prob_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(existence_prob_marker_ptr);
    }
    // Get marker for velocity text
    geometry_msgs::msg::Point vel_vis_position;
    vel_vis_position.x = uuid_vis_position.x - 0.5;
    vel_vis_position.y = uuid_vis_position.y;
    vel_vis_position.z = uuid_vis_position.z - 0.5;
    auto velocity_text_marker = get_velocity_text_marker_ptr(
      object.kinematics.twist_with_covariance.twist, vel_vis_position, object.classification);
    if (velocity_text_marker) {
      auto velocity_text_marker_ptr = velocity_text_marker.value();
      velocity_text_marker_ptr->header = msg->header;
      velocity_text_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(velocity_text_marker_ptr);
    }

    // Get marker for acceleration text
    geometry_msgs::msg::Point acc_vis_position;
    acc_vis_position.x = uuid_vis_position.x - 1.0;
    acc_vis_position.y = uuid_vis_position.y;
    acc_vis_position.z = uuid_vis_position.z - 1.0;
    auto acceleration_text_marker = get_acceleration_text_marker_ptr(
      object.kinematics.acceleration_with_covariance.accel, acc_vis_position,
      object.classification);
    if (acceleration_text_marker) {
      auto acceleration_text_marker_ptr = acceleration_text_marker.value();
      acceleration_text_marker_ptr->header = msg->header;
      acceleration_text_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(acceleration_text_marker_ptr);
    }

    // Get marker for twist
    auto twist_marker = get_twist_marker_ptr(
      object.kinematics.pose_with_covariance, object.kinematics.twist_with_covariance,
      get_line_width());
    if (twist_marker) {
      auto twist_marker_ptr = twist_marker.value();
      twist_marker_ptr->header = msg->header;
      twist_marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(twist_marker_ptr);
    }

    // Get marker for twist covariance
    auto twist_covariance_marker = get_twist_covariance_marker_ptr(
      object.kinematics.pose_with_covariance, object.kinematics.twist_with_covariance,
      get_line_width() * 0.5);
    if (twist_covariance_marker) {
      auto marker_ptr = twist_covariance_marker.value();
      marker_ptr->header = msg->header;
      marker_ptr->id = uuid_to_marker_id(object.object_id);
      add_marker(marker_ptr);
    }

  }
}

}  // namespace object_detection
}  // namespace rviz_plugins
}  // namespace autoware

// Export the plugin
#include <pluginlib/class_list_macros.hpp>  // NOLINT
PLUGINLIB_EXPORT_CLASS(
  autoware::rviz_plugins::object_detection::TrackedObjectsDisplay, rviz_common::Display)

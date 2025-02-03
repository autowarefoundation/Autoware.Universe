// Copyright 2023 TIER IV, Inc.
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

// Copyright 2024 AutoCore, Inc.
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

#include "bytetrack3D/bytetrack3D.hpp"

#include <bytetrack3D/bytetrack3D_node.hpp>
#include <rclcpp/qos.hpp>

#include "autoware_perception_msgs/msg/object_classification.hpp"

#include <rmw/qos_profiles.h>

#include <utility>
#include <vector>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

namespace bytetrack3D
{
ByteTrack3DNode::ByteTrack3DNode(const rclcpp::NodeOptions & node_options)
: Node("bytetrack3D", node_options)
{
  using std::placeholders::_1;
  using std::chrono_literals::operator""ms;

  int track_buffer_length = declare_parameter("track_buffer_length", 30);

  this->bytetrack3D_ = std::make_unique<bytetrack3D::ByteTrack3D>(track_buffer_length);

  detection_rect_sub_ = this->create_subscription<autoware_perception_msgs::msg::DetectedObjects>(
    "~/in/objects", 1, std::bind(&ByteTrack3DNode::on_rect, this, _1));

  objects_pub_ =
    this->create_publisher<autoware_perception_msgs::msg::TrackedObjects>("~/out/objects", 1);
}

void ByteTrack3DNode::on_rect(
  const autoware_perception_msgs::msg::DetectedObjects::ConstSharedPtr msg)
{
  using Label = autoware_perception_msgs::msg::ObjectClassification;

  autoware_perception_msgs::msg::TrackedObjects out_objects;

  // Unpack detection results
  bytetrack3D::ObjectArray object_array;
  for (const auto & feat_obj : msg->objects) {
    Object obj;
    obj.x = feat_obj.kinematics.pose_with_covariance.pose.position.x;
    obj.y = feat_obj.kinematics.pose_with_covariance.pose.position.y;
    obj.z = feat_obj.kinematics.pose_with_covariance.pose.position.z;
    auto q = feat_obj.kinematics.pose_with_covariance.pose.orientation;
    obj.yaw =
      std::atan2(2.0 * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
    obj.l = feat_obj.shape.dimensions.x;
    obj.w = feat_obj.shape.dimensions.y;
    obj.h = feat_obj.shape.dimensions.z;
    obj.score = feat_obj.existence_probability;
    obj.type = feat_obj.classification.front().label;
    object_array.emplace_back(obj);
  }

  bytetrack3D::ObjectArray objects = bytetrack3D_->update_tracker(object_array);
  for (const auto & tracked_object : objects) {
    autoware_perception_msgs::msg::TrackedObject object;
    unique_identifier_msgs::msg::UUID uuid_msg;
    auto tracked_uuid = tracked_object.unique_id;
    std::memcpy(uuid_msg.uuid.data(), &tracked_uuid, tracked_uuid.size());
    object.object_id = uuid_msg;
    object.existence_probability = tracked_object.score;
    object.classification.emplace_back(
      autoware_perception_msgs::build<Label>().label(tracked_object.type).probability(1.0f));
    object.kinematics.pose_with_covariance.pose.position.x = tracked_object.x;
    object.kinematics.pose_with_covariance.pose.position.y = tracked_object.y;
    object.kinematics.pose_with_covariance.pose.position.z = tracked_object.z;
    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, tracked_object.yaw);
    object.kinematics.pose_with_covariance.pose.orientation = tf2::toMsg(q);
    object.shape.dimensions.x = tracked_object.l;
    object.shape.dimensions.y = tracked_object.w;
    object.shape.dimensions.z = tracked_object.h;
    object.kinematics.twist_with_covariance.twist.linear.x = tracked_object.vx;
    object.kinematics.twist_with_covariance.twist.linear.y = tracked_object.vy;
    object.kinematics.twist_with_covariance.twist.linear.z = tracked_object.vz;
    object.kinematics.twist_with_covariance.twist.angular.z = tracked_object.vyaw;

    out_objects.objects.push_back(object);
  }

  out_objects.header = msg->header;
  objects_pub_->publish(out_objects);
}
}  // namespace bytetrack3D

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(bytetrack3D::ByteTrack3DNode)

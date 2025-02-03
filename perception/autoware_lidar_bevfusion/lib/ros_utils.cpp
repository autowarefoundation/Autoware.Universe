// Copyright 2025 TIER IV, Inc.
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

#include "autoware/lidar_bevfusion/ros_utils.hpp"

#include <autoware/object_recognition_utils/object_recognition_utils.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/math/constants.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace autoware::lidar_bevfusion
{

using Label = autoware_perception_msgs::msg::ObjectClassification;

bool is_data_layout_compatible_with_point_xyzirc(const sensor_msgs::msg::PointCloud2 & input)
{
  using PointIndex = autoware::point_types::PointXYZIRCIndex;
  using autoware::point_types::PointXYZIRC;
  if (input.fields.size() < 6) {
    return false;
  }
  bool same_layout = true;
  const auto & field_x = input.fields.at(static_cast<size_t>(PointIndex::X));
  same_layout &= field_x.name == "x";
  same_layout &= field_x.offset == offsetof(PointXYZIRC, x);
  same_layout &= field_x.datatype == sensor_msgs::msg::PointField::FLOAT32;
  same_layout &= field_x.count == 1;
  const auto & field_y = input.fields.at(static_cast<size_t>(PointIndex::Y));
  same_layout &= field_y.name == "y";
  same_layout &= field_y.offset == offsetof(PointXYZIRC, y);
  same_layout &= field_y.datatype == sensor_msgs::msg::PointField::FLOAT32;
  same_layout &= field_y.count == 1;
  const auto & field_z = input.fields.at(static_cast<size_t>(PointIndex::Z));
  same_layout &= field_z.name == "z";
  same_layout &= field_z.offset == offsetof(PointXYZIRC, z);
  same_layout &= field_z.datatype == sensor_msgs::msg::PointField::FLOAT32;
  same_layout &= field_z.count == 1;
  const auto & field_intensity = input.fields.at(static_cast<size_t>(PointIndex::Intensity));
  same_layout &= field_intensity.name == "intensity";
  same_layout &= field_intensity.offset == offsetof(PointXYZIRC, intensity);
  same_layout &= field_intensity.datatype == sensor_msgs::msg::PointField::UINT8;
  same_layout &= field_intensity.count == 1;
  const auto & field_return_type = input.fields.at(static_cast<size_t>(PointIndex::ReturnType));
  same_layout &= field_return_type.name == "return_type";
  same_layout &= field_return_type.offset == offsetof(PointXYZIRC, return_type);
  same_layout &= field_return_type.datatype == sensor_msgs::msg::PointField::UINT8;
  same_layout &= field_return_type.count == 1;
  const auto & field_ring = input.fields.at(static_cast<size_t>(PointIndex::Channel));
  same_layout &= field_ring.name == "channel";
  same_layout &= field_ring.offset == offsetof(PointXYZIRC, channel);
  same_layout &= field_ring.datatype == sensor_msgs::msg::PointField::UINT16;
  same_layout &= field_ring.count == 1;

  return same_layout;
}

void box3DToDetectedObject(
  const Box3D & box3d, const std::vector<std::string> & class_names,
  autoware_perception_msgs::msg::DetectedObject & obj)
{
  obj.existence_probability = box3d.score;

  // classification
  autoware_perception_msgs::msg::ObjectClassification classification;
  classification.probability = 1.0f;
  if (box3d.label >= 0 && static_cast<std::size_t>(box3d.label) < class_names.size()) {
    classification.label = getSemanticType(class_names[box3d.label]);
  } else {
    classification.label = Label::UNKNOWN;
    RCLCPP_WARN_STREAM(rclcpp::get_logger("lidar_bevfusion"), "Unexpected label: UNKNOWN is set.");
  }

  if (autoware::object_recognition_utils::isCarLikeVehicle(classification.label)) {
    obj.kinematics.orientation_availability =
      autoware_perception_msgs::msg::DetectedObjectKinematics::SIGN_UNKNOWN;
  }

  obj.classification.emplace_back(classification);

  // pose and shape
  obj.kinematics.pose_with_covariance.pose.position =
    autoware::universe_utils::createPoint(box3d.x, box3d.y, box3d.z);
  obj.kinematics.pose_with_covariance.pose.orientation =
    autoware::universe_utils::createQuaternionFromYaw(box3d.yaw);
  obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  obj.shape.dimensions =
    autoware::universe_utils::createTranslation(box3d.length, box3d.width, box3d.height);
}

std::uint8_t getSemanticType(const std::string & class_name)
{
  if (class_name == "CAR") {
    return Label::CAR;
  } else if (class_name == "TRUCK") {
    return Label::TRUCK;
  } else if (class_name == "BUS") {
    return Label::BUS;
  } else if (class_name == "TRAILER") {
    return Label::TRAILER;
  } else if (class_name == "MOTORCYCLE") {
    return Label::MOTORCYCLE;
  } else if (class_name == "BICYCLE") {
    return Label::BICYCLE;
  } else if (class_name == "PEDESTRIAN") {
    return Label::PEDESTRIAN;
  } else {  // CONSTRUCTION_VEHICLE, BARRIER, TRAFFIC_CONE
    return Label::UNKNOWN;
  }
}

}  // namespace autoware::lidar_bevfusion

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

#ifndef LIDAR_TRANSFUSION__ROS_UTILS_HPP_
#define LIDAR_TRANSFUSION__ROS_UTILS_HPP_

#include "lidar_transfusion/utils.hpp"

#include <autoware_point_types/types.hpp>

#include <autoware_perception_msgs/msg/detected_object_kinematics.hpp>
#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <autoware_perception_msgs/msg/object_classification.hpp>
#include <autoware_perception_msgs/msg/shape.hpp>
#include <sensor_msgs/msg/point_field.hpp>

#include <cstdint>
#include <string>
#include <vector>

#define CHECK_OFFSET(structure1, structure2, field)             \
  static_assert(                                                \
    offsetof(structure1, field) == offsetof(structure2, field), \
    "Offset of " #field " in " #structure1 " does not match expected offset.")
#define CHECK_TYPE(structure1, structure2, field)                                  \
  static_assert(                                                                   \
    std::is_same<decltype(structure1::field), decltype(structure2::field)>::value, \
    "Type of " #field " in " #structure1 " does not match expected type.")
#define CHECK_FIELD(structure1, structure2, field) \
  CHECK_OFFSET(structure1, structure2, field);     \
  CHECK_TYPE(structure1, structure2, field)

namespace lidar_transfusion
{
using sensor_msgs::msg::PointField;

struct TransfusionInput
{
  float x;
  float y;
  float z;
  uint8_t intensity;
};

CHECK_FIELD(TransfusionInput, autoware_point_types::PointXYZIRCAEDT, x);
CHECK_FIELD(TransfusionInput, autoware_point_types::PointXYZIRCAEDT, y);
CHECK_FIELD(TransfusionInput, autoware_point_types::PointXYZIRCAEDT, z);
CHECK_FIELD(TransfusionInput, autoware_point_types::PointXYZIRCAEDT, intensity);

struct CloudInfo
{
  uint32_t x_offset{0};
  uint32_t y_offset{sizeof(TransfusionInput::x)};
  uint32_t z_offset{sizeof(TransfusionInput::x) + sizeof(TransfusionInput::y)};
  uint32_t intensity_offset{
    sizeof(TransfusionInput::x) + sizeof(TransfusionInput::y) + sizeof(TransfusionInput::z)};
  uint8_t x_datatype{PointField::FLOAT32};
  uint8_t y_datatype{PointField::FLOAT32};
  uint8_t z_datatype{PointField::FLOAT32};
  uint8_t intensity_datatype{PointField::UINT8};
  uint8_t x_count{1};
  uint8_t y_count{1};
  uint8_t z_count{1};
  uint8_t intensity_count{1};
  uint32_t point_step{sizeof(autoware_point_types::PointXYZIRCAEDT)};
  bool is_bigendian{false};

  bool operator!=(const CloudInfo & rhs) const
  {
    return x_offset != rhs.x_offset || y_offset != rhs.y_offset || z_offset != rhs.z_offset ||
           intensity_offset != rhs.intensity_offset || x_datatype != rhs.x_datatype ||
           y_datatype != rhs.y_datatype || z_datatype != rhs.z_datatype ||
           intensity_datatype != rhs.intensity_datatype || x_count != rhs.x_count ||
           y_count != rhs.y_count || z_count != rhs.z_count ||
           intensity_count != rhs.intensity_count || is_bigendian != rhs.is_bigendian;
  }

  friend std::ostream & operator<<(std::ostream & os, const CloudInfo & info)
  {
    os << "x_offset: " << static_cast<int>(info.x_offset) << std::endl;
    os << "y_offset: " << static_cast<int>(info.y_offset) << std::endl;
    os << "z_offset: " << static_cast<int>(info.z_offset) << std::endl;
    os << "intensity_offset: " << static_cast<int>(info.intensity_offset) << std::endl;
    os << "x_datatype: " << static_cast<int>(info.x_datatype) << std::endl;
    os << "y_datatype: " << static_cast<int>(info.y_datatype) << std::endl;
    os << "z_datatype: " << static_cast<int>(info.z_datatype) << std::endl;
    os << "intensity_datatype: " << static_cast<int>(info.intensity_datatype) << std::endl;
    os << "x_count: " << static_cast<int>(info.x_count) << std::endl;
    os << "y_count: " << static_cast<int>(info.y_count) << std::endl;
    os << "z_count: " << static_cast<int>(info.z_count) << std::endl;
    os << "intensity_count: " << static_cast<int>(info.intensity_count) << std::endl;
    os << "is_bigendian: " << static_cast<int>(info.is_bigendian) << std::endl;
    return os;
  }
};

void box3DToDetectedObject(
  const Box3D & box3d, const std::vector<std::string> & class_names,
  autoware_perception_msgs::msg::DetectedObject & obj);

uint8_t getSemanticType(const std::string & class_name);

}  // namespace lidar_transfusion

#endif  // LIDAR_TRANSFUSION__ROS_UTILS_HPP_

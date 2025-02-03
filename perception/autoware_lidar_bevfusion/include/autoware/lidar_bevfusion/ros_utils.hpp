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

#ifndef AUTOWARE__LIDAR_BEVFUSION__ROS_UTILS_HPP_
#define AUTOWARE__LIDAR_BEVFUSION__ROS_UTILS_HPP_

#include "autoware/lidar_bevfusion/preprocess/point_type.hpp"
#include "autoware/lidar_bevfusion/utils.hpp"

#include <autoware/point_types/types.hpp>

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
    "Offset of " #field " in " #structure1 " does not match the one in " #structure2 ".")
#define CHECK_TYPE(structure1, structure2, field)                             \
  static_assert(                                                              \
    std::is_same_v<decltype(structure1::field), decltype(structure2::field)>, \
    "Type of " #field " in " #structure1 " and " #structure1 " have different types.")
#define CHECK_FIELD(structure1, structure2, field) \
  CHECK_OFFSET(structure1, structure2, field);     \
  CHECK_TYPE(structure1, structure2, field)

namespace autoware::lidar_bevfusion
{
using sensor_msgs::msg::PointField;

CHECK_FIELD(InputPointType, autoware::point_types::PointXYZIRC, x);
CHECK_FIELD(InputPointType, autoware::point_types::PointXYZIRC, y);
CHECK_FIELD(InputPointType, autoware::point_types::PointXYZIRC, z);
CHECK_FIELD(InputPointType, autoware::point_types::PointXYZIRC, intensity);
static_assert(sizeof(InputPointType) == sizeof(autoware::point_types::PointXYZIRC));

// TODO(knzo25): will move this from the pointcloud preprocessor to aut autoware_point_types after
// this is merged
bool is_data_layout_compatible_with_point_xyzirc(const sensor_msgs::msg::PointCloud2 & input);

void box3DToDetectedObject(
  const Box3D & box3d, const std::vector<std::string> & class_names,
  autoware_perception_msgs::msg::DetectedObject & obj);

uint8_t getSemanticType(const std::string & class_name);

}  // namespace autoware::lidar_bevfusion

#endif  // AUTOWARE__LIDAR_BEVFUSION__ROS_UTILS_HPP_

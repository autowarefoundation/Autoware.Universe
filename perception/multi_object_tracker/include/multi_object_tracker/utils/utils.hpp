// Copyright 2020 Tier IV, Inc.
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
// Author: v1.0 Yukihiro Saito
//

#ifndef MULTI_OBJECT_TRACKER__UTILS__UTILS_HPP_
#define MULTI_OBJECT_TRACKER__UTILS__UTILS_HPP_

#include <tier4_autoware_utils/tier4_autoware_utils.hpp>

#include <autoware_auto_perception_msgs/msg/detected_object.hpp>
#include <autoware_auto_perception_msgs/msg/shape.hpp>
#include <autoware_auto_perception_msgs/msg/tracked_object.hpp>
#include <geometry_msgs/msg/polygon.hpp>
#include <geometry_msgs/msg/vector3.hpp>

#include <cmath>
#include <tuple>
#include <vector>

namespace utils
{
double getPolygonArea(const geometry_msgs::msg::Polygon & footprint);
double getRectangleArea(const geometry_msgs::msg::Vector3 & dimensions);
double getCircleArea(const geometry_msgs::msg::Vector3 & dimensions);
double getArea(const autoware_auto_perception_msgs::msg::Shape & shape);
double get2dIoU(
  const std::tuple<
    const geometry_msgs::msg::Pose &, const autoware_auto_perception_msgs::msg::Shape &> & object1,
  const std::tuple<
    const geometry_msgs::msg::Pose &, const autoware_auto_perception_msgs::msg::Shape &> & object2);
double get2dIoU(
  const autoware_auto_perception_msgs::msg::TrackedObject & object1,
  const autoware_auto_perception_msgs::msg::TrackedObject & object2);
std::uint8_t getHighestProbLabel(
  const std::vector<autoware_auto_perception_msgs::msg::ObjectClassification> & classification);
autoware_auto_perception_msgs::msg::TrackedObject toTrackedObject(
  const autoware_auto_perception_msgs::msg::DetectedObject & detected_object);
geometry_msgs::msg::Polygon rotatePolygon(
  const geometry_msgs::msg::Polygon & polygon, const double & angle);

enum MSG_COV_IDX {
  X_X = 0,
  X_Y = 1,
  X_Z = 2,
  X_ROLL = 3,
  X_PITCH = 4,
  X_YAW = 5,
  Y_X = 6,
  Y_Y = 7,
  Y_Z = 8,
  Y_ROLL = 9,
  Y_PITCH = 10,
  Y_YAW = 11,
  Z_X = 12,
  Z_Y = 13,
  Z_Z = 14,
  Z_ROLL = 15,
  Z_PITCH = 16,
  Z_YAW = 17,
  ROLL_X = 18,
  ROLL_Y = 19,
  ROLL_Z = 20,
  ROLL_ROLL = 21,
  ROLL_PITCH = 22,
  ROLL_YAW = 23,
  PITCH_X = 24,
  PITCH_Y = 25,
  PITCH_Z = 26,
  PITCH_ROLL = 27,
  PITCH_PITCH = 28,
  PITCH_YAW = 29,
  YAW_X = 30,
  YAW_Y = 31,
  YAW_Z = 32,
  YAW_ROLL = 33,
  YAW_PITCH = 34,
  YAW_YAW = 35
};
}  // namespace utils

#endif  // MULTI_OBJECT_TRACKER__UTILS__UTILS_HPP_

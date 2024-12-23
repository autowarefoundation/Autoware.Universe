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

#ifndef AUTOWARE__MULTI_OBJECT_TRACKER__UTILS__UTILS_HPP_
#define AUTOWARE__MULTI_OBJECT_TRACKER__UTILS__UTILS_HPP_

#include "autoware/multi_object_tracker/object_model/types.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <autoware_perception_msgs/msg/shape.hpp>

#include <tf2/utils.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace autoware::multi_object_tracker
{
namespace utils
{
enum BBOX_IDX {
  FRONT_SURFACE = 0,
  RIGHT_SURFACE = 1,
  REAR_SURFACE = 2,
  LEFT_SURFACE = 3,
  FRONT_R_CORNER = 4,
  REAR_R_CORNER = 5,
  REAR_L_CORNER = 6,
  FRONT_L_CORNER = 7,
  INSIDE = 8,
  INVALID = -1
};

/**
 * @brief Determine the Nearest Corner or Surface of detected object observed from ego vehicle
 *
 * @param x: object x coordinate in map frame
 * @param y: object y coordinate in map frame
 * @param yaw: object yaw orientation in map frame
 * @param width: object bounding box width
 * @param length: object bounding box length
 * @param self_transform: Ego vehicle position in map frame
 * @return int index
 */
inline int getNearestCornerOrSurface(
  const double x, const double y, const double yaw, const double width, const double length,
  const geometry_msgs::msg::Transform & self_transform)
{
  // get local vehicle pose
  const double x0 = self_transform.translation.x;
  const double y0 = self_transform.translation.y;

  // localize self vehicle pose to object coordinate
  // R.T (X0-X)
  const double xl = std::cos(yaw) * (x0 - x) + std::sin(yaw) * (y0 - y);
  const double yl = -std::sin(yaw) * (x0 - x) + std::cos(yaw) * (y0 - y);

  // Determine Index
  //     x+ (front)
  //         __
  // y+     |  | y-
  // (left) |  | (right)
  //         --
  //     x- (rear)
  int xgrid = 0;
  int ygrid = 0;
  const int labels[3][3] = {
    {BBOX_IDX::FRONT_L_CORNER, BBOX_IDX::FRONT_SURFACE, BBOX_IDX::FRONT_R_CORNER},
    {BBOX_IDX::LEFT_SURFACE, BBOX_IDX::INSIDE, BBOX_IDX::RIGHT_SURFACE},
    {BBOX_IDX::REAR_L_CORNER, BBOX_IDX::REAR_SURFACE, BBOX_IDX::REAR_R_CORNER}};
  if (xl > length / 2.0) {
    xgrid = 0;  // front
  } else if (xl > -length / 2.0) {
    xgrid = 1;  // middle
  } else {
    xgrid = 2;  // rear
  }
  if (yl > width / 2.0) {
    ygrid = 0;  // left
  } else if (yl > -width / 2.0) {
    ygrid = 1;  // middle
  } else {
    ygrid = 2;  // right
  }

  return labels[xgrid][ygrid];  // 0 to 7 + 1(null) value
}

/**
 * @brief Calc bounding box center offset caused by shape change
 * @param dw: width update [m] =  w_new - w_old
 * @param dl: length update [m] = l_new - l_old
 * @param indx: nearest corner index
 * @return 2d offset vector caused by shape change
 */
inline Eigen::Vector2d calcOffsetVectorFromShapeChange(
  const double dw, const double dl, const int indx)
{
  Eigen::Vector2d offset;
  // if surface
  if (indx == BBOX_IDX::FRONT_SURFACE) {
    offset(0, 0) = dl / 2.0;  // move forward
    offset(1, 0) = 0;
  } else if (indx == BBOX_IDX::RIGHT_SURFACE) {
    offset(0, 0) = 0;
    offset(1, 0) = -dw / 2.0;  // move right
  } else if (indx == BBOX_IDX::REAR_SURFACE) {
    offset(0, 0) = -dl / 2.0;  // move backward
    offset(1, 0) = 0;
  } else if (indx == BBOX_IDX::LEFT_SURFACE) {
    offset(0, 0) = 0;
    offset(1, 0) = dw / 2.0;  // move left
  }
  // if corner
  if (indx == BBOX_IDX::FRONT_R_CORNER) {
    offset(0, 0) = dl / 2.0;   // move forward
    offset(1, 0) = -dw / 2.0;  // move right
  } else if (indx == BBOX_IDX::REAR_R_CORNER) {
    offset(0, 0) = -dl / 2.0;  // move backward
    offset(1, 0) = -dw / 2.0;  // move right
  } else if (indx == BBOX_IDX::REAR_L_CORNER) {
    offset(0, 0) = -dl / 2.0;  // move backward
    offset(1, 0) = dw / 2.0;   // move left
  } else if (indx == BBOX_IDX::FRONT_L_CORNER) {
    offset(0, 0) = dl / 2.0;  // move forward
    offset(1, 0) = dw / 2.0;  // move left
  }
  return offset;  // do nothing if indx == INVALID or INSIDE
}

/**
 * @brief Convert input object center to tracking point based on nearest corner information
 * 1. update anchor offset vector, 2. offset input bbox based on tracking_offset vector and
 * prediction yaw angle
 * @param w: last input bounding box width
 * @param l: last input bounding box length
 * @param indx: last input bounding box closest corner index
 * @param input_object: input object bounding box
 * @param yaw: current yaw estimation
 * @param offset_object: output tracking measurement to feed ekf
 * @return nearest corner index(int)
 */
inline void calcAnchorPointOffset(
  const double w, const double l, const int indx, const types::DynamicObject & input_object,
  const double & yaw, types::DynamicObject & offset_object, Eigen::Vector2d & tracking_offset)
{
  // copy value
  offset_object = input_object;
  // invalid index
  if (indx == BBOX_IDX::INSIDE) {
    return;  // do nothing
  }

  // current object width and height
  const double w_n = input_object.shape.dimensions.y;
  const double l_n = input_object.shape.dimensions.x;

  // update offset
  const Eigen::Vector2d offset = calcOffsetVectorFromShapeChange(w_n - w, l_n - l, indx);
  tracking_offset = offset;

  // offset input object
  const Eigen::Matrix2d R = Eigen::Rotation2Dd(yaw).toRotationMatrix();
  const Eigen::Vector2d rotated_offset = R * tracking_offset;
  offset_object.kinematics.pose_with_covariance.pose.position.x += rotated_offset.x();
  offset_object.kinematics.pose_with_covariance.pose.position.y += rotated_offset.y();
}

}  // namespace utils
}  // namespace autoware::multi_object_tracker

#endif  // AUTOWARE__MULTI_OBJECT_TRACKER__UTILS__UTILS_HPP_

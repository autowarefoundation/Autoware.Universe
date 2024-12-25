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
// Author: v1.0 Yukihiro Saito, Taekjin Lee

#include "autoware/multi_object_tracker/object_model/shapes.hpp"

#include <Eigen/Geometry>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/boost_polygon_utils.hpp>

#include <autoware_perception_msgs/msg/shape.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <boost/geometry.hpp>

#include <tf2/utils.h>
#include <tf2_ros/transform_listener.h>

#include <algorithm>
#include <string>
#include <vector>

namespace autoware::multi_object_tracker
{
namespace shapes
{
inline boost::optional<geometry_msgs::msg::Transform> getTransform(
  const tf2_ros::Buffer & tf_buffer, const std::string & source_frame_id,
  const std::string & target_frame_id, const rclcpp::Time & time)
{
  try {
    geometry_msgs::msg::TransformStamped self_transform_stamped;
    self_transform_stamped = tf_buffer.lookupTransform(
      target_frame_id, source_frame_id, time, rclcpp::Duration::from_seconds(0.5));
    return self_transform_stamped.transform;
  } catch (tf2::TransformException & ex) {
    RCLCPP_WARN_STREAM(rclcpp::get_logger("multi_object_tracker"), ex.what());
    return boost::none;
  }
}

bool transformObjects(
  const types::DynamicObjectList & input_msg, const std::string & target_frame_id,
  const tf2_ros::Buffer & tf_buffer, types::DynamicObjectList & output_msg)
{
  output_msg = input_msg;

  // transform to world coordinate
  if (input_msg.header.frame_id != target_frame_id) {
    output_msg.header.frame_id = target_frame_id;
    tf2::Transform tf_target2objects_world;
    tf2::Transform tf_target2objects;
    tf2::Transform tf_objects_world2objects;
    {
      const auto ros_target2objects_world =
        getTransform(tf_buffer, input_msg.header.frame_id, target_frame_id, input_msg.header.stamp);
      if (!ros_target2objects_world) {
        return false;
      }
      tf2::fromMsg(*ros_target2objects_world, tf_target2objects_world);
    }
    for (auto & object : output_msg.objects) {
      auto & pose_with_cov = object.kinematics.pose_with_covariance;
      tf2::fromMsg(pose_with_cov.pose, tf_objects_world2objects);
      tf_target2objects = tf_target2objects_world * tf_objects_world2objects;
      // transform pose, frame difference and object pose
      tf2::toMsg(tf_target2objects, pose_with_cov.pose);
      // transform covariance, only the frame difference
      pose_with_cov.covariance =
        tf2::transformCovariance(pose_with_cov.covariance, tf_target2objects_world);
    }
  }
  return true;
}

inline double getSumArea(const std::vector<autoware::universe_utils::Polygon2d> & polygons)
{
  return std::accumulate(
    polygons.begin(), polygons.end(), 0.0, [](double acc, autoware::universe_utils::Polygon2d p) {
      return acc + boost::geometry::area(p);
    });
}

inline double getIntersectionArea(
  const autoware::universe_utils::Polygon2d & source_polygon,
  const autoware::universe_utils::Polygon2d & target_polygon)
{
  std::vector<autoware::universe_utils::Polygon2d> intersection_polygons;
  boost::geometry::intersection(source_polygon, target_polygon, intersection_polygons);
  return getSumArea(intersection_polygons);
}

inline double getUnionArea(
  const autoware::universe_utils::Polygon2d & source_polygon,
  const autoware::universe_utils::Polygon2d & target_polygon)
{
  std::vector<autoware::universe_utils::Polygon2d> union_polygons;
  boost::geometry::union_(source_polygon, target_polygon, union_polygons);
  return getSumArea(union_polygons);
}

double get2dIoU(
  const types::DynamicObject & source_object, const types::DynamicObject & target_object,
  const double min_union_area)
{
  static const double MIN_AREA = 1e-6;

  const auto source_polygon = autoware::universe_utils::toPolygon2d(
    source_object.kinematics.pose_with_covariance.pose, source_object.shape);
  if (boost::geometry::area(source_polygon) < MIN_AREA) return 0.0;
  const auto target_polygon = autoware::universe_utils::toPolygon2d(
    target_object.kinematics.pose_with_covariance.pose, target_object.shape);
  if (boost::geometry::area(target_polygon) < MIN_AREA) return 0.0;

  const double intersection_area = getIntersectionArea(source_polygon, target_polygon);
  if (intersection_area < MIN_AREA) return 0.0;
  const double union_area = getUnionArea(source_polygon, target_polygon);

  const double iou =
    union_area < min_union_area ? 0.0 : std::min(1.0, intersection_area / union_area);
  return iou;
}

/**
 * @brief convert convex hull shape object to bounding box object
 * @param input_object: input convex hull objects
 * @param output_object: output bounding box objects
 */
bool convertConvexHullToBoundingBox(
  const types::DynamicObject & input_object, types::DynamicObject & output_object)
{
  // check footprint size
  if (input_object.shape.footprint.points.size() < 3) {
    return false;
  }

  // look for bounding box boundary
  float max_x = 0;
  float max_y = 0;
  float min_x = 0;
  float min_y = 0;
  float max_z = 0;
  for (const auto & point : input_object.shape.footprint.points) {
    max_x = std::max(max_x, point.x);
    max_y = std::max(max_y, point.y);
    min_x = std::min(min_x, point.x);
    min_y = std::min(min_y, point.y);
    max_z = std::max(max_z, point.z);
  }

  // calc new center
  const Eigen::Vector2d center{
    input_object.kinematics.pose_with_covariance.pose.position.x,
    input_object.kinematics.pose_with_covariance.pose.position.y};
  const auto yaw = tf2::getYaw(input_object.kinematics.pose_with_covariance.pose.orientation);
  const Eigen::Matrix2d R_inv = Eigen::Rotation2Dd(-yaw).toRotationMatrix();
  const Eigen::Vector2d new_local_center{(max_x + min_x) / 2.0, (max_y + min_y) / 2.0};
  const Eigen::Vector2d new_center = center + R_inv.transpose() * new_local_center;

  // set output parameters
  output_object = input_object;
  output_object.kinematics.pose_with_covariance.pose.position.x = new_center.x();
  output_object.kinematics.pose_with_covariance.pose.position.y = new_center.y();

  output_object.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;
  output_object.shape.dimensions.x = max_x - min_x;
  output_object.shape.dimensions.y = max_y - min_y;
  output_object.shape.dimensions.z = max_z;

  return true;
}

bool getMeasurementYaw(
  const types::DynamicObject & object, const double & predicted_yaw, double & measurement_yaw)
{
  measurement_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);

  // check orientation sign is known or not, and fix the limiting delta yaw
  double limiting_delta_yaw = M_PI_2;
  if (object.kinematics.orientation_availability == types::OrientationAvailability::AVAILABLE) {
    limiting_delta_yaw = M_PI;
  }
  // limiting delta yaw, even the availability is unknown
  while (std::fabs(predicted_yaw - measurement_yaw) > limiting_delta_yaw) {
    if (measurement_yaw < predicted_yaw) {
      measurement_yaw += 2 * limiting_delta_yaw;
    } else {
      measurement_yaw -= 2 * limiting_delta_yaw;
    }
  }
  // return false if the orientation is unknown
  return object.kinematics.orientation_availability != types::OrientationAvailability::UNAVAILABLE;
}

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
int getNearestCornerOrSurface(
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
void calcAnchorPointOffset(
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

}  // namespace shapes

}  // namespace autoware::multi_object_tracker

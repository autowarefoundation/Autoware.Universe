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

#include <autoware/autonomous_emergency_braking/utils.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>

#include <optional>
#include <string>
#include <vector>

namespace autoware::motion::control::autonomous_emergency_braking::utils
{
using autoware::universe_utils::Polygon2d;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjects;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;
using geometry_msgs::msg::TransformStamped;
using geometry_msgs::msg::Vector3;

std::optional<ObjectData> getObjectOnPathData(
  const std::vector<Pose> & ego_path, const geometry_msgs::msg::Point & obj_position,
  const rclcpp::Time & stamp, const double path_length, const double path_width,
  const double path_expansion, const double longitudinal_offset, const double object_speed)
{
  const auto current_p = [&]() {
    const auto & p = ego_path.front().position;
    return autoware::universe_utils::createPoint(p.x, p.y, p.z);
  }();
  const double obj_arc_length =
    autoware::motion_utils::calcSignedArcLength(ego_path, current_p, obj_position);
  if (
    std::isnan(obj_arc_length) || obj_arc_length < 0.0 ||
    obj_arc_length > path_length + longitudinal_offset) {
    return {};
  }

  // calculate the lateral offset between the ego vehicle and the object
  const double lateral_offset =
    std::abs(autoware::motion_utils::calcLateralOffset(ego_path, obj_position));

  // object is outside region of interest
  if (std::isnan(lateral_offset) || lateral_offset > path_width + path_expansion) {
    return {};
  }

  // If the object is behind the ego, we need to use the backward long offset. The distance should
  // be a positive number in any case
  const double dist_ego_to_object = obj_arc_length - longitudinal_offset;
  ObjectData obj;
  obj.stamp = stamp;
  obj.position = obj_position;
  obj.velocity = object_speed;
  obj.distance_to_object = std::abs(dist_ego_to_object);
  obj.is_target = (lateral_offset < path_width);
  return obj;
}

std::optional<double> getLongitudinalOffset(
  const std::vector<Pose> & ego_path, const double front_offset, const double rear_offset)
{
  const auto ego_is_driving_forward_opt = autoware::motion_utils::isDrivingForward(ego_path);
  if (!ego_is_driving_forward_opt.has_value()) {
    return {};
  }
  const bool ego_is_driving_forward = ego_is_driving_forward_opt.value();
  return (ego_is_driving_forward) ? front_offset : rear_offset;
}

PredictedObject transformObjectFrame(
  const PredictedObject & input, const geometry_msgs::msg::TransformStamped & transform_stamped)
{
  PredictedObject output = input;
  const auto & linear_twist = input.kinematics.initial_twist_with_covariance.twist.linear;
  const auto & angular_twist = input.kinematics.initial_twist_with_covariance.twist.angular;
  const auto & pose = input.kinematics.initial_pose_with_covariance.pose;

  geometry_msgs::msg::Pose t_pose;
  Vector3 t_linear_twist;
  Vector3 t_angular_twist;

  tf2::doTransform(pose, t_pose, transform_stamped);
  tf2::doTransform(linear_twist, t_linear_twist, transform_stamped);
  tf2::doTransform(angular_twist, t_angular_twist, transform_stamped);

  output.kinematics.initial_pose_with_covariance.pose = t_pose;
  output.kinematics.initial_twist_with_covariance.twist.linear = t_linear_twist;
  output.kinematics.initial_twist_with_covariance.twist.angular = t_angular_twist;
  return output;
}

Polygon2d convertPolygonObjectToGeometryPolygon(
  const Pose & current_pose, const autoware_perception_msgs::msg::Shape & obj_shape)
{
  if (obj_shape.footprint.points.empty()) {
    return {};
  }
  Polygon2d object_polygon;
  tf2::Transform tf_map2obj;
  fromMsg(current_pose, tf_map2obj);
  const auto obj_points = obj_shape.footprint.points;
  object_polygon.outer().reserve(obj_points.size() + 1);
  for (const auto & obj_point : obj_points) {
    tf2::Vector3 obj(obj_point.x, obj_point.y, obj_point.z);
    tf2::Vector3 tf_obj = tf_map2obj * obj;
    object_polygon.outer().emplace_back(tf_obj.x(), tf_obj.y());
  }
  object_polygon.outer().push_back(object_polygon.outer().front());
  boost::geometry::correct(object_polygon);

  return object_polygon;
}

Polygon2d convertCylindricalObjectToGeometryPolygon(
  const Pose & current_pose, const autoware_perception_msgs::msg::Shape & obj_shape)
{
  Polygon2d object_polygon;

  const double obj_x = current_pose.position.x;
  const double obj_y = current_pose.position.y;

  constexpr int n = 20;
  const double r = obj_shape.dimensions.x / 2;
  object_polygon.outer().reserve(n + 1);
  for (int i = 0; i < n; ++i) {
    object_polygon.outer().emplace_back(
      obj_x + r * std::cos(2.0 * M_PI / n * i), obj_y + r * std::sin(2.0 * M_PI / n * i));
  }

  object_polygon.outer().push_back(object_polygon.outer().front());
  boost::geometry::correct(object_polygon);

  return object_polygon;
}

Polygon2d convertBoundingBoxObjectToGeometryPolygon(
  const Pose & current_pose, const double & base_to_front, const double & base_to_rear,
  const double & base_to_width)
{
  const auto mapped_point = [](const double & length_scalar, const double & width_scalar) {
    tf2::Vector3 map;
    map.setX(length_scalar);
    map.setY(width_scalar);
    map.setZ(0.0);
    map.setW(1.0);
    return map;
  };

  // set vertices at map coordinate
  const tf2::Vector3 p1_map = std::invoke(mapped_point, base_to_front, -base_to_width);
  const tf2::Vector3 p2_map = std::invoke(mapped_point, base_to_front, base_to_width);
  const tf2::Vector3 p3_map = std::invoke(mapped_point, -base_to_rear, base_to_width);
  const tf2::Vector3 p4_map = std::invoke(mapped_point, -base_to_rear, -base_to_width);

  // transform vertices from map coordinate to object coordinate
  tf2::Transform tf_map2obj;
  tf2::fromMsg(current_pose, tf_map2obj);
  const tf2::Vector3 p1_obj = tf_map2obj * p1_map;
  const tf2::Vector3 p2_obj = tf_map2obj * p2_map;
  const tf2::Vector3 p3_obj = tf_map2obj * p3_map;
  const tf2::Vector3 p4_obj = tf_map2obj * p4_map;

  Polygon2d object_polygon;
  object_polygon.outer().reserve(5);
  object_polygon.outer().emplace_back(p1_obj.x(), p1_obj.y());
  object_polygon.outer().emplace_back(p2_obj.x(), p2_obj.y());
  object_polygon.outer().emplace_back(p3_obj.x(), p3_obj.y());
  object_polygon.outer().emplace_back(p4_obj.x(), p4_obj.y());

  object_polygon.outer().push_back(object_polygon.outer().front());
  boost::geometry::correct(object_polygon);

  return object_polygon;
}

Polygon2d convertObjToPolygon(const PredictedObject & obj)
{
  Polygon2d object_polygon{};
  if (obj.shape.type == autoware_perception_msgs::msg::Shape::CYLINDER) {
    object_polygon = utils::convertCylindricalObjectToGeometryPolygon(
      obj.kinematics.initial_pose_with_covariance.pose, obj.shape);
  } else if (obj.shape.type == autoware_perception_msgs::msg::Shape::BOUNDING_BOX) {
    const double & length_m = obj.shape.dimensions.x / 2;
    const double & width_m = obj.shape.dimensions.y / 2;
    object_polygon = utils::convertBoundingBoxObjectToGeometryPolygon(
      obj.kinematics.initial_pose_with_covariance.pose, length_m, length_m, width_m);
  } else if (obj.shape.type == autoware_perception_msgs::msg::Shape::POLYGON) {
    object_polygon = utils::convertPolygonObjectToGeometryPolygon(
      obj.kinematics.initial_pose_with_covariance.pose, obj.shape);
  } else {
    throw std::runtime_error("Unsupported shape type");
  }
  return object_polygon;
}

std::optional<geometry_msgs::msg::TransformStamped> getTransform(
  const std::string & target_frame, const std::string & source_frame,
  const tf2_ros::Buffer & tf_buffer, const rclcpp::Logger & logger)
{
  geometry_msgs::msg::TransformStamped tf_current_pose;
  try {
    tf_current_pose = tf_buffer.lookupTransform(
      target_frame, source_frame, rclcpp::Time(0), rclcpp::Duration::from_seconds(1.0));
  } catch (tf2::TransformException & ex) {
    RCLCPP_ERROR_STREAM(
      logger, "[AEB] Failed to look up transform from " + source_frame + " to " + target_frame);
    return std::nullopt;
  }
  return std::make_optional(tf_current_pose);
}

void fillMarkerFromPolygon(
  const std::vector<Polygon2d> & polygons, visualization_msgs::msg::Marker & polygon_marker)
{
  for (const auto & poly : polygons) {
    for (size_t dp_idx = 0; dp_idx < poly.outer().size(); ++dp_idx) {
      const auto & boost_cp = poly.outer().at(dp_idx);
      const auto & boost_np = poly.outer().at((dp_idx + 1) % poly.outer().size());
      const auto curr_point =
        autoware::universe_utils::createPoint(boost_cp.x(), boost_cp.y(), 0.0);
      const auto next_point =
        autoware::universe_utils::createPoint(boost_np.x(), boost_np.y(), 0.0);
      polygon_marker.points.push_back(curr_point);
      polygon_marker.points.push_back(next_point);
    }
  }
}

void fillMarkerFromPolygon(
  const std::vector<Polygon3d> & polygons, visualization_msgs::msg::Marker & polygon_marker)
{
  for (const auto & poly : polygons) {
    for (size_t dp_idx = 0; dp_idx < poly.outer().size(); ++dp_idx) {
      const auto & boost_cp = poly.outer().at(dp_idx);
      const auto & boost_np = poly.outer().at((dp_idx + 1) % poly.outer().size());
      const auto curr_point =
        autoware::universe_utils::createPoint(boost_cp.x(), boost_cp.y(), boost_cp.z());
      const auto next_point =
        autoware::universe_utils::createPoint(boost_np.x(), boost_np.y(), boost_np.z());
      polygon_marker.points.push_back(curr_point);
      polygon_marker.points.push_back(next_point);
    }
  }
}

}  // namespace autoware::motion::control::autonomous_emergency_braking::utils

// Copyright 2015-2021 Autoware Foundation
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

#include "autoware_vehicle_info_utils/vehicle_info.hpp"

#include <rclcpp/rclcpp.hpp>

#include <limits>

namespace autoware::vehicle_info_utils
{
autoware::universe_utils::LinearRing2d VehicleInfo::createFootprint(const double margin) const
{
  return createFootprint(margin, margin);
}

autoware::universe_utils::LinearRing2d VehicleInfo::createFootprint(
  const double lat_margin, const double lon_margin) const
{
  using autoware::universe_utils::LinearRing2d;
  using autoware::universe_utils::Point2d;

  const double x_front = front_overhang_m + wheel_base_m + lon_margin;
  const double x_center = wheel_base_m / 2.0;
  const double x_rear = -(rear_overhang_m + lon_margin);
  const double y_left = wheel_tread_m / 2.0 + left_overhang_m + lat_margin;
  const double y_right = -(wheel_tread_m / 2.0 + right_overhang_m + lat_margin);

  LinearRing2d footprint;
  footprint.push_back(Point2d{x_front, y_left});
  footprint.push_back(Point2d{x_front, y_right});
  footprint.push_back(Point2d{x_center, y_right});
  footprint.push_back(Point2d{x_rear, y_right});
  footprint.push_back(Point2d{x_rear, y_left});
  footprint.push_back(Point2d{x_center, y_left});
  footprint.push_back(Point2d{x_front, y_left});

  return footprint;
}

VehicleInfo createVehicleInfo(
  const double wheel_radius_m, const double wheel_width_m, const double wheel_base_m_arg,
  const double wheel_tread_m, const double front_overhang_m, const double rear_overhang_m,
  const double left_overhang_m, const double right_overhang_m, const double vehicle_height_m,
  const double max_steer_angle_rad_arg)
{
  double wheel_base_m = wheel_base_m_arg;
  static constexpr double MIN_WHEEL_BASE_M = 1e-6;
  if (std::abs(wheel_base_m) < MIN_WHEEL_BASE_M) {
    RCLCPP_ERROR(
      rclcpp::get_logger("vehicle_info"), "wheel_base_m %f is almost 0.0, clamping to %f",
      wheel_base_m, MIN_WHEEL_BASE_M);
    wheel_base_m = MIN_WHEEL_BASE_M;
  }
  double max_steer_angle_rad = max_steer_angle_rad_arg;
  static constexpr double MAX_STEER_ANGLE_RAD = 1e-6;
  if (std::abs(max_steer_angle_rad) < MAX_STEER_ANGLE_RAD) {
    RCLCPP_ERROR(
      rclcpp::get_logger("vehicle_info"), "max_steer_angle_rad %f is almost 0.0, clamping to %f",
      max_steer_angle_rad, MAX_STEER_ANGLE_RAD);
    max_steer_angle_rad = MAX_STEER_ANGLE_RAD;
  }

  if (
    wheel_radius_m <= 0 || wheel_width_m <= 0 || wheel_base_m <= 0 || wheel_tread_m <= 0 ||
    front_overhang_m <= 0 || rear_overhang_m <= 0 || left_overhang_m <= 0 ||
    right_overhang_m <= 0 || vehicle_height_m <= 0 || max_steer_angle_rad <= 0) {
    RCLCPP_ERROR(
      rclcpp::get_logger("vehicle_info"), "given parameters contain non positive values");
  }

  // Calculate derived parameters
  const double vehicle_length_m_ = front_overhang_m + wheel_base_m + rear_overhang_m;
  const double vehicle_width_m_ = wheel_tread_m + left_overhang_m + right_overhang_m;
  const double min_longitudinal_offset_m_ = -rear_overhang_m;
  const double max_longitudinal_offset_m_ = front_overhang_m + wheel_base_m;
  const double min_lateral_offset_m_ = -(wheel_tread_m / 2.0 + right_overhang_m);
  const double max_lateral_offset_m_ = wheel_tread_m / 2.0 + left_overhang_m;
  const double min_height_offset_m_ = 0.0;
  const double max_height_offset_m_ = vehicle_height_m;

  return VehicleInfo{
    // Base parameters
    wheel_radius_m,
    wheel_width_m,
    wheel_base_m,
    wheel_tread_m,
    front_overhang_m,
    rear_overhang_m,
    left_overhang_m,
    right_overhang_m,
    vehicle_height_m,
    max_steer_angle_rad,
    // Derived parameters
    vehicle_length_m_,
    vehicle_width_m_,
    min_longitudinal_offset_m_,
    max_longitudinal_offset_m_,
    min_lateral_offset_m_,
    max_lateral_offset_m_,
    min_height_offset_m_,
    max_height_offset_m_,
  };
}

double VehicleInfo::calcMaxCurvature() const
{
  const double radius = wheel_base_m / std::tan(max_steer_angle_rad);
  const double curvature = 1.0 / radius;
  return curvature;
}
double VehicleInfo::calcCurvatureFromSteerAngle(const double steer_angle) const
{
  if (std::abs(steer_angle) < 1e-6) {
    return std::numeric_limits<double>::max();
  }
  const double radius = wheel_base_m / std::tan(steer_angle);
  const double curvature = 1.0 / radius;
  return curvature;
}

double VehicleInfo::calcSteerAngleFromCurvature(const double curvature) const
{
  if (std::abs(curvature) < 1e-6) {
    return 0.0;
  }

  const double radius = 1.0 / curvature;
  const double steer_angle = std::atan2(wheel_base_m, radius);
  return steer_angle;
}
}  // namespace autoware::vehicle_info_utils

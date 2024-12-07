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

#include "autoware/motion_utils/trajectory/path_shift.hpp"

#include "autoware/motion_utils/trajectory/trajectory.hpp"

#include <algorithm>
#include <string>

namespace autoware::motion_utils
{
double calc_feasible_velocity_from_jerk(
  const double lateral, const double jerk, const double longitudinal_distance)
{
  const double j = std::abs(jerk);
  const double l = std::abs(lateral);
  const double d = std::abs(longitudinal_distance);
  if (j < 1.0e-8 || l < 1.0e-8) {
    const std::string error_message(
      std::string(__func__) + ": Failed to calculate velocity due to invalid arg");
    RCLCPP_WARN(get_logger(), "%s", error_message.c_str());
    return 1.0e10;
  }
  return d / (4.0 * std::pow(0.5 * l / j, 1.0 / 3.0));
}

double calc_lateral_dist_from_jerk(
  const double longitudinal, const double jerk, const double velocity)
{
  const double j = std::abs(jerk);
  const double d = std::abs(longitudinal);
  const double v = std::abs(velocity);

  if (j < 1.0e-8 || v < 1.0e-8) {
    const std::string error_message(
      std::string(__func__) + ": Failed to calculate lateral distance due to invalid arg");
    RCLCPP_WARN(get_logger(), "%s", error_message.c_str());
    return 1.0e10;
  }
  return 2.0 * std::pow(d / (4.0 * v), 3.0) * j;
}

double calc_longitudinal_dist_from_jerk(
  const double lateral, const double jerk, const double velocity)
{
  const double j = std::abs(jerk);
  const double l = std::abs(lateral);
  const double v = std::abs(velocity);
  if (j < 1.0e-8) {
    const std::string error_message(
      std::string(__func__) + ": Failed to calculate longitudinal distance due to invalid arg");
    RCLCPP_WARN(get_logger(), "%s", error_message.c_str());
    return 1.0e10;
  }
  return 4.0 * std::pow(0.5 * l / j, 1.0 / 3.0) * v;
}

double calc_shift_time_from_jerk(const double lateral, const double jerk, const double acc)
{
  const double j = std::abs(jerk);
  const double a = std::abs(acc);
  const double l = std::abs(lateral);
  if (j < 1.0e-8 || a < 1.0e-8) {
    const std::string error_message(
      std::string(__func__) + ": Failed to calculate shift time due to invalid arg");
    RCLCPP_WARN(get_logger(), "%s", error_message.c_str());
    return 1.0e10;  // TODO(Horibe) maybe invalid arg?
  }

  // time with constant jerk
  double tj = a / j;

  // time with constant acceleration (zero jerk)
  double ta = (std::sqrt(a * a + 4.0 * j * j * l / a) - 3.0 * a) / (2.0 * j);

  if (ta < 0.0) {
    // it will not hit the acceleration limit this time
    tj = std::pow(l / (2.0 * j), 1.0 / 3.0);
    ta = 0.0;
  }

  const double t_total = 4.0 * tj + 2.0 * ta;
  return t_total;
}

double calc_jerk_from_lat_lon_distance(
  const double lateral, const double longitudinal, const double velocity)
{
  constexpr double ep = 1.0e-3;
  const double lat = std::abs(lateral);
  const double lon = std::max(std::abs(longitudinal), ep);
  const double v = std::abs(velocity);
  return 0.5 * lat * std::pow(4.0 * v / lon, 3);
}

}  // namespace autoware::motion_utils

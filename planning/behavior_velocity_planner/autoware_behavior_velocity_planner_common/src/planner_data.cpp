// Copyright 2019 Autoware Foundation
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

#include "autoware/behavior_velocity_planner_common/planner_data.hpp"

#include "autoware_vehicle_info_utils/vehicle_info_utils.hpp"

#include <vector>

namespace autoware::behavior_velocity_planner
{
PlannerData::PlannerData(rclcpp::Node & node)
: clock_(node.get_clock()),
  vehicle_info_(autoware::vehicle_info_utils::VehicleInfoUtils(node).getVehicleInfo())
{
  max_stop_acceleration_threshold = node.declare_parameter<double>("max_accel");
  max_stop_jerk_threshold = node.declare_parameter<double>("max_jerk");
  system_delay = node.declare_parameter<double>("system_delay");
  delay_response_time = node.declare_parameter<double>("delay_response_time");
}

bool PlannerData::isVehicleStopped(const double stop_duration) const
{
  if (velocity_buffer.empty()) {
    return false;
  }

  const auto now = clock_->now();
  std::vector<double> vs;
  for (const auto & velocity : velocity_buffer) {
    vs.push_back(velocity.twist.linear.x);

    const auto & s = velocity.header.stamp;
    const auto time_diff =
      now >= s ? now - s : rclcpp::Duration(0, 0);  // Note: negative time throws an exception.
    if (time_diff.seconds() >= stop_duration) {
      break;
    }
  }

  constexpr double stop_velocity = 1e-3;
  for (const auto & v : vs) {
    if (v >= stop_velocity) {
      return false;
    }
  }

  return true;
}

std::optional<TrafficSignalStamped> PlannerData::getTrafficSignal(
  const lanelet::Id id, const bool keep_last_observation) const
{
  const auto & traffic_light_id_map =
    keep_last_observation ? traffic_light_id_map_last_observed_ : traffic_light_id_map_raw_;
  if (traffic_light_id_map.count(id) == 0) {
    return std::nullopt;
  }
  return std::make_optional<TrafficSignalStamped>(traffic_light_id_map.at(id));
}
}  // namespace autoware::behavior_velocity_planner

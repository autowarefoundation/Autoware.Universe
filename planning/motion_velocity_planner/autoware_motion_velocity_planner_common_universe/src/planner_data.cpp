// Copyright 2024 Autoware Foundation
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

#include <autoware/motion_utils/trajectory/interpolation.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/motion_velocity_planner_common_universe/planner_data.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/geometry/LineString.h>
#include <lanelet2_core/primitives/BoundingBox.h>

#include <vector>

namespace autoware::motion_velocity_planner
{
std::optional<TrafficSignalStamped> PlannerData::get_traffic_signal(
  const lanelet::Id id, const bool keep_last_observation) const
{
  const auto & traffic_light_id_map =
    keep_last_observation ? traffic_light_id_map_last_observed_ : traffic_light_id_map_raw_;
  if (traffic_light_id_map.count(id) == 0) {
    return std::nullopt;
  }
  return std::make_optional<TrafficSignalStamped>(traffic_light_id_map.at(id));
}

std::optional<double> PlannerData::calculate_min_deceleration_distance(
  const double target_velocity) const
{
  return motion_utils::calcDecelDistWithJerkAndAccConstraints(
    current_odometry.twist.twist.linear.x, target_velocity,
    current_acceleration.accel.accel.linear.x, velocity_smoother_->getMinDecel(),
    std::abs(velocity_smoother_->getMinJerk()), velocity_smoother_->getMinJerk());
}

std::vector<StopPoint> PlannerData::calculate_map_stop_points(
  const std::vector<autoware_planning_msgs::msg::TrajectoryPoint> & trajectory) const
{
  std::vector<StopPoint> stop_points;
  if (!route_handler) {
    return stop_points;
  }
  universe_utils::LineString2d trajectory_ls;
  for (const auto & p : trajectory) {
    trajectory_ls.emplace_back(p.pose.position.x, p.pose.position.y);
  }
  const auto candidates = route_handler->getLaneletMapPtr()->laneletLayer.search(
    boost::geometry::return_envelope<lanelet::BoundingBox2d>(trajectory_ls));
  for (const auto & candidate : candidates) {
    const auto stop_lines = lanelet::utils::query::stopLinesLanelet(candidate);
    for (const auto & stop_line : stop_lines) {
      const auto stop_line_2d = lanelet::utils::to2D(stop_line).basicLineString();
      universe_utils::MultiPoint2d intersections;
      boost::geometry::intersection(trajectory_ls, stop_line_2d, intersections);
      for (const auto & intersection : intersections) {
        const auto p =
          geometry_msgs::msg::Point().set__x(intersection.x()).set__y(intersection.y());
        const auto stop_line_arc_length = motion_utils::calcSignedArcLength(trajectory, 0UL, p);
        StopPoint sp;
        sp.ego_trajectory_arc_length =
          stop_line_arc_length - vehicle_info_.max_longitudinal_offset_m;
        if (sp.ego_trajectory_arc_length < 0.0) {
          continue;
        }
        sp.stop_line = stop_line_2d;
        sp.ego_stop_pose =
          motion_utils::calcInterpolatedPose(trajectory, sp.ego_trajectory_arc_length);
        stop_points.push_back(sp);
      }
    }
  }
  return stop_points;
}
}  // namespace autoware::motion_velocity_planner

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

#include "autoware/lane_departure_checker/utils.hpp"

#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>

#include <boost/geometry.hpp>

#include <lanelet2_core/geometry/Polygon.h>

#include <vector>

namespace
{
struct FootprintMargin
{
  double lon;
  double lat;
};

FootprintMargin calcFootprintMargin(
  const geometry_msgs::msg::PoseWithCovariance & covariance, const double scale)
{
  const auto Cov_in_map = covariance.covariance;
  Eigen::Matrix2d Cov_xy_map;
  Cov_xy_map << Cov_in_map[0 * 6 + 0], Cov_in_map[0 * 6 + 1], Cov_in_map[1 * 6 + 0],
    Cov_in_map[1 * 6 + 1];

  const double yaw_vehicle = tf2::getYaw(covariance.pose.orientation);

  // To get a position in a transformed coordinate, rotate the inverse direction
  Eigen::Matrix2d R_map2vehicle;
  R_map2vehicle << std::cos(-yaw_vehicle), -std::sin(-yaw_vehicle), std::sin(-yaw_vehicle),
    std::cos(-yaw_vehicle);
  // Rotate covariance E((X, Y)^t*(X, Y)) = E(R*(x,y)*(x,y)^t*R^t)
  // when Rotate point (X, Y)^t= R*(x, y)^t.
  const Eigen::Matrix2d Cov_xy_vehicle = R_map2vehicle * Cov_xy_map * R_map2vehicle.transpose();

  // The longitudinal/lateral length is represented
  // in Cov_xy_vehicle(0,0), Cov_xy_vehicle(1,1) respectively.
  return FootprintMargin{Cov_xy_vehicle(0, 0) * scale, Cov_xy_vehicle(1, 1) * scale};
}
}  // namespace

namespace autoware::lane_departure_checker::utils
{
TrajectoryPoints cutTrajectory(const TrajectoryPoints & trajectory, const double length)
{
  if (trajectory.empty()) {
    return {};
  }

  TrajectoryPoints cut;

  double total_length = 0.0;
  auto last_point = autoware::universe_utils::fromMsg(trajectory.front().pose.position);
  auto end_it = std::next(trajectory.cbegin());
  for (; end_it != trajectory.cend(); ++end_it) {
    const auto remain_distance = length - total_length;

    // Over length
    if (remain_distance <= 0) {
      break;
    }

    const auto & new_pose = end_it->pose;
    const auto new_point = autoware::universe_utils::fromMsg(new_pose.position);
    const auto points_distance = boost::geometry::distance(last_point.to_2d(), new_point.to_2d());

    // Require interpolation
    if (remain_distance <= points_distance) {
      const Eigen::Vector3d p_interpolated =
        last_point + remain_distance * (new_point - last_point).normalized();

      TrajectoryPoint p;
      p.pose.position.x = p_interpolated.x();
      p.pose.position.y = p_interpolated.y();
      p.pose.position.z = p_interpolated.z();
      p.pose.orientation = new_pose.orientation;

      cut.push_back(p);
      break;
    }

    total_length += points_distance;
    last_point = new_point;
  }
  cut.insert(cut.begin(), trajectory.begin(), end_it);

  return cut;
}

TrajectoryPoints resampleTrajectory(const Trajectory & trajectory, const double interval)
{
  if (trajectory.points.size() < 2) {
    return trajectory.points;
  }

  TrajectoryPoints resampled;

  resampled.push_back(trajectory.points.front());
  auto prev_point = autoware::universe_utils::fromMsg(trajectory.points.front().pose.position);
  for (size_t i = 1; i < trajectory.points.size() - 1; ++i) {
    const auto & traj_point = trajectory.points.at(i);

    const auto next_point = autoware::universe_utils::fromMsg(traj_point.pose.position);

    if (boost::geometry::distance(prev_point.to_2d(), next_point.to_2d()) >= interval) {
      resampled.push_back(traj_point);
    }

    prev_point = next_point;
  }
  resampled.push_back(trajectory.points.back());

  return resampled;
}

std::vector<LinearRing2d> createVehicleFootprints(
  const geometry_msgs::msg::PoseWithCovariance & covariance, const TrajectoryPoints & trajectory,
  const autoware::vehicle_info_utils::VehicleInfo & vehicle_info,
  const double footprint_margin_scale)
{
  // Calculate longitudinal and lateral margin based on covariance
  const auto margin = calcFootprintMargin(covariance, footprint_margin_scale);

  // Create vehicle footprint in base_link coordinate
  const auto local_vehicle_footprint = vehicle_info.createFootprint(margin.lat, margin.lon);

  // Create vehicle footprint on each TrajectoryPoint
  std::vector<LinearRing2d> vehicle_footprints;
  for (const auto & p : trajectory) {
    vehicle_footprints.push_back(
      transformVector(local_vehicle_footprint, autoware::universe_utils::pose2transform(p.pose)));
  }

  return vehicle_footprints;
}

std::vector<LinearRing2d> createVehicleFootprints(
  const PathWithLaneId & path, const autoware::vehicle_info_utils::VehicleInfo & vehicle_info,
  const double footprint_extra_margin)
{
  // Create vehicle footprint in base_link coordinate
  const auto local_vehicle_footprint = vehicle_info.createFootprint(footprint_extra_margin);

  // Create vehicle footprint on each Path point
  std::vector<LinearRing2d> vehicle_footprints;
  for (const auto & p : path.points) {
    vehicle_footprints.push_back(transformVector(
      local_vehicle_footprint, autoware::universe_utils::pose2transform(p.point.pose)));
  }

  return vehicle_footprints;
}

lanelet::ConstLanelets getCandidateLanelets(
  const lanelet::ConstLanelets & route_lanelets,
  const std::vector<LinearRing2d> & vehicle_footprints)
{
  lanelet::ConstLanelets candidate_lanelets;

  // Find lanes within the convex hull of footprints
  const auto footprint_hull = createHullFromFootprints(vehicle_footprints);

  for (const auto & route_lanelet : route_lanelets) {
    const auto poly = route_lanelet.polygon2d().basicPolygon();
    if (!boost::geometry::disjoint(poly, footprint_hull)) {
      candidate_lanelets.push_back(route_lanelet);
    }
  }

  return candidate_lanelets;
}

LinearRing2d createHullFromFootprints(const std::vector<LinearRing2d> & footprints)
{
  MultiPoint2d combined;
  for (const auto & footprint : footprints) {
    for (const auto & p : footprint) {
      combined.push_back(p);
    }
  }

  LinearRing2d hull;
  boost::geometry::convex_hull(combined, hull);

  return hull;
}

std::vector<LinearRing2d> createVehiclePassingAreas(
  const std::vector<LinearRing2d> & vehicle_footprints)
{
  if (vehicle_footprints.empty()) {
    return {};
  }

  if (vehicle_footprints.size() == 1) {
    return {vehicle_footprints.front()};
  }

  std::vector<LinearRing2d> areas;
  areas.reserve(vehicle_footprints.size() - 1);

  for (size_t i = 0; i < vehicle_footprints.size() - 1; ++i) {
    const auto & footprint1 = vehicle_footprints.at(i);
    const auto & footprint2 = vehicle_footprints.at(i + 1);
    areas.push_back(createHullFromFootprints({footprint1, footprint2}));
  }

  return areas;
}

PoseDeviation calcTrajectoryDeviation(
  const Trajectory & trajectory, const geometry_msgs::msg::Pose & pose, const double dist_threshold,
  const double yaw_threshold)
{
  const auto nearest_idx = autoware::motion_utils::findFirstNearestIndexWithSoftConstraints(
    trajectory.points, pose, dist_threshold, yaw_threshold);
  return autoware::universe_utils::calcPoseDeviation(trajectory.points.at(nearest_idx).pose, pose);
}

double calcMaxSearchLengthForBoundaries(
  const Trajectory & trajectory, const autoware::vehicle_info_utils::VehicleInfo & vehicle_info)
{
  const double max_ego_lon_length = std::max(
    std::abs(vehicle_info.max_longitudinal_offset_m),
    std::abs(vehicle_info.min_longitudinal_offset_m));
  const double max_ego_lat_length = std::max(
    std::abs(vehicle_info.max_lateral_offset_m), std::abs(vehicle_info.min_lateral_offset_m));
  const double max_ego_search_length = std::hypot(max_ego_lon_length, max_ego_lat_length);
  return autoware::motion_utils::calcArcLength(trajectory.points) + max_ego_search_length;
}
}  // namespace autoware::lane_departure_checker::utils

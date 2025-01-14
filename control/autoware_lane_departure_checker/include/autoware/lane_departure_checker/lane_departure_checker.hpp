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

#ifndef AUTOWARE__LANE_DEPARTURE_CHECKER__LANE_DEPARTURE_CHECKER_HPP_
#define AUTOWARE__LANE_DEPARTURE_CHECKER__LANE_DEPARTURE_CHECKER_HPP_

#include "autoware/lane_departure_checker/parameters.hpp"

#include <autoware/universe_utils/system/time_keeper.hpp>
#include <autoware_vehicle_info_utils/vehicle_info_utils.hpp>
#include <rosidl_runtime_cpp/message_initialization.hpp>

#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/union.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/geometry/LaneletMap.h>
#include <lanelet2_core/geometry/LineString.h>
#include <lanelet2_core/geometry/Polygon.h>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::lane_departure_checker
{
using autoware::universe_utils::Segment2d;
using tier4_planning_msgs::msg::PathWithLaneId;
typedef boost::geometry::index::rtree<Segment2d, boost::geometry::index::rstar<16>> SegmentRtree;

class LaneDepartureChecker
{
public:
  explicit LaneDepartureChecker(
    std::shared_ptr<universe_utils::TimeKeeper> time_keeper =
      std::make_shared<universe_utils::TimeKeeper>())
  : time_keeper_(time_keeper)
  {
  }

  LaneDepartureChecker(
    const Param & param, const autoware::vehicle_info_utils::VehicleInfo & vehicle_info,
    std::shared_ptr<universe_utils::TimeKeeper> time_keeper =
      std::make_shared<universe_utils::TimeKeeper>())
  : param_(param),
    vehicle_info_ptr_(std::make_shared<autoware::vehicle_info_utils::VehicleInfo>(vehicle_info)),
    time_keeper_(time_keeper)
  {
  }
  Output update(const Input & input);

  void setParam(const Param & param) { param_ = param; }

  bool checkPathWillLeaveLane(
    const lanelet::ConstLanelets & lanelets, const PathWithLaneId & path) const;

  std::vector<std::pair<double, lanelet::Lanelet>> getLaneletsFromPath(
    const lanelet::LaneletMapPtr lanelet_map_ptr, const PathWithLaneId & path) const;

  std::optional<autoware::universe_utils::Polygon2d> getFusedLaneletPolygonForPath(
    const lanelet::LaneletMapPtr lanelet_map_ptr, const PathWithLaneId & path) const;

  bool updateFusedLaneletPolygonForPath(
    const lanelet::LaneletMapPtr lanelet_map_ptr, const PathWithLaneId & path,
    std::vector<lanelet::Id> & fused_lanelets_id,
    std::optional<autoware::universe_utils::Polygon2d> & fused_lanelets_polygon) const;

  bool checkPathWillLeaveLane(
    const lanelet::LaneletMapPtr lanelet_map_ptr, const PathWithLaneId & path) const;

  bool checkPathWillLeaveLane(
    const lanelet::LaneletMapPtr lanelet_map_ptr, const PathWithLaneId & path,
    std::vector<lanelet::Id> & fused_lanelets_id,
    std::optional<autoware::universe_utils::Polygon2d> & fused_lanelets_polygon) const;

  PathWithLaneId cropPointsOutsideOfLanes(
    const lanelet::LaneletMapPtr lanelet_map_ptr, const PathWithLaneId & path,
    const size_t end_index, std::vector<lanelet::Id> & fused_lanelets_id,
    std::optional<autoware::universe_utils::Polygon2d> & fused_lanelets_polygon);

  static bool isOutOfLane(
    const lanelet::ConstLanelets & candidate_lanelets, const LinearRing2d & vehicle_footprint);

private:
  Param param_;
  std::shared_ptr<autoware::vehicle_info_utils::VehicleInfo> vehicle_info_ptr_;

  bool willLeaveLane(
    const lanelet::ConstLanelets & candidate_lanelets,
    const std::vector<LinearRing2d> & vehicle_footprints) const;

  static SegmentRtree extractUncrossableBoundaries(
    const lanelet::LaneletMap & lanelet_map, const geometry_msgs::msg::Point & ego_point,
    const double max_search_length, const std::vector<std::string> & boundary_types_to_detect);

  bool willCrossBoundary(
    const std::vector<LinearRing2d> & vehicle_footprints,
    const SegmentRtree & uncrossable_segments) const;

  lanelet::BasicPolygon2d toBasicPolygon2D(const LinearRing2d & footprint_hull) const;
  autoware::universe_utils::Polygon2d toPolygon2D(const lanelet::BasicPolygon2d & poly) const;

  mutable std::shared_ptr<universe_utils::TimeKeeper> time_keeper_;
};
}  // namespace autoware::lane_departure_checker

#endif  // AUTOWARE__LANE_DEPARTURE_CHECKER__LANE_DEPARTURE_CHECKER_HPP_

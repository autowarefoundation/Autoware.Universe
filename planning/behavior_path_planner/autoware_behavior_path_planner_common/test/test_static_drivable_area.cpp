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

#include "autoware/behavior_path_planner_common/data_manager.hpp"
#include "autoware/behavior_path_planner_common/utils/drivable_area_expansion/static_drivable_area.hpp"

#include <autoware/route_handler/route_handler.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <nav_msgs/msg/detail/odometry__struct.hpp>

#include <boost/geometry/algorithms/detail/disjoint/interface.hpp>
#include <boost/geometry/algorithms/detail/intersects/interface.hpp>

#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>

#include <memory>
#include <vector>

const auto intersection_map =
  autoware::test_utils::make_map_bin_msg(autoware::test_utils::get_absolute_path_to_lanelet_map(
    "autoware_test_utils", "intersection/lanelet2_map.osm"));

using autoware::behavior_path_planner::DrivableLanes;
using autoware::test_utils::make_lanelet;

DrivableLanes make_drivable_lanes(const lanelet::ConstLanelet & ll)
{
  using autoware::behavior_path_planner::utils::generateDrivableLanes;
  return generateDrivableLanes({ll}).front();
}

bool equal(const geometry_msgs::msg::Point & p1, const geometry_msgs::msg::Point & p2)
{
  return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

bool equal(const DrivableLanes & l1, const DrivableLanes & l2)
{
  if (l1.middle_lanes.size() != l2.middle_lanes.size()) {
    return false;
  }
  auto are_equal = true;
  are_equal &= boost::geometry::equals(
    l1.left_lane.polygon2d().basicPolygon(), l2.left_lane.polygon2d().basicPolygon());
  are_equal &= boost::geometry::equals(
    l1.right_lane.polygon2d().basicPolygon(), l2.right_lane.polygon2d().basicPolygon());
  for (auto i = 0UL; i < l1.middle_lanes.size(); ++i) {
    are_equal &= boost::geometry::equals(
      l1.middle_lanes[i].polygon2d().basicPolygon(), l2.middle_lanes[i].polygon2d().basicPolygon());
  }
  return are_equal;
}

TEST(StaticDrivableArea, getOverlappedLaneletId)
{
  using autoware::behavior_path_planner::utils::getOverlappedLaneletId;

  std::vector<DrivableLanes> lanes;
  {  // empty lanes
    const auto result = getOverlappedLaneletId(lanes);
    EXPECT_FALSE(result.has_value());
  }
  {  // lanes at 0
    const DrivableLanes l =
      make_drivable_lanes(make_lanelet({0.0, 1.0}, {5.0, 1.0}, {5.0, -1.0}, {5.0, -1.0}));
    lanes.push_back(l);
    const auto result = getOverlappedLaneletId(lanes);
    EXPECT_FALSE(result.has_value());
  }
  {  // lanes at 1, overlap with 0 but ignored since it is the following lane
    const DrivableLanes l =
      make_drivable_lanes(make_lanelet({4.0, 1.0}, {8.0, 1.0}, {4.0, -1.0}, {8.0, -1.0}));
    lanes.push_back(l);
    const auto result = getOverlappedLaneletId(lanes);
    EXPECT_FALSE(result.has_value());
  }
  {  // lanes at 2, overlap with 1 but ignored since it is the following lane
    const DrivableLanes l =
      make_drivable_lanes(make_lanelet({6.0, 1.0}, {10.0, 1.0}, {6.0, -1.0}, {10.0, -1.0}));
    lanes.push_back(l);
    const auto result = getOverlappedLaneletId(lanes);
    EXPECT_FALSE(result.has_value());
  }
  {  // lanes at 3, overlap with 1 so 3 is returned
    const DrivableLanes l =
      make_drivable_lanes(make_lanelet({5.0, 0.0}, {5.0, 5.0}, {6.0, 0.0}, {6.0, 5.0}));
    lanes.push_back(l);
    const auto result = getOverlappedLaneletId(lanes);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 3UL);
  }
  {  // lanes at 4, overlap with 2 but since 3 overlaps first it is still returned
    const DrivableLanes l =
      make_drivable_lanes(make_lanelet({7.0, 0.0}, {7.0, 5.0}, {8.0, 0.0}, {8.0, 5.0}));
    lanes.push_back(l);
    const auto result = getOverlappedLaneletId(lanes);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 3UL);
  }
  {  // change 1 to no longer overlap with 3 and now 4 is the first overlap
    const DrivableLanes l = make_drivable_lanes(
      make_lanelet({100.0, 110.0}, {110.0, 100.0}, {100.0, 90.0}, {100.0, 90.0}));
    lanes[1] = l;
    const auto result = getOverlappedLaneletId(lanes);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 4UL);
  }
}

TEST(StaticDrivableArea, cutOverlappedLanes)
{
  using autoware::behavior_path_planner::utils::cutOverlappedLanes;
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  std::vector<DrivableLanes> lanes;
  {  // empty inputs
    const auto result = cutOverlappedLanes(path, lanes);
    EXPECT_TRUE(result.empty());
    EXPECT_TRUE(path.points.empty());
  }
  constexpr auto path_size = 10UL;
  const auto reset_path = [&]() {
    path.points.clear();
    for (auto i = 0UL; i < path_size; ++i) {
      path.points.emplace_back();
      path.points.back().point.pose.position.x = static_cast<double>(i) * 1.0;
      path.points.back().point.pose.position.y = 0.0;
    }
  };
  {  // add some path points
    reset_path();
    const auto result = cutOverlappedLanes(path, lanes);
    EXPECT_TRUE(result.empty());
    ASSERT_EQ(path.points.size(), path_size);
    for (auto i = 0UL; i < path_size; ++i) {
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.x, i * 1.0);
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.y, 0.0);
    }
  }
  {  // add some drivable lanes without any overlap (no overlap -> path is not modified)
    reset_path();
    lanes.push_back(
      make_drivable_lanes(make_lanelet({0.0, 1.0}, {2.0, 1.0}, {0.0, -1.0}, {2.0, -1.0})));
    const auto result = cutOverlappedLanes(path, lanes);
    ASSERT_EQ(result.size(), lanes.size());
    EXPECT_TRUE(equal(result.front(), lanes.front()));
    ASSERT_EQ(path.points.size(), path_size);
    for (auto i = 0UL; i < path_size; ++i) {
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.x, i * 1.0);
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.y, 0.0);
    }
  }
  {  // add more drivable lanes without an overlap (no overlap -> path is not modified)
    reset_path();
    lanes.push_back(
      make_drivable_lanes(make_lanelet({2.0, 1.0}, {4.0, 1.0}, {2.0, -1.0}, {4.0, -1.0})));
    lanes.push_back(
      make_drivable_lanes(make_lanelet({4.0, 1.0}, {6.0, 1.0}, {4.0, -1.0}, {6.0, -1.0})));
    const auto result = cutOverlappedLanes(path, lanes);
    ASSERT_EQ(result.size(), lanes.size());
    for (auto i = 0UL; i < result.size(); ++i) {
      EXPECT_TRUE(equal(result[i], lanes[i]));
    }
    ASSERT_EQ(path.points.size(), path_size);
    for (auto i = 0UL; i < path_size; ++i) {
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.x, i * 1.0);
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.y, 0.0);
    }
  }
  {  // add an overlapping lane
    reset_path();
    lanes.push_back(
      make_drivable_lanes(make_lanelet({2.5, -1.0}, {2.5, 1.0}, {3.5, -1.0}, {3.5, 1.0})));
    const auto result = cutOverlappedLanes(path, lanes);
    // the last lane is cut
    ASSERT_EQ(result.size() + 1, lanes.size());
    for (auto i = 0UL; i < result.size(); ++i) {
      EXPECT_TRUE(equal(result[i], lanes[i]));
    }
    // since the path points do not have ids, all points are cut
    EXPECT_TRUE(path.points.empty());
  }
  {  // add the overlapping lane id to the path points
    reset_path();
    for (auto & p : path.points) {
      p.lane_ids.push_back(lanes.back().left_lane.id());
    }
    cutOverlappedLanes(path, lanes);
    // since the overlapped lane was removed, the path points were still cut
    EXPECT_TRUE(path.points.empty());
  }
  {  // add the first lane id to some path points, only these points will be kept
    reset_path();
    constexpr auto filtered_start = 3UL;
    constexpr auto filtered_size = 5UL;
    for (auto i = filtered_start; i < filtered_start + filtered_size; ++i) {
      path.points[i].lane_ids.push_back(lanes.front().left_lane.id());
    }
    cutOverlappedLanes(path, lanes);
    ASSERT_EQ(path.points.size(), filtered_size);
    for (auto i = 0UL; i < filtered_size; ++i) {
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.x, (i + filtered_start) * 1.0);
      EXPECT_DOUBLE_EQ(path.points[i].point.pose.position.y, 0.0);
    }
  }
}

TEST(StaticDrivableArea, generateDrivableLanes)
{
  using autoware::behavior_path_planner::utils::generateDrivableLanes;
  lanelet::ConstLanelets lanelets;
  lanelet::Lanelet ll;
  {  // empty case
    const auto lanes = generateDrivableLanes(lanelets);
    EXPECT_TRUE(lanes.empty());
  }
  {  // single lanelet: it is found in the drivable lane's left/right lanes
    ll.setId(0);
    lanelets.push_back(ll);
    const auto lanes = generateDrivableLanes(lanelets);
    ASSERT_EQ(lanes.size(), lanelets.size());
    EXPECT_TRUE(lanes[0].middle_lanes.empty());
    EXPECT_EQ(lanes[0].left_lane.id(), lanelets[0].id());
    EXPECT_EQ(lanes[0].right_lane.id(), lanelets[0].id());
  }
  {  // many lanelets: they are all in the calculated drivable lane
    for (auto i = 1; i < 20; ++i) {
      ll.setId(0);
      lanelets.push_back(ll);
    }
    const auto lanes = generateDrivableLanes(lanelets);
    ASSERT_EQ(lanes.size(), lanelets.size());
    for (auto i = 0UL; i < lanes.size(); ++i) {
      EXPECT_TRUE(lanes[i].middle_lanes.empty());
      EXPECT_EQ(lanes[i].left_lane.id(), lanelets[i].id());
      EXPECT_EQ(lanes[i].right_lane.id(), lanelets[i].id());
    }
  }
}

TEST(StaticDrivableArea, generateDrivableArea_subfunction)
{
  using autoware::behavior_path_planner::utils::generateDrivableArea;
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  autoware_internal_planning_msgs::msg::PathPointWithLaneId p;
  generateDrivableArea(path, 0.0, 0.0, true);
  EXPECT_TRUE(path.left_bound.empty());
  EXPECT_TRUE(path.right_bound.empty());
  // add only 1 point : drivable area with 1 point
  p.point.pose.position.set__x(0.0).set__y(0.0);
  path.points.push_back(p);
  auto lon_offset = 0.0;
  auto lat_offset = 0.0;
  generateDrivableArea(path, lon_offset, lat_offset, true);
  // 3 points in the resulting drivable area: 1 for the path point and 2 for front/rear offset
  ASSERT_EQ(path.left_bound.size(), 3UL);
  ASSERT_EQ(path.right_bound.size(), 3UL);
  // no offset so we expect exactly the same points
  for (auto i = 0UL; i < 3UL; ++i) {
    EXPECT_TRUE(equal(path.points.front().point.pose.position, path.left_bound[i]));
    EXPECT_TRUE(equal(path.points.front().point.pose.position, path.right_bound[i]));
  }
  // add some offset
  lon_offset = 1.0;
  lat_offset = 0.5;
  generateDrivableArea(path, lon_offset, lat_offset, true);
  ASSERT_EQ(path.left_bound.size(), 3UL);
  ASSERT_EQ(path.right_bound.size(), 3UL);
  EXPECT_DOUBLE_EQ(path.left_bound[0].x, -lon_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[1].x, 0.0);
  EXPECT_DOUBLE_EQ(path.left_bound[2].x, lon_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[0].x, -lon_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[1].x, 0.0);
  EXPECT_DOUBLE_EQ(path.right_bound[2].x, lon_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[0].y, lat_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[1].y, lat_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[2].y, lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[0].y, -lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[1].y, -lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[2].y, -lat_offset);
  // set driving_forward to false: longitudinal offset is inversely applied
  generateDrivableArea(path, lon_offset, lat_offset, false);
  ASSERT_EQ(path.left_bound.size(), 3UL);
  ASSERT_EQ(path.right_bound.size(), 3UL);
  EXPECT_DOUBLE_EQ(path.left_bound[0].x, lon_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[1].x, 0.0);
  EXPECT_DOUBLE_EQ(path.left_bound[2].x, -lon_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[0].x, lon_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[1].x, 0.0);
  EXPECT_DOUBLE_EQ(path.right_bound[2].x, -lon_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[0].y, lat_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[1].y, lat_offset);
  EXPECT_DOUBLE_EQ(path.left_bound[2].y, lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[0].y, -lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[1].y, -lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound[2].y, -lat_offset);
  // add more points
  for (auto x = 1; x < 10; ++x) {
    // space points by more than 2m to avoid resampling
    p.point.pose.position.set__x(x * 3.0).set__y(0.0);
    path.points.push_back(p);
  }
  generateDrivableArea(path, lon_offset, lat_offset, true);
  ASSERT_EQ(path.left_bound.size(), path.points.size() + 2UL);
  ASSERT_EQ(path.right_bound.size(), path.points.size() + 2UL);
  EXPECT_DOUBLE_EQ(path.left_bound.front().x, -lon_offset);
  EXPECT_DOUBLE_EQ(path.right_bound.front().x, -lon_offset);
  EXPECT_DOUBLE_EQ(path.left_bound.back().x, path.points.back().point.pose.position.x + lon_offset);
  EXPECT_DOUBLE_EQ(
    path.right_bound.back().x, path.points.back().point.pose.position.x + lon_offset);
  EXPECT_DOUBLE_EQ(path.left_bound.front().y, lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound.front().y, -lat_offset);
  EXPECT_DOUBLE_EQ(path.left_bound.back().y, lat_offset);
  EXPECT_DOUBLE_EQ(path.right_bound.back().y, -lat_offset);
  for (auto i = 1UL; i + 1 < path.points.size(); ++i) {
    const auto & path_p = path.points[i - 1].point.pose.position;
    EXPECT_DOUBLE_EQ(path.left_bound[i].x, path_p.x);
    EXPECT_DOUBLE_EQ(path.right_bound[i].x, path_p.x);
    EXPECT_DOUBLE_EQ(path.left_bound[i].y, path_p.y + lat_offset);
    EXPECT_DOUBLE_EQ(path.right_bound[i].y, path_p.y - lat_offset);
  }
  // case with self intersections
  path.points.clear();
  p.point.pose.position.set__x(0.0).set__y(0.0);
  path.points.push_back(p);
  p.point.pose.position.set__x(3.0).set__y(0.0);
  path.points.push_back(p);
  p.point.pose.position.set__x(0.0).set__y(3.0);
  path.points.push_back(p);
  lon_offset = 0.0;
  lat_offset = 3.0;
  generateDrivableArea(path, lon_offset, lat_offset, false);
  // TODO(Anyone): self intersection case looks buggy
  ASSERT_EQ(path.left_bound.size(), path.points.size() + 2UL);
  ASSERT_EQ(path.right_bound.size(), path.points.size() + 2UL);
  EXPECT_TRUE(equal(path.left_bound[0], path.left_bound[1]));
  EXPECT_TRUE(equal(path.right_bound[0], path.right_bound[1]));
  EXPECT_TRUE(equal(path.left_bound[3], path.left_bound[4]));
  EXPECT_TRUE(equal(path.right_bound[3], path.right_bound[4]));
  EXPECT_DOUBLE_EQ(path.left_bound[1].x, 0.0);
  EXPECT_DOUBLE_EQ(path.left_bound[1].y, 3.0);
  EXPECT_DOUBLE_EQ(path.left_bound[2].x, 3.0);
  EXPECT_DOUBLE_EQ(path.left_bound[2].y, 3.0);
  EXPECT_DOUBLE_EQ(path.left_bound[3].x, 0.0);
  EXPECT_DOUBLE_EQ(path.left_bound[3].y, 6.0);
  EXPECT_DOUBLE_EQ(path.right_bound[1].x, 0.0);
  EXPECT_DOUBLE_EQ(path.right_bound[1].y, -3.0);
  EXPECT_DOUBLE_EQ(path.right_bound[2].x, 3.0);
  EXPECT_DOUBLE_EQ(path.right_bound[2].y, -3.0);
  EXPECT_DOUBLE_EQ(path.right_bound[3].x, 0.0);
  EXPECT_DOUBLE_EQ(path.right_bound[3].y, 0.0);
}

TEST(StaticDrivableArea, getBoundWithIntersectionAreas)
{
  using autoware::behavior_path_planner::utils::getBoundWithIntersectionAreas;
  std::vector<lanelet::ConstPoint3d> original_bound;
  std::shared_ptr<autoware::route_handler::RouteHandler> route_handler =
    std::make_shared<autoware::route_handler::RouteHandler>();
  std::vector<DrivableLanes> drivable_lanes;
  bool is_left = false;
  auto result =
    getBoundWithIntersectionAreas(original_bound, route_handler, drivable_lanes, is_left);
  EXPECT_TRUE(result.empty());

  route_handler->setMap(intersection_map);
  DrivableLanes lanes;
  const auto lanelet_with_intersection_area = route_handler->getLaneletsFromId(3101);
  lanes.middle_lanes = {};
  lanes.right_lane = lanelet_with_intersection_area;
  lanes.left_lane = lanelet_with_intersection_area;
  for (const auto & p : lanelet_with_intersection_area.rightBound()) {
    original_bound.push_back(p);
  }
  drivable_lanes.push_back(lanes);
  result = getBoundWithIntersectionAreas(original_bound, route_handler, drivable_lanes, is_left);
  // the expanded bound includes the intersection area so its size is larger
  EXPECT_GT(result.size(), original_bound.size());
}

TEST(StaticDrivableArea, combineDrivableAreaInfo)
{
  using autoware::behavior_path_planner::utils::combineDrivableAreaInfo;
  autoware::behavior_path_planner::DrivableAreaInfo drivable_area_info1;
  autoware::behavior_path_planner::DrivableAreaInfo drivable_area_info2;
  {
    const auto combined = combineDrivableAreaInfo(drivable_area_info1, drivable_area_info2);
    EXPECT_TRUE(combined.drivable_lanes.empty());
  }
  {  // combination of obstacles
    drivable_area_info1.obstacles.emplace_back().pose.position.x = 1.0;
    drivable_area_info2.obstacles.emplace_back().pose.position.x = 2.0;
    const auto combined = combineDrivableAreaInfo(drivable_area_info1, drivable_area_info2);
    ASSERT_EQ(combined.obstacles.size(), 2UL);
    EXPECT_DOUBLE_EQ(combined.obstacles[0].pose.position.x, 1.0);
    EXPECT_DOUBLE_EQ(combined.obstacles[1].pose.position.x, 2.0);
  }
  {  // combination of the drivable lanes
    DrivableLanes lanes;
    lanes.middle_lanes.push_back(lanelet::Lanelet(0));
    lanes.middle_lanes.push_back(lanelet::Lanelet(1));
    drivable_area_info1.drivable_lanes.push_back(lanes);
    lanes.middle_lanes.clear();
    lanes.middle_lanes.push_back(lanelet::Lanelet(2));
    lanes.middle_lanes.push_back(lanelet::Lanelet(3));
    drivable_area_info2.drivable_lanes.push_back(lanes);
    const auto combined = combineDrivableAreaInfo(drivable_area_info1, drivable_area_info2);
    ASSERT_EQ(combined.drivable_lanes.size(), 1UL);
    ASSERT_EQ(combined.drivable_lanes[0].middle_lanes.size(), 4UL);
    for (auto id = 0UL; id < combined.drivable_lanes[0].middle_lanes.size(); ++id) {
      EXPECT_EQ(combined.drivable_lanes[0].middle_lanes[id].id(), id);
    }
  }
  {  // combination of the parameters
    drivable_area_info1.drivable_margin = 5.0;
    drivable_area_info2.drivable_margin = 2.0;
    drivable_area_info1.enable_expanding_freespace_areas = false;
    drivable_area_info2.enable_expanding_freespace_areas = false;
    drivable_area_info1.enable_expanding_intersection_areas = true;
    drivable_area_info2.enable_expanding_intersection_areas = true;
    drivable_area_info1.enable_expanding_hatched_road_markings = false;
    drivable_area_info2.enable_expanding_hatched_road_markings = true;
    const auto combined = combineDrivableAreaInfo(drivable_area_info1, drivable_area_info2);
    EXPECT_DOUBLE_EQ(combined.drivable_margin, 5.0);  // expect the maximum of the margins
    // expect OR of the booleans
    EXPECT_FALSE(combined.enable_expanding_freespace_areas);
    EXPECT_TRUE(combined.enable_expanding_intersection_areas);
    EXPECT_TRUE(combined.enable_expanding_hatched_road_markings);
  }
}

TEST(DISABLED_StaticDrivableArea, generateDrivableArea)
{
  using autoware::behavior_path_planner::PlannerData;
  using autoware::behavior_path_planner::utils::generateDrivableArea;
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  std::vector<DrivableLanes> lanes;
  bool enable_expanding_hatched_road_markings = true;
  bool enable_expanding_intersection_areas = true;
  bool enable_expanding_freespace_areas = true;
  bool is_driving_forward = true;
  PlannerData planner_data;
  planner_data.drivable_area_expansion_parameters.enabled = false;  // disable dynamic expansion
  planner_data.parameters.ego_nearest_dist_threshold = 1.0;
  planner_data.parameters.ego_nearest_yaw_threshold = M_PI;
  auto planner_data_ptr = std::make_shared<PlannerData>(planner_data);
  // empty: no crash
  EXPECT_NO_FATAL_FAILURE(generateDrivableArea(
    path, lanes, enable_expanding_hatched_road_markings, enable_expanding_intersection_areas,
    enable_expanding_freespace_areas, planner_data_ptr, is_driving_forward));
  planner_data.route_handler = std::make_shared<autoware::route_handler::RouteHandler>();
  planner_data.route_handler->setMap(intersection_map);
  // create a path from a lanelet centerline
  constexpr auto lanelet_id = 3008377;
  const auto ll = planner_data.route_handler->getLaneletsFromId(lanelet_id);
  const auto shoulder_ll = planner_data.route_handler->getLaneletsFromId(3008385);
  lanes = autoware::behavior_path_planner::utils::generateDrivableLanes({ll, shoulder_ll});
  lanelet::BasicLineString2d path_ls;
  for (const auto & p : ll.centerline().basicLineString()) {
    autoware_internal_planning_msgs::msg::PathPointWithLaneId pp;
    pp.point.pose.position.x = p.x();
    pp.point.pose.position.y = p.y();
    pp.point.pose.position.z = p.z();
    pp.lane_ids = {lanelet_id};
    path.points.push_back(pp);
    path_ls.emplace_back(p.x(), p.y());
  }
  auto odometry = nav_msgs::msg::Odometry();
  odometry.pose.pose = path.points.front().point.pose;
  planner_data.self_odometry = std::make_shared<nav_msgs::msg::Odometry>(odometry);
  planner_data_ptr = std::make_shared<PlannerData>(planner_data);
  generateDrivableArea(
    path, lanes, enable_expanding_hatched_road_markings, enable_expanding_intersection_areas,
    enable_expanding_freespace_areas, planner_data_ptr, is_driving_forward);

  ASSERT_EQ(path.left_bound.size(), ll.leftBound().size());
  ASSERT_EQ(path.right_bound.size(), ll.rightBound().size());
  {
    lanelet::BasicLineString2d left_ls;
    lanelet::BasicLineString2d right_ls;
    for (const auto & p : path.left_bound) {
      left_ls.emplace_back(p.x, p.y);
    }
    for (const auto & p : path.right_bound) {
      right_ls.emplace_back(p.x, p.y);
    }
    EXPECT_FALSE(boost::geometry::intersects(path_ls, left_ls));
    EXPECT_FALSE(boost::geometry::intersects(path_ls, right_ls));  // TODO(someone): this is failing
  }
  // reverse case
  is_driving_forward = false;
  odometry.pose.pose = std::prev(path.points.end(), 2)->point.pose;
  planner_data.self_odometry = std::make_shared<nav_msgs::msg::Odometry>(odometry);
  planner_data_ptr = std::make_shared<PlannerData>(planner_data);
  generateDrivableArea(
    path, lanes, enable_expanding_hatched_road_markings, enable_expanding_intersection_areas,
    enable_expanding_freespace_areas, planner_data_ptr, is_driving_forward);
  ASSERT_EQ(path.left_bound.size(), ll.leftBound().size());
  ASSERT_EQ(path.right_bound.size(), ll.rightBound().size());
  {
    lanelet::BasicLineString2d left_ls;
    lanelet::BasicLineString2d right_ls;
    for (const auto & p : path.left_bound) {
      left_ls.emplace_back(p.x, p.y);
    }
    for (const auto & p : path.right_bound) {
      right_ls.emplace_back(p.x, p.y);
    }
    EXPECT_FALSE(boost::geometry::intersects(path_ls, left_ls));
    EXPECT_FALSE(boost::geometry::intersects(path_ls, right_ls));
  }
}

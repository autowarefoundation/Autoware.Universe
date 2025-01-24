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

#include "../src/utils.hpp"

#include <autoware/behavior_velocity_planner_common/planner_data.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/logger.hpp>

#include <autoware_perception_msgs/msg/predicted_object.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <autoware_internal_planning_msgs/msg/path_point_with_lane_id.hpp>
#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/io/wkt/write.hpp>

#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>

#include <stdexcept>

template <class Point, class Polygon>
bool point_in_polygon(const Point & p, const Polygon & poly)
{
  return std::find_if(poly.outer().begin(), poly.outer().end(), [&](const auto & o) {
           return p.x() == o.x() && p.y() == o.y();
         }) != poly.outer().end();
}

autoware_internal_planning_msgs::msg::PathWithLaneId generate_straight_path(
  const size_t nb_points, const float velocity = 0.0, const double resolution = 1.0)
{
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  for (auto x = 0UL; x < nb_points; ++x) {
    autoware_internal_planning_msgs::msg::PathPointWithLaneId p;
    p.point.pose.position.x = resolution * static_cast<double>(x);
    p.point.pose.position.y = 0.0;
    p.point.longitudinal_velocity_mps = velocity;
    path.points.push_back(p);
  }
  return path;
}

TEST(NoStoppingAreaTest, isTargetStuckVehicleType)
{
  using autoware::behavior_velocity_planner::no_stopping_area::is_vehicle_type;
  autoware_perception_msgs::msg::PredictedObject object;
  EXPECT_NO_FATAL_FAILURE(is_vehicle_type(object));
  autoware_perception_msgs::msg::ObjectClassification classification;
  for (const auto label : {
         autoware_perception_msgs::msg::ObjectClassification::CAR,
         autoware_perception_msgs::msg::ObjectClassification::BUS,
         autoware_perception_msgs::msg::ObjectClassification::TRUCK,
         autoware_perception_msgs::msg::ObjectClassification::TRAILER,
         autoware_perception_msgs::msg::ObjectClassification::MOTORCYCLE,
       }) {
    classification.label = label;
    object.classification = {classification};
    EXPECT_TRUE(is_vehicle_type(object));
  }
}

TEST(NoStoppingAreaTest, insertStopPoint)
{
  using autoware::behavior_velocity_planner::no_stopping_area::insert_stop_point;
  constexpr auto nb_points = 10;
  constexpr auto default_velocity = 5.0;
  const autoware_internal_planning_msgs::msg::PathWithLaneId base_path =
    generate_straight_path(nb_points, default_velocity);
  autoware::behavior_velocity_planner::no_stopping_area::PathIndexWithPose stop_point;
  // stop exactly at a point, expect no new points but a 0 velocity at the stop point and after
  auto path = base_path;
  stop_point.first = 4UL;
  stop_point.second = path.points[stop_point.first].point.pose;
  insert_stop_point(path, stop_point);
  ASSERT_EQ(path.points.size(), nb_points);
  for (auto i = 0UL; i < stop_point.first; ++i) {
    EXPECT_EQ(path.points[i].point.longitudinal_velocity_mps, default_velocity);
  }
  for (auto i = stop_point.first; i < nb_points; ++i) {
    EXPECT_EQ(path.points[i].point.longitudinal_velocity_mps, 0.0);
  }

  // stop between points
  path = base_path;
  stop_point.first = 3UL;
  stop_point.second.position.x = 3.5;
  insert_stop_point(path, stop_point);
  ASSERT_EQ(path.points.size(), nb_points + 1);
  EXPECT_EQ(path.points[stop_point.first + 1].point.pose.position.x, stop_point.second.position.x);
  for (auto i = 0UL; i <= stop_point.first; ++i) {
    EXPECT_EQ(path.points[i].point.longitudinal_velocity_mps, default_velocity);
  }
  for (auto i = stop_point.first + 1; i < nb_points + 1; ++i) {
    EXPECT_EQ(path.points[i].point.longitudinal_velocity_mps, 0.0);
  }

  // stop at the last point: exception  // TODO(anyone): is this okay ?
  path = base_path;
  stop_point.first = path.points.size() - 1;
  stop_point.second = path.points.back().point.pose;
  EXPECT_THROW(insert_stop_point(path, stop_point), std::out_of_range);
}

TEST(NoStoppingAreaTest, generateStopLine)
{
  using autoware::behavior_velocity_planner::no_stopping_area::generate_stop_line;
  constexpr auto nb_points = 10;
  const autoware_internal_planning_msgs::msg::PathWithLaneId path = generate_straight_path(nb_points);
  lanelet::ConstPolygons3d no_stopping_areas;
  double ego_width = 0.0;
  double stop_line_margin = 0.0;
  // no areas, expect no stop line
  auto stop_line = generate_stop_line(path, no_stopping_areas, ego_width, stop_line_margin);
  EXPECT_FALSE(stop_line.has_value());

  // area outside of the path
  lanelet::Polygon3d poly;
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 3.0, -2.0));
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 3.0, -0.5));
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, -0.5));
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, -2.0));
  no_stopping_areas.push_back(poly);
  stop_line = generate_stop_line(path, no_stopping_areas, ego_width, stop_line_margin);
  EXPECT_FALSE(stop_line.has_value());
  // area on the path, 0 margin and 0 width
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, -1.0));
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, 1.0));
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 6.0, 1.0));
  poly.push_back(lanelet::Point3d(lanelet::InvalId, 6.0, -1.0));
  no_stopping_areas.push_back(poly);
  stop_line = generate_stop_line(path, no_stopping_areas, ego_width, stop_line_margin);
  EXPECT_TRUE(stop_line.has_value());
  ASSERT_EQ(stop_line->size(), 2UL);
  EXPECT_EQ(stop_line->front().x(), 5.0);
  EXPECT_EQ(stop_line->front().y(), 0.0);
  EXPECT_EQ(stop_line->back().x(), 5.0);
  EXPECT_EQ(stop_line->back().y(), 0.0);
  // set a margin
  stop_line_margin = 1.0;
  stop_line = generate_stop_line(path, no_stopping_areas, ego_width, stop_line_margin);
  EXPECT_TRUE(stop_line.has_value());
  ASSERT_EQ(stop_line->size(), 2UL);
  EXPECT_EQ(stop_line->front().x(), 4.0);
  EXPECT_EQ(stop_line->front().y(), 0.0);
  EXPECT_EQ(stop_line->back().x(), 4.0);
  EXPECT_EQ(stop_line->back().y(), 0.0);
  // set a width
  ego_width = 2.0;
  stop_line = generate_stop_line(path, no_stopping_areas, ego_width, stop_line_margin);
  EXPECT_TRUE(stop_line.has_value());
  ASSERT_EQ(stop_line->size(), 2UL);
  // TODO(anyone): if we stop at this stop line ego overlaps the 1st area, is this okay ?
  EXPECT_EQ(stop_line->front().x(), 4.0);
  EXPECT_EQ(stop_line->front().y(), 2.0);
  EXPECT_EQ(stop_line->back().x(), 4.0);
  EXPECT_EQ(stop_line->back().y(), -2.0);
}

TEST(NoStoppingAreaTest, isStoppable)
{
  using autoware::behavior_velocity_planner::no_stopping_area::is_stoppable;
  autoware::behavior_velocity_planner::no_stopping_area::PassJudge pass_judge;
  geometry_msgs::msg::Pose self_pose;
  geometry_msgs::msg::Pose line_pose;
  rclcpp::Clock clock;
  auto logger = rclcpp::get_logger("test_logger");
  logger.set_level(rclcpp::Logger::Level::Debug);
  autoware::behavior_velocity_planner::no_stopping_area::EgoData ego_data;
  ego_data.current_velocity = 1.0;
  ego_data.current_acceleration = 0.0;
  ego_data.delay_response_time = 0.0;
  ego_data.max_stop_acc = -1.0;
  ego_data.max_stop_jerk = -1.0;
  // try to stop 1m ahead
  self_pose.position.x = 0.0;
  self_pose.position.y = 0.0;
  line_pose.position.x = 1.0;
  line_pose.position.y = 0.0;

  // enough distance to stop and slow velocity
  EXPECT_TRUE(is_stoppable(pass_judge, self_pose, line_pose, ego_data, logger, clock));
  EXPECT_TRUE(pass_judge.is_stoppable);
  EXPECT_FALSE(pass_judge.pass_judged);

  // unstoppable and fast velocity
  ego_data.current_velocity = 10.0;
  EXPECT_FALSE(is_stoppable(pass_judge, self_pose, line_pose, ego_data, logger, clock));
  EXPECT_FALSE(pass_judge.is_stoppable);
  EXPECT_TRUE(pass_judge.pass_judged);

  // unstoppable and slow velocity, but since we already judged, it is not updated
  ego_data.current_velocity = 1.9;
  EXPECT_FALSE(is_stoppable(pass_judge, self_pose, line_pose, ego_data, logger, clock));
  EXPECT_FALSE(pass_judge.is_stoppable);
  EXPECT_TRUE(pass_judge.pass_judged);

  // reset and result is updated
  // TODO(someone): is this the correct behavior ? distance is unstoppable but result is stoppable
  pass_judge.pass_judged = false;
  EXPECT_TRUE(is_stoppable(pass_judge, self_pose, line_pose, ego_data, logger, clock));
  EXPECT_TRUE(pass_judge.is_stoppable);
  EXPECT_TRUE(pass_judge.pass_judged);
}

TEST(NoStoppingAreaTest, generateEgoNoStoppingAreaLanePolygon)
{
  using autoware::behavior_velocity_planner::no_stopping_area::
    generate_ego_no_stopping_area_lane_polygon;
  using autoware::universe_utils::Point2d;
  geometry_msgs::msg::Pose ego_pose;  // ego at (0,0)
  ego_pose.position.x = 0.0;
  ego_pose.position.y = 0.0;
  const autoware_internal_planning_msgs::msg::PathWithLaneId path =
    generate_straight_path(8);  // ego path at x = 0, 1,..., 7 and y=0
  double margin = 1.0;          // desired margin added to the polygon after the end of the area
  double max_polygon_length = 10.0;  // maximum length of the generated polygon
  double path_expand_width = 2.0;    // width of the generated polygon
  auto logger = rclcpp::get_logger("test_logger");
  logger.set_level(rclcpp::Logger::Level::Debug);
  rclcpp::Clock clock;

  lanelet::Polygon3d no_stopping_area;
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.0, -1.0));
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.0, 1.0));
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, 1.0));
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, -1.0));
  const auto no_stopping_area_reg_elem =
    lanelet::autoware::NoStoppingArea::make(lanelet::InvalId, {}, {no_stopping_area}, {});
  // basic case
  {
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      path, ego_pose, *no_stopping_area_reg_elem, margin, max_polygon_length, path_expand_width,
      logger, clock);
    autoware::universe_utils::Polygon2d simplified_poly;
    EXPECT_FALSE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
    // simplify the polygon to make it easier to check
    boost::geometry::simplify(lane_poly, simplified_poly, 1.0);
    EXPECT_EQ(simplified_poly.outer().size(), 5UL);  // closed polygon so 1st point == last point
    // polygon over the path/no_stop_area overlap and with the desired width
    // TODO(someone): we expect x=6 (end of area [5] + margin [1]) but current is 5.5 because a
    // point is counted twice
    // EXPECT_TRUE(point_in_polygon(Point2d(6.0, 2.0), simplified_poly));
    // EXPECT_TRUE(point_in_polygon(Point2d(6.0, -2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(3.0, 2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(3.0, -2.0), simplified_poly));
  }

  // big margin -> get a polygon until the end of the path
  {
    const double big_margin = 10.0;
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      path, ego_pose, *no_stopping_area_reg_elem, big_margin, max_polygon_length, path_expand_width,
      logger, clock);
    autoware::universe_utils::Polygon2d simplified_poly;
    EXPECT_FALSE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
    // simplify the polygon to make it easier to check
    boost::geometry::simplify(lane_poly, simplified_poly, 1.0);
    EXPECT_EQ(simplified_poly.outer().size(), 5UL);  // closed polygon so 1st point == last point
    // polygon over the path/no_stop_area overlap and with the desired width
    EXPECT_TRUE(point_in_polygon(Point2d(7.0, 2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(7.0, -2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(3.0, 2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(3.0, -2.0), simplified_poly));
  }
  // small max polygon length
  {
    const double small_polygon_length = 5.0;
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      path, ego_pose, *no_stopping_area_reg_elem, margin, small_polygon_length, path_expand_width,
      logger, clock);
    autoware::universe_utils::Polygon2d simplified_poly;
    EXPECT_FALSE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
    // simplify the polygon to make it easier to check
    boost::geometry::simplify(lane_poly, simplified_poly, 1.0);
    EXPECT_EQ(simplified_poly.outer().size(), 5UL);  // closed polygon so 1st point == last point
    // polygon over the path/no_stop_area overlap and with the desired width
    // TODO(someone): the length calculation is currently buggy
    // ego at x=0, no_stopping_area starts at x=3 so we expect a lane polygon of length = 2.0, but
    // some point is counted twice so the length is only 1.5 (interpolation interval is 0.5)
    // EXPECT_TRUE(point_in_polygon(Point2d(4.0, 2.0), simplified_poly));
    // EXPECT_TRUE(point_in_polygon(Point2d(4.0, -2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(3.0, 2.0), simplified_poly));
    EXPECT_TRUE(point_in_polygon(Point2d(3.0, -2.0), simplified_poly));
  }

  // cases where the polygon returned is empty
  // path is empty
  {
    autoware_internal_planning_msgs::msg::PathWithLaneId empty_path;
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      empty_path, ego_pose, *no_stopping_area_reg_elem, margin, max_polygon_length,
      path_expand_width, logger, clock);
    EXPECT_TRUE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
  }
  // path points do not enter the no stopping area
  // ego is far from the path
  {
    auto far_ego_pose = ego_pose;
    far_ego_pose.position.y = 10.0;
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      path, far_ego_pose, *no_stopping_area_reg_elem, margin, max_polygon_length, path_expand_width,
      logger, clock);
    EXPECT_TRUE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
  }
  // no stopping area starts after the minimum length
  {
    const double short_max_polygon_length = 2.0;
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      path, ego_pose, *no_stopping_area_reg_elem, margin, short_max_polygon_length,
      path_expand_width, logger, clock);
    EXPECT_TRUE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
  }
  // path crosses the no stopping area but the current interpolation (0.5) is not enough to detect
  // it
  // TODO(someone): should this be fixed ?
  {
    lanelet::Polygon3d small_no_stopping_area;
    small_no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.1, -1.0));
    small_no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.1, 1.0));
    small_no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.4, 1.0));
    small_no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.4, -1.0));
    const auto small_no_stopping_area_reg_elem =
      lanelet::autoware::NoStoppingArea::make(lanelet::InvalId, {}, {small_no_stopping_area}, {});
    const auto lane_poly = generate_ego_no_stopping_area_lane_polygon(
      path, ego_pose, *small_no_stopping_area_reg_elem, margin, max_polygon_length,
      path_expand_width, logger, clock);
    EXPECT_TRUE(lane_poly.outer().empty());
    EXPECT_TRUE(lane_poly.inners().empty());
  }
}

TEST(NoStoppingAreaTest, checkStopLinesInNoStoppingArea)
{
  using autoware::behavior_velocity_planner::no_stopping_area::check_stop_lines_in_no_stopping_area;
  autoware_internal_planning_msgs::msg::PathWithLaneId path;
  autoware::universe_utils::Polygon2d poly;
  autoware::behavior_velocity_planner::no_stopping_area::DebugData debug_data;

  // empty inputs
  EXPECT_FALSE(check_stop_lines_in_no_stopping_area(path, poly, debug_data));

  constexpr auto nb_points = 10;
  constexpr auto non_stopped_velocity = 5.0;
  const autoware_internal_planning_msgs::msg::PathWithLaneId non_stopping_path =
    generate_straight_path(nb_points, non_stopped_velocity);
  path = non_stopping_path;
  // set x=4 and x=5 to be stopping points
  path.points[4].point.longitudinal_velocity_mps = 0.0;
  path.points[5].point.longitudinal_velocity_mps = 0.0;
  // empty polygon
  EXPECT_FALSE(check_stop_lines_in_no_stopping_area(path, poly, debug_data));
  // non stopping path
  poly.outer().emplace_back(3.0, -1.0);
  poly.outer().emplace_back(3.0, 1.0);
  poly.outer().emplace_back(5.0, 1.0);
  poly.outer().emplace_back(5.0, -1.0);
  poly.outer().push_back(poly.outer().front());  // close the polygon
  EXPECT_FALSE(check_stop_lines_in_no_stopping_area(non_stopping_path, poly, debug_data));
  // stop in the area
  EXPECT_TRUE(check_stop_lines_in_no_stopping_area(path, poly, debug_data));
  // if stop in the area is within 1m of the end of the path we ignore it: only for 1st stop
  path.points[8].point.longitudinal_velocity_mps = 0.0;
  path.points[9].point.longitudinal_velocity_mps = 0.0;
  EXPECT_TRUE(check_stop_lines_in_no_stopping_area(path, poly, debug_data));
  // if stop in the area is within 1m of the end of the path we ignore it
  path.points[4].point.longitudinal_velocity_mps = non_stopped_velocity;
  path.points[5].point.longitudinal_velocity_mps = non_stopped_velocity;
  EXPECT_FALSE(check_stop_lines_in_no_stopping_area(path, poly, debug_data));
}

TEST(NoStoppingAreaTest, getStopLineGeometry2d)
{
  using autoware::behavior_velocity_planner::no_stopping_area::generate_stop_line;
  using autoware::behavior_velocity_planner::no_stopping_area::get_stop_line_geometry2d;
  const autoware_internal_planning_msgs::msg::PathWithLaneId path = generate_straight_path(10);
  lanelet::Polygon3d no_stopping_area;
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.0, -1.0));
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 3.0, 1.0));
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, 1.0));
  no_stopping_area.push_back(lanelet::Point3d(lanelet::InvalId, 5.0, -1.0));
  double stop_line_margin = 1.0;
  double stop_line_extend_length = 1.0;
  double vehicle_width = 1.0;
  {  // get stop line of the regulatory element extended by the extend length
    lanelet::LineString3d reg_elem_stop_line;
    reg_elem_stop_line.push_back(lanelet::Point3d(lanelet::InvalId, 0.0, 0.0));
    reg_elem_stop_line.push_back(lanelet::Point3d(lanelet::InvalId, 1.0, 0.0));
    const auto no_stopping_area_reg_elem = lanelet::autoware::NoStoppingArea::make(
      lanelet::InvalId, {}, {no_stopping_area}, reg_elem_stop_line);
    const auto stop_line = get_stop_line_geometry2d(
      path, *no_stopping_area_reg_elem, stop_line_margin, stop_line_extend_length, vehicle_width);
    ASSERT_TRUE(stop_line.has_value());
    ASSERT_EQ(stop_line->size(), 2UL);
    EXPECT_EQ(stop_line->front().x(), -1.0);
    EXPECT_EQ(stop_line->front().y(), 0.0);
    EXPECT_EQ(stop_line->back().x(), 2.0);
    EXPECT_EQ(stop_line->back().y(), 0.0);
  }
  {  // regulatory element has no stop line -> get the same stop line as generate_stop_line
    const auto no_stopping_area_reg_elem =
      lanelet::autoware::NoStoppingArea::make(lanelet::InvalId, {}, {no_stopping_area}, {});
    const auto stop_line = get_stop_line_geometry2d(
      path, *no_stopping_area_reg_elem, stop_line_margin, stop_line_extend_length, vehicle_width);
    const auto generated_stop_line =
      generate_stop_line(path, {no_stopping_area}, vehicle_width, stop_line_margin);
    ASSERT_TRUE(stop_line.has_value());
    ASSERT_TRUE(generated_stop_line.has_value());
    ASSERT_EQ(stop_line->size(), generated_stop_line->size());
    for (auto i = 0UL; i < stop_line->size(); ++i) {
      EXPECT_EQ(stop_line->at(i).x(), generated_stop_line->at(i).x());
      EXPECT_EQ(stop_line->at(i).y(), generated_stop_line->at(i).y());
    }
  }
}

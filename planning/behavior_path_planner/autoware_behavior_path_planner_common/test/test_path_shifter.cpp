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

#include "autoware/behavior_path_planner_common/utils/path_shifter/path_shifter.hpp"

#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <autoware_planning_msgs/msg/detail/trajectory__struct.hpp>

#include <gtest/gtest.h>

#include <utility>
#include <vector>

namespace autoware::behavior_path_planner
{

class PathShifterTest : public ::testing::Test
{
protected:
  PathShifter path_shifter_;
  ShiftedPath path;

  void SetUp() override { path_shifter_.set_base_offset(0.01); }

  static ShiftedPath generate_shifted_path(
    size_t num_points, double longitudinal_interval, double lateral_interval)
  {
    ShiftedPath path;
    auto trajectory =
      autoware::test_utils::generateTrajectory<autoware_planning_msgs::msg::Trajectory>(
        num_points, longitudinal_interval);

    size_t i = 0;
    for (auto const & point : trajectory.points) {
      PathPointWithLaneId path_point_with_lane_id;
      path_point_with_lane_id.point.pose = point.pose;
      path_point_with_lane_id.point.lateral_velocity_mps = point.lateral_velocity_mps;
      path_point_with_lane_id.point.longitudinal_velocity_mps = point.longitudinal_velocity_mps;
      path_point_with_lane_id.point.heading_rate_rps = point.heading_rate_rps;
      path.path.points.push_back(path_point_with_lane_id);
      path.shift_length.push_back(static_cast<double>(i) * lateral_interval);
      i++;
    }

    return path;
  }

  bool check_shift_lines_alignment(const ShiftLineArray & shift_lines)
  {
    return path_shifter_.check_shift_lines_alignment(shift_lines);
  }

  static void add_lateral_offset_on_index_point(ShiftedPath * path, double offset, size_t index)
  {
    PathShifter::add_lateral_offset_on_index_point(path, offset, index);
  }

  static void shift_base_length(ShiftedPath * path, double offset)
  {
    PathShifter::shift_base_length(path, offset);
  }

  void sort_shift_lines_along_path(ShiftLineArray & shift_lines)
  {
    path_shifter_.sort_shift_lines_along_path(shift_lines);
  }

  static std::pair<std::vector<double>, std::vector<double>> get_base_lengths_without_accel_limit(
    const double arc_length, const double shift_length, const bool offset_back)
  {
    return PathShifter::get_base_lengths_without_accel_limit(arc_length, shift_length, offset_back);
  }

  static std::pair<std::vector<double>, std::vector<double>> get_base_lengths_without_accel_limit(
    const double arc_length, const double shift_length, const double velocity,
    const double longitudinal_acc, const double total_time, const bool offset_back)
  {
    return PathShifter::get_base_lengths_without_accel_limit(
      arc_length, shift_length, velocity, longitudinal_acc, total_time, offset_back);
  }

  std::pair<std::vector<double>, std::vector<double>> calc_base_lengths(
    const double arc_length, const double shift_length, const bool offset_back)
  {
    return path_shifter_.calc_base_lengths(arc_length, shift_length, offset_back);
  }
};

TEST_F(PathShifterTest, initial_state)
{
  ShiftedPath shifted_path;
  ShiftLineArray shift_lines;

  EXPECT_DOUBLE_EQ(path_shifter_.getLastShiftLength(), path_shifter_.getBaseOffset());
  EXPECT_FALSE(path_shifter_.getLastShiftLine().has_value());
  EXPECT_FALSE(path_shifter_.generate(&shifted_path));
}

TEST_F(PathShifterTest, get_base_lengths_without_accel_limit)
{
  double arc_length = 100.0;
  double shift_length = 20.0;
  double velocity = 10.0;
  double longitudinal_acc = 2.0;
  double total_time = 10.0;
  bool offset_back = false;

  // Condition: without offset_back
  auto result = get_base_lengths_without_accel_limit(arc_length, shift_length, offset_back);
  std::vector<double> expected_base_lon = {0.0, 25.0, 75.0, 100.0};
  std::vector<double> expected_base_lat = {20.0, 55.0 / 3.0, 5.0 / 3.0, 0.0};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_DOUBLE_EQ(result.first.at(i), expected_base_lon.at(i));
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }

  result = get_base_lengths_without_accel_limit(
    arc_length, shift_length, velocity, longitudinal_acc, total_time, offset_back);
  expected_base_lon = {0.0, 31.25, arc_length, arc_length};
  expected_base_lat = {shift_length, 11.0 / 12.0 * shift_length, shift_length / 12.0, 0.0};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_DOUBLE_EQ(result.first.at(i), expected_base_lon.at(i));
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }

  // Condition: with offset_back
  offset_back = true;
  result = get_base_lengths_without_accel_limit(arc_length, shift_length, offset_back);
  expected_base_lon = {0.0, 25.0, 75.0, 100.0};
  expected_base_lat = {0.0, 5.0 / 3.0, 55.0 / 3.0, 20.0};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_DOUBLE_EQ(result.first.at(i), expected_base_lon.at(i));
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }

  result = get_base_lengths_without_accel_limit(
    arc_length, shift_length, velocity, longitudinal_acc, total_time, offset_back);
  expected_base_lon = {0.0, 31.25, arc_length, arc_length};
  expected_base_lat = {0.0, shift_length / 12.0, 11.0 / 12.0 * shift_length, shift_length};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_DOUBLE_EQ(result.first.at(i), expected_base_lon.at(i));
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }
}

TEST_F(PathShifterTest, calc_base_lengths)
{
  double arc_length = 100.0;
  double shift_length = 20.0;
  bool offset_back = true;
  const double epsilon = 1e-06;

  // Condition: zero velocity and zero longitudinal acceleration
  path_shifter_.setVelocity(0.0);
  auto result = calc_base_lengths(arc_length, shift_length, offset_back);

  std::vector<double> expected_base_lon = {0.0, 25.0, 75.0, 100.0};
  std::vector<double> expected_base_lat = {0.0, 5.0 / 3.0, 55.0 / 3.0, 20.0};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_DOUBLE_EQ(result.first.at(i), expected_base_lon.at(i));
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }

  // Condition: zero acceleration
  path_shifter_.setVelocity(10.0);
  path_shifter_.setLongitudinalAcceleration(0.0);
  result = calc_base_lengths(arc_length, shift_length, offset_back);

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_DOUBLE_EQ(result.first.at(i), expected_base_lon.at(i));
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }

  // Condition: no shift
  shift_length = 0.0;
  path_shifter_.setLongitudinalAcceleration(2.0);
  path_shifter_.setLateralAccelerationLimit(1.0);
  result = calc_base_lengths(arc_length, shift_length, offset_back);

  expected_base_lon = {0.0, 17.838137, 67.838137, 100.0};
  expected_base_lat = {0.0, 0.0, 0.0, 0.0};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_NEAR(result.first.at(i), expected_base_lon.at(i), epsilon);
    EXPECT_DOUBLE_EQ(result.second.at(i), expected_base_lat.at(i));
  }

  // Condition: no limitation
  arc_length = 30.0;
  shift_length = 5.0;
  path_shifter_.setVelocity(6.0);
  path_shifter_.setLongitudinalAcceleration(1.0);
  path_shifter_.setLateralAccelerationLimit(2.0);
  result = calc_base_lengths(arc_length, shift_length, offset_back);

  expected_base_lon = {0.0, 3.6645406, 8.765561, 13.196938, 17.967602, 24.462500, 30.0};
  expected_base_lat = {0.0, 0.113095, 1.079422, 2.5, 3.920578, 4.886904, 5.0};

  ASSERT_EQ(result.first.size(), expected_base_lon.size());
  ASSERT_EQ(result.second.size(), expected_base_lat.size());

  for (size_t i = 0; i < result.first.size(); i++) {
    EXPECT_NEAR(result.first.at(i), expected_base_lon.at(i), epsilon);
    EXPECT_NEAR(result.second.at(i), expected_base_lat.at(i), epsilon);
  }
}

TEST_F(PathShifterTest, check_shift_lines_alignment)
{
  ShiftLineArray shift_lines;

  // Condition: empty shift_lines
  EXPECT_TRUE(check_shift_lines_alignment(shift_lines));

  // Condition: single shift line  aligned
  ShiftLine shift_line;
  shift_line.start_idx = 1;
  shift_line.end_idx = 10;
  shift_lines.push_back(shift_line);
  EXPECT_TRUE(check_shift_lines_alignment(shift_lines));

  // Condition: 2 shift line including inappropriate align
  shift_line.start_idx = 10;
  shift_line.end_idx = 4;
  shift_lines.push_back(shift_line);
  EXPECT_FALSE(check_shift_lines_alignment(shift_lines));
}

TEST_F(PathShifterTest, sort_shift_lines_along_path)
{
  ShiftLineArray shift_lines;
  size_t i = 0;

  // Condition: no shift line
  sort_shift_lines_along_path(shift_lines);
  EXPECT_TRUE(shift_lines.empty());

  // Condition: already sorted shift line
  ShiftLine shit_line;
  shit_line.start_idx = 0;
  shift_lines.push_back(shit_line);
  shit_line.start_idx = 5;
  shift_lines.push_back(shit_line);
  shit_line.start_idx = 10;
  shift_lines.push_back(shit_line);
  sort_shift_lines_along_path(shift_lines);
  for (const auto & shift_line : shift_lines) {
    EXPECT_EQ(shift_line.start_idx, i * 5);
    i++;
  }

  // Condition: unsorted shift line
  shift_lines.clear();
  shit_line.start_idx = 10;
  shift_lines.push_back(shit_line);
  shit_line.start_idx = 0;
  shift_lines.push_back(shit_line);
  shit_line.start_idx = 5;
  shift_lines.push_back(shit_line);
  sort_shift_lines_along_path(shift_lines);
  i = 0;
  for (const auto & shift_line : shift_lines) {
    EXPECT_EQ(shift_line.start_idx, i * 5);
    i++;
  }
}

TEST_F(PathShifterTest, add_lateral_offset_on_index_point)
{
  auto path = generate_shifted_path(3, 0.1, 0.1);
  size_t index = 1;

  // Condition: no offset
  double offset = 0.0;
  add_lateral_offset_on_index_point(&path, offset, index);
  EXPECT_DOUBLE_EQ(path.shift_length.at(index), 0.1);

  // Condition: with offset
  offset = 0.5;
  add_lateral_offset_on_index_point(&path, offset, index);
  EXPECT_DOUBLE_EQ(path.shift_length.at(index), 0.6);
}

TEST_F(PathShifterTest, shift_base_length)
{
  double lateral_interval = 0.1;
  auto path = generate_shifted_path(3, 0.1, lateral_interval);
  double offset = 0.1;

  shift_base_length(&path, offset);

  size_t i = 0;
  for (double shift_length : path.shift_length) {
    EXPECT_DOUBLE_EQ(shift_length, i * lateral_interval + offset);
    i++;
  }
}

TEST_F(PathShifterTest, generate)
{
  ShiftedPath shifted_path;

  // Condition: no reference path
  EXPECT_FALSE(path_shifter_.generate(&shifted_path));

  // Condition: no shift lines
  auto reference_path = autoware::test_utils::generateTrajectory<PathWithLaneId>(10, 1.0);
  path_shifter_.setPath(reference_path);
  EXPECT_TRUE(path_shifter_.generate(&shifted_path));

  // Condition: next index of start index would be end index
  std::vector<ShiftLine> lines;
  ShiftLine line;
  line.start.position = universe_utils::createPoint(0.0, 0.0, 0.0);
  line.end.position = universe_utils::createPoint(1.1, 0.0, 0.0);
  lines.push_back(line);
  path_shifter_.setShiftLines(lines);
  EXPECT_FALSE(path_shifter_.generate(&shifted_path));

  // Condition: apply spline shifter
  lines.front().start_shift_length = 0.0;
  lines.front().end.position.x = 8.0;
  lines.front().end_shift_length = 1.0;
  path_shifter_.setShiftLines(lines);
  EXPECT_TRUE(path_shifter_.generate(&shifted_path));

  ASSERT_EQ(shifted_path.shift_length.size(), 10);
  EXPECT_DOUBLE_EQ(shifted_path.shift_length.at(0), 0.02);
  EXPECT_DOUBLE_EQ(shifted_path.shift_length.at(8), 1.0);

  // Condition: apply linear
  EXPECT_TRUE(path_shifter_.generate(&shifted_path, true, SHIFT_TYPE::LINEAR));
  ASSERT_EQ(shifted_path.shift_length.size(), 10);
  EXPECT_DOUBLE_EQ(shifted_path.shift_length.at(0), 0.03);
  EXPECT_DOUBLE_EQ(shifted_path.shift_length.at(8), 1.0);
}

}  // namespace autoware::behavior_path_planner

// Copyright 2021 Tier IV, Inc.
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

#ifndef AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__PATH_SHIFTER__PATH_SHIFTER_HPP_
#define AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__PATH_SHIFTER__PATH_SHIFTER_HPP_

#include <autoware/universe_utils/ros/uuid_helper.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/logging.hpp>

#include <geometry_msgs/msg/point.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>
#include <unique_identifier_msgs/msg/uuid.hpp>

#include <optional>
#include <string>
#include <utility>
#include <vector>
namespace autoware::behavior_path_planner
{
using autoware::universe_utils::generateUUID;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;
using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;
using unique_identifier_msgs::msg::UUID;

struct ShiftLine
{
  ShiftLine() : id(generateUUID()) {}

  Pose start{};  // shift start point in absolute coordinate
  Pose end{};    // shift start point in absolute coordinate

  // relative shift length at the start point related to the reference path
  double start_shift_length{};

  // relative shift length at the end point related to the reference path
  double end_shift_length{};

  size_t start_idx{};  // associated start-point index for the reference path
  size_t end_idx{};    // associated end-point index for the reference path

  // for unique_id
  UUID id{};
};
using ShiftLineArray = std::vector<ShiftLine>;

struct ShiftedPath
{
  PathWithLaneId path{};
  std::vector<double> shift_length{};
};

enum class SHIFT_TYPE {
  LINEAR = 0,
  SPLINE = 1,
};

class PathShifter
{
public:
  // setter & getter

  /**
   * @brief  Set reference path.
   */
  void setPath(const PathWithLaneId & path);

  /**
   * @brief  Set velocity used to apply a lateral acceleration limit.
   */
  void setVelocity(const double velocity);

  /**
   * @brief  Set lateral acceleration limit
   */
  void setLateralAccelerationLimit(const double lateral_acc);

  /**
   * @brief  Set longitudinal acceleration
   */
  void setLongitudinalAcceleration(const double longitudinal_acc);

  /**
   * @brief  Add shift line. You don't have to care about the start/end_idx.
   */
  void addShiftLine(const ShiftLine & line);

  /**
   * @brief  Set new shift point. You don't have to care about the start/end_idx.
   */
  void setShiftLines(const std::vector<ShiftLine> & lines);

  /**
   * @brief  Get shift points.
   */
  std::vector<ShiftLine> getShiftLines() const { return shift_lines_; }

  /**
   * @brief  Get base offset.
   */
  double getBaseOffset() const { return base_offset_; }

  /**
   * @brief  Get reference path.
   */
  PathWithLaneId getReferencePath() const { return reference_path_; }

  /**
   * @brief  Generate a shifted path according to the given reference path and shift points.
   * @return False if the path is empty or shift points have conflicts.
   */
  bool generate(
    ShiftedPath * shifted_path, const bool offset_back = true,
    const SHIFT_TYPE type = SHIFT_TYPE::SPLINE) const;

  /**
   * @brief Remove behind shift points and add the removed offset to the base_offset_.
   * @details The previous offset information is stored in the base_offset_.
   *          This should be called after generate().
   */
  void removeBehindShiftLineAndSetBaseOffset(const size_t nearest_idx);

  double getLastShiftLength() const;

  std::optional<ShiftLine> getLastShiftLine() const;

private:
  // The reference path along which the shift will be performed.
  PathWithLaneId reference_path_;

  // Shift points used for shifted-path generation.
  ShiftLineArray shift_lines_;

  // The amount of shift length to the entire path.
  double base_offset_{0.0};

  // Used to apply a lateral acceleration limit
  double velocity_{0.0};

  // lateral acceleration limit considered in the path planning
  double lateral_acc_limit_{-1.0};

  double longitudinal_acc_{0.0};

  // Logger
  mutable rclcpp::Logger logger_{
    rclcpp::get_logger("behavior_path_planner").get_child("path_shifter")};

  // Clock
  mutable rclcpp::Clock clock_{RCL_ROS_TIME};

  /**
   * @brief Calculate basic points to generate shifted path.
   * @param arclength Longitudinal length in the Frenet coordinate.
   * @param shift_length Lateral length in the Frenet coordinate.
   * @param offset_back Whether to apply shifting after shift.
   * @return First is longitudinal points, and second is lateral points.
   */
  std::pair<std::vector<double>, std::vector<double>> calc_base_lengths(
    const double arclength, const double shift_length, const bool offset_back) const;

  /**
   * @brief Calculate basic points to generate shifted path without considering acceleration
   * limitation.
   * @param arclength Longitudinal length in the Frenet coordinate.
   * @param shift_length Lateral length in the Frenet coordinate.
   * @param offset_back Whether to apply shifting after shift.
   * @return First is longitudinal points, and second is lateral points.
   */
  static std::pair<std::vector<double>, std::vector<double>> get_base_lengths_without_accel_limit(
    const double arclength, const double shift_length, const bool offset_back);

  /**
   * @brief Calculate basic points to generate shifted path without considering acceleration
   * limitation.
   * @param arclength Longitudinal length in the Frenet coordinate.
   * @param shift_length Lateral length in the Frenet coordinate.
   * @param offset_back Whether to apply shifting after shift.
   * @return First is longitudinal points, and second is lateral points.
   */
  static std::pair<std::vector<double>, std::vector<double>> get_base_lengths_without_accel_limit(
    const double arclength, const double shift_length, const double velocity,
    const double longitudinal_acc, const double total_time, const bool offset_back);

  /**
   * @brief Calculate path index for shift_lines and set is_index_aligned_ to true.
   */
  void update_shift_lines_indices(ShiftLineArray & shift_lines) const;

  /**
   * @brief Sort the points in order from the front of the path.
   */
  void sort_shift_lines_along_path(ShiftLineArray & shift_lines) const;

  /**
   * @brief Generate shifted path from reference_path_ and shift_lines_ with linear shifting.
   */
  void apply_linear_shifter(ShiftedPath * shifted_path) const;

  /**
   * @brief Generate shifted path from reference_path_ and shift_lines_ with spline_based shifting.
   * @details Calculate the shift so that the horizontal jerk remains constant. This is achieved by
   *          dividing the shift interval into four parts and apply a cubic spline to them.
   *          The resultant shifting shape is closed to the Clothoid curve.
   */
  void apply_spline_shifter(ShiftedPath * shifted_path, const bool offset_back) const;

  ////////////////////////////////////////
  // Helper Functions
  ////////////////////////////////////////

  /**
   * @brief Check if the shift points are aligned in order and have no conflict range.
   */
  bool check_shift_lines_alignment(const ShiftLineArray & shift_lines) const;

  /**
   * @brief Add offset to specific point in shifted path.
   * @param path Shifted path.
   * @param offset Lateral offset.
   * @param index Target point index.
   */
  static void add_lateral_offset_on_index_point(ShiftedPath * path, double offset, size_t index);

  /**
   * @brief Add offset distance ti shifted path.
   * @param path Shifted path.
   * @param offset Lateral offset.
   */
  static void shift_base_length(ShiftedPath * path, double offset);

  void set_base_offset(const double val)
  {
    RCLCPP_DEBUG(logger_, "base_offset is changed: %f -> %f", base_offset_, val);
    base_offset_ = val;
  }

public:
  friend class PathShifterTest;
};

}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_PLANNER_COMMON__UTILS__PATH_SHIFTER__PATH_SHIFTER_HPP_

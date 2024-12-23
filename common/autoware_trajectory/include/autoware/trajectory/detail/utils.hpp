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

#ifndef AUTOWARE__TRAJECTORY__DETAIL__UTILS_HPP_
#define AUTOWARE__TRAJECTORY__DETAIL__UTILS_HPP_

#include "lanelet2_core/primitives/Point.h"

#include <Eigen/Core>

#include <autoware_planning_msgs/msg/path_point.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <tier4_planning_msgs/msg/path_point_with_lane_id.hpp>

#include <set>
#include <vector>

namespace autoware::trajectory::detail
{
/**
 * @brief Convert various point types to geometry_msgs::msg::Point.
 * @param p The input point to be converted.
 * @return geometry_msgs::msg::Point The converted point.
 */
geometry_msgs::msg::Point to_point(const geometry_msgs::msg::Point & p);
geometry_msgs::msg::Point to_point(const geometry_msgs::msg::Pose & p);
geometry_msgs::msg::Point to_point(const Eigen::Vector2d & p);
geometry_msgs::msg::Point to_point(const autoware_planning_msgs::msg::PathPoint & p);
geometry_msgs::msg::Point to_point(const tier4_planning_msgs::msg::PathPointWithLaneId & p);
geometry_msgs::msg::Point to_point(const lanelet::BasicPoint2d & p);
geometry_msgs::msg::Point to_point(const lanelet::ConstPoint3d & p);

/**
 * @brief Merge multiple vectors into one, keeping only unique elements.
 * @tparam Vectors Variadic template parameter for vector types.
 * @param vectors Vectors to be merged.
 * @return std::vector<double> Merged vector with unique elements.
 */
template <typename... Vectors>
std::vector<double> merge_vectors(const Vectors &... vectors)
{
  std::set<double> unique_elements;

  // Helper function to insert elements into the set
  auto insert_elements = [&unique_elements](const auto & vec) {
    unique_elements.insert(vec.begin(), vec.end());
  };

  // Expand the parameter pack and insert elements from each vector
  (insert_elements(vectors), ...);

  // Convert the set to std::vector<double>
  return {unique_elements.begin(), unique_elements.end()};
}

/**
 * @brief Ensures the vector has at least a specified number of points by linearly interpolating
 * values.
 *
 * @param x Input vector of double values.
 * @param min_points Minimum number of points required.
 * @return A vector with at least `min_points` elements.
 *
 * @note If `x.size() >= min_points`, the input vector is returned as-is.
 *
 * @code
 * std::vector<double> input = {1.0, 4.0, 6.0};
 * auto result = fill_bases(input, 6);
 * // result: {1.0, 2.0, 3.0, 4.0, 5.0, 6.0}
 * @endcode
 */
std::vector<double> fill_bases(const std::vector<double> & x, const size_t & min_points);

std::vector<double> crop_bases(
  const std::vector<double> & x, const double & start, const double & end);

}  // namespace autoware::trajectory::detail

#endif  // AUTOWARE__TRAJECTORY__DETAIL__UTILS_HPP_

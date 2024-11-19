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

#ifndef AUTOWARE__TRAJECTORY__POINT_HPP_
#define AUTOWARE__TRAJECTORY__POINT_HPP_

#include "autoware/trajectory/detail/utils.hpp"
#include "autoware/trajectory/interpolator/cubic_spline.hpp"
#include "autoware/trajectory/interpolator/interpolator.hpp"
#include "autoware/trajectory/interpolator/linear.hpp"

#include <Eigen/Dense>

#include <geometry_msgs/msg/point.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace autoware::trajectory
{

template <typename PointType>
class Trajectory;

/**
 * @brief Trajectory class for geometry_msgs::msg::Point
 */
template <>
class Trajectory<geometry_msgs::msg::Point>
{
  using PointType = geometry_msgs::msg::Point;

protected:
  std::shared_ptr<interpolator::InterpolatorInterface<double>>
    x_interpolator_;  //!< Interpolator for x
  std::shared_ptr<interpolator::InterpolatorInterface<double>>
    y_interpolator_;  //!< Interpolator for y
  std::shared_ptr<interpolator::InterpolatorInterface<double>>
    z_interpolator_;  //!< Interpolator for z

  std::vector<double> bases_;  //!< Axis of the trajectory

  double start_{0.0}, end_{0.0};  //!< Start and end of the arc length of the trajectory

  using ConstraintFunction = std::function<bool(const double & s)>;

  /**
   * @brief Validate the arc length is within the trajectory
   * @param s Arc length
   */
  [[nodiscard]] double clamp(const double & s, bool show_warning = false) const;

public:
  /**
   * @brief Get the length of the trajectory
   * @return Length of the trajectory
   */
  [[nodiscard]] double length() const;

  /**
   * @brief Compute the point on the trajectory at a given s value
   * @param s Arc length
   * @return Point on the trajectory
   */
  [[nodiscard]] PointType compute(double s) const;

  /**
   * @brief Build the trajectory from the points
   * @param points Vector of points
   * @return True if the build is successful
   */
  bool build(const std::vector<PointType> & points);

  /**
   * @brief Get the azimuth angle at a given s value
   * @param s Arc length
   * @return Azimuth in radians
   */
  [[nodiscard]] double azimuth(double s) const;

  /**
   * @brief Get the elevation angle at a given s value
   * @param s Arc length
   * @return Elevation in radians
   */
  [[nodiscard]] double elevation(double s) const;

  /**
   * @brief Get the curvature at a given s value
   * @param s Arc length
   * @return Curvature
   */
  [[nodiscard]] double curvature(double s) const;

  /**
   * @brief Find the closest point with constraint
   * @tparam InputPointType Type of input point
   * @param p Input point
   * @param constraints Constraint function
   * @return Optional arc length of the closest point
   */
  template <typename InputPointType>
  [[nodiscard]] std::optional<double> closest_with_constraint(
    const InputPointType & p, const ConstraintFunction & constraints) const
  {
    using trajectory::detail::to_point;
    Eigen::Vector2d point(to_point(p).x, to_point(p).y);
    std::vector<double> distances_from_segments;
    std::vector<double> lengthes_from_start_points;

    auto axis = detail::crop_bases(bases_, start_, end_);

    for (size_t i = 1; i < axis.size(); ++i) {
      Eigen::Vector2d p0;
      Eigen::Vector2d p1;
      p0 << x_interpolator_->compute(axis.at(i - 1)), y_interpolator_->compute(axis.at(i - 1));
      p1 << x_interpolator_->compute(axis.at(i)), y_interpolator_->compute(axis.at(i));
      Eigen::Vector2d v = p1 - p0;
      Eigen::Vector2d w = point - p0;
      double c1 = w.dot(v);
      double c2 = v.dot(v);
      double length_from_start_point = NAN;
      double distance_from_segment = NAN;
      if (c1 <= 0) {
        length_from_start_point = axis.at(i - 1);
        distance_from_segment = (point - p0).norm();
      } else if (c2 <= c1) {
        length_from_start_point = axis.at(i);
        distance_from_segment = (point - p1).norm();
      } else {
        length_from_start_point = axis.at(i - 1) + c1 / c2 * (p1 - p0).norm();
        distance_from_segment = (point - (p0 + (c1 / c2) * v)).norm();
      }
      if (constraints(length_from_start_point)) {
        distances_from_segments.push_back(distance_from_segment);
        lengthes_from_start_points.push_back(length_from_start_point);
      }
    }
    if (distances_from_segments.empty()) {
      return std::nullopt;
    }

    auto min_it = std::min_element(distances_from_segments.begin(), distances_from_segments.end());

    return lengthes_from_start_points[std::distance(distances_from_segments.begin(), min_it)] -
           start_;
  }

  /**
   * @brief Find the closest point
   * @tparam InputPointType Type of input point
   * @param p Input point
   * @return Arc length of the closest point
   */
  template <typename InputPointType>
  [[nodiscard]] double closest(const InputPointType & p) const
  {
    auto s = closest_with_constraint(p, [](const double &) { return true; });
    return *s;
  }
  /**
   * @brief Find the crossing point with constraint
   * @tparam InputPointType Type of input point
   * @param start Start point
   * @param end End point
   * @param constraints Constraint function
   * @return Optional arc length of the crossing point
   */
  template <typename InputPointType>
  [[nodiscard]] std::optional<double> crossed_with_constraint(
    const InputPointType & start, const InputPointType & end,
    const ConstraintFunction & constraints) const
  {
    using trajectory::detail::to_point;
    Eigen::Vector2d line_start(to_point(start).x, to_point(start).y);
    Eigen::Vector2d line_end(to_point(end).x, to_point(end).y);
    Eigen::Vector2d line_dir = line_end - line_start;

    auto axis = detail::crop_bases(bases_, start_, end_);

    for (size_t i = 1; i < axis.size(); ++i) {
      Eigen::Vector2d p0;
      Eigen::Vector2d p1;
      p0 << x_interpolator_->compute(axis.at(i - 1)), y_interpolator_->compute(axis.at(i - 1));
      p1 << x_interpolator_->compute(axis.at(i)), y_interpolator_->compute(axis.at(i));

      Eigen::Vector2d segment_dir = p1 - p0;

      double det = segment_dir.x() * line_dir.y() - segment_dir.y() * line_dir.x();

      if (std::abs(det) < 1e-10) {
        continue;
      }

      Eigen::Vector2d p0_to_line_start = line_start - p0;

      double t = (p0_to_line_start.x() * line_dir.y() - p0_to_line_start.y() * line_dir.x()) / det;
      double u =
        (p0_to_line_start.x() * segment_dir.y() - p0_to_line_start.y() * segment_dir.x()) / det;

      if (t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0) {
        double intersection_s = axis.at(i - 1) + t * (axis.at(i) - axis.at(i - 1));
        if (constraints(intersection_s)) {
          return intersection_s - start_;
        }
      }
    }

    return std::nullopt;
  }

  /**
   * @brief Find the crossing point
   * @tparam InputPointType Type of input point
   * @param start Start point
   * @param end End point
   * @return Optional arc length of the crossing point
   */
  template <typename InputPointType>
  [[nodiscard]] std::optional<double> crossed(
    const InputPointType & start, const InputPointType & end) const
  {
    return crossed_with_constraint(start, end, [](const double &) { return true; });
  }

  /**
   * @brief Restore the trajectory points
   * @param min_points Minimum number of points
   * @return Vector of points
   */
  [[nodiscard]] std::vector<PointType> restore(const size_t & min_points = 4) const;

  void crop(const double & start, const double & length);

  class Builder
  {
  private:
    std::unique_ptr<Trajectory> trajectory_;

  public:
    Builder()
    {
      trajectory_ = std::make_unique<Trajectory>();
      // Default interpolators
      set_xy_interpolator<interpolator::CubicSpline>();
      set_z_interpolator<interpolator::Linear>();
    }

    template <class InterpolatorType, class... Args>
    Builder & set_xy_interpolator(Args &&... args)
    {
      trajectory_->x_interpolator_ =
        std::make_shared<InterpolatorType>(std::forward<Args>(args)...);
      trajectory_->y_interpolator_ =
        std::make_shared<InterpolatorType>(std::forward<Args>(args)...);
      return *this;
    }

    template <class InterpolatorType, class... Args>
    Builder & set_z_interpolator(Args &&... args)
    {
      trajectory_->z_interpolator_ =
        std::make_shared<InterpolatorType>(std::forward<Args>(args)...);
      return *this;
    }

    std::optional<Trajectory> build(const std::vector<PointType> & points)
    {
      if (trajectory_->build(points)) {
        auto result = std::make_optional<Trajectory>(std::move(*trajectory_));
        trajectory_.reset();
        return result;
      }
      return std::nullopt;
    }
  };
};

}  // namespace autoware::trajectory

#endif  // AUTOWARE__TRAJECTORY__POINT_HPP_

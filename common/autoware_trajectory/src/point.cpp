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

#include "autoware/trajectory/point.hpp"

#include "autoware/trajectory/detail/utils.hpp"
#include "autoware/trajectory/interpolator/cubic_spline.hpp"
#include "autoware/trajectory/interpolator/linear.hpp"

#include <Eigen/Core>
#include <rclcpp/logging.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <vector>

namespace autoware::trajectory
{

using PointType = geometry_msgs::msg::Point;

Trajectory<PointType>::Trajectory()
: x_interpolator_(std::make_shared<interpolator::CubicSpline>()),
  y_interpolator_(std::make_shared<interpolator::CubicSpline>()),
  z_interpolator_(std::make_shared<interpolator::Linear>())
{
}

Trajectory<PointType>::Trajectory(const Trajectory & rhs)
{
  x_interpolator_ = rhs.x_interpolator_->clone();
  y_interpolator_ = rhs.y_interpolator_->clone();
  z_interpolator_ = rhs.z_interpolator_->clone();
  bases_ = rhs.bases_;
  start_ = rhs.start_;
  end_ = rhs.end_;
}

Trajectory<PointType> & Trajectory<PointType>::operator=(const Trajectory & rhs)
{
  if (this != &rhs) {
    x_interpolator_ = rhs.x_interpolator_->clone();
    y_interpolator_ = rhs.y_interpolator_->clone();
    z_interpolator_ = rhs.z_interpolator_->clone();
    bases_ = rhs.bases_;
    start_ = rhs.start_;
    end_ = rhs.end_;
  }
  return *this;
}

bool Trajectory<PointType>::build(const std::vector<PointType> & points)
{
  std::vector<double> xs;
  std::vector<double> ys;
  std::vector<double> zs;

  bases_.emplace_back(0.0);
  xs.emplace_back(points[0].x);
  ys.emplace_back(points[0].y);
  zs.emplace_back(points[0].z);

  for (size_t i = 1; i < points.size(); ++i) {
    Eigen::Vector2d p0(points[i - 1].x, points[i - 1].y);
    Eigen::Vector2d p1(points[i].x, points[i].y);
    bases_.emplace_back(bases_.back() + (p1 - p0).norm());
    xs.emplace_back(points[i].x);
    ys.emplace_back(points[i].y);
    zs.emplace_back(points[i].z);
  }

  start_ = bases_.front();
  end_ = bases_.back();

  bool is_valid = true;
  is_valid &= x_interpolator_->build(bases_, xs);
  is_valid &= y_interpolator_->build(bases_, ys);
  is_valid &= z_interpolator_->build(bases_, zs);

  return is_valid;
}

double Trajectory<PointType>::clamp(const double & s, bool show_warning) const
{
  if ((s < 0 || s > length()) && show_warning) {
    RCLCPP_WARN(
      rclcpp::get_logger("Trajectory"), "The arc length %f is out of the trajectory length %f", s,
      length());
  }
  return std::clamp(s, 0.0, length()) + start_;
}

std::vector<double> Trajectory<PointType>::get_internal_bases() const
{
  auto bases = detail::crop_bases(bases_, start_, end_);
  std::transform(
    bases.begin(), bases.end(), bases.begin(), [this](const double & s) { return s - start_; });
  return bases;
}

double Trajectory<PointType>::length() const
{
  return end_ - start_;
}

PointType Trajectory<PointType>::compute(double s) const
{
  s = clamp(s, true);
  PointType result;
  result.x = x_interpolator_->compute(s);
  result.y = y_interpolator_->compute(s);
  result.z = z_interpolator_->compute(s);
  return result;
}

double Trajectory<PointType>::azimuth(double s) const
{
  s = clamp(s, true);
  double dx = x_interpolator_->compute_first_derivative(s);
  double dy = y_interpolator_->compute_first_derivative(s);
  return std::atan2(dy, dx);
}

double Trajectory<PointType>::elevation(double s) const
{
  s = clamp(s, true);
  double dz = z_interpolator_->compute_first_derivative(s);
  return std::atan2(dz, 1.0);
}

double Trajectory<PointType>::curvature(double s) const
{
  s = clamp(s, true);
  double dx = x_interpolator_->compute_first_derivative(s);
  double ddx = x_interpolator_->compute_second_derivative(s);
  double dy = y_interpolator_->compute_first_derivative(s);
  double ddy = y_interpolator_->compute_second_derivative(s);
  return std::abs(dx * ddy - dy * ddx) / std::pow(dx * dx + dy * dy, 1.5);
}

std::vector<PointType> Trajectory<PointType>::restore(const size_t & min_points) const
{
  auto bases = get_internal_bases();
  bases = detail::fill_bases(bases, min_points);
  std::vector<PointType> points;
  points.reserve(bases.size());
  for (const auto & s : bases) {
    points.emplace_back(compute(s));
  }
  return points;
}

void Trajectory<PointType>::crop(const double & start, const double & length)
{
  start_ = std::clamp(start_ + start, start_, end_);
  end_ = std::clamp(start_ + length, start_, end_);
}

std::vector<std::pair<double, double>> Trajectory<PointType>::get_segments(
  const ConstraintFunction & constraints) const
{
  std::vector<std::pair<double, double>> segments;
  auto bases = get_internal_bases();
  bool segment_start = false;
  for (const double & s : bases) {
    if (constraints(s)) {
      if (!segment_start) {
        segments.emplace_back(s, s);
        segment_start = true;
      } else {
        segments.back().second = s;
      }
    } else {
      segment_start = false;
    }
  }
  return segments;
}

}  // namespace autoware::trajectory

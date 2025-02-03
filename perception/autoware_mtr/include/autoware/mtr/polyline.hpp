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

#ifndef AUTOWARE__MTR__POLYLINE_HPP_
#define AUTOWARE__MTR__POLYLINE_HPP_

#include <array>
#include <cmath>
#include <cstddef>
#include <tuple>
#include <vector>

namespace autoware::mtr
{
constexpr size_t PointStateDim = 7;

enum PolylineLabel { LANE = 0, ROAD_LINE = 1, ROAD_EDGE = 2, CROSSWALK = 3 };

struct LanePoint
{
  // Construct a new instance filling all elements by `0.0f`.
  LanePoint() : data_({0.0f}) {}

  /**
   * @brief Construct a new instance with specified values.
   *
   * @param x X position.
   * @param y Y position.
   * @param z Z position.
   * @param dx Normalized delta x.
   * @param dy Normalized delta y.
   * @param dz Normalized delta z.
   * @param label Label.
   */
  LanePoint(
    const float x, const float y, const float z, const float dx, const float dy, const float dz,
    const float label)
  : data_({x, y, z, dx, dy, dz, label}), x_(x), y_(y), z_(z), label_(label)
  {
  }

  // Construct a new instance filling all elements by `0.0f`.
  static LanePoint empty() noexcept { return LanePoint(); }

  // Return the point state dimensions `D`.
  static size_t dim() { return PointStateDim; }

  // Return the x position of the point.
  float x() const { return x_; }

  // Return the y position of the point.
  float y() const { return y_; }

  // Return the z position of the point.
  float z() const { return z_; }

  // Return the label of the point.
  float label() const { return label_; }

  /**
   * @brief Return the distance between myself and another one.
   *
   * @param other Another point.
   * @return float Distance between myself and another one.
   */
  float distance(const LanePoint & other) const
  {
    return std::hypot(x_ - other.x(), y_ - other.y(), z_ - other.z());
  }

  // Return the address pointer of data array.
  const float * data_ptr() const noexcept { return data_.data(); }

private:
  std::array<float, PointStateDim> data_;
  float x_{0.0f}, y_{0.0f}, z_{0.0f}, label_{0.0f};
};

struct PolylineData
{
  /**
   * @brief Construct a new PolylineData instance.
   *
   * @param points Source points vector.
   * @param min_num_polyline The minimum number of polylines should be generated. If the number of
   * polylines, resulting in separating input points, is less than this value, empty polylines will
   * be added.
   * @param max_num_point The maximum number of points that each polyline can include. If the
   * polyline contains fewer points than this value, empty points will be added.
   * @param distance_threshold The distance threshold to separate polylines.
   */
  PolylineData(
    const std::vector<LanePoint> & points, const size_t min_num_polyline,
    const size_t max_num_point, const float distance_threshold)
  : num_polyline_(0), num_point_(max_num_point), distance_threshold_(distance_threshold)
  {
    std::size_t point_cnt = 0;

    // point_cnt > PointNum at a to a new polyline group
    // distance > threshold -> add to a new polyline group
    for (std::size_t i = 0; i < points.size(); ++i) {
      auto & cur_point = points.at(i);

      if (i == 0) {
        addNewPolyline(cur_point, point_cnt);
        continue;
      }

      if (point_cnt >= num_point_) {
        addNewPolyline(cur_point, point_cnt);
      } else if (const auto & prev_point = points.at(i - 1);
                 cur_point.distance(prev_point) >= distance_threshold_ ||
                 cur_point.label() != prev_point.label()) {
        if (point_cnt < num_point_) {
          addEmptyPoints(point_cnt);
        }
        addNewPolyline(cur_point, point_cnt);
      } else {
        addPoint(cur_point, point_cnt);
      }
    }
    addEmptyPoints(point_cnt);

    if (num_polyline_ < min_num_polyline) {
      addEmptyPolyline(min_num_polyline - num_polyline_);
    }
  }

  // Return the number of polylines `K`.
  size_t num_polyline() const { return num_polyline_; }

  // Return the number of points contained in each polyline `P`.
  size_t num_point() const { return num_point_; }

  // Return the number of point dimensions `D`.
  static size_t state_dim() { return PointStateDim; }

  // Return the number of all elements `K*P*D`.
  size_t size() const { return num_polyline_ * num_point_ * state_dim(); }

  // Return the number of state dimensions of MTR input `D+2`.
  size_t input_dim() const { return state_dim() + 2; }

  // Return the data shape ordering in `(K, P, D)`.
  std::tuple<size_t, size_t, size_t> shape() const
  {
    return {num_polyline_, num_point_, state_dim()};
  }

  // Return the address pointer of data array.
  const float * data_ptr() const noexcept { return data_.data(); }

private:
  /**
   * @brief Add a new polyline group filled by empty points. This member function increments
   * `PolylineNum` by `num_polyline` internally.
   *
   * @param num_polyline The number of polylines to add.
   */
  void addEmptyPolyline(size_t num_polyline)
  {
    for (size_t i = 0; i < num_polyline; ++i) {
      size_t point_cnt = 0;
      auto empty_point = LanePoint::empty();
      addNewPolyline(empty_point, point_cnt);
      addEmptyPoints(point_cnt);
    }
  }

  /**
   * @brief Add a new polyline group with the specified point. This member function increments
   * `PolylineNum` by `1` internally.
   *
   * @param point LanePoint instance.
   * @param point_cnt The current count of points, which will be reset to `1`.
   */
  void addNewPolyline(const LanePoint & point, size_t & point_cnt)
  {
    const auto s = point.data_ptr();
    for (size_t d = 0; d < state_dim(); ++d) {
      data_.push_back(*(s + d));
    }
    ++num_polyline_;
    point_cnt = 1;
  }

  /**
   * @brief Add `(PointNum - point_cnt)` empty points filled by `0.0`.
   *
   * @param point_cnt The number of current count of points, which will be reset to `PointNum`.
   */
  void addEmptyPoints(size_t & point_cnt)
  {
    const auto s = LanePoint::empty().data_ptr();
    for (std::size_t n = point_cnt; n < num_point_; ++n) {
      for (std::size_t d = 0; d < state_dim(); ++d) {
        data_.push_back(*(s + d));
      }
    }
    point_cnt = num_point_;
  }

  /**
   * @brief Add the specified point and increment `point_cnt` by `1`.
   *
   * @param point
   * @param point_cnt
   */
  void addPoint(const LanePoint & point, std::size_t & point_cnt)
  {
    const auto s = point.data_ptr();
    for (size_t d = 0; d < state_dim(); ++d) {
      data_.push_back(*(s + d));
    }
    ++point_cnt;
  }

  size_t num_polyline_;
  size_t num_point_;
  std::vector<float> data_;
  const float distance_threshold_;
};
}  // namespace autoware::mtr
#endif  // AUTOWARE__MTR__POLYLINE_HPP_

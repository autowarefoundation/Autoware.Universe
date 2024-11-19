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

#ifndef AUTOWARE__TRAJECTORY__POSE_HPP_
#define AUTOWARE__TRAJECTORY__POSE_HPP_

#include "autoware/trajectory/interpolator/cubic_spline.hpp"
#include "autoware/trajectory/interpolator/interpolator.hpp"
#include "autoware/trajectory/interpolator/spherical_linear.hpp"
#include "autoware/trajectory/point.hpp"

#include <geometry_msgs/msg/quaternion.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace autoware::trajectory
{

/**
 * @brief Trajectory class for geometry_msgs::msg::Pose
 */
template <>
class Trajectory<geometry_msgs::msg::Pose> : public Trajectory<geometry_msgs::msg::Point>
{
  using BaseClass = Trajectory<geometry_msgs::msg::Point>;
  using PointType = geometry_msgs::msg::Pose;

protected:
  std::shared_ptr<interpolator::InterpolatorInterface<geometry_msgs::msg::Quaternion>>
    orientation_interpolator_;  //!< Interpolator for orientations

public:
  bool build(const std::vector<PointType> & points);

  /**
   * @brief Compute the pose on the trajectory at a given s value
   * @param s Arc length
   * @return Pose on the trajectory
   */
  [[nodiscard]] PointType compute(double s) const;

  /**
   * @brief Restore the trajectory poses
   * @return Vector of poses
   */
  [[nodiscard]] std::vector<PointType> restore(const size_t & min_points = 4) const;

  /**
   * @brief Align the orientation with the direction
   */
  void align_orientation_with_trajectory_direction();

  class Builder
  {
  private:
    std::unique_ptr<Trajectory> trajectory_;

  public:
    Builder()
    {
      trajectory_ = std::make_unique<Trajectory>();
      set_xy_interpolator<interpolator::CubicSpline>();
      set_z_interpolator<interpolator::Linear>();
      set_orientation_interpolator<interpolator::SphericalLinear>();
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

    template <class InterpolatorType, class... Args>
    Builder & set_orientation_interpolator(Args &&... args)
    {
      trajectory_->orientation_interpolator_ =
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

#endif  // AUTOWARE__TRAJECTORY__POSE_HPP_

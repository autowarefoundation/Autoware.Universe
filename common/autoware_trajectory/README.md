# Autoware Trajectory

This package provides classes to manage/manipulate Trajectory.

## Example Usage

This section describes Example Usage of `Trajectory<autoware_planning_msgs::msg::PathPoint>`

- Load Trajectory from point array

  ```cpp
  #include "autoware/trajectory/path_point.hpp"

  ...

  std::vector<autoware_planning_msgs::msg::PathPoint> points = ... // Load points from somewhere

  using autoware::trajectory::Trajectory;

  std::optional<Trajectory<autoware_planning_msgs::msg::PathPoint>> trajectory =
    Trajectory<autoware_planning_msgs::msg::PathPoint>::Builder{}
      .build(points);
  ```

- You can also specify interpolation method

  ```cpp
  using autoware::trajectory::interpolator::CubicSpline;

  std::optional<Trajectory<autoware_planning_msgs::msg::PathPoint>> trajectory =
    Trajectory<autoware_planning_msgs::msg::PathPoint>::Builder{}
      .set_xy_interpolator<CubicSpline>()  // Set interpolator for x-y plane
      .build(points);
  ```

- Access point on Trajectory

  ```cpp
  autoware_planning_msgs::msg::PathPoint point = trajectory->compute(1.0);  // Get point at s=0.0. s is distance from start point on Trajectory.
  ```

- Get length of Trajectory

  ```cpp
  double length = trajectory->length();
  ```

- Set 3.0[m] ~ 5.0[m] part of velocity to 0.0

  ```cpp
  trajectory->longitudinal_velocity_mps(3.0, 5.0) = 0.0;
  ```

- Crop Trajectory from 1.0[m] to 2.0[m]

  ```cpp
  trajectory->crop(1.0, 2.0);
  ```

- Restore points

  ```cpp
  std::vector<autoware_planning_msgs::msg::PathPoint> points = trajectory->restore();
  ```

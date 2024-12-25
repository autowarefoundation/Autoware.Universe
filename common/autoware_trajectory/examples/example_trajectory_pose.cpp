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

#include "autoware/trajectory/pose.hpp"

#include <geometry_msgs/msg/pose.hpp>

#include <matplotlibcpp17/pyplot.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Vector3.h>

#include <vector>

geometry_msgs::msg::Pose pose(double x, double y)
{
  geometry_msgs::msg::Pose p;
  p.position.x = x;
  p.position.y = y;
  p.position.z = 0.0;
  return p;
}

int main()
{
  pybind11::scoped_interpreter guard{};

  auto plt = matplotlibcpp17::pyplot::import();

  std::vector<geometry_msgs::msg::Pose> poses = {
    pose(0.49, 0.59), pose(0.61, 1.22), pose(0.86, 1.93), pose(1.20, 2.56), pose(1.51, 3.17),
    pose(1.85, 3.76), pose(2.14, 4.26), pose(2.60, 4.56), pose(3.07, 4.55), pose(3.61, 4.30),
    pose(3.95, 4.01), pose(4.29, 3.68), pose(4.90, 3.25), pose(5.54, 3.10), pose(6.24, 3.18),
    pose(6.88, 3.54), pose(7.51, 4.25), pose(7.85, 4.93), pose(8.03, 5.73), pose(8.16, 6.52),
    pose(8.31, 7.28), pose(8.45, 7.93), pose(8.68, 8.45), pose(8.96, 8.96), pose(9.32, 9.36)};

  using autoware::trajectory::Trajectory;
  using autoware::trajectory::interpolator::CubicSpline;

  auto trajectory = Trajectory<geometry_msgs::msg::Pose>::Builder{}.build(poses);

  trajectory->align_orientation_with_trajectory_direction();

  {
    std::vector<double> x;
    std::vector<double> y;

    for (double i = 0.0; i <= trajectory->length(); i += 0.01) {
      auto p = trajectory->compute(i);
      x.push_back(p.position.x);
      y.push_back(p.position.y);
    }

    plt.plot(Args(x, y), Kwargs("label"_a = "Trajectory", "color"_a = "blue"));
  }

  {
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> dx;
    std::vector<double> dy;

    for (double i = 0.0; i <= trajectory->length(); i += 1.0) {
      auto p = trajectory->compute(i);
      x.push_back(p.position.x);
      y.push_back(p.position.y);

      tf2::Vector3 x_axis(1.0, 0.0, 0.0);
      tf2::Quaternion q(p.orientation.x, p.orientation.y, p.orientation.z, p.orientation.w);
      tf2::Vector3 direction = tf2::quatRotate(q, x_axis);
      dx.push_back(direction.x());
      dy.push_back(direction.y());
      // double azimuth = trajectory->azimuth(i);
      // dx.push_back(std::cos(azimuth));
      // dy.push_back(std::sin(azimuth));
    }
    plt.quiver(Args(x, y, dx, dy), Kwargs("label"_a = "Direction", "color"_a = "green"));
  }

  plt.axis(Args("equal"));
  plt.legend();
  plt.show();
}

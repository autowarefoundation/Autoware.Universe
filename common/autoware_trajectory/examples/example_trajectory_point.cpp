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

#include "autoware/trajectory/interpolator/cubic_spline.hpp"
#include "autoware/trajectory/point.hpp"

#include <geometry_msgs/msg/point.hpp>

#include <matplotlibcpp17/pyplot.h>

#include <iostream>
#include <vector>

geometry_msgs::msg::Point pose(double x, double y)
{
  geometry_msgs::msg::Point p;
  p.x = x;
  p.y = y;
  return p;
}

int main()
{
  pybind11::scoped_interpreter guard{};

  auto plt = matplotlibcpp17::pyplot::import();

  std::vector<geometry_msgs::msg::Point> points = {
    pose(0.49, 0.59), pose(0.61, 1.22), pose(0.86, 1.93), pose(1.20, 2.56), pose(1.51, 3.17),
    pose(1.85, 3.76), pose(2.14, 4.26), pose(2.60, 4.56), pose(3.07, 4.55), pose(3.61, 4.30),
    pose(3.95, 4.01), pose(4.29, 3.68), pose(4.90, 3.25), pose(5.54, 3.10), pose(6.24, 3.18),
    pose(6.88, 3.54), pose(7.51, 4.25), pose(7.85, 4.93), pose(8.03, 5.73), pose(8.16, 6.52),
    pose(8.31, 7.28), pose(8.45, 7.93), pose(8.68, 8.45), pose(8.96, 8.96), pose(9.32, 9.36)};

  {
    std::vector<double> x;
    std::vector<double> y;

    for (const auto & p : points) {
      x.push_back(p.x);
      y.push_back(p.y);
    }

    plt.scatter(Args(x, y), Kwargs("label"_a = "Original", "color"_a = "red"));
  }

  using autoware::trajectory::Trajectory;
  using autoware::trajectory::interpolator::CubicSpline;

  auto trajectory = Trajectory<geometry_msgs::msg::Point>::Builder()
                      .set_xy_interpolator<CubicSpline>()
                      .set_z_interpolator<CubicSpline>()
                      .build(points);
  std::cout << "Trajectory length: " << trajectory->length() << std::endl;

  trajectory->crop(2.0, trajectory->length() - 4.0);

  std::cout << "Trajectory length after cropping: " << trajectory->length() << std::endl;

  {
    std::vector<double> x;
    std::vector<double> y;

    for (double i = 0.0; i <= trajectory->length(); i += 0.01) {
      auto p = trajectory->compute(i);
      x.push_back(p.x);
      y.push_back(p.y);
    }

    plt.plot(Args(x, y), Kwargs("label"_a = "Trajectory", "color"_a = "blue"));
  }

  {
    geometry_msgs::msg::Point p;
    p.x = 5.37;
    p.y = 6.0;

    auto s = trajectory->closest(p);
    auto closest = trajectory->compute(s);

    plt.scatter(Args(p.x, p.y), Kwargs("color"_a = "green"));
    plt.scatter(
      Args(closest.x, closest.y), Kwargs("label"_a = "Closest on trajectory", "color"_a = "green"));

    plt.plot(
      Args(std::vector<double>{p.x, closest.x}, std::vector<double>{p.y, closest.y}),
      Kwargs("color"_a = "green"));
  }
  {
    geometry_msgs::msg::Point p1;
    geometry_msgs::msg::Point p2;
    p1.x = 6.97;
    p1.y = 6.36;
    p2.x = 9.23;
    p2.y = 5.92;

    auto s = trajectory->crossed(p1, p2);
    auto crossed = trajectory->compute(s.value());

    plt.plot(
      Args(std::vector<double>{p1.x, p2.x}, std::vector<double>{p1.y, p2.y}),
      Kwargs("color"_a = "purple"));

    plt.scatter(
      Args(crossed.x, crossed.y),
      Kwargs("label"_a = "Crossed on trajectory", "color"_a = "purple"));
  }
  {
    auto restored = trajectory->restore(50);
    std::vector<double> x;
    std::vector<double> y;
    for (const auto & p : restored) {
      x.push_back(p.x);
      y.push_back(p.y);
    }
    plt.scatter(Args(x, y), Kwargs("label"_a = "Restored", "color"_a = "orange", "marker"_a = "x"));
  }

  plt.axis(Args("equal"));
  plt.legend();
  plt.show();
}

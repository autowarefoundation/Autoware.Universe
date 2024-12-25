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

#include "autoware/trajectory/interpolator/akima_spline.hpp"
#include "autoware/trajectory/interpolator/cubic_spline.hpp"
#include "autoware/trajectory/interpolator/interpolator.hpp"
#include "autoware/trajectory/interpolator/linear.hpp"
#include "autoware/trajectory/interpolator/nearest_neighbor.hpp"
#include "autoware/trajectory/interpolator/stairstep.hpp"

#include <matplotlibcpp17/pyplot.h>

#include <random>
#include <vector>

int main()
{
  pybind11::scoped_interpreter guard{};
  auto plt = matplotlibcpp17::pyplot::import();

  // create random values
  std::vector<double> bases = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
  std::vector<double> values;
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_real_distribution<> dist(-1.0, 1.0);
  for (size_t i = 0; i < bases.size(); ++i) {
    values.push_back(dist(engine));
  }
  // Scatter Data
  plt.scatter(Args(bases, values));

  using autoware::trajectory::interpolator::InterpolatorInterface;
  // Linear Interpolator
  {
    using autoware::trajectory::interpolator::Linear;
    auto interpolator = *Linear::Builder{}.set_bases(bases).set_values(values).build();
    std::vector<double> x;
    std::vector<double> y;
    for (double i = bases.front(); i < bases.back(); i += 0.01) {
      x.push_back(i);
      y.push_back(interpolator.compute(i));
    }
    plt.plot(Args(x, y), Kwargs("label"_a = "Linear"));
  }

  // AkimaSpline Interpolator
  {
    using autoware::trajectory::interpolator::AkimaSpline;

    auto interpolator = *AkimaSpline::Builder{}.set_bases(bases).set_values(values).build();
    std::vector<double> x;
    std::vector<double> y;
    for (double i = bases.front(); i < bases.back(); i += 0.01) {
      x.push_back(i);
      y.push_back(interpolator.compute(i));
    }
    plt.plot(Args(x, y), Kwargs("label"_a = "AkimaSpline"));
  }

  // CubicSpline Interpolator
  {
    using autoware::trajectory::interpolator::CubicSpline;
    auto interpolator = *CubicSpline::Builder{}.set_bases(bases).set_values(values).build();
    std::vector<double> x;
    std::vector<double> y;
    for (double i = bases.front(); i < bases.back(); i += 0.01) {
      x.push_back(i);
      y.push_back(interpolator.compute(i));
    }
    plt.plot(Args(x, y), Kwargs("label"_a = "CubicSpline"));
  }

  // NearestNeighbor Interpolator
  {
    using autoware::trajectory::interpolator::NearestNeighbor;
    auto interpolator =
      *NearestNeighbor<double>::Builder{}.set_bases(bases).set_values(values).build();
    std::vector<double> x;
    std::vector<double> y;
    for (double i = bases.front(); i < bases.back(); i += 0.01) {
      x.push_back(i);
      y.push_back(interpolator.compute(i));
    }
    plt.plot(Args(x, y), Kwargs("label"_a = "NearestNeighbor"));
  }

  // Stairstep Interpolator
  {
    using autoware::trajectory::interpolator::Stairstep;
    auto interpolator = *Stairstep<double>::Builder{}.set_bases(bases).set_values(values).build();
    std::vector<double> x;
    std::vector<double> y;
    for (double i = bases.front(); i < bases.back(); i += 0.01) {
      x.push_back(i);
      y.push_back(interpolator.compute(i));
    }
    plt.plot(Args(x, y), Kwargs("label"_a = "Stairstep"));
  }

  plt.legend();
  plt.show();
  return 0;
}

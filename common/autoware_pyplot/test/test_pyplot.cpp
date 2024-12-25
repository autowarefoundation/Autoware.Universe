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

#include <autoware/pyplot/patches.hpp>
#include <autoware/pyplot/pyplot.hpp>

#include <gtest/gtest.h>
#include <pybind11/embed.h>
/*
  very weirdly, you must include <pybind11/stl.h> to pass vector. Although the code
  compiles without it, the executable crashes at runtime
 */
#include <pybind11/stl.h>

#include <vector>

TEST(PyPlot, single_plot)
{
  // NOTE: somehow, running multiple tests simultaneously causes the python interpreter to crash
  py::scoped_interpreter guard{};
  auto plt = autoware::pyplot::import();
  {
    plt.plot(Args(std::vector<int>({1, 3, 2, 4})), Kwargs("color"_a = "blue", "linewidth"_a = 1.0));
    plt.xlabel(Args("x-title"));
    plt.ylabel(Args("y-title"));
    plt.title(Args("title"));
    plt.xlim(Args(0, 5));
    plt.ylim(Args(0, 5));
    plt.grid(Args(true));
    plt.savefig(Args("test_single_plot.png"));
  }
  {
    auto [fig, axes] = plt.subplots(1, 2);
    auto & ax1 = axes[0];
    auto & ax2 = axes[1];

    auto c =
      autoware::pyplot::Circle(Args(py::make_tuple(0, 0), 0.5), Kwargs("fc"_a = "g", "ec"_a = "r"));
    ax1.add_patch(Args(c.unwrap()));

    auto e = autoware::pyplot::Ellipse(
      Args(py::make_tuple(-0.25, 0), 0.5, 0.25), Kwargs("fc"_a = "b", "ec"_a = "y"));
    ax1.add_patch(Args(e.unwrap()));

    auto r = autoware::pyplot::Rectangle(
      Args(py::make_tuple(0, 0), 0.25, 0.5), Kwargs("ec"_a = "#000000", "fill"_a = false));
    ax2.add_patch(Args(r.unwrap()));

    ax1.set_aspect(Args("equal"));
    ax2.set_aspect(Args("equal"));
    plt.savefig(Args("test_double_plot.svg"));
  }
}

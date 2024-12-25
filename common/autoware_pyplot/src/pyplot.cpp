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

#include <autoware/pyplot/loader.hpp>
#include <autoware/pyplot/pyplot.hpp>

#include <tuple>
#include <vector>

namespace autoware::pyplot
{
PyPlot::PyPlot(const pybind11::module & mod_) : mod(mod_)
{
  load_attrs();
}

void PyPlot::load_attrs()
{
  LOAD_FUNC_ATTR(axes, mod);
  LOAD_FUNC_ATTR(cla, mod);
  LOAD_FUNC_ATTR(clf, mod);
  LOAD_FUNC_ATTR(figure, mod);
  LOAD_FUNC_ATTR(gca, mod);
  LOAD_FUNC_ATTR(gcf, mod);
  LOAD_FUNC_ATTR(gci, mod);
  LOAD_FUNC_ATTR(grid, mod);
  LOAD_FUNC_ATTR(imshow, mod);
  LOAD_FUNC_ATTR(legend, mod);
  LOAD_FUNC_ATTR(plot, mod);
  LOAD_FUNC_ATTR(quiver, mod);
  LOAD_FUNC_ATTR(savefig, mod);
  LOAD_FUNC_ATTR(scatter, mod);
  LOAD_FUNC_ATTR(show, mod);
  LOAD_FUNC_ATTR(subplot, mod);
  LOAD_FUNC_ATTR(subplots, mod);
  LOAD_FUNC_ATTR(title, mod);
  LOAD_FUNC_ATTR(xlabel, mod);
  LOAD_FUNC_ATTR(xlim, mod);
  LOAD_FUNC_ATTR(ylabel, mod);
  LOAD_FUNC_ATTR(ylim, mod);
}

axes::Axes PyPlot::axes(const pybind11::dict & kwargs)
{
  return axes::Axes{axes_attr(**kwargs)};
}

PyObjectWrapper PyPlot::cla(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{cla_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::clf(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{clf_attr(*args, **kwargs)};
}

figure::Figure PyPlot::figure(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return figure::Figure{figure_attr(*args, **kwargs)};
}

axes::Axes PyPlot::gca(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return axes::Axes{gca_attr(*args, **kwargs)};
}

figure::Figure PyPlot::gcf(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return figure::Figure{gcf_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::gci(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{gci_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::grid(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{grid_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::imshow(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{imshow_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::legend(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{legend_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::plot(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{plot_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::quiver(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{quiver_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::scatter(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{scatter_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::savefig(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{savefig_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::show(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{show_attr(*args, **kwargs)};
}

axes::Axes PyPlot::subplot(const pybind11::dict & kwargs)
{
  return axes::Axes(subplot_attr(**kwargs));
}

axes::Axes PyPlot::subplot(int cri)
{
  return axes::Axes(subplot_attr(cri));
}

std::tuple<figure::Figure, axes::Axes> PyPlot::subplots(const pybind11::dict & kwargs)
{
  pybind11::list ret = subplots_attr(**kwargs);
  pybind11::object fig = ret[0];
  pybind11::object ax = ret[1];
  return {figure::Figure(fig), axes::Axes(ax)};
}

std::tuple<figure::Figure, std::vector<axes::Axes>> PyPlot::subplots(
  int r, int c, const pybind11::dict & kwargs)
{
  // subplots() returns [][] (if r > 1 && c > 1) else []
  // return []axes in row-major
  // NOTE: equal to Axes.flat
  pybind11::tuple args = pybind11::make_tuple(r, c);
  pybind11::list ret = subplots_attr(*args, **kwargs);
  std::vector<axes::Axes> axes;
  pybind11::object fig = ret[0];
  figure::Figure figure(fig);
  if (r == 1 && c == 1) {
    // python returns Axes
    axes.emplace_back(ret[1]);
  } else if (r == 1 || c == 1) {
    // python returns []Axes
    pybind11::list axs = ret[1];
    for (int i = 0; i < r * c; ++i) axes.emplace_back(axs[i]);
  } else {
    // python returns [][]Axes
    pybind11::list axs = ret[1];
    for (pybind11::size_t i = 0; i < axs.size(); ++i) {
      pybind11::list ax_i = axs[i];
      for (unsigned j = 0; j < ax_i.size(); ++j) axes.emplace_back(ax_i[j]);
    }
  }
  return {figure, axes};
}

PyObjectWrapper PyPlot::title(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{title_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::xlabel(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{xlabel_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::xlim(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{xlim_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::ylabel(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{ylabel_attr(*args, **kwargs)};
}

PyObjectWrapper PyPlot::ylim(const pybind11::tuple & args, const pybind11::dict & kwargs)
{
  return PyObjectWrapper{ylim_attr(*args, **kwargs)};
}

PyPlot import()
{
  auto mod = pybind11::module::import("matplotlib.pyplot");
  auto g_pyplot = PyPlot(mod);
  return g_pyplot;
}

}  // namespace autoware::pyplot

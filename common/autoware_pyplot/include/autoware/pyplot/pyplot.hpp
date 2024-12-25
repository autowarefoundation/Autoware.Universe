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

#ifndef AUTOWARE__PYPLOT__PYPLOT_HPP_
#define AUTOWARE__PYPLOT__PYPLOT_HPP_

#include <autoware/pyplot/axes.hpp>
#include <autoware/pyplot/common.hpp>
#include <autoware/pyplot/figure.hpp>

#include <tuple>
#include <utility>
#include <vector>

namespace autoware::pyplot
{
struct DECL_VISIBILITY PyPlot
{
public:
  explicit PyPlot(const pybind11::module & mod_);

  axes::Axes axes(const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper axis(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper cla(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper clf(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  figure::Figure figure(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  axes::Axes gca(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  figure::Figure gcf(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper gci(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper grid(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper imshow(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper legend(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper plot(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper quiver(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper savefig(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper scatter(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper show(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  axes::Axes subplot(const pybind11::dict & kwargs = pybind11::dict());
  axes::Axes subplot(int cri);

  std::tuple<figure::Figure, axes::Axes> subplots(const pybind11::dict & kwargs = pybind11::dict());
  std::tuple<figure::Figure, std::vector<axes::Axes>> subplots(
    int r, int c, const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper title(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper xlabel(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper xlim(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper ylabel(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

  PyObjectWrapper ylim(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());

private:
  void load_attrs();
  pybind11::module mod;
  pybind11::object axes_attr;
  pybind11::object cla_attr;
  pybind11::object clf_attr;
  pybind11::object figure_attr;
  pybind11::object gca_attr;
  pybind11::object gcf_attr;
  pybind11::object gci_attr;
  pybind11::object grid_attr;
  pybind11::object imshow_attr;
  pybind11::object legend_attr;
  pybind11::object plot_attr;
  pybind11::object quiver_attr;
  pybind11::object savefig_attr;
  pybind11::object scatter_attr;
  pybind11::object show_attr;
  pybind11::object subplot_attr;
  pybind11::object subplots_attr;
  pybind11::object title_attr;
  pybind11::object xlabel_attr;
  pybind11::object xlim_attr;
  pybind11::object ylabel_attr;
  pybind11::object ylim_attr;
};

PyPlot import();

}  // namespace autoware::pyplot

template <typename... ArgsT>
pybind11::tuple Args(ArgsT &&... args)
{
  return pybind11::make_tuple(std::forward<ArgsT>(args)...);
}

using Kwargs = pybind11::dict;

namespace py = pybind11;
using namespace py::literals;

#endif  // AUTOWARE__PYPLOT__PYPLOT_HPP_

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

#include <autoware/pyplot/axes.hpp>
#include <autoware/pyplot/loader.hpp>

#include <tuple>

namespace autoware::pyplot
{
inline namespace axes
{
Axes::Axes(const pybind11::object & object) : PyObjectWrapper(object)
{
  load_attrs();
}
Axes::Axes(pybind11::object && object) : PyObjectWrapper(object)
{
  load_attrs();
}

PyObjectWrapper Axes::add_patch(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{add_patch_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::cla(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{cla_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::contour(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{contour_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::fill(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{fill_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::fill_between(
  const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{fill_between_attr(*args, **kwargs)};
}

std::tuple<double, double> Axes::get_xlim() const
{
  const pybind11::list ret = get_xlim_attr();
  return {ret[0].cast<double>(), ret[1].cast<double>()};
}

std::tuple<double, double> Axes::get_ylim() const
{
  const pybind11::list ret = get_ylim_attr();
  return {ret[0].cast<double>(), ret[1].cast<double>()};
}

PyObjectWrapper Axes::grid(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{grid_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::imshow(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{imshow_attr(*args, **kwargs)};
}

legend::Legend Axes::legend(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return legend::Legend{legend_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::plot(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{plot_attr(*args, **kwargs)};
}

quiver::Quiver Axes::quiver(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return Quiver{quiver_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::set_aspect(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{set_aspect_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::set_title(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{set_title_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::set_xlabel(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{set_xlabel_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::set_xlim(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{set_xlim_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::set_ylabel(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{set_ylabel_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::set_ylim(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{set_ylim_attr(*args, **kwargs)};
}

PyObjectWrapper Axes::text(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{text_attr(*args, **kwargs)};
}

void Axes::load_attrs()
{
  LOAD_FUNC_ATTR(add_patch, self_);
  LOAD_FUNC_ATTR(cla, self_);
  LOAD_FUNC_ATTR(contour, self_);
  LOAD_FUNC_ATTR(contourf, self_);
  LOAD_FUNC_ATTR(fill, self_);
  LOAD_FUNC_ATTR(fill_between, self_);
  LOAD_FUNC_ATTR(get_xlim, self_);
  LOAD_FUNC_ATTR(get_ylim, self_);
  LOAD_FUNC_ATTR(grid, self_);
  LOAD_FUNC_ATTR(imshow, self_);
  LOAD_FUNC_ATTR(legend, self_);
  LOAD_FUNC_ATTR(quiver, self_);
  LOAD_FUNC_ATTR(plot, self_);
  LOAD_FUNC_ATTR(scatter, self_);
  LOAD_FUNC_ATTR(set_aspect, self_);
  LOAD_FUNC_ATTR(set_title, self_);
  LOAD_FUNC_ATTR(set_xlabel, self_);
  LOAD_FUNC_ATTR(set_xlim, self_);
  LOAD_FUNC_ATTR(set_ylabel, self_);
  LOAD_FUNC_ATTR(set_ylim, self_);
  LOAD_FUNC_ATTR(text, self_);
}
}  // namespace axes
}  // namespace autoware::pyplot

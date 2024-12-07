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

#include <autoware/pyplot/figure.hpp>
#include <autoware/pyplot/loader.hpp>

namespace autoware::pyplot
{
inline namespace figure
{
Figure::Figure(const pybind11::object & object) : PyObjectWrapper(object)
{
  load_attrs();
}
Figure::Figure(pybind11::object && object) : PyObjectWrapper(object)
{
  load_attrs();
}

void Figure::load_attrs()
{
  LOAD_FUNC_ATTR(add_axes, self_);
  LOAD_FUNC_ATTR(add_subplot, self_);
  LOAD_FUNC_ATTR(colorbar, self_);
  LOAD_FUNC_ATTR(savefig, self_);
  LOAD_FUNC_ATTR(tight_layout, self_);
}

axes::Axes Figure::add_axes(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return axes::Axes{add_axes_attr(*args, **kwargs)};
}

axes::Axes Figure::add_subplot(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return axes::Axes{add_subplot_attr(*args, **kwargs)};
}

PyObjectWrapper Figure::colorbar(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{colorbar_attr(*args, **kwargs)};
}

PyObjectWrapper Figure::savefig(const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{savefig_attr(*args, **kwargs)};
}

PyObjectWrapper Figure::tight_layout(
  const pybind11::tuple & args, const pybind11::dict & kwargs) const
{
  return PyObjectWrapper{tight_layout_attr(*args, **kwargs)};
}
}  // namespace figure
}  // namespace autoware::pyplot

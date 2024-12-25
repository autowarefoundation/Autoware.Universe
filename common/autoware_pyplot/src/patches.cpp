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

namespace autoware::pyplot
{
inline namespace patches
{
Circle::Circle(const pybind11::tuple & args, const pybind11::dict & kwargs)
: PyObjectWrapper(pybind11::module::import("matplotlib.patches").attr("Circle"))
{
  self_ = self_(*args, **kwargs);
}

Ellipse::Ellipse(const pybind11::tuple & args, const pybind11::dict & kwargs)
: PyObjectWrapper(pybind11::module::import("matplotlib.patches").attr("Ellipse"))
{
  self_ = self_(*args, **kwargs);
}

Rectangle::Rectangle(const pybind11::tuple & args, const pybind11::dict & kwargs)
: PyObjectWrapper(pybind11::module::import("matplotlib.patches").attr("Rectangle"))
{
  self_ = self_(*args, **kwargs);
}

Polygon::Polygon(const pybind11::tuple & args, const pybind11::dict & kwargs)
: PyObjectWrapper(pybind11::module::import("matplotlib.patches").attr("Polygon"))
{
  self_ = self_(*args, **kwargs);
}
}  // namespace patches
}  // namespace autoware::pyplot

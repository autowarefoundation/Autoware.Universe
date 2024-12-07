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

#ifndef AUTOWARE__PYPLOT__FIGURE_HPP_
#define AUTOWARE__PYPLOT__FIGURE_HPP_

#include <autoware/pyplot/axes.hpp>
#include <autoware/pyplot/common.hpp>

namespace autoware::pyplot
{
inline namespace figure
{
class DECL_VISIBILITY Figure : public PyObjectWrapper
{
public:
  explicit Figure(const pybind11::object & object);
  explicit Figure(pybind11::object && object);

  axes::Axes add_axes(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  axes::Axes add_subplot(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper colorbar(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper savefig(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper tight_layout(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

private:
  void load_attrs();

  pybind11::object add_axes_attr;
  pybind11::object add_subplot_attr;
  pybind11::object colorbar_attr;
  pybind11::object savefig_attr;
  pybind11::object tight_layout_attr;
};
}  // namespace figure
}  // namespace autoware::pyplot
#endif  // AUTOWARE__PYPLOT__FIGURE_HPP_

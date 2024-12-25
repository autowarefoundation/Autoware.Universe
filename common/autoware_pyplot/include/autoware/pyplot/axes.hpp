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

#ifndef AUTOWARE__PYPLOT__AXES_HPP_
#define AUTOWARE__PYPLOT__AXES_HPP_

#include <autoware/pyplot/common.hpp>
#include <autoware/pyplot/legend.hpp>
#include <autoware/pyplot/quiver.hpp>

#include <tuple>

namespace autoware::pyplot
{
inline namespace axes
{
class DECL_VISIBILITY Axes : public PyObjectWrapper
{
public:
  explicit Axes(const pybind11::object & object);
  explicit Axes(pybind11::object && object);

  PyObjectWrapper add_patch(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper bar_label(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper cla(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper contour(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper errorbar(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper fill(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper fill_between(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  std::tuple<double, double> get_xlim() const;

  std::tuple<double, double> get_ylim() const;

  PyObjectWrapper grid(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper imshow(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  legend::Legend legend(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper plot(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  quiver::Quiver quiver(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper set_aspect(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper set_title(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper set_xlabel(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper set_xlim(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper set_ylabel(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper set_ylim(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

  PyObjectWrapper text(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

private:
  void load_attrs();

  pybind11::object add_patch_attr;
  pybind11::object cla_attr;
  pybind11::object contour_attr;
  pybind11::object contourf_attr;
  pybind11::object fill_attr;
  pybind11::object fill_between_attr;
  pybind11::object get_xlim_attr;
  pybind11::object get_ylim_attr;
  pybind11::object grid_attr;
  pybind11::object imshow_attr;
  pybind11::object legend_attr;
  pybind11::object quiver_attr;
  pybind11::object plot_attr;
  pybind11::object scatter_attr;
  pybind11::object set_aspect_attr;
  pybind11::object set_title_attr;
  pybind11::object set_xlabel_attr;
  pybind11::object set_xlim_attr;
  pybind11::object set_ylabel_attr;
  pybind11::object set_ylim_attr;
  pybind11::object text_attr;
};
}  // namespace axes
}  // namespace autoware::pyplot
#endif  // AUTOWARE__PYPLOT__AXES_HPP_

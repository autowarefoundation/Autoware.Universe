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

#ifndef AUTOWARE__PYPLOT__PATCHES_HPP_
#define AUTOWARE__PYPLOT__PATCHES_HPP_

#include <autoware/pyplot/common.hpp>

namespace autoware::pyplot
{
inline namespace patches
{
class DECL_VISIBILITY Circle : public PyObjectWrapper
{
public:
  explicit Circle(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());
};

class DECL_VISIBILITY Ellipse : public PyObjectWrapper
{
public:
  explicit Ellipse(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());
};

class DECL_VISIBILITY Rectangle : public PyObjectWrapper
{
public:
  explicit Rectangle(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());
};

class DECL_VISIBILITY Polygon : public PyObjectWrapper
{
public:
  explicit Polygon(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict());
};
}  // namespace patches
}  // namespace autoware::pyplot
#endif  // AUTOWARE__PYPLOT__PATCHES_HPP_

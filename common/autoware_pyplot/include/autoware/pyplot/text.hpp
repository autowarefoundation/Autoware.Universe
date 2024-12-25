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

#ifndef AUTOWARE__PYPLOT__TEXT_HPP_
#define AUTOWARE__PYPLOT__TEXT_HPP_

#include <autoware/pyplot/common.hpp>

namespace autoware::pyplot
{
inline namespace text
{
class DECL_VISIBILITY Text : public PyObjectWrapper
{
public:
  explicit Text(const pybind11::object & object);
  explicit Text(pybind11::object && object);

  PyObjectWrapper set_rotation(
    const pybind11::tuple & args = pybind11::tuple(),
    const pybind11::dict & kwargs = pybind11::dict()) const;

private:
  void load_attrs();
  pybind11::object set_rotation_attr;
};
}  // namespace text
}  // namespace autoware::pyplot
#endif  // AUTOWARE__PYPLOT__TEXT_HPP_

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

#include <autoware/pyplot/common.hpp>

#include <utility>

namespace autoware::pyplot
{
inline namespace common
{
PyObjectWrapper::PyObjectWrapper(const pybind11::object & object)
{
  self_ = object;
}
PyObjectWrapper::PyObjectWrapper(pybind11::object && object)
{
  self_ = std::move(object);
}
}  // namespace common
}  // namespace autoware::pyplot

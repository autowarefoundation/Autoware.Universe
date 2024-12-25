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

#ifndef AUTOWARE__PYPLOT__COMMON_HPP_
#define AUTOWARE__PYPLOT__COMMON_HPP_

#include <pybind11/pybind11.h>

namespace autoware::pyplot
{

#ifdef _WIN32
#define DECL_VISIBILITY __declspec(dllexport)
#else
#define DECL_VISIBILITY __attribute__((visibility("hidden")))
#endif

inline namespace common
{
class DECL_VISIBILITY PyObjectWrapper
{
public:
  explicit PyObjectWrapper(const pybind11::object & object);
  explicit PyObjectWrapper(pybind11::object && object);
  pybind11::object unwrap() const { return self_; }

protected:
  pybind11::object self_;
};
}  // namespace common
}  // namespace autoware::pyplot

#endif  // AUTOWARE__PYPLOT__COMMON_HPP_

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

#ifndef AUTOWARE__PYPLOT__QUIVER_HPP_
#define AUTOWARE__PYPLOT__QUIVER_HPP_

#include <autoware/pyplot/common.hpp>

namespace autoware::pyplot
{
inline namespace quiver
{
class DECL_VISIBILITY Quiver : public PyObjectWrapper
{
public:
  explicit Quiver(const pybind11::object & object);
  explicit Quiver(pybind11::object && object);
};
}  // namespace quiver
}  // namespace autoware::pyplot
#endif  // AUTOWARE__PYPLOT__QUIVER_HPP_

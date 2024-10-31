// Copyright 2019-2021 Apex.AI, Inc.
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
//
// Co-developed by Tier IV, Inc. and Apex.AI, Inc.
#ifndef AUTOWARE__TIME_UTILS__TIME_UTILS_HPP_
#define AUTOWARE__TIME_UTILS__TIME_UTILS_HPP_

#include <autoware/time_utils/visibility_control.hpp>
#include <builtin_interfaces/msg/duration.hpp>
#include <builtin_interfaces/msg/time.hpp>

#include <chrono>

namespace autoware::time_utils
{
/// Standard interpolation
TIME_UTILS_PUBLIC std::chrono::nanoseconds interpolate(
  std::chrono::nanoseconds a, std::chrono::nanoseconds b, float t) noexcept;
}  // namespace autoware::time_utils

#endif  // AUTOWARE__TIME_UTILS__TIME_UTILS_HPP_

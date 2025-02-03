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

#include "autoware/mtr/intention_point.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace autoware::mtr
{
std::vector<float> IntentionPoint::as_array(const std::vector<std::string> & label_names) const
{
  std::vector<float> points;
  points.reserve(label_names.size() * num_cluster * state_dim());
  for (const auto & name : label_names) {
    const auto & label_points = data_map_.at(name);
    for (const auto & p : label_points) {
      points.emplace_back(p);
    }
  }
  return points;
}

size_t IntentionPoint::size() const noexcept
{
  return num_cluster * state_dim();
}
}  // namespace autoware::mtr

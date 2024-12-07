// Copyright 2022 TIER IV, Inc.
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

#include <autoware/motion_utils/factor/steering_factor_interface.hpp>

#include <string>

namespace autoware::motion_utils
{
void SteeringFactorInterface::set(
  const std::array<Pose, 2> & pose, const std::array<double, 2> distance, const uint16_t direction,
  const uint16_t status, const std::string & detail)
{
  steering_factor_.pose = pose;
  std::array<float, 2> converted_distance{0.0, 0.0};
  for (int i = 0; i < 2; ++i) converted_distance[i] = static_cast<float>(distance[i]);
  steering_factor_.distance = converted_distance;
  steering_factor_.behavior = behavior_;
  steering_factor_.direction = direction;
  steering_factor_.status = status;
  steering_factor_.detail = detail;
}
}  // namespace autoware::motion_utils

// Copyright 2024 The Autoware Foundation
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

#ifndef AUTOWARE__TRAJECTORY_FOLLOWER_BASE__CONTROL_HORIZON_HPP_
#define AUTOWARE__TRAJECTORY_FOLLOWER_BASE__CONTROL_HORIZON_HPP_

#include "autoware_control_msgs/msg/lateral.hpp"
#include "autoware_control_msgs/msg/longitudinal.hpp"

#include <vector>

namespace autoware::motion::control::trajectory_follower
{

using autoware_control_msgs::msg::Lateral;
using autoware_control_msgs::msg::Longitudinal;

struct LateralHorizon
{
  double time_step_ms;
  std::vector<Lateral> controls;
};

struct LongitudinalHorizon
{
  double time_step_ms;
  std::vector<Longitudinal> controls;
};

}  // namespace autoware::motion::control::trajectory_follower
#endif  // AUTOWARE__TRAJECTORY_FOLLOWER_BASE__CONTROL_HORIZON_HPP_

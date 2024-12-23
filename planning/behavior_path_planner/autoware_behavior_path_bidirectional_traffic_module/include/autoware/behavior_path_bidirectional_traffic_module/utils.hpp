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

#ifndef AUTOWARE__BEHAVIOR_PATH_BIDIRECTIONAL_TRAFFIC_MODULE__UTILS_HPP_
#define AUTOWARE__BEHAVIOR_PATH_BIDIRECTIONAL_TRAFFIC_MODULE__UTILS_HPP_

#include <lanelet2_core/Forward.h>

#include <vector>

namespace autoware::behavior_path_planner
{

using BidirectionalLanePair = std::pair<lanelet::ConstLanelet, lanelet::ConstLanelet>;
using BidirectionalLanes = std::vector<BidirectionalLanePair>;

bool is_bidirectional_traffic(
  const lanelet::ConstLanelet & lanelet_a, const lanelet::ConstLanelet & lanelet_b);

BidirectionalLanes search_bidirectional_lane_from_map(const lanelet::LaneletMap & map);

}  // namespace autoware::behavior_path_planner

#endif  // AUTOWARE__BEHAVIOR_PATH_BIDIRECTIONAL_TRAFFIC_MODULE__UTILS_HPP_

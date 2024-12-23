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

#include "autoware/behavior_path_bidirectional_traffic_module/utils.hpp"

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/Lanelet.h>

namespace autoware::behavior_path_planner
{

bool is_bidirectional_traffic(
  const lanelet::ConstLanelet & lanelet_a, const lanelet::ConstLanelet & lanelet_b)
{
  return (lanelet_a.leftBound().id() == lanelet_b.rightBound().id()) &&
         (lanelet_a.rightBound().id() == lanelet_b.leftBound().id());
}

BidirectionalLanes search_bidirectional_lane_from_map(const lanelet::LaneletMap & map)
{
  BidirectionalLanes bidirectional_traffics;

  // Lambda function to check and add bidirectional lane pairs
  auto add_bidirectional_traffic_pair = [&bidirectional_traffics](
                                          const lanelet::ConstLanelet & lanelet_a,
                                          const lanelet::ConstLanelet & lanelet_b) {
    if (is_bidirectional_traffic(lanelet_a, lanelet_b)) {
      bidirectional_traffics.emplace_back(lanelet_a, lanelet_b);
    }
  };

  // Iterate over all lanelet pairs without duplication
  for (auto it_a = map.laneletLayer.begin(); it_a != map.laneletLayer.end(); ++it_a) {
    std::for_each(
      std::next(it_a), map.laneletLayer.end(), [&](const lanelet::ConstLanelet & lanelet_b) {
        add_bidirectional_traffic_pair(*it_a, lanelet_b);
      });
  }
  return bidirectional_traffics;
}

}  // namespace autoware::behavior_path_planner

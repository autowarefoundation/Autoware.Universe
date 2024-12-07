// Copyright 2020 Tier IV, Inc.
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

#include "autoware/motion_utils/marker/virtual_wall_marker_creator.hpp"
#include "autoware/universe_utils/geometry/geometry.hpp"
#include "scene.hpp"

namespace autoware::behavior_velocity_planner
{

autoware::motion_utils::VirtualWalls StopLineModule::createVirtualWalls()
{
  autoware::motion_utils::VirtualWalls virtual_walls;

  if (debug_data_.stop_pose && (state_ == State::APPROACH || state_ == State::STOPPED)) {
    autoware::motion_utils::VirtualWall wall;
    wall.text = "stopline";
    wall.style = autoware::motion_utils::VirtualWallType::stop;
    wall.ns = std::to_string(module_id_) + "_";
    wall.pose = autoware::universe_utils::calcOffsetPose(
      *debug_data_.stop_pose, debug_data_.base_link2front, 0.0, 0.0);
    virtual_walls.push_back(wall);
  }
  return virtual_walls;
}
}  // namespace autoware::behavior_velocity_planner

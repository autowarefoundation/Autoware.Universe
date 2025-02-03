// Copyright 2021 Tier IV, Inc.
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

#ifndef AUTOWARE__PROBABILISTIC_OCCUPANCY_GRID_MAP__UPDATER__OGM_UPDATER_INTERFACE_HPP_
#define AUTOWARE__PROBABILISTIC_OCCUPANCY_GRID_MAP__UPDATER__OGM_UPDATER_INTERFACE_HPP_

#include "autoware/probabilistic_occupancy_grid_map/cost_value/cost_value.hpp"
#include "autoware/probabilistic_occupancy_grid_map/costmap_2d/occupancy_grid_map_base.hpp"

#ifdef USE_CUDA
#include "autoware/probabilistic_occupancy_grid_map/utils/utils_kernel.hpp"
#endif

#include <rclcpp/node.hpp>

namespace autoware::occupancy_grid_map
{
namespace costmap_2d
{
class OccupancyGridMapUpdaterInterface : public OccupancyGridMapInterface
{
public:
  OccupancyGridMapUpdaterInterface(
    bool use_cuda, const unsigned int cells_size_x, const unsigned int cells_size_y,
    const float resolution)
  : OccupancyGridMapInterface(use_cuda, cells_size_x, cells_size_y, resolution)
  {
  }
  virtual ~OccupancyGridMapUpdaterInterface() = default;
  virtual bool update(const OccupancyGridMapInterface & single_frame_occupancy_grid_map) = 0;
  virtual void initRosParam(rclcpp::Node & node) = 0;
};

}  // namespace costmap_2d
}  // namespace autoware::occupancy_grid_map

#endif  // AUTOWARE__PROBABILISTIC_OCCUPANCY_GRID_MAP__UPDATER__OGM_UPDATER_INTERFACE_HPP_

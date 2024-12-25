// Copyright 2020-2024 Tier IV, Inc.
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

/*
 * Copyright 2018-2019 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************
 *
 */

#include "autoware/costmap_generator/utils/object_map_utils.hpp"

#include <autoware/grid_map_utils/polygon_iterator.hpp>
#include <grid_map_core/Polygon.hpp>

#include <tf2/time.h>

#include <string>
#include <vector>

namespace autoware::costmap_generator::object_map
{

void fill_polygon_areas(
  grid_map::GridMap & out_grid_map, const std::vector<geometry_msgs::msg::Polygon> & in_polygons,
  const std::string & in_grid_layer_name, const float in_layer_background_value,
  const float in_fill_value)
{
  if (!out_grid_map.exists(in_grid_layer_name)) {
    out_grid_map.add(in_grid_layer_name);
  }
  out_grid_map[in_grid_layer_name].setConstant(in_layer_background_value);

  for (const auto & poly : in_polygons) {
    grid_map::Polygon grid_map_poly;
    for (const auto & p : poly.points) {
      grid_map_poly.addVertex({p.x, p.y});
    }
    for (grid_map_utils::PolygonIterator it(out_grid_map, grid_map_poly); !it.isPastEnd(); ++it) {
      out_grid_map.at(in_grid_layer_name, *it) = in_fill_value;
    }
  }
}

}  // namespace autoware::costmap_generator::object_map

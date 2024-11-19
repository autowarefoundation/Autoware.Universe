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

#ifndef AUTOWARE__COSTMAP_GENERATOR__UTILS__OBJECT_MAP_UTILS_HPP_
#define AUTOWARE__COSTMAP_GENERATOR__UTILS__OBJECT_MAP_UTILS_HPP_

#include <grid_map_ros/grid_map_ros.hpp>
#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/polygon.hpp>
#include <grid_map_msgs/msg/grid_map.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <string>
#include <vector>

namespace autoware::costmap_generator::object_map
{
/*!
 * Fill the grid_map in the areas defined by the given polygons
 * @param[out] out_grid_map GridMap object to add the road grid
 * @param[in] in_polygons polygons to fill in the grid map
 * @param[in] in_grid_layer_name Name to assign to the layer
 * @param[in] in_layer_background_value Empty state value
 * @param[in] in_fill_value Value to fill inside the given polygons
 */
void fill_polygon_areas(
  grid_map::GridMap & out_grid_map, const std::vector<geometry_msgs::msg::Polygon> & in_polygons,
  const std::string & in_grid_layer_name, const float in_layer_background_value,
  const float in_fill_value);

}  // namespace autoware::costmap_generator::object_map

#endif  // AUTOWARE__COSTMAP_GENERATOR__UTILS__OBJECT_MAP_UTILS_HPP_

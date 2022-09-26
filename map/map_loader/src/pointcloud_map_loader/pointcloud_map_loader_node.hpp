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

/*
 * Copyright 2015-2019 Autoware Foundation. All rights reserved.
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
 */

#ifndef MAP_LOADER__POINTCLOUD_MAP_LOADER_NODE_HPP_
#define MAP_LOADER__POINTCLOUD_MAP_LOADER_NODE_HPP_

// #include "autoware_map_msgs/srv/load_pcd_partially.hpp"
// #include "autoware_map_msgs/srv/load_pcd_partially_for_publish.hpp"
#include "autoware_map_msgs/srv/load_pcd_maps_general.hpp"
#include "autoware_map_msgs/msg/area_info.hpp"
#include "autoware_map_msgs/msg/pcd_map_with_id.hpp"

#include "differential_map_loader_module.hpp"
#include "pointcloud_map_loader_module.hpp"

#include <rclcpp/rclcpp.hpp>

#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl/common/common.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <random>
#include <set>
#include <string>
#include <vector>

class PointCloudMapLoaderNode : public rclcpp::Node
{
public:
  explicit PointCloudMapLoaderNode(const rclcpp::NodeOptions & options);

private:
  // ros param
  float leaf_size_;

  std::unique_ptr<DifferentialMapLoaderModule> differential_map_loader_;
  std::unique_ptr<PointcloudMapLoaderModule> pcd_map_loader_;
  std::unique_ptr<PointcloudMapLoaderModule> downsampled_pcd_map_loader_;

  std::vector<std::string> getPcdPaths(
    const std::vector<std::string> & pcd_paths_or_directory) const;
};

#endif  // MAP_LOADER__POINTCLOUD_MAP_LOADER_NODE_HPP_

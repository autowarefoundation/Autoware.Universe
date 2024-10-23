// Copyright 2024 The Autoware Contributors
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

#ifndef MAP_LOADER__LANELET2_DIFFERENTIAL_LOADER_MODULE_HPP_
#define MAP_LOADER__LANELET2_DIFFERENTIAL_LOADER_MODULE_HPP_

#include "autoware_lanelet2_extension/io/autoware_multi_osm_parser.hpp"
#include "map_loader/utils.hpp"

#include <autoware/geography_utils/lanelet2_projector.hpp>
#include <autoware_lanelet2_extension/io/autoware_osm_parser.hpp>
#include <autoware_lanelet2_extension/projection/mgrs_projector.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <component_interface_specs/map.hpp>
#include <component_interface_utils/rclcpp.hpp>
#include <pugixml.hpp>
#include <rclcpp/rclcpp.hpp>

#include "autoware_map_msgs/srv/get_selected_lanelet2_map.hpp"
#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>
#include <autoware_map_msgs/msg/lanelet_map_meta_data.hpp>
#include <tier4_map_msgs/msg/map_projector_info.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/LineString.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_projection/UTM.h>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

using GetDifferentialLanelet2Map = autoware_map_msgs::srv::GetSelectedLanelet2Map;
using autoware_map_msgs::msg::LaneletMapBin;

class Lanelet2DifferentialLoaderModule
{
public:
  explicit Lanelet2DifferentialLoaderModule(
    rclcpp::Node * node, const double & center_line_resolution);

  void setLaneletMapMetadata(
    std::map<std::string, Lanelet2FileMetaData> & lanelet2_metadata_dict, double x_res,
    double y_res);

  void setProjectionInfo(const tier4_map_msgs::msg::MapProjectorInfo & projector_info);

  lanelet::LaneletMapPtr differentialLanelet2Load(std::vector<std::string> & lanelet2_paths);

private:
  rclcpp::Logger logger_;
  rclcpp::Clock::SharedPtr clock_;

  std::map<std::string, Lanelet2FileMetaData> lanelet2_file_metadata_dict_;

  rclcpp::Service<GetDifferentialLanelet2Map>::SharedPtr get_differential_lanelet2_maps_service_;

  component_interface_utils::Publisher<map_interface::LaneletMapMetaData>::SharedPtr
    pub_lanelet_map_meta_data_;

  std::optional<tier4_map_msgs::msg::MapProjectorInfo> projector_info_;

  double center_line_resolution_;

  bool onServiceGetDifferentialLanelet2Map(
    GetDifferentialLanelet2Map::Request::SharedPtr req,
    GetDifferentialLanelet2Map::Response::SharedPtr res);

  autoware_map_msgs::msg::LaneletMapMetaData getLaneletMapMetaDataMsg(
    const double & x_res, const double & y_res) const;
};

#endif  // MAP_LOADER__LANELET2_DIFFERENTIAL_LOADER_MODULE_HPP_

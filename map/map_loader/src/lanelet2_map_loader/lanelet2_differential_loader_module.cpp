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

#include "map_loader/lanelet2_differential_loader_module.hpp"

#include "lanelet2_local_projector.hpp"
#include "map_loader/lanelet2_map_loader_node.hpp"

Lanelet2DifferentialLoaderModule::Lanelet2DifferentialLoaderModule(
  rclcpp::Node * node, const double & center_line_resolution)
: logger_(node->get_logger()),
  clock_(node->get_clock()),
  center_line_resolution_(center_line_resolution)
{
  const auto metadata_adaptor = component_interface_utils::NodeAdaptor(node);
  metadata_adaptor.init_pub(pub_lanelet_map_meta_data_);

  get_differential_lanelet2_maps_service_ = node->create_service<GetDifferentialLanelet2Map>(
    "service/get_differential_lanelet_map",
    std::bind(
      &Lanelet2DifferentialLoaderModule::onServiceGetDifferentialLanelet2Map, this,
      std::placeholders::_1, std::placeholders::_2));
}

bool Lanelet2DifferentialLoaderModule::onServiceGetDifferentialLanelet2Map(
  GetDifferentialLanelet2Map::Request::SharedPtr req,
  GetDifferentialLanelet2Map::Response::SharedPtr res)
{
  std::vector<std::string> lanelet2_paths;
  for (const auto & id : req->cell_ids) {
    auto it = std::find_if(
      lanelet2_file_metadata_dict_.begin(), lanelet2_file_metadata_dict_.end(),
      [&id](const auto & file) { return file.second.id == id; });
    if (it == lanelet2_file_metadata_dict_.end()) {
      continue;
    }
    if (!std::filesystem::exists(it->first)) {
      continue;
    }
    lanelet2_paths.push_back(it->first);
  }
  if (lanelet2_paths.empty()) {
    RCLCPP_ERROR(logger_, "Failed to load differential lanelet2 map");
    return false;
  }

  const auto map = differentialLanelet2Load(lanelet2_paths);
  const auto map_bin_msg =
    Lanelet2MapLoaderNode::create_map_bin_msg(map, lanelet2_paths[0], rclcpp::Clock().now());

  res->lanelet2_cells = map_bin_msg;
  res->header.frame_id = "map";

  return true;
}

lanelet::LaneletMapPtr Lanelet2DifferentialLoaderModule::differentialLanelet2Load(
  std::vector<std::string> & lanelet2_paths)
{
  if (projector_info_.value().projector_type != tier4_map_msgs::msg::MapProjectorInfo::LOCAL) {
    std::unique_ptr<lanelet::Projector> projector =
      autoware::geography_utils::get_lanelet2_projector(projector_info_.value());

    lanelet::ErrorMessages errors{};
    lanelet::io_handlers::MultiOsmParser parser(*projector);
    lanelet::LaneletMapPtr map = parser.parse(lanelet2_paths, errors);

    if (!errors.empty()) {
      for (const auto & error : errors) {
        RCLCPP_ERROR_STREAM(rclcpp::get_logger("map_loader"), error);
      }
    }

    return map;
  } else {
    const lanelet::projection::LocalProjector projector;
    lanelet::ErrorMessages errors{};
    lanelet::io_handlers::MultiOsmParser parser(projector);
    lanelet::LaneletMapPtr map = parser.parse(lanelet2_paths, errors);

    if (!errors.empty()) {
      for (const auto & error : errors) {
        RCLCPP_ERROR_STREAM(rclcpp::get_logger("map_loader"), error);
      }
    }

    // overwrite local_x, local_y
    for (lanelet::Point3d point : map->pointLayer) {
      if (point.hasAttribute("local_x")) {
        point.x() = point.attribute("local_x").asDouble().value();
      }
      if (point.hasAttribute("local_y")) {
        point.y() = point.attribute("local_y").asDouble().value();
      }
    }

    // realign lanelet borders using updated points
    for (lanelet::Lanelet lanelet : map->laneletLayer) {
      auto left = lanelet.leftBound();
      auto right = lanelet.rightBound();
      std::tie(left, right) = lanelet::geometry::align(left, right);
      lanelet.setLeftBound(left);
      lanelet.setRightBound(right);
    }

    return map;
  }
}

void Lanelet2DifferentialLoaderModule::setLaneletMapMetadata(
  std::map<std::string, Lanelet2FileMetaData> & lanelet2_metadata_dict, double x_res, double y_res)
{
  lanelet2_file_metadata_dict_ = lanelet2_metadata_dict;

  const auto msg = getLaneletMapMetaDataMsg(x_res, y_res);
  pub_lanelet_map_meta_data_->publish(msg);
}

void Lanelet2DifferentialLoaderModule::setProjectionInfo(
  const tier4_map_msgs::msg::MapProjectorInfo & projector_info)
{
  projector_info_ = projector_info;
}

autoware_map_msgs::msg::LaneletMapMetaData
Lanelet2DifferentialLoaderModule::getLaneletMapMetaDataMsg(
  const double & x_res, const double & y_res) const
{
  autoware_map_msgs::msg::LaneletMapMetaData metadata;
  for (const auto & file : lanelet2_file_metadata_dict_) {
    autoware_map_msgs::msg::LaneletMapCellMetaData cell_msg;
    cell_msg.cell_id = file.second.id;
    cell_msg.min_x = file.second.min_x;
    cell_msg.min_y = file.second.min_y;
    cell_msg.max_x = file.second.min_x + x_res;
    cell_msg.max_y = file.second.min_y + y_res;

    metadata.metadata_list.push_back(cell_msg);
  }
  metadata.header.frame_id = "map";
  metadata.header.stamp = clock_->now();

  return metadata;
}

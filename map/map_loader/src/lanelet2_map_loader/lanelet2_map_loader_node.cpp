// Copyright 2021 TierIV
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
 * Copyright 2019 Autoware Foundation. All rights reserved.
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
 *
 * Authors: Simon Thompson, Ryohsuke Mitsudome
 *
 */

#include "map_loader/lanelet2_map_loader_node.hpp"

#include "lanelet2_local_projector.hpp"

#include <ament_index_cpp/get_package_prefix.hpp>
#include <autoware/geography_utils/lanelet2_projector.hpp>
#include <autoware_lanelet2_extension/io/autoware_osm_parser.hpp>
#include <autoware_lanelet2_extension/projection/mgrs_projector.hpp>
#include <autoware_lanelet2_extension/projection/transverse_mercator_projector.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <rclcpp/rclcpp.hpp>

#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/LineString.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_projection/UTM.h>

#include <filesystem>
#include <stdexcept>
#include <string>

namespace
{
bool isOsmFile(const std::string & f)
{
  if (std::filesystem::is_directory(f)) {
    return false;
  }

  const std::string ext = std::filesystem::path(f).extension();

  if (ext != ".osm" && ext != ".OSM") {
    return false;
  }

  return true;
}
}  // namespace

using autoware_map_msgs::msg::LaneletMapBin;
using tier4_map_msgs::msg::MapProjectorInfo;

Lanelet2MapLoaderNode::Lanelet2MapLoaderNode(const rclcpp::NodeOptions & options)
: Node("lanelet2_map_loader", options)
{
  const auto adaptor = component_interface_utils::NodeAdaptor(this);

  // subscription
  adaptor.init_sub(
    sub_map_projector_info_,
    [this](const MapProjectorInfo::Message::ConstSharedPtr msg) { on_map_projector_info(msg); });

  declare_parameter<bool>("allow_unsupported_version");
  declare_parameter<std::string>("lanelet2_map_path");
  declare_parameter<double>("center_line_resolution");
  declare_parameter<bool>("use_waypoints");
  declare_parameter<bool>("enable_differential_map_loading");

  if (get_parameter("enable_differential_map_loading").as_bool()) {
    differential_loader_module_ = std::make_unique<Lanelet2DifferentialLoaderModule>(
      this, get_parameter("center_line_resolution").as_double());
  }
}

void Lanelet2MapLoaderNode::on_map_projector_info(
  const MapProjectorInfo::Message::ConstSharedPtr msg)
{
  const auto allow_unsupported_version = get_parameter("allow_unsupported_version").as_bool();
  auto lanelet2_filename = get_parameter("lanelet2_map_path").as_string();
  const auto center_line_resolution = get_parameter("center_line_resolution").as_double();
  const auto use_waypoints = get_parameter("use_waypoints").as_bool();
  const auto enable_differential_map_loading =
    get_parameter("enable_differential_map_loading").as_bool();

  lanelet::LaneletMapPtr map;

  if (!enable_differential_map_loading) {
    map = load_map(lanelet2_filename, *msg);
    if (!map) {
      RCLCPP_ERROR(get_logger(), "Failed to load lanelet2_map. Not published.");
      return;
    }
  } else {
    RCLCPP_INFO(get_logger(), "Differential lanelet2 map loading is enabled.");

    std::vector<std::string> lanelet2_paths_or_directory = {
      get_parameter("lanelet2_map_path").as_string()};
    std::string lanelet2_metadata_path =
      declare_parameter<std::string>("lanelet2_map_metadata_path");
    double x_resolution, y_resolution;

    std::map<std::string, Lanelet2FileMetaData> lanelet2_metadata_dict;
    if (std::filesystem::exists(lanelet2_metadata_path)) {
      lanelet2_metadata_dict = get_lanelet2_metadata(
        lanelet2_metadata_path, get_lanelet2_paths(lanelet2_paths_or_directory), x_resolution,
        y_resolution);
    } else {
      if (lanelet2_paths_or_directory.size() == 1) {
        // Create a dummy metadata for a single osm file
        lanelet2_metadata_dict =
          get_dummy_lanelet2_metadata(lanelet2_filename, msg, x_resolution, y_resolution);
      } else {
        throw std::runtime_error("Lanelet2 metadata file not found: " + lanelet2_metadata_path);
      }
    }

    {
      // set metadata and projection info to differential loader module
      differential_loader_module_->setLaneletMapMetadata(
        lanelet2_metadata_dict, x_resolution, y_resolution);
      differential_loader_module_->setProjectionInfo(*msg);
    }

    {
      // load whole map for once
      std::vector<std::string> lanelet2_paths;
      lanelet2_paths.reserve(lanelet2_metadata_dict.size());
      for (const auto & [lanelet2_path, _] : lanelet2_metadata_dict) {
        lanelet2_paths.push_back(lanelet2_path);
      }
      map = differential_loader_module_->differentialLanelet2Load(lanelet2_paths);
    }
    lanelet2_filename =
      lanelet2_metadata_dict.begin()
        ->first;  // TODO(StepTurtle): find better way: `parseVersions` function read the osm
                  // file to get map version info, so `lanelet2_filename` should
                  // be osm file path, it changes `lanelet2_filename` hard coded.
                  // Because of this, `lanelet2_filename` can't be const in line 103.
  }

  std::string format_version{"null"}, map_version{""};
  lanelet::io_handlers::AutowareOsmParser::parseVersions(
    lanelet2_filename, &format_version, &map_version);
  if (format_version == "null" || format_version.empty() || !isdigit(format_version[0])) {
    RCLCPP_WARN(
      get_logger(),
      "%s has no format_version(null) or non semver-style format_version(%s) information",
      lanelet2_filename.c_str(), format_version.c_str());
    if (!allow_unsupported_version) {
      throw std::invalid_argument(
        "allow_unsupported_version is false, so stop loading lanelet map");
    }
  } else if (const auto map_major_ver_opt = lanelet::io_handlers::parseMajorVersion(format_version);
             map_major_ver_opt.has_value()) {
    const auto map_major_ver = map_major_ver_opt.value();
    if (map_major_ver > static_cast<uint64_t>(lanelet::autoware::version)) {
      RCLCPP_WARN(
        get_logger(),
        "format_version(%ld) of the provided map(%s) is larger than the supported version(%ld)",
        map_major_ver, lanelet2_filename.c_str(),
        static_cast<uint64_t>(lanelet::autoware::version));
      if (!allow_unsupported_version) {
        throw std::invalid_argument(
          "allow_unsupported_version is false, so stop loading lanelet map");
      }
    }
  }
  RCLCPP_INFO(get_logger(), "Loaded map format_version: %s", format_version.c_str());

  // overwrite centerline
  if (use_waypoints) {
    lanelet::utils::overwriteLaneletsCenterlineWithWaypoints(map, center_line_resolution, false);
  } else {
    lanelet::utils::overwriteLaneletsCenterline(map, center_line_resolution, false);
  }

  // create map bin msg
  const auto map_bin_msg = create_map_bin_msg(map, lanelet2_filename, now());

  // create publisher and publish
  pub_map_bin_ =
    create_publisher<LaneletMapBin>("output/lanelet2_map", rclcpp::QoS{1}.transient_local());
  pub_map_bin_->publish(map_bin_msg);
  RCLCPP_INFO(get_logger(), "Succeeded to load lanelet2_map. Map is published.");
}

lanelet::LaneletMapPtr Lanelet2MapLoaderNode::load_map(
  const std::string & lanelet2_filename,
  const tier4_map_msgs::msg::MapProjectorInfo & projector_info)
{
  lanelet::ErrorMessages errors{};
  if (projector_info.projector_type != tier4_map_msgs::msg::MapProjectorInfo::LOCAL) {
    std::unique_ptr<lanelet::Projector> projector =
      autoware::geography_utils::get_lanelet2_projector(projector_info);
    lanelet::LaneletMapPtr map = lanelet::load(lanelet2_filename, *projector, &errors);
    if (errors.empty()) {
      return map;
    }
  } else {
    const lanelet::projection::LocalProjector projector;
    lanelet::LaneletMapPtr map = lanelet::load(lanelet2_filename, projector, &errors);

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

  for (const auto & error : errors) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("map_loader"), error);
  }
  return nullptr;
}

LaneletMapBin Lanelet2MapLoaderNode::create_map_bin_msg(
  const lanelet::LaneletMapPtr map, const std::string & lanelet2_filename, const rclcpp::Time & now)
{
  std::string format_version{};
  std::string map_version{};
  lanelet::io_handlers::AutowareOsmParser::parseVersions(
    lanelet2_filename, &format_version, &map_version);

  LaneletMapBin map_bin_msg;
  map_bin_msg.header.stamp = now;
  map_bin_msg.header.frame_id = "map";
  map_bin_msg.version_map_format = format_version;
  map_bin_msg.version_map = map_version;
  lanelet::utils::conversion::toBinMsg(map, &map_bin_msg);

  return map_bin_msg;
}

std::vector<std::string> Lanelet2MapLoaderNode::get_lanelet2_paths(
  const std::vector<std::string> & lanelet2_paths_or_directory) const
{
  std::vector<std::string> lanelet2_paths;
  for (const auto & path : lanelet2_paths_or_directory) {
    if (!std::filesystem::exists(path)) {
      RCLCPP_ERROR_STREAM(get_logger(), "No such file or directory: " << path);
      continue;
    }

    if (isOsmFile(path)) {
      lanelet2_paths.push_back(path);
    }

    if (std::filesystem::is_directory(path)) {
      for (const auto & file : std::filesystem::directory_iterator(path)) {
        const auto filename = file.path().string();
        if (isOsmFile(filename)) {
          lanelet2_paths.push_back(filename);
        }
      }
    }
  }
  return lanelet2_paths;
}

std::map<std::string, Lanelet2FileMetaData> Lanelet2MapLoaderNode::get_lanelet2_metadata(
  const std::string & lanelet2_metadata_path, const std::vector<std::string> & lanelet2_paths,
  double & x_resolution, double & y_resolution) const
{
  std::map<std::string, Lanelet2FileMetaData> lanelet2_metadata_dict;
  lanelet2_metadata_dict = loadLanelet2Metadata(lanelet2_metadata_path, x_resolution, y_resolution);
  lanelet2_metadata_dict = replaceWithAbsolutePath(lanelet2_metadata_dict, lanelet2_paths);
  RCLCPP_INFO_STREAM(get_logger(), "Loaded Lanelet2 metadata: " << lanelet2_metadata_path);

  return lanelet2_metadata_dict;
}

std::map<std::string, Lanelet2FileMetaData> Lanelet2MapLoaderNode::get_dummy_lanelet2_metadata(
  const std::string & lanelet2_path,
  const MapProjectorInfo::Message::ConstSharedPtr projection_info, double & x_resolution,
  double & y_resolution)
{
  const auto map = load_map(lanelet2_path, *projection_info);

  declare_parameter<double>("dummy_metadata.min_x");
  declare_parameter<double>("dummy_metadata.min_y");
  declare_parameter<double>("dummy_metadata.x_resolution");
  declare_parameter<double>("dummy_metadata.y_resolution");

  Lanelet2FileMetaData tile;
  tile.id = "0";
  tile.min_x = get_parameter("dummy_metadata.min_x").as_double();
  tile.min_y = get_parameter("dummy_metadata.min_y").as_double();
  x_resolution = get_parameter("dummy_metadata.x_resolution").as_double();
  y_resolution = get_parameter("dummy_metadata.y_resolution").as_double();

  return std::map<std::string, Lanelet2FileMetaData>{{lanelet2_path, tile}};
}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(Lanelet2MapLoaderNode)

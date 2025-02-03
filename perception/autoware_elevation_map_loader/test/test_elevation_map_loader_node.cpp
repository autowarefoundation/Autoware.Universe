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

#include "elevation_map_loader_node.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <gtest/gtest.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

#include <vector>

using autoware::elevation_map_loader::ElevationMapLoaderNode;

std::shared_ptr<autoware::test_utils::AutowareTestManager> generateTestManager()
{
  auto test_manager = std::make_shared<autoware::test_utils::AutowareTestManager>();
  return test_manager;
}

std::shared_ptr<ElevationMapLoaderNode> generateNode()
{
  auto node_options = rclcpp::NodeOptions{};
  const auto detected_object_validation_dir =
    ament_index_cpp::get_package_share_directory("autoware_elevation_map_loader");
  const auto parameter_file_path =
    detected_object_validation_dir + "/config/elevation_map_parameters.yaml";

  // define parameters
  node_options.append_parameter_override("param_file_path", parameter_file_path);
  node_options.append_parameter_override("use_sequential_load", false);
  node_options.append_parameter_override(
    "elevation_map_path", detected_object_validation_dir + "/data/elevation_maps");
  return std::make_shared<ElevationMapLoaderNode>(node_options);
}

std::shared_ptr<sensor_msgs::msg::PointCloud2> createEmptyPointCloud2(const std::string & frame_id)
{
  auto pointcloud_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();

  pointcloud_msg->header.frame_id = frame_id;
  pointcloud_msg->header.stamp = rclcpp::Clock().now();

  pointcloud_msg->height = 1;
  pointcloud_msg->width = 0;
  pointcloud_msg->is_dense = false;
  pointcloud_msg->is_bigendian = false;

  sensor_msgs::PointCloud2Modifier modifier(*pointcloud_msg);
  modifier.setPointCloud2Fields(
    3, "x", 1, sensor_msgs::msg::PointField::FLOAT32, "y", 1, sensor_msgs::msg::PointField::FLOAT32,
    "z", 1, sensor_msgs::msg::PointField::FLOAT32);

  modifier.resize(0);

  return pointcloud_msg;
}

// load pcd map from autoware_test_utils
std::shared_ptr<sensor_msgs::msg::PointCloud2> loadMapPointcloud()
{
  const std::string autoware_test_util_dir =
    ament_index_cpp::get_package_share_directory("autoware_test_utils");
  const std::string pcd_file = autoware_test_util_dir + "/test_map/pointcloud_map.pcd";

  pcl::PointCloud<pcl::PointXYZ>::Ptr pcl_map(new pcl::PointCloud<pcl::PointXYZ>);
  if (pcl::io::loadPCDFile<pcl::PointXYZ>(pcd_file, *pcl_map) == -1) {
    std::cerr << "Failed to load PCD file" << std::endl;
    return nullptr;
  }

  // sensor_msgs::msg::PointCloud2 conversion
  auto pointcloud_msg = std::make_shared<sensor_msgs::msg::PointCloud2>();
  pcl::toROSMsg(*pcl_map, *pointcloud_msg);

  // set header
  pointcloud_msg->header.frame_id = "map";
  pointcloud_msg->header.stamp = rclcpp::Clock().now();

  return pointcloud_msg;
}

TEST(ElevationMapLoaderNode, testSamplePCInput)
{
  // initialize node and test manager
  rclcpp::init(0, nullptr);
  auto test_manager = generateTestManager();
  auto node = generateNode();
  const std::string input_topic = "/input/pointcloud_map";
  const std::string output_topic = "/output/elevation_map";
  const std::string api_topic = "/api/autoware/get/map/info/hash";

  // add callback to count output
  uint32_t counter = 0;
  auto callback = [&counter](const grid_map_msgs::msg::GridMap::ConstSharedPtr msg) {
    (void)msg;
    ++counter;
  };
  // (optional) qos settings
  rclcpp::QoS durable_qos{1};
  durable_qos.transient_local();

  // subscribe output
  test_manager->set_subscriber<grid_map_msgs::msg::GridMap>(output_topic, callback, durable_qos);

  durable_qos.transient_local();
  // ad api call back
  tier4_external_api_msgs::msg::MapHash map_hash_msg;
  map_hash_msg.pcd = ".empty";  // empty pcd
  test_manager->test_pub_msg<tier4_external_api_msgs::msg::MapHash>(
    node, api_topic, map_hash_msg, durable_qos);

  // null pointcloud
  sensor_msgs::msg::PointCloud2 null_pointcloud = *loadMapPointcloud();
  test_manager->test_pub_msg<sensor_msgs::msg::PointCloud2>(
    node, input_topic, null_pointcloud, durable_qos);

  // check counter output
  EXPECT_EQ(counter, 1);

  rclcpp::shutdown();
}

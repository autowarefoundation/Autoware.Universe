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

#include "../src/voxel_based_compare_map_filter/node.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/point_types/types.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <point_cloud_msg_wrapper/point_cloud_msg_wrapper.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>

#include <gtest/gtest.h>

using autoware::compare_map_segmentation::VoxelBasedCompareMapFilterComponent;
using autoware::point_types::PointXYZIRC;
using autoware::point_types::PointXYZIRCGenerator;
using point_cloud_msg_wrapper::PointCloud2Modifier;
using sensor_msgs::msg::PointCloud2;

std::shared_ptr<autoware::test_utils::AutowareTestManager> generateTestManager()
{
  auto test_manager = std::make_shared<autoware::test_utils::AutowareTestManager>();
  return test_manager;
};

std::shared_ptr<VoxelBasedCompareMapFilterComponent> generateNode(
  const bool use_dynamic_map_loading)
{
  auto node_options = rclcpp::NodeOptions{};
  const auto compare_map_segmentation_dir =
    ament_index_cpp::get_package_share_directory("autoware_compare_map_segmentation");
  node_options.arguments(
    {"--ros-args", "--params-file",
     compare_map_segmentation_dir + "/config/voxel_based_compare_map_filter.param.yaml"});

  node_options.append_parameter_override("use_dynamic_map_loading", use_dynamic_map_loading);
  node_options.append_parameter_override("input_frame", "map");
  node_options.append_parameter_override("publish_debug_pcd", true);

  return std::make_unique<VoxelBasedCompareMapFilterComponent>(node_options);
};
PointCloud2 create_pointcloud(const int number_of_point)
{
  PointCloud2 pointcloud;
  PointCloud2Modifier<PointXYZIRC, PointXYZIRCGenerator> modifier(pointcloud, "map");
  modifier.resize(number_of_point);
  for (int i = 0; i < number_of_point; ++i) {
    modifier[i].x = static_cast<float>(i) / 10.0F;
    modifier[i].y = 0.0F;
    modifier[i].z = 0.0F;
    modifier[i].intensity = 0U;
    modifier[i].return_type = 0U;
    modifier[i].channel = 0U;
  }
  return pointcloud;
}

TEST(VoxelBasedCompareMapFilterComponentTest, testEmptyInputPointcloud)
{
  rclcpp::init(0, nullptr);
  const std::string input_map_topic = "/map";
  const std::string input_pc_topic = "/input";
  const std::string output_pc_topic = "/output";
  auto test_manager = generateTestManager();
  auto test_target_node = generateNode(false);

  int counter = 0;
  PointCloud2 output_pc_msg;
  auto callback = [&counter, &output_pc_msg](const PointCloud2::ConstSharedPtr msg) {
    output_pc_msg = *msg;
    ++counter;
  };
  rclcpp::QoS output_qos(5);
  output_qos.durability_volatile();
  output_qos.keep_last(1);
  output_qos.best_effort();
  test_manager->set_subscriber<PointCloud2>(output_pc_topic, callback, output_qos);

  // create and publish map pointcloud topic
  PointCloud2 input_map_pointcloud = create_pointcloud(10);
  rclcpp::QoS map_qos(5);
  map_qos.transient_local();
  map_qos.keep_last(1);
  map_qos.reliable();
  test_manager->test_pub_msg<PointCloud2>(
    test_target_node, input_map_topic, input_map_pointcloud, map_qos);

  rclcpp::QoS input_qos(10);
  input_qos.durability_volatile();
  input_qos.keep_last(1);
  input_qos.best_effort();
  // create and publish input pointcloud topic
  PointCloud2 input_pc_msg = create_pointcloud(100);
  test_manager->test_pub_msg<PointCloud2>(
    test_target_node, input_pc_topic, input_pc_msg, input_qos);
  EXPECT_EQ(counter, 1);
  EXPECT_GT(output_pc_msg.data.size(), 0);
  EXPECT_GT(input_pc_msg.data.size(), output_pc_msg.data.size());
}

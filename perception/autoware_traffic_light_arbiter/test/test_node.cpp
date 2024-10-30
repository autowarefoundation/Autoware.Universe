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

#include "autoware/traffic_light_arbiter/traffic_light_arbiter.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <vector>

using autoware::TrafficLightArbiter;
using LaneletMapBin = autoware_map_msgs::msg::LaneletMapBin;
using TrafficSignalArray = autoware_perception_msgs::msg::TrafficLightGroupArray;
using TrafficSignal = autoware_perception_msgs::msg::TrafficLightGroup;
using TrafficElement = autoware_perception_msgs::msg::TrafficLightElement;

std::shared_ptr<autoware::test_utils::AutowareTestManager> generateTestManager()
{
  auto test_manager = std::make_shared<autoware::test_utils::AutowareTestManager>();
  return test_manager;
}

std::shared_ptr<TrafficLightArbiter> generateNode()
{
  auto node_options = rclcpp::NodeOptions{};
  const auto package_dir =
    ament_index_cpp::get_package_share_directory("autoware_traffic_light_arbiter");
  node_options.arguments(
    {"--ros-args", "--params-file", package_dir + "/config/traffic_light_arbiter.param.yaml"});
  return std::make_shared<TrafficLightArbiter>(node_options);
}

void generateMap(LaneletMapBin & vector_map_msg)
{
  const auto package_dir = ament_index_cpp::get_package_share_directory("autoware_test_utils");
  lanelet::LaneletMapPtr vector_map_ptr =
    autoware::test_utils::loadMap(package_dir + "/test_map/lanelet2_map.osm");
  vector_map_msg.header.frame_id = "base_link";
  lanelet::utils::conversion::toBinMsg(vector_map_ptr, &vector_map_msg);
}

void generatePerceptionMsg(TrafficSignalArray & perception_msg)
{
  // traffic_light_group_id 1: 1012
  {
    TrafficSignal traffic_light_groups;
    traffic_light_groups.traffic_light_group_id = 1012;
    // elements 1: red + circle
    {
      TrafficElement elements;
      elements.color = TrafficElement::RED;
      elements.shape = TrafficElement::CIRCLE;
      elements.status = TrafficElement::SOLID_ON;
      elements.confidence = 0.9;
      traffic_light_groups.elements.push_back(elements);
    }
    // elements 2: green + right arrow
    {
      TrafficElement elements;
      elements.color = TrafficElement::GREEN;
      elements.shape = TrafficElement::RIGHT_ARROW;
      elements.status = TrafficElement::SOLID_ON;
      elements.confidence = 0.8;
      traffic_light_groups.elements.push_back(elements);
    }
    perception_msg.traffic_light_groups.push_back(traffic_light_groups);
  }
}

void generateExternalMsg(TrafficSignalArray & external_msg)
{
  // traffic_light_group_id 1: 1012
  {
    TrafficSignal traffic_light_groups;
    traffic_light_groups.traffic_light_group_id = 1012;
    // elements 1: green + circle
    {
      TrafficElement elements;
      elements.color = TrafficElement::GREEN;
      elements.shape = TrafficElement::CIRCLE;
      elements.status = TrafficElement::SOLID_ON;
      elements.confidence = 0.6;
      traffic_light_groups.elements.push_back(elements);
    }
    // elements 2: green + right arrow
    {
      TrafficElement elements;
      elements.color = TrafficElement::GREEN;
      elements.shape = TrafficElement::RIGHT_ARROW;
      elements.status = TrafficElement::SOLID_ON;
      elements.confidence = 0.5;
      traffic_light_groups.elements.push_back(elements);
    }
    external_msg.traffic_light_groups.push_back(traffic_light_groups);
  }
}

bool isMsgEqual(const TrafficSignalArray & input_msg, const TrafficSignalArray & gt_msg)
{
  // check number of groups
  if (input_msg.traffic_light_groups.size() != gt_msg.traffic_light_groups.size()) {
    return false;
  }

  for (std::size_t group_idx = 0; group_idx < input_msg.traffic_light_groups.size(); ++group_idx) {
    const auto & input_traffic_light_group = input_msg.traffic_light_groups.at(group_idx);
    const auto & gt_traffic_light_group = gt_msg.traffic_light_groups.at(group_idx);

    // check traffic_light_group_id
    if (
      input_traffic_light_group.traffic_light_group_id !=
      gt_traffic_light_group.traffic_light_group_id) {
      return false;
    }

    // check number of elements
    if (input_traffic_light_group.elements.size() != gt_traffic_light_group.elements.size()) {
      std::cout << input_traffic_light_group.elements.size() << std::endl;
      std::cout << gt_traffic_light_group.elements.size() << std::endl;
      return false;
    }

    for (std::size_t element_idx = 0; element_idx < input_traffic_light_group.elements.size();
         ++element_idx) {
      const auto & input_traffic_light_element = input_traffic_light_group.elements.at(element_idx);
      const auto & gt_traffic_light_element = gt_traffic_light_group.elements.at(element_idx);

      // check color
      if (input_traffic_light_element.color != gt_traffic_light_element.color) {
        return false;
      }

      // check shape
      if (input_traffic_light_element.shape != gt_traffic_light_element.shape) {
        return false;
      }

      // check status
      if (input_traffic_light_element.status != gt_traffic_light_element.status) {
        return false;
      }

      // check confidence
      constexpr float error = std::numeric_limits<float>::epsilon();
      if (
        std::fabs(input_traffic_light_element.confidence - gt_traffic_light_element.confidence) >
        error) {
        return false;
      }
    }
  }
  return true;
}

TEST(TrafficLightArbiterTest, testTrafficSignalOnlyPerceptionMsg)
{
  rclcpp::init(0, nullptr);
  const std::string input_map_topic = "/traffic_light_arbiter/sub/vector_map";
  const std::string input_perception_topic =
    "/traffic_light_arbiter/sub/perception_traffic_signals";
  const std::string output_topic = "/traffic_light_arbiter/pub/traffic_signals";
  auto test_manager = generateTestManager();
  auto test_target_node = generateNode();

  // map msg preparation
  LaneletMapBin vector_map_msg;
  generateMap(vector_map_msg);

  // perception msg preparation
  TrafficSignalArray perception_msg;
  generatePerceptionMsg(perception_msg);

  // test callback preparation
  TrafficSignalArray latest_msg;
  auto callback = [&latest_msg](const TrafficSignalArray::ConstSharedPtr msg) {
    latest_msg = *msg;
  };
  test_manager->set_subscriber<TrafficSignalArray>(output_topic, callback);

  test_manager->test_pub_msg<LaneletMapBin>(
    test_target_node, input_map_topic, vector_map_msg, rclcpp::QoS(1).transient_local());
  test_manager->test_pub_msg<TrafficSignalArray>(
    test_target_node, input_perception_topic, perception_msg);

  EXPECT_EQ(isMsgEqual(latest_msg, perception_msg), true);
  rclcpp::shutdown();
}

TEST(TrafficLightArbiterTest, testTrafficSignalOnlyExternalMsg)
{
  rclcpp::init(0, nullptr);
  const std::string input_map_topic = "/traffic_light_arbiter/sub/vector_map";
  const std::string input_external_topic = "/traffic_light_arbiter/sub/external_traffic_signals";
  const std::string output_topic = "/traffic_light_arbiter/pub/traffic_signals";
  auto test_manager = generateTestManager();
  auto test_target_node = generateNode();

  // map msg preparation
  LaneletMapBin vector_map_msg;
  generateMap(vector_map_msg);

  // external msg preparation
  TrafficSignalArray external_msg;
  generateExternalMsg(external_msg);

  // test callback preparation
  TrafficSignalArray latest_msg;
  auto callback = [&latest_msg](const TrafficSignalArray::ConstSharedPtr msg) {
    latest_msg = *msg;
  };
  test_manager->set_subscriber<TrafficSignalArray>(output_topic, callback);

  test_manager->test_pub_msg<LaneletMapBin>(
    test_target_node, input_map_topic, vector_map_msg, rclcpp::QoS(1).transient_local());
  test_manager->test_pub_msg<TrafficSignalArray>(
    test_target_node, input_external_topic, external_msg);

  EXPECT_EQ(isMsgEqual(latest_msg, external_msg), true);
  rclcpp::shutdown();
}

TEST(TrafficLightArbiterTest, testTrafficSignalBothMsg)
{
  rclcpp::init(0, nullptr);
  const std::string input_map_topic = "/traffic_light_arbiter/sub/vector_map";
  const std::string input_perception_topic =
    "/traffic_light_arbiter/sub/perception_traffic_signals";
  const std::string input_external_topic = "/traffic_light_arbiter/sub/external_traffic_signals";
  const std::string output_topic = "/traffic_light_arbiter/pub/traffic_signals";
  auto test_manager = generateTestManager();
  auto test_target_node = generateNode();

  // map preparation
  LaneletMapBin vector_map_msg;
  generateMap(vector_map_msg);

  // perception preparation
  TrafficSignalArray perception_msg;
  generatePerceptionMsg(perception_msg);

  // external preparation
  TrafficSignalArray external_msg;
  generateExternalMsg(external_msg);

  // test callback preparation
  TrafficSignalArray latest_msg;
  auto callback = [&latest_msg](const TrafficSignalArray::ConstSharedPtr msg) {
    latest_msg = *msg;
  };
  test_manager->set_subscriber<TrafficSignalArray>(output_topic, callback);

  test_manager->test_pub_msg<LaneletMapBin>(
    test_target_node, input_map_topic, vector_map_msg, rclcpp::QoS(1).transient_local());
  test_manager->test_pub_msg<TrafficSignalArray>(
    test_target_node, input_external_topic, external_msg);
  test_manager->test_pub_msg<TrafficSignalArray>(
    test_target_node, input_perception_topic, perception_msg);

  // latest_msg should be equal to perception_msg because it has higher confidence than external_msg
  EXPECT_EQ(isMsgEqual(latest_msg, perception_msg), true);
  rclcpp::shutdown();
}

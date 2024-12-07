// Copyright 2024 TIER IV, inc.
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

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "shape_estimation_node.hpp"

#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/executors.hpp>

#include "tier4_perception_msgs/msg/detail/detected_objects_with_feature__struct.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

namespace
{
using autoware::shape_estimation::ShapeEstimationNode;
using autoware::test_utils::AutowareTestManager;
using sensor_msgs::msg::PointCloud2;
using tier4_perception_msgs::msg::DetectedObjectsWithFeature;
using tier4_perception_msgs::msg::DetectedObjectWithFeature;

/**
 * @brief Return a new `autoware::test_utils::AutowareTestManager` instance as a shared pointer.
 *
 * @return Shared pointer of `autoware::test_utils::AutowareTestManager`.
 */
std::shared_ptr<AutowareTestManager> generate_test_manager()
{
  return std::make_shared<AutowareTestManager>();
}

/**
 * @brief Return a new `autoware::shape_estimation::ShapeEstimationNode` instance as a shared
 * pointer.
 *
 * @param input_topic Input topic name.
 * @param output_topic Output topic name.
 * @return Shared pointer of `autoware::shape_estimation::ShapeEstimationNode`.
 */
std::shared_ptr<ShapeEstimationNode> generate_node(
  const std::string & input_topic, const std::string & output_topic)
{
  const auto package_dir =
    ament_index_cpp::get_package_share_directory("autoware_shape_estimation");

  auto node_options = rclcpp::NodeOptions();
  node_options.arguments(
    {"--ros-args", "--params-file", package_dir + "/config/shape_estimation.param.yaml", "-r",
     "input:=" + input_topic, "-r", "objects:=" + output_topic});
  return std::make_shared<ShapeEstimationNode>(node_options);
}

/**
 * @brief Return a new `PointCloud<pcl::PointXYZ>` object shaping like L.
 *
 * @param length Length of L box in [m].
 * @param width Width of L box in [m].
 * @param height Height of L box in [m].
 * @param yaw Yaw rotation offset in [rad]. Defaults to 0.0.
 * @param offset_x X position offset in [m]. Defaults to 0.0.
 * @param offset_y Y position offset in [m]. Defaults to 0.0.
 * @return Created a `PointCloud<pcl::PointXYZ>` object.
 *
 * @note This function is deprecated with `test/shape_estimation/test_shape_estimator.cpp`.
 */
PointCloud2 generate_cluster(
  double length, double width, double height, double yaw = 0.0, double offset_x = 0.0,
  double offset_y = 0.0)
{
  constexpr double x_step = 0.4;
  constexpr double y_step = 0.2;

  pcl::PointCloud<pcl::PointXYZ> cluster;
  for (double x = -0.5 * length; x < 0.5 * length; x += x_step) {
    cluster.emplace_back(x, 0.5 * width, 0.0);
  }
  for (double y = -0.5 * width; y < 0.5 * width; y += y_step) {
    cluster.emplace_back(-0.5 * length, y, 0.0);
  }
  cluster.emplace_back(0.5 * length, -0.5 * width, 0.0);
  cluster.emplace_back(0.5 * length, 0.5 * width, 0.0);
  cluster.emplace_back(-0.5 * length, -0.5 * width, 0.0);
  cluster.emplace_back(-0.5 * length, 0.5 * width, 0.0);
  cluster.emplace_back(0.0, 0.0, height);

  if (yaw != 0.0 || offset_x != 0.0 || offset_y != 0.0) {
    for (auto & point : cluster) {
      const double x = point.x;
      const double y = point.y;
      // rotate
      point.x = x * cos(yaw) - y * sin(yaw);
      point.y = x * sin(yaw) + y * cos(yaw);
      // offset
      point.x += offset_x;
      point.y += offset_y;
    }
  }

  PointCloud2 output;
  pcl::toROSMsg(cluster, output);
  return output;
}

/**
 * @brief Return a new `tier4_perception_msgs::msg::DetectedObjectsWithFeature` instance.
 *
 * @param stamp ROS timestamp.
 * @param as_empty Whether to return a object without any cluster.
 * @return `tier4_perception_msgs::msg::DetectedObjectsWithFeature`.
 */
DetectedObjectsWithFeature generate_feature_objects(const rclcpp::Time & stamp, bool as_empty)
{
  constexpr double length = 4.0;
  constexpr double width = 2.0;
  constexpr double height = 1.0;

  DetectedObjectsWithFeature output;
  output.header.frame_id = "base_link";
  output.header.stamp = stamp;

  if (!as_empty) {
    DetectedObjectWithFeature object;
    object.feature.cluster = generate_cluster(length, width, height);
    output.feature_objects.emplace_back(object);
  }

  return output;
}
}  // namespace

/**
 * Test `ShapeEstimationNode`.
 *
 * This test case checks whether the node works with the arbitrary input.
 */
TEST(ShapeEstimationNode, testShapeEstimationNodeWithArbitraryCluster)
{
  const std::string input_topic = "/shape_estimation/input";
  const std::string output_topic = "/shape_estimation/output";
  auto test_manager = generate_test_manager();
  auto node = generate_node(input_topic, output_topic);

  // set output subscriber
  int counter = 0;
  auto callback = [&counter](const DetectedObjectsWithFeature::ConstSharedPtr) { ++counter; };
  test_manager->set_subscriber<DetectedObjectsWithFeature>(output_topic, callback);

  // prepare input
  auto stamp = node->get_clock()->now();
  constexpr bool as_empty = false;
  auto input = generate_feature_objects(stamp, as_empty);
  test_manager->test_pub_msg<DetectedObjectsWithFeature>(node, input_topic, input);

  // check if the node worked
  EXPECT_EQ(1, counter);
}

/**
 * Test `ShapeEstimationNode`.
 *
 * This test case checks whether the node works with the empty cluster input.
 */
TEST(ShapeEstimationNode, testShapeEstimationNodeWithEmptyCluster)
{
  const std::string input_topic = "/shape_estimation/input";
  const std::string output_topic = "/shape_estimation/output";
  auto test_manager = generate_test_manager();
  auto node = generate_node(input_topic, output_topic);

  // set output subscriber
  int counter = 0;
  auto callback = [&counter](const DetectedObjectsWithFeature::ConstSharedPtr) { ++counter; };
  test_manager->set_subscriber<DetectedObjectsWithFeature>(output_topic, callback);

  // prepare input
  auto stamp = node->get_clock()->now();
  constexpr bool as_empty = true;
  auto input = generate_feature_objects(stamp, as_empty);
  test_manager->test_pub_msg<DetectedObjectsWithFeature>(node, input_topic, input);

  // check if the node worked
  EXPECT_EQ(1, counter);
}

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

#include "detected_object_feature_remover_node.hpp"

#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/time.hpp>

#include "tier4_perception_msgs/msg/detail/detected_object_with_feature__struct.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

namespace
{
using autoware::detected_object_feature_remover::DetectedObjectFeatureRemover;
using autoware::test_utils::AutowareTestManager;
using autoware_perception_msgs::msg::DetectedObject;
using autoware_perception_msgs::msg::DetectedObjects;
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
 * @brief Return a new `autoware::detected_object_feature_remover::DetectedObjectFeatureRemover`
 * instance as a shared pointer.
 *
 * @return Shared pointer of
 * `autoware::detected_object_feature_remover::DetectedObjectFeatureRemover`.
 */
std::shared_ptr<DetectedObjectFeatureRemover> generate_node(
  const std::string & input_topic, const std::string & output_topic)
{
  auto node_options = rclcpp::NodeOptions{};
  node_options.arguments(
    {"--ros-args", "-r", "~/input:=" + input_topic, "-r", "~/output:=" + output_topic});

  return std::make_shared<DetectedObjectFeatureRemover>(node_options);
}

/**
 * @brief Return a new `autoware_perception_msgs::msg::DetectedObject` instance.
 *
 * @return New instance of `autoware_perception_msgs::msg::DetectedObject`.
 */
DetectedObject generate_detected_object()
{
  DetectedObject output;
  output.kinematics.pose_with_covariance.pose.position.x = 1.0;
  output.kinematics.pose_with_covariance.pose.position.y = 2.0;
  output.kinematics.pose_with_covariance.pose.position.z = 3.0;
  output.kinematics.pose_with_covariance.pose.orientation.x = 0.0;
  output.kinematics.pose_with_covariance.pose.orientation.y = 0.0;
  output.kinematics.pose_with_covariance.pose.orientation.z = 0.0;
  output.kinematics.pose_with_covariance.pose.orientation.w = 1.0;
  return output;
}

/**
 * @brief Return a new `tier4_perception_msgs::msg::DetectedObjectWithFeature` instance.
 * @details Return a object including a single object created by `generate_detected_object`.
 *
 * @return New instance of `tier4_perception_msgs::msg::DetectedObjectWithFeature`.
 */
DetectedObjectWithFeature generate_feature_object()
{
  DetectedObjectWithFeature output;
  output.object = generate_detected_object();
  return output;
}

/**
 * @brief Return a new `tier4_perception_msgs::msg::DetectedObjectsWithFeature` instance.
 * @details If `as_emtpy=true`, returns a instance without any objects. Otherwise, returns a
 * instance including a single object created by `generate_feature_object`.
 *
 * @param stamp Timestamp of the message.
 * @param as_empty Whether to return a instance without any objects.
 * @return New instance of `tier4_perception_mgs::msg::DetectedObjectsWIthFeature`.
 */
DetectedObjectsWithFeature generate_feature_objects(const rclcpp::Time & stamp, bool as_empty)
{
  DetectedObjectsWithFeature output;
  output.header.frame_id = "base_link";
  output.header.stamp = stamp;
  if (!as_empty) {
    auto object = generate_feature_object();
    output.feature_objects.emplace_back(object);
  }
  return output;
}
}  // namespace

/**
 * Test suite of DetectedObjectFeatureRemover.
 *
 * This test case checks whether the node works if the arbitrary object is input.
 */
TEST(FeatureRemoverTest, TestArbitraryObject)
{
  const std::string input_topic = "/detected_object_feature_remover/input";
  const std::string output_topic = "/detected_object_feature_remover/output";
  auto test_manager = generate_test_manager();
  auto node = generate_node(input_topic, output_topic);

  // set output subscriber
  DetectedObjects output;
  auto callback = [&output](const DetectedObjects::ConstSharedPtr msg) {
    output.header = msg->header;
    output.objects = msg->objects;
  };
  test_manager->set_subscriber<DetectedObjects>(output_topic, callback);

  // prepare input
  auto stamp = node->get_clock()->now();
  constexpr bool as_empty = false;
  auto input = generate_feature_objects(stamp, as_empty);
  test_manager->test_pub_msg<DetectedObjectsWithFeature>(node, input_topic, input);

  // Check output
  auto expect = generate_detected_object();
  EXPECT_EQ(1, output.objects.size());
  EXPECT_EQ(expect, output.objects.front());
}

/**
 * Test suite of DetectedObjectFeatureRemover.
 *
 * This test case checks whether the node works if the empty object is input.
 */
TEST(FeatureRemoverTest, TestEmptyObject)
{
  const std::string input_topic = "/detected_object_feature_remover/input";
  const std::string output_topic = "/detected_object_feature_remover/output";
  auto test_manager = generate_test_manager();
  auto node = generate_node(input_topic, output_topic);

  // set output subscriber
  DetectedObjects output;
  auto callback = [&output](const DetectedObjects::ConstSharedPtr msg) {
    output.header = msg->header;
    output.objects = msg->objects;
  };
  test_manager->set_subscriber<DetectedObjects>(output_topic, callback);

  // prepare input
  auto stamp = node->get_clock()->now();
  constexpr bool as_empty = true;
  auto input = generate_feature_objects(stamp, as_empty);
  test_manager->test_pub_msg<DetectedObjectsWithFeature>(node, input_topic, input);

  // Check output
  EXPECT_EQ(0, output.objects.size());
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);
  bool result = RUN_ALL_TESTS();
  rclcpp::shutdown();
  return result;
}

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

#include <rclcpp/rclcpp.hpp>
#include <tier4_autoware_utils/ros/published_time_publisher.hpp>

#include <autoware_internal_msgs/msg/published_time.hpp>
#include <std_msgs/msg/header.hpp>

#include <gtest/gtest.h>

class PublishedTimePublisherTest : public ::testing::Test
{
protected:
  std::shared_ptr<rclcpp::Node> node_{nullptr};
  std::shared_ptr<tier4_autoware_utils::PublishedTimePublisher> published_time_publisher_{nullptr};
  std::shared_ptr<rclcpp::Publisher<std_msgs::msg::Header>> test_publisher_{nullptr};
  std::shared_ptr<rclcpp::Subscription<autoware_internal_msgs::msg::PublishedTime>>
    test_subscriber_{nullptr};
  autoware_internal_msgs::msg::PublishedTime::ConstSharedPtr published_time_{nullptr};

  void SetUp() override
  {
    ASSERT_TRUE(rclcpp::ok());

    // Create a node
    node_ = std::make_shared<rclcpp::Node>("PublishedTimePublisher_test_node");
    // Create a publisher
    test_publisher_ =
      node_->create_publisher<std_msgs::msg::Header>("PublishedTimePublisher_test_topic", 1);
    // Create a PublishedTimePublisher
    published_time_publisher_ =
      std::make_shared<tier4_autoware_utils::PublishedTimePublisher>(node_.get());
    // Create a subscriber
    test_subscriber_ = node_->create_subscription<autoware_internal_msgs::msg::PublishedTime>(
      "PublishedTimePublisher_test_topic/debug/published_time", 1,
      [this](autoware_internal_msgs::msg::PublishedTime::SharedPtr msg) {
        this->published_time_ = msg;
      });
    rclcpp::spin_some(node_);
  }

  void TearDown() override {}
};

TEST_F(PublishedTimePublisherTest, PublishMsgWithHeader)
{
  std_msgs::msg::Header header;
  header.stamp = rclcpp::Time(1234);

  // Use Published Time Publisher with a timestamp
  published_time_publisher_->publish(test_publisher_, header);
  rclcpp::spin_some(node_);

  // Check if the published time is the same as the header
  EXPECT_EQ(published_time_->header.stamp, header.stamp);
}

TEST_F(PublishedTimePublisherTest, PublishMsgWithTimestamp)
{
  std_msgs::msg::Header header;
  header.stamp = rclcpp::Time(4321);

  // Use Published Time Publisher with a timestamp
  published_time_publisher_->publish(test_publisher_, header.stamp);
  rclcpp::spin_some(node_);

  // Check if the published time is the same as the header
  EXPECT_EQ(published_time_->header.stamp, header.stamp);
}

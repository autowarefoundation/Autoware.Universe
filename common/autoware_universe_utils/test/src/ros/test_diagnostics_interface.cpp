// Copyright 2024 Autoware Foundation
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

#include "autoware/universe_utils/ros/diagnostics_interface.hpp"

#include <rclcpp/rclcpp.hpp>

#include <diagnostic_msgs/msg/diagnostic_array.hpp>
#include <diagnostic_msgs/msg/diagnostic_status.hpp>
#include <diagnostic_msgs/msg/key_value.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <string>

using autoware::universe_utils::DiagnosticsInterface;

class TestDiagnosticsInterface : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a test node
    node_ = std::make_shared<rclcpp::Node>("test_diagnostics_interface");
  }

  // Automatically destroyed at the end of each test
  std::shared_ptr<rclcpp::Node> node_;
};

/*
 * Test clear():
 * Verify that calling clear() resets DiagnosticStatus values, level, and message.
 */
TEST_F(TestDiagnosticsInterface, ClearTest)
{
  DiagnosticsInterface module(node_.get(), "test_diagnostic");

  // Add some key-value pairs and update level/message
  module.add_key_value("param1", 42);
  module.update_level_and_message(
    diagnostic_msgs::msg::DiagnosticStatus::WARN, "Something is not OK");

  // Call clear()
  module.clear();

  // After calling clear(), publish and check the contents of the message
  diagnostic_msgs::msg::DiagnosticArray::ConstSharedPtr received_msg;
  auto sub = node_->create_subscription<diagnostic_msgs::msg::DiagnosticArray>(
    "/diagnostics", 10,
    [&](diagnostic_msgs::msg::DiagnosticArray::ConstSharedPtr msg) { received_msg = msg; });

  // Publish the message
  module.publish(node_->now());

  // Spin to allow the subscriber to receive messages
  rclcpp::spin_some(node_);

  ASSERT_NE(nullptr, received_msg);
  ASSERT_FALSE(received_msg->status.empty());
  const auto & status = received_msg->status.front();

  // After clear(), key-value pairs should be empty
  EXPECT_TRUE(status.values.empty());

  // After clear(), level should be OK (=0)
  EXPECT_EQ(status.level, diagnostic_msgs::msg::DiagnosticStatus::OK);

  // After clear(), message is empty internally,
  // but "OK" is set during publishing when level == OK
  EXPECT_EQ(status.message, "OK");
}

/*
 * Test add_key_value():
 * Verify that adding the same key updates its value instead of adding a duplicate.
 */
TEST_F(TestDiagnosticsInterface, AddKeyValueTest)
{
  DiagnosticsInterface module(node_.get(), "test_diagnostic");

  // Call the template version of add_key_value() with different types
  module.add_key_value("string_key", std::string("initial_value"));
  module.add_key_value("int_key", 123);
  module.add_key_value("bool_key", true);

  // Overwrite the value for "string_key"
  module.add_key_value("string_key", std::string("updated_value"));

  // Capture the published message to verify its contents
  diagnostic_msgs::msg::DiagnosticArray::ConstSharedPtr received_msg;
  auto sub = node_->create_subscription<diagnostic_msgs::msg::DiagnosticArray>(
    "/diagnostics", 10,
    [&](diagnostic_msgs::msg::DiagnosticArray::ConstSharedPtr msg) { received_msg = msg; });
  module.publish(node_->now());
  rclcpp::spin_some(node_);

  ASSERT_NE(nullptr, received_msg);
  ASSERT_FALSE(received_msg->status.empty());
  const auto & status = received_msg->status.front();

  // Expect 3 key-value pairs
  ASSERT_EQ(status.values.size(), 3U);

  // Helper lambda to find a value by key
  auto find_value = [&](const std::string & key) -> std::string {
    for (const auto & kv : status.values) {
      if (kv.key == key) {
        return kv.value;
      }
    }
    return "";
  };

  EXPECT_EQ(find_value("string_key"), "updated_value");
  EXPECT_EQ(find_value("int_key"), "123");
  EXPECT_EQ(find_value("bool_key"), "True");

  // Status level should still be OK
  EXPECT_EQ(status.level, diagnostic_msgs::msg::DiagnosticStatus::OK);
  // Message should be "OK"
  EXPECT_EQ(status.message, "OK");
}

/*
 * Test update_level_and_message():
 * Verify that the level is updated to the highest severity and
 * that messages are concatenated if level > OK.
 */
TEST_F(TestDiagnosticsInterface, UpdateLevelAndMessageTest)
{
  DiagnosticsInterface module(node_.get(), "test_diagnostic");

  // Initial status is level=OK(0), message=""
  module.update_level_and_message(diagnostic_msgs::msg::DiagnosticStatus::OK, "Initial OK");
  // Update to WARN (1)
  module.update_level_and_message(diagnostic_msgs::msg::DiagnosticStatus::WARN, "Low battery");
  // Update to ERROR (2)
  module.update_level_and_message(diagnostic_msgs::msg::DiagnosticStatus::ERROR, "Sensor failure");
  // Another WARN (1) after ERROR should not downgrade the overall level
  module.update_level_and_message(
    diagnostic_msgs::msg::DiagnosticStatus::WARN, "Should not override error");

  diagnostic_msgs::msg::DiagnosticArray::ConstSharedPtr received_msg;
  auto sub = node_->create_subscription<diagnostic_msgs::msg::DiagnosticArray>(
    "/diagnostics", 10,
    [&](diagnostic_msgs::msg::DiagnosticArray::ConstSharedPtr msg) { received_msg = msg; });

  module.publish(node_->now());
  rclcpp::spin_some(node_);

  ASSERT_NE(nullptr, received_msg);
  ASSERT_FALSE(received_msg->status.empty());
  const auto & status = received_msg->status.front();

  // Final level should be ERROR (2)
  EXPECT_EQ(status.level, diagnostic_msgs::msg::DiagnosticStatus::ERROR);

  // The message should contain all parts that were added when level > OK.
  // Thus, we expect something like:
  // "Low battery; Sensor failure; Should not override error"
  const std::string & final_message = status.message;
  EXPECT_FALSE(final_message.find("Initial OK") != std::string::npos);
  EXPECT_TRUE(final_message.find("Low battery") != std::string::npos);
  EXPECT_TRUE(final_message.find("Sensor failure") != std::string::npos);
  EXPECT_TRUE(final_message.find("Should not override error") != std::string::npos);
}

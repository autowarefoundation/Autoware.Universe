// Copyright 2023 The Autoware Contributors
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
#include "service_log_checker.hpp"

#include <rclcpp/node_options.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <string>

using ServiceLog = tier4_system_msgs::msg::ServiceLog;
using DiagnosticArray = diagnostic_msgs::msg::DiagnosticArray;
using ServiceLogChecker = autoware::component_interface_tools::ServiceLogChecker;

TEST(ServiceCheckerTest, ServiceChecker)
{
  class PubManager : public rclcpp::Node
  {
  public:
    PubManager() : Node("test_pub_node")
    {
      pub_odom_ = create_publisher<ServiceLog>("service_log", 1);
      sub_odom_ = create_subscription<DiagnosticArray>(
        "/diagnostics", 1, std::bind(&PubManager::on_service_log, this, std::placeholders::_1));
    }
    rclcpp::Publisher<ServiceLog>::SharedPtr pub_odom_;
    rclcpp::Subscription<DiagnosticArray>::SharedPtr sub_odom_;
    bool flag = false;
    void on_service_log(const DiagnosticArray::ConstSharedPtr msg)
    {
      if (msg->status.size() > 0) {
        auto diag_array = msg->status[0].message.c_str();
        EXPECT_EQ(diag_array, "ERROR");
        flag = true;
      }
    }
  };

  rclcpp::init(0, nullptr);
  auto node_options = rclcpp::NodeOptions{};
  auto test_target_node = std::make_shared<ServiceLogChecker>(node_options);
  auto test_log = std::make_shared<PubManager>();
  ServiceLog log;
  log.type = 6;
  log.name = "test";
  log.node = "test_node";
  test_log->pub_odom_->publish(log);

  while (!test_log->flag) {
  }
}

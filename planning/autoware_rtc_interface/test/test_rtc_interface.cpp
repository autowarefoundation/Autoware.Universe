// Copyright 2024 TIER IV, Inc. All rights reserved.
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

#include "autoware/rtc_interface/rtc_interface.hpp"

#include <autoware/universe_utils/ros/uuid_helper.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/time.hpp>

#include "tier4_rtc_msgs/msg/detail/command__struct.hpp"

#include <gtest/gtest.h>

using tier4_rtc_msgs::msg::State;

namespace autoware::rtc_interface
{
class RTCInterfaceTest : public ::testing::Test
{
public:
  void update_cooperate_command_status(const std::vector<CooperateCommand> & commands)
  {
    rtc_interface_->updateCooperateCommandStatus(commands);
  }

  std::vector<CooperateCommand> get_stored_command() { return rtc_interface_->stored_commands_; }

protected:
  rclcpp::Node::SharedPtr node;
  std::shared_ptr<autoware::rtc_interface::RTCInterface> rtc_interface_;

  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    node = std::make_shared<rclcpp::Node>("rtc_interface_node_test");
    rtc_interface_ =
      std::make_shared<autoware::rtc_interface::RTCInterface>(node.get(), "TestRTC", false);
  }

  void TearDown() override { rclcpp::shutdown(); }
};

TEST_F(RTCInterfaceTest, uuid_to_string)
{
  auto uuid = autoware::universe_utils::generateUUID();
  rclcpp::Time stamp(1.0, 0);

  // Condition: no registered uuid
  EXPECT_FALSE(rtc_interface_->isRegistered(uuid));
  EXPECT_FALSE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_TRUE(rtc_interface_->isTerminated(uuid));

  // Condition: register uuid with WAITING_FOR_EXECUTION
  rtc_interface_->updateCooperateStatus(
    uuid, true, State::WAITING_FOR_EXECUTION, 10.0, 100.0, stamp);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_TRUE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_FALSE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));

  // Condition: remove registered uuid
  rtc_interface_->removeCooperateStatus(uuid);
  EXPECT_FALSE(rtc_interface_->isRegistered(uuid));

  // Condition: register uuid with FAILED (should start with WAITING_FOR_EXECUTION)
  rtc_interface_->updateCooperateStatus(uuid, true, State::FAILED, 10.0, 100.0, stamp);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_TRUE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_FALSE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));

  // Condition: update uuid as force activated
  std::vector<CooperateCommand> commands;
  CooperateCommand command;
  command.uuid = uuid;
  command.command.type = tier4_rtc_msgs::msg::Command::ACTIVATE;
  commands.push_back(command);
  rtc_interface_->updateCooperateStatus(uuid, false, State::RUNNING, 10.0, 100.0, stamp);
  update_cooperate_command_status(commands);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_TRUE(rtc_interface_->isActivated(uuid));
  EXPECT_TRUE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));

  // Condition: terminate uuid by transiting to SUCCEEDED
  rtc_interface_->updateCooperateStatus(uuid, false, State::SUCCEEDED, 10.0, 100.0, stamp);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_FALSE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_TRUE(rtc_interface_->isTerminated(uuid));

  // Condition: clear registered uuid
  rtc_interface_->clearCooperateStatus();
  EXPECT_FALSE(rtc_interface_->isRegistered(uuid));
  EXPECT_TRUE(get_stored_command().empty());

  // Condition: update uuid as force deactivated
  commands.front().command.type = tier4_rtc_msgs::msg::Command::DEACTIVATE;
  rtc_interface_->updateCooperateStatus(
    uuid, true, State::WAITING_FOR_EXECUTION, 10.0, 100.0, stamp);
  rtc_interface_->updateCooperateStatus(uuid, true, State::RUNNING, 10.0, 100.0, stamp);
  update_cooperate_command_status(commands);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_FALSE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_TRUE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));

  // Condition: uuid transiting to ABORTING
  rtc_interface_->updateCooperateStatus(uuid, true, State::ABORTING, 10.0, 100.0, stamp);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_FALSE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));

  // Condition: uuid transiting to FAILED
  rtc_interface_->updateCooperateStatus(uuid, true, State::FAILED, 10.0, 100.0, stamp);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_FALSE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_TRUE(rtc_interface_->isTerminated(uuid));

  rtc_interface_->clearCooperateStatus();

  // Condition: register uuid with request to operator and transit to RUNNING (which shouldn't be
  // allowed)
  rtc_interface_->updateCooperateStatus(
    uuid, true, State::WAITING_FOR_EXECUTION, 10.0, 100.0, stamp, true);
  rtc_interface_->updateCooperateStatus(uuid, true, State::RUNNING, 10.0, 100.0, stamp, true);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_FALSE(rtc_interface_->isActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_FALSE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));

  // Condition: update uuid cooperate command
  commands.front().command.type = tier4_rtc_msgs::msg::Command::ACTIVATE;
  update_cooperate_command_status(commands);
  EXPECT_TRUE(rtc_interface_->isRegistered(uuid));
  EXPECT_TRUE(rtc_interface_->isActivated(uuid));
  EXPECT_TRUE(rtc_interface_->isForceActivated(uuid));
  EXPECT_FALSE(rtc_interface_->isForceDeactivated(uuid));
  EXPECT_TRUE(rtc_interface_->isRTCEnabled(uuid));
  EXPECT_FALSE(rtc_interface_->isTerminated(uuid));
}
}  // namespace autoware::rtc_interface

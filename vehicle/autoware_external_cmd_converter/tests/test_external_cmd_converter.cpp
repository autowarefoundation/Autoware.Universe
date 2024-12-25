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

#include "autoware_external_cmd_converter/node.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/time.hpp>

#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <string>
using autoware::external_cmd_converter::ExternalCmdConverterNode;
using nav_msgs::msg::Odometry;
using tier4_control_msgs::msg::GateMode;
using GearCommand = autoware_vehicle_msgs::msg::GearCommand;
using autoware_control_msgs::msg::Control;
using ExternalControlCommand = tier4_external_api_msgs::msg::ControlCommandStamped;
using autoware::raw_vehicle_cmd_converter::AccelMap;
using autoware::raw_vehicle_cmd_converter::BrakeMap;

class TestExternalCmdConverter : public ::testing::Test
{
public:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    set_up_node();
  }

  void set_up_node()
  {
    auto node_options = rclcpp::NodeOptions{};
    const auto external_cmd_converter_dir =
      ament_index_cpp::get_package_share_directory("autoware_external_cmd_converter");
    node_options.arguments(
      {"--ros-args", "--params-file",
       external_cmd_converter_dir + "/config/test_external_cmd_converter.param.yaml"});
    external_cmd_converter_ = std::make_shared<ExternalCmdConverterNode>(node_options);
  }

  void TearDown() override
  {
    external_cmd_converter_ = nullptr;
    rclcpp::shutdown();
  }

  /**
   * @brief Test the external command converter's check_remote_topic_rate method under
   * different circumstances:
   * @param received_data wether to simulate if the emergency heartbeat and gate mode messages have
   * been received or not
   * @param is_external set the gate mode to Auto or External
   * @param time_has_passed wether to sleep for a certain time to make sure the timeout period of
   * the function has expired or not.
   */
  bool test_check_remote_topic_rate(bool received_data, bool is_auto, bool time_has_passed)
  {
    if (!received_data) {
      return external_cmd_converter_->check_remote_topic_rate();
    }
    GateMode gate;
    gate.data = (is_auto) ? tier4_control_msgs::msg::GateMode::AUTO
                          : tier4_control_msgs::msg::GateMode::EXTERNAL;
    external_cmd_converter_->current_gate_mode_ = std::make_shared<GateMode>(gate);
    external_cmd_converter_->latest_cmd_received_time_ =
      std::make_shared<rclcpp::Time>(external_cmd_converter_->now());

    if (!time_has_passed) {
      return external_cmd_converter_->check_remote_topic_rate();
    }
    const int sleep_time = static_cast<int>(external_cmd_converter_->control_command_timeout_) + 1;
    std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
    return external_cmd_converter_->check_remote_topic_rate();
  }

  /**
   * @brief Test the external command converter's check_emergency_stop_topic_timeout method under
   * different circumstances:
   * @param received_data wether to simulate if the emergency heartbeat and gate mode messages have
   * been received or not
   * @param is_auto set the gate mode to Auto or External
   * @param time_has_passed wether to sleep for a certain time to make sure the timeout period of
   * the function has expired or not.
   */
  bool test_check_emergency_stop_topic_timeout(
    bool received_data, bool is_auto, bool time_has_passed)
  {
    if (!received_data) {
      return external_cmd_converter_->check_emergency_stop_topic_timeout();
    }
    tier4_external_api_msgs::msg::Heartbeat::ConstSharedPtr msg;
    external_cmd_converter_->on_emergency_stop_heartbeat(msg);
    GateMode gate;
    gate.data = (is_auto) ? tier4_control_msgs::msg::GateMode::AUTO
                          : tier4_control_msgs::msg::GateMode::EXTERNAL;
    external_cmd_converter_->current_gate_mode_ = std::make_shared<GateMode>(gate);

    const int sleep_time = static_cast<int>(external_cmd_converter_->emergency_stop_timeout_) + 1;
    if (time_has_passed) std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
    return external_cmd_converter_->check_emergency_stop_topic_timeout();
  }

  /**
   * @brief Test the external command converter's get_shift_velocity_sign. This function is a
   * wrapper to call the converter's function during the test and does nothing but pass the command
   * to the function.
   * @param cmd the command to be passed to the get_shift_velocity_sign member function.
   */
  double test_get_shift_velocity_sign(const GearCommand & cmd)
  {
    return external_cmd_converter_->get_shift_velocity_sign(cmd);
  }

  /**
   * @brief Helper function to initialize the accel and brake maps for testing.
   */
  void initialize_maps(const std::string & accel_map, const std::string & brake_map)
  {
    external_cmd_converter_->accel_map_.readAccelMapFromCSV(accel_map);
    external_cmd_converter_->brake_map_.readBrakeMapFromCSV(brake_map);
    external_cmd_converter_->acc_map_initialized_ = true;
  }

  /**
   * @brief Test the external command converter's calculate_acc. This function is a
   * wrapper to call the converter's function during the test and does nothing but pass the command
   * to the function.
   * @param cmd the command to be passed to the calculate_acc member function.
   * @param vel the velocity to be passed to the calculate_acc member function.
   */
  double test_calculate_acc(

    const ExternalControlCommand & cmd, const double vel)
  {
    return external_cmd_converter_->calculate_acc(cmd, vel);
  }

  std::shared_ptr<ExternalCmdConverterNode> external_cmd_converter_;
};

/**
 * @brief Test the external command converter's check_emergency_stop_topic_timeout method under
 * different circumstances by setting different combinations of conditions for the function
 * execution. the test_check_emergency_stop_topic_timeout() function's brief should be read for
 * further details.
 */
TEST_F(TestExternalCmdConverter, testCheckEmergencyStopTimeout)
{
  EXPECT_TRUE(test_check_emergency_stop_topic_timeout(false, false, false));
  EXPECT_FALSE(test_check_emergency_stop_topic_timeout(true, true, false));
  EXPECT_TRUE(test_check_emergency_stop_topic_timeout(true, false, false));
  EXPECT_FALSE(test_check_emergency_stop_topic_timeout(true, false, true));
}

/**
 * @brief Test the external command converter's check_remote_topic_rate method under
 * different circumstances by setting different combinations of conditions for the function
 * execution. the test_check_remote_topic_rate() function's brief should be read for further
 * details.
 */
TEST_F(TestExternalCmdConverter, testRemoteTopicRate)
{
  EXPECT_TRUE(test_check_remote_topic_rate(false, false, false));
  EXPECT_TRUE(test_check_remote_topic_rate(true, true, false));
  EXPECT_TRUE(test_check_remote_topic_rate(true, true, true));
  EXPECT_FALSE(test_check_remote_topic_rate(true, false, true));
}

/**
 * @brief Test the external command converter's get_shift_velocity_sign function. It tests different
 * combinations of input commands for the functions.
 */
TEST_F(TestExternalCmdConverter, testGetShiftVelocitySign)
{
  GearCommand cmd;
  cmd.command = GearCommand::DRIVE;
  EXPECT_DOUBLE_EQ(1.0, test_get_shift_velocity_sign(cmd));
  cmd.command = GearCommand::LOW;
  EXPECT_DOUBLE_EQ(1.0, test_get_shift_velocity_sign(cmd));
  cmd.command = GearCommand::REVERSE;
  EXPECT_DOUBLE_EQ(-1.0, test_get_shift_velocity_sign(cmd));
  cmd.command = GearCommand::LOW_2;
  EXPECT_DOUBLE_EQ(0.0, test_get_shift_velocity_sign(cmd));
}

/**
 * @brief Test the external command converter's calculate_acc function. It tests different
 * combinations of input commands and velocities for the functions.
 */
TEST_F(TestExternalCmdConverter, testCalculateAcc)
{
  const auto map_dir =
    ament_index_cpp::get_package_share_directory("autoware_raw_vehicle_cmd_converter");
  const auto accel_map_path = map_dir + "/data/default/accel_map.csv";
  const auto brake_map_path = map_dir + "/data/default/brake_map.csv";
  initialize_maps(accel_map_path, brake_map_path);

  // test standard cases
  {
    ExternalControlCommand cmd;
    cmd.control.throttle = 1.0;
    cmd.control.brake = 0.0;
    double vel = 0.0;
    EXPECT_TRUE(test_calculate_acc(cmd, vel) > 0.0);
    cmd.control.throttle = 0.0;
    cmd.control.brake = 1.0;
    EXPECT_TRUE(test_calculate_acc(cmd, vel) < 0.0);

    cmd.control.throttle = 1.0;
    cmd.control.brake = 0.5;
    vel = 1.0;
    EXPECT_TRUE(test_calculate_acc(cmd, vel) > 0.0);
    cmd.control.throttle = 0.5;
    cmd.control.brake = 1.0;
    EXPECT_TRUE(test_calculate_acc(cmd, vel) < 0.0);
  }
  // test border cases
  {
    // input brake or throttle are infinite. finite velocity.
    ExternalControlCommand cmd;
    cmd.control.throttle = std::numeric_limits<double>::infinity();
    cmd.control.brake = 0.0;
    double vel = 0.0;
    double acc = test_calculate_acc(cmd, vel);
    EXPECT_DOUBLE_EQ(acc, 0.0);
    cmd.control.throttle = 0.0;
    cmd.control.brake = std::numeric_limits<double>::infinity();
    acc = test_calculate_acc(cmd, vel);
    EXPECT_DOUBLE_EQ(acc, 0.0);

    // input brake or throttle are infinite. velocity is infinite.
    // Check that acc is not NaN and not infinite

    vel = std::numeric_limits<double>::infinity();
    cmd.control.throttle = std::numeric_limits<double>::infinity();
    cmd.control.brake = 0.0;
    acc = test_calculate_acc(cmd, vel);
    EXPECT_FALSE(std::isnan(acc));
    EXPECT_FALSE(std::isinf(acc));
    EXPECT_DOUBLE_EQ(acc, 0.0);
    cmd.control.throttle = 0.0;
    cmd.control.brake = std::numeric_limits<double>::infinity();
    acc = test_calculate_acc(cmd, vel);
    EXPECT_FALSE(std::isnan(acc));
    EXPECT_FALSE(std::isinf(acc));
    EXPECT_DOUBLE_EQ(acc, 0.0);

    cmd.control.throttle = std::numeric_limits<double>::infinity();
    cmd.control.brake = std::numeric_limits<double>::infinity();
    acc = test_calculate_acc(cmd, vel);
    EXPECT_FALSE(std::isnan(acc));
    EXPECT_FALSE(std::isinf(acc));
    EXPECT_DOUBLE_EQ(acc, 0.0);

    cmd.control.throttle = std::numeric_limits<double>::quiet_NaN();
    cmd.control.brake = std::numeric_limits<double>::quiet_NaN();
    acc = test_calculate_acc(cmd, vel);
    EXPECT_FALSE(std::isnan(acc));
    EXPECT_FALSE(std::isinf(acc));
    EXPECT_DOUBLE_EQ(acc, 0.0);

    cmd.control.throttle = std::numeric_limits<double>::signaling_NaN();
    cmd.control.brake = std::numeric_limits<double>::signaling_NaN();
    acc = test_calculate_acc(cmd, vel);
    EXPECT_FALSE(std::isnan(acc));
    EXPECT_FALSE(std::isinf(acc));
    EXPECT_DOUBLE_EQ(acc, 0.0);
  }
}

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

#include "../src/node.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_planning_test_manager/autoware_planning_test_manager.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <utility>

namespace autoware::surround_obstacle_checker
{
auto generateTestTargetNode() -> std::shared_ptr<SurroundObstacleCheckerNode>
{
  rclcpp::init(0, nullptr);

  auto node_options = rclcpp::NodeOptions{};
  const auto autoware_test_utils_dir =
    ament_index_cpp::get_package_share_directory("autoware_test_utils");

  autoware::test_utils::updateNodeOptions(
    node_options,
    {autoware_test_utils_dir + "/config/test_common.param.yaml",
     autoware_test_utils_dir + "/config/test_nearest_search.param.yaml",
     autoware_test_utils_dir + "/config/test_vehicle_info.param.yaml",
     ament_index_cpp::get_package_share_directory("autoware_surround_obstacle_checker") +
       "/config/surround_obstacle_checker.param.yaml"});

  return std::make_shared<SurroundObstacleCheckerNode>(node_options);
}

class SurroundObstacleCheckerNodeTest : public ::testing::Test
{
public:
  SurroundObstacleCheckerNodeTest() : node_{generateTestTargetNode()} {}

  auto isStopRequired(
    const bool is_obstacle_found, const bool is_vehicle_stopped, const State & state,
    const std::optional<rclcpp::Time> & last_obstacle_found_time, const double time_threshold) const
    -> std::pair<bool, std::optional<rclcpp::Time>>
  {
    return node_->isStopRequired(
      is_obstacle_found, is_vehicle_stopped, state, last_obstacle_found_time, time_threshold);
  }

private:
  std::shared_ptr<SurroundObstacleCheckerNode> node_;
};

TEST_F(SurroundObstacleCheckerNodeTest, isStopRequired)
{
  const auto LAST_STOP_TIME = rclcpp::Clock{RCL_ROS_TIME}.now();

  using namespace std::literals::chrono_literals;
  rclcpp::sleep_for(500ms);

  {
    constexpr double THRESHOLD = 1.0;
    const auto [is_stop, stop_time] =
      isStopRequired(false, false, State::STOP, LAST_STOP_TIME, THRESHOLD);
    EXPECT_FALSE(is_stop);
    EXPECT_EQ(stop_time, std::nullopt);
  }

  {
    constexpr double THRESHOLD = 1.0;
    const auto [is_stop, stop_time] =
      isStopRequired(false, true, State::PASS, LAST_STOP_TIME, THRESHOLD);
    EXPECT_FALSE(is_stop);
    EXPECT_EQ(stop_time, std::nullopt);
  }

  {
    constexpr double THRESHOLD = 1.0;
    const auto [is_stop, stop_time] =
      isStopRequired(true, true, State::STOP, LAST_STOP_TIME, THRESHOLD);

    ASSERT_TRUE(stop_time.has_value());

    const auto time_diff = rclcpp::Clock{RCL_ROS_TIME}.now() - stop_time.value();

    EXPECT_TRUE(is_stop);
    EXPECT_NEAR(time_diff.seconds(), 0.0, 1e-3);
  }

  {
    constexpr double THRESHOLD = 1.0;
    const auto [is_stop, stop_time] =
      isStopRequired(false, true, State::STOP, LAST_STOP_TIME, THRESHOLD);

    ASSERT_TRUE(stop_time.has_value());

    const auto time_diff = rclcpp::Clock{RCL_ROS_TIME}.now() - stop_time.value();

    EXPECT_TRUE(is_stop);
    EXPECT_NEAR(time_diff.seconds(), 0.5, 1e-3);
  }

  {
    constexpr double THRESHOLD = 0.25;
    const auto [is_stop, stop_time] =
      isStopRequired(false, true, State::STOP, LAST_STOP_TIME, THRESHOLD);
    EXPECT_FALSE(is_stop);
    EXPECT_EQ(stop_time, std::nullopt);
  }

  {
    constexpr double THRESHOLD = 1.0;
    const auto [is_stop, stop_time] =
      isStopRequired(false, true, State::STOP, std::nullopt, THRESHOLD);
    EXPECT_FALSE(is_stop);
    EXPECT_EQ(stop_time, std::nullopt);
  }

  rclcpp::shutdown();
}
}  // namespace autoware::surround_obstacle_checker

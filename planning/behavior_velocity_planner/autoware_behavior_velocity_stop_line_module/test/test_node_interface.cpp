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

#include <autoware/behavior_velocity_planner/test_utils.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace autoware::behavior_velocity_planner
{
TEST(PlanningModuleInterfaceTest, NodeTestWithExceptionPathWithLaneID)
{
  rclcpp::init(0, nullptr);
  auto test_manager = autoware::behavior_velocity_planner::generateTestManager();
  auto test_target_node = autoware::behavior_velocity_planner::generateNode({});

  autoware::behavior_velocity_planner::publishMandatoryTopics(test_manager, test_target_node);

  // test with nominal path_with_lane_id
  ASSERT_NO_THROW_WITH_ERROR_MSG(test_manager->testWithNominalPathWithLaneId(test_target_node));
  EXPECT_GE(test_manager->getReceivedTopicNum(), 1);

  // test with empty path_with_lane_id
  ASSERT_NO_THROW_WITH_ERROR_MSG(test_manager->testWithAbnormalPathWithLaneId(test_target_node));
  rclcpp::shutdown();
}

TEST(PlanningModuleInterfaceTest, NodeTestWithOffTrackEgoPose)
{
  rclcpp::init(0, nullptr);

  const auto plugin_info_vec = {autoware::behavior_velocity_planner::PluginInfo{
    "stop_line", "autoware::behavior_velocity_planner::StopLineModulePlugin"}};

  auto test_manager = autoware::behavior_velocity_planner::generateTestManager();
  auto test_target_node = autoware::behavior_velocity_planner::generateNode(plugin_info_vec);
  autoware::behavior_velocity_planner::publishMandatoryTopics(test_manager, test_target_node);

  // test for normal trajectory
  ASSERT_NO_THROW_WITH_ERROR_MSG(test_manager->testWithNominalPathWithLaneId(test_target_node));

  // make sure behavior_path_planner is running
  EXPECT_GE(test_manager->getReceivedTopicNum(), 1);

  ASSERT_NO_THROW_WITH_ERROR_MSG(test_manager->testOffTrackFromPathWithLaneId(test_target_node));

  rclcpp::shutdown();
}
}  // namespace autoware::behavior_velocity_planner

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

#include "dynamic_obstacle.hpp"
#include "state_machine.hpp"
#include "utils.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/clock.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/time.hpp>

#include <tier4_planning_msgs/msg/detail/path_point_with_lane_id__struct.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

using autoware::behavior_velocity_planner::DynamicObstacle;
using autoware::behavior_velocity_planner::run_out_utils::StateMachine;
using autoware::behavior_velocity_planner::run_out_utils::StateParam;

using geometry_msgs::msg::Point;
using tier4_planning_msgs::msg::PathPointWithLaneId;
using tier4_planning_msgs::msg::PathWithLaneId;

class TestStateMachine : public ::testing::Test
{
  void SetUp() override
  {
    StateParam state_params;
    state_params.stop_thresh = stop_thresh_;
    state_params.stop_time_thresh = stop_time_thresh_;
    state_params.disable_approach_dist = disable_approach_dist_;
    state_params.keep_approach_duration = keep_approach_duration_;
    state_machine_ptr_ = std::make_shared<StateMachine>(state_params);
  }

public:
  std::shared_ptr<StateMachine> state_machine_ptr_;
  float stop_thresh_{2.0};
  float stop_time_thresh_{1.0};
  float keep_approach_duration_{1.0};
  float disable_approach_dist_{1.0};
};

TEST_F(TestStateMachine, testToString)
{
  using State = autoware::behavior_velocity_planner::run_out_utils::StateMachine::State;
  State state = state_machine_ptr_->getCurrentState();

  auto state_string = state_machine_ptr_->toString(state);
  EXPECT_EQ(state_string, "GO");

  state = State::STOP;
  state_string = state_machine_ptr_->toString(state);
  EXPECT_EQ(state_string, "STOP");

  state = State::APPROACH;
  state_string = state_machine_ptr_->toString(state);
  EXPECT_EQ(state_string, "APPROACH");

  state = State::UNKNOWN;
  state_string = state_machine_ptr_->toString(state);
  EXPECT_EQ(state_string, "UNKNOWN");
}

TEST_F(TestStateMachine, testUpdateState)
{
  rclcpp::init(0, nullptr);
  using State = autoware::behavior_velocity_planner::run_out_utils::StateMachine::State;
  using StateInput = autoware::behavior_velocity_planner::run_out_utils::StateMachine::StateInput;
  constexpr float dist_to_collision{10.0};
  StateInput state_input{stop_thresh_ / 2.0, dist_to_collision, {}};
  state_input.current_obstacle = DynamicObstacle();
  rclcpp::Clock clock(RCL_ROS_TIME);

  EXPECT_TRUE(state_machine_ptr_->getCurrentState() == State::GO);

  // current velocity < stop_threshold. GO -> STOP
  state_machine_ptr_->updateState(state_input, clock);
  EXPECT_TRUE(state_machine_ptr_->getCurrentState() == State::STOP);

  // if STOP state continues for a certain time, transit to APPROACH state
  const int sleep_time = static_cast<int>(stop_time_thresh_) + 1;
  std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
  state_machine_ptr_->updateState(state_input, clock);
  EXPECT_TRUE(state_machine_ptr_->getCurrentState() == State::APPROACH);
  rclcpp::shutdown();
}

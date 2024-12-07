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

#include "autoware/freespace_planner/freespace_planner_node.hpp"
#include "autoware/freespace_planner/utils.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>

#include <geometry_msgs/msg/pose.h>
#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <utility>
#include <vector>

using autoware::freespace_planner::FreespacePlannerNode;
using autoware_planning_msgs::msg::Trajectory;
using autoware_planning_msgs::msg::TrajectoryPoint;
using geometry_msgs::msg::Pose;
using nav_msgs::msg::OccupancyGrid;
using nav_msgs::msg::Odometry;

class TestFreespacePlanner : public ::testing::Test
{
public:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    set_up_node();
    set_up_costmap();
    set_up_trajectory();
  }

  void set_up_node()
  {
    auto node_options = rclcpp::NodeOptions{};
    const auto autoware_test_utils_dir =
      ament_index_cpp::get_package_share_directory("autoware_test_utils");
    const auto freespace_planner_dir =
      ament_index_cpp::get_package_share_directory("autoware_freespace_planner");
    node_options.arguments(
      {"--ros-args", "--params-file",
       autoware_test_utils_dir + "/config/test_vehicle_info.param.yaml", "--params-file",
       freespace_planner_dir + "/config/freespace_planner.param.yaml"});
    freespace_planner_ = std::make_shared<FreespacePlannerNode>(node_options);
  }

  void set_up_costmap()
  {
    const size_t width = 80;
    const size_t height = 80;
    const double resolution = 0.5;
    costmap_ = autoware::test_utils::makeCostMapMsg(width, height, resolution);
  }

  void set_up_trajectory()
  {
    trajectory_.points.clear();

    const double y = 0.5 * costmap_.info.height * costmap_.info.resolution;

    // forward trajectory for 20.0 m
    double x = 5.0;
    for (; x < 25.0 + std::numeric_limits<double>::epsilon(); x += costmap_.info.resolution) {
      TrajectoryPoint point;
      point.pose.position.x = x;
      point.pose.position.y = y;
      point.longitudinal_velocity_mps = 1.0;
      trajectory_.points.push_back(point);
    }

    // backward trajectory for 10.0 m
    x = trajectory_.points.back().pose.position.x - costmap_.info.resolution;
    for (; x > 15.0 - std::numeric_limits<double>::epsilon(); x -= costmap_.info.resolution) {
      TrajectoryPoint point;
      point.pose.position.x = x;
      point.pose.position.y = y;
      point.longitudinal_velocity_mps = -1.0;
      trajectory_.points.push_back(point);
    }

    // forward trajectory for 20.0 m
    x = trajectory_.points.back().pose.position.x + costmap_.info.resolution;
    for (; x < 35.0 + std::numeric_limits<double>::epsilon(); x += costmap_.info.resolution) {
      TrajectoryPoint point;
      point.pose.position.x = x;
      point.pose.position.y = y;
      point.longitudinal_velocity_mps = 1.0;
      trajectory_.points.push_back(point);
    }

    reversing_indices = autoware::freespace_planner::utils::get_reversing_indices(trajectory_);
  }

  [[nodiscard]] OccupancyGrid get_blocked_costmap() const
  {
    auto costmap = costmap_;
    const auto block_y = 0.5 * costmap.info.height * costmap.info.resolution;
    const auto block_x = 20.0;
    const int index_x = std::round(block_x / costmap.info.resolution);
    const int index_y = std::round(block_y / costmap.info.resolution);
    const auto id = index_y * costmap.info.width + index_x;
    costmap.data.at(id) = 100;
    return costmap;
  }

  bool test_is_plan_required(
    const bool empty_traj = false, const bool colliding = false, const bool out_of_course = false)
  {
    freespace_planner_->trajectory_ = Trajectory();
    freespace_planner_->partial_trajectory_ = Trajectory();
    freespace_planner_->reversing_indices_.clear();
    freespace_planner_->obs_found_time_ = {};

    freespace_planner_->occupancy_grid_ = std::make_shared<OccupancyGrid>(costmap_);

    if (empty_traj) {
      return freespace_planner_->isPlanRequired();
    }

    freespace_planner_->trajectory_ = trajectory_;
    freespace_planner_->reversing_indices_ = reversing_indices;
    freespace_planner_->partial_trajectory_ =
      autoware::freespace_planner::utils::get_partial_trajectory(
        trajectory_, 0, reversing_indices.front(),
        std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME));
    freespace_planner_->current_pose_.pose = trajectory_.points.front().pose;

    if (colliding) {
      freespace_planner_->obs_found_time_ = freespace_planner_->get_clock()->now();
      freespace_planner_->occupancy_grid_ = std::make_shared<OccupancyGrid>(get_blocked_costmap());
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (out_of_course) {
      freespace_planner_->current_pose_.pose.position.y +=
        (freespace_planner_->node_param_.th_course_out_distance_m + 0.1);
    }

    return freespace_planner_->isPlanRequired();
  }

  std::pair<size_t, size_t> test_update_target_index(
    const bool stopped = false, const bool near_target = false)
  {
    freespace_planner_->trajectory_ = trajectory_;
    freespace_planner_->reversing_indices_ = reversing_indices;
    freespace_planner_->prev_target_index_ = 0;
    freespace_planner_->target_index_ = autoware::freespace_planner::utils::get_next_target_index(
      trajectory_.points.size(), reversing_indices, freespace_planner_->prev_target_index_);
    freespace_planner_->partial_trajectory_ =
      autoware::freespace_planner::utils::get_partial_trajectory(
        trajectory_, freespace_planner_->prev_target_index_, freespace_planner_->target_index_,
        std::make_shared<rclcpp::Clock>(RCL_SYSTEM_TIME));

    Odometry odom;
    odom.pose.pose = trajectory_.points.front().pose;
    odom.twist.twist.linear.x = 1.0;

    if (stopped) odom.twist.twist.linear.x = 0.0;
    if (near_target) odom.pose.pose = trajectory_.points.at(freespace_planner_->target_index_).pose;

    freespace_planner_->odom_buffer_.clear();
    freespace_planner_->odom_ = std::make_shared<Odometry>(odom);
    freespace_planner_->odom_buffer_.push_back(freespace_planner_->odom_);
    freespace_planner_->current_pose_.pose = odom.pose.pose;

    freespace_planner_->updateTargetIndex();

    return {freespace_planner_->prev_target_index_, freespace_planner_->target_index_};
  }

  void TearDown() override
  {
    freespace_planner_ = nullptr;
    trajectory_ = Trajectory();
    costmap_ = OccupancyGrid();
    rclcpp::shutdown();
  }

  std::shared_ptr<FreespacePlannerNode> freespace_planner_;
  OccupancyGrid costmap_;
  Trajectory trajectory_;
  std::vector<size_t> reversing_indices;
};

TEST_F(TestFreespacePlanner, testIsPlanRequired)
{
  EXPECT_FALSE(test_is_plan_required());
  // test with empty current trajectory
  EXPECT_TRUE(test_is_plan_required(true));
  // test with blocked trajectory
  EXPECT_TRUE(test_is_plan_required(false, true));
  // test with deviation from trajectory
  EXPECT_TRUE(test_is_plan_required(false, false, true));
}

TEST_F(TestFreespacePlanner, testUpdateTargetIndex)
{
  size_t prev_target_index, target_index;
  std::tie(prev_target_index, target_index) = test_update_target_index();
  EXPECT_EQ(prev_target_index, 0ul);
  EXPECT_EQ(target_index, reversing_indices.front());

  std::tie(prev_target_index, target_index) = test_update_target_index(true);
  EXPECT_EQ(prev_target_index, 0ul);
  EXPECT_EQ(target_index, reversing_indices.front());

  std::tie(prev_target_index, target_index) = test_update_target_index(false, true);
  EXPECT_EQ(prev_target_index, 0ul);
  EXPECT_EQ(target_index, reversing_indices.front());

  std::tie(prev_target_index, target_index) = test_update_target_index(true, true);
  EXPECT_EQ(prev_target_index, reversing_indices.front());
  EXPECT_EQ(target_index, *std::next(reversing_indices.begin()));
}

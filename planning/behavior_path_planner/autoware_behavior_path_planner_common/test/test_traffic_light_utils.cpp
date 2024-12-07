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

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "autoware/behavior_path_planner_common/data_manager.hpp"
#include "autoware/behavior_path_planner_common/utils/traffic_light_utils.hpp"
#include "autoware_test_utils/autoware_test_utils.hpp"
#include "autoware_test_utils/mock_data_parser.hpp"

#include <autoware/universe_utils/geometry/geometry.hpp>

#include <autoware_perception_msgs/msg/detail/traffic_light_group__struct.hpp>
#include <autoware_planning_msgs/msg/detail/lanelet_route__struct.hpp>
#include <geometry_msgs/msg/detail/pose__struct.hpp>
#include <geometry_msgs/msg/detail/twist__struct.hpp>
#include <tier4_planning_msgs/msg/detail/path_with_lane_id__struct.hpp>

#include <gtest/gtest.h>
#include <lanelet2_core/Forward.h>
#include <yaml-cpp/node/node.h>

#include <limits>
#include <memory>
#include <string>

using autoware::behavior_path_planner::PlannerData;
using autoware::behavior_path_planner::TrafficSignalStamped;
using autoware_perception_msgs::msg::TrafficLightGroupArray;
using autoware_planning_msgs::msg::LaneletRoute;
using geometry_msgs::msg::Pose;

const double epsilon = 1e-06;

class TrafficLightTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    planner_data_ = std::make_shared<PlannerData>();
    const auto test_data_file =
      ament_index_cpp::get_package_share_directory("autoware_behavior_path_planner_common") +
      "/test_data/test_traffic_light.yaml";
    YAML::Node config = YAML::LoadFile(test_data_file);

    set_current_pose(config);
    set_route_handler(config);
    set_traffic_signal(config);
  }

  void set_current_pose(YAML::Node config)
  {
    const auto self_odometry =
      autoware::test_utils::parse<nav_msgs::msg::Odometry>(config["self_odometry"]);
    planner_data_->self_odometry = std::make_shared<const nav_msgs::msg::Odometry>(self_odometry);
  }

  void set_route_handler(YAML::Node config)
  {
    const auto route = autoware::test_utils::parse<LaneletRoute>(config["route"]);
    const auto map_path =
      autoware::test_utils::resolve_pkg_share_uri(config["map_path_uri"].as<std::string>());
    if (!map_path.has_value()) return;
    const auto intersection_map = autoware::test_utils::make_map_bin_msg(map_path.value());
    planner_data_->route_handler->setMap(intersection_map);
    planner_data_->route_handler->setRoute(route);

    for (const auto & segment : route.segments) {
      lanelets.push_back(
        planner_data_->route_handler->getLaneletsFromId(segment.preferred_primitive.id));
    }
  }

  void set_traffic_signal(YAML::Node config)
  {
    const auto traffic_light =
      autoware::test_utils::parse<TrafficLightGroupArray>(config["traffic_signal"]);
    for (const auto & signal : traffic_light.traffic_light_groups) {
      TrafficSignalStamped traffic_signal;
      traffic_signal.stamp = rclcpp::Clock{RCL_ROS_TIME}.now();
      traffic_signal.signal = signal;
      planner_data_->traffic_light_id_map[signal.traffic_light_group_id] = traffic_signal;
    }
  }

  void set_zero_velocity()
  {
    nav_msgs::msg::Odometry odometry;
    odometry.pose.pose = planner_data_->self_odometry->pose.pose;
    odometry.twist.twist.linear = autoware::universe_utils::createVector3(0.0, 0.0, 0.0);
    planner_data_->self_odometry = std::make_shared<const nav_msgs::msg::Odometry>(odometry);
  }

  lanelet::ConstLanelets lanelets;
  std::shared_ptr<PlannerData> planner_data_;
};

TEST_F(TrafficLightTest, getDistanceToNextTrafficLight)
{
  using autoware::behavior_path_planner::utils::traffic_light::getDistanceToNextTrafficLight;

  {
    const Pose pose = autoware::test_utils::createPose(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    const lanelet::ConstLanelets empty_lanelets;
    EXPECT_DOUBLE_EQ(
      getDistanceToNextTrafficLight(pose, empty_lanelets), std::numeric_limits<double>::infinity());
  }
  {
    EXPECT_NEAR(
      getDistanceToNextTrafficLight(planner_data_->self_odometry->pose.pose, lanelets), 117.1599371,
      epsilon);
  }
}

TEST_F(TrafficLightTest, calcDistanceToRedTrafficLight)
{
  using autoware::behavior_path_planner::utils::traffic_light::calcDistanceToRedTrafficLight;

  {
    const tier4_planning_msgs::msg::PathWithLaneId path;
    const lanelet::ConstLanelets empty_lanelets;
    EXPECT_FALSE(calcDistanceToRedTrafficLight(empty_lanelets, path, planner_data_).has_value());
  }
  {
    const auto path = planner_data_->route_handler->getCenterLinePath(lanelets, 0.0, 300.0);
    const auto distance = calcDistanceToRedTrafficLight(lanelets, path, planner_data_);
    ASSERT_TRUE(distance.has_value());
    EXPECT_NEAR(distance.value(), 117.1096960, epsilon);
  }
}

TEST_F(TrafficLightTest, isStoppedAtRedTrafficLightWithinDistance)
{
  using autoware::behavior_path_planner::utils::traffic_light::
    isStoppedAtRedTrafficLightWithinDistance;
  const auto distance_threshold = 10.0;
  const auto path = planner_data_->route_handler->getCenterLinePath(lanelets, 0.0, 300.0);
  {
    EXPECT_FALSE(
      isStoppedAtRedTrafficLightWithinDistance(lanelets, path, planner_data_, distance_threshold));
  }
  {
    set_zero_velocity();
    EXPECT_FALSE(
      isStoppedAtRedTrafficLightWithinDistance(lanelets, path, planner_data_, distance_threshold));
  }
  {
    set_zero_velocity();
    EXPECT_TRUE(isStoppedAtRedTrafficLightWithinDistance(lanelets, path, planner_data_));
  }
}

TEST_F(TrafficLightTest, isTrafficSignalStop)
{
  using autoware::behavior_path_planner::utils::traffic_light::isTrafficSignalStop;

  {
    const lanelet::ConstLanelets empty_lanelets;
    EXPECT_FALSE(isTrafficSignalStop(empty_lanelets, planner_data_));
  }
  {
    EXPECT_TRUE(isTrafficSignalStop(lanelets, planner_data_));
  }
}

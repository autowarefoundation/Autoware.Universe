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

#include "autoware/behavior_path_lane_change_module/manager.hpp"
#include "autoware/behavior_path_lane_change_module/scene.hpp"
#include "autoware/behavior_path_lane_change_module/structs/data.hpp"
#include "autoware/behavior_path_planner_common/data_manager.hpp"
#include "autoware_test_utils/autoware_test_utils.hpp"
#include "autoware_test_utils/mock_data_parser.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <autoware_perception_msgs/msg/predicted_objects.hpp>

#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <string>

using autoware::behavior_path_planner::FilteredLanesObjects;
using autoware::behavior_path_planner::LaneChangeModuleManager;
using autoware::behavior_path_planner::LaneChangeModuleType;
using autoware::behavior_path_planner::NormalLaneChange;
using autoware::behavior_path_planner::PlannerData;
using autoware::behavior_path_planner::lane_change::CommonDataPtr;
using autoware::behavior_path_planner::lane_change::LCParamPtr;
using autoware::behavior_path_planner::lane_change::RouteHandlerPtr;
using autoware::route_handler::Direction;
using autoware::route_handler::RouteHandler;
using autoware::test_utils::get_absolute_path_to_config;
using autoware::test_utils::get_absolute_path_to_lanelet_map;
using autoware::test_utils::get_absolute_path_to_route;
using autoware_map_msgs::msg::LaneletMapBin;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_planning_msgs::msg::LaneletRoute;
using geometry_msgs::msg::Pose;
using tier4_planning_msgs::msg::PathWithLaneId;

class TestNormalLaneChange : public ::testing::Test
{
public:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    init_param();
    init_module();
  }

  void init_param()
  {
    auto node_options = get_node_options();
    auto node = rclcpp::Node(name_, node_options);
    planner_data_->init_parameters(node);
    lc_param_ptr_ = LaneChangeModuleManager::set_params(&node, node.get_name());
    planner_data_->route_handler = init_route_handler();

    ego_pose_ = autoware::test_utils::createPose(-50.0, 1.75, 0.0, 0.0, 0.0, 0.0);
    planner_data_->self_odometry = set_odometry(ego_pose_);
    const auto objects_file =
      ament_index_cpp::get_package_share_directory("autoware_behavior_path_lane_change_module") +
      "/test_data/test_object_filter.yaml";

    YAML::Node yaml_node = YAML::LoadFile(objects_file);
    const auto objects = autoware::test_utils::parse<PredictedObjects>(yaml_node);
    planner_data_->dynamic_object = std::make_shared<PredictedObjects>(objects);
  }

  void init_module()
  {
    normal_lane_change_ =
      std::make_shared<NormalLaneChange>(lc_param_ptr_, lc_type_, lc_direction_);
    normal_lane_change_->setData(planner_data_);
    set_previous_approved_path();
  }

  [[nodiscard]] const CommonDataPtr & get_common_data_ptr() const
  {
    return normal_lane_change_->common_data_ptr_;
  }

  [[nodiscard]] rclcpp::NodeOptions get_node_options() const
  {
    auto node_options = rclcpp::NodeOptions{};

    const auto common_param =
      get_absolute_path_to_config(test_utils_dir_, "test_common.param.yaml");
    const auto nearest_search_param =
      get_absolute_path_to_config(test_utils_dir_, "test_nearest_search.param.yaml");
    const auto vehicle_info_param =
      get_absolute_path_to_config(test_utils_dir_, "test_vehicle_info.param.yaml");

    std::string bpp_dir{"autoware_behavior_path_planner"};
    const auto bpp_param = get_absolute_path_to_config(bpp_dir, "behavior_path_planner.param.yaml");
    const auto drivable_area_expansion_param =
      get_absolute_path_to_config(bpp_dir, "drivable_area_expansion.param.yaml");
    const auto scene_module_manager_param =
      get_absolute_path_to_config(bpp_dir, "scene_module_manager.param.yaml");

    std::string lc_dir{"autoware_behavior_path_lane_change_module"};
    const auto lc_param = get_absolute_path_to_config(lc_dir, "lane_change.param.yaml");

    autoware::test_utils::updateNodeOptions(
      node_options, {common_param, nearest_search_param, vehicle_info_param, bpp_param,
                     drivable_area_expansion_param, scene_module_manager_param, lc_param});
    return node_options;
  }

  [[nodiscard]] RouteHandlerPtr init_route_handler() const
  {
    std::string autoware_route_handler_dir{"autoware_route_handler"};
    std::string lane_change_right_test_route_filename{"lane_change_test_route.yaml"};
    std::string lanelet_map_filename{"2km_test.osm"};
    const auto lanelet2_path =
      get_absolute_path_to_lanelet_map(test_utils_dir_, lanelet_map_filename);
    const auto map_bin_msg = autoware::test_utils::make_map_bin_msg(lanelet2_path, 5.0);
    auto route_handler_ptr = std::make_shared<RouteHandler>(map_bin_msg);
    const auto rh_test_route =
      get_absolute_path_to_route(autoware_route_handler_dir, lane_change_right_test_route_filename);
    if (
      const auto route_opt =
        autoware::test_utils::parse<std::optional<LaneletRoute>>(rh_test_route)) {
      route_handler_ptr->setRoute(*route_opt);
    }

    return route_handler_ptr;
  }

  static std::shared_ptr<nav_msgs::msg::Odometry> set_odometry(const Pose & pose)
  {
    nav_msgs::msg::Odometry odom;
    odom.pose.pose = pose;
    return std::make_shared<nav_msgs::msg::Odometry>(odom);
  }

  [[nodiscard]] FilteredLanesObjects & get_filtered_objects() const
  {
    return normal_lane_change_->filtered_objects_;
  }

  void set_previous_approved_path()
  {
    normal_lane_change_->prev_module_output_.path = create_previous_approved_path();
  }

  [[nodiscard]] PathWithLaneId create_previous_approved_path() const
  {
    const auto & common_data_ptr = get_common_data_ptr();
    const auto & route_handler_ptr = common_data_ptr->route_handler_ptr;
    lanelet::ConstLanelet closest_lane;
    const auto current_pose = planner_data_->self_odometry->pose.pose;
    route_handler_ptr->getClosestLaneletWithinRoute(current_pose, &closest_lane);
    const auto backward_distance = common_data_ptr->bpp_param_ptr->backward_path_length;
    const auto forward_distance = common_data_ptr->bpp_param_ptr->forward_path_length;
    const auto current_lanes = route_handler_ptr->getLaneletSequence(
      closest_lane, current_pose, backward_distance, forward_distance);

    return route_handler_ptr->getCenterLinePath(
      current_lanes, 0.0, std::numeric_limits<double>::max());
  }

  void TearDown() override
  {
    normal_lane_change_ = nullptr;
    lc_param_ptr_ = nullptr;
    planner_data_ = nullptr;
    rclcpp::shutdown();
  }

  LCParamPtr lc_param_ptr_;
  std::shared_ptr<NormalLaneChange> normal_lane_change_;
  std::shared_ptr<PlannerData> planner_data_ = std::make_shared<PlannerData>();
  LaneChangeModuleType lc_type_{LaneChangeModuleType::NORMAL};
  Direction lc_direction_{Direction::RIGHT};
  std::string name_{"test_lane_change_scene"};
  std::string test_utils_dir_{"autoware_test_utils"};
  Pose ego_pose_;
};

TEST_F(TestNormalLaneChange, testBaseClassInitialize)
{
  const auto type = normal_lane_change_->getModuleType();
  const auto type_str = normal_lane_change_->getModuleTypeStr();

  ASSERT_EQ(type, LaneChangeModuleType::NORMAL);
  const auto is_type_str = type_str == "NORMAL";
  ASSERT_TRUE(is_type_str);

  ASSERT_EQ(normal_lane_change_->getDirection(), Direction::RIGHT);

  ASSERT_TRUE(get_common_data_ptr());

  ASSERT_TRUE(get_common_data_ptr()->is_data_available());
  ASSERT_FALSE(get_common_data_ptr()->is_lanes_available());
}

TEST_F(TestNormalLaneChange, testUpdateLanes)
{
  constexpr auto is_approved = true;

  normal_lane_change_->update_lanes(is_approved);

  ASSERT_FALSE(get_common_data_ptr()->is_lanes_available());

  normal_lane_change_->update_lanes(!is_approved);

  ASSERT_TRUE(get_common_data_ptr()->is_lanes_available());
}

TEST_F(TestNormalLaneChange, testGetPathWhenInvalid)
{
  constexpr auto is_approved = true;
  normal_lane_change_->update_lanes(!is_approved);
  normal_lane_change_->update_filtered_objects();
  normal_lane_change_->update_transient_data(!is_approved);
  normal_lane_change_->updateLaneChangeStatus();
  const auto & lc_status = normal_lane_change_->getLaneChangeStatus();

  ASSERT_FALSE(lc_status.is_valid_path);
}

TEST_F(TestNormalLaneChange, testFilteredObjects)
{
  constexpr auto is_approved = true;
  ego_pose_ = autoware::test_utils::createPose(1.0, 1.75, 0.0, 0.0, 0.0, 0.0);
  planner_data_->self_odometry = set_odometry(ego_pose_);
  set_previous_approved_path();

  normal_lane_change_->update_lanes(!is_approved);
  normal_lane_change_->update_filtered_objects();

  const auto & filtered_objects = get_filtered_objects();

  const auto filtered_size =
    filtered_objects.current_lane.size() + filtered_objects.target_lane_leading.size() +
    filtered_objects.target_lane_trailing.size() + filtered_objects.others.size();
  EXPECT_EQ(filtered_size, planner_data_->dynamic_object->objects.size());
  EXPECT_EQ(filtered_objects.current_lane.size(), 1);
  EXPECT_EQ(filtered_objects.target_lane_leading.size(), 2);
  EXPECT_EQ(filtered_objects.target_lane_trailing.size(), 0);
  EXPECT_EQ(filtered_objects.others.size(), 1);
}

TEST_F(TestNormalLaneChange, testGetPathWhenValid)
{
  constexpr auto is_approved = true;
  ego_pose_ = autoware::test_utils::createPose(1.0, 1.75, 0.0, 0.0, 0.0, 0.0);
  planner_data_->self_odometry = set_odometry(ego_pose_);
  normal_lane_change_->setData(planner_data_);
  set_previous_approved_path();
  normal_lane_change_->update_lanes(!is_approved);
  normal_lane_change_->update_filtered_objects();
  normal_lane_change_->update_transient_data(!is_approved);
  const auto lane_change_required = normal_lane_change_->isLaneChangeRequired();

  ASSERT_TRUE(lane_change_required);

  normal_lane_change_->updateLaneChangeStatus();
  const auto & lc_status = normal_lane_change_->getLaneChangeStatus();

  ASSERT_TRUE(lc_status.is_valid_path);
}

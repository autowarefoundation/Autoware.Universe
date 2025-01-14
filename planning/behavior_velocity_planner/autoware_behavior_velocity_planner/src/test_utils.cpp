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

#include "autoware/behavior_velocity_planner/test_utils.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_planning_test_manager/autoware_planning_test_manager.hpp>
#include <autoware_test_utils/autoware_test_utils.hpp>

#include <memory>
#include <string>
#include <vector>

namespace autoware::behavior_velocity_planner
{
std::shared_ptr<PlanningInterfaceTestManager> generateTestManager()
{
  auto test_manager = std::make_shared<PlanningInterfaceTestManager>();

  // set subscriber with topic name: behavior_velocity_planner → test_node_
  test_manager->setPathSubscriber("behavior_velocity_planner_node/output/path");

  // set behavior_velocity_planner node's input topic name(this topic is changed to test node)
  test_manager->setPathWithLaneIdTopicName(
    "behavior_velocity_planner_node/input/path_with_lane_id");

  test_manager->setInitialPoseTopicName("behavior_velocity_planner_node/input/vehicle_odometry");
  test_manager->setOdometryTopicName("behavior_velocity_planner_node/input/vehicle_odometry");

  return test_manager;
}

std::shared_ptr<BehaviorVelocityPlannerNode> generateNode(
  const std::vector<PluginInfo> & plugin_info_vec)
{
  auto node_options = rclcpp::NodeOptions{};

  const auto autoware_test_utils_dir =
    ament_index_cpp::get_package_share_directory("autoware_test_utils");
  const auto behavior_velocity_planner_common_dir =
    ament_index_cpp::get_package_share_directory("autoware_behavior_velocity_planner_common");
  const auto behavior_velocity_planner_dir =
    ament_index_cpp::get_package_share_directory("autoware_behavior_velocity_planner");
  const auto velocity_smoother_dir =
    ament_index_cpp::get_package_share_directory("autoware_velocity_smoother");

  const auto get_behavior_velocity_module_config = [](const std::string & module) {
    const auto package_name = "autoware_behavior_velocity_" + module + "_module";
    const auto package_path = ament_index_cpp::get_package_share_directory(package_name);
    return package_path + "/config/" + module + ".param.yaml";
  };

  std::vector<std::string> plugin_names;
  for (const auto & plugin_info : plugin_info_vec) {
    plugin_names.emplace_back(plugin_info.plugin_name);
  }

  std::vector<rclcpp::Parameter> params;
  params.emplace_back("launch_modules", plugin_names);
  params.emplace_back("is_simulation", false);
  node_options.parameter_overrides(params);

  auto yaml_files = std::vector<std::string>{
    autoware_test_utils_dir + "/config/test_common.param.yaml",
    autoware_test_utils_dir + "/config/test_nearest_search.param.yaml",
    autoware_test_utils_dir + "/config/test_vehicle_info.param.yaml",
    velocity_smoother_dir + "/config/default_velocity_smoother.param.yaml",
    velocity_smoother_dir + "/config/Analytical.param.yaml",
    behavior_velocity_planner_common_dir + "/config/behavior_velocity_planner_common.param.yaml",
    behavior_velocity_planner_dir + "/config/behavior_velocity_planner.param.yaml"};
  for (const auto & plugin_info : plugin_info_vec) {
    yaml_files.push_back(get_behavior_velocity_module_config(plugin_info.module_name));
  }

  autoware::test_utils::updateNodeOptions(node_options, yaml_files);

  // TODO(Takagi, Isamu): set launch_modules
  // TODO(Kyoichi Sugahara) set to true launch_virtual_traffic_light
  // TODO(Kyoichi Sugahara) set to true launch_occlusion_spot
  // TODO(Kyoichi Sugahara) set to true launch_run_out
  // TODO(Kyoichi Sugahara) set to true launch_speed_bump

  return std::make_shared<BehaviorVelocityPlannerNode>(node_options);
}

void publishMandatoryTopics(
  std::shared_ptr<PlanningInterfaceTestManager> test_manager,
  std::shared_ptr<BehaviorVelocityPlannerNode> test_target_node)
{
  // publish necessary topics from test_manager
  test_manager->publishTF(test_target_node, "/tf");
  test_manager->publishAcceleration(test_target_node, "behavior_velocity_planner_node/input/accel");
  test_manager->publishPredictedObjects(
    test_target_node, "behavior_velocity_planner_node/input/dynamic_objects");
  test_manager->publishPointCloud(
    test_target_node, "behavior_velocity_planner_node/input/no_ground_pointcloud");
  test_manager->publishOdometry(
    test_target_node, "behavior_velocity_planner_node/input/vehicle_odometry");
  test_manager->publishAcceleration(test_target_node, "behavior_velocity_planner_node/input/accel");
  test_manager->publishMap(test_target_node, "behavior_velocity_planner_node/input/vector_map");
  test_manager->publishTrafficSignals(
    test_target_node, "behavior_velocity_planner_node/input/traffic_signals");
  test_manager->publishMaxVelocity(
    test_target_node, "behavior_velocity_planner_node/input/external_velocity_limit_mps");
  test_manager->publishOccupancyGrid(
    test_target_node, "behavior_velocity_planner_node/input/occupancy_grid");
}
}  // namespace autoware::behavior_velocity_planner

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

#include "autoware/behavior_path_goal_planner_module/goal_searcher.hpp"
#include "autoware/behavior_path_goal_planner_module/util.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware/behavior_path_goal_planner_module/manager.hpp>
#include <autoware/behavior_path_goal_planner_module/pull_over_planner/shift_pull_over.hpp>
#include <autoware/behavior_path_planner/behavior_path_planner_node.hpp>
#include <autoware/behavior_path_planner_common/data_manager.hpp>
#include <autoware/behavior_path_planner_common/utils/parking_departure/utils.hpp>
#include <autoware/behavior_path_planner_common/utils/path_safety_checker/safety_check.hpp>
#include <autoware/behavior_path_planner_common/utils/path_utils.hpp>
#include <autoware/route_handler/route_handler.hpp>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>
#include <autoware_lanelet2_extension/io/autoware_osm_parser.hpp>
#include <autoware_lanelet2_extension/projection/mgrs_projector.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>

#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>
#include <autoware_planning_msgs/msg/lanelet_primitive.hpp>
#include <autoware_planning_msgs/msg/lanelet_route.hpp>
#include <autoware_planning_msgs/msg/lanelet_segment.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <lanelet2_io/Io.h>
#include <matplotlibcpp17/pyplot.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std::chrono_literals;  // NOLINT

using autoware::behavior_path_planner::BehaviorModuleOutput;
using autoware::behavior_path_planner::GoalCandidate;
using autoware::behavior_path_planner::GoalCandidates;
using autoware::behavior_path_planner::GoalPlannerParameters;
using autoware::behavior_path_planner::PlannerData;
using autoware::behavior_path_planner::PullOverPath;
using autoware::behavior_path_planner::utils::parking_departure::calcFeasibleDecelDistance;
using autoware_planning_msgs::msg::LaneletRoute;
using tier4_planning_msgs::msg::PathWithLaneId;

void plot_footprint(
  matplotlibcpp17::axes::Axes & axes, const autoware::universe_utils::LinearRing2d & footprint,
  const std::string & color = "blue")
{
  std::vector<double> xs, ys;
  for (const auto & pt : footprint) {
    xs.push_back(pt.x());
    ys.push_back(pt.y());
  }
  xs.push_back(xs.front());
  ys.push_back(ys.front());
  axes.plot(Args(xs, ys), Kwargs("color"_a = color, "linestyle"_a = "dotted"));
}

void plot_goal_candidates(
  matplotlibcpp17::axes::Axes & axes, const GoalCandidates & goals,
  const autoware::universe_utils::LinearRing2d & local_footprint,
  const std::string & color = "green")
{
  std::vector<double> xs, ys;
  std::vector<double> yaw_cos, yaw_sin;
  for (const auto & goal : goals) {
    const auto goal_footprint =
      transformVector(local_footprint, autoware::universe_utils::pose2transform(goal.goal_pose));
    plot_footprint(axes, goal_footprint);
    xs.push_back(goal.goal_pose.position.x);
    ys.push_back(goal.goal_pose.position.y);
    axes.text(Args(xs.back(), ys.back(), std::to_string(goal.id)));
    const double yaw = autoware::universe_utils::getRPY(goal.goal_pose).z;
    yaw_cos.push_back(std::cos(yaw));
    yaw_sin.push_back(std::sin(yaw));
  }
  axes.scatter(Args(xs, ys), Kwargs("color"_a = color));
  axes.quiver(
    Args(xs, ys, yaw_cos, yaw_sin),
    Kwargs("angles"_a = "xy", "scale_units"_a = "xy", "scale"_a = 1.0));
}

void plot_path_with_lane_id(
  matplotlibcpp17::axes::Axes & axes, const PathWithLaneId & path,
  const std::string & color = "red", const std::string & label = "")
{
  std::vector<double> xs, ys;
  for (const auto & point : path.points) {
    xs.push_back(point.point.pose.position.x);
    ys.push_back(point.point.pose.position.y);
  }
  if (label == "") {
    axes.plot(Args(xs, ys), Kwargs("color"_a = color, "linewidth"_a = 1.0));
  } else {
    axes.plot(Args(xs, ys), Kwargs("color"_a = color, "linewidth"_a = 1.0, "label"_a = label));
  }
}

void plot_lanelet(
  matplotlibcpp17::axes::Axes & axes, lanelet::ConstLanelet lanelet,
  const std::string & color = "blue", const double linewidth = 0.5)
{
  const auto lefts = lanelet.leftBound();
  const auto rights = lanelet.rightBound();
  std::vector<double> xs_left, ys_left;
  for (const auto & point : lefts) {
    xs_left.push_back(point.x());
    ys_left.push_back(point.y());
  }

  std::vector<double> xs_right, ys_right;
  for (const auto & point : rights) {
    xs_right.push_back(point.x());
    ys_right.push_back(point.y());
  }

  std::vector<double> xs_center, ys_center;
  for (const auto & point : lanelet.centerline()) {
    xs_center.push_back(point.x());
    ys_center.push_back(point.y());
  }

  axes.plot(Args(xs_left, ys_left), Kwargs("color"_a = color, "linewidth"_a = linewidth));
  axes.plot(Args(xs_right, ys_right), Kwargs("color"_a = color, "linewidth"_a = linewidth));
  axes.plot(
    Args(xs_center, ys_center),
    Kwargs("color"_a = "black", "linewidth"_a = linewidth, "linestyle"_a = "dashed"));
}

std::shared_ptr<PlannerData> instantiate_planner_data(
  rclcpp::Node::SharedPtr node, const std::string & map_path, const LaneletRoute & route_msg)
{
  lanelet::ErrorMessages errors{};
  lanelet::projection::MGRSProjector projector{};
  const lanelet::LaneletMapPtr lanelet_map_ptr = lanelet::load(map_path, projector, &errors);
  if (!errors.empty()) {
    for (const auto & error : errors) {
      std::cout << error << std::endl;
    }
    return nullptr;
  }
  autoware_map_msgs::msg::LaneletMapBin map_bin;
  lanelet::utils::conversion::toBinMsg(
    lanelet_map_ptr, &map_bin);  // TODO(soblin): pass lanelet_map_ptr to RouteHandler

  auto planner_data = std::make_shared<PlannerData>();
  planner_data->init_parameters(*node);
  planner_data->route_handler->setMap(map_bin);
  planner_data->route_handler->setRoute(route_msg);

  nav_msgs::msg::Odometry odom;
  odom.pose.pose = route_msg.start_pose;
  auto odometry = std::make_shared<const nav_msgs::msg::Odometry>(odom);
  planner_data->self_odometry = odometry;

  geometry_msgs::msg::AccelWithCovarianceStamped accel;
  accel.accel.accel.linear.x = 0.537018510955429;
  accel.accel.accel.linear.y = -0.2435352815388478;
  auto accel_ptr = std::make_shared<const geometry_msgs::msg::AccelWithCovarianceStamped>(accel);
  planner_data->self_acceleration = accel_ptr;
  return planner_data;
}

bool hasEnoughDistance(
  const PullOverPath & pull_over_path, const PathWithLaneId & long_tail_reference_path,
  const std::shared_ptr<const PlannerData> planner_data, const GoalPlannerParameters & parameters)
{
  using autoware::motion_utils::calcSignedArcLength;

  const Pose & current_pose = planner_data->self_odometry->pose.pose;
  const double current_vel = planner_data->self_odometry->twist.twist.linear.x;

  // when the path is separated and start_pose is close,
  // once stopped, the vehicle cannot start again.
  // so need enough distance to restart.
  // distance to restart should be less than decide_path_distance.
  // otherwise, the goal would change immediately after departure.
  const bool is_separated_path = pull_over_path.partial_paths().size() > 1;
  const double distance_to_start = calcSignedArcLength(
    long_tail_reference_path.points, current_pose.position, pull_over_path.start_pose().position);
  const double distance_to_restart = parameters.decide_path_distance / 2;
  const double eps_vel = 0.01;
  const bool is_stopped = std::abs(current_vel) < eps_vel;
  if (is_separated_path && is_stopped && distance_to_start < distance_to_restart) {
    return false;
  }

  const auto current_to_stop_distance = calcFeasibleDecelDistance(
    planner_data, parameters.maximum_deceleration, parameters.maximum_jerk, 0.0);
  if (!current_to_stop_distance) {
    return false;
  }

  /*
  // If the stop line is subtly exceeded, it is assumed that there is not enough distance to the
  // starting point of parking, so to prevent this, once the vehicle has stopped, it also has a
  // stop_distance_buffer to allow for the amount exceeded.
  const double buffer = is_stopped ? stop_distance_buffer_ : 0.0;
  if (distance_to_start + buffer < *current_to_stop_distance) {
    return false;
    }*/

  return true;
}

std::vector<PullOverPath> selectPullOverPaths(
  const std::vector<PullOverPath> & pull_over_path_candidates,
  const GoalCandidates & goal_candidates, const std::shared_ptr<const PlannerData> planner_data,
  const GoalPlannerParameters & parameters, const BehaviorModuleOutput & previous_module_output)
{
  using autoware::behavior_path_planner::utils::getExtendedCurrentLanesFromPath;
  using autoware::motion_utils::calcSignedArcLength;

  const auto & goal_pose = planner_data->route_handler->getOriginalGoalPose();
  const double backward_length =
    parameters.backward_goal_search_length + parameters.decide_path_distance;

  std::vector<size_t> sorted_path_indices;
  sorted_path_indices.reserve(pull_over_path_candidates.size());

  std::unordered_map<int, GoalCandidate> goal_candidate_map;
  for (const auto & goal_candidate : goal_candidates) {
    goal_candidate_map[goal_candidate.id] = goal_candidate;
  }
  for (size_t i = 0; i < pull_over_path_candidates.size(); ++i) {
    const auto & path = pull_over_path_candidates[i];
    const auto goal_candidate_it = goal_candidate_map.find(path.goal_id());
    if (goal_candidate_it != goal_candidate_map.end() && goal_candidate_it->second.is_safe) {
      sorted_path_indices.push_back(i);
    }
  }

  const double prev_path_front_to_goal_dist = calcSignedArcLength(
    previous_module_output.path.points,
    previous_module_output.path.points.front().point.pose.position, goal_pose.position);
  const auto & long_tail_reference_path = [&]() {
    if (prev_path_front_to_goal_dist > backward_length) {
      return previous_module_output.path;
    }
    // get road lanes which is at least backward_length[m] behind the goal
    const auto road_lanes = getExtendedCurrentLanesFromPath(
      previous_module_output.path, planner_data, backward_length, 0.0, false);
    const auto goal_pose_length = lanelet::utils::getArcCoordinates(road_lanes, goal_pose).length;
    return planner_data->route_handler->getCenterLinePath(
      road_lanes, std::max(0.0, goal_pose_length - backward_length),
      goal_pose_length + parameters.forward_goal_search_length);
  }();

  sorted_path_indices.erase(
    std::remove_if(
      sorted_path_indices.begin(), sorted_path_indices.end(),
      [&](const size_t i) {
        return !hasEnoughDistance(
          pull_over_path_candidates[i], long_tail_reference_path, planner_data, parameters);
      }),
    sorted_path_indices.end());

  // compare to sort pull_over_path_candidates based on the order in efficient_path_order
  const auto comparePathTypePriority = [&](const PullOverPath & a, const PullOverPath & b) -> bool {
    const auto & order = parameters.efficient_path_order;
    const auto a_pos = std::find(order.begin(), order.end(), magic_enum::enum_name(a.type()));
    const auto b_pos = std::find(order.begin(), order.end(), magic_enum::enum_name(b.type()));
    return a_pos < b_pos;
  };

  const auto & soft_margins = parameters.object_recognition_collision_check_soft_margins;
  const auto & hard_margins = parameters.object_recognition_collision_check_hard_margins;

  const auto [margins, margins_with_zero] =
    std::invoke([&]() -> std::tuple<std::vector<double>, std::vector<double>> {
      std::vector<double> margins = soft_margins;
      margins.insert(margins.end(), hard_margins.begin(), hard_margins.end());
      std::vector<double> margins_with_zero = margins;
      margins_with_zero.push_back(0.0);
      return std::make_tuple(margins, margins_with_zero);
    });

  // Create a map of PullOverPath pointer to largest collision check margin
  std::map<size_t, double> path_id_to_rough_margin_map;
  const auto & target_objects = autoware_perception_msgs::msg::PredictedObjects{};
  for (const size_t i : sorted_path_indices) {
    const auto & path = pull_over_path_candidates[i];
    const double distance =
      autoware::behavior_path_planner::utils::path_safety_checker::calculateRoughDistanceToObjects(
        path.parking_path(), target_objects, planner_data->parameters, false, "max");
    auto it = std::lower_bound(
      margins_with_zero.begin(), margins_with_zero.end(), distance, std::greater<double>());
    if (it == margins_with_zero.end()) {
      path_id_to_rough_margin_map[path.id()] = margins_with_zero.back();
    } else {
      path_id_to_rough_margin_map[path.id()] = *it;
    }
  }

  // sorts in descending order so the item with larger margin comes first
  std::stable_sort(
    sorted_path_indices.begin(), sorted_path_indices.end(),
    [&](const size_t a_i, const size_t b_i) {
      const auto & a = pull_over_path_candidates[a_i];
      const auto & b = pull_over_path_candidates[b_i];
      if (
        std::abs(path_id_to_rough_margin_map[a.id()] - path_id_to_rough_margin_map[b.id()]) <
        0.01) {
        return false;
      }
      return path_id_to_rough_margin_map[a.id()] > path_id_to_rough_margin_map[b.id()];
    });

  // STEP2-3: Sort by curvature
  // If the curvature is less than the threshold, prioritize the path.
  const auto isHighCurvature = [&](const PullOverPath & path) -> bool {
    return path.parking_path_max_curvature() >= parameters.high_curvature_threshold;
  };

  const auto isSoftMargin = [&](const PullOverPath & path) -> bool {
    const double margin = path_id_to_rough_margin_map[path.id()];
    return std::any_of(
      soft_margins.begin(), soft_margins.end(),
      [margin](const double soft_margin) { return std::abs(margin - soft_margin) < 0.01; });
  };
  const auto isSameHardMargin = [&](const PullOverPath & a, const PullOverPath & b) -> bool {
    return !isSoftMargin(a) && !isSoftMargin(b) &&
           std::abs(path_id_to_rough_margin_map[a.id()] - path_id_to_rough_margin_map[b.id()]) <
             0.01;
  };

  // NOTE: this is just partition sort based on curvature threshold within each sub partitions
  std::stable_sort(
    sorted_path_indices.begin(), sorted_path_indices.end(),
    [&](const size_t a_i, const size_t b_i) {
      const auto & a = pull_over_path_candidates[a_i];
      const auto & b = pull_over_path_candidates[b_i];
      // if both are soft margin or both are same hard margin, prioritize the path with lower
      // curvature.
      if ((isSoftMargin(a) && isSoftMargin(b)) || isSameHardMargin(a, b)) {
        return !isHighCurvature(a) && isHighCurvature(b);
      }
      // otherwise, keep the order based on the margin.
      return false;
    });

  // STEP2-4: Sort pull_over_path_candidates based on the order in efficient_path_order keeping
  // the collision check margin and curvature priority.
  if (parameters.path_priority == "efficient_path") {
    std::stable_sort(
      sorted_path_indices.begin(), sorted_path_indices.end(),
      [&](const size_t a_i, const size_t b_i) {
        // if any of following conditions are met, sort by path type priority
        // - both are soft margin
        // - both are same hard margin
        const auto & a = pull_over_path_candidates[a_i];
        const auto & b = pull_over_path_candidates[b_i];
        if ((isSoftMargin(a) && isSoftMargin(b)) || isSameHardMargin(a, b)) {
          return comparePathTypePriority(a, b);
        }
        // otherwise, keep the order.
        return false;
      });
  }

  std::vector<PullOverPath> selected;
  for (const auto & sorted_index : sorted_path_indices) {
    selected.push_back(pull_over_path_candidates.at(sorted_index));
  }

  return selected;
}

std::optional<PathWithLaneId> calculate_centerline_path(
  const geometry_msgs::msg::Pose & original_goal_pose,
  const std::shared_ptr<PlannerData> planner_data, const GoalPlannerParameters & parameters)
{
  const auto refined_goal_opt =
    autoware::behavior_path_planner::goal_planner_utils::calcRefinedGoal(
      original_goal_pose, planner_data->route_handler, true,
      planner_data->parameters.vehicle_length, planner_data->parameters.base_link2front,
      planner_data->parameters.base_link2front, parameters);
  if (!refined_goal_opt) {
    return std::nullopt;
  }
  const auto & refined_goal = refined_goal_opt.value();

  const auto & route_handler = planner_data->route_handler;
  const double forward_length = parameters.forward_goal_search_length;
  const double backward_length = parameters.backward_goal_search_length;
  const bool use_bus_stop_area = parameters.bus_stop_area.use_bus_stop_area;
  /*
  const double margin_from_boundary = parameters.margin_from_boundary;
  const double lateral_offset_interval = use_bus_stop_area
                                           ? parameters.bus_stop_area.lateral_offset_interval
                                           : parameters.lateral_offset_interval;
  const double max_lateral_offset = parameters.max_lateral_offset;
  const double ignore_distance_from_lane_start = parameters.ignore_distance_from_lane_start;
  */

  const auto pull_over_lanes =
    autoware::behavior_path_planner::goal_planner_utils::getPullOverLanes(
      *route_handler, true, parameters.backward_goal_search_length,
      parameters.forward_goal_search_length);
  const auto departure_check_lane =
    autoware::behavior_path_planner::goal_planner_utils::createDepartureCheckLanelet(
      pull_over_lanes, *route_handler, true);
  const auto goal_arc_coords = lanelet::utils::getArcCoordinates(pull_over_lanes, refined_goal);
  const double s_start = std::max(0.0, goal_arc_coords.length - backward_length);
  const double s_end = goal_arc_coords.length + forward_length;
  const double longitudinal_interval = use_bus_stop_area
                                         ? parameters.bus_stop_area.goal_search_interval
                                         : parameters.goal_search_interval;
  auto center_line_path = autoware::behavior_path_planner::utils::resamplePathWithSpline(
    route_handler->getCenterLinePath(pull_over_lanes, s_start, s_end), longitudinal_interval);
  return center_line_path;
}

int main(int argc, char ** argv)
{
  using autoware::behavior_path_planner::utils::getReferencePath;
  rclcpp::init(argc, argv);

  autoware_planning_msgs::msg::LaneletRoute route_msg;
  route_msg.start_pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(geometry_msgs::build<geometry_msgs::msg::Point>().x(729.944).y(695.124).z(381.18))
      .orientation(
        geometry_msgs::build<geometry_msgs::msg::Quaternion>().x(0.0).y(0.0).z(0.437138).w(
          0.899395));
  route_msg.goal_pose =
    geometry_msgs::build<geometry_msgs::msg::Pose>()
      .position(geometry_msgs::build<geometry_msgs::msg::Point>().x(797.526).y(694.105).z(381.18))
      .orientation(
        geometry_msgs::build<geometry_msgs::msg::Quaternion>().x(0.0).y(0.0).z(-0.658723).w(
          0.752386));

  route_msg.segments = std::vector<autoware_planning_msgs::msg::LaneletSegment>{
    autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletSegment>()
      .preferred_primitive(
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15214)
          .primitive_type(""))
      .primitives(std::vector<autoware_planning_msgs::msg::LaneletPrimitive>{
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15214)
          .primitive_type("lane"),
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15213)
          .primitive_type("lane"),
      }),
    autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletSegment>()
      .preferred_primitive(
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15226)
          .primitive_type(""))
      .primitives(std::vector<autoware_planning_msgs::msg::LaneletPrimitive>{
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15226)
          .primitive_type("lane"),
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15225)
          .primitive_type("lane"),
      }),
    autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletSegment>()
      .preferred_primitive(
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15228)
          .primitive_type(""))
      .primitives(std::vector<autoware_planning_msgs::msg::LaneletPrimitive>{
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15228)
          .primitive_type("lane"),
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15229)
          .primitive_type("lane"),
      }),
    autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletSegment>()
      .preferred_primitive(
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15231)
          .primitive_type(""))
      .primitives(std::vector<autoware_planning_msgs::msg::LaneletPrimitive>{
        autoware_planning_msgs::build<autoware_planning_msgs::msg::LaneletPrimitive>()
          .id(15231)
          .primitive_type("lane"),
      }),
  };
  route_msg.allow_modification = false;

  auto node_options = rclcpp::NodeOptions{};
  node_options.parameter_overrides(
    std::vector<rclcpp::Parameter>{{"launch_modules", std::vector<std::string>{}}});
  node_options.arguments(std::vector<std::string>{
    "--ros-args", "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_behavior_path_planner") +
      "/config/behavior_path_planner.param.yaml",
    "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_behavior_path_planner") +
      "/config/drivable_area_expansion.param.yaml",
    "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_behavior_path_planner") +
      "/config/scene_module_manager.param.yaml",
    "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_test_utils") +
      "/config/test_common.param.yaml",
    "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_test_utils") +
      "/config/test_nearest_search.param.yaml",
    "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_test_utils") +
      "/config/test_vehicle_info.param.yaml",
    "--params-file",
    ament_index_cpp::get_package_share_directory("autoware_behavior_path_goal_planner_module") +
      "/config/goal_planner.param.yaml"});
  auto node = rclcpp::Node::make_shared("plot_map", node_options);

  auto planner_data = instantiate_planner_data(
    node,
    ament_index_cpp::get_package_share_directory("autoware_test_utils") +
      "/test_map/road_shoulder/lanelet2_map.osm",
    route_msg);

  lanelet::ConstLanelet current_route_lanelet;
  planner_data->route_handler->getClosestLaneletWithinRoute(
    route_msg.start_pose, &current_route_lanelet);
  const auto reference_path = getReferencePath(current_route_lanelet, planner_data);
  auto goal_planner_parameter =
    autoware::behavior_path_planner::GoalPlannerModuleManager::initGoalPlannerParameters(
      node.get(), "goal_planner.");
  const auto vehicle_info = autoware::vehicle_info_utils::VehicleInfoUtils(*node).getVehicleInfo();
  autoware::lane_departure_checker::LaneDepartureChecker lane_departure_checker{};
  lane_departure_checker.setVehicleInfo(vehicle_info);
  autoware::lane_departure_checker::Param lane_departure_checker_params;
  lane_departure_checker_params.footprint_extra_margin =
    goal_planner_parameter.lane_departure_check_expansion_margin;
  lane_departure_checker.setParam(lane_departure_checker_params);
  const auto footprint = vehicle_info.createFootprint();
  autoware::behavior_path_planner::GoalSearcher goal_searcher(goal_planner_parameter, footprint);
  const auto goal_candidates = goal_searcher.search(planner_data);

  pybind11::scoped_interpreter guard{};
  auto plt = matplotlibcpp17::pyplot::import();
  auto [fig, axes] = plt.subplots(1, 2);

  auto & ax1 = axes[0];
  auto & ax2 = axes[1];
  const std::vector<lanelet::Id> ids{15213, 15214, 15225, 15226, 15224, 15227,
                                     15228, 15229, 15230, 15231, 15232};
  for (const auto & id : ids) {
    const auto lanelet = planner_data->route_handler->getLaneletMapPtr()->laneletLayer.get(id);
    plot_lanelet(ax1, lanelet);
    plot_lanelet(ax2, lanelet);
  }

  plot_goal_candidates(ax1, goal_candidates, footprint);

  plot_path_with_lane_id(ax2, reference_path.path, "green", "reference_path");

  std::vector<PullOverPath> candidates;
  for (const auto & goal_candidate : goal_candidates) {
    auto shift_pull_over_planner = autoware::behavior_path_planner::ShiftPullOver(
      *node, goal_planner_parameter, lane_departure_checker);
    const auto pull_over_path_opt =
      shift_pull_over_planner.plan(goal_candidate, 0, planner_data, reference_path);
    if (pull_over_path_opt) {
      const auto & pull_over_path = pull_over_path_opt.value();
      const auto & full_path = pull_over_path.full_path();
      candidates.push_back(pull_over_path);
      plot_path_with_lane_id(ax1, full_path);
    }
  }
  [[maybe_unused]] const auto filtered_paths = selectPullOverPaths(
    candidates, goal_candidates, planner_data, goal_planner_parameter, reference_path);

  const auto centerline_path =
    calculate_centerline_path(route_msg.goal_pose, planner_data, goal_planner_parameter);
  if (centerline_path) {
    plot_path_with_lane_id(ax2, centerline_path.value(), "red", "centerline_path");
  }
  ax1.set_aspect(Args("equal"));
  ax2.set_aspect(Args("equal"));
  ax1.legend();
  ax2.legend();
  plt.show();

  rclcpp::shutdown();
  return 0;
}

// Copyright 2023 TIER IV, Inc.
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

#ifndef PATH_SMOOTHING__ELASTIC_BAND_HPP_
#define PATH_SMOOTHING__ELASTIC_BAND_HPP_

#include "osqp_interface/osqp_interface.hpp"
#include "path_smoothing/common_structs.hpp"
#include "path_smoothing/parameters.hpp"
#include "path_smoothing/type_alias.hpp"

#include <Eigen/Core>

#include <memory>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

namespace path_smoothing
{
class EBPathSmoother
{
public:
  EBPathSmoother(
    rclcpp::Node * node, const bool enable_debug_info, const EgoNearestParam ego_nearest_param,
    const CommonParam & common_param, const std::shared_ptr<TimeKeeper> time_keeper_ptr);

  std::optional<std::vector<autoware_auto_planning_msgs::msg::TrajectoryPoint>> get_trajectory(
    const PlannerData & planner_data);

  void initialize(const bool enable_debug_info, const CommonParam & common_param);
  void reset_previous_data();
  void onParam(const std::vector<rclcpp::Parameter> & parameters);

private:
  struct Constraint2d
  {
    struct Constraint
    {
      Eigen::Vector2d coef;
      double upper_bound;
      double lower_bound;
    };

    Constraint lon;
    Constraint lat;
  };

  // arguments
  bool enable_debug_info_;
  EgoNearestParam ego_nearest_param_;
  CommonParam common_param_;
  EBParam eb_param_;
  mutable std::shared_ptr<TimeKeeper> time_keeper_ptr_;
  rclcpp::Logger logger_;

  // publisher
  rclcpp::Publisher<Trajectory>::SharedPtr debug_eb_traj_pub_;
  rclcpp::Publisher<Trajectory>::SharedPtr debug_eb_fixed_traj_pub_;

  std::unique_ptr<autoware::common::osqp::OSQPInterface> osqp_solver_ptr_;
  std::shared_ptr<std::vector<TrajectoryPoint>> prev_eb_traj_points_ptr_{nullptr};

  std::vector<TrajectoryPoint> insert_fixed_point(
    const std::vector<TrajectoryPoint> & traj_point) const;

  std::tuple<std::vector<TrajectoryPoint>, size_t> get_padded_trajectory_points(
    const std::vector<TrajectoryPoint> & traj_points) const;

  void update_constraint(
    const std_msgs::msg::Header & header, const std::vector<TrajectoryPoint> & traj_points,
    const bool is_goal_contained, const int pad_start_idx);

  std::optional<std::vector<double>> optimize_trajectory();

  std::optional<std::vector<autoware_auto_planning_msgs::msg::TrajectoryPoint>>
  convert_optimized_points_to_trajectory(
    const std::vector<double> & optimized_points, const std::vector<TrajectoryPoint> & traj_points,
    const int pad_start_idx) const;
};
}  // namespace path_smoothing

#endif  // PATH_SMOOTHING__ELASTIC_BAND_HPP_

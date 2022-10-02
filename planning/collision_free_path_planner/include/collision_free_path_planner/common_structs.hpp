// Copyright 2021 Tier IV, Inc.
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

#ifndef COLLISION_FREE_PATH_PLANNER__COMMON_STRUCTS_HPP_
#define COLLISION_FREE_PATH_PLANNER__COMMON_STRUCTS_HPP_

#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
#include "rclcpp/rclcpp.hpp"
#include "tier4_autoware_utils/tier4_autoware_utils.hpp"

#include "autoware_auto_perception_msgs/msg/predicted_object.hpp"
#include "autoware_auto_planning_msgs/msg/path.hpp"
#include "autoware_auto_planning_msgs/msg/trajectory_point.hpp"
#include "nav_msgs/msg/map_meta_data.hpp"

#include <boost/optional.hpp>

#include <memory>
#include <string>
#include <vector>

namespace collision_free_path_planner
{
struct ReferencePoint;

struct Bounds;
using VehicleBounds = std::vector<Bounds>;
using SequentialBounds = std::vector<Bounds>;

using BoundsCandidates = std::vector<Bounds>;
using SequentialBoundsCandidates = std::vector<BoundsCandidates>;

// message type
using autoware_auto_perception_msgs::msg::PredictedObject;
using autoware_auto_planning_msgs::msg::Path;
using autoware_auto_planning_msgs::msg::TrajectoryPoint;

struct CVMaps
{
  cv::Mat drivable_area;
  cv::Mat clearance_map;
  cv::Mat only_objects_clearance_map;
  cv::Mat area_with_objects_map;
  nav_msgs::msg::MapMetaData map_info;
};

struct QPParam
{
  int max_iteration;
  double eps_abs;
  double eps_rel;
};

struct EBParam
{
  double clearance_for_fixing;
  double clearance_for_straight_line;
  double clearance_for_joint;
  double clearance_for_only_smoothing;
  QPParam qp_param;

  int num_joint_buffer_points;
  int num_offset_for_begin_idx;

  double delta_arc_length_for_eb;
  int num_sampling_points_for_eb;
};

struct VehicleParam
{
  double wheelbase;
  double length;
  double width;
  double rear_overhang;
  double front_overhang;
  // double max_steer_rad;
};

struct ConstrainRectangle
{
  geometry_msgs::msg::Point top_left;
  geometry_msgs::msg::Point top_right;
  geometry_msgs::msg::Point bottom_left;
  geometry_msgs::msg::Point bottom_right;
  double velocity;
  bool is_empty_driveable_area = false;
  bool is_including_only_smooth_range = true;
};

struct DebugData
{
  struct StreamWithPrint
  {
    StreamWithPrint & operator<<(const std::string & s)
    {
      sstream << s;
      if (s.back() == '\n') {
        std::string tmp_str = sstream.str();
        debug_str += tmp_str;

        if (is_showing_calculation_time) {
          tmp_str.pop_back();  // NOTE* remove '\n' which is unnecessary for RCLCPP_INFO_STREAM
          RCLCPP_INFO_STREAM(rclcpp::get_logger("collision_free_path_planner.time"), tmp_str);
        }
        sstream.str("");
      }
      return *this;
    }

    StreamWithPrint & operator<<(const double d)
    {
      sstream << d;
      return *this;
    }

    std::string getString() const { return debug_str; }

    bool is_showing_calculation_time;
    std::string debug_str = "\n";
    std::stringstream sstream;
  };

  void init(
    const bool local_is_showing_calculation_time,
    const geometry_msgs::msg::Pose & local_current_ego_pose)
  {
    msg_stream.is_showing_calculation_time = local_is_showing_calculation_time;
    current_ego_pose = local_current_ego_pose;
  }

  StreamWithPrint msg_stream;
  size_t mpt_visualize_sampling_num;
  geometry_msgs::msg::Pose current_ego_pose;
  std::vector<double> vehicle_circle_radiuses;
  std::vector<double> vehicle_circle_longitudinal_offsets;

  boost::optional<geometry_msgs::msg::Pose> stop_pose_by_drivable_area = boost::none;
  std::vector<geometry_msgs::msg::Point> interpolated_points;
  std::vector<geometry_msgs::msg::Point> straight_points;
  std::vector<geometry_msgs::msg::Pose> fixed_points;
  std::vector<geometry_msgs::msg::Pose> non_fixed_points;
  std::vector<ConstrainRectangle> constrain_rectangles;
  std::vector<TrajectoryPoint> avoiding_traj_points;
  std::vector<PredictedObject> avoiding_objects;

  cv::Mat clearance_map;
  cv::Mat only_object_clearance_map;
  cv::Mat area_with_objects_map;

  std::vector<std::vector<geometry_msgs::msg::Pose>> vehicle_circles_pose;
  std::vector<ReferencePoint> ref_points;

  std::vector<geometry_msgs::msg::Pose> mpt_ref_poses;
  std::vector<double> lateral_errors;

  std::vector<TrajectoryPoint> eb_traj;
  std::vector<TrajectoryPoint> mpt_fixed_traj;
  std::vector<TrajectoryPoint> mpt_ref_traj;
  std::vector<TrajectoryPoint> mpt_traj;
  std::vector<TrajectoryPoint> extended_fixed_traj;
  std::vector<TrajectoryPoint> extended_non_fixed_traj;

  SequentialBoundsCandidates sequential_bounds_candidates;
};

struct Trajectories
{
  std::vector<TrajectoryPoint> smoothed_trajectory;
  std::vector<ReferencePoint> mpt_ref_points;
  std::vector<TrajectoryPoint> model_predictive_trajectory;
};

struct TrajectoryParam
{
  bool is_avoiding_unknown;
  bool is_avoiding_car;
  bool is_avoiding_truck;
  bool is_avoiding_bus;
  bool is_avoiding_bicycle;
  bool is_avoiding_motorbike;
  bool is_avoiding_pedestrian;
  bool is_avoiding_animal;
  int num_sampling_points;
  double delta_arc_length_for_trajectory;
  double delta_dist_threshold_for_closest_point;
  double delta_yaw_threshold_for_closest_point;
  double delta_yaw_threshold_for_straight;
  double trajectory_length;
  double forward_fixing_min_distance;
  double forward_fixing_min_time;
  double backward_fixing_distance;
  double max_avoiding_ego_velocity_ms;
  double max_avoiding_objects_velocity_ms;
  double center_line_width;
  double acceleration_for_non_deceleration_range;
  int num_fix_points_for_extending;
  double max_dist_for_extending_end_point;

  double ego_nearest_dist_threshold;
  double ego_nearest_yaw_threshold;
};

struct PlannerData
{
  Path path;
  geometry_msgs::msg::Pose ego_pose{};
  double ego_vel{};
  std::vector<PredictedObject> objects{};
  bool enable_avoidance{false};
};
}  // namespace collision_free_path_planner

namespace tier4_autoware_utils
{
template <>
geometry_msgs::msg::Point getPoint(const collision_free_path_planner::ReferencePoint & p);

template <>
geometry_msgs::msg::Pose getPose(const collision_free_path_planner::ReferencePoint & p);
}  // namespace tier4_autoware_utils

#endif  // COLLISION_FREE_PATH_PLANNER__COMMON_STRUCTS_HPP_

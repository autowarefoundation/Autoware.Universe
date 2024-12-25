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

#ifndef MAP_BASED_PREDICTION__MAP_BASED_PREDICTION_NODE_HPP_
#define MAP_BASED_PREDICTION__MAP_BASED_PREDICTION_NODE_HPP_

#include "map_based_prediction/data_structure.hpp"
#include "map_based_prediction/path_generator.hpp"
#include "map_based_prediction/predictor_vru.hpp"

#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/ros/debug_publisher.hpp>
#include <autoware/universe_utils/ros/polling_subscriber.hpp>
#include <autoware/universe_utils/ros/published_time_publisher.hpp>
#include <autoware/universe_utils/ros/transform_listener.hpp>
#include <autoware/universe_utils/ros/update_param.hpp>
#include <autoware/universe_utils/system/lru_cache.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>
#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/pose.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_routing/Forward.h>
#include <lanelet2_routing/LaneletPath.h>
#include <lanelet2_traffic_rules/TrafficRules.h>
#include <tf2/LinearMath/Quaternion.h>

#include <algorithm>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace std
{
template <>
struct hash<lanelet::routing::LaneletPath>
{
  // 0x9e3779b9 is a magic number. See
  // https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
  size_t operator()(const lanelet::routing::LaneletPath & path) const
  {
    size_t seed = 0;
    for (const auto & lanelet : path) {
      seed ^= hash<int64_t>{}(lanelet.id()) + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
    }
    return seed;
  }
};
}  // namespace std
namespace autoware::map_based_prediction
{
using autoware_planning_msgs::msg::TrajectoryPoint;
using tier4_debug_msgs::msg::StringStamped;
using TrajectoryPoints = std::vector<TrajectoryPoint>;

class MapBasedPredictionNode : public rclcpp::Node
{
public:
  explicit MapBasedPredictionNode(const rclcpp::NodeOptions & node_options);

private:
  // ROS Publisher and Subscriber
  rclcpp::Publisher<PredictedObjects>::SharedPtr pub_objects_;
  rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr pub_debug_markers_;
  rclcpp::Subscription<TrackedObjects>::SharedPtr sub_objects_;
  rclcpp::Subscription<LaneletMapBin>::SharedPtr sub_map_;
  autoware::universe_utils::InterProcessPollingSubscriber<TrafficLightGroupArray>
    sub_traffic_signals_{this, "/traffic_signals"};

  // debug publisher
  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> processing_time_publisher_;

  // Object History
  std::unordered_map<std::string, std::deque<ObjectData>> road_users_history_;

  // Lanelet Map Pointers
  std::shared_ptr<lanelet::LaneletMap> lanelet_map_ptr_;
  std::shared_ptr<lanelet::routing::RoutingGraph> routing_graph_ptr_;
  std::shared_ptr<lanelet::traffic_rules::TrafficRules> traffic_rules_ptr_;

  // parameter update
  OnSetParametersCallbackHandle::SharedPtr set_param_res_;
  rcl_interfaces::msg::SetParametersResult onParam(
    const std::vector<rclcpp::Parameter> & parameters);

  // Pose Transform Listener
  autoware::universe_utils::TransformListener transform_listener_{this};

  // Path Generator
  std::shared_ptr<PathGenerator> path_generator_;

  // Predictor
  std::shared_ptr<PredictorVru> predictor_vru_;

  ////// Parameters

  // Object Parameters
  bool enable_delay_compensation_;

  //// Vehicle Parameters
  // Lanelet Parameters
  double dist_threshold_for_searching_lanelet_;
  double delta_yaw_threshold_for_searching_lanelet_;
  double sigma_lateral_offset_;
  double sigma_yaw_angle_deg_;
  bool consider_only_routable_neighbours_;

  // Pedestrian Parameters
  bool remember_lost_crosswalk_users_;

  // Object history parameters
  double object_buffer_time_length_;
  double history_time_length_;
  double cutoff_freq_of_velocity_lpf_;

  // Prediction Parameters
  std::string lane_change_detection_method_;
  double dist_threshold_to_bound_;
  double time_threshold_to_bound_;
  double dist_ratio_threshold_to_left_bound_;
  double dist_ratio_threshold_to_right_bound_;
  double diff_dist_threshold_to_left_bound_;
  double diff_dist_threshold_to_right_bound_;
  int num_continuous_state_transition_;

  // Path generation parameters
  double lateral_control_time_horizon_;
  PredictionTimeHorizon prediction_time_horizon_;
  double prediction_time_horizon_rate_for_validate_lane_length_;
  double prediction_sampling_time_interval_;
  double min_velocity_for_map_based_prediction_;
  double reference_path_resolution_;
  bool check_lateral_acceleration_constraints_;
  double max_lateral_accel_;
  double min_acceleration_before_curve_;
  bool use_vehicle_acceleration_;
  double speed_limit_multiplier_;
  double acceleration_exponential_half_life_;

  ////// Member Functions
  // Node callbacks
  void mapCallback(const LaneletMapBin::ConstSharedPtr msg);
  void trafficSignalsCallback(const TrafficLightGroupArray::ConstSharedPtr msg);
  void objectsCallback(const TrackedObjects::ConstSharedPtr in_objects);

  // Map process
  bool doesPathCrossFence(
    const PredictedPath & predicted_path, const lanelet::ConstLineString3d & fence_line);
  lanelet::BasicLineString2d convertToFenceLine(const lanelet::ConstLineString3d & fence);

  // Object process
  PredictedObject convertToPredictedObject(const TrackedObject & tracked_object);
  void updateObjectData(TrackedObject & object);
  geometry_msgs::msg::Pose compensateTimeDelay(
    const geometry_msgs::msg::Pose & delayed_pose, const geometry_msgs::msg::Twist & twist,
    const double dt) const;

  //// Vehicle process
  // Lanelet process
  LaneletsData getCurrentLanelets(const TrackedObject & object);
  bool isDuplicated(
    const std::pair<double, lanelet::ConstLanelet> & target_lanelet,
    const LaneletsData & lanelets_data);
  bool isDuplicated(
    const PredictedPath & predicted_path, const std::vector<PredictedPath> & predicted_paths);
  std::optional<lanelet::Id> getTrafficSignalId(const lanelet::ConstLanelet & way_lanelet);

  // Vehicle history process
  void updateRoadUsersHistory(
    const std_msgs::msg::Header & header, const TrackedObject & object,
    const LaneletsData & current_lanelets_data);

  // Vehicle Maneuver Prediction
  Maneuver predictObjectManeuver(
    const std::string & object_id, const geometry_msgs::msg::Pose & object_pose,
    const LaneletData & current_lanelet_data, const double object_detected_time);
  Maneuver predictObjectManeuverByTimeToLaneChange(
    const std::string & object_id, const LaneletData & current_lanelet_data,
    const double object_detected_time);
  Maneuver predictObjectManeuverByLatDiffDistance(
    const std::string & object_id, const geometry_msgs::msg::Pose & object_pose,
    const LaneletData & current_lanelet_data, const double object_detected_time);

  double calcRightLateralOffset(
    const lanelet::ConstLineString2d & boundary_line, const geometry_msgs::msg::Pose & search_pose);
  double calcLeftLateralOffset(
    const lanelet::ConstLineString2d & boundary_line, const geometry_msgs::msg::Pose & search_pose);
  ManeuverProbability calculateManeuverProbability(
    const Maneuver & predicted_maneuver, const bool & left_paths_exists,
    const bool & right_paths_exists, const bool & center_paths_exists) const;

  // Vehicle path process
  PredictedObject getPredictionForNonVehicleObject(
    const std_msgs::msg::Header & header, const TrackedObject & object);
  std::optional<PredictedObject> getPredictionForVehicleObject(
    const std_msgs::msg::Header & header, const TrackedObject & object,
    const double objects_detected_time, visualization_msgs::msg::MarkerArray & debug_markers);
  std::optional<size_t> searchProperStartingRefPathIndex(
    const TrackedObject & object, const PosePath & pose_path) const;
  std::vector<LaneletPathWithPathInfo> getPredictedReferencePath(
    const TrackedObject & object, const LaneletsData & current_lanelets_data,
    const double object_detected_time, const double time_horizon);
  std::vector<PredictedRefPath> convertPredictedReferencePath(
    const TrackedObject & object,
    const std::vector<LaneletPathWithPathInfo> & lanelet_ref_paths) const;
  mutable universe_utils::LRUCache<lanelet::routing::LaneletPath, std::pair<PosePath, double>>
    lru_cache_of_convert_path_type_{1000};
  std::pair<PosePath, double> convertLaneletPathToPosePath(
    const lanelet::routing::LaneletPath & path) const;

  ////// Debugger
  std::unique_ptr<autoware::universe_utils::PublishedTimePublisher> published_time_publisher_;
  rclcpp::Publisher<autoware::universe_utils::ProcessingTimeDetail>::SharedPtr
    detailed_processing_time_publisher_;
  std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_;
  visualization_msgs::msg::Marker getDebugMarker(
    const TrackedObject & object, const Maneuver & maneuver, const size_t obj_num);

  //// Node functions
  void publish(
    const PredictedObjects & output,
    const visualization_msgs::msg::MarkerArray & debug_markers) const;

  // NOTE: This function is copied from the motion_velocity_smoother package.
  // TODO(someone): Consolidate functions and move them to a common
  inline std::vector<double> calcTrajectoryCurvatureFrom3Points(
    const TrajectoryPoints & trajectory, size_t idx_dist)
  {
    using autoware::universe_utils::calcCurvature;
    using autoware::universe_utils::getPoint;

    if (trajectory.size() < 3) {
      const std::vector<double> k_arr(trajectory.size(), 0.0);
      return k_arr;
    }

    // if the idx size is not enough, change the idx_dist
    const auto max_idx_dist = static_cast<size_t>(std::floor((trajectory.size() - 1) / 2.0));
    idx_dist = std::max(1ul, std::min(idx_dist, max_idx_dist));

    if (idx_dist < 1) {
      throw std::logic_error("idx_dist less than 1 is not expected");
    }

    // calculate curvature by circle fitting from three points
    std::vector<double> k_arr(trajectory.size(), 0.0);

    for (size_t i = 1; i + 1 < trajectory.size(); i++) {
      double curvature = 0.0;
      const auto p0 = getPoint(trajectory.at(i - std::min(idx_dist, i)));
      const auto p1 = getPoint(trajectory.at(i));
      const auto p2 = getPoint(trajectory.at(i + std::min(idx_dist, trajectory.size() - 1 - i)));
      try {
        curvature = calcCurvature(p0, p1, p2);
      } catch (std::exception const & e) {
        // ...code that handles the error...
        RCLCPP_WARN(rclcpp::get_logger("map_based_prediction"), "%s", e.what());
        if (i > 1) {
          curvature = k_arr.at(i - 1);  // previous curvature
        } else {
          curvature = 0.0;
        }
      }
      k_arr.at(i) = curvature;
    }
    // copy curvatures for the last and first points;
    k_arr.at(0) = k_arr.at(1);
    k_arr.back() = k_arr.at((trajectory.size() - 2));

    return k_arr;
  }

  inline TrajectoryPoints toTrajectoryPoints(const PredictedPath & path, const double velocity)
  {
    TrajectoryPoints out_trajectory;
    std::for_each(
      path.path.begin(), path.path.end(), [&out_trajectory, velocity](const auto & pose) {
        TrajectoryPoint p;
        p.pose = pose;
        p.longitudinal_velocity_mps = velocity;
        out_trajectory.push_back(p);
      });
    return out_trajectory;
  };

  inline bool isLateralAccelerationConstraintSatisfied(
    const TrajectoryPoints & trajectory, const double delta_time)
  {
    constexpr double epsilon = 1E-6;
    if (delta_time < epsilon) throw std::invalid_argument("delta_time must be a positive value");

    if (trajectory.size() < 3) return true;
    const double max_lateral_accel_abs = std::fabs(max_lateral_accel_);

    double arc_length = 0.0;
    for (size_t i = 1; i < trajectory.size(); ++i) {
      const auto current_pose = trajectory.at(i).pose;
      const auto next_pose = trajectory.at(i - 1).pose;
      // Compute distance between poses
      const double delta_s = std::hypot(
        next_pose.position.x - current_pose.position.x,
        next_pose.position.y - current_pose.position.y);
      arc_length += delta_s;

      // Compute change in heading
      tf2::Quaternion q_current, q_next;
      tf2::convert(current_pose.orientation, q_current);
      tf2::convert(next_pose.orientation, q_next);
      double delta_theta = q_current.angleShortestPath(q_next);
      // Handle wrap-around
      if (delta_theta > M_PI) {
        delta_theta -= 2.0 * M_PI;
      } else if (delta_theta < -M_PI) {
        delta_theta += 2.0 * M_PI;
      }

      const double yaw_rate = std::max(std::abs(delta_theta / delta_time), 1.0E-5);

      const double current_speed = std::abs(trajectory.at(i).longitudinal_velocity_mps);
      // Compute lateral acceleration
      const double lateral_acceleration = std::abs(current_speed * yaw_rate);
      if (lateral_acceleration < max_lateral_accel_abs) continue;
      const double v_curvature_max = std::sqrt(max_lateral_accel_abs / yaw_rate);
      const double t =
        (v_curvature_max - current_speed) / min_acceleration_before_curve_;  // acc is negative
      const double distance_to_slow_down =
        current_speed * t + 0.5 * min_acceleration_before_curve_ * std::pow(t, 2);
      if (distance_to_slow_down > arc_length) return false;
    }
    return true;
  };
};
}  // namespace autoware::map_based_prediction

#endif  // MAP_BASED_PREDICTION__MAP_BASED_PREDICTION_NODE_HPP_

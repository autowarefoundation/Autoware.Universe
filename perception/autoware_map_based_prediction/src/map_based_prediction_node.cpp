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

#include "map_based_prediction/map_based_prediction_node.hpp"

#include "map_based_prediction/utils.hpp"

#include <autoware/interpolation/linear_interpolation.hpp>
#include <autoware/motion_utils/resample/resample.hpp>
#include <autoware/motion_utils/trajectory/trajectory.hpp>
#include <autoware/universe_utils/geometry/geometry.hpp>
#include <autoware/universe_utils/math/constants.hpp>
#include <autoware/universe_utils/math/normalization.hpp>
#include <autoware/universe_utils/math/unit_conversion.hpp>
#include <autoware/universe_utils/ros/uuid_helper.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <autoware_utils/autoware_utils.hpp>

#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <glog/logging.h>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/geometry/LaneletMap.h>
#include <lanelet2_core/geometry/Point.h>
#include <lanelet2_routing/RoutingGraph.h>
#include <tf2/utils.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <deque>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::map_based_prediction
{
using autoware::universe_utils::ScopedTimeTrack;

namespace
{
/**
 * @brief First order Low pass filtering
 *
 * @param prev_y previous filtered value
 * @param prev_x previous input value
 * @param x current input value
 * @param cutoff_freq  cutoff frequency in Hz not rad/s (1/s)
 * @param sampling_time  sampling time of discrete system (s)
 *
 * @return double current filtered value
 */
double FirstOrderLowpassFilter(
  const double prev_y, const double prev_x, const double x, const double sampling_time = 0.1,
  const double cutoff_freq = 0.1)
{
  // Eq:  yn = a yn-1 + b (xn-1 + xn)
  const double wt = 2.0 * M_PI * cutoff_freq * sampling_time;
  const double a = (2.0 - wt) / (2.0 + wt);
  const double b = wt / (2.0 + wt);

  return a * prev_y + b * (prev_x + x);
}

/**
 * @brief calc lateral offset from pose to linestring
 *
 * @param boundary_line 2d line strings
 * @param search_pose search point
 * @return double
 */
double calcAbsLateralOffset(
  const lanelet::ConstLineString2d & boundary_line, const geometry_msgs::msg::Pose & search_pose)
{
  std::vector<geometry_msgs::msg::Point> boundary_path(boundary_line.size());
  for (size_t i = 0; i < boundary_path.size(); ++i) {
    const double x = boundary_line[i].x();
    const double y = boundary_line[i].y();
    boundary_path[i] = autoware::universe_utils::createPoint(x, y, 0.0);
  }

  return std::fabs(autoware::motion_utils::calcLateralOffset(boundary_path, search_pose.position));
}

/**
 * @brief init lateral kinematics struct
 *
 * @param lanelet closest lanelet
 * @param pose search pose
 * @return lateral kinematics data struct
 */
LateralKinematicsToLanelet initLateralKinematics(
  const lanelet::ConstLanelet & lanelet, geometry_msgs::msg::Pose pose)
{
  LateralKinematicsToLanelet lateral_kinematics;

  const lanelet::ConstLineString2d left_bound = lanelet.leftBound2d();
  const lanelet::ConstLineString2d right_bound = lanelet.rightBound2d();
  const double left_dist = calcAbsLateralOffset(left_bound, pose);
  const double right_dist = calcAbsLateralOffset(right_bound, pose);

  // calc boundary distance
  lateral_kinematics.dist_from_left_boundary = left_dist;
  lateral_kinematics.dist_from_right_boundary = right_dist;
  // velocities are not init in the first step
  lateral_kinematics.left_lateral_velocity = 0;
  lateral_kinematics.right_lateral_velocity = 0;
  lateral_kinematics.filtered_left_lateral_velocity = 0;
  lateral_kinematics.filtered_right_lateral_velocity = 0;
  return lateral_kinematics;
}

/**
 * @brief calc lateral velocity and filtered velocity of object in a lanelet
 *
 * @param prev_lateral_kinematics previous lateral lanelet kinematics
 * @param current_lateral_kinematics current lateral lanelet kinematics
 * @param dt sampling time [s]
 */
void calcLateralKinematics(
  const LateralKinematicsToLanelet & prev_lateral_kinematics,
  LateralKinematicsToLanelet & current_lateral_kinematics, const double dt, const double cutoff)
{
  // calc velocity via backward difference
  current_lateral_kinematics.left_lateral_velocity =
    (current_lateral_kinematics.dist_from_left_boundary -
     prev_lateral_kinematics.dist_from_left_boundary) /
    dt;
  current_lateral_kinematics.right_lateral_velocity =
    (current_lateral_kinematics.dist_from_right_boundary -
     prev_lateral_kinematics.dist_from_right_boundary) /
    dt;

  // low pass filtering left velocity: default cut_off is 0.6 Hz
  current_lateral_kinematics.filtered_left_lateral_velocity = FirstOrderLowpassFilter(
    prev_lateral_kinematics.filtered_left_lateral_velocity,
    prev_lateral_kinematics.left_lateral_velocity, current_lateral_kinematics.left_lateral_velocity,
    dt, cutoff);
  current_lateral_kinematics.filtered_right_lateral_velocity = FirstOrderLowpassFilter(
    prev_lateral_kinematics.filtered_right_lateral_velocity,
    prev_lateral_kinematics.right_lateral_velocity,
    current_lateral_kinematics.right_lateral_velocity, dt, cutoff);
}

/**
 * @brief look for matching lanelet between current/previous object state and calculate velocity
 *
 * @param prev_obj previous ObjectData
 * @param current_obj current ObjectData to be updated
 * @param routing_graph_ptr_ routing graph pointer
 */
void updateLateralKinematicsVector(
  const ObjectData & prev_obj, ObjectData & current_obj,
  const lanelet::routing::RoutingGraphPtr routing_graph_ptr_, const double lowpass_cutoff)
{
  const double dt = (current_obj.header.stamp.sec - prev_obj.header.stamp.sec) +
                    (current_obj.header.stamp.nanosec - prev_obj.header.stamp.nanosec) * 1e-9;
  if (dt < 1e-6) {
    return;  // do not update
  }

  // look for matching lanelet between current and previous kinematics
  for (auto & current_set : current_obj.lateral_kinematics_set) {
    const auto & current_lane = current_set.first;
    auto & current_lateral_kinematics = current_set.second;

    // 1. has same lanelet
    if (prev_obj.lateral_kinematics_set.count(current_lane) != 0) {
      const auto & prev_lateral_kinematics = prev_obj.lateral_kinematics_set.at(current_lane);
      calcLateralKinematics(
        prev_lateral_kinematics, current_lateral_kinematics, dt, lowpass_cutoff);
      break;
    }
    // 2. successive lanelet
    for (auto & prev_set : prev_obj.lateral_kinematics_set) {
      const auto & prev_lane = prev_set.first;
      const auto & prev_lateral_kinematics = prev_set.second;
      const bool successive_lanelet =
        routing_graph_ptr_->routingRelation(prev_lane, current_lane) ==
        lanelet::routing::RelationType::Successor;
      if (successive_lanelet) {  // lanelet can be connected
        calcLateralKinematics(
          prev_lateral_kinematics, current_lateral_kinematics, dt,
          lowpass_cutoff);  // calc velocity
        break;
      }
    }
  }
}

/**
 * @brief Get the Right LineSharing Lanelets object
 *
 * @param current_lanelet
 * @param lanelet_map_ptr
 * @return lanelet::ConstLanelets
 */
lanelet::ConstLanelets getRightLineSharingLanelets(
  const lanelet::ConstLanelet & current_lanelet, const lanelet::LaneletMapPtr & lanelet_map_ptr)
{
  lanelet::ConstLanelets
    output_lanelets;  // create an empty container of type lanelet::ConstLanelets

  // step1: look for lane sharing current right bound
  lanelet::Lanelets right_lane_candidates =
    lanelet_map_ptr->laneletLayer.findUsages(current_lanelet.rightBound());
  for (auto & candidate : right_lane_candidates) {
    // exclude self lanelet
    if (candidate == current_lanelet) continue;
    // if candidate has linestring as left bound, assign it to output
    if (candidate.leftBound() == current_lanelet.rightBound()) {
      output_lanelets.push_back(candidate);
    }
  }
  return output_lanelets;  // return empty
}

/**
 * @brief Get the Left LineSharing Lanelets object
 *
 * @param current_lanelet
 * @param lanelet_map_ptr
 * @return lanelet::ConstLanelets
 */
lanelet::ConstLanelets getLeftLineSharingLanelets(
  const lanelet::ConstLanelet & current_lanelet, const lanelet::LaneletMapPtr & lanelet_map_ptr)
{
  lanelet::ConstLanelets
    output_lanelets;  // create an empty container of type lanelet::ConstLanelets

  // step1: look for lane sharing current left bound
  lanelet::Lanelets left_lane_candidates =
    lanelet_map_ptr->laneletLayer.findUsages(current_lanelet.leftBound());
  for (auto & candidate : left_lane_candidates) {
    // exclude self lanelet
    if (candidate == current_lanelet) continue;
    // if candidate has linestring as right bound, assign it to output
    if (candidate.rightBound() == current_lanelet.leftBound()) {
      output_lanelets.push_back(candidate);
    }
  }
  return output_lanelets;  // return empty
}

/**
 * @brief Check if the lanelet is isolated in routing graph
 * @param current_lanelet
 * @param lanelet_map_ptr
 */
bool isIsolatedLanelet(
  const lanelet::ConstLanelet & lanelet, lanelet::routing::RoutingGraphPtr & graph)
{
  const auto & following_lanelets = graph->following(lanelet);
  const auto & left_lanelets = graph->lefts(lanelet);
  const auto & right_lanelets = graph->rights(lanelet);
  return left_lanelets.empty() && right_lanelets.empty() && following_lanelets.empty();
}

/**
 * @brief Get the Possible Paths For Isolated Lanelet object
 * @param lanelet
 * @return lanelet::routing::LaneletPaths
 */
lanelet::routing::LaneletPaths getPossiblePathsForIsolatedLanelet(
  const lanelet::ConstLanelet & lanelet)
{
  lanelet::ConstLanelets possible_lanelets;
  possible_lanelets.push_back(lanelet);
  lanelet::routing::LaneletPaths possible_paths;
  // need to initialize path with constant lanelets
  lanelet::routing::LaneletPath possible_path(possible_lanelets);
  possible_paths.push_back(possible_path);
  return possible_paths;
}

/**
 * @brief validate isolated lanelet length has enough length for prediction
 * @param lanelet
 * @param object: object information for calc length threshold
 * @param prediction_time: time horizon[s] for calc length threshold
 * @return bool
 */
bool validateIsolatedLaneletLength(
  const lanelet::ConstLanelet & lanelet, const TrackedObject & object, const double prediction_time)
{
  // get closest center line point to object
  const auto & center_line = lanelet.centerline2d();
  const auto & obj_pos = object.kinematics.pose_with_covariance.pose.position;
  const lanelet::BasicPoint2d obj_point(obj_pos.x, obj_pos.y);
  // get end point of the center line
  const auto & end_point = center_line.back();
  // calc approx distance between closest point and end point
  const double approx_distance = lanelet::geometry::distance2d(obj_point, end_point);
  // calc min length for prediction
  const double abs_speed = std::hypot(
    object.kinematics.twist_with_covariance.twist.linear.x,
    object.kinematics.twist_with_covariance.twist.linear.y);
  const double min_length = abs_speed * prediction_time;
  return approx_distance > min_length;
}

lanelet::ConstLanelets getLanelets(const map_based_prediction::LaneletsData & data)
{
  lanelet::ConstLanelets lanelets;
  for (const auto & lanelet_data : data) {
    lanelets.push_back(lanelet_data.lanelet);
  }

  return lanelets;
}

void replaceObjectYawWithLaneletsYaw(
  const LaneletsData & current_lanelets, TrackedObject & transformed_object)
{
  // return if no lanelet is found
  if (current_lanelets.empty()) return;
  auto & pose_with_cov = transformed_object.kinematics.pose_with_covariance;
  // for each lanelet, calc lanelet angle and calculate mean angle
  double sum_x = 0.0;
  double sum_y = 0.0;
  for (const auto & current_lanelet : current_lanelets) {
    const auto lanelet_angle =
      lanelet::utils::getLaneletAngle(current_lanelet.lanelet, pose_with_cov.pose.position);
    sum_x += std::cos(lanelet_angle);
    sum_y += std::sin(lanelet_angle);
  }
  const double mean_yaw_angle = std::atan2(sum_y, sum_x);
  double roll, pitch, yaw;
  tf2::Quaternion original_quaternion;
  tf2::fromMsg(pose_with_cov.pose.orientation, original_quaternion);
  tf2::Matrix3x3(original_quaternion).getRPY(roll, pitch, yaw);
  tf2::Quaternion filtered_quaternion;
  filtered_quaternion.setRPY(roll, pitch, mean_yaw_angle);
  pose_with_cov.pose.orientation = tf2::toMsg(filtered_quaternion);
}

}  // namespace

MapBasedPredictionNode::MapBasedPredictionNode(const rclcpp::NodeOptions & node_options)
: Node("map_based_prediction", node_options)
{
  if (!google::IsGoogleLoggingInitialized()) {
    google::InitGoogleLogging("map_based_prediction_node");
    google::InstallFailureSignalHandler();
  }
  enable_delay_compensation_ = declare_parameter<bool>("enable_delay_compensation");
  prediction_time_horizon_.vehicle = declare_parameter<double>("prediction_time_horizon.vehicle");
  prediction_time_horizon_.pedestrian =
    declare_parameter<double>("prediction_time_horizon.pedestrian");
  prediction_time_horizon_.unknown = declare_parameter<double>("prediction_time_horizon.unknown");
  lateral_control_time_horizon_ =
    declare_parameter<double>("lateral_control_time_horizon");  // [s] for lateral control point
  prediction_sampling_time_interval_ = declare_parameter<double>("prediction_sampling_delta_time");
  min_velocity_for_map_based_prediction_ =
    declare_parameter<double>("min_velocity_for_map_based_prediction");

  dist_threshold_for_searching_lanelet_ =
    declare_parameter<double>("dist_threshold_for_searching_lanelet");
  delta_yaw_threshold_for_searching_lanelet_ =
    declare_parameter<double>("delta_yaw_threshold_for_searching_lanelet");
  sigma_lateral_offset_ = declare_parameter<double>("sigma_lateral_offset");
  sigma_yaw_angle_deg_ = declare_parameter<double>("sigma_yaw_angle_deg");
  object_buffer_time_length_ = declare_parameter<double>("object_buffer_time_length");
  history_time_length_ = declare_parameter<double>("history_time_length");

  check_lateral_acceleration_constraints_ =
    declare_parameter<bool>("check_lateral_acceleration_constraints");
  max_lateral_accel_ = declare_parameter<double>("max_lateral_accel");
  min_acceleration_before_curve_ = declare_parameter<double>("min_acceleration_before_curve");

  {  // lane change detection
    lane_change_detection_method_ = declare_parameter<std::string>("lane_change_detection.method");

    // lane change detection by time_to_change_lane
    dist_threshold_to_bound_ = declare_parameter<double>(
      "lane_change_detection.time_to_change_lane.dist_threshold_for_lane_change_detection");  // 1m
    time_threshold_to_bound_ = declare_parameter<double>(
      "lane_change_detection.time_to_change_lane.time_threshold_for_lane_change_detection");
    cutoff_freq_of_velocity_lpf_ = declare_parameter<double>(
      "lane_change_detection.time_to_change_lane.cutoff_freq_of_velocity_for_lane_change_"
      "detection");

    // lane change detection by lat_diff_distance
    dist_ratio_threshold_to_left_bound_ = declare_parameter<double>(
      "lane_change_detection.lat_diff_distance.dist_ratio_threshold_to_left_bound");
    dist_ratio_threshold_to_right_bound_ = declare_parameter<double>(
      "lane_change_detection.lat_diff_distance.dist_ratio_threshold_to_right_bound");
    diff_dist_threshold_to_left_bound_ = declare_parameter<double>(
      "lane_change_detection.lat_diff_distance.diff_dist_threshold_to_left_bound");
    diff_dist_threshold_to_right_bound_ = declare_parameter<double>(
      "lane_change_detection.lat_diff_distance.diff_dist_threshold_to_right_bound");

    num_continuous_state_transition_ =
      declare_parameter<int>("lane_change_detection.num_continuous_state_transition");

    consider_only_routable_neighbours_ =
      declare_parameter<bool>("lane_change_detection.consider_only_routable_neighbours");
  }
  reference_path_resolution_ = declare_parameter<double>("reference_path_resolution");
  /* prediction path will disabled when the estimated path length exceeds lanelet length. This
   * parameter control the estimated path length = vx * th * (rate)  */
  prediction_time_horizon_rate_for_validate_lane_length_ =
    declare_parameter<double>("prediction_time_horizon_rate_for_validate_shoulder_lane_length");

  use_vehicle_acceleration_ = declare_parameter<bool>("use_vehicle_acceleration");
  speed_limit_multiplier_ = declare_parameter<double>("speed_limit_multiplier");
  acceleration_exponential_half_life_ =
    declare_parameter<double>("acceleration_exponential_half_life");

  // initialize VRU predictor
  predictor_vru_ = std::make_unique<PredictorVru>(*this);

  // VRU parameters
  remember_lost_crosswalk_users_ =
    declare_parameter<bool>("use_crosswalk_user_history.remember_lost_users");
  {
    bool match_lost_and_appeared_crosswalk_users =
      declare_parameter<bool>("use_crosswalk_user_history.match_lost_and_appeared_users");
    double min_crosswalk_user_velocity = declare_parameter<double>("min_crosswalk_user_velocity");
    double max_crosswalk_user_delta_yaw_threshold_for_lanelet =
      declare_parameter<double>("max_crosswalk_user_delta_yaw_threshold_for_lanelet");
    bool use_crosswalk_signal =
      declare_parameter<bool>("crosswalk_with_signal.use_crosswalk_signal");
    double threshold_velocity_assumed_as_stopping =
      declare_parameter<double>("crosswalk_with_signal.threshold_velocity_assumed_as_stopping");
    std::vector<double> distance_set_for_no_intention_to_walk =
      declare_parameter<std::vector<double>>(
        "crosswalk_with_signal.distance_set_for_no_intention_to_walk");
    std::vector<double> timeout_set_for_no_intention_to_walk =
      declare_parameter<std::vector<double>>(
        "crosswalk_with_signal.timeout_set_for_no_intention_to_walk");
    predictor_vru_->setParameters(
      match_lost_and_appeared_crosswalk_users, min_crosswalk_user_velocity,
      max_crosswalk_user_delta_yaw_threshold_for_lanelet, use_crosswalk_signal,
      threshold_velocity_assumed_as_stopping, distance_set_for_no_intention_to_walk,
      timeout_set_for_no_intention_to_walk, prediction_sampling_time_interval_,
      prediction_time_horizon_.pedestrian);
  }

  // debug parameter
  bool use_time_publisher = declare_parameter<bool>("publish_processing_time");
  bool use_time_keeper = declare_parameter<bool>("publish_processing_time_detail");
  bool use_debug_marker = declare_parameter<bool>("publish_debug_markers");

  // initialize path generator
  path_generator_ = std::make_shared<PathGenerator>(prediction_sampling_time_interval_);

  path_generator_->setUseVehicleAcceleration(use_vehicle_acceleration_);
  path_generator_->setAccelerationHalfLife(acceleration_exponential_half_life_);

  // subscribers
  sub_objects_ = this->create_subscription<TrackedObjects>(
    "~/input/objects", 1,
    std::bind(&MapBasedPredictionNode::objectsCallback, this, std::placeholders::_1));
  sub_map_ = this->create_subscription<LaneletMapBin>(
    "/vector_map", rclcpp::QoS{1}.transient_local(),
    std::bind(&MapBasedPredictionNode::mapCallback, this, std::placeholders::_1));

  // publishers
  pub_objects_ = this->create_publisher<PredictedObjects>("~/output/objects", rclcpp::QoS{1});

  // debug publishers
  if (use_time_publisher) {
    processing_time_publisher_ =
      std::make_unique<autoware::universe_utils::DebugPublisher>(this, "map_based_prediction");
    published_time_publisher_ =
      std::make_unique<autoware::universe_utils::PublishedTimePublisher>(this);
    stop_watch_ptr_ =
      std::make_unique<autoware::universe_utils::StopWatch<std::chrono::milliseconds>>();
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");
  }

  if (use_time_keeper) {
    detailed_processing_time_publisher_ =
      this->create_publisher<autoware::universe_utils::ProcessingTimeDetail>(
        "~/debug/processing_time_detail_ms", 1);
    auto time_keeper = autoware::universe_utils::TimeKeeper(detailed_processing_time_publisher_);
    time_keeper_ = std::make_shared<autoware::universe_utils::TimeKeeper>(time_keeper);
    path_generator_->setTimeKeeper(time_keeper_);
    predictor_vru_->setTimeKeeper(time_keeper_);
  }

  if (use_debug_marker) {
    pub_debug_markers_ =
      this->create_publisher<visualization_msgs::msg::MarkerArray>("maneuver", rclcpp::QoS{1});
  }
  // dynamic reconfigure
  set_param_res_ = this->add_on_set_parameters_callback(
    std::bind(&MapBasedPredictionNode::onParam, this, std::placeholders::_1));
}

rcl_interfaces::msg::SetParametersResult MapBasedPredictionNode::onParam(
  const std::vector<rclcpp::Parameter> & parameters)
{
  using autoware::universe_utils::updateParam;

  updateParam(parameters, "max_lateral_accel", max_lateral_accel_);
  updateParam(parameters, "min_acceleration_before_curve", min_acceleration_before_curve_);
  updateParam(
    parameters, "check_lateral_acceleration_constraints", check_lateral_acceleration_constraints_);
  updateParam(parameters, "use_vehicle_acceleration", use_vehicle_acceleration_);
  updateParam(parameters, "speed_limit_multiplier", speed_limit_multiplier_);
  updateParam(
    parameters, "acceleration_exponential_half_life", acceleration_exponential_half_life_);

  path_generator_->setUseVehicleAcceleration(use_vehicle_acceleration_);
  path_generator_->setAccelerationHalfLife(acceleration_exponential_half_life_);

  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;
  result.reason = "success";
  return result;
}

void MapBasedPredictionNode::mapCallback(const LaneletMapBin::ConstSharedPtr msg)
{
  RCLCPP_DEBUG(get_logger(), "[Map Based Prediction]: Start loading lanelet");
  lanelet_map_ptr_ = std::make_shared<lanelet::LaneletMap>();
  lanelet::utils::conversion::fromBinMsg(
    *msg, lanelet_map_ptr_, &traffic_rules_ptr_, &routing_graph_ptr_);
  lru_cache_of_convert_path_type_.clear();  // clear cache
  RCLCPP_DEBUG(get_logger(), "[Map Based Prediction]: Map is loaded");

  predictor_vru_->setLaneletMap(lanelet_map_ptr_);
}

void MapBasedPredictionNode::trafficSignalsCallback(
  const TrafficLightGroupArray::ConstSharedPtr msg)
{
  // load traffic signals to the predictor
  predictor_vru_->setTrafficSignal(*msg);
}

void MapBasedPredictionNode::objectsCallback(const TrackedObjects::ConstSharedPtr in_objects)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  if (stop_watch_ptr_) stop_watch_ptr_->toc("processing_time", true);

  // take traffic_signal
  {
    const auto msg = sub_traffic_signals_.takeData();
    if (msg) {
      trafficSignalsCallback(msg);
    }
  }

  // Guard for map pointer and frame transformation
  if (!lanelet_map_ptr_) {
    return;
  }

  // get world to map transform
  geometry_msgs::msg::TransformStamped::ConstSharedPtr world2map_transform;
  bool is_object_not_in_map_frame = in_objects->header.frame_id != "map";
  if (is_object_not_in_map_frame) {
    world2map_transform = transform_listener_.getTransform(
      "map",                        // target
      in_objects->header.frame_id,  // src
      in_objects->header.stamp, rclcpp::Duration::from_seconds(0.1));
    if (!world2map_transform) return;
  }

  // Get objects detected time
  const double objects_detected_time = rclcpp::Time(in_objects->header.stamp).seconds();

  // Remove old objects information in object history
  // road users
  utils::removeOldObjectsHistory(
    objects_detected_time, object_buffer_time_length_, road_users_history_);
  // crosswalk users
  predictor_vru_->removeOldKnownMatches(objects_detected_time, object_buffer_time_length_);

  // result output
  PredictedObjects output;
  output.header = in_objects->header;
  output.header.frame_id = "map";

  // result debug
  visualization_msgs::msg::MarkerArray debug_markers;

  // get current crosswalk users for later prediction
  predictor_vru_->loadCurrentCrosswalkUsers(*in_objects);

  // for each object
  for (const auto & object : in_objects->objects) {
    TrackedObject transformed_object = object;

    // transform object frame if it's based on map frame
    if (is_object_not_in_map_frame) {
      geometry_msgs::msg::PoseStamped pose_in_map;
      geometry_msgs::msg::PoseStamped pose_orig;
      pose_orig.pose = object.kinematics.pose_with_covariance.pose;
      tf2::doTransform(pose_orig, pose_in_map, *world2map_transform);
      transformed_object.kinematics.pose_with_covariance.pose = pose_in_map.pose;
    }

    // get tracking label and update it for the prediction
    const auto & label_ = transformed_object.classification.front().label;
    const auto label = utils::changeLabelForPrediction(label_, object, lanelet_map_ptr_);

    switch (label) {
      case ObjectClassification::PEDESTRIAN:
      case ObjectClassification::BICYCLE: {
        // Run pedestrian/bicycle prediction
        const auto predicted_vru =
          getPredictionForNonVehicleObject(output.header, transformed_object);
        output.objects.emplace_back(predicted_vru);
        break;
      }
      case ObjectClassification::CAR:
      case ObjectClassification::BUS:
      case ObjectClassification::TRAILER:
      case ObjectClassification::MOTORCYCLE:
      case ObjectClassification::TRUCK: {
        const auto predicted_object_opt = getPredictionForVehicleObject(
          output.header, transformed_object, objects_detected_time, debug_markers);
        if (predicted_object_opt) {
          output.objects.push_back(predicted_object_opt.value());
        }
        break;
      }
      default: {
        auto predicted_unknown_object = utils::convertToPredictedObject(transformed_object);
        PredictedPath predicted_path = path_generator_->generatePathForNonVehicleObject(
          transformed_object, prediction_time_horizon_.unknown);
        predicted_path.confidence = 1.0;

        predicted_unknown_object.kinematics.predicted_paths.push_back(predicted_path);
        output.objects.push_back(predicted_unknown_object);
        break;
      }
    }
  }

  // process lost crosswalk users to tackle unstable detection
  if (remember_lost_crosswalk_users_) {
    PredictedObjects retrieved_objects = predictor_vru_->retrieveUndetectedObjects();
    output.objects.insert(
      output.objects.end(), retrieved_objects.objects.begin(), retrieved_objects.objects.end());
  }

  // Publish Results
  publish(output, debug_markers);

  // Publish Processing Time
  if (stop_watch_ptr_) {
    const auto processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    const auto cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    processing_time_publisher_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    processing_time_publisher_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);
  }
}

void MapBasedPredictionNode::publish(
  const PredictedObjects & output, const visualization_msgs::msg::MarkerArray & debug_markers) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  pub_objects_->publish(output);
  if (published_time_publisher_)
    published_time_publisher_->publish_if_subscribed(pub_objects_, output.header.stamp);
  if (pub_debug_markers_) pub_debug_markers_->publish(debug_markers);
}

void MapBasedPredictionNode::updateObjectData(TrackedObject & object)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  if (
    object.kinematics.orientation_availability ==
    autoware_perception_msgs::msg::TrackedObjectKinematics::AVAILABLE) {
    return;
  }

  // Compute yaw angle from the velocity and position of the object
  const auto & object_pose = object.kinematics.pose_with_covariance.pose;
  const auto & object_twist = object.kinematics.twist_with_covariance.twist;
  const auto future_object_pose = autoware::universe_utils::calcOffsetPose(
    object_pose, object_twist.linear.x * 0.1, object_twist.linear.y * 0.1, 0.0);

  // assumption: the object vx is much larger than vy
  if (object.kinematics.twist_with_covariance.twist.linear.x >= 0.0) return;

  // calculate absolute velocity and do nothing if it is too slow
  const double abs_object_speed = std::hypot(
    object.kinematics.twist_with_covariance.twist.linear.x,
    object.kinematics.twist_with_covariance.twist.linear.y);
  constexpr double min_abs_speed = 1e-1;  // 0.1 m/s
  if (abs_object_speed < min_abs_speed) return;

  switch (object.kinematics.orientation_availability) {
    case autoware_perception_msgs::msg::TrackedObjectKinematics::SIGN_UNKNOWN: {
      const auto original_yaw =
        tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
      // flip the angle
      object.kinematics.pose_with_covariance.pose.orientation =
        autoware::universe_utils::createQuaternionFromYaw(
          autoware::universe_utils::pi + original_yaw);
      break;
    }
    default: {
      const auto updated_object_yaw = autoware::universe_utils::calcAzimuthAngle(
        object_pose.position, future_object_pose.position);

      object.kinematics.pose_with_covariance.pose.orientation =
        autoware::universe_utils::createQuaternionFromYaw(updated_object_yaw);
      break;
    }
  }
  object.kinematics.twist_with_covariance.twist.linear.x *= -1.0;
  object.kinematics.twist_with_covariance.twist.linear.y *= -1.0;

  return;
}

LaneletsData MapBasedPredictionNode::getCurrentLanelets(const TrackedObject & object)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  return utils::getCurrentLanelets(
    object, lanelet_map_ptr_, road_users_history_, dist_threshold_for_searching_lanelet_,
    delta_yaw_threshold_for_searching_lanelet_, sigma_lateral_offset_, sigma_yaw_angle_deg_);
}

void MapBasedPredictionNode::updateRoadUsersHistory(
  const std_msgs::msg::Header & header, const TrackedObject & object,
  const LaneletsData & current_lanelets_data)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  std::string object_id = autoware::universe_utils::toHexString(object.object_id);
  const auto current_lanelets = getLanelets(current_lanelets_data);

  ObjectData single_object_data;
  single_object_data.header = header;
  single_object_data.current_lanelets = current_lanelets;
  single_object_data.future_possible_lanelets = current_lanelets;
  single_object_data.pose = object.kinematics.pose_with_covariance.pose;
  const double object_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
  single_object_data.pose.orientation =
    autoware::universe_utils::createQuaternionFromYaw(object_yaw);
  single_object_data.time_delay = std::fabs((this->get_clock()->now() - header.stamp).seconds());
  single_object_data.twist = object.kinematics.twist_with_covariance.twist;

  // Init lateral kinematics
  for (const auto & current_lane : current_lanelets) {
    const LateralKinematicsToLanelet lateral_kinematics =
      initLateralKinematics(current_lane, single_object_data.pose);
    single_object_data.lateral_kinematics_set[current_lane] = lateral_kinematics;
  }

  if (road_users_history_.count(object_id) == 0) {
    // New Object(Create a new object in object histories)
    std::deque<ObjectData> object_data = {single_object_data};
    road_users_history_.emplace(object_id, object_data);
  } else {
    // Object that is already in the object buffer
    std::deque<ObjectData> & object_data = road_users_history_.at(object_id);
    // get previous object data and update
    const auto prev_object_data = object_data.back();
    updateLateralKinematicsVector(
      prev_object_data, single_object_data, routing_graph_ptr_, cutoff_freq_of_velocity_lpf_);

    object_data.push_back(single_object_data);
  }
}

std::vector<LaneletPathWithPathInfo> MapBasedPredictionNode::getPredictedReferencePath(
  const TrackedObject & object, const LaneletsData & current_lanelets_data,
  const double object_detected_time, const double time_horizon)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // Step 1. Set conditions for the prediction
  const double obj_vel = std::hypot(
    object.kinematics.twist_with_covariance.twist.linear.x,
    object.kinematics.twist_with_covariance.twist.linear.y);

  // Using a decaying acceleration model. Consult the README for more information about the model.
  const double obj_acc = (use_vehicle_acceleration_)
                           ? std::hypot(
                               object.kinematics.acceleration_with_covariance.accel.linear.x,
                               object.kinematics.acceleration_with_covariance.accel.linear.y)
                           : 0.0;
  const double t_h = time_horizon;
  const double lambda = std::log(2) / acceleration_exponential_half_life_;
  const double validate_time_horizon = t_h * prediction_time_horizon_rate_for_validate_lane_length_;
  const double final_speed_after_acceleration =
    obj_vel + obj_acc * (1.0 / lambda) * (1.0 - std::exp(-lambda * t_h));

  auto get_search_distance_with_decaying_acc = [&]() -> double {
    const double acceleration_distance =
      obj_acc * (1.0 / lambda) * t_h +
      obj_acc * (1.0 / (lambda * lambda)) * std::expm1(-lambda * t_h);
    double search_dist = acceleration_distance + obj_vel * t_h;
    return search_dist;
  };

  auto get_search_distance_with_partial_acc = [&](const double final_speed) -> double {
    constexpr double epsilon = 1E-5;
    if (std::abs(obj_acc) < epsilon) {
      // Assume constant speed
      return obj_vel * t_h;
    }
    // Time to reach final speed
    const double t_f = (-1.0 / lambda) * std::log(1 - ((final_speed - obj_vel) * lambda) / obj_acc);
    // It is assumed the vehicle accelerates until final_speed is reached and
    // then continues at constant speed for the rest of the time horizon
    const double search_dist =
      // Distance covered while accelerating
      obj_acc * (1.0 / lambda) * t_f +
      obj_acc * (1.0 / std::pow(lambda, 2)) * std::expm1(-lambda * t_f) + obj_vel * t_f +
      // Distance covered at constant speed
      final_speed * (t_h - t_f);
    return search_dist;
  };

  std::string object_id = autoware::universe_utils::toHexString(object.object_id);
  geometry_msgs::msg::Pose object_pose = object.kinematics.pose_with_covariance.pose;

  // Step 2. Get possible paths for each lanelet
  std::vector<LaneletPathWithPathInfo> lanelet_ref_paths;
  for (const auto & current_lanelet_data : current_lanelets_data) {
    std::vector<LaneletPathWithPathInfo> ref_paths_per_lanelet;

    // Set condition on each lanelet
    lanelet::routing::PossiblePathsParams possible_params{0, {}, 0, false, true};
    double target_speed_limit = 0.0;
    {
      const lanelet::traffic_rules::SpeedLimitInformation limit =
        traffic_rules_ptr_->speedLimit(current_lanelet_data.lanelet);
      const double legal_speed_limit = static_cast<double>(limit.speedLimit.value());
      target_speed_limit = legal_speed_limit * speed_limit_multiplier_;
      const bool final_speed_surpasses_limit = final_speed_after_acceleration > target_speed_limit;
      const bool object_has_surpassed_limit_already = obj_vel > target_speed_limit;

      double search_dist = (final_speed_surpasses_limit && !object_has_surpassed_limit_already)
                             ? get_search_distance_with_partial_acc(target_speed_limit)
                             : get_search_distance_with_decaying_acc();
      search_dist += lanelet::utils::getLaneletLength3d(current_lanelet_data.lanelet);
      possible_params.routingCostLimit = search_dist;
    }

    // lambda function to get possible paths for isolated lanelet
    // isolated is often caused by lanelet with no connection e.g. shoulder-lane
    auto getPathsForNormalOrIsolatedLanelet = [&](const lanelet::ConstLanelet & lanelet) {
      // if lanelet is not isolated, return normal possible paths
      if (!isIsolatedLanelet(lanelet, routing_graph_ptr_)) {
        return routing_graph_ptr_->possiblePaths(lanelet, possible_params);
      }
      // if lanelet is isolated, check if it has enough length
      if (!validateIsolatedLaneletLength(lanelet, object, validate_time_horizon)) {
        return lanelet::routing::LaneletPaths{};
      } else {
        // if lanelet has enough length, return possible paths
        return getPossiblePathsForIsolatedLanelet(lanelet);
      }
    };

    // lambda function to extract left/right lanelets
    auto getLeftOrRightLanelets = [&](
                                    const lanelet::ConstLanelet & lanelet,
                                    const bool get_left) -> std::optional<lanelet::ConstLanelet> {
      const auto opt =
        get_left ? routing_graph_ptr_->left(lanelet) : routing_graph_ptr_->right(lanelet);
      if (!!opt) {
        return *opt;
      }
      if (!consider_only_routable_neighbours_) {
        const auto adjacent = get_left ? routing_graph_ptr_->adjacentLeft(lanelet)
                                       : routing_graph_ptr_->adjacentRight(lanelet);
        if (!!adjacent) {
          return *adjacent;
        }
        // search for unconnected lanelet
        const auto unconnected_lanelets =
          get_left ? getLeftLineSharingLanelets(lanelet, lanelet_map_ptr_)
                   : getRightLineSharingLanelets(lanelet, lanelet_map_ptr_);
        // just return first candidate of unconnected lanelet for now
        if (!unconnected_lanelets.empty()) {
          return unconnected_lanelets.front();
        }
      }

      // if no candidate lanelet found, return empty
      return std::nullopt;
    };

    bool left_paths_exists = false;
    bool right_paths_exists = false;
    bool center_paths_exists = false;

    // a-1. Get the left lanelet
    {
      PredictedRefPath ref_path_info;
      lanelet::routing::LaneletPaths left_paths;
      const auto left_lanelet = getLeftOrRightLanelets(current_lanelet_data.lanelet, true);
      if (!!left_lanelet) {
        left_paths = getPathsForNormalOrIsolatedLanelet(left_lanelet.value());
        left_paths_exists = !left_paths.empty();
      }
      ref_path_info.speed_limit = target_speed_limit;
      ref_path_info.maneuver = Maneuver::LEFT_LANE_CHANGE;
      for (auto & path : left_paths) {
        ref_paths_per_lanelet.emplace_back(path, ref_path_info);
      }
    }

    // a-2. Get the right lanelet
    {
      PredictedRefPath ref_path_info;
      lanelet::routing::LaneletPaths right_paths;
      const auto right_lanelet = getLeftOrRightLanelets(current_lanelet_data.lanelet, false);
      if (!!right_lanelet) {
        right_paths = getPathsForNormalOrIsolatedLanelet(right_lanelet.value());
        right_paths_exists = !right_paths.empty();
      }
      ref_path_info.speed_limit = target_speed_limit;
      ref_path_info.maneuver = Maneuver::RIGHT_LANE_CHANGE;
      for (auto & path : right_paths) {
        ref_paths_per_lanelet.emplace_back(path, ref_path_info);
      }
    }

    // a-3. Get the center lanelet
    {
      PredictedRefPath ref_path_info;
      lanelet::routing::LaneletPaths center_paths =
        getPathsForNormalOrIsolatedLanelet(current_lanelet_data.lanelet);
      center_paths_exists = !center_paths.empty();
      ref_path_info.speed_limit = target_speed_limit;
      ref_path_info.maneuver = Maneuver::LANE_FOLLOW;
      for (auto & path : center_paths) {
        ref_paths_per_lanelet.emplace_back(path, ref_path_info);
      }
    }

    // Skip calculations if all paths are empty
    if (ref_paths_per_lanelet.empty()) {
      continue;
    }

    // b. Predict Object Maneuver
    const Maneuver predicted_maneuver =
      predictObjectManeuver(object_id, object_pose, current_lanelet_data, object_detected_time);

    // c. Allocate probability for each predicted maneuver
    const float & path_prob = current_lanelet_data.probability;
    const auto maneuver_prob = calculateManeuverProbability(
      predicted_maneuver, left_paths_exists, right_paths_exists, center_paths_exists);
    for (auto & ref_path : ref_paths_per_lanelet) {
      auto & ref_path_info = ref_path.second;
      ref_path_info.probability = maneuver_prob.at(ref_path_info.maneuver) * path_prob;
    }

    // move the calculated ref paths to the lanelet_ref_paths
    lanelet_ref_paths.insert(
      lanelet_ref_paths.end(), ref_paths_per_lanelet.begin(), ref_paths_per_lanelet.end());
  }

  // update future possible lanelets
  if (road_users_history_.count(object_id) != 0) {
    std::vector<lanelet::ConstLanelet> & possible_lanelets =
      road_users_history_.at(object_id).back().future_possible_lanelets;
    for (const auto & ref_path : lanelet_ref_paths) {
      for (const auto & lanelet : ref_path.first) {
        if (
          std::find(possible_lanelets.begin(), possible_lanelets.end(), lanelet) ==
          possible_lanelets.end()) {
          possible_lanelets.push_back(lanelet);
        }
      }
    }
  }

  return lanelet_ref_paths;
}

/**
 * @brief Do lane change prediction
 * @return predicted manuever (lane follow, left/right lane change)
 */
Maneuver MapBasedPredictionNode::predictObjectManeuver(
  const std::string & object_id, const geometry_msgs::msg::Pose & object_pose,
  const LaneletData & current_lanelet_data, const double object_detected_time)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // calculate maneuver
  const auto current_maneuver = [&]() {
    if (lane_change_detection_method_ == "time_to_change_lane") {
      return predictObjectManeuverByTimeToLaneChange(
        object_id, current_lanelet_data, object_detected_time);
    } else if (lane_change_detection_method_ == "lat_diff_distance") {
      return predictObjectManeuverByLatDiffDistance(
        object_id, object_pose, current_lanelet_data, object_detected_time);
    }
    throw std::logic_error("Lane change detection method is invalid.");
  }();

  if (road_users_history_.count(object_id) == 0) {
    return current_maneuver;
  }
  auto & object_info = road_users_history_.at(object_id);

  // update maneuver in object history
  if (!object_info.empty()) {
    object_info.back().one_shot_maneuver = current_maneuver;
  }

  // decide maneuver considering previous results
  if (object_info.size() < 2) {
    object_info.back().output_maneuver = current_maneuver;
    return current_maneuver;
  }
  // NOTE: The index of previous maneuver is not object_info.size() - 1
  const auto prev_output_maneuver =
    object_info.at(static_cast<int>(object_info.size()) - 2).output_maneuver;

  for (int i = 0;
       i < std::min(num_continuous_state_transition_, static_cast<int>(object_info.size())); ++i) {
    const auto & tmp_maneuver =
      object_info.at(static_cast<int>(object_info.size()) - 1 - i).one_shot_maneuver;
    if (tmp_maneuver != current_maneuver) {
      object_info.back().output_maneuver = prev_output_maneuver;
      return prev_output_maneuver;
    }
  }

  object_info.back().output_maneuver = current_maneuver;
  return current_maneuver;
}

Maneuver MapBasedPredictionNode::predictObjectManeuverByTimeToLaneChange(
  const std::string & object_id, const LaneletData & current_lanelet_data,
  const double /*object_detected_time*/)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // Step1. Check if we have the object in the buffer
  if (road_users_history_.count(object_id) == 0) {
    return Maneuver::LANE_FOLLOW;
  }

  const std::deque<ObjectData> & object_info = road_users_history_.at(object_id);

  // Step2. Check if object history length longer than history_time_length
  const int latest_id = static_cast<int>(object_info.size()) - 1;
  // object history is not long enough
  if (latest_id < 1) {
    return Maneuver::LANE_FOLLOW;
  }

  // Step3. get object lateral kinematics
  const auto & latest_info = object_info.at(static_cast<size_t>(latest_id));

  bool not_found_corresponding_lanelet = true;
  double left_dist, right_dist;
  double v_left_filtered, v_right_filtered;
  if (latest_info.lateral_kinematics_set.count(current_lanelet_data.lanelet) != 0) {
    const auto & lateral_kinematics =
      latest_info.lateral_kinematics_set.at(current_lanelet_data.lanelet);
    left_dist = lateral_kinematics.dist_from_left_boundary;
    right_dist = lateral_kinematics.dist_from_right_boundary;
    v_left_filtered = lateral_kinematics.filtered_left_lateral_velocity;
    v_right_filtered = lateral_kinematics.filtered_right_lateral_velocity;
    not_found_corresponding_lanelet = false;
  }

  // return lane follow when catch exception
  if (not_found_corresponding_lanelet) {
    return Maneuver::LANE_FOLLOW;
  }

  const double latest_lane_width = left_dist + right_dist;
  if (latest_lane_width < 1e-3) {
    RCLCPP_ERROR(get_logger(), "[Map Based Prediction]: Lane Width is too small");
    return Maneuver::LANE_FOLLOW;
  }

  // Step 4. check time to reach left/right bound
  const double epsilon = 1e-9;
  const double margin_to_reach_left_bound = left_dist / (std::fabs(v_left_filtered) + epsilon);
  const double margin_to_reach_right_bound = right_dist / (std::fabs(v_right_filtered) + epsilon);

  // Step 5. detect lane change
  if (
    left_dist < right_dist &&                              // in left side,
    left_dist < dist_threshold_to_bound_ &&                // close to boundary,
    v_left_filtered < 0 &&                                 // approaching,
    margin_to_reach_left_bound < time_threshold_to_bound_  // will soon arrive to left bound
  ) {
    return Maneuver::LEFT_LANE_CHANGE;
  } else if (
    right_dist < left_dist &&                               // in right side,
    right_dist < dist_threshold_to_bound_ &&                // close to boundary,
    v_right_filtered < 0 &&                                 // approaching,
    margin_to_reach_right_bound < time_threshold_to_bound_  // will soon arrive to right bound
  ) {
    return Maneuver::RIGHT_LANE_CHANGE;
  }

  return Maneuver::LANE_FOLLOW;
}

Maneuver MapBasedPredictionNode::predictObjectManeuverByLatDiffDistance(
  const std::string & object_id, const geometry_msgs::msg::Pose & object_pose,
  const LaneletData & current_lanelet_data, const double /*object_detected_time*/)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // Step1. Check if we have the object in the buffer
  if (road_users_history_.count(object_id) == 0) {
    return Maneuver::LANE_FOLLOW;
  }

  const std::deque<ObjectData> & object_info = road_users_history_.at(object_id);
  const double current_time = (this->get_clock()->now()).seconds();

  // Step2. Get the previous id
  int prev_id = static_cast<int>(object_info.size()) - 1;
  while (prev_id >= 0) {
    const double prev_time_delay = object_info.at(prev_id).time_delay;
    const double prev_time =
      rclcpp::Time(object_info.at(prev_id).header.stamp).seconds() + prev_time_delay;
    // if (object_detected_time - prev_time > history_time_length_) {
    if (current_time - prev_time > history_time_length_) {
      break;
    }
    --prev_id;
  }

  if (prev_id < 0) {
    return Maneuver::LANE_FOLLOW;
  }

  // Step3. Get closest previous lanelet ID
  const auto & prev_info = object_info.at(static_cast<size_t>(prev_id));
  const auto prev_pose = prev_info.pose;
  const lanelet::ConstLanelets prev_lanelets =
    object_info.at(static_cast<size_t>(prev_id)).current_lanelets;
  if (prev_lanelets.empty()) {
    return Maneuver::LANE_FOLLOW;
  }
  lanelet::ConstLanelet prev_lanelet = prev_lanelets.front();
  double closest_prev_yaw = std::numeric_limits<double>::max();
  for (const auto & lanelet : prev_lanelets) {
    const double lane_yaw = lanelet::utils::getLaneletAngle(lanelet, prev_pose.position);
    const double delta_yaw = tf2::getYaw(prev_pose.orientation) - lane_yaw;
    const double normalized_delta_yaw = autoware::universe_utils::normalizeRadian(delta_yaw);
    if (normalized_delta_yaw < closest_prev_yaw) {
      closest_prev_yaw = normalized_delta_yaw;
      prev_lanelet = lanelet;
    }
  }

  // Step4. Check if the vehicle has changed lane
  const auto current_lanelet = current_lanelet_data.lanelet;
  const auto current_pose = object_pose;
  const double dist = autoware::universe_utils::calcDistance2d(prev_pose, current_pose);
  lanelet::routing::LaneletPaths possible_paths =
    routing_graph_ptr_->possiblePaths(prev_lanelet, dist + 2.0, 0, false);
  bool has_lane_changed = true;
  if (prev_lanelet == current_lanelet) {
    has_lane_changed = false;
  } else {
    for (const auto & path : possible_paths) {
      for (const auto & lanelet : path) {
        if (lanelet == current_lanelet) {
          has_lane_changed = false;
          break;
        }
      }
    }
  }

  if (has_lane_changed) {
    return Maneuver::LANE_FOLLOW;
  }

  // Step5. Lane Change Detection
  const lanelet::ConstLineString2d prev_left_bound = prev_lanelet.leftBound2d();
  const lanelet::ConstLineString2d prev_right_bound = prev_lanelet.rightBound2d();
  const lanelet::ConstLineString2d current_left_bound = current_lanelet.leftBound2d();
  const lanelet::ConstLineString2d current_right_bound = current_lanelet.rightBound2d();
  const double prev_left_dist = calcLeftLateralOffset(prev_left_bound, prev_pose);
  const double prev_right_dist = calcRightLateralOffset(prev_right_bound, prev_pose);
  const double current_left_dist = calcLeftLateralOffset(current_left_bound, current_pose);
  const double current_right_dist = calcRightLateralOffset(current_right_bound, current_pose);
  const double prev_lane_width = std::fabs(prev_left_dist) + std::fabs(prev_right_dist);
  const double current_lane_width = std::fabs(current_left_dist) + std::fabs(current_right_dist);
  if (prev_lane_width < 1e-3 || current_lane_width < 1e-3) {
    RCLCPP_ERROR(get_logger(), "[Map Based Prediction]: Lane Width is too small");
    return Maneuver::LANE_FOLLOW;
  }

  const double current_left_dist_ratio = current_left_dist / current_lane_width;
  const double current_right_dist_ratio = current_right_dist / current_lane_width;
  const double diff_left_current_prev = current_left_dist - prev_left_dist;
  const double diff_right_current_prev = current_right_dist - prev_right_dist;

  if (
    current_left_dist_ratio > dist_ratio_threshold_to_left_bound_ &&
    diff_left_current_prev > diff_dist_threshold_to_left_bound_) {
    return Maneuver::LEFT_LANE_CHANGE;
  } else if (
    current_right_dist_ratio < dist_ratio_threshold_to_right_bound_ &&
    diff_right_current_prev < diff_dist_threshold_to_right_bound_) {
    return Maneuver::RIGHT_LANE_CHANGE;
  }

  return Maneuver::LANE_FOLLOW;
}

geometry_msgs::msg::Pose MapBasedPredictionNode::compensateTimeDelay(
  const geometry_msgs::msg::Pose & delayed_pose, const geometry_msgs::msg::Twist & twist,
  const double dt) const
{
  if (!enable_delay_compensation_) {
    return delayed_pose;
  }

  /*  == Nonlinear model ==
   *
   * x_{k+1}   = x_k + vx_k * cos(yaw_k) * dt - vy_k * sin(yaw_k) * dt
   * y_{k+1}   = y_k + vx_k * sin(yaw_k) * dt + vy_k * cos(yaw_k) * dt
   * yaw_{k+1} = yaw_k + (wz_k) * dt
   *
   */

  const double vx = twist.linear.x;
  const double vy = twist.linear.y;
  const double wz = twist.angular.z;
  const double prev_yaw = tf2::getYaw(delayed_pose.orientation);
  const double prev_x = delayed_pose.position.x;
  const double prev_y = delayed_pose.position.y;
  const double prev_z = delayed_pose.position.z;

  const double curr_x = prev_x + vx * std::cos(prev_yaw) * dt - vy * std::sin(prev_yaw) * dt;
  const double curr_y = prev_y + vx * std::sin(prev_yaw) * dt + vy * std::cos(prev_yaw) * dt;
  const double curr_z = prev_z;
  const double curr_yaw = prev_yaw + wz * dt;

  geometry_msgs::msg::Pose current_pose;
  current_pose.position = autoware::universe_utils::createPoint(curr_x, curr_y, curr_z);
  current_pose.orientation = autoware::universe_utils::createQuaternionFromYaw(curr_yaw);

  return current_pose;
}

double MapBasedPredictionNode::calcRightLateralOffset(
  const lanelet::ConstLineString2d & boundary_line, const geometry_msgs::msg::Pose & search_pose)
{
  std::vector<geometry_msgs::msg::Point> boundary_path(boundary_line.size());
  for (size_t i = 0; i < boundary_path.size(); ++i) {
    const double x = boundary_line[i].x();
    const double y = boundary_line[i].y();
    boundary_path[i] = autoware::universe_utils::createPoint(x, y, 0.0);
  }

  return std::fabs(autoware::motion_utils::calcLateralOffset(boundary_path, search_pose.position));
}

double MapBasedPredictionNode::calcLeftLateralOffset(
  const lanelet::ConstLineString2d & boundary_line, const geometry_msgs::msg::Pose & search_pose)
{
  return -calcRightLateralOffset(boundary_line, search_pose);
}

ManeuverProbability MapBasedPredictionNode::calculateManeuverProbability(
  const Maneuver & predicted_maneuver, const bool & left_paths_exists,
  const bool & right_paths_exists, const bool & center_paths_exists) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  float left_lane_change_probability = 0.0;
  float right_lane_change_probability = 0.0;
  float lane_follow_probability = 0.0;
  if (left_paths_exists && predicted_maneuver == Maneuver::LEFT_LANE_CHANGE) {
    constexpr float LF_PROB_WHEN_LC = 0.9;  // probability for lane follow during lane change
    constexpr float LC_PROB_WHEN_LC = 1.0;  // probability for left lane change
    left_lane_change_probability = LC_PROB_WHEN_LC;
    right_lane_change_probability = 0.0;
    lane_follow_probability = LF_PROB_WHEN_LC;
  } else if (right_paths_exists && predicted_maneuver == Maneuver::RIGHT_LANE_CHANGE) {
    constexpr float LF_PROB_WHEN_LC = 0.9;  // probability for lane follow during lane change
    constexpr float RC_PROB_WHEN_LC = 1.0;  // probability for right lane change
    left_lane_change_probability = 0.0;
    right_lane_change_probability = RC_PROB_WHEN_LC;
    lane_follow_probability = LF_PROB_WHEN_LC;
  } else if (center_paths_exists) {
    constexpr float LF_PROB = 1.0;  // probability for lane follow
    constexpr float LC_PROB = 0.3;  // probability for left lane change
    constexpr float RC_PROB = 0.3;  // probability for right lane change
    if (predicted_maneuver == Maneuver::LEFT_LANE_CHANGE) {
      // If prediction says left change, but left lane is empty, assume lane follow
      left_lane_change_probability = 0.0;
      right_lane_change_probability = (right_paths_exists) ? RC_PROB : 0.0;
    } else if (predicted_maneuver == Maneuver::RIGHT_LANE_CHANGE) {
      // If prediction says right change, but right lane is empty, assume lane follow
      left_lane_change_probability = (left_paths_exists) ? LC_PROB : 0.0;
      right_lane_change_probability = 0.0;
    } else {
      // Predicted Maneuver is Lane Follow
      left_lane_change_probability = LC_PROB;
      right_lane_change_probability = RC_PROB;
    }
    lane_follow_probability = LF_PROB;
  } else {
    // Center path is empty
    constexpr float LC_PROB = 1.0;  // probability for left lane change
    constexpr float RC_PROB = 1.0;  // probability for right lane change
    lane_follow_probability = 0.0;

    // If the given lane is empty, the probability goes to 0
    left_lane_change_probability = left_paths_exists ? LC_PROB : 0.0;
    right_lane_change_probability = right_paths_exists ? RC_PROB : 0.0;
  }

  const float MIN_PROBABILITY = 1e-3;
  const float max_prob = std::max(
    MIN_PROBABILITY, std::max(
                       lane_follow_probability,
                       std::max(left_lane_change_probability, right_lane_change_probability)));

  // Insert Normalized Probability
  ManeuverProbability maneuver_prob;
  maneuver_prob[Maneuver::LEFT_LANE_CHANGE] = left_lane_change_probability / max_prob;
  maneuver_prob[Maneuver::RIGHT_LANE_CHANGE] = right_lane_change_probability / max_prob;
  maneuver_prob[Maneuver::LANE_FOLLOW] = lane_follow_probability / max_prob;

  return maneuver_prob;
}

std::vector<PredictedRefPath> MapBasedPredictionNode::convertPredictedReferencePath(
  const TrackedObject & object,
  const std::vector<LaneletPathWithPathInfo> & lanelet_ref_paths) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  std::vector<PredictedRefPath> converted_ref_paths;

  // Step 1. Convert lanelet path to pose path
  for (const auto & ref_path : lanelet_ref_paths) {
    const auto & lanelet_path = ref_path.first;
    const auto & ref_path_info = ref_path.second;
    const auto converted_path = convertLaneletPathToPosePath(lanelet_path);
    PredictedRefPath predicted_path;
    predicted_path.probability = ref_path_info.probability;
    predicted_path.path = converted_path.first;
    predicted_path.width = converted_path.second;
    predicted_path.maneuver = ref_path_info.maneuver;
    predicted_path.speed_limit = ref_path_info.speed_limit;
    converted_ref_paths.push_back(predicted_path);
  }

  // Step 2. Search starting point for each reference path
  for (auto it = converted_ref_paths.begin(); it != converted_ref_paths.end();) {
    auto & pose_path = it->path;
    if (pose_path.empty()) {
      continue;
    }

    const std::optional<size_t> opt_starting_idx =
      searchProperStartingRefPathIndex(object, pose_path);

    if (opt_starting_idx.has_value()) {
      // Trim the reference path
      pose_path.erase(pose_path.begin(), pose_path.begin() + opt_starting_idx.value());
      ++it;
    } else {
      // Proper starting point is not found, remove the reference path
      it = converted_ref_paths.erase(it);
    }
  }

  return converted_ref_paths;
}

std::pair<PosePath, double> MapBasedPredictionNode::convertLaneletPathToPosePath(
  const lanelet::routing::LaneletPath & path) const
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  if (lru_cache_of_convert_path_type_.contains(path)) {
    return *lru_cache_of_convert_path_type_.get(path);
  }

  std::pair<PosePath, double> converted_path_and_width;
  {
    PosePath converted_path;
    double width = 10.0;  // Initialize with a large value

    // Insert Positions. Note that we start inserting points from previous lanelet
    if (!path.empty()) {
      lanelet::ConstLanelets prev_lanelets = routing_graph_ptr_->previous(path.front());
      if (!prev_lanelets.empty()) {
        lanelet::ConstLanelet prev_lanelet = prev_lanelets.front();
        bool init_flag = true;
        geometry_msgs::msg::Pose prev_p;
        for (const auto & lanelet_p : prev_lanelet.centerline()) {
          geometry_msgs::msg::Pose current_p;
          current_p.position = lanelet::utils::conversion::toGeomMsgPt(lanelet_p);
          if (init_flag) {
            init_flag = false;
            prev_p = current_p;
            continue;
          }

          // only considers yaw of the lanelet
          const double lane_yaw = std::atan2(
            current_p.position.y - prev_p.position.y, current_p.position.x - prev_p.position.x);
          const double sin_yaw_half = std::sin(lane_yaw / 2.0);
          const double cos_yaw_half = std::cos(lane_yaw / 2.0);
          current_p.orientation.x = 0.0;
          current_p.orientation.y = 0.0;
          current_p.orientation.z = sin_yaw_half;
          current_p.orientation.w = cos_yaw_half;

          converted_path.push_back(current_p);
          prev_p = current_p;
        }
      }
    }

    for (const auto & lanelet : path) {
      bool init_flag = true;
      geometry_msgs::msg::Pose prev_p;
      for (const auto & lanelet_p : lanelet.centerline()) {
        geometry_msgs::msg::Pose current_p;
        current_p.position = lanelet::utils::conversion::toGeomMsgPt(lanelet_p);
        if (init_flag) {
          init_flag = false;
          prev_p = current_p;
          continue;
        }

        // Prevent from inserting same points
        if (!converted_path.empty()) {
          const auto last_p = converted_path.back();
          const double tmp_dist = autoware::universe_utils::calcDistance2d(last_p, current_p);
          if (tmp_dist < 1e-6) {
            prev_p = current_p;
            continue;
          }
        }

        const double lane_yaw = std::atan2(
          current_p.position.y - prev_p.position.y, current_p.position.x - prev_p.position.x);
        const double sin_yaw_half = std::sin(lane_yaw / 2.0);
        const double cos_yaw_half = std::cos(lane_yaw / 2.0);
        current_p.orientation.x = 0.0;
        current_p.orientation.y = 0.0;
        current_p.orientation.z = sin_yaw_half;
        current_p.orientation.w = cos_yaw_half;

        converted_path.push_back(current_p);
        prev_p = current_p;
      }

      // Update minimum width
      const auto left_bound = lanelet.leftBound2d();
      const auto right_bound = lanelet.rightBound2d();
      const double lanelet_width_front = std::hypot(
        left_bound.front().x() - right_bound.front().x(),
        left_bound.front().y() - right_bound.front().y());
      width = std::min(width, lanelet_width_front);
    }

    // Resample Path
    const bool use_akima_spline_for_xy = true;
    const bool use_lerp_for_z = true;
    // the options use_akima_spline_for_xy and use_lerp_for_z are set to true
    // but the implementation of use_akima_spline_for_xy in resamplePoseVector and
    // resamplePointVector is opposite to the options so the options are set to true to use linear
    // interpolation for xy
    const auto resampled_converted_path = autoware::motion_utils::resamplePoseVector(
      converted_path, reference_path_resolution_, use_akima_spline_for_xy, use_lerp_for_z);
    converted_path_and_width = std::make_pair(resampled_converted_path, width);
  }

  lru_cache_of_convert_path_type_.put(path, converted_path_and_width);
  return converted_path_and_width;
}

PredictedObject MapBasedPredictionNode::getPredictionForNonVehicleObject(
  const std_msgs::msg::Header & header, const TrackedObject & object)
{
  return predictor_vru_->predict(header, object);
}

std::optional<PredictedObject> MapBasedPredictionNode::getPredictionForVehicleObject(
  const std_msgs::msg::Header & header, const TrackedObject & transformed_object,
  const double objects_detected_time, visualization_msgs::msg::MarkerArray & debug_markers)
{
  auto object = transformed_object;

  // Update object yaw and velocity
  updateObjectData(object);

  // Get Closest Lanelet
  const auto current_lanelets = utils::getCurrentLanelets(
    object, lanelet_map_ptr_, road_users_history_, dist_threshold_for_searching_lanelet_,
    delta_yaw_threshold_for_searching_lanelet_, sigma_lateral_offset_, sigma_yaw_angle_deg_);

  // Update Objects History
  updateRoadUsersHistory(header, object, current_lanelets);

  // For off lane obstacles
  if (current_lanelets.empty()) {
    PredictedPath predicted_path =
      path_generator_->generatePathForOffLaneVehicle(object, prediction_time_horizon_.vehicle);
    predicted_path.confidence = 1.0;
    if (predicted_path.path.empty()) {
      return std::nullopt;
    }

    auto predicted_object_vehicle = utils::convertToPredictedObject(object);
    predicted_object_vehicle.kinematics.predicted_paths.push_back(predicted_path);
    return predicted_object_vehicle;
  }

  // For too-slow vehicle
  const double abs_obj_speed = std::hypot(
    object.kinematics.twist_with_covariance.twist.linear.x,
    object.kinematics.twist_with_covariance.twist.linear.y);
  if (std::fabs(abs_obj_speed) < min_velocity_for_map_based_prediction_) {
    PredictedPath predicted_path =
      path_generator_->generatePathForLowSpeedVehicle(object, prediction_time_horizon_.vehicle);
    predicted_path.confidence = 1.0;
    if (predicted_path.path.empty()) {
      return std::nullopt;
    }

    auto predicted_slow_object = utils::convertToPredictedObject(object);
    predicted_slow_object.kinematics.predicted_paths.push_back(predicted_path);
    return predicted_slow_object;
  }

  // Get Predicted Reference Path for Each Maneuver and current lanelets
  // return: <probability, paths>
  const auto lanelet_ref_paths = getPredictedReferencePath(
    object, current_lanelets, objects_detected_time, prediction_time_horizon_.vehicle);
  const auto ref_paths = convertPredictedReferencePath(object, lanelet_ref_paths);

  // If predicted reference path is empty, assume this object is out of the lane
  if (ref_paths.empty()) {
    PredictedPath predicted_path =
      path_generator_->generatePathForOffLaneVehicle(object, prediction_time_horizon_.vehicle);
    predicted_path.confidence = 1.0;
    if (predicted_path.path.empty()) {
      return std::nullopt;
    }

    auto predicted_object_out_of_lane = utils::convertToPredictedObject(object);
    predicted_object_out_of_lane.kinematics.predicted_paths.push_back(predicted_path);
    return predicted_object_out_of_lane;
  }

  // Get Debug Marker for On Lane Vehicles
  if (pub_debug_markers_) {
    const auto max_prob_path = std::max_element(
      ref_paths.begin(), ref_paths.end(),
      [](const PredictedRefPath & a, const PredictedRefPath & b) {
        return a.probability < b.probability;
      });
    const auto debug_marker =
      getDebugMarker(object, max_prob_path->maneuver, debug_markers.markers.size());
    debug_markers.markers.push_back(debug_marker);
  }

  // Fix object angle if its orientation unreliable (e.g. far object by radar sensor)
  // This prevent bending predicted path
  TrackedObject yaw_fixed_object = object;
  if (
    object.kinematics.orientation_availability ==
    autoware_perception_msgs::msg::TrackedObjectKinematics::UNAVAILABLE) {
    replaceObjectYawWithLaneletsYaw(current_lanelets, yaw_fixed_object);
  }
  // Generate Predicted Path
  std::vector<PredictedPath> predicted_paths;
  double min_avg_curvature = std::numeric_limits<double>::max();
  PredictedPath path_with_smallest_avg_curvature;

  for (const auto & ref_path : ref_paths) {
    PredictedPath predicted_path = path_generator_->generatePathForOnLaneVehicle(
      yaw_fixed_object, ref_path.path, prediction_time_horizon_.vehicle,
      lateral_control_time_horizon_, ref_path.width, ref_path.speed_limit);
    if (predicted_path.path.empty()) continue;

    if (!check_lateral_acceleration_constraints_) {
      predicted_path.confidence = ref_path.probability;
      predicted_paths.push_back(predicted_path);
      continue;
    }

    // Check lat. acceleration constraints
    const auto trajectory_with_const_velocity = toTrajectoryPoints(predicted_path, abs_obj_speed);

    if (isLateralAccelerationConstraintSatisfied(
          trajectory_with_const_velocity, prediction_sampling_time_interval_)) {
      predicted_path.confidence = ref_path.probability;
      predicted_paths.push_back(predicted_path);
      continue;
    }

    // Calculate curvature assuming the trajectory points interval is constant
    // In case all paths are deleted, a copy of the straightest path is kept

    constexpr double curvature_calculation_distance = 2.0;
    constexpr double points_interval = 1.0;
    const size_t idx_dist = static_cast<size_t>(
      std::max(static_cast<int>((curvature_calculation_distance) / points_interval), 1));
    const auto curvature_v =
      calcTrajectoryCurvatureFrom3Points(trajectory_with_const_velocity, idx_dist);
    if (curvature_v.empty()) {
      continue;
    }
    const auto curvature_avg =
      std::accumulate(curvature_v.begin(), curvature_v.end(), 0.0) / curvature_v.size();
    if (curvature_avg < min_avg_curvature) {
      min_avg_curvature = curvature_avg;
      path_with_smallest_avg_curvature = predicted_path;
      path_with_smallest_avg_curvature.confidence = ref_path.probability;
    }
  }

  if (predicted_paths.empty()) predicted_paths.push_back(path_with_smallest_avg_curvature);
  // Normalize Path Confidence and output the predicted object

  float sum_confidence = 0.0;
  for (const auto & predicted_path : predicted_paths) {
    sum_confidence += predicted_path.confidence;
  }
  const float min_sum_confidence_value = 1e-3;
  sum_confidence = std::max(sum_confidence, min_sum_confidence_value);

  auto predicted_object = utils::convertToPredictedObject(transformed_object);

  for (auto & predicted_path : predicted_paths) {
    predicted_path.confidence = predicted_path.confidence / sum_confidence;
    if (predicted_object.kinematics.predicted_paths.size() >= 100) break;
    predicted_object.kinematics.predicted_paths.push_back(predicted_path);
  }
  return predicted_object;
}

std::optional<size_t> MapBasedPredictionNode::searchProperStartingRefPathIndex(
  const TrackedObject & object, const PosePath & pose_path) const
{
  std::unique_ptr<ScopedTimeTrack> st1_ptr;
  if (time_keeper_) st1_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  bool is_position_found = false;
  std::optional<size_t> opt_index{std::nullopt};
  auto & index = opt_index.emplace(0);

  // starting segment index is a segment close enough to the object
  const auto obj_point = object.kinematics.pose_with_covariance.pose.position;
  {
    std::unique_ptr<ScopedTimeTrack> st2_ptr;
    if (time_keeper_)
      st2_ptr = std::make_unique<ScopedTimeTrack>("find_close_segment_index", *time_keeper_);
    double min_dist_sq = std::numeric_limits<double>::max();
    constexpr double acceptable_dist_sq = 1.0;  // [m2]
    for (size_t i = 0; i < pose_path.size(); i++) {
      const double dx = pose_path.at(i).position.x - obj_point.x;
      const double dy = pose_path.at(i).position.y - obj_point.y;
      const double dist_sq = dx * dx + dy * dy;
      if (dist_sq < min_dist_sq) {
        min_dist_sq = dist_sq;
        index = i;
      }
      if (dist_sq < acceptable_dist_sq) {
        break;
      }
    }
  }

  // calculate score that object can reach the target path smoothly, and search the
  // starting segment index that have the best score
  size_t idx = 0;
  {  // find target segmentation index
    std::unique_ptr<ScopedTimeTrack> st3_ptr;
    if (time_keeper_)
      st3_ptr = std::make_unique<ScopedTimeTrack>("find_target_seg_index", *time_keeper_);

    constexpr double search_distance = 22.0;       // [m]
    constexpr double yaw_diff_limit = M_PI / 3.0;  // 60 degrees

    const double obj_yaw = tf2::getYaw(object.kinematics.pose_with_covariance.pose.orientation);
    const size_t search_segment_count =
      static_cast<size_t>(std::floor(search_distance / reference_path_resolution_));
    const size_t search_segment_num =
      std::min(search_segment_count, static_cast<size_t>(pose_path.size() - index));

    // search for the best score, which is the smallest
    double best_score = 1e9;  // initial value is large enough
    for (size_t i = 0; i < search_segment_num; ++i) {
      const auto & path_pose = pose_path.at(index + i);
      // yaw difference
      const double path_yaw = tf2::getYaw(path_pose.orientation);
      const double relative_path_yaw = autoware_utils::normalize_radian(path_yaw - obj_yaw);
      if (std::abs(relative_path_yaw) > yaw_diff_limit) {
        continue;
      }

      const double dx = path_pose.position.x - obj_point.x;
      const double dy = path_pose.position.y - obj_point.y;
      const double dx_cp = std::cos(obj_yaw) * dx + std::sin(obj_yaw) * dy;
      const double dy_cp = -std::sin(obj_yaw) * dx + std::cos(obj_yaw) * dy;
      const double neutral_yaw = std::atan2(dy_cp, dx_cp) * 2.0;
      const double delta_yaw = autoware_utils::normalize_radian(path_yaw - obj_yaw - neutral_yaw);
      if (std::abs(delta_yaw) > yaw_diff_limit) {
        continue;
      }

      // objective function score
      constexpr double weight_ratio = 0.01;
      double score = delta_yaw * delta_yaw + weight_ratio * neutral_yaw * neutral_yaw;
      constexpr double acceptable_score = 1e-3;

      if (score < best_score) {
        best_score = score;
        idx = i;
        is_position_found = true;
        if (score < acceptable_score) {
          // if the score is small enough, we can break the loop
          break;
        }
      }
    }
  }

  // update starting segment index
  index += idx;
  index = std::clamp(index, 0ul, pose_path.size() - 1);

  return is_position_found ? opt_index : std::nullopt;
}

}  // namespace autoware::map_based_prediction

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::map_based_prediction::MapBasedPredictionNode)

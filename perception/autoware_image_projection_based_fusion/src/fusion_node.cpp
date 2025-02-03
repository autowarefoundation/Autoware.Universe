// Copyright 2025 TIER IV, Inc.
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

#include <iomanip>
#define EIGEN_MPL2_ONLY

#include "autoware/image_projection_based_fusion/fusion_collector.hpp"
#include "autoware/image_projection_based_fusion/fusion_node.hpp"
#include "autoware/image_projection_based_fusion/fusion_types.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <autoware/image_projection_based_fusion/utils/utils.hpp>

#include <tier4_perception_msgs/msg/detected_object_with_feature.hpp>

#include <boost/optional.hpp>

#include <cmath>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

static double processing_time_ms = 0;

namespace autoware::image_projection_based_fusion
{
using autoware::universe_utils::ScopedTimeTrack;

template <class Msg3D, class Msg2D, class ExportObj>
FusionNode<Msg3D, Msg2D, ExportObj>::FusionNode(
  const std::string & node_name, const rclcpp::NodeOptions & options)
: Node(node_name, options), tf_buffer_(this->get_clock()), tf_listener_(tf_buffer_)
{
  // set rois_number
  rois_number_ = static_cast<std::size_t>(declare_parameter<int32_t>("rois_number"));
  if (rois_number_ < 1) {
    RCLCPP_ERROR(
      this->get_logger(), "minimum rois_number_ is 1. current rois_number_ is %zu", rois_number_);
  }
  if (rois_number_ > 8) {
    RCLCPP_WARN(
      this->get_logger(),
      "Current rois_number_ is %zu. Large rois number may cause performance issue.", rois_number_);
  }

  // Set parameters
  timeout_sec_ = declare_parameter<double>("timeout_sec");

  std::vector<std::string> input_rois_topics;
  std::vector<std::string> input_camera_info_topics;

  input_rois_topics.resize(rois_number_);
  input_camera_info_topics.resize(rois_number_);

  for (std::size_t roi_i = 0; roi_i < rois_number_; ++roi_i) {
    input_rois_topics.at(roi_i) = declare_parameter<std::string>(
      "input/rois" + std::to_string(roi_i),
      "/perception/object_recognition/detection/rois" + std::to_string(roi_i));

    input_camera_info_topics.at(roi_i) = declare_parameter<std::string>(
      "input/camera_info" + std::to_string(roi_i),
      "/sensing/camera/camera" + std::to_string(roi_i) + "/camera_info");
  }

  // subscribe camera info
  camera_info_subs_.resize(rois_number_);
  for (std::size_t roi_i = 0; roi_i < rois_number_; ++roi_i) {
    std::function<void(const sensor_msgs::msg::CameraInfo::ConstSharedPtr msg)> fnc =
      std::bind(&FusionNode::camera_info_callback, this, std::placeholders::_1, roi_i);
    camera_info_subs_.at(roi_i) = this->create_subscription<sensor_msgs::msg::CameraInfo>(
      input_camera_info_topics.at(roi_i), rclcpp::QoS{1}.best_effort(), fnc);
  }

  // subscribe rois
  rois_subs_.resize(rois_number_);
  for (std::size_t roi_i = 0; roi_i < rois_number_; ++roi_i) {
    std::function<void(const typename Msg2D::ConstSharedPtr msg)> roi_callback =
      std::bind(&FusionNode::roi_callback, this, std::placeholders::_1, roi_i);
    rois_subs_.at(roi_i) = this->create_subscription<Msg2D>(
      input_rois_topics.at(roi_i), rclcpp::QoS{1}.best_effort(), roi_callback);
  }

  // subscribe 3d detection
  std::function<void(const typename Msg3D::ConstSharedPtr msg)> sub_callback =
    std::bind(&FusionNode::sub_callback, this, std::placeholders::_1);
  det3d_sub_ =
    this->create_subscription<Msg3D>("input", rclcpp::QoS(1).best_effort(), sub_callback);

  // initialization on each 2d detections
  set_det2d_status(rois_number_);

  // parameters for approximation grid
  approx_grid_cell_w_size_ = declare_parameter<float>("approximation_grid_cell_width");
  approx_grid_cell_h_size_ = declare_parameter<float>("approximation_grid_cell_height");

  // parameters for out_of_scope filter
  filter_scope_min_x_ = declare_parameter<double>("filter_scope_min_x");
  filter_scope_max_x_ = declare_parameter<double>("filter_scope_max_x");
  filter_scope_min_y_ = declare_parameter<double>("filter_scope_min_y");
  filter_scope_max_y_ = declare_parameter<double>("filter_scope_max_y");
  filter_scope_min_z_ = declare_parameter<double>("filter_scope_min_z");
  filter_scope_max_z_ = declare_parameter<double>("filter_scope_max_z");

  rosbag_length_ = declare_parameter<double>("rosbag_length");
  publish_previous_but_late_output_msg_ =
    declare_parameter<bool>("publish_previous_but_late_output_msg");
  matching_strategy_ = declare_parameter<std::string>("matching_strategy.type");

  // debugger
  debug_mode_ = declare_parameter<bool>("debug_mode");
  if (debug_mode_) {
    std::vector<std::string> input_camera_topics;
    input_camera_topics.resize(rois_number_);
    for (std::size_t roi_i = 0; roi_i < rois_number_; ++roi_i) {
      input_camera_topics.at(roi_i) = declare_parameter<std::string>(
        "input/image" + std::to_string(roi_i),
        "/sensing/camera/camera" + std::to_string(roi_i) + "/image_rect_color");
    }
    auto image_buffer_size =
      static_cast<std::size_t>(declare_parameter<int32_t>("image_buffer_size"));
    debugger_ =
      std::make_shared<Debugger>(this, rois_number_, image_buffer_size, input_camera_topics);

    // input topic timing publisher
    debug_internal_pub_ =
      std::make_unique<autoware::universe_utils::DebugPublisher>(this, get_name());
  }

  // time keeper
  bool use_time_keeper = declare_parameter<bool>("publish_processing_time_detail");
  if (use_time_keeper) {
    detailed_processing_time_publisher_ =
      this->create_publisher<autoware::universe_utils::ProcessingTimeDetail>(
        "~/debug/processing_time_detail_ms", 1);
    auto time_keeper = autoware::universe_utils::TimeKeeper(detailed_processing_time_publisher_);
    time_keeper_ = std::make_shared<autoware::universe_utils::TimeKeeper>(time_keeper);
  }

  // initialize debug tool
  {
    stop_watch_ptr_ =
      std::make_unique<autoware::universe_utils::StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ = std::make_unique<autoware::universe_utils::DebugPublisher>(this, get_name());
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");
  }

  // Diagnostic Updater
  diagnostic_updater_.setHardwareID(node_name + "_checker");
  diagnostic_updater_.add(node_name + "fusion_status", this, &FusionNode::check_fusion_status);
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::init_strategy()
{
  if (matching_strategy_ == "naive") {
    fusion_matching_strategy_ = std::make_unique<NaiveMatchingStrategy<Msg3D, Msg2D, ExportObj>>(
      std::dynamic_pointer_cast<FusionNode>(shared_from_this()), rois_number_);
  } else if (matching_strategy_ == "advanced") {
    fusion_matching_strategy_ = std::make_unique<AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>>(
      std::dynamic_pointer_cast<FusionNode>(shared_from_this()), rois_number_);
    // subscribe diagnostics
    sub_diag_ = this->create_subscription<diagnostic_msgs::msg::DiagnosticArray>(
      "/diagnostics", 10, std::bind(&FusionNode::diagnostic_callback, this, std::placeholders::_1));
  } else {
    throw std::runtime_error("Matching strategy must be 'advanced' or 'naive'");
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::set_det2d_status(std::size_t rois_number_)
{
  // camera projection settings
  std::vector<bool> point_project_to_unrectified_image =
    declare_parameter<std::vector<bool>>("point_project_to_unrectified_image");
  if (rois_number_ > point_project_to_unrectified_image.size()) {
    throw std::runtime_error(
      "The number of point_project_to_unrectified_image does not match the number of rois "
      "topics.");
  }
  std::vector<bool> approx_camera_projection =
    declare_parameter<std::vector<bool>>("approximate_camera_projection");
  if (rois_number_ != approx_camera_projection.size()) {
    const std::size_t current_size = approx_camera_projection.size();
    RCLCPP_WARN(
      get_logger(),
      "The number of elements in approximate_camera_projection should be the same as in "
      "rois_number_. "
      "It has %zu elements. But rois_number_ is %zu",
      current_size, rois_number_);
    if (current_size < rois_number_) {
      approx_camera_projection.resize(rois_number_);
      for (std::size_t i = current_size; i < rois_number_; i++) {
        approx_camera_projection.at(i) = true;
      }
    }
  }

  // 2d detection status initialization
  det2d_list_.resize(rois_number_);
  for (std::size_t roi_i = 0; roi_i < rois_number_; ++roi_i) {
    det2d_list_.at(roi_i).id = roi_i;
    det2d_list_.at(roi_i).project_to_unrectified_image =
      point_project_to_unrectified_image.at(roi_i);
    det2d_list_.at(roi_i).approximate_camera_projection = approx_camera_projection.at(roi_i);
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::camera_info_callback(
  const sensor_msgs::msg::CameraInfo::ConstSharedPtr input_camera_info_msg,
  const std::size_t camera_id)
{
  // create the CameraProjection when the camera info arrives for the first time
  // assuming the camera info does not change while the node is running
  auto & det2d = det2d_list_.at(camera_id);
  if (!det2d.camera_projector_ptr && check_camera_info(*input_camera_info_msg)) {
    det2d.camera_projector_ptr = std::make_unique<CameraProjection>(
      *input_camera_info_msg, approx_grid_cell_w_size_, approx_grid_cell_h_size_,
      det2d.project_to_unrectified_image, det2d.approximate_camera_projection);
    det2d.camera_projector_ptr->initialize();
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::preprocess(Msg3D & output_msg __attribute__((unused)))
{
  // do nothing by default
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::export_process(
  typename Msg3D::SharedPtr & output_det3d_msg,
  std::unordered_map<std::size_t, double> id_to_stamp_map,
  std::shared_ptr<FusionCollectorInfoBase> collector_info)
{
  ExportObj output_msg;
  postprocess(*(output_det3d_msg), output_msg);

  current_output_msg_timestamp_ = rclcpp::Time(output_msg.header.stamp).seconds();
  if (
    current_output_msg_timestamp_ < latest_output_msg_timestamp_ &&
    !publish_previous_but_late_output_msg_) {
    // Publish the output msg if the rosbag replays in loop
    if (latest_output_msg_timestamp_ - current_output_msg_timestamp_ > rosbag_length_) {
      publish_output_msg_ = true;  // Force publishing in this case
    } else {
      drop_previous_but_late_output_msg_ = true;  // Otherwise, drop the late output msg
    }
  } else {
    // Publish pointcloud if timestamps are valid or the condition doesn't apply
    publish_output_msg_ = true;
  }

  if (publish_output_msg_) {
    publish(output_msg);
  }

  diagnostic_collector_info_ = std::move(collector_info);
  diagnostic_id_to_stamp_map_ = std::move(id_to_stamp_map);
  diagnostic_updater_.force_update();

  // add processing time for debug
  if (debug_publisher_) {
    const double cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    const double pipeline_latency_ms =
      std::chrono::duration<double, std::milli>(
        std::chrono::nanoseconds(
          (this->get_clock()->now() - output_det3d_msg->header.stamp).nanoseconds()))
        .count();
    debug_publisher_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    debug_publisher_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms",
      processing_time_ms + stop_watch_ptr_->toc("processing_time", true));
    debug_publisher_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
      "debug/pipeline_latency_ms", pipeline_latency_ms);
    processing_time_ms = 0;
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::sub_callback(
  const typename Msg3D::ConstSharedPtr det3d_msg)
{
  if (!fusion_matching_strategy_) {
    init_strategy();
  }

  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);
  stop_watch_ptr_->toc("processing_time", true);

  manage_collector_list();
  // protect fusion collectors list
  std::unique_lock<std::mutex> fusion_collectors_lock(fusion_collectors_mutex_);

  auto det3d_timestamp = rclcpp::Time(det3d_msg->header.stamp).seconds();
  // For each callback, check whether there is a exist collector that matches this cloud
  std::optional<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> fusion_collector =
    std::nullopt;
  auto matching_params = std::make_shared<Det3dMatchingParams>();
  matching_params->det3d_timestamp = det3d_timestamp;

  if (!fusion_collectors_.empty()) {
    fusion_collector =
      fusion_matching_strategy_->match_det3d_to_collector(fusion_collectors_, matching_params);
  }

  bool process_success = false;
  if (fusion_collector.has_value()) {
    auto collector = fusion_collector.value();
    if (collector) {
      fusion_collectors_lock.unlock();
      process_success = fusion_collector.value()->process_msg_3d(det3d_msg);
    }
  }

  if (!process_success) {
    auto new_fusion_collector = std::make_shared<FusionCollector<Msg3D, Msg2D, ExportObj>>(
      std::dynamic_pointer_cast<FusionNode>(shared_from_this()), timeout_sec_, det2d_list_,
      debug_mode_);

    fusion_collectors_.push_back(new_fusion_collector);
    fusion_collectors_lock.unlock();

    fusion_matching_strategy_->set_collector_info(new_fusion_collector, matching_params);
    (void)new_fusion_collector->process_msg_3d(det3d_msg);
  }

  // TODO(vivid): check the logic of clearing debugger.
  if (debugger_) {
    debugger_->clear();
  }

  if (matching_strategy_ == "advanced") {
    // remove outdated messages in the concatenated map
    manage_concatenated_status_map(det3d_timestamp);
  }

  // // add timestamp interval for debug
  // if (debug_internal_pub_) {
  //   double timestamp_interval_ms = (matched_stamp - timestamp_nsec) / 1e6;
  //   debug_internal_pub_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
  //     "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_ms", timestamp_interval_ms);
  //   debug_internal_pub_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
  //     "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_offset_ms",
  //     timestamp_interval_ms - det2d.input_offset_ms);
  // }

  // processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::roi_callback(
  const typename Msg2D::ConstSharedPtr det2d_msg, const std::size_t roi_i)
{
  if (!fusion_matching_strategy_) {
    init_strategy();
  }

  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  stop_watch_ptr_->toc("processing_time", true);

  manage_collector_list();
  // protect fusion collectors list
  std::unique_lock<std::mutex> fusion_collectors_lock(fusion_collectors_mutex_);

  auto rois_timestamp = rclcpp::Time(det2d_msg->header.stamp).seconds();
  auto matching_params = std::make_shared<RoisMatchingParams>();
  matching_params->rois_id = roi_i;
  matching_params->rois_timestamp = rois_timestamp;

  // For each callback, check whether there is a exist collector that matches this cloud
  std::optional<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> fusion_collector =
    std::nullopt;

  if (!fusion_collectors_.empty()) {
    fusion_collector =
      fusion_matching_strategy_->match_rois_to_collector(fusion_collectors_, matching_params);
  }

  bool process_success = false;
  if (fusion_collector.has_value()) {
    auto collector = fusion_collector.value();
    if (collector) {
      fusion_collectors_lock.unlock();
      process_success = fusion_collector.value()->process_rois(roi_i, det2d_msg);
    }
  }

  if (!process_success) {
    auto new_fusion_collector = std::make_shared<FusionCollector<Msg3D, Msg2D, ExportObj>>(
      std::dynamic_pointer_cast<FusionNode>(shared_from_this()), timeout_sec_, det2d_list_,
      debug_mode_);

    fusion_collectors_.push_back(new_fusion_collector);
    fusion_collectors_lock.unlock();

    fusion_matching_strategy_->set_collector_info(new_fusion_collector, matching_params);
    (void)new_fusion_collector->process_rois(roi_i, det2d_msg);
  }

  if (debugger_) {
    debugger_->clear();
  }

  // if (debug_internal_pub_) {
  //       double timestamp_interval_ms = (timestamp_nsec - cached_det3d_msg_timestamp_) / 1e6;
  //       debug_internal_pub_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
  //         "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_ms", timestamp_interval_ms);
  //       debug_internal_pub_->publish<autoware_internal_debug_msgs::msg::Float64Stamped>(
  //         "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_offset_ms",
  //         timestamp_interval_ms - det2d.input_offset_ms);
  // }

  // processing_time_ms = processing_time_ms + stop_watch_ptr_->toc("processing_time", true);
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::diagnostic_callback(
  const diagnostic_msgs::msg::DiagnosticArray::SharedPtr diagnostic_msg)
{
  for (const auto & status : diagnostic_msg->status) {
    // Filter for the concatenate_and_time_sync_node diagnostic message
    if (status.name == "concatenate_data: /sensing/lidar/concatenate_data_status") {
      // Temporary map to hold key-value pairs for this status
      std::unordered_map<std::string, std::string> key_value_map;
      std::optional<double> concatenate_timestamp_opt;

      for (const auto & value : status.values) {
        key_value_map[value.key] = value.value;

        // If the key is the concatenated cloud timestamp, try to parse it
        if (value.key == "concatenated_cloud_timestamp") {
          try {
            // concatenate_timestamp_opt = std::round(std::stod(value.value) * 1000.0) / 1000.0;
            concatenate_timestamp_opt = std::stod(value.value);
          } catch (const std::exception & e) {
            RCLCPP_ERROR(get_logger(), "Error parsing concatenated cloud timestamp: %s", e.what());
          }
        }
      }

      // Ensure a valid timestamp was parsed before storing
      if (concatenate_timestamp_opt.has_value()) {
        concatenated_status_map_[concatenate_timestamp_opt.value()] = key_value_map;
        RCLCPP_INFO(
          get_logger(), "Stored concatenation status for timestamp: %.9f",
          concatenate_timestamp_opt.value());
      } else {
        RCLCPP_WARN(
          get_logger(), "Missing or invalid concatenated cloud timestamp, status not stored.");
      }
    }
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::manage_concatenated_status_map(
  const double & current_timestamp)
{
  // TOOD(vivid): there might be a better way to do this
  // Clean up old entries from concatenated_status_map_
  double threshold = 1.0;  // second
  auto it = concatenated_status_map_.begin();
  while (it != concatenated_status_map_.end()) {
    if (current_timestamp - it->first > threshold) {
      RCLCPP_DEBUG(
        get_logger(), "Removing old concatenation status for timestamp: %.9f", it->first);
      it = concatenated_status_map_.erase(it);
    } else {
      ++it;
    }
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::postprocess(
  const Msg3D & processing_msg __attribute__((unused)),
  ExportObj & output_msg __attribute__((unused)))
{
  // do nothing by default
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::publish(const ExportObj & output_msg)
{
  if (pub_ptr_->get_subscription_count() < 1) {
    return;
  }
  pub_ptr_->publish(output_msg);
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::manage_collector_list()
{
  std::lock_guard<std::mutex> collectors_lock(fusion_collectors_mutex_);

  for (auto it = fusion_collectors_.begin(); it != fusion_collectors_.end();) {
    if ((*it)->fusion_finished()) {
      it = fusion_collectors_.erase(it);  // Erase and move the iterator to the next element
    } else {
      ++it;  // Move to the next element
    }
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
std::optional<std::unordered_map<std::string, std::string>>
FusionNode<Msg3D, Msg2D, ExportObj>::find_concatenation_status(double timestamp)
{
  auto it = concatenated_status_map_.find(timestamp);
  if (it != concatenated_status_map_.end()) {
    return it->second;
  }
  return std::nullopt;
}

template <class Msg3D, class Msg2D, class ExportObj>
std::string FusionNode<Msg3D, Msg2D, ExportObj>::format_timestamp(double timestamp)
{
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(9) << timestamp;
  return oss.str();
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::show_diagnostic_message(
  std::unordered_map<std::size_t, double> id_to_stamp_map,
  std::shared_ptr<FusionCollectorInfoBase> collector_info)
{
  det3d_fused_ = false;
  diagnostic_collector_info_ = std::move(collector_info);
  diagnostic_id_to_stamp_map_ = std::move(id_to_stamp_map);
  diagnostic_updater_.force_update();
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::check_fusion_status(
  diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  if (publish_output_msg_ || drop_previous_but_late_output_msg_ || !det3d_fused_) {
    stat.add("det3d/is_fused", det3d_fused_);
    if (det3d_fused_) {
      stat.add("fused_timestamp", format_timestamp(current_output_msg_timestamp_));
    }

    if (
      auto naive_info = std::dynamic_pointer_cast<NaiveCollectorInfo>(diagnostic_collector_info_)) {
      stat.add("first_input_arrival_timestamp", format_timestamp(naive_info->timestamp));
    } else if (
      auto advanced_info =
        std::dynamic_pointer_cast<AdvancedCollectorInfo>(diagnostic_collector_info_)) {
      stat.add(
        "reference_timestamp_min",
        format_timestamp(advanced_info->timestamp - advanced_info->noise_window));
      stat.add(
        "reference_timestamp_max",
        format_timestamp(advanced_info->timestamp + advanced_info->noise_window));
    }

    bool rois_miss = false;

    bool fusion_success = true;
    for (std::size_t id = 0; id < rois_number_; ++id) {
      bool input_rois_fused = true;
      if (diagnostic_id_to_stamp_map_.find(id) != diagnostic_id_to_stamp_map_.end()) {
        stat.add(
          "rois" + std::to_string(id) + "/timestamp",
          format_timestamp(diagnostic_id_to_stamp_map_[id]));
      } else {
        rois_miss = true;
        fusion_success = false;
        input_rois_fused = false;
      }
      stat.add("rois" + std::to_string(id) + "/is_fused", input_rois_fused);
    }

    stat.add("fusion_success", fusion_success);

    int8_t level = diagnostic_msgs::msg::DiagnosticStatus::OK;
    std::string message = "Fused output is published and include all rois and det3d msg";

    if (drop_previous_but_late_output_msg_) {
      if (rois_miss) {
        level = diagnostic_msgs::msg::DiagnosticStatus::ERROR;
        message =
          "Fused output msg misses some rois and is not published because it arrived "
          "too late";
      } else {
        level = diagnostic_msgs::msg::DiagnosticStatus::ERROR;
        message = "Fused output msg is not published as it is too late";
      }
    } else {
      if (rois_miss) {
        level = diagnostic_msgs::msg::DiagnosticStatus::ERROR;
        message = "Fused output msg is published but misses some rois";
      }
    }

    stat.summary(level, message);

    publish_output_msg_ = false;
    drop_previous_but_late_output_msg_ = false;
    det3d_fused_ = true;
  } else {
    const int8_t level = diagnostic_msgs::msg::DiagnosticStatus::OK;
    const std::string message = "Fusion node launch successfully, but waiting for input pointcloud";
    stat.summary(level, message);
  }
}

// Explicit instantiation for the supported types

// pointpainting fusion
template class FusionNode<PointCloudMsgType, RoiMsgType, DetectedObjects>;

// roi cluster fusion
template class FusionNode<ClusterMsgType, RoiMsgType, ClusterMsgType>;

// roi detected-object fusion
template class FusionNode<DetectedObjects, RoiMsgType, DetectedObjects>;

// roi pointcloud fusion
template class FusionNode<PointCloudMsgType, RoiMsgType, ClusterMsgType>;

// segment pointcloud fusion
template class FusionNode<PointCloudMsgType, Image, PointCloudMsgType>;

}  // namespace autoware::image_projection_based_fusion

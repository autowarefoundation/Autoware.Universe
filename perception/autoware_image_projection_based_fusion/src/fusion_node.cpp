// Copyright 2022 TIER IV, Inc.
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

#define EIGEN_MPL2_ONLY

#include "autoware/image_projection_based_fusion/fusion_node.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <autoware/image_projection_based_fusion/utils/utils.hpp>

#include <tier4_perception_msgs/msg/detected_object_with_feature.hpp>

#include <boost/optional.hpp>

#include <cmath>
#include <list>
#include <memory>
#include <string>
#include <vector>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#endif

// static int publish_counter = 0;
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
  const std::size_t rois_number =
    static_cast<std::size_t>(declare_parameter<int32_t>("rois_number"));
  if (rois_number < 1) {
    RCLCPP_ERROR(
      this->get_logger(), "minimum rois_number is 1. current rois_number is %zu", rois_number);
  }
  if (rois_number > 8) {
    RCLCPP_WARN(
      this->get_logger(),
      "Current rois_number is %zu. Large rois number may cause performance issue.", rois_number);
  }

  // Set parameters
  match_threshold_ms_ = declare_parameter<double>("match_threshold_ms");
  timeout_ms_ = declare_parameter<double>("timeout_ms");

  std::vector<std::string> input_rois_topics;
  std::vector<std::string> input_camera_info_topics;

  input_rois_topics.resize(rois_number);
  input_camera_info_topics.resize(rois_number);

  for (std::size_t roi_i = 0; roi_i < rois_number; ++roi_i) {
    input_rois_topics.at(roi_i) = declare_parameter<std::string>(
      "input/rois" + std::to_string(roi_i),
      "/perception/object_recognition/detection/rois" + std::to_string(roi_i));

    input_camera_info_topics.at(roi_i) = declare_parameter<std::string>(
      "input/camera_info" + std::to_string(roi_i),
      "/sensing/camera/camera" + std::to_string(roi_i) + "/camera_info");
  }

  // subscribe camera info
  camera_info_subs_.resize(rois_number);
  for (std::size_t roi_i = 0; roi_i < rois_number; ++roi_i) {
    std::function<void(const sensor_msgs::msg::CameraInfo::ConstSharedPtr msg)> fnc =
      std::bind(&FusionNode::cameraInfoCallback, this, std::placeholders::_1, roi_i);
    camera_info_subs_.at(roi_i) = this->create_subscription<sensor_msgs::msg::CameraInfo>(
      input_camera_info_topics.at(roi_i), rclcpp::QoS{1}.best_effort(), fnc);
  }

  // subscribe rois
  rois_subs_.resize(rois_number);
  for (std::size_t roi_i = 0; roi_i < rois_number; ++roi_i) {
    std::function<void(const typename Msg2D::ConstSharedPtr msg)> roi_callback =
      std::bind(&FusionNode::roiCallback, this, std::placeholders::_1, roi_i);
    rois_subs_.at(roi_i) = this->create_subscription<Msg2D>(
      input_rois_topics.at(roi_i), rclcpp::QoS{1}.best_effort(), roi_callback);
  }

  // subscribe 3d detection
  std::function<void(const typename Msg3D::ConstSharedPtr msg)> sub_callback =
    std::bind(&FusionNode::subCallback, this, std::placeholders::_1);
  sub_ = this->create_subscription<Msg3D>("input", rclcpp::QoS(1).best_effort(), sub_callback);

  // Set timer
  const auto period_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double, std::milli>(timeout_ms_));
  timer_ = rclcpp::create_timer(
    this, get_clock(), period_ns, std::bind(&FusionNode::timer_callback, this));

  // initialization on each 2d detections
  setDet2DStatus(rois_number);

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

  // debugger
  if (declare_parameter("debug_mode", false)) {
    std::vector<std::string> input_camera_topics;
    input_camera_topics.resize(rois_number);
    for (std::size_t roi_i = 0; roi_i < rois_number; ++roi_i) {
      input_camera_topics.at(roi_i) = declare_parameter<std::string>(
        "input/image" + std::to_string(roi_i),
        "/sensing/camera/camera" + std::to_string(roi_i) + "/image_rect_color");
    }
    std::size_t image_buffer_size =
      static_cast<std::size_t>(declare_parameter<int32_t>("image_buffer_size"));
    debugger_ =
      std::make_shared<Debugger>(this, rois_number, image_buffer_size, input_camera_topics);
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
    using autoware::universe_utils::DebugPublisher;
    using autoware::universe_utils::StopWatch;
    stop_watch_ptr_ = std::make_unique<StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ = std::make_unique<DebugPublisher>(this, get_name());
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::setDet2DStatus(std::size_t rois_number)
{
  // camera offset settings
  std::vector<double> input_offset_ms = declare_parameter<std::vector<double>>("input_offset_ms");
  if (!input_offset_ms.empty() && rois_number > input_offset_ms.size()) {
    throw std::runtime_error("The number of offsets does not match the number of topics.");
  }

  // camera projection settings
  std::vector<bool> point_project_to_unrectified_image =
    declare_parameter<std::vector<bool>>("point_project_to_unrectified_image");
  if (rois_number > point_project_to_unrectified_image.size()) {
    throw std::runtime_error(
      "The number of point_project_to_unrectified_image does not match the number of rois "
      "topics.");
  }
  std::vector<bool> approx_camera_projection =
    declare_parameter<std::vector<bool>>("approximate_camera_projection");
  if (rois_number != approx_camera_projection.size()) {
    const std::size_t current_size = approx_camera_projection.size();
    RCLCPP_WARN(
      get_logger(),
      "The number of elements in approximate_camera_projection should be the same as in "
      "rois_number. "
      "It has %zu elements.",
      current_size);
    if (current_size < rois_number) {
      approx_camera_projection.resize(rois_number);
      for (std::size_t i = current_size; i < rois_number; i++) {
        approx_camera_projection.at(i) = true;
      }
    }
  }

  // 2d detection status initialization
  det2d_list_.resize(rois_number);
  for (std::size_t roi_i = 0; roi_i < rois_number; ++roi_i) {
    det2d_list_.at(roi_i).id = roi_i;
    det2d_list_.at(roi_i).project_to_unrectified_image =
      point_project_to_unrectified_image.at(roi_i);
    det2d_list_.at(roi_i).approximate_camera_projection = approx_camera_projection.at(roi_i);
    det2d_list_.at(roi_i).input_offset_ms = input_offset_ms.at(roi_i);
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::cameraInfoCallback(
  const sensor_msgs::msg::CameraInfo::ConstSharedPtr input_camera_info_msg,
  const std::size_t camera_id)
{
  // create the CameraProjection when the camera info arrives for the first time
  // assuming the camera info does not change while the node is running
  auto & det2d = det2d_list_.at(camera_id);
  if (!det2d.camera_projector_ptr && checkCameraInfo(*input_camera_info_msg)) {
    det2d.camera_projector_ptr = std::make_unique<CameraProjection>(
      *input_camera_info_msg, approx_grid_cell_w_size_, approx_grid_cell_h_size_,
      det2d.project_to_unrectified_image, det2d.approximate_camera_projection);
    det2d.camera_projector_ptr->initialize();
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::preprocess(Msg3D & ouput_msg __attribute__((unused)))
{
  // do nothing by default
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::exportProcess()
{
  timer_->cancel();

  ExportObj output_msg;
  postprocess(*(cached_det3d_msg_ptr_), output_msg);
  publish(output_msg);

  // add processing time for debug
  if (debug_publisher_) {
    const double cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    const double pipeline_latency_ms =
      std::chrono::duration<double, std::milli>(
        std::chrono::nanoseconds(
          (this->get_clock()->now() - cached_det3d_msg_ptr_->header.stamp).nanoseconds()))
        .count();
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms",
      processing_time_ms + stop_watch_ptr_->toc("processing_time", true));
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/pipeline_latency_ms", pipeline_latency_ms);
    processing_time_ms = 0;
  }
  cached_det3d_msg_ptr_ = nullptr;
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::subCallback(
  const typename Msg3D::ConstSharedPtr det3d_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  if (cached_det3d_msg_ptr_ != nullptr) {
    // PROCESS: if the main message is remained (and roi is not collected all) publish the main
    // message may processed partially with arrived 2d rois
    stop_watch_ptr_->toc("processing_time", true);
    exportProcess();

    // reset flags
    for (auto & det2d : det2d_list_) {
      det2d.is_fused = false;
    }
  }

  // TIMING: reset timer to the timeout time
  auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double, std::milli>(timeout_ms_));
  try {
    setPeriod(period.count());
  } catch (rclcpp::exceptions::RCLError & ex) {
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "%s", ex.what());
  }
  timer_->reset();

  stop_watch_ptr_->toc("processing_time", true);

  // PROCESS: preprocess the main message
  typename Msg3D::SharedPtr output_msg = std::make_shared<Msg3D>(*det3d_msg);
  preprocess(*output_msg);

  // PROCESS: fuse the main message with the cached roi messages
  // (please ask maintainers before parallelize this loop because debugger is not thread safe)
  int64_t timestamp_nsec =
    (*output_msg).header.stamp.sec * static_cast<int64_t>(1e9) + (*output_msg).header.stamp.nanosec;
  // for loop for each roi
  for (auto & det2d : det2d_list_) {
    const auto roi_i = det2d.id;

    // check camera info
    if (det2d.camera_projector_ptr == nullptr) {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(), *this->get_clock(), 5000, "no camera info. id is %zu", roi_i);
      continue;
    }
    auto & det2d_msgs = det2d.cached_det2d_msgs;

    // check if the roi is collected
    if (det2d_msgs.size() == 0) continue;

    // MATCH: get the closest roi message, and remove outdated messages
    int64_t min_interval = 1e9;
    int64_t matched_stamp = -1;
    std::list<int64_t> outdate_stamps;
    for (const auto & [roi_stamp, value] : det2d_msgs) {
      int64_t new_stamp = timestamp_nsec + det2d.input_offset_ms * static_cast<int64_t>(1e6);
      int64_t interval = abs(static_cast<int64_t>(roi_stamp) - new_stamp);

      if (interval <= min_interval && interval <= match_threshold_ms_ * static_cast<int64_t>(1e6)) {
        min_interval = interval;
        matched_stamp = roi_stamp;
      } else if (
        static_cast<int64_t>(roi_stamp) < new_stamp &&
        interval > match_threshold_ms_ * static_cast<int64_t>(1e6)) {
        outdate_stamps.push_back(static_cast<int64_t>(roi_stamp));
      }
    }
    for (auto stamp : outdate_stamps) {
      det2d_msgs.erase(stamp);
    }

    // PROCESS: if matched, fuse the main message with the roi message
    if (matched_stamp != -1) {
      if (debugger_) {
        debugger_->clear();
      }

      fuseOnSingleImage(*det3d_msg, det2d, *(det2d_msgs[matched_stamp]), *output_msg);
      det2d_msgs.erase(matched_stamp);
      det2d.is_fused = true;

      // add timestamp interval for debug
      if (debug_publisher_) {
        double timestamp_interval_ms = (matched_stamp - timestamp_nsec) / 1e6;
        debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
          "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_ms", timestamp_interval_ms);
        debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
          "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_offset_ms",
          timestamp_interval_ms - det2d.input_offset_ms);
      }
    }
  }

  // PROCESS: check if the fused message is ready to publish
  cached_det3d_msg_timestamp_ = timestamp_nsec;
  cached_det3d_msg_ptr_ = output_msg;
  if (checkAllDet2dFused()) {
    // if all camera fused, postprocess and publish the main message
    exportProcess();

    // reset flags
    for (auto & det2d : det2d_list_) {
      det2d.is_fused = false;
    }
  } else {
    // if all of rois are not collected, publish the old Msg(if exists) and cache the
    // current Msg
    processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::roiCallback(
  const typename Msg2D::ConstSharedPtr det2d_msg, const std::size_t roi_i)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  stop_watch_ptr_->toc("processing_time", true);

  auto & det2d = det2d_list_.at(roi_i);

  int64_t timestamp_nsec =
    (*det2d_msg).header.stamp.sec * static_cast<int64_t>(1e9) + (*det2d_msg).header.stamp.nanosec;
  // if cached Msg exist, try to match
  if (cached_det3d_msg_ptr_ != nullptr) {
    int64_t new_stamp =
      cached_det3d_msg_timestamp_ + det2d.input_offset_ms * static_cast<int64_t>(1e6);
    int64_t interval = abs(timestamp_nsec - new_stamp);

    // PROCESS: if matched, fuse the main message with the roi message
    if (interval < match_threshold_ms_ * static_cast<int64_t>(1e6) && det2d.is_fused == false) {
      // check camera info
      if (det2d.camera_projector_ptr == nullptr) {
        RCLCPP_WARN_THROTTLE(
          this->get_logger(), *this->get_clock(), 5000, "no camera info. id is %zu", roi_i);
        det2d.cached_det2d_msgs[timestamp_nsec] = det2d_msg;
        return;
      }

      if (debugger_) {
        debugger_->clear();
      }
      // PROCESS: fuse the main message with the roi message
      fuseOnSingleImage(*(cached_det3d_msg_ptr_), det2d, *det2d_msg, *(cached_det3d_msg_ptr_));
      det2d.is_fused = true;

      if (debug_publisher_) {
        double timestamp_interval_ms = (timestamp_nsec - cached_det3d_msg_timestamp_) / 1e6;
        debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
          "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_ms", timestamp_interval_ms);
        debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
          "debug/roi" + std::to_string(roi_i) + "/timestamp_interval_offset_ms",
          timestamp_interval_ms - det2d.input_offset_ms);
      }

      // PROCESS: if all camera fused, postprocess and publish the main message
      if (checkAllDet2dFused()) {
        exportProcess();
        // reset flags
        for (auto & status : det2d_list_) {
          status.is_fused = false;
        }
      }
      processing_time_ms = processing_time_ms + stop_watch_ptr_->toc("processing_time", true);
      return;
    }
  }
  // store roi msg if not matched
  det2d.cached_det2d_msgs[timestamp_nsec] = det2d_msg;
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::timer_callback()
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  using std::chrono_literals::operator""ms;
  timer_->cancel();

  // PROCESS: if timeout, postprocess cached msg
  if (cached_det3d_msg_ptr_ != nullptr) {
    stop_watch_ptr_->toc("processing_time", true);
    exportProcess();
  }

  // reset flags whether the message is fused or not
  for (auto & det2d : det2d_list_) {
    det2d.is_fused = false;
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionNode<Msg3D, Msg2D, ExportObj>::setPeriod(const int64_t new_period)
{
  if (!timer_) {
    return;
  }
  int64_t old_period = 0;
  rcl_ret_t ret = rcl_timer_get_period(timer_->get_timer_handle().get(), &old_period);
  if (ret != RCL_RET_OK) {
    rclcpp::exceptions::throw_from_rcl_error(ret, "Couldn't get old period");
  }
  ret = rcl_timer_exchange_period(timer_->get_timer_handle().get(), new_period, &old_period);
  if (ret != RCL_RET_OK) {
    rclcpp::exceptions::throw_from_rcl_error(ret, "Couldn't exchange_period");
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

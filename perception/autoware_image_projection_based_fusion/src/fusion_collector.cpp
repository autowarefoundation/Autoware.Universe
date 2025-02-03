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

#include "autoware/image_projection_based_fusion/fusion_collector.hpp"

#include "autoware/image_projection_based_fusion/fusion_node.hpp"
#include "autoware/image_projection_based_fusion/fusion_types.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware::image_projection_based_fusion
{

template <class Msg3D, class Msg2D, class ExportObj>
FusionCollector<Msg3D, Msg2D, ExportObj>::FusionCollector(
  std::shared_ptr<FusionNode<Msg3D, Msg2D, ExportObj>> && ros2_parent_node, double timeout_sec,
  const std::vector<Det2dStatus<Msg2D>> & det2d_list, bool debug_mode)
: ros2_parent_node_(std::move(ros2_parent_node)),
  timeout_sec_(timeout_sec),
  det2d_list_(det2d_list),
  debug_mode_(debug_mode)
{
  const auto period_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(timeout_sec_));

  timer_ =
    rclcpp::create_timer(ros2_parent_node_, ros2_parent_node_->get_clock(), period_ns, [this]() {
      std::lock_guard<std::mutex> fusion_lock(fusion_mutex_);
      if (fusion_finished_) return;
      fusion_callback();
    });
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionCollector<Msg3D, Msg2D, ExportObj>::set_info(
  std::shared_ptr<FusionCollectorInfoBase> fusion_collector_info)
{
  fusion_collector_info_ = std::move(fusion_collector_info);
}

template <class Msg3D, class Msg2D, class ExportObj>
std::shared_ptr<FusionCollectorInfoBase> FusionCollector<Msg3D, Msg2D, ExportObj>::get_info() const
{
  return fusion_collector_info_;
}

template <class Msg3D, class Msg2D, class ExportObj>
bool FusionCollector<Msg3D, Msg2D, ExportObj>::process_msg_3d(
  const typename Msg3D::ConstSharedPtr msg_3d)
{
  std::lock_guard<std::mutex> fusion_lock(fusion_mutex_);
  if (fusion_finished_) return false;

  if (det3d_msg_ != nullptr) {
    RCLCPP_WARN_STREAM_THROTTLE(
      ros2_parent_node_->get_logger(), *ros2_parent_node_->get_clock(),
      std::chrono::milliseconds(10000).count(),
      "Pointcloud already exists in the collector. Check the timestamp of the pointcloud.");
  }

  det3d_msg_ = msg_3d;
  if (ready_to_fuse()) {
    fusion_callback();
  }

  return true;
}

template <class Msg3D, class Msg2D, class ExportObj>
bool FusionCollector<Msg3D, Msg2D, ExportObj>::process_rois(
  const std::size_t & roi_id, const typename Msg2D::ConstSharedPtr det2d_msg)
{
  std::lock_guard<std::mutex> fusion_lock(fusion_mutex_);
  if (fusion_finished_) return false;

  if (id_to_roi_map_.find(roi_id) != id_to_roi_map_.end()) {
    RCLCPP_WARN_STREAM_THROTTLE(
      ros2_parent_node_->get_logger(), *ros2_parent_node_->get_clock(),
      std::chrono::milliseconds(10000).count(),
      "ROIS '" << roi_id << "' already exists in the collector. Check the timestamp of the rois.");
  }
  id_to_roi_map_[roi_id] = det2d_msg;
  if (ready_to_fuse()) {
    fusion_callback();
  }

  return true;
}

template <class Msg3D, class Msg2D, class ExportObj>
bool FusionCollector<Msg3D, Msg2D, ExportObj>::ready_to_fuse()
{
  return id_to_roi_map_.size() == rois_number_ && det3d_msg_ != nullptr;
}

template <class Msg3D, class Msg2D, class ExportObj>
bool FusionCollector<Msg3D, Msg2D, ExportObj>::fusion_finished() const
{
  return fusion_finished_;
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionCollector<Msg3D, Msg2D, ExportObj>::fusion_callback()
{
  if (debug_mode_) {
    show_debug_message();
  }

  // All pointcloud and rois are received or the timer has timed out, cancel the timer and fuse
  // them.
  timer_->cancel();

  std::unordered_map<std::size_t, double> id_to_stamp_map;
  for (const auto & [roi_id, roi_msg] : id_to_roi_map_) {
    id_to_stamp_map[roi_id] = rclcpp::Time(roi_msg->header.stamp).seconds();
  }

  if (!det3d_msg_) {
    RCLCPP_WARN(
      ros2_parent_node_->get_logger(),
      "The Det3d message is not in the fusion collector, so the fusion process will be skipped.");
    fusion_finished_ = true;
    // TODO(vivid): call another functino to show the message on diagnostic.
    ros2_parent_node_->show_diagnostic_message(id_to_stamp_map, fusion_collector_info_);
    return;
  }

  typename Msg3D::SharedPtr output_det3d_msg = std::make_shared<Msg3D>(*det3d_msg_);
  ros2_parent_node_->preprocess(*output_det3d_msg);

  for (const auto & [roi_id, roi_msg] : id_to_roi_map_) {
    if (det2d_list_[roi_id].camera_projector_ptr == nullptr) {
      RCLCPP_WARN_THROTTLE(
        ros2_parent_node_->get_logger(), *ros2_parent_node_->get_clock(), 5000,
        "no camera info. id is %zu", roi_id);
      continue;
    }
    ros2_parent_node_->fuse_on_single_image(
      *det3d_msg_, det2d_list_[roi_id], *roi_msg, *output_det3d_msg);
  }

  ros2_parent_node_->export_process(output_det3d_msg, id_to_stamp_map, fusion_collector_info_);
  fusion_finished_ = true;
}

template <class Msg3D, class Msg2D, class ExportObj>
bool FusionCollector<Msg3D, Msg2D, ExportObj>::rois_exists(const std::size_t & rois_id)
{
  return id_to_roi_map_.find(rois_id) != id_to_roi_map_.end();
}

template <class Msg3D, class Msg2D, class ExportObj>
bool FusionCollector<Msg3D, Msg2D, ExportObj>::det3d_exists()
{
  return det3d_msg_ != nullptr;
}

template <class Msg3D, class Msg2D, class ExportObj>
void FusionCollector<Msg3D, Msg2D, ExportObj>::show_debug_message()
{
  auto time_until_trigger = timer_->time_until_trigger();
  std::stringstream log_stream;
  log_stream << std::fixed << std::setprecision(6);
  log_stream << "Collector's fusion callback time: "
             << ros2_parent_node_->get_clock()->now().seconds() << " seconds\n";

  if (
    auto advanced_info = std::dynamic_pointer_cast<AdvancedCollectorInfo>(fusion_collector_info_)) {
    log_stream << "Advanced strategy:\n Fusion collector's reference time min: "
               << advanced_info->timestamp - advanced_info->noise_window
               << " to max: " << advanced_info->timestamp + advanced_info->noise_window
               << " seconds\n";
  } else if (
    auto naive_info = std::dynamic_pointer_cast<NaiveCollectorInfo>(fusion_collector_info_)) {
    log_stream << "Naive strategy:\n Fusino collector's timestamp: " << naive_info->timestamp
               << " seconds\n";
  }

  log_stream << "Time until trigger: " << (time_until_trigger.count() / 1e9) << " seconds\n";
  if (det3d_msg_) {
    log_stream << "Det3d msg: [" << rclcpp::Time(det3d_msg_->header.stamp).seconds() << "]\n";
  } else {
    log_stream << "Det3d msg: [Is empty]\n";
  }
  log_stream << "ROIs: [";
  std::string separator = "";
  for (const auto & [id, rois] : id_to_roi_map_) {
    log_stream << separator;
    log_stream << "[rois " << id << ", " << rclcpp::Time(rois->header.stamp).seconds() << "]";
    separator = ", ";
  }

  log_stream << "]\n";

  RCLCPP_INFO(ros2_parent_node_->get_logger(), "%s", log_stream.str().c_str());
}

// Explicit instantiation for the supported types

// pointpainting fusion
template class FusionCollector<PointCloudMsgType, RoiMsgType, DetectedObjects>;

// roi cluster fusion
template class FusionCollector<ClusterMsgType, RoiMsgType, ClusterMsgType>;

// roi detected-object fusion
template class FusionCollector<DetectedObjects, RoiMsgType, DetectedObjects>;

// roi pointcloud fusion
template class FusionCollector<PointCloudMsgType, RoiMsgType, ClusterMsgType>;

// segment pointcloud fusion
template class FusionCollector<PointCloudMsgType, Image, PointCloudMsgType>;

}  // namespace autoware::image_projection_based_fusion

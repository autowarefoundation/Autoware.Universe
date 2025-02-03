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

#include "autoware/image_projection_based_fusion/fusion_matching_strategy.hpp"

#include "autoware/image_projection_based_fusion/fusion_collector.hpp"
#include "autoware/image_projection_based_fusion/fusion_node.hpp"
#include "autoware/image_projection_based_fusion/fusion_types.hpp"

#include <rclcpp/rclcpp.hpp>

#include <cmath>
#include <cstddef>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::image_projection_based_fusion
{

template <class Msg3D, class Msg2D, class ExportObj>
NaiveMatchingStrategy<Msg3D, Msg2D, ExportObj>::NaiveMatchingStrategy(
  std::shared_ptr<FusionNode<Msg3D, Msg2D, ExportObj>> && ros2_parent_node, std::size_t rois_number)
: ros2_parent_node_(std::move(ros2_parent_node))
{
  auto rois_timestamp_offsets =
    ros2_parent_node_->template declare_parameter<std::vector<double>>("rois_timestamp_offsets");
  threshold_ = ros2_parent_node_->template declare_parameter<double>("matching_strategy.threshold");

  if (rois_timestamp_offsets.size() != rois_number) {
    throw std::runtime_error("The number of rois does not match the number of timestamp offsets.");
  }

  for (size_t i = 0; i < rois_number; i++) {
    id_to_offset_map_[i] = rois_timestamp_offsets[i];
  }

  RCLCPP_INFO(ros2_parent_node_->get_logger(), "Utilize naive matching strategy for fusion nodes.");
}

template <class Msg3D, class Msg2D, class ExportObj>
std::optional<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>>
NaiveMatchingStrategy<Msg3D, Msg2D, ExportObj>::match_rois_to_collector(
  const std::list<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> & fusion_collectors,
  const std::shared_ptr<RoisMatchingParams> & params) const
{
  std::optional<double> smallest_time_difference = std::nullopt;
  std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>> closest_collector = nullptr;

  for (const auto & fusion_collector : fusion_collectors) {
    if (!fusion_collector->rois_exists(params->rois_id)) {
      auto info = fusion_collector->get_info();
      if (auto naive_info = std::dynamic_pointer_cast<NaiveCollectorInfo>(info)) {
        double time_difference = std::abs(params->rois_timestamp - naive_info->timestamp);
        if (
          !smallest_time_difference.has_value() ||
          (time_difference < smallest_time_difference.value() &&
           time_difference < naive_info->threshold)) {
          smallest_time_difference = time_difference;
          closest_collector = fusion_collector;
        }
      }
    }
  }

  if (closest_collector != nullptr) {
    return closest_collector;
  }
  return std::nullopt;
}

template <class Msg3D, class Msg2D, class ExportObj>
std::optional<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>>
NaiveMatchingStrategy<Msg3D, Msg2D, ExportObj>::match_det3d_to_collector(
  const std::list<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> & fusion_collectors,
  const std::shared_ptr<Det3dMatchingParams> & params)
{
  std::optional<double> smallest_time_difference = std::nullopt;
  std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>> closest_collector = nullptr;

  for (const auto & fusion_collector : fusion_collectors) {
    if (!fusion_collector->det3d_exists()) {
      auto info = fusion_collector->get_info();
      if (auto naive_info = std::dynamic_pointer_cast<NaiveCollectorInfo>(info)) {
        double time_difference = std::abs(params->det3d_timestamp - naive_info->timestamp);
        if (
          !smallest_time_difference.has_value() ||
          (time_difference < smallest_time_difference.value() &&
           time_difference < naive_info->threshold)) {
          smallest_time_difference = time_difference;
          closest_collector = fusion_collector;
        }
      }
    }
  }

  if (closest_collector != nullptr) {
    return closest_collector;
  }
  return std::nullopt;
}

template <class Msg3D, class Msg2D, class ExportObj>
void NaiveMatchingStrategy<Msg3D, Msg2D, ExportObj>::set_collector_info(
  std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>> & collector,
  const std::shared_ptr<MatchingParamsBase> & matching_params)
{
  if (
    auto det3d_matching_params = std::dynamic_pointer_cast<Det3dMatchingParams>(matching_params)) {
    auto info =
      std::make_shared<NaiveCollectorInfo>(det3d_matching_params->det3d_timestamp, threshold_);
    collector->set_info(info);
  } else if (
    auto rois_matching_params = std::dynamic_pointer_cast<RoisMatchingParams>(matching_params)) {
    auto info = std::make_shared<NaiveCollectorInfo>(
      rois_matching_params->rois_timestamp - id_to_offset_map_.at(rois_matching_params->rois_id),
      threshold_);
    collector->set_info(info);
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::AdvancedMatchingStrategy(
  std::shared_ptr<FusionNode<Msg3D, Msg2D, ExportObj>> && ros2_parent_node, std::size_t rois_number)
: ros2_parent_node_(std::move(ros2_parent_node))
{
  auto rois_timestamp_offsets =
    ros2_parent_node_->template declare_parameter<std::vector<double>>("rois_timestamp_offsets");
  det3d_noise_window_ =
    ros2_parent_node_->template declare_parameter<double>("matching_strategy.det3d_noise_window");
  auto rois_timestamp_noise_window =
    ros2_parent_node_->template declare_parameter<std::vector<double>>(
      "matching_strategy.rois_timestamp_noise_window");

  if (rois_timestamp_offsets.size() != rois_number) {
    throw std::runtime_error("The number of rois does not match the number of timestamp offsets.");
  }
  if (rois_timestamp_noise_window.size() != rois_number) {
    throw std::runtime_error(
      "The number of rois does not match the number of timestamp noise window.");
  }

  for (size_t i = 0; i < rois_number; i++) {
    id_to_offset_map_[i] = rois_timestamp_offsets[i];
    id_to_noise_window_map_[i] = rois_timestamp_noise_window[i];
  }

  RCLCPP_INFO(
    ros2_parent_node_->get_logger(), "Utilize advanced matching strategy for fusion nodes.");
}

template <class Msg3D, class Msg2D, class ExportObj>
std::optional<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>>
AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::match_rois_to_collector(
  const std::list<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> & fusion_collectors,
  const std::shared_ptr<RoisMatchingParams> & params) const
{
  for (const auto & fusion_collector : fusion_collectors) {
    auto info = fusion_collector->get_info();
    if (auto advanced_info = std::dynamic_pointer_cast<AdvancedCollectorInfo>(info)) {
      auto reference_timestamp_min = advanced_info->timestamp - advanced_info->noise_window;
      auto reference_timestamp_max = advanced_info->timestamp + advanced_info->noise_window;
      double time = params->rois_timestamp - id_to_offset_map_.at(params->rois_id);
      if (
        time < reference_timestamp_max + id_to_noise_window_map_.at(params->rois_id) &&
        time > reference_timestamp_min - id_to_noise_window_map_.at(params->rois_id)) {
        return fusion_collector;
      }
    }
  }
  return std::nullopt;
}

template <class Msg3D, class Msg2D, class ExportObj>
std::optional<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>>
AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::match_det3d_to_collector(
  const std::list<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> & fusion_collectors,
  const std::shared_ptr<Det3dMatchingParams> & params)
{
  auto concatenated_status = ros2_parent_node_->find_concatenation_status(params->det3d_timestamp);

  // TODO(vivid): double check this logic and fix the name
  double offset = get_offset(params->det3d_timestamp, concatenated_status);

  for (const auto & fusion_collector : fusion_collectors) {
    auto info = fusion_collector->get_info();
    if (auto advanced_info = std::dynamic_pointer_cast<AdvancedCollectorInfo>(info)) {
      auto reference_timestamp_min = advanced_info->timestamp - advanced_info->noise_window;
      auto reference_timestamp_max = advanced_info->timestamp + advanced_info->noise_window;
      if (
        params->det3d_timestamp - offset < reference_timestamp_max + det3d_noise_window_ &&
        params->det3d_timestamp - offset > reference_timestamp_min - det3d_noise_window_) {
        return fusion_collector;
      }
    }
  }
  return std::nullopt;
}

template <class Msg3D, class Msg2D, class ExportObj>
void AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::set_collector_info(
  std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>> & collector,
  const std::shared_ptr<MatchingParamsBase> & matching_params)
{
  if (
    auto det3d_matching_params = std::dynamic_pointer_cast<Det3dMatchingParams>(matching_params)) {
    auto concatenated_status =
      ros2_parent_node_->find_concatenation_status(det3d_matching_params->det3d_timestamp);
    double offset = get_offset(det3d_matching_params->det3d_timestamp, concatenated_status);

    auto info = std::make_shared<AdvancedCollectorInfo>(
      det3d_matching_params->det3d_timestamp - offset, det3d_noise_window_);
    collector->set_info(info);
  } else if (
    auto rois_matching_params = std::dynamic_pointer_cast<RoisMatchingParams>(matching_params)) {
    auto info = std::make_shared<AdvancedCollectorInfo>(
      rois_matching_params->rois_timestamp - id_to_offset_map_.at(rois_matching_params->rois_id),
      id_to_noise_window_map_[rois_matching_params->rois_id]);
    collector->set_info(info);
  }
}

template <class Msg3D, class Msg2D, class ExportObj>
double AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::get_offset(
  const double & det3d_timestamp,
  const std::optional<std::unordered_map<std::string, std::string>> & concatenated_status)
{
  double offset = 0.0;

  if (concatenated_status) {
    const auto & status_map = concatenated_status.value();

    if (
      status_map.find("cloud_concatenation_success") != status_map.end() &&
      status_map.find("reference_timestamp_min") != status_map.end() &&
      status_map.find("reference_timestamp_max") != status_map.end()) {
      bool concatenation_success = (status_map.at("cloud_concatenation_success") == "True");

      if (
        !concatenation_success &&
        det3d_timestamp > std::stod(status_map.at("reference_timestamp_max"))) {
        // The defined earliest pointcloud is missed in the concatenation
        double reference_min = std::stod(status_map.at("reference_timestamp_min"));
        double reference_max = std::stod(status_map.at("reference_timestamp_max"));
        offset = det3d_timestamp - (reference_min + (reference_max - reference_min) / 2);
      } else if (!database_created_ && concatenation_success) {
        // Ensure "cloud_concatenated_timestamp" key exists before accessing
        if (status_map.find("concatenated_cloud_timestamp") != status_map.end()) {
          double concatenated_cloud_timestamp =
            std::stod(status_map.at("concatenated_cloud_timestamp"));
          update_fractional_timestamp_set(concatenated_cloud_timestamp);
          success_status_counter_++;
          offset = 0.0;

          if (success_status_counter_ > success_threshold) {
            database_created_ = true;
          }
        }
      }
    }
  } else {
    RCLCPP_WARN(
      ros2_parent_node_->get_logger(),
      "If concatenated_status is missing, find the offset using the timestamp difference");
    // If concatenated_status is missing, find the offset using the timestamp difference
    if (database_created_) {
      offset = compute_offset(det3d_timestamp);
      RCLCPP_WARN(ros2_parent_node_->get_logger(), "Use database and find offset: %f", offset);

    } else {
      offset = 0.0;
    }
  }

  return offset;
}

template <class Msg3D, class Msg2D, class ExportObj>
double AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::extract_fractional(double timestamp)
{
  return fmod(timestamp, 1.0);
}

template <class Msg3D, class Msg2D, class ExportObj>
void AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::update_fractional_timestamp_set(
  double timestamp)
{
  double fractional_part = extract_fractional(timestamp);

  // Check if the new timestamp belongs to an existing element within noise tolerance
  for (auto existing_timestamp : fractional_timestamp_set_) {
    if (std::abs(fractional_part - existing_timestamp) < det3d_noise_window_ * 2) {
      existing_timestamp = (existing_timestamp + fractional_part) / 2;
      return;  // If it belongs to an existing group, average the timestamp
    }
  }

  fractional_timestamp_set_.insert(fractional_part);
}

template <class Msg3D, class Msg2D, class ExportObj>
double AdvancedMatchingStrategy<Msg3D, Msg2D, ExportObj>::compute_offset(double input_timestamp)
{
  if (fractional_timestamp_set_.empty()) {
    return 0.0;
  }

  double fractional_part = extract_fractional(input_timestamp);
  double expected_timestamp = -1.0;

  // Check if input timestamp is within an existing timestamp ± noise_tolerance
  for (const auto & timestamp : fractional_timestamp_set_) {
    if (
      fractional_part >= timestamp - det3d_noise_window_ &&
      fractional_part < timestamp + det3d_noise_window_) {
      return 0.0;  // If within range, offset is zero
    }
  }

  // Find the closest timestamp ≤ fractional_part
  auto it = fractional_timestamp_set_.lower_bound(fractional_part);
  if (it == fractional_timestamp_set_.end()) {
    --it;
    expected_timestamp = floor(input_timestamp) + *it;
  } else if (it == fractional_timestamp_set_.begin()) {
    // **If `new_timestamp` is smaller than all stored timestamps, use the largest timestamp**
    expected_timestamp = floor(input_timestamp) - 1 + *fractional_timestamp_set_.rbegin();
  } else {
    --it;
    expected_timestamp = floor(input_timestamp) + *it;
  }
  return input_timestamp - expected_timestamp;
}

// pointpainting fusion
template class NaiveMatchingStrategy<PointCloudMsgType, RoiMsgType, DetectedObjects>;

// roi cluster fusion
template class NaiveMatchingStrategy<ClusterMsgType, RoiMsgType, ClusterMsgType>;

// roi detected-object fusion
template class NaiveMatchingStrategy<DetectedObjects, RoiMsgType, DetectedObjects>;

// roi pointcloud fusion
template class NaiveMatchingStrategy<PointCloudMsgType, RoiMsgType, ClusterMsgType>;

// segment pointcloud fusion
template class NaiveMatchingStrategy<PointCloudMsgType, Image, PointCloudMsgType>;

// pointpainting fusion
template class AdvancedMatchingStrategy<PointCloudMsgType, RoiMsgType, DetectedObjects>;

// roi cluster fusion
template class AdvancedMatchingStrategy<ClusterMsgType, RoiMsgType, ClusterMsgType>;

// roi detected-object fusion
template class AdvancedMatchingStrategy<DetectedObjects, RoiMsgType, DetectedObjects>;

// roi pointcloud fusion
template class AdvancedMatchingStrategy<PointCloudMsgType, RoiMsgType, ClusterMsgType>;

// segment pointcloud fusion
template class AdvancedMatchingStrategy<PointCloudMsgType, Image, PointCloudMsgType>;

}  // namespace autoware::image_projection_based_fusion

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

#include "autoware/pointcloud_preprocessor/concatenate_data/cloud_collector.hpp"

#include <autoware/pointcloud_preprocessor/concatenate_data/collector_matching_strategy.hpp>
#include <rclcpp/rclcpp.hpp>

#include <cmath>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace autoware::pointcloud_preprocessor
{

template <typename PointCloudMessage>
NaiveMatchingStrategy<PointCloudMessage>::NaiveMatchingStrategy(rclcpp::Node & node)
{
  RCLCPP_INFO(node.get_logger(), "Utilize naive matching strategy");
}

template <typename PointCloudMessage>
std::optional<std::shared_ptr<CloudCollector<PointCloudMessage>>>
NaiveMatchingStrategy<PointCloudMessage>::match_cloud_to_collector(
  const std::list<std::shared_ptr<CloudCollector<PointCloudMessage>>> & cloud_collectors,
  const MatchingParams & params) const
{
  std::optional<double> smallest_time_difference = std::nullopt;
  std::shared_ptr<CloudCollector<PointCloudMessage>> closest_collector = nullptr;

  for (const auto & cloud_collector : cloud_collectors) {
    if (!cloud_collector->topic_exists(params.topic_name)) {
      auto info = cloud_collector->get_info();
      if (auto naive_info = std::dynamic_pointer_cast<NaiveCollectorInfo>(info)) {
        double time_difference = std::abs(params.cloud_arrival_time - naive_info->timestamp);
        if (!smallest_time_difference || time_difference < smallest_time_difference) {
          smallest_time_difference = time_difference;
          closest_collector = cloud_collector;
        }
      }
    }
  }

  if (closest_collector != nullptr) {
    return closest_collector;
  }
  return std::nullopt;
}

template <typename PointCloudMessage>
void NaiveMatchingStrategy<PointCloudMessage>::set_collector_info(
  std::shared_ptr<CloudCollector<PointCloudMessage>> & collector,
  const MatchingParams & matching_params)
{
  auto info = std::make_shared<NaiveCollectorInfo>(matching_params.cloud_arrival_time);
  collector->set_info(info);
}

template <typename PointCloudMessage>
AdvancedMatchingStrategy<PointCloudMessage>::AdvancedMatchingStrategy(
  rclcpp::Node & node, std::vector<std::string> input_topics)
{
  auto lidar_timestamp_offsets =
    node.declare_parameter<std::vector<double>>("matching_strategy.lidar_timestamp_offsets");
  auto lidar_timestamp_noise_window =
    node.declare_parameter<std::vector<double>>("matching_strategy.lidar_timestamp_noise_window");

  if (lidar_timestamp_offsets.size() != input_topics.size()) {
    throw std::runtime_error(
      "The number of topics does not match the number of timestamp offsets.");
  }
  if (lidar_timestamp_noise_window.size() != input_topics.size()) {
    throw std::runtime_error(
      "The number of topics does not match the number of timestamp noise window.");
  }

  for (size_t i = 0; i < input_topics.size(); i++) {
    topic_to_offset_map_[input_topics[i]] = lidar_timestamp_offsets[i];
    topic_to_noise_window_map_[input_topics[i]] = lidar_timestamp_noise_window[i];
  }

  RCLCPP_INFO(node.get_logger(), "Utilize advanced matching strategy");
}

template <typename PointCloudMessage>
std::optional<std::shared_ptr<CloudCollector<PointCloudMessage>>>
AdvancedMatchingStrategy<PointCloudMessage>::match_cloud_to_collector(
  const std::list<std::shared_ptr<CloudCollector<PointCloudMessage>>> & cloud_collectors,
  const MatchingParams & params) const
{
  for (const auto & cloud_collector : cloud_collectors) {
    auto info = cloud_collector->get_info();
    if (auto advanced_info = std::dynamic_pointer_cast<AdvancedCollectorInfo>(info)) {
      auto reference_timestamp_min = advanced_info->timestamp - advanced_info->noise_window;
      auto reference_timestamp_max = advanced_info->timestamp + advanced_info->noise_window;
      double time = params.cloud_timestamp - topic_to_offset_map_.at(params.topic_name);
      if (
        time < reference_timestamp_max + topic_to_noise_window_map_.at(params.topic_name) &&
        time > reference_timestamp_min - topic_to_noise_window_map_.at(params.topic_name)) {
        return cloud_collector;
      }
    }
  }
  return std::nullopt;
}

template <typename PointCloudMessage>
void AdvancedMatchingStrategy<PointCloudMessage>::set_collector_info(
  std::shared_ptr<CloudCollector<PointCloudMessage>> & collector,
  const MatchingParams & matching_params)
{
  auto info = std::make_shared<AdvancedCollectorInfo>(
    matching_params.cloud_timestamp - topic_to_offset_map_.at(matching_params.topic_name),
    topic_to_noise_window_map_[matching_params.topic_name]);
  collector->set_info(info);
}

}  // namespace autoware::pointcloud_preprocessor

template class autoware::pointcloud_preprocessor::NaiveMatchingStrategy<
  sensor_msgs::msg::PointCloud2>;
template class autoware::pointcloud_preprocessor::AdvancedMatchingStrategy<
  sensor_msgs::msg::PointCloud2>;

#ifdef USE_CUDA
template class autoware::pointcloud_preprocessor::NaiveMatchingStrategy<
  cuda_blackboard::CudaPointCloud2>;
template class autoware::pointcloud_preprocessor::AdvancedMatchingStrategy<
  cuda_blackboard::CudaPointCloud2>;
#endif

// Copyright 2024 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef AUTOWARE__CUDA_ORGANIZED_POINTCLOUD_ADAPTER__CUDA_ORGANIZED_POINTCLOUD_ADAPTER_NODE_HPP_
#define AUTOWARE__CUDA_ORGANIZED_POINTCLOUD_ADAPTER__CUDA_ORGANIZED_POINTCLOUD_ADAPTER_NODE_HPP_

#include <autoware/point_types/types.hpp>
#include <autoware/universe_utils/ros/debug_publisher.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <cuda_blackboard/cuda_adaptation.hpp>
#include <cuda_blackboard/cuda_blackboard_publisher.hpp>
#include <cuda_blackboard/cuda_pointcloud2.hpp>
#include <rclcpp/rclcpp.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>

#include <tf2/transform_datatypes.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <chrono>
#include <deque>
#include <memory>
#include <vector>

namespace autoware::cuda_organized_pointcloud_adapter
{

class CudaOrganizedPointcloudAdapterNode : public rclcpp::Node
{
public:
  explicit CudaOrganizedPointcloudAdapterNode(const rclcpp::NodeOptions & node_options);
  ~CudaOrganizedPointcloudAdapterNode() = default;

private:
  void estimatePointcloudRingInfo(
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_pointcloud_msg_ptr);

  bool orderPointcloud(
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_pointcloud_msg_ptr);

  // Callback
  void pointcloudCallback(
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_pointcloud_msg_ptr);

  // Subscriber
  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr pointcloud_sub_{};

  // CUDA pub
  std::unique_ptr<cuda_blackboard::CudaBlackboardPublisher<cuda_blackboard::CudaPointCloud2>> pub_;

  std::size_t num_rings_{0};
  std::size_t max_points_per_ring_{0};

  std::vector<std::size_t> next_ring_index_;
  cuda_blackboard::HostUniquePtr<autoware::point_types::PointXYZIRCAEDT[]> host_buffer_;
  cuda_blackboard::CudaUniquePtr<std::uint8_t[]> device_buffer_;

  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> debug_publisher_;
};

}  // namespace autoware::cuda_organized_pointcloud_adapter

#endif  // AUTOWARE__CUDA_ORGANIZED_POINTCLOUD_ADAPTER__CUDA_ORGANIZED_POINTCLOUD_ADAPTER_NODE_HPP_

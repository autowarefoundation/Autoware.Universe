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

#include "autoware/cuda_organized_pointcloud_adapter/cuda_organized_pointcloud_adapter_node.hpp"

#include <cuda_runtime.h>

#include <vector>

namespace autoware::cuda_organized_pointcloud_adapter
{
using sensor_msgs::msg::PointCloud2;

CudaOrganizedPointcloudAdapterNode::CudaOrganizedPointcloudAdapterNode(
  const rclcpp::NodeOptions & node_options)
: Node("cuda_organized_pointcloud_adapter", node_options)
{
  pub_ =
    std::make_unique<cuda_blackboard::CudaBlackboardPublisher<cuda_blackboard::CudaPointCloud2>>(
      *this, "~/output/pointcloud");

  rclcpp::SubscriptionOptions sub_options;
  sub_options.use_intra_process_comm = rclcpp::IntraProcessSetting::Enable;

  pointcloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
    "~/input/pointcloud", rclcpp::SensorDataQoS{}.keep_last(1),
    std::bind(&CudaOrganizedPointcloudAdapterNode::pointcloudCallback, this, std::placeholders::_1),
    sub_options);

  // initialize debug tool
  {
    using autoware::universe_utils::DebugPublisher;
    using autoware::universe_utils::StopWatch;

    stop_watch_ptr_ = std::make_unique<StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ = std::make_unique<DebugPublisher>(this, "cuda_organized_pointcloud_adapter");
    stop_watch_ptr_->tic("processing_time");
  }
}

void CudaOrganizedPointcloudAdapterNode::estimatePointcloudRingInfo(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_pointcloud_msg_ptr)
{
  const autoware::point_types::PointXYZIRCAEDT * input_buffer =
    reinterpret_cast<const autoware::point_types::PointXYZIRCAEDT *>(
      input_pointcloud_msg_ptr->data.data());

  std::size_t max_ring = 0;

  for (std::size_t i = 0; i < input_pointcloud_msg_ptr->width * input_pointcloud_msg_ptr->height;
       i++) {
    const autoware::point_types::PointXYZIRCAEDT & point = input_buffer[i];
    const std::size_t ring = static_cast<std::size_t>(point.channel);
    max_ring = std::max(max_ring, ring);
  }

  // Set max rings to the next power of two
  num_rings_ = std::pow(2, std::ceil(std::log2(max_ring + 1)));
  num_rings_ = std::max(num_rings_, static_cast<std::size_t>(16));
  std::vector<std::size_t> ring_points(num_rings_, 0);

  for (std::size_t i = 0; i < input_pointcloud_msg_ptr->width * input_pointcloud_msg_ptr->height;
       i++) {
    const autoware::point_types::PointXYZIRCAEDT & point = input_buffer[i];
    const std::size_t ring = point.channel;
    ring_points[ring]++;
  }

  // Set max points per ring to the next multiple of 512
  max_points_per_ring_ = *std::max_element(ring_points.begin(), ring_points.end());
  max_points_per_ring_ = std::max(max_points_per_ring_, static_cast<std::size_t>(512));
  max_points_per_ring_ = (max_points_per_ring_ + 511) / 512 * 512;

  next_ring_index_.resize(num_rings_);
  std::fill(next_ring_index_.begin(), next_ring_index_.end(), 0);
  host_buffer_ = cuda_blackboard::make_host_unique<autoware::point_types::PointXYZIRCAEDT[]>(
    num_rings_ * max_points_per_ring_);

  device_buffer_ = cuda_blackboard::make_unique<std::uint8_t[]>(
    num_rings_ * max_points_per_ring_ * sizeof(autoware::point_types::PointXYZIRCAEDT));

  RCLCPP_INFO_STREAM(
    get_logger(), "Estimated rings: " << num_rings_
                                      << ", max_points_per_ring: " << max_points_per_ring_
                                      << ". This should only be done during the first iterations. "
                                         "Otherwise, performance will be affected.");
}

bool CudaOrganizedPointcloudAdapterNode::orderPointcloud(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_pointcloud_msg_ptr)
{
  const autoware::point_types::PointXYZIRCAEDT * input_buffer =
    reinterpret_cast<const autoware::point_types::PointXYZIRCAEDT *>(
      input_pointcloud_msg_ptr->data.data());

  bool ring_overflow = false;
  bool point_overflow = false;

  autoware::point_types::PointXYZIRCAEDT * buffer = host_buffer_.get();

  for (std::size_t i = 0; i < input_pointcloud_msg_ptr->width * input_pointcloud_msg_ptr->height;
       i++) {
    const autoware::point_types::PointXYZIRCAEDT & point = input_buffer[i];
    const std::size_t raw_ring = point.channel;
    const std::size_t ring = raw_ring % num_rings_;

    const std::size_t raw_index = next_ring_index_[ring];
    const std::size_t index = raw_index % max_points_per_ring_;

    ring_overflow |= raw_ring >= num_rings_;
    point_overflow |= raw_index >= max_points_per_ring_;

    buffer[ring * max_points_per_ring_ + index] = point;
    next_ring_index_[ring] = raw_index + 1;
  }

  return !ring_overflow && !point_overflow;
}

void CudaOrganizedPointcloudAdapterNode::pointcloudCallback(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr input_pointcloud_msg_ptr)
{
  stop_watch_ptr_->toc("processing_time", true);

  // TODO(knzo25): check the pointcloud layout at least once

  assert(input_pointcloud_msg_ptr->point_step == sizeof(autoware::point_types::PointXYZIRCAEDT));

  if (num_rings_ == 0 || max_points_per_ring_ == 0) {
    estimatePointcloudRingInfo(input_pointcloud_msg_ptr);
  }

  if (!orderPointcloud(input_pointcloud_msg_ptr)) {
    estimatePointcloudRingInfo(input_pointcloud_msg_ptr);
    orderPointcloud(input_pointcloud_msg_ptr);
  }

  // Copy to cuda memory
  cudaMemcpy(
    device_buffer_.get(), host_buffer_.get(),
    num_rings_ * max_points_per_ring_ * sizeof(autoware::point_types::PointXYZIRCAEDT),
    cudaMemcpyHostToDevice);

  auto cuda_pointcloud_msg_ptr = std::make_unique<cuda_blackboard::CudaPointCloud2>();
  cuda_pointcloud_msg_ptr->fields = input_pointcloud_msg_ptr->fields;
  cuda_pointcloud_msg_ptr->width = max_points_per_ring_;
  cuda_pointcloud_msg_ptr->height = num_rings_;
  cuda_pointcloud_msg_ptr->point_step = sizeof(autoware::point_types::PointXYZIRCAEDT);
  cuda_pointcloud_msg_ptr->row_step =
    max_points_per_ring_ * sizeof(autoware::point_types::PointXYZIRCAEDT);
  cuda_pointcloud_msg_ptr->data =
    std::move(device_buffer_); /*reinterpret_cast<uint8_t *>(device_buffer_)*/
  ;
  cuda_pointcloud_msg_ptr->is_dense = input_pointcloud_msg_ptr->is_dense;
  cuda_pointcloud_msg_ptr->header = input_pointcloud_msg_ptr->header;

  pub_->publish(std::move(cuda_pointcloud_msg_ptr));

  if (debug_publisher_) {
    const double processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);

    double now_stamp_seconds = rclcpp::Time(this->get_clock()->now()).seconds();
    double cloud_stamp_seconds = rclcpp::Time(input_pointcloud_msg_ptr->header.stamp).seconds();

    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/latency_ms", 1000.f * (now_stamp_seconds - cloud_stamp_seconds));
  }

  // Allocate cuda memory
  device_buffer_ = cuda_blackboard::make_unique<std::uint8_t[]>(
    num_rings_ * max_points_per_ring_ * sizeof(autoware::point_types::PointXYZIRCAEDT));

  // Clear indexes
  std::fill(next_ring_index_.begin(), next_ring_index_.end(), 0);

  // Clear pointcloud buffer
  auto host_buffer = host_buffer_.get();
  for (std::size_t i = 0; i < num_rings_ * max_points_per_ring_; i++) {
    host_buffer[i] = autoware::point_types::PointXYZIRCAEDT{};
  }
}

}  // namespace autoware::cuda_organized_pointcloud_adapter

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(
  autoware::cuda_organized_pointcloud_adapter::CudaOrganizedPointcloudAdapterNode)

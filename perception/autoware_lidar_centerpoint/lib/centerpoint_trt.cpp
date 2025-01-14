// Copyright 2021 TIER IV, Inc.
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

#include "autoware/lidar_centerpoint/centerpoint_trt.hpp"

#include "autoware/lidar_centerpoint/centerpoint_config.hpp"
#include "autoware/lidar_centerpoint/network/scatter_kernel.hpp"
#include "autoware/lidar_centerpoint/preprocess/preprocess_kernel.hpp"

#include <autoware/universe_utils/math/constants.hpp>
#include <autoware/universe_utils/ros/diagnostics_interface.hpp>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware::lidar_centerpoint
{
CenterPointTRT::CenterPointTRT(
  const TrtCommonConfig & encoder_param, const TrtCommonConfig & head_param,
  const DensificationParam & densification_param, const CenterPointConfig & config)
: config_(config)
{
  vg_ptr_ = std::make_unique<VoxelGenerator>(densification_param, config_);
  post_proc_ptr_ = std::make_unique<PostProcessCUDA>(config_);

  initPtr();
  initTrt(encoder_param, head_param);

  cudaStreamCreate(&stream_);
}

CenterPointTRT::~CenterPointTRT()
{
  if (stream_) {
    cudaStreamSynchronize(stream_);
    cudaStreamDestroy(stream_);
  }
}

void CenterPointTRT::initPtr()
{
  voxels_size_ =
    config_.max_voxel_size_ * config_.max_point_in_voxel_size_ * config_.point_feature_size_;
  coordinates_size_ = config_.max_voxel_size_ * config_.point_dim_size_;
  encoder_in_feature_size_ =
    config_.max_voxel_size_ * config_.max_point_in_voxel_size_ * config_.encoder_in_feature_size_;
  const auto pillar_features_size = config_.max_voxel_size_ * config_.encoder_out_feature_size_;
  spatial_features_size_ =
    config_.grid_size_x_ * config_.grid_size_y_ * config_.encoder_out_feature_size_;
  const auto grid_xy_size = config_.down_grid_size_x_ * config_.down_grid_size_y_;

  voxels_buffer_size_ = config_.grid_size_x_ * config_.grid_size_y_ *
                        config_.max_point_in_voxel_size_ * config_.point_feature_size_;
  mask_size_ = config_.grid_size_x_ * config_.grid_size_y_;

  // host
  points_.resize(config_.cloud_capacity_ * config_.point_feature_size_);

  // device
  voxels_d_ = cuda::make_unique<float[]>(voxels_size_);
  coordinates_d_ = cuda::make_unique<int[]>(coordinates_size_);
  num_points_per_voxel_d_ = cuda::make_unique<float[]>(config_.max_voxel_size_);
  encoder_in_features_d_ = cuda::make_unique<float[]>(encoder_in_feature_size_);
  pillar_features_d_ = cuda::make_unique<float[]>(pillar_features_size);
  spatial_features_d_ = cuda::make_unique<float[]>(spatial_features_size_);
  head_out_heatmap_d_ = cuda::make_unique<float[]>(grid_xy_size * config_.class_size_);
  head_out_offset_d_ = cuda::make_unique<float[]>(grid_xy_size * config_.head_out_offset_size_);
  head_out_z_d_ = cuda::make_unique<float[]>(grid_xy_size * config_.head_out_z_size_);
  head_out_dim_d_ = cuda::make_unique<float[]>(grid_xy_size * config_.head_out_dim_size_);
  head_out_rot_d_ = cuda::make_unique<float[]>(grid_xy_size * config_.head_out_rot_size_);
  head_out_vel_d_ = cuda::make_unique<float[]>(grid_xy_size * config_.head_out_vel_size_);
  points_d_ = cuda::make_unique<float[]>(config_.cloud_capacity_ * config_.point_feature_size_);
  voxels_buffer_d_ = cuda::make_unique<float[]>(voxels_buffer_size_);
  mask_d_ = cuda::make_unique<unsigned int[]>(mask_size_);
  num_voxels_d_ = cuda::make_unique<unsigned int[]>(1);

  points_aux_d_ = cuda::make_unique<float[]>(config_.cloud_capacity_ * config_.point_feature_size_);
  shuffle_indices_d_ = cuda::make_unique<unsigned int[]>(config_.cloud_capacity_);

  std::vector<unsigned int> indexes(config_.cloud_capacity_);
  std::iota(indexes.begin(), indexes.end(), 0);

  std::default_random_engine e(0);
  std::shuffle(indexes.begin(), indexes.end(), e);

  std::srand(std::time(nullptr));

  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    shuffle_indices_d_.get(), indexes.data(), config_.cloud_capacity_ * sizeof(unsigned int),
    cudaMemcpyHostToDevice, stream_));
}

void CenterPointTRT::initTrt(
  const TrtCommonConfig & encoder_param, const TrtCommonConfig & head_param)
{
  // encoder input profile
  auto enc_in_dims = nvinfer1::Dims{
    3,
    {static_cast<int32_t>(config_.max_voxel_size_),
     static_cast<int32_t>(config_.max_point_in_voxel_size_),
     static_cast<int32_t>(config_.encoder_in_feature_size_)}};
  std::vector<tensorrt_common::ProfileDims> encoder_profile_dims{
    tensorrt_common::ProfileDims(0, enc_in_dims, enc_in_dims, enc_in_dims)};
  auto encoder_profile_dims_ptr =
    std::make_unique<std::vector<tensorrt_common::ProfileDims>>(encoder_profile_dims);

  // head input profile
  auto head_in_dims = nvinfer1::Dims{
    4,
    {static_cast<int32_t>(config_.batch_size_),
     static_cast<int32_t>(config_.encoder_out_feature_size_),
     static_cast<int32_t>(config_.grid_size_y_), static_cast<int32_t>(config_.grid_size_x_)}};
  std::vector<tensorrt_common::ProfileDims> head_profile_dims{
    tensorrt_common::ProfileDims(0, head_in_dims, head_in_dims, head_in_dims)};
  std::unordered_map<int32_t, std::int32_t> out_channel_map = {
    {1, static_cast<int32_t>(config_.class_size_)},
    {2, static_cast<int32_t>(config_.head_out_offset_size_)},
    {3, static_cast<int32_t>(config_.head_out_z_size_)},
    {4, static_cast<int32_t>(config_.head_out_dim_size_)},
    {5, static_cast<int32_t>(config_.head_out_rot_size_)},
    {6, static_cast<int32_t>(config_.head_out_vel_size_)}};
  for (const auto & [tensor_name, channel_size] : out_channel_map) {
    auto dims = nvinfer1::Dims{
      4,
      {static_cast<int32_t>(config_.batch_size_), channel_size,
       static_cast<int32_t>(config_.down_grid_size_y_),
       static_cast<int32_t>(config_.down_grid_size_x_)}};
    head_profile_dims.emplace_back(tensor_name, dims, dims, dims);
  }
  auto head_profile_dims_ptr =
    std::make_unique<std::vector<tensorrt_common::ProfileDims>>(head_profile_dims);

  // initialize trt wrappers
  encoder_trt_ptr_ = std::make_unique<tensorrt_common::TrtCommon>(encoder_param);

  head_trt_ptr_ = std::make_unique<tensorrt_common::TrtCommon>(head_param);

  // setup trt engines
  if (
    !encoder_trt_ptr_->setup(std::move(encoder_profile_dims_ptr)) ||
    !head_trt_ptr_->setup(std::move(head_profile_dims_ptr))) {
    throw std::runtime_error("Failed to setup TRT engine.");
  }

  // set input shapes
  if (
    !encoder_trt_ptr_->setInputShape(0, enc_in_dims) ||
    !head_trt_ptr_->setInputShape(0, head_in_dims)) {
    throw std::runtime_error("Failed to set input shape.");
  }
}

bool CenterPointTRT::detect(
  const sensor_msgs::msg::PointCloud2 & input_pointcloud_msg, const tf2_ros::Buffer & tf_buffer,
  std::vector<Box3D> & det_boxes3d, bool & is_num_pillars_within_range)
{
  is_num_pillars_within_range = true;

  CHECK_CUDA_ERROR(cudaMemsetAsync(
    encoder_in_features_d_.get(), 0, encoder_in_feature_size_ * sizeof(float), stream_));
  CHECK_CUDA_ERROR(
    cudaMemsetAsync(spatial_features_d_.get(), 0, spatial_features_size_ * sizeof(float), stream_));

  if (!preprocess(input_pointcloud_msg, tf_buffer)) {
    RCLCPP_WARN(rclcpp::get_logger("lidar_centerpoint"), "Fail to preprocess and skip to detect.");
    return false;
  }

  inference();

  postProcess(det_boxes3d);

  // Check the actual number of pillars after inference to avoid unnecessary synchronization.
  unsigned int num_pillars = 0;
  CHECK_CUDA_ERROR(
    cudaMemcpy(&num_pillars, num_voxels_d_.get(), sizeof(unsigned int), cudaMemcpyDeviceToHost));

  if (num_pillars >= config_.max_voxel_size_) {
    rclcpp::Clock clock{RCL_ROS_TIME};
    RCLCPP_WARN_THROTTLE(
      rclcpp::get_logger("lidar_centerpoint"), clock, 1000,
      "The actual number of pillars (%u) exceeds its maximum value (%zu). "
      "Please considering increasing it since it may limit the detection performance.",
      num_pillars, config_.max_voxel_size_);
    is_num_pillars_within_range = false;
  }

  return true;
}

bool CenterPointTRT::preprocess(
  const sensor_msgs::msg::PointCloud2 & input_pointcloud_msg, const tf2_ros::Buffer & tf_buffer)
{
  bool is_success = vg_ptr_->enqueuePointCloud(input_pointcloud_msg, tf_buffer, stream_);
  if (!is_success) {
    return false;
  }

  const std::size_t count = vg_ptr_->generateSweepPoints(points_aux_d_.get(), stream_);
  const std::size_t random_offset = std::rand() % config_.cloud_capacity_;
  CHECK_CUDA_ERROR(shufflePoints_launch(
    points_aux_d_.get(), shuffle_indices_d_.get(), points_d_.get(), count, config_.cloud_capacity_,
    random_offset, stream_));

  CHECK_CUDA_ERROR(cudaMemsetAsync(num_voxels_d_.get(), 0, sizeof(unsigned int), stream_));
  CHECK_CUDA_ERROR(
    cudaMemsetAsync(voxels_buffer_d_.get(), 0, voxels_buffer_size_ * sizeof(float), stream_));
  CHECK_CUDA_ERROR(cudaMemsetAsync(mask_d_.get(), 0, mask_size_ * sizeof(int), stream_));
  CHECK_CUDA_ERROR(cudaMemsetAsync(voxels_d_.get(), 0, voxels_size_ * sizeof(float), stream_));
  CHECK_CUDA_ERROR(
    cudaMemsetAsync(coordinates_d_.get(), 0, coordinates_size_ * sizeof(int), stream_));
  CHECK_CUDA_ERROR(cudaMemsetAsync(
    num_points_per_voxel_d_.get(), 0, config_.max_voxel_size_ * sizeof(float), stream_));

  CHECK_CUDA_ERROR(generateVoxels_random_launch(
    points_d_.get(), config_.cloud_capacity_, config_.range_min_x_, config_.range_max_x_,
    config_.range_min_y_, config_.range_max_y_, config_.range_min_z_, config_.range_max_z_,
    config_.voxel_size_x_, config_.voxel_size_y_, config_.voxel_size_z_, config_.grid_size_y_,
    config_.grid_size_x_, mask_d_.get(), voxels_buffer_d_.get(), stream_));

  CHECK_CUDA_ERROR(generateBaseFeatures_launch(
    mask_d_.get(), voxels_buffer_d_.get(), config_.grid_size_y_, config_.grid_size_x_,
    config_.max_voxel_size_, num_voxels_d_.get(), voxels_d_.get(), num_points_per_voxel_d_.get(),
    coordinates_d_.get(), stream_));

  CHECK_CUDA_ERROR(generateFeatures_launch(
    voxels_d_.get(), num_points_per_voxel_d_.get(), coordinates_d_.get(), num_voxels_d_.get(),
    config_.max_voxel_size_, config_.voxel_size_x_, config_.voxel_size_y_, config_.voxel_size_z_,
    config_.range_min_x_, config_.range_min_y_, config_.range_min_z_, encoder_in_features_d_.get(),
    stream_));

  return true;
}

void CenterPointTRT::inference()
{
  // pillar encoder network
  std::vector<void *> encoder_tensors = {encoder_in_features_d_.get(), pillar_features_d_.get()};
  encoder_trt_ptr_->setTensorsAddresses(encoder_tensors);
  encoder_trt_ptr_->enqueueV3(stream_);

  // scatter
  CHECK_CUDA_ERROR(scatterFeatures_launch(
    pillar_features_d_.get(), coordinates_d_.get(), num_voxels_d_.get(), config_.max_voxel_size_,
    config_.encoder_out_feature_size_, config_.grid_size_x_, config_.grid_size_y_,
    spatial_features_d_.get(), stream_));

  // head network
  std::vector<void *> head_tensors = {spatial_features_d_.get(), head_out_heatmap_d_.get(),
                                      head_out_offset_d_.get(),  head_out_z_d_.get(),
                                      head_out_dim_d_.get(),     head_out_rot_d_.get(),
                                      head_out_vel_d_.get()};
  head_trt_ptr_->setTensorsAddresses(head_tensors);

  head_trt_ptr_->enqueueV3(stream_);
}

void CenterPointTRT::postProcess(std::vector<Box3D> & det_boxes3d)
{
  CHECK_CUDA_ERROR(post_proc_ptr_->generateDetectedBoxes3D_launch(
    head_out_heatmap_d_.get(), head_out_offset_d_.get(), head_out_z_d_.get(), head_out_dim_d_.get(),
    head_out_rot_d_.get(), head_out_vel_d_.get(), det_boxes3d, stream_));
  if (det_boxes3d.size() == 0) {
    RCLCPP_DEBUG_STREAM(rclcpp::get_logger("lidar_centerpoint"), "No detected boxes.");
  }
}

}  // namespace autoware::lidar_centerpoint

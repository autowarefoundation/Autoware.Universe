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

#include "autoware/lidar_transfusion/transfusion_trt.hpp"

#include "autoware/lidar_transfusion/preprocess/preprocess_kernel.hpp"
#include "autoware/lidar_transfusion/transfusion_config.hpp"

#include <autoware/tensorrt_common/utils.hpp>
#include <autoware/universe_utils/math/constants.hpp>

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

namespace autoware::lidar_transfusion
{

TransfusionTRT::TransfusionTRT(
  const tensorrt_common::TrtCommonConfig & trt_config,
  const DensificationParam & densification_param, TransfusionConfig config)
: config_(std::move(config))
{
  vg_ptr_ = std::make_unique<VoxelGenerator>(densification_param, config_, stream_);
  stop_watch_ptr_ =
    std::make_unique<autoware::universe_utils::StopWatch<std::chrono::milliseconds>>();
  stop_watch_ptr_->tic("processing/inner");
  initPtr();
  initTrt(trt_config);

  CHECK_CUDA_ERROR(cudaStreamCreate(&stream_));
}

TransfusionTRT::~TransfusionTRT()
{
  if (stream_) {
    cudaStreamSynchronize(stream_);
    cudaStreamDestroy(stream_);
  }
}

void TransfusionTRT::initPtr()
{
  // point cloud to voxels
  voxel_features_size_ =
    config_.max_voxels_ * config_.max_num_points_per_pillar_ * config_.num_point_feature_size_;
  voxel_num_size_ = config_.max_voxels_;
  voxel_idxs_size_ = config_.max_voxels_ * config_.num_point_values_;

  // output of TRT -- input of post-process
  cls_size_ = config_.num_proposals_ * config_.num_classes_;
  box_size_ = config_.num_proposals_ * config_.num_box_values_;
  dir_cls_size_ = config_.num_proposals_ * 2;  // x, y
  cls_output_d_ = cuda::make_unique<float[]>(cls_size_);
  box_output_d_ = cuda::make_unique<float[]>(box_size_);
  dir_cls_output_d_ = cuda::make_unique<float[]>(dir_cls_size_);

  params_input_d_ = cuda::make_unique<unsigned int>();
  voxel_features_d_ = cuda::make_unique<float[]>(voxel_features_size_);
  voxel_num_d_ = cuda::make_unique<unsigned int[]>(voxel_num_size_);
  voxel_idxs_d_ = cuda::make_unique<unsigned int[]>(voxel_idxs_size_);
  points_d_ = cuda::make_unique<float[]>(config_.cloud_capacity_ * config_.num_point_feature_size_);
  points_aux_d_ =
    cuda::make_unique<float[]>(config_.cloud_capacity_ * config_.num_point_feature_size_);
  shuffle_indices_d_ = cuda::make_unique<unsigned int[]>(config_.cloud_capacity_);

  std::vector<unsigned int> indexes(config_.cloud_capacity_);
  std::iota(indexes.begin(), indexes.end(), 0);

  std::default_random_engine e(0);
  std::shuffle(indexes.begin(), indexes.end(), e);

  std::srand(std::time(nullptr));

  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    shuffle_indices_d_.get(), indexes.data(), config_.cloud_capacity_ * sizeof(unsigned int),
    cudaMemcpyHostToDevice, stream_));

  pre_ptr_ = std::make_unique<PreprocessCuda>(config_, stream_);
  post_ptr_ = std::make_unique<PostprocessCuda>(config_, stream_);
}

void TransfusionTRT::initTrt(const tensorrt_common::TrtCommonConfig & trt_config)
{
  std::vector<autoware::tensorrt_common::NetworkIO> network_io{
    autoware::tensorrt_common::NetworkIO(
      "voxels", {3, {-1, config_.points_per_voxel_, config_.num_point_feature_size_}}),
    autoware::tensorrt_common::NetworkIO("num_points", {1, {-1}}),
    autoware::tensorrt_common::NetworkIO("coors", {2, {-1, config_.num_point_values_}}),
    autoware::tensorrt_common::NetworkIO(
      "cls_score0", {3, {config_.batch_size_, config_.num_classes_, config_.num_proposals_}}),
    autoware::tensorrt_common::NetworkIO(
      "bbox_pred0", {3, {config_.batch_size_, config_.num_box_values_, config_.num_proposals_}}),
    autoware::tensorrt_common::NetworkIO(
      "dir_cls_pred0", {3, {config_.batch_size_, 2, config_.num_proposals_}})};

  std::vector<autoware::tensorrt_common::ProfileDims> profile_dims{
    autoware::tensorrt_common::ProfileDims(
      "voxels",
      {3,
       {config_.min_voxel_size_, config_.min_point_in_voxel_size_,
        config_.min_network_feature_size_}},
      {3,
       {config_.opt_voxel_size_, config_.opt_point_in_voxel_size_,
        config_.opt_network_feature_size_}},
      {3,
       {config_.max_voxel_size_, config_.max_point_in_voxel_size_,
        config_.max_network_feature_size_}}),
    autoware::tensorrt_common::ProfileDims(
      "num_points", {1, {static_cast<int32_t>(config_.min_points_size_)}},
      {1, {static_cast<int32_t>(config_.opt_points_size_)}},
      {1, {static_cast<int32_t>(config_.max_points_size_)}}),
    autoware::tensorrt_common::ProfileDims(
      "coors", {2, {config_.min_coors_size_, config_.min_coors_dim_size_}},
      {2, {config_.opt_coors_size_, config_.opt_coors_dim_size_}},
      {2, {config_.max_coors_size_, config_.max_coors_dim_size_}})};

  auto network_io_ptr =
    std::make_unique<std::vector<autoware::tensorrt_common::NetworkIO>>(network_io);
  auto profile_dims_ptr =
    std::make_unique<std::vector<autoware::tensorrt_common::ProfileDims>>(profile_dims);

  network_trt_ptr_ = std::make_unique<autoware::tensorrt_common::TrtCommon>(trt_config);
  if (!network_trt_ptr_->setup(std::move(profile_dims_ptr), std::move(network_io_ptr)))
    throw std::runtime_error("Failed to setup TRT engine.");
}

bool TransfusionTRT::detect(
  const sensor_msgs::msg::PointCloud2 & msg, const tf2_ros::Buffer & tf_buffer,
  std::vector<Box3D> & det_boxes3d, std::unordered_map<std::string, double> & proc_timing)
{
  stop_watch_ptr_->toc("processing/inner", true);
  if (!preprocess(msg, tf_buffer)) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_transfusion"), "Fail to preprocess and skip to detect.");
    return false;
  }
  proc_timing.emplace(
    "debug/processing_time/preprocess_ms", stop_watch_ptr_->toc("processing/inner", true));

  if (!inference()) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_transfusion"), "Fail to inference and skip to detect.");
    return false;
  }
  proc_timing.emplace(
    "debug/processing_time/inference_ms", stop_watch_ptr_->toc("processing/inner", true));

  if (!postprocess(det_boxes3d)) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_transfusion"), "Fail to postprocess and skip to detect.");
    return false;
  }
  proc_timing.emplace(
    "debug/processing_time/postprocess_ms", stop_watch_ptr_->toc("processing/inner", true));

  return true;
}

bool TransfusionTRT::preprocess(
  const sensor_msgs::msg::PointCloud2 & msg, const tf2_ros::Buffer & tf_buffer)
{
  if (!vg_ptr_->enqueuePointCloud(msg, tf_buffer)) {
    return false;
  }

  cuda::clear_async(cls_output_d_.get(), cls_size_, stream_);
  cuda::clear_async(box_output_d_.get(), box_size_, stream_);
  cuda::clear_async(dir_cls_output_d_.get(), dir_cls_size_, stream_);
  cuda::clear_async(voxel_features_d_.get(), voxel_features_size_, stream_);
  cuda::clear_async(voxel_num_d_.get(), voxel_num_size_, stream_);
  cuda::clear_async(voxel_idxs_d_.get(), voxel_idxs_size_, stream_);
  cuda::clear_async(params_input_d_.get(), 1, stream_);
  cuda::clear_async(
    points_aux_d_.get(), config_.cloud_capacity_ * config_.num_point_feature_size_, stream_);
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  const auto count = vg_ptr_->generateSweepPoints(msg, points_aux_d_);
  RCLCPP_DEBUG_STREAM(rclcpp::get_logger("lidar_transfusion"), "Generated sweep points: " << count);

  const std::size_t random_offset = std::rand() % config_.cloud_capacity_;
  pre_ptr_->shufflePoints_launch(
    points_aux_d_.get(), shuffle_indices_d_.get(), points_d_.get(), count, config_.cloud_capacity_,
    random_offset);

  pre_ptr_->generateVoxels(
    points_d_.get(), config_.cloud_capacity_, params_input_d_.get(), voxel_features_d_.get(),
    voxel_num_d_.get(), voxel_idxs_d_.get());
  unsigned int params_input;
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    &params_input, params_input_d_.get(), sizeof(unsigned int), cudaMemcpyDeviceToHost, stream_));
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));
  auto params_input_i32 = static_cast<int32_t>(params_input);

  if (params_input_i32 < config_.min_voxel_size_) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_transfusion"),
      "Too few voxels (" << params_input << ") for actual optimization profile ("
                         << config_.min_voxel_size_ << ")");
    return false;
  }

  if (params_input > config_.max_voxels_) {
    rclcpp::Clock clock{RCL_ROS_TIME};
    RCLCPP_WARN_THROTTLE(
      rclcpp::get_logger("lidar_transfusion"), clock, 1000,
      "The actual number of voxels (%u) exceeds its maximum value (%zu). "
      "Please considering increasing it since it may limit the detection performance.",
      params_input, config_.max_voxels_);

    params_input = config_.max_voxels_;
  }

  RCLCPP_DEBUG_STREAM(
    rclcpp::get_logger("lidar_transfusion"), "Generated input voxels: " << params_input);

  bool success = true;

  // Inputs
  success &= network_trt_ptr_->setTensor(
    "voxels", voxel_features_d_.get(),
    nvinfer1::Dims3{
      params_input_i32, config_.max_num_points_per_pillar_,
      static_cast<int32_t>(config_.num_point_feature_size_)});
  success &= network_trt_ptr_->setTensor(
    "num_points", voxel_num_d_.get(), nvinfer1::Dims{1, {params_input_i32}});
  success &= network_trt_ptr_->setTensor(
    "coors", voxel_idxs_d_.get(), nvinfer1::Dims2{params_input_i32, config_.num_point_values_});

  // Outputs
  success &= network_trt_ptr_->setTensor("cls_score0", cls_output_d_.get());
  success &= network_trt_ptr_->setTensor("bbox_pred0", box_output_d_.get());
  success &= network_trt_ptr_->setTensor("dir_cls_pred0", dir_cls_output_d_.get());

  return success;
}

bool TransfusionTRT::inference()
{
  auto status = network_trt_ptr_->enqueueV3(stream_);
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  if (!status) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_transfusion"), "Fail to enqueue and skip to detect.");
    return false;
  }
  return true;
}

bool TransfusionTRT::postprocess(std::vector<Box3D> & det_boxes3d)
{
  CHECK_CUDA_ERROR(post_ptr_->generateDetectedBoxes3D_launch(
    cls_output_d_.get(), box_output_d_.get(), dir_cls_output_d_.get(), det_boxes3d, stream_));
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));
  return true;
}

}  //  namespace autoware::lidar_transfusion

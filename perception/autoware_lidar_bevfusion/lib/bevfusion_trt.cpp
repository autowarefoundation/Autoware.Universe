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

#include "autoware/lidar_bevfusion/bevfusion_trt.hpp"

#include "autoware/lidar_bevfusion/bevfusion_config.hpp"
#include "autoware/lidar_bevfusion/preprocess/point_type.hpp"
#include "autoware/lidar_bevfusion/preprocess/precomputed_features.hpp"
#include "autoware/lidar_bevfusion/preprocess/preprocess_kernel.hpp"

#include <autoware/cuda_utils/cuda_utils.hpp>
#include <autoware/universe_utils/math/constants.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware::lidar_bevfusion
{

BEVFusionTRT::BEVFusionTRT(
  const tensorrt_common::TrtCommonConfig & trt_config,
  const DensificationParam & densification_param, const BEVFusionConfig & config)
: config_(config)
{
  vg_ptr_ = std::make_unique<VoxelGenerator>(densification_param, config_, stream_);

  stop_watch_ptr_ =
    std::make_unique<autoware::universe_utils::StopWatch<std::chrono::milliseconds>>();
  stop_watch_ptr_->tic("processing/inner");

  initPtr();
  initTrt(trt_config);

  CHECK_CUDA_ERROR(cudaStreamCreate(&stream_));

  camera_streams_.resize(config_.num_cameras_);
  for (std::int64_t i = 0; i < config_.num_cameras_; i++) {
    CHECK_CUDA_ERROR(cudaStreamCreate(&camera_streams_[i]));
  }
}

BEVFusionTRT::~BEVFusionTRT()
{
  if (stream_) {
    cudaStreamSynchronize(stream_);
    cudaStreamDestroy(stream_);
  }

  for (std::int64_t i = 0; i < config_.num_cameras_; i++) {
    if (camera_streams_[i]) {
      cudaStreamSynchronize(camera_streams_[i]);
      cudaStreamDestroy(camera_streams_[i]);
    }
  }
}

void BEVFusionTRT::initPtr()
{
  // point cloud to voxels
  voxel_features_size_ =
    config_.max_num_voxels_ * config_.max_points_per_voxel_ * config_.num_point_feature_size_;
  voxel_coords_size_ = 3 * config_.max_num_voxels_;

  // output of TRT -- input of post-process
  bbox_pred_size_ = config_.num_proposals_ * config_.num_box_values_;
  label_pred_output_d_ = autoware::cuda_utils::make_unique<std::int64_t[]>(config_.num_proposals_);
  bbox_pred_output_d_ = autoware::cuda_utils::make_unique<float[]>(bbox_pred_size_);
  score_output_d_ = autoware::cuda_utils::make_unique<float[]>(config_.num_proposals_);

  // lidar branch
  voxel_features_d_ = autoware::cuda_utils::make_unique<float[]>(voxel_features_size_);
  voxel_coords_d_ = autoware::cuda_utils::make_unique<std::int32_t[]>(voxel_coords_size_);
  num_points_per_voxel_d_ =
    autoware::cuda_utils::make_unique<std::int32_t[]>(config_.max_num_voxels_);
  points_d_ = autoware::cuda_utils::make_unique<float[]>(
    config_.cloud_capacity_ * config_.num_point_feature_size_);

  // pre computed tensors
  if (config_.sensor_fusion_) {
    lidar2image_d_ = autoware::cuda_utils::make_unique<float[]>(config_.num_cameras_ * 4 * 4);
    std::int64_t num_geom_feats = config_.num_cameras_ * config_.features_height_ *
                                  config_.features_width_ * config_.num_depth_features_;
    geom_feats_d_ = autoware::cuda_utils::make_unique<std::int32_t[]>(4 * num_geom_feats);
    kept_d_ = autoware::cuda_utils::make_unique<std::uint8_t[]>(num_geom_feats);
    ranks_d_ = autoware::cuda_utils::make_unique<std::int64_t[]>(num_geom_feats);
    indices_d_ = autoware::cuda_utils::make_unique<std::int64_t[]>(num_geom_feats);

    // image branch
    roi_tensor_d_ = autoware::cuda_utils::make_unique<std::uint8_t[]>(
      config_.num_cameras_ * config_.roi_height_ * config_.roi_width_ * 3);
    for (std::int64_t camera_id = 0; camera_id < config_.num_cameras_; camera_id++) {
      image_buffers_d_.emplace_back(autoware::cuda_utils::make_unique<std::uint8_t[]>(
        config_.raw_image_height_ * config_.raw_image_width_ * 3));
    }
    camera_masks_d_ = autoware::cuda_utils::make_unique<float[]>(config_.num_cameras_);
  }

  pre_ptr_ = std::make_unique<PreprocessCuda>(config_, stream_, true);
  post_ptr_ = std::make_unique<PostprocessCuda>(config_, stream_);
}

void BEVFusionTRT::initTrt(const tensorrt_common::TrtCommonConfig & trt_config)
{
  std::vector<autoware::tensorrt_common::NetworkIO> network_io;

  // Lidar branch
  network_io.emplace_back(
    "voxels",
    nvinfer1::Dims{3, {-1, config_.max_points_per_voxel_, config_.num_point_feature_size_}});
  network_io.emplace_back("num_points_per_voxel", nvinfer1::Dims{1, {-1}});
  network_io.emplace_back("coors", nvinfer1::Dims{2, {-1, 3}});

  // Camera branch
  if (config_.sensor_fusion_) {
    network_io.emplace_back("points", nvinfer1::Dims{2, {-1, 5}});
    network_io.emplace_back("camera_mask", nvinfer1::Dims{1, {-1}});
    network_io.emplace_back(
      "imgs", nvinfer1::Dims{4, {-1, 3, config_.roi_height_, config_.roi_width_}});
    network_io.emplace_back("lidar2image", nvinfer1::Dims{3, {-1, 4, 4}});  // 4x4 matrix

    network_io.emplace_back("geom_feats", nvinfer1::Dims{2, {-1, 4}});
    network_io.emplace_back("kept", nvinfer1::Dims{1, {-1}});
    network_io.emplace_back("ranks", nvinfer1::Dims{1, {-1}});
    network_io.emplace_back("indices", nvinfer1::Dims{1, {-1}});
  }

  // Outputs
  network_io.emplace_back(
    "bbox_pred", nvinfer1::Dims{2, {config_.num_box_values_, config_.num_proposals_}});
  network_io.emplace_back("score", nvinfer1::Dims{1, {config_.num_proposals_}});
  network_io.emplace_back("label_pred", nvinfer1::Dims{1, {config_.num_proposals_}});

  std::vector<autoware::tensorrt_common::ProfileDims> profile_dims;

  // Lidar branch
  profile_dims.emplace_back(
    "voxels",
    nvinfer1::Dims{
      3, {config_.voxels_num_[0], config_.max_points_per_voxel_, config_.num_point_feature_size_}},
    nvinfer1::Dims{
      3, {config_.voxels_num_[1], config_.max_points_per_voxel_, config_.num_point_feature_size_}},
    nvinfer1::Dims{
      3, {config_.voxels_num_[2], config_.max_points_per_voxel_, config_.num_point_feature_size_}});

  profile_dims.emplace_back(
    "num_points_per_voxel", nvinfer1::Dims{1, {config_.voxels_num_[0]}},
    nvinfer1::Dims{1, {config_.voxels_num_[1]}}, nvinfer1::Dims{1, {config_.voxels_num_[2]}});

  profile_dims.emplace_back(
    "coors", nvinfer1::Dims{2, {config_.voxels_num_[0], 3}},
    nvinfer1::Dims{2, {config_.voxels_num_[1], 3}}, nvinfer1::Dims{2, {config_.voxels_num_[2], 3}});

  // Camera branch
  if (config_.sensor_fusion_) {
    profile_dims.emplace_back(
      "points", nvinfer1::Dims{2, {config_.voxels_num_[0], 5}},
      nvinfer1::Dims{2, {config_.voxels_num_[1], 5}},
      nvinfer1::Dims{2, {config_.voxels_num_[2], 5}});

    profile_dims.emplace_back(
      "camera_mask", nvinfer1::Dims{1, {1}}, nvinfer1::Dims{1, {config_.num_cameras_}},
      nvinfer1::Dims{1, {config_.num_cameras_}});

    profile_dims.emplace_back(
      "imgs", nvinfer1::Dims{4, {1, 3, config_.roi_height_, config_.roi_width_}},
      nvinfer1::Dims{4, {config_.num_cameras_, 3, config_.roi_height_, config_.roi_width_}},
      nvinfer1::Dims{4, {config_.num_cameras_, 3, config_.roi_height_, config_.roi_width_}});
    profile_dims.emplace_back(
      "lidar2image", nvinfer1::Dims{3, {1, 4, 4}}, nvinfer1::Dims{3, {config_.num_cameras_, 4, 4}},
      nvinfer1::Dims{3, {config_.num_cameras_, 4, 4}});

    const std::int64_t num_geom_feats = config_.num_cameras_ * config_.features_height_ *
                                        config_.features_width_ * config_.num_depth_features_;

    profile_dims.emplace_back(
      "geom_feats", nvinfer1::Dims{2, {0, 4}}, nvinfer1::Dims{2, {num_geom_feats, 4}},
      nvinfer1::Dims{2, {num_geom_feats, 4}});

    profile_dims.emplace_back(
      "kept", nvinfer1::Dims{1, {0}}, nvinfer1::Dims{1, {num_geom_feats}},
      nvinfer1::Dims{1, {num_geom_feats}});

    profile_dims.emplace_back(
      "ranks", nvinfer1::Dims{1, {0}}, nvinfer1::Dims{1, {num_geom_feats}},
      nvinfer1::Dims{1, {num_geom_feats}});

    profile_dims.emplace_back(
      "indices", nvinfer1::Dims{1, {0}}, nvinfer1::Dims{1, {num_geom_feats}},
      nvinfer1::Dims{1, {num_geom_feats}});
  }

  auto network_io_ptr =
    std::make_unique<std::vector<autoware::tensorrt_common::NetworkIO>>(network_io);
  auto profile_dims_ptr =
    std::make_unique<std::vector<autoware::tensorrt_common::ProfileDims>>(profile_dims);

  network_trt_ptr_ = std::make_unique<autoware::tensorrt_common::TrtCommon>(
    trt_config, std::make_shared<autoware::tensorrt_common::Profiler>(),
    std::vector<std::string>{config_.plugins_path_});

  if (!network_trt_ptr_->setup(std::move(profile_dims_ptr), std::move(network_io_ptr))) {
    throw std::runtime_error("Failed to setup TRT engine." + config_.plugins_path_);
  }

  if (config_.sensor_fusion_) {
    /* nvinfer1::Dims input_imgs_shape;
    input_imgs_shape.nbDims = 4;
    input_imgs_shape.d[0] = config_.num_cameras_;
    input_imgs_shape.d[1] = 3;
    input_imgs_shape.d[2] = config_.roi_height_;
    input_imgs_shape.d[3] = config_.roi_width_;

    nvinfer1::Dims input_lidar2image_shape;
    input_lidar2image_shape.nbDims = 3;
    input_lidar2image_shape.d[0] = config_.num_cameras_;
    input_lidar2image_shape.d[1] = 4;
    input_lidar2image_shape.d[2] = 4;

    nvinfer1::Dims input_geom_feats_shape;
    input_geom_feats_shape.nbDims = 2;
    input_geom_feats_shape.d[0] = num_ranks_;
    input_geom_feats_shape.d[1] = 4;

    nvinfer1::Dims input_kept_shape;
    input_kept_shape.nbDims = 1;
    input_kept_shape.d[0] = num_kept_;

    nvinfer1::Dims input_ranks_shape;
    input_ranks_shape.nbDims = 1;
    input_ranks_shape.d[0] = num_ranks_;

    nvinfer1::Dims input_indices_shape;
    input_indices_shape.nbDims = 1;
    input_indices_shape.d[0] = num_indices_; */

    std::vector<float> lidar2image_host(config_.num_cameras_ * 4 * 4);
    cudaMemcpy(
      lidar2image_host.data(), lidar2image_d_.get(), config_.num_cameras_ * 4 * 4 * sizeof(float),
      cudaMemcpyDeviceToHost);

    network_trt_ptr_->setInputShape("camera_mask", nvinfer1::Dims{1, {config_.num_cameras_}});
    network_trt_ptr_->setInputShape(
      "imgs",
      nvinfer1::Dims{4, {config_.num_cameras_, 3, config_.roi_height_, config_.roi_width_}});
    network_trt_ptr_->setInputShape("lidar2image", nvinfer1::Dims{3, {config_.num_cameras_, 4, 4}});
    network_trt_ptr_->setInputShape("geom_feats", nvinfer1::Dims{2, {num_ranks_, 4}});
    network_trt_ptr_->setInputShape("kept", nvinfer1::Dims{1, {num_kept_}});
    network_trt_ptr_->setInputShape("ranks", nvinfer1::Dims{1, {num_ranks_}});
    network_trt_ptr_->setInputShape("indices", nvinfer1::Dims{1, {num_indices_}});
  }

  network_trt_ptr_->setTensorAddress("voxels", voxel_features_d_.get());
  network_trt_ptr_->setTensorAddress("num_points_per_voxel", num_points_per_voxel_d_.get());
  network_trt_ptr_->setTensorAddress("coors", voxel_coords_d_.get());

  if (config_.sensor_fusion_) {
    network_trt_ptr_->setTensorAddress("points", points_d_.get());
    network_trt_ptr_->setTensorAddress("camera_mask", camera_masks_d_.get());
    network_trt_ptr_->setTensorAddress("imgs", roi_tensor_d_.get());
    network_trt_ptr_->setTensorAddress("lidar2image", lidar2image_d_.get());
    network_trt_ptr_->setTensorAddress("geom_feats", geom_feats_d_.get());
    network_trt_ptr_->setTensorAddress("kept", kept_d_.get());
    network_trt_ptr_->setTensorAddress("ranks", ranks_d_.get());
    network_trt_ptr_->setTensorAddress("indices", indices_d_.get());
  }

  network_trt_ptr_->setTensorAddress("label_pred", label_pred_output_d_.get());
  network_trt_ptr_->setTensorAddress("bbox_pred", bbox_pred_output_d_.get());
  network_trt_ptr_->setTensorAddress("score", score_output_d_.get());
}

bool BEVFusionTRT::detect(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr & pc_msg,
  const std::vector<sensor_msgs::msg::Image::ConstSharedPtr> & image_msgs,
  const std::vector<float> & camera_masks, const tf2_ros::Buffer & tf_buffer,
  std::vector<Box3D> & det_boxes3d, std::unordered_map<std::string, double> & proc_timing)
{
  stop_watch_ptr_->toc("processing/inner", true);
  if (!preProcess(pc_msg, image_msgs, camera_masks, tf_buffer)) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_bevfusion"), "Pre-process failed. Skipping detection.");
    return false;
  }
  proc_timing.emplace(
    "debug/processing_time/preprocess_ms", stop_watch_ptr_->toc("processing/inner", true));

  if (!inference()) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_bevfusion"), "Inference failed. Skipping detection.");
    return false;
  }
  proc_timing.emplace(
    "debug/processing_time/inference_ms", stop_watch_ptr_->toc("processing/inner", true));

  if (!postProcess(det_boxes3d)) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_bevfusion"), "Post-process failed. Skipping detection");
    return false;
  }
  proc_timing.emplace(
    "debug/processing_time/postprocess_ms", stop_watch_ptr_->toc("processing/inner", true));

  return true;
}

void BEVFusionTRT::setIntrinsicsExtrinsics(
  std::vector<sensor_msgs::msg::CameraInfo> & camera_info_vector,
  std::vector<Matrix4fRowM> & lidar2camera_vector)
{
  roi_start_y_vector_.clear();
  std::vector<Matrix4fRowM> img_aug_matrices;

  for (std::int64_t i = 0; i < config_.num_cameras_; i++) {
    float fx = camera_info_vector[i].p[0];
    float fy = camera_info_vector[i].p[5];
    float cx = camera_info_vector[i].p[2];
    float cy = camera_info_vector[i].p[6];

    Matrix4fRowM camera2lidar_matrix = lidar2camera_vector[i].inverse();
    float r31 = camera2lidar_matrix(2, 0);
    float r32 = camera2lidar_matrix(2, 1);
    float r33 = camera2lidar_matrix(2, 2);

    float yl = cy + cx * (fy / fx) * (r31 / r32) - fy * (r33 / r32);
    float yr =
      cy + (cx - config_.raw_image_width_ + 1) * (fy / fx) * (r31 / r32) - fy * (r33 / r32);
    float yh = std::max(0.0f, std::min(yr, yl));
    float yh_resized = yh * config_.img_aug_scale_y_;
    int crop_h = static_cast<int>(
      std::min(yh_resized, static_cast<float>(config_.resized_height_ - config_.roi_height_)));

    Matrix4fRowM img_aug_matrix = Matrix4fRowM::Identity();
    img_aug_matrix(0, 0) = config_.img_aug_scale_x_;
    img_aug_matrix(1, 1) = config_.img_aug_scale_y_;
    img_aug_matrix(1, 3) = -static_cast<float>(crop_h);

    img_aug_matrices.push_back(img_aug_matrix);
    roi_start_y_vector_.push_back(crop_h);
  }

  auto [lidar2images_flattened, geom_feats, kept, ranks, indices] =
    precompute_features(lidar2camera_vector, img_aug_matrices, camera_info_vector, config_);

  assert(static_cast<std::int64_t>(lidar2images_flattened.size()) == config_.num_cameras_ * 4 * 4);

  assert(
    static_cast<std::int64_t>(geom_feats.size()) <=
    config_.num_cameras_ * 4 * config_.features_height_ * config_.features_width_ *
      config_.num_depth_features_);
  assert(
    static_cast<std::int64_t>(kept.size()) == config_.num_cameras_ * config_.features_height_ *
                                                config_.features_width_ *
                                                config_.num_depth_features_);
  assert(
    static_cast<std::int64_t>(ranks.size()) <= config_.num_cameras_ * config_.features_height_ *
                                                 config_.features_width_ *
                                                 config_.num_depth_features_);
  assert(
    static_cast<std::int64_t>(indices.size()) <= config_.num_cameras_ * config_.features_height_ *
                                                   config_.features_width_ *
                                                   config_.num_depth_features_);

  num_geom_feats_ = static_cast<std::int64_t>(geom_feats.size());
  num_kept_ = static_cast<std::int64_t>(kept.size());
  num_ranks_ = static_cast<std::int64_t>(ranks.size());
  num_indices_ = static_cast<std::int64_t>(indices.size());

  assert(num_geom_feats_ == 4 * num_ranks_);
  assert(num_ranks_ == num_indices_);

  cudaMemcpy(
    lidar2image_d_.get(), lidar2images_flattened.data(),
    config_.num_cameras_ * 4 * 4 * sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(
    geom_feats_d_.get(), geom_feats.data(), num_geom_feats_ * sizeof(std::int32_t),
    cudaMemcpyHostToDevice);
  cudaMemcpy(kept_d_.get(), kept.data(), num_kept_ * sizeof(std::uint8_t), cudaMemcpyHostToDevice);
  cudaMemcpy(
    ranks_d_.get(), ranks.data(), num_ranks_ * sizeof(std::int64_t), cudaMemcpyHostToDevice);
  cudaMemcpy(
    indices_d_.get(), indices.data(), num_indices_ * sizeof(std::int64_t), cudaMemcpyHostToDevice);
}

bool BEVFusionTRT::preProcess(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr & pc_msg,
  const std::vector<sensor_msgs::msg::Image::ConstSharedPtr> & image_msgs,
  const std::vector<float> & camera_masks, const tf2_ros::Buffer & tf_buffer)
{
  using autoware::cuda_utils::clear_async;

  if (!is_data_layout_compatible_with_point_xyzirc(*pc_msg)) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_bevfusion"), "Invalid point type. Skipping detection.");
    return false;
  }

  if (!vg_ptr_->enqueuePointCloud(*pc_msg, tf_buffer)) {
    return false;
  }

  // TODO(knzo25): these should be able to be removed as they are filled by TensorRT
  clear_async(label_pred_output_d_.get(), config_.num_proposals_, stream_);
  clear_async(bbox_pred_output_d_.get(), bbox_pred_size_, stream_);
  clear_async(score_output_d_.get(), config_.num_proposals_, stream_);

  clear_async(voxel_features_d_.get(), voxel_features_size_, stream_);
  clear_async(voxel_coords_d_.get(), voxel_coords_size_, stream_);
  clear_async(num_points_per_voxel_d_.get(), config_.max_num_voxels_, stream_);
  clear_async(points_d_.get(), config_.cloud_capacity_ * config_.num_point_feature_size_, stream_);
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  // TODO(knzo25): move this to each image callback
  if (config_.sensor_fusion_) {
    int start_x =
      std::max(0, static_cast<int>(config_.resized_width_) - static_cast<int>(config_.roi_width_)) /
      2;

    for (std::int64_t camera_id = 0; camera_id < config_.num_cameras_; camera_id++) {
      int start_y = roi_start_y_vector_[camera_id];
      cudaMemcpyAsync(
        image_buffers_d_[camera_id].get(), image_msgs[camera_id]->data.data(),
        config_.raw_image_height_ * config_.raw_image_width_ * 3, cudaMemcpyHostToDevice, stream_);

      pre_ptr_->resize_and_extract_roi_launch(
        image_buffers_d_[camera_id].get(),
        &roi_tensor_d_[camera_id * config_.roi_height_ * config_.roi_width_ * 3],
        config_.raw_image_height_, config_.raw_image_width_, config_.resized_height_,
        config_.resized_width_, config_.roi_height_, config_.roi_width_, start_y, start_x,
        camera_streams_[camera_id]);
    }

    cudaMemcpyAsync(
      camera_masks_d_.get(), camera_masks.data(), config_.num_cameras_ * sizeof(float),
      cudaMemcpyHostToDevice, stream_);
  }

  const auto num_points = vg_ptr_->generateSweepPoints(points_d_);

  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  auto num_voxels = static_cast<std::int64_t>(pre_ptr_->generateVoxels(
    points_d_.get(), num_points, voxel_features_d_.get(), voxel_coords_d_.get(),
    num_points_per_voxel_d_.get()));

  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  if (num_voxels < config_.min_num_voxels_) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_bevfusion"),
      "Too few voxels (" << num_voxels << ") for the actual optimization profile ("
                         << config_.min_num_voxels_ << ")");
    return false;
  }
  if (num_voxels > config_.max_num_voxels_) {
    RCLCPP_ERROR_STREAM(
      rclcpp::get_logger("lidar_bevfusion"),
      "Actual number of voxels (" << num_voxels
                                  << ") is over the limit for the actual optimization profile ("
                                  << config_.max_num_voxels_ << "). Clipping to the limit.");
    num_voxels = config_.max_num_voxels_;
  }

  network_trt_ptr_->setInputShape(
    "voxels", nvinfer1::Dims{
                3, {num_voxels, config_.max_points_per_voxel_, config_.num_point_feature_size_}});
  network_trt_ptr_->setInputShape("num_points_per_voxel", nvinfer1::Dims{1, {num_voxels}});
  network_trt_ptr_->setInputShape("coors", nvinfer1::Dims{2, {num_voxels, 3}});

  if (config_.sensor_fusion_) {
    network_trt_ptr_->setInputShape(
      "points", nvinfer1::Dims{2, {static_cast<std::int64_t>(num_points), 5}});
    network_trt_ptr_->setInputShape("geom_feats", nvinfer1::Dims{2, {num_ranks_, 4}});
    network_trt_ptr_->setInputShape("kept", nvinfer1::Dims{1, {num_kept_}});
    network_trt_ptr_->setInputShape("ranks", nvinfer1::Dims{1, {num_ranks_}});
    network_trt_ptr_->setInputShape("indices", nvinfer1::Dims{1, {num_indices_}});
  }

  for (std::int64_t i = 0; i < config_.num_cameras_; i++) {
    cudaStreamSynchronize(camera_streams_[i]);
  }

  return true;
}

bool BEVFusionTRT::inference()
{
  auto status = network_trt_ptr_->enqueueV3(stream_);
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  if (!status) {
    RCLCPP_WARN_STREAM(
      rclcpp::get_logger("lidar_bevfusion"), "Fail to enqueue and skip to detect.");
    return false;
  }

  return true;
}

bool BEVFusionTRT::postProcess(std::vector<Box3D> & det_boxes3d)
{
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  CHECK_CUDA_ERROR(post_ptr_->generateDetectedBoxes3D_launch(
    label_pred_output_d_.get(), bbox_pred_output_d_.get(), score_output_d_.get(), det_boxes3d,
    stream_));
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));
  return true;
}

}  //  namespace autoware::lidar_bevfusion

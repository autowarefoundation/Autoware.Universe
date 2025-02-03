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
/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES.
 * All rights reserved. SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "autoware/lidar_bevfusion/preprocess/point_type.hpp"
#include "autoware/lidar_bevfusion/preprocess/preprocess_kernel.hpp"

#include <autoware/cuda_utils/cuda_check_error.hpp>

#include <spconvlib/spconv/csrc/sparse/all/ops3d/Point2Voxel.h>

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace autoware::lidar_bevfusion
{

PreprocessCuda::PreprocessCuda(
  const BEVFusionConfig & config, cudaStream_t & stream, bool allocate_buffers)
: stream_(stream), config_(config)
{
  if (allocate_buffers) {
    hash_key_value_ = tv::empty({config.cloud_capacity_ * 2}, tv::custom128, 0);
    point_indice_data_ = tv::empty({config.cloud_capacity_}, tv::int64, 0);
    points_voxel_id_ = tv::empty({config.cloud_capacity_}, tv::int64, 0);
  }
}

__global__ void generateSweepPoints_kernel(
  const InputPointType * __restrict__ input_points, std::size_t points_size, float time_lag,
  const float * transform_array, int num_features, float * __restrict__ output_points)
{
  int point_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (point_idx >= points_size) return;

  const InputPointType * input_point = &input_points[point_idx];
  float input_x = input_point->x;
  float input_y = input_point->y;
  float input_z = input_point->z;
  float input_intensity = static_cast<float>(input_point->intensity);

  output_points[point_idx * num_features] = transform_array[0] * input_x +
                                            transform_array[4] * input_y +
                                            transform_array[8] * input_z + transform_array[12];
  output_points[point_idx * num_features + 1] = transform_array[1] * input_x +
                                                transform_array[5] * input_y +
                                                transform_array[9] * input_z + transform_array[13];
  output_points[point_idx * num_features + 2] = transform_array[2] * input_x +
                                                transform_array[6] * input_y +
                                                transform_array[10] * input_z + transform_array[14];
  output_points[point_idx * num_features + 3] = input_intensity;
  output_points[point_idx * num_features + 4] = time_lag;
}

cudaError_t PreprocessCuda::generateSweepPoints_launch(
  const InputPointType * input_data, std::size_t points_size, float time_lag,
  const float * transform_array, float * output_points)
{
  dim3 blocks(divup(points_size, config_.threads_per_block_));
  dim3 threads(config_.threads_per_block_);

  generateSweepPoints_kernel<<<blocks, threads, 0, stream_>>>(
    input_data, points_size, time_lag, transform_array, config_.num_point_feature_size_,
    output_points);

  cudaError_t err = cudaGetLastError();
  return err;
}

std::size_t PreprocessCuda::generateVoxels(
  const float * points, unsigned int points_size, float * voxel_features,
  std::int32_t * voxel_coords, std::int32_t * num_points_per_voxel)
{
  using Point2VoxelGPU3D = spconvlib::spconv::csrc::sparse::all::ops3d::Point2Voxel;

  std::array<float, 3> vsize_xyz{
    config_.voxel_z_size_, config_.voxel_y_size_, config_.voxel_x_size_};

  std::array<std::int32_t, 3> grid_size{
    static_cast<std::int32_t>(config_.grid_z_size_),
    static_cast<std::int32_t>(config_.grid_y_size_),
    static_cast<std::int32_t>(config_.grid_x_size_)};

  std::array<std::int64_t, 3> grid_stride{
    static_cast<std::int64_t>(config_.grid_y_size_ * config_.grid_x_size_),
    static_cast<std::int64_t>(config_.grid_x_size_), 1};

  std::array<float, 6> coors_range{config_.min_z_range_, config_.min_y_range_,
                                   config_.min_x_range_, config_.max_z_range_,
                                   config_.max_y_range_, config_.max_x_range_};

  tv::Tensor pc = tv::from_blob(
    points, {static_cast<std::int64_t>(points_size), config_.num_point_feature_size_}, tv::float32,
    0);

  auto point_limit = pc.dim(0);

  tv::Tensor voxels_padded = tv::from_blob(
    voxel_features, {config_.max_num_voxels_, config_.max_points_per_voxel_, pc.dim(1)},
    tv::float32, 0);

  tv::Tensor indices_padded_no_batch =
    tv::from_blob(voxel_coords, {config_.max_num_voxels_, 3}, tv::int32, 0);

  tv::Tensor num_points_per_voxel_tensor =
    tv::from_blob(num_points_per_voxel, {config_.max_num_voxels_}, tv::int32, 0);

  auto p2v_res = Point2VoxelGPU3D::point_to_voxel_hash_static(
    pc, voxels_padded, indices_padded_no_batch, num_points_per_voxel_tensor, hash_key_value_,
    point_indice_data_, points_voxel_id_, vsize_xyz, grid_size, grid_stride, coors_range, true,
    false, reinterpret_cast<std::uintptr_t>(stream_));

  std::size_t real_num_voxels = static_cast<std::size_t>(std::get<0>(p2v_res).dim(0));

  return real_num_voxels;
}

__global__ void resize_and_extract_roi(
  const std::uint8_t * __restrict__ input_img, std::uint8_t * __restrict__ output_img, int H,
  int W,                     // Original image dimensions (Height, Width)
  int H2, int W2,            // Resized image dimensions (Height, Width)
  int H3, int W3,            // ROI dimensions (Height, Width)
  int y_start, int x_start)  // ROI top-left coordinates in resized image
{
  // Calculate the global thread indices
  int i = blockIdx.x * blockDim.x + threadIdx.x;  // Width index in output (ROI) image
  int j = blockIdx.y * blockDim.y + threadIdx.y;  // Height index in output (ROI) image

  // Check if the thread corresponds to a valid pixel in the ROI
  if (i >= W3 || j >= H3) return;

  // Compute scaling factors from original to resized image
  float scale_y = static_cast<float>(H) / H2;
  float scale_x = static_cast<float>(W) / W2;

  // Map output pixel (i, j) in ROI to position in resized image
  int i_resized = i + x_start;
  int j_resized = j + y_start;

  // Map position in resized image back to position in original image
  float i_original = (i_resized + 0.5f) * scale_x - 0.5f;
  float j_original = (j_resized + 0.5f) * scale_y - 0.5f;

  // Compute coordinates for bilinear interpolation
  int i0 = static_cast<int>(floorf(i_original));
  int j0 = static_cast<int>(floorf(j_original));
  int i1 = i0 + 1;
  int j1 = j0 + 1;

  // Compute interpolation weights
  float di = i_original - i0;
  float dj = j_original - j0;

  float w00 = (1.0f - di) * (1.0f - dj);
  float w01 = (1.0f - di) * dj;
  float w10 = di * (1.0f - dj);
  float w11 = di * dj;

  // Loop over the three color channels
  for (int c = 0; c < 3; ++c) {
    float v00 = 0.0f, v01 = 0.0f, v10 = 0.0f, v11 = 0.0f;

    // Boundary checks for each neighboring pixel
    if (i0 >= 0 && i0 < W && j0 >= 0 && j0 < H)
      v00 = static_cast<float>(input_img[(j0 * W + i0) * 3 + c]);
    if (i0 >= 0 && i0 < W && j1 >= 0 && j1 < H)
      v01 = static_cast<float>(input_img[(j1 * W + i0) * 3 + c]);
    if (i1 >= 0 && i1 < W && j0 >= 0 && j0 < H)
      v10 = static_cast<float>(input_img[(j0 * W + i1) * 3 + c]);
    if (i1 >= 0 && i1 < W && j1 >= 0 && j1 < H)
      v11 = static_cast<float>(input_img[(j1 * W + i1) * 3 + c]);

    // Compute the interpolated pixel value
    float value = w00 * v00 + w01 * v01 + w10 * v10 + w11 * v11;

    // Store the result in the output ROI image
    output_img[(j * W3 + i) * 3 + c] = static_cast<std::uint8_t>(value);
  }
}

cudaError_t PreprocessCuda::resize_and_extract_roi_launch(
  const std::uint8_t * input_img, std::uint8_t * output_img, int H,
  int W,                     // Original image dimensions
  int H2, int W2,            // Resized image dimensions
  int H3, int W3,            // ROI dimensions
  int y_start, int x_start,  // ROI top-left coordinates in resized image
  cudaStream_t stream)
{
  // Define the block and grid dimensions
  dim3 threads(16, 16);
  dim3 blocks(divup(W3, threads.x), divup(H3, threads.y));

  // Launch the kernel
  resize_and_extract_roi<<<blocks, threads, 0, stream>>>(
    input_img, output_img, H, W, H2, W2, H3, W3, y_start, x_start);

  // Check for errors
  return cudaGetLastError();
}

}  // namespace autoware::lidar_bevfusion

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

#include "autoware/lidar_bevfusion/postprocess/circle_nms_kernel.hpp"
#include "autoware/lidar_bevfusion/postprocess/postprocess_kernel.hpp"

#include <thrust/count.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/sort.h>

#include <cstddef>

namespace autoware::lidar_bevfusion
{
struct is_score_greater
{
  is_score_greater(float t) : t_(t) {}

  __device__ bool operator()(const Box3D & b) { return b.score > t_; }

private:
  float t_{0.0};
};

struct is_kept
{
  __device__ bool operator()(const bool keep) { return keep; }
};

struct score_greater
{
  __device__ bool operator()(const Box3D & lb, const Box3D & rb) { return lb.score > rb.score; }
};

__device__ inline float sigmoid(float x)
{
  return 1.0f / (1.0f + expf(-x));
}

__global__ void generateBoxes3D_kernel(
  const std::int64_t * __restrict__ label_pred_output, const float * __restrict__ bbox_pred_output,
  const float * __restrict__ score_output, const float voxel_size_x, const float voxel_size_y,
  const float min_x_range, const float min_y_range, const int num_proposals,
  const float out_size_factor, const float * __restrict__ yaw_norm_thresholds,
  Box3D * __restrict__ det_boxes3d)
{
  int point_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (point_idx >= num_proposals) {
    return;
  }

  const float yaw_sin = bbox_pred_output[6 * num_proposals + point_idx];
  const float yaw_cos = bbox_pred_output[7 * num_proposals + point_idx];
  const float yaw_norm = sqrtf(yaw_sin * yaw_sin + yaw_cos * yaw_cos);
  const int label = static_cast<int>(label_pred_output[point_idx]);

  det_boxes3d[point_idx].label = label;
  det_boxes3d[point_idx].score =
    yaw_norm >= yaw_norm_thresholds[label] ? score_output[point_idx] : 0.f;

  det_boxes3d[point_idx].x =
    bbox_pred_output[0 * num_proposals + point_idx] * out_size_factor * voxel_size_x + min_x_range;
  det_boxes3d[point_idx].y =
    bbox_pred_output[1 * num_proposals + point_idx] * out_size_factor * voxel_size_y + min_y_range;
  det_boxes3d[point_idx].z = bbox_pred_output[2 * num_proposals + point_idx];
  det_boxes3d[point_idx].length = expf(bbox_pred_output[3 * num_proposals + point_idx]);
  det_boxes3d[point_idx].width = expf(bbox_pred_output[4 * num_proposals + point_idx]);
  det_boxes3d[point_idx].height = expf(bbox_pred_output[5 * num_proposals + point_idx]);
  det_boxes3d[point_idx].yaw = atan2f(yaw_sin, yaw_cos);
  det_boxes3d[point_idx].vx = bbox_pred_output[8 * num_proposals + point_idx];
  det_boxes3d[point_idx].vy = bbox_pred_output[9 * num_proposals + point_idx];
}

PostprocessCuda::PostprocessCuda(const BEVFusionConfig & config, cudaStream_t & stream)
: config_(config), stream_(stream)
{
}

// cspell: ignore divup
cudaError_t PostprocessCuda::generateDetectedBoxes3D_launch(
  const std::int64_t * label_pred_output, const float * bbox_pred_output,
  const float * score_output, std::vector<Box3D> & det_boxes3d, cudaStream_t stream)
{
  dim3 threads = {config_.threads_per_block_};
  dim3 blocks = {divup(config_.num_proposals_, threads.x)};

  auto boxes3d_d = thrust::device_vector<Box3D>(config_.num_proposals_);
  auto yaw_norm_thresholds_d = thrust::device_vector<float>(
    config_.yaw_norm_thresholds_.begin(), config_.yaw_norm_thresholds_.end());

  generateBoxes3D_kernel<<<blocks, threads, 0, stream>>>(
    label_pred_output, bbox_pred_output, score_output, config_.voxel_x_size_, config_.voxel_y_size_,
    config_.min_x_range_, config_.min_y_range_, config_.num_proposals_, config_.out_size_factor_,
    thrust::raw_pointer_cast(yaw_norm_thresholds_d.data()),
    thrust::raw_pointer_cast(boxes3d_d.data()));

  // suppress by score
  const auto num_det_boxes3d = thrust::count_if(
    thrust::device, boxes3d_d.begin(), boxes3d_d.end(), is_score_greater(config_.score_threshold_));

  if (num_det_boxes3d == 0) {
    return cudaGetLastError();
  }

  thrust::device_vector<Box3D> det_boxes3d_d(num_det_boxes3d);
  thrust::copy_if(
    thrust::device, boxes3d_d.begin(), boxes3d_d.end(), det_boxes3d_d.begin(),
    is_score_greater(config_.score_threshold_));

  // sort by score
  thrust::sort(det_boxes3d_d.begin(), det_boxes3d_d.end(), score_greater());

  // supress by NMS
  thrust::device_vector<bool> final_keep_mask_d(num_det_boxes3d);
  const auto num_final_det_boxes3d =
    circleNMS(det_boxes3d_d, config_.circle_nms_dist_threshold_, final_keep_mask_d, stream);
  thrust::device_vector<Box3D> final_det_boxes3d_d(num_final_det_boxes3d);
  thrust::copy_if(
    thrust::device, det_boxes3d_d.begin(), det_boxes3d_d.end(), final_keep_mask_d.begin(),
    final_det_boxes3d_d.begin(), is_kept());

  // memcpy device to host
  det_boxes3d.resize(num_final_det_boxes3d);
  thrust::copy(final_det_boxes3d_d.begin(), final_det_boxes3d_d.end(), det_boxes3d.begin());

  return cudaGetLastError();
}

}  // namespace autoware::lidar_bevfusion

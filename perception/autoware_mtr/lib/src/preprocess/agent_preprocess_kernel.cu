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

#include "preprocess/agent_preprocess_kernel.cuh"

#include <iostream>

__global__ void agentPreprocessKernel(
  const int B, const int N, const int T, const int D, const int C, const int sdc_index,
  const int * target_index, const int * object_type_index, const float * timestamps,
  const float * in_trajectory, float * out_data, bool * out_mask, float * out_last_pos)
{
  int b = blockIdx.x * blockDim.x + threadIdx.x;
  int n = blockIdx.y * blockDim.y + threadIdx.y;
  int t = blockIdx.z * blockDim.z + threadIdx.z;

  if (b >= B || n >= N || t >= T) {
    return;
  }

  const int out_idx = (b * N * T + n * T + t) * (D - 2 + C + 2 + T + 1 + 2);

  // === out data ===
  // --- transform trajectory to target centric coords ---
  const int src_trajectory_idx = (n * T + t) * D;
  const float x = in_trajectory[src_trajectory_idx];
  const float y = in_trajectory[src_trajectory_idx + 1];
  const float z = in_trajectory[src_trajectory_idx + 2];
  const float length = in_trajectory[src_trajectory_idx + 3];
  const float width = in_trajectory[src_trajectory_idx + 4];
  const float height = in_trajectory[src_trajectory_idx + 5];
  const float yaw = in_trajectory[src_trajectory_idx + 6];
  const float vx = in_trajectory[src_trajectory_idx + 7];
  const float vy = in_trajectory[src_trajectory_idx + 8];
  const float ax = in_trajectory[src_trajectory_idx + 9];
  const float ay = in_trajectory[src_trajectory_idx + 10];
  const float is_valid = in_trajectory[src_trajectory_idx + 11];

  // extract targets trajectories
  const int center_idx = (target_index[b] * T + T - 1) * D;
  const float center_x = in_trajectory[center_idx];
  const float center_y = in_trajectory[center_idx + 1];
  const float center_z = in_trajectory[center_idx + 2];
  const float center_yaw = in_trajectory[center_idx + 6];
  const float center_cos = cos(center_yaw);
  const float center_sin = sin(center_yaw);

  // do transform
  const float trans_x = center_cos * (x - center_x) - center_sin * (y - center_y);
  const float trans_y = center_sin * (x - center_x) + center_cos * (y - center_y);
  const float trans_z = z - center_z;
  const float trans_yaw = yaw - center_yaw;
  const float trans_vx = center_cos * vx - center_sin * vy;
  const float trans_vy = center_sin * vx + center_cos * vy;
  const float trans_ax = center_cos * ax - center_sin * ay;
  const float trans_ay = center_sin * ax + center_cos * ay;

  out_data[out_idx] = trans_x;
  out_data[out_idx + 1] = trans_y;
  out_data[out_idx + 2] = trans_z;
  out_data[out_idx + 3] = length;
  out_data[out_idx + 4] = width;
  out_data[out_idx + 5] = height;

  // --- onehot ---
  const int onehot_idx = out_idx + 6;
  out_data[onehot_idx + object_type_index[n]] = 1.0f;

  if (target_index[b] == n) {
    out_data[onehot_idx + C] = 1.0f;
  }

  if (sdc_index == n) {
    out_data[onehot_idx + C + 1] = 1.0f;
  }

  // --- embedding ---
  const int embed_idx = onehot_idx + C + 2;
  // time embedding
  out_data[embed_idx + t] = 1.0f;
  out_data[embed_idx + T] = timestamps[t];
  // heading embedding
  out_data[embed_idx + T + 1] = sin(trans_yaw);
  out_data[embed_idx + T + 2] = cos(trans_yaw);

  const int other_idx = embed_idx + T + 3;
  out_data[other_idx] = trans_vx;
  out_data[other_idx + 1] = trans_vy;
  out_data[other_idx + 2] = trans_ax;
  out_data[other_idx + 3] = trans_ay;

  // === mask ===
  const int mask_idx = b * N * T + n * T + t;
  out_mask[mask_idx] = is_valid == 1.0f;

  // === last pos ===
  if (t == T - 1) {
    const int pos_idx = (b * N + n) * 3;
    out_last_pos[pos_idx] = trans_x;
    out_last_pos[pos_idx + 1] = trans_y;
    out_last_pos[pos_idx + 2] = trans_z;
  }
}

cudaError_t agentPreprocessLauncher(
  const int B, const int N, const int T, const int D, const int C, const int sdc_index,
  const int * target_index, const int * object_type_index, const float * timestamps,
  const float * in_trajectory, float * out_data, bool * out_mask, float * out_last_pos,
  cudaStream_t stream)
{
  if (D != 12) {
    std::cerr << "D must be 12, but got " << D << std::endl;
    return cudaError::cudaErrorInvalidValue;
  }

  // TODO: update the number of blocks and threads to guard from `cudaErrorIllegalAccess`
  constexpr int threadsPerBlock = 256;
  dim3 blocks(B, N, T);
  agentPreprocessKernel<<<blocks, threadsPerBlock, 0, stream>>>(
    B, N, T, D, C, sdc_index, target_index, object_type_index, timestamps, in_trajectory, out_data,
    out_mask, out_last_pos);

  return cudaGetLastError();
}

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

#include "common/trt_plugin_helper.hpp"
#include "knn/trt_knn_batch_kernel.hpp"

__global__ void knn_batch_kernel(
  const int32_t n, const int32_t m, const int32_t k, const float * xyz, const float * query_xyz,
  const int * batch_idxs, const int * query_batch_offsets, int * output)
{
  const int pt_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (pt_idx >= n) {
    return;
  }

  xyz += pt_idx * 3;
  output += pt_idx * k;

  float ox = xyz[0];
  float oy = xyz[1];
  float oz = xyz[2];

  float best[100];
  int best_idx[100];
  for (int i = 0; i < k; ++i) {
    best[i] = 1e20;
    best_idx[i] = -1;
  }

  int batch_idx = batch_idxs[pt_idx];
  if (batch_idx < 0) {
    return;
  }

  int start_idx = query_batch_offsets[batch_idx];
  int end_idx = query_batch_offsets[batch_idx + 1];
  for (int i = start_idx; i < end_idx; ++i) {
    float x = query_xyz[i * 3 + 0];
    float y = query_xyz[i * 3 + 1];
    float z = query_xyz[i * 3 + 2];
    float d2 = (ox - x) * (ox - x) + (oy - y) * (oy - y) + (oz - z) * (oz - z);
    for (int32_t p = 0; p < k; ++p) {
      if (d2 < best[p]) {
        for (int32_t q = k - 1; q > p; --q) {
          best[q] = best[q - 1];
          best_idx[q] = best_idx[q - 1];
        }
        best[p] = d2;
        best_idx[p] = i - start_idx;
        break;
      }
    }
  }

  for (int i = 0; i < k; ++i) {
    output[i] = best_idx[i];
  }
}

cudaError_t KnnBatchLauncher(
  const int32_t n, const int32_t m, const int32_t k, const float * xyz, const float * query_xyz,
  const int * batch_idxs, const int * query_batch_offsets, int * output, cudaStream_t stream)
{
  dim3 blocks(DIVUP(n, THREADS_PER_BLOCK));
  dim3 threads(THREADS_PER_BLOCK);

  knn_batch_kernel<<<blocks, threads, 0, stream>>>(
    n, m, k, xyz, query_xyz, batch_idxs, query_batch_offsets, output);

  return cudaGetLastError();
}

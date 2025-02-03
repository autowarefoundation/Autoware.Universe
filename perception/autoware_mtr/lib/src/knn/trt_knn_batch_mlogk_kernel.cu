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
#include "knn/trt_knn_batch_mlogk_kernel.hpp"

__global__ void knn_batch_mlogk_kernel(
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

  float best[150];
  int best_idx[150];

  int32_t heap_len = 0;

  for (int i = 0; i <= k; ++i) {
    best[i] = 1e20;
    best_idx[i] = -1;
  }

  int batch_idx = batch_idxs[pt_idx];
  if (batch_idx < 0) {
    return;
  }

  int start_idx = query_batch_offsets[batch_idx];
  int end_idx = query_batch_offsets[batch_idx + 1];

  int tmp_idx;
  float tmp_val;
  for (int i = start_idx; i < end_idx; ++i) {
    float x = query_xyz[i * 3 + 0];
    float y = query_xyz[i * 3 + 1];
    float z = query_xyz[i * 3 + 2];
    float d2 = (ox - x) * (ox - x) + (oy - y) * (oy - y) + (oz - z) * (oz - z);

    if (heap_len < k) {
      ++heap_len;
      best[heap_len] = d2;
      best_idx[heap_len] = i - start_idx;
      int cur_idx = heap_len, fa_idx = cur_idx >> 1;
      while (fa_idx > 0) {
        if (best[cur_idx] < best[fa_idx]) {
          break;
        }
        tmp_idx = best_idx[cur_idx];
        best_idx[cur_idx] = best_idx[fa_idx];
        best_idx[fa_idx] = tmp_idx;
        tmp_val = best[cur_idx];
        best[cur_idx] = best[fa_idx];
        best[fa_idx] = tmp_val;
        cur_idx = fa_idx;
        fa_idx = cur_idx >> 1;
      }
    } else {
      if (d2 > best[1]) {
        continue;
      }
      best[1] = d2;
      best_idx[1] = i - start_idx;

      int32_t cur_idx = 1, son_idx;
      while (cur_idx <= k) {
        son_idx = cur_idx << 1;
        if (son_idx > k) {
          break;
        }
        if (son_idx + 1 <= k && best[son_idx] < best[son_idx + 1]) {
          ++son_idx;
        }

        if (son_idx <= k && best[cur_idx] < best[son_idx]) {
          tmp_idx = best_idx[cur_idx];
          best_idx[cur_idx] = best_idx[son_idx];
          best_idx[son_idx] = tmp_idx;
          tmp_val = best[cur_idx];
          best[cur_idx] = best[son_idx];
          best[son_idx] = tmp_val;
        } else {
          break;
        }
        cur_idx = son_idx;
      }
    }
  }

  for (int i = 1; i <= k; ++i) {
    output[i - 1] = best_idx[i];
  }
}

cudaError_t KnnBatchMlogKLauncher(
  const int32_t n, const int32_t m, const int32_t k, const float * xyz, const float * query_xyz,
  const int * batch_idxs, const int * query_batch_offsets, int * output, cudaStream_t stream)
{
  dim3 blocks(DIVUP(n, THREADS_PER_BLOCK));
  dim3 threads(THREADS_PER_BLOCK);

  knn_batch_mlogk_kernel<<<blocks, threads, 0, stream>>>(
    n, m, k, xyz, query_xyz, batch_idxs, query_batch_offsets, output);

  return cudaGetLastError();
}

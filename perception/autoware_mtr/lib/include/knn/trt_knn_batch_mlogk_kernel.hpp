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

#ifndef KNN__TRT_KNN_BATCH_MLOGK_KERNEL_HPP_
#define KNN__TRT_KNN_BATCH_MLOGK_KERNEL_HPP_

#include <cuda_runtime.h>

cudaError_t KnnBatchMlogKLauncher(
  const int32_t n, const int32_t m, const int32_t k, const float * xyz, const float * query_xyz,
  const int * batch_idx, const int * query_batch_offsets, int * output, cudaStream_t stream);

#endif  // KNN__TRT_KNN_BATCH_MLOGK_KERNEL_HPP_

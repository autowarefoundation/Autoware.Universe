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

#ifndef ATTENTION__TRT_ATTN_VALUE_COMPUTATION_KERNEL_HPP_
#define ATTENTION__TRT_ATTN_VALUE_COMPUTATION_KERNEL_HPP_

#include <cuda_runtime.h>

#include <cstdint>

/**
 * @brief The launcher to invoke attention value computation kernel.
 *
 * @param B The size of batch.
 * @param Q The size of query.
 * @param L The size of local.
 * @param K The size of key.
 * @param numHead The number of heads.
 * @param headDim The number of head dimensions.
 * @param queryBatchCnt The number of queries for each batch, in shape [B].
 * @param keyBatchCnt The number of keys for each batch, in shape [B].
 * @param indexPairBatch The indices of batch for corresponding query, in shape [Q].
 * @param indexPair The indices of key for corresponding query, in shape [Q*L].
 * @param attnWeight Source attention weights, in shape [Q*L*numHead].
 * @param valueFeature Source value features, in shape [K*numHead*headDim].
 * @param output Output container, in shape [Q*numHead*headDim].
 * @param stream CUDA stream.
 *
 * @return cudaError_t CUDA error type.
 */
cudaError_t AttentionValueComputationLauncher(
  const int32_t B, const int32_t Q, const int32_t L, const int32_t K, const int32_t numHead,
  const int32_t headDim, const int * queryBatchCnt, const int * keyBatchCnt,
  const int * indexPairBatch, const int * indexPair, const float * attnWeight,
  const float * valueFeature, float * output, cudaStream_t stream);

#endif  // ATTENTION__TRT_ATTN_VALUE_COMPUTATION_KERNEL_HPP_

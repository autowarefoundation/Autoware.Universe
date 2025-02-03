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

#include "attention/trt_attn_value_computation_kernel.hpp"

#include <iostream>

/**
 * @brief Attention value computation kernel.
 *
 * @tparam d The size of shared memory, which should be equal to `L`.
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
 */
template <unsigned int d>
__global__ void attentionValueComputationKernel(
  const int32_t B, const int32_t Q, const int32_t L, const int32_t K, const int32_t numHead,
  const int32_t headDim, const int * queryBatchCnt, const int * keyBatchCnt,
  const int * indexPairBatch, const int * indexPair, const float * attnWeight,
  const float * valueFeature, float * output)
{
  const int query_idx = blockIdx.x;
  const int head_idx = blockIdx.y;
  const int hdim_idx = threadIdx.x;

  if (query_idx >= Q || head_idx >= numHead || hdim_idx >= headDim) {
    return;
  }

  // get key_start_idx
  const int batch_idx = indexPairBatch[query_idx];
  if (batch_idx < 0) {
    return;
  }

  int key_start_idx = 0;
  for (int i = 0; i < batch_idx; ++i) {
    key_start_idx += keyBatchCnt[i];
  }
  // get shared variables
  __shared__ float sharedAttnWeight[d];
  __shared__ int sharedValueIdx[d];
  for (int i = 0; i < L; i += blockDim.x) {
    sharedAttnWeight[i] = attnWeight[query_idx * L * numHead + i * numHead + head_idx];

    const int cur_key_idx = indexPair[query_idx * L + i];
    sharedValueIdx[i] = cur_key_idx == -1 ? -1 : cur_key_idx + key_start_idx;
  }
  __syncthreads();

  output += query_idx * numHead * headDim + head_idx * headDim + hdim_idx;

  float attn_result = 0.0f;
  for (int i = 0; i < L; ++i) {
    // TODO: fix bug (an illegal memory access was encountered)
    // value_idx need to guard with value_idx >= 0 && value_idx < K
    if (const int value_idx = sharedValueIdx[i]; value_idx >= 0 && value_idx < K) {
      attn_result += sharedAttnWeight[i] *
                     valueFeature[value_idx * numHead * headDim + head_idx * headDim + hdim_idx];
    }
  }
  output[0] = attn_result;
}

cudaError_t AttentionValueComputationLauncher(
  const int32_t B, const int32_t Q, const int32_t L, const int32_t K, const int32_t numHead,
  const int32_t headDim, const int * queryBatchCnt, const int * keyBatchCnt,
  const int * indexPairBatch, const int * indexPair, const float * attnWeight,
  const float * valueFeature, float * output, cudaStream_t stream)
{
  if (L > 512) {
    return cudaError::cudaErrorInvalidValue;
  }

  dim3 blocks(Q, numHead);
  dim3 threads(headDim);

  switch (L) {
    case 16:
      attentionValueComputationKernel<16><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
    case 32:
      attentionValueComputationKernel<32><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
    case 64:
      attentionValueComputationKernel<64><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
    case 128:
      attentionValueComputationKernel<128><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
    case 320:
      attentionValueComputationKernel<320><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
    case 384:
      attentionValueComputationKernel<384><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
    default:
      attentionValueComputationKernel<512><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        attnWeight, valueFeature, output);
      break;
  }

  return cudaGetLastError();
}

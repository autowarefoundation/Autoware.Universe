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

#include "attention/trt_attn_weight_computation_kernel.hpp"

/**
 * @brief Attention weight computation kernel.
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
 * @param queryFeature Source query features, in shape [Q*numHead*headDim].
 * @param keyFeature Source key features, in shape [K*numHead*headDim].
 * @param output Output container, in shape [Q*L*numHead].
 */
template <unsigned int d>
__global__ void attentionWeightComputationKernel(
  const int32_t B, const int32_t Q, const int32_t L, const int32_t K, const int32_t numHead,
  const int32_t headDim, const int * queryBatchCnt, const int * keyBatchCnt,
  const int * indexPairBatch, const int * indexPair, const float * queryFeature,
  const float * keyFeature, float * output)
{
  const int query_idx = blockIdx.x;
  const int head_idx = blockIdx.y;
  const int local_key_idx = threadIdx.x;

  const int index = query_idx * L + local_key_idx;

  if (query_idx >= Q || head_idx >= numHead || local_key_idx >= L) {
    return;
  }

  // build shared query features
  __shared__ float sharedQueryFeature[d];
  for (int i = local_key_idx; i < headDim; i += blockDim.x) {
    sharedQueryFeature[i] = queryFeature[query_idx * numHead * headDim + head_idx * headDim + i];
  }
  __syncthreads();

  if (indexPair[index] < 0) {
    // ignore index
    return;
  }

  // get real key index
  const int batch_idx = indexPairBatch[query_idx];
  if (batch_idx < 0) {
    return;
  }

  int key_start_idx = 0;
  for (int i = 0; i < batch_idx; ++i) {
    key_start_idx += keyBatchCnt[i];
  }
  key_start_idx += indexPair[index];

  // get key features
  keyFeature += key_start_idx * numHead * headDim + head_idx * headDim;
  output += index * numHead + head_idx;

  float attn_weight = 0.0f;
  for (int i = 0; i < headDim; ++i) {
    attn_weight += keyFeature[i] * sharedQueryFeature[i];
  }
  output[0] = attn_weight;
}

cudaError_t AttentionWeightComputationLauncher(
  const int32_t B, const int32_t Q, const int32_t L, const int32_t K, const int32_t numHead,
  const int32_t headDim, const int * queryBatchCnt, const int * keyBatchCnt,
  const int * indexPairBatch, const int * indexPair, const float * queryFeature,
  const float * keyFeature, float * output, cudaStream_t stream)
{
  if (headDim > 150) {
    return cudaError::cudaErrorInvalidValue;
  }

  dim3 blocks(Q, numHead);
  dim3 threads(L);

  switch (headDim) {
    case 16:
      attentionWeightComputationKernel<16><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
    case 24:
      attentionWeightComputationKernel<24><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
    case 32:
      attentionWeightComputationKernel<32><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
    case 48:
      attentionWeightComputationKernel<48><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
    case 64:
      attentionWeightComputationKernel<64><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
    case 128:
      attentionWeightComputationKernel<128><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
    default:
      attentionWeightComputationKernel<150><<<blocks, threads, 0, stream>>>(
        B, Q, L, K, numHead, headDim, queryBatchCnt, keyBatchCnt, indexPairBatch, indexPair,
        queryFeature, keyFeature, output);
      break;
  }

  return cudaGetLastError();
}

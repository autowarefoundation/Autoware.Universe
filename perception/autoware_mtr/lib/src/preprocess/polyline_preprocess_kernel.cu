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

#include "preprocess/polyline_preprocess_kernel.cuh"

#include <cub/cub.cuh>

#include <float.h>

#include <iostream>

__global__ void transformPolylineKernel(
  const int K, const int P, const int PointDim, const float * inPolyline, const int B,
  const int AgentDim, const float * targetState, float * outPolyline, bool * outPolylineMask)
{
  int b = blockIdx.x * blockDim.x + threadIdx.x;
  int k = blockIdx.y * blockDim.y + threadIdx.y;
  int p = blockIdx.z * blockDim.z + threadIdx.z;

  if (b >= B || k >= K || p >= P) {
    return;
  }

  const int inIdx = (k * P + p) * PointDim;
  const int outIdx = b * K * P + k * P + p;
  bool isValid = false;
  for (int d = 0; d < PointDim - 1; ++d) {
    if (inPolyline[inIdx + d] != 0.0f) {
      isValid = true;
    }
  }
  outPolylineMask[outIdx] = isValid;

  // initialize output polyline with 0.0
  for (int d = 0; d < PointDim + 2; ++d) {
    outPolyline[outIdx * (PointDim + 2) + d] = 0.0f;
  }

  // set transformed values if valid, otherwise all 0.0.
  if (isValid) {
    const float x = inPolyline[inIdx];
    const float y = inPolyline[inIdx + 1];
    const float z = inPolyline[inIdx + 2];
    const float dx = inPolyline[inIdx + 3];
    const float dy = inPolyline[inIdx + 4];
    const float dz = inPolyline[inIdx + 5];
    const float typeID = inPolyline[inIdx + 6];

    const int centerIdx = b * AgentDim;
    const float centerX = targetState[centerIdx];
    const float centerY = targetState[centerIdx + 1];
    const float centerZ = targetState[centerIdx + 2];
    const float centerYaw = targetState[centerIdx + 6];
    const float centerCos = cosf(centerYaw);
    const float centerSin = sinf(centerYaw);

    // do transform
    const float transX = centerCos * (x - centerX) - centerSin * (y - centerY);
    const float transY = centerSin * (x - centerX) + centerCos * (y - centerY);
    const float transZ = z - centerZ;
    const float transDx = centerCos * dx - centerSin * dy;
    const float transDy = centerSin * dx + centerCos * dy;
    const float transDz = dz;

    outPolyline[outIdx * (PointDim + 2)] = transX;
    outPolyline[outIdx * (PointDim + 2) + 1] = transY;
    outPolyline[outIdx * (PointDim + 2) + 2] = transZ;
    outPolyline[outIdx * (PointDim + 2) + 3] = transDx;
    outPolyline[outIdx * (PointDim + 2) + 4] = transDy;
    outPolyline[outIdx * (PointDim + 2) + 5] = transDz;
    outPolyline[outIdx * (PointDim + 2) + 6] = typeID;
  }
}

__global__ void setPreviousPositionKernel(
  const int B, const int K, const int P, const int D, float * polyline)
{
  int b = blockIdx.x * blockDim.x + threadIdx.x;
  int k = blockIdx.y * blockDim.y + threadIdx.y;
  int p = blockIdx.z * blockDim.z + threadIdx.z;

  if (b >= B || k >= K || p >= P) {
    return;
  }

  const int curIdx = (b * K * P + k * P + p) * D;
  const int preIdx = p == 0 ? curIdx : (b * K * P + k * P + p - 1) * D;

  polyline[curIdx + D - 2] = polyline[preIdx];      // x
  polyline[curIdx + D - 1] = polyline[preIdx + 1];  // y
}

__global__ void calculateCenterDistanceKernel(
  const int B, const int K, const int P, const int D, const float * polyline,
  const bool * polylineMask, float * distance)
{
  int b = blockIdx.x * blockDim.x + threadIdx.x;
  int k = blockIdx.y * blockDim.y + threadIdx.y;
  if (b >= B || k >= K) {
    return;
  }

  // calculate polyline center
  double sumX = 0.0, sumY = 0.0;
  double numValid = 0.0;
  for (int p = 0; p < P; ++p) {
    int idx = b * K * P + k * P + p;
    if (polylineMask[idx]) {
      sumX += polyline[idx * D];
      sumY += polyline[idx * D + 1];
      ++numValid;
    }
  }
  float centerX = static_cast<float>(sumX / max(1.0, numValid));
  float centerY = static_cast<float>(sumY / max(1.0, numValid));

  distance[b * K + k] = hypot(centerX, centerY);
}

template <unsigned int BLOCK_THREADS, unsigned int ITEMS_PER_THREAD>
__global__ void extractTopKPolylineKernel(
  const int K, const int B, const int L, const int P, const int D, const float * inPolyline,
  const bool * inPolylineMask, const float * inDistance, float * outPolyline,
  bool * outPolylineMask)
{
  int b = blockIdx.x;                             // Batch index
  int tid = threadIdx.x;                          // Thread local index in this CUDA block
  int p = blockIdx.y * blockDim.y + threadIdx.y;  // Point index
  int d = blockIdx.z * blockDim.z + threadIdx.z;  // Dim index
  // Since all threads in a block expected to work wiht CUB, `return` shold not be called here
  // if (b >= B || tid >= L || p >= P || d >= D) {
  //   return;
  // }

  // Specialize BlockRadixSort type
  using BlockRadixSortT = cub::BlockRadixSort<float, BLOCK_THREADS, ITEMS_PER_THREAD, unsigned int>;
  using TempStorageT = typename BlockRadixSortT::TempStorage;

  __shared__ TempStorageT temp_storage;

  float distances[ITEMS_PER_THREAD] = {0};
  unsigned int distance_indices[ITEMS_PER_THREAD] = {0};
  for (unsigned int i = 0; i < ITEMS_PER_THREAD; i++) {
    int polyline_idx = BLOCK_THREADS * i + tid;  // index order don't need to care.
    int distance_idx = b * L + polyline_idx;
    distance_indices[i] = polyline_idx;
    distances[i] = (polyline_idx < L && distance_idx < B * L) ? inDistance[distance_idx] : FLT_MAX;
  }

  BlockRadixSortT(temp_storage).Sort(distances, distance_indices);
  // Block-wide sync barrier necessary to refer the sort result
  __syncthreads();

  if (b >= B || tid >= L || p >= P || d >= D) {
    return;
  }

  for (unsigned int i = 0; i < ITEMS_PER_THREAD; i++) {
    int consective_polyline_idx =
      tid * ITEMS_PER_THREAD + i;  // To keep sorted order, theads have to write consective region
    int inIdx = b * L * P + distance_indices[i] * P + p;
    int outIdx = b * K * P + consective_polyline_idx * P + p;
    if (consective_polyline_idx >= K || fabsf(FLT_MAX - distances[i]) < FLT_EPSILON) {
      continue;
    }
    outPolyline[outIdx * D + d] = inPolyline[inIdx * D + d];
    outPolylineMask[outIdx] = inPolylineMask[inIdx];
  }
}

__global__ void calculatePolylineCenterKernel(
  const int B, const int K, const int P, const int D, const float * polyline,
  const bool * polylineMask, float * center)
{
  int b = blockIdx.x * blockDim.x + threadIdx.x;
  int k = blockIdx.y * blockDim.y + threadIdx.y;

  if (b >= B || k >= K) {
    return;
  }

  // initialize with 0.0
  int centerIdx = (b * K + k) * 3;
  for (int d = 0; d < 3; ++d) {
    center[centerIdx + d] = 0.0f;
  }

  // calculate polyline center
  double sumX = 0.0, sumY = 0.0, sumZ = 0.0;
  double numValid = 0.0;
  for (int p = 0; p < P; ++p) {
    int idx = b * K * P + k * P + p;
    if (polylineMask[idx]) {
      sumX += polyline[idx * D];
      sumY += polyline[idx * D + 1];
      sumZ += polyline[idx * D + 2];
      ++numValid;
    }
  }

  center[centerIdx] = static_cast<float>(sumX / max(1.0, numValid));
  center[centerIdx + 1] = static_cast<float>(sumY / max(1.0, numValid));
  center[centerIdx + 2] = static_cast<float>(sumZ / max(1.0, numValid));
}

cudaError_t polylinePreprocessWithTopkLauncher(
  const int K, const int L, const int P, const int PointDim, const float * inPolyline, const int B,
  const int AgentDim, const float * targetState, float * tmpPolyline, bool * tmpPolylineMask,
  float * tmpDistance, float * outPolyline, bool * outPolylineMask, float * outPolylineCenter,
  cudaStream_t stream)
{
  if (L < K) {
    std::cerr << "L must be greater than K, but got L: " << L << ", K: " << K << std::endl;
    return cudaError_t::cudaErrorInvalidValue;
  }

  const int outPointDim = PointDim + 2;

  // TODO: update the number of blocks and threads to guard from `cudaErrorIllegalAccess`
  constexpr int threadsPerBlock = 256;
  const dim3 blocks1(B, L, P);
  transformPolylineKernel<<<blocks1, threadsPerBlock, 0, stream>>>(
    L, P, PointDim, inPolyline, B, AgentDim, targetState, tmpPolyline, tmpPolylineMask);

  const dim3 blocks2(B, L);
  calculateCenterDistanceKernel<<<blocks2, threadsPerBlock, 0, stream>>>(
    B, L, P, outPointDim, tmpPolyline, tmpPolylineMask, tmpDistance);

  const dim3 blocks3(B, P, outPointDim);
  constexpr unsigned int itemsPerThread = 24;
  if (threadsPerBlock * itemsPerThread < L) {
    std::cerr << "Larger L (" << L << ") than acceptable range (< "
              << threadsPerBlock * itemsPerThread << ") detected." << std::endl;
    return cudaError_t::cudaErrorInvalidValue;
  }
  extractTopKPolylineKernel<threadsPerBlock, itemsPerThread>
    <<<blocks3, threadsPerBlock, 0, stream>>>(
      K, B, L, P, outPointDim, tmpPolyline, tmpPolylineMask, tmpDistance, outPolyline,
      outPolylineMask);

  const dim3 blocks4(B, K, P);
  setPreviousPositionKernel<<<blocks4, threadsPerBlock, 0, stream>>>(
    B, K, P, outPointDim, outPolyline);

  const dim3 blocks5(B, K);
  calculatePolylineCenterKernel<<<blocks5, threadsPerBlock, 0, stream>>>(
    B, K, P, outPointDim, outPolyline, outPolylineMask, outPolylineCenter);

  return cudaGetLastError();
}

cudaError_t polylinePreprocessLauncher(
  const int K, const int P, const int PointDim, const float * inPolyline, const int B,
  const int AgentDim, const float * targetState, float * outPolyline, bool * outPolylineMask,
  float * outPolylineCenter, cudaStream_t stream)
{
  const int outPointDim = PointDim + 2;

  // TODO: update the number of blocks and threads to guard from `cudaErrorIllegalAccess`
  constexpr int threadsPerBlock = 256;
  const dim3 block3d(B, K, P);
  transformPolylineKernel<<<block3d, threadsPerBlock, 0, stream>>>(
    K, P, PointDim, inPolyline, B, AgentDim, targetState, outPolyline, outPolylineMask);

  setPreviousPositionKernel<<<block3d, threadsPerBlock, 0, stream>>>(
    B, K, P, outPointDim, outPolyline);

  const dim3 block2d(B, K);
  calculatePolylineCenterKernel<<<block2d, threadsPerBlock, 0, stream>>>(
    B, K, P, outPointDim, outPolyline, outPolylineMask, outPolylineCenter);

  return cudaGetLastError();
}

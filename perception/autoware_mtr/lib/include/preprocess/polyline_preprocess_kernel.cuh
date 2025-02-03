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

#ifndef PREPROCESS__POLYLINE_PREPROCESS_KERNEL_CUH_
#define PREPROCESS__POLYLINE_PREPROCESS_KERNEL_CUH_

/**
 * @brief Transform polylines to target agent coordinate system and extend its feature with previous
 * x, y`(D->D+2)`.
 *
 * Points which all elements are 0.0 except of typeID are filled by 0.0 and the corresponding
 * mask value is 0.0 too.
 *
 * @param K The number of polylines.
 * @param P The number of points contained in each polyline.
 * @param PointDim The number of point dimensions, expecting (x, y, z, dx, dy, dz, typeID).
 * @param inPolyline Source polyline, in shape [K*P*D].
 * @param B The number of target agents.
 * @param AgentDim The number of agent state dimensions, expecting (x, y, z, length, width, height,
 * yaw, vx, vy, ax, ay).
 * @param targetState Source target agent states, in shape [B*AgentDim].
 * @param outPolyline Output polyline, in shape [B*K*P*(PointDim+2)].
 * @param outPolylineMask Output polyline mask, in shape [B*K*P].
 */
__global__ void transformPolylineKernel(
  const int K, const int P, const int PointDim, const float * inPolyline, const int B,
  const int AgentDim, const float * targetState, float * outPolyline, bool * outPolylineMask);

/**
 * @brief Set the Previous Position Kernel object
 *
 * @param B The number of target agents.
 * @param K The number of polylines.
 * @param P The number of points contained in each polyline.
 * @param D The number of point dimensions, expecting (x, y, ..., preX, preY).
 * @param polyline Source polyline, in shape [B*K*P*D].
 */
__global__ void setPreviousPositionKernel(
  const int B, const int K, const int P, const int D, float * polyline);

/**
 * @brief Calculate center distance from target agent to each polyline.
 *
 * @note Polyline must have been transformed to target agent coordinates system.
 *
 * @param B The number of target agents.
 * @param K The number of polylines.
 * @param P The number of points contained in each polyline.
 * @param D The number of point dimensions, expecting [x, y, ...].
 * @param polyline Source polyline, in shape [B*K*P*D].
 * @param polylineMask Source polyline mask, in shape [B*K*P].
 * @param distance Output calculated distances, in shape [B*K].
 */
__global__ void calculateCenterDistanceKernel(
  const int B, const int K, const int P, const int D, const float * polyline,
  const bool * polylineMask, float * distance);

/**
 * @brief Extract K polylines with the smallest distances.
 *
 * @note Because this kernel supposes to allocate shared memory dynamically it is necessary to
 * specify `sizeof(float) * L` in the kernel execution configuration.
 *
 * @param K The number of polylines to be extracted.
 * @param B The number of target agents.
 * @param L The number of original polylines.
 * @param P The number of points contained in each polyline.
 * @param D The number of point dimensions.
 * @param inPolyline Source polyline, in shape [B*L*P*D].
 * @param inPolylineMask Source polyline mask, in shape [B*L*P].
 * @param inDistance Source distances from target agents to the centers of each polyline, in shape
 * [B*L].
 * @param outPolyline Output polyline, in shape [B*K*P*D].
 * @param outPolylineMask Output polyline mask, in shape [B*K*P].
 */
__global__ void extractTopKPolylineKernel(
  const int K, const int B, const int L, const int P, const int D, const float * inPolyline,
  const bool * inPolylineMask, const float * inDistance, float * outPolyline,
  bool * outPolylineMask);

/**
 * @brief Calculate center positions of each polyline with respect to target agent coordinates
 * system.
 *
 * @note Polyline must have been transformed to target agent coordinates system.
 *
 * @param B The number of target agents.
 * @param K The number of polylines.
 * @param P The number of points contained in each polyline.
 * @param D The number of point dimensions, expecting (x, y, z, ...).
 * @param polyline Source polyline, in shape [B*K*P*D].
 * @param polylineMask Source polyline mask, in shape [B*K*P].
 * @param center Output centers, in shape [B*K*3].
 */
__global__ void calculatePolylineCenterKernel(
  const int B, const int K, const int P, const int D, const float * polyline,
  const bool * polylineMask, float * center);

/**
 * @brief In cases of the number of batch polylines (L) is greater than K,
 *  extracts the topK elements.
 *
 * @param K The number of polylines to be extracted.
 * @param L The number of original polylines.
 * @param P The number of points contained in each polyline.
 * @param PointDim The number of point state dimensions.
 * @param inPolyline Source polylines, in shape [L*P*PointDim].
 * @param B The number of target agents.
 * @param AgentDim The number of agent state dimensions.
 * @param targetState Target agent state at the latest timestamp, in shape [B*AgentDim].
 * @param tmpPolyline A container to store transformed polyline temporary, in shape
 * [B*L*P*(PointDim+2)].
 * @param tmpPolylineMask A container to store transformed polyline mask temporary, in shape
 * [B*L*P].
 * @param tmpDistance A container to store distances temporary, in shape [B*L].
 * @param outPolyline Output polylines, in shape [B*K*P*(PointDim+2)].
 * @param outPolylineMask Output polyline masks, in shape [B*K*P].
 * @param outPolylineCenter Output magnitudes of each polyline with respect to target coords,
 *  in shape [B*K*3].
 * @param stream CUDA stream.
 * @return cudaError_t
 */
cudaError_t polylinePreprocessWithTopkLauncher(
  const int K, const int L, const int P, const int PointDim, const float * inPolyline, const int B,
  const int AgentDim, const float * targetState, float * tmpPolyline, bool * tmpPolylineMask,
  float * tmpDistance, float * outPolyline, bool * outPolylineMask, float * outPolylineCenter,
  cudaStream_t stream);

/**
 * @brief Do preprocess for polyline if the number of batched polylines is K.
 *
 * @param K The number of polylines.
 * @param P The number of points contained in each polyline.
 * @param PointDim The number of point state dimensions.
 * @param inPolyline Source polylines, in shape [K*P*PointDim].
 * @param B The number of target agents.
 * @param AgentDim The number of agent state dimensions.
 * @param targetState Target agent state at the latest timestamp, in shape [B*AgentDim].
 * @param outPolyline Output polylines, in shape [B*K*P*(PointDim+2)].
 * @param outPolylineMask Output polyline masks, in shape [B*K*P].
 * @param outPolylineCenter Output magnitudes of each polyline with respect to target coords,
 *  in shape [B*K*3].
 * @param stream CUDA stream.
 * @return cudaError_t
 */
cudaError_t polylinePreprocessLauncher(
  const int K, const int P, const int PointDim, const float * inPolyline, const int B,
  const int AgentDim, const float * targetState, float * outPolyline, bool * outPolylineMask,
  float * outPolylineCenter, cudaStream_t stream);

#endif  // PREPROCESS__POLYLINE_PREPROCESS_KERNEL_CUH_

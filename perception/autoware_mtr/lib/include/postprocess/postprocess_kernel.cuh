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

#ifndef POSTPROCESS__POSTPROCESS_KERNEL_CUH__
#define POSTPROCESS__POSTPROCESS_KERNEL_CUH__

#include <cuda_runtime.h>

/**
 * @brief Transform predicted trajectory from target agent coords to world coords.
 *
 * @param B The number of target agents.
 * @param M The number of modes.
 * @param T The number of future timestamps.
 * @param AgentDim The number of target agent state dimensions, expecting (x, y, z, length, width,
 * height, yaw, vx, vy, ax, ay).
 * @param targetState Source target agent state, in shape [B*AgentDim].
 * @param PredDim The number of predicted state dimension, expecting (x, y, ?, ?, ?, vx, vy).
 * @param predTrajectory Predicted trajectory, in shape [B*M*T*PredDim].
 */
__global__ void transformTrajectoryKernel(
  const int B, const int M, const int T, const int AgentDim, const float * targetState,
  const int PredDim, float * predTrajectory);

/**
 * @brief Execute postprocess to predicted score and trajectory.
 *
 * @param B The number of target agents.
 * @param M The number of modes.
 * @param T The number of future timestamps.
 * @param AgentDim The number of target agent state dimensions, expecting (x, y, z, length, width,
 * height, yaw, vx, vy, ax, ay).
 * @param targetState Target agent states at the latest timestamp, in shape [B*inDim].
 * @param PredDim The number predicted state dimensions, expecting (x, y, ?, ?, ?, vx, vy).
 * @param predTrajectory Predicted trajectory, in shape [B*M*T*PredDim].
 * @param stream CUDA stream.
 * @return cudaError_t CUDA error type.
 */
cudaError_t postprocessLauncher(
  const int B, const int M, const int T, const int AgentDim, const float * targetState,
  const int PredDim, float * predTrajectory, cudaStream_t stream);

#endif  // POSTPROCESS__POSTPROCESS_KERNEL_CUH__

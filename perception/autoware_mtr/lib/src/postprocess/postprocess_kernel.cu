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

__global__ void transformTrajectoryKernel(
  const int B, const int M, const int T, const int AgentDim, const float * targetState,
  const int PredDim, float * predTrajectory)
{
  int b = blockIdx.x * blockDim.x + threadIdx.x;
  int m = blockIdx.y * blockDim.y + threadIdx.y;
  int t = blockIdx.z * blockDim.z + threadIdx.z;

  if (b >= B || m >= M || t >= T) {
    return;
  }

  const int predIdx = (b * M * T + m * T + t) * PredDim;
  const float predX = predTrajectory[predIdx];
  const float predY = predTrajectory[predIdx + 1];

  const int targetIdx = b * AgentDim;
  const float targetX = targetState[targetIdx];
  const float targetY = targetState[targetIdx + 1];
  const float targetYaw = targetState[targetIdx + 6];
  const float targetCos = cosf(targetYaw);
  const float targetSin = sinf(targetYaw);

  predTrajectory[predIdx] = targetCos * predX - targetSin * predY + targetX;
  predTrajectory[predIdx + 1] = targetSin * predX + targetCos * predY + targetY;
}

cudaError_t postprocessLauncher(
  const int B, const int M, const int T, const int AgentDim, const float * targetState,
  const int PredDim, float * predTrajectory, cudaStream_t stream)
{
  // TODO: update the number of blocks and threads to guard from `cudaErrorIllegalAccess`
  constexpr int threadsPerBlock = 256;
  dim3 blocks(B, M, T);

  transformTrajectoryKernel<<<blocks, threadsPerBlock, 0, stream>>>(
    B, M, T, AgentDim, targetState, PredDim, predTrajectory);

  return cudaGetLastError();
}

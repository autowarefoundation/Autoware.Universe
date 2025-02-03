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

#include "autoware/pointcloud_preprocessor/concatenate_data/combine_cloud_handler_kernel.hpp"

#include <cuda_runtime.h>

#include <cstdio>

namespace autoware::pointcloud_preprocessor
{

__global__ void transform_kernel(
  const PointTypeStruct * input_points, int num_points, TransformStruct transform,
  PointTypeStruct * output_points)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < num_points) {
    float x = input_points[idx].x;
    float y = input_points[idx].y;
    float z = input_points[idx].z;

    output_points[idx].x =
      transform.m11 * x + transform.m12 * y + transform.m13 * z + transform.translation_x;
    output_points[idx].y =
      transform.m21 * x + transform.m22 * y + transform.m23 * z + transform.translation_y;
    output_points[idx].z =
      transform.m31 * x + transform.m32 * y + transform.m33 * z + transform.translation_z;
    output_points[idx].intensity = input_points[idx].intensity;
    output_points[idx].return_type = input_points[idx].return_type;
    output_points[idx].channel = input_points[idx].channel;
  }
}

void transform_launch(
  const PointTypeStruct * input_points, int num_points, TransformStruct transform,
  PointTypeStruct * output_points, cudaStream_t & stream)
{
  int threadsPerBlock = 256;
  int blocksPerGrid = (num_points + threadsPerBlock - 1) / threadsPerBlock;

  transform_kernel<<<blocksPerGrid, threadsPerBlock, 0, stream>>>(
    input_points, num_points, transform, output_points);
}

}  // namespace autoware::pointcloud_preprocessor

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

#ifndef AUTOWARE__POINTCLOUD_PREPROCESSOR__CONCATENATE_DATA__COMBINE_CLOUD_HANDLER_KERNEL_HPP_
#define AUTOWARE__POINTCLOUD_PREPROCESSOR__CONCATENATE_DATA__COMBINE_CLOUD_HANDLER_KERNEL_HPP_

#ifdef USE_CUDA
#include <cuda_runtime.h>
#endif

#include <cstdint>

namespace autoware::pointcloud_preprocessor
{

struct TransformStruct
{
  float translation_x;
  float translation_y;
  float translation_z;
  float m11;
  float m12;
  float m13;
  float m21;
  float m22;
  float m23;
  float m31;
  float m32;
  float m33;
};

struct PointTypeStruct
{
  float x;
  float y;
  float z;
  std::uint8_t intensity;
  std::uint8_t return_type;
  std::uint16_t channel;
};

#ifdef USE_CUDA
void transform_launch(
  const PointTypeStruct * input_points, int num_points, TransformStruct transform,
  PointTypeStruct * output_points, cudaStream_t & stream);
#endif

}  // namespace autoware::pointcloud_preprocessor

#endif  // AUTOWARE__POINTCLOUD_PREPROCESSOR__CONCATENATE_DATA__COMBINE_CLOUD_HANDLER_KERNEL_HPP_

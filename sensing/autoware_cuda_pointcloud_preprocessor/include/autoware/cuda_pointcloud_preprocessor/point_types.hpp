
// Copyright 2024 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef AUTOWARE__CUDA_POINTCLOUD_PREPROCESSOR__POINT_TYPES_HPP_
#define AUTOWARE__CUDA_POINTCLOUD_PREPROCESSOR__POINT_TYPES_HPP_

#include <cstdint>

namespace autoware::cuda_pointcloud_preprocessor
{

// Note: We can not use PCL nor uniform initialization here because of thrust
struct OutputPointType
{
  float x;
  float y;
  float z;
  std::uint8_t intensity;
  std::uint8_t return_type;
  std::uint16_t channel;
};

struct InputPointType
{
  float x;
  float y;
  float z;
  std::uint8_t intensity;
  std::uint8_t return_type;
  std::uint16_t channel;
  float azimuth;
  float elevation;
  float distance;
  std::uint32_t time_stamp;
};

}  // namespace autoware::cuda_pointcloud_preprocessor

#endif  // AUTOWARE__CUDA_POINTCLOUD_PREPROCESSOR__POINT_TYPES_HPP_

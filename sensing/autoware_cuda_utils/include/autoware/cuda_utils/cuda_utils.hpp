// Copyright 2025 Tier IV, Inc.
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

// This code is licensed under CC0 1.0 Universal (Public Domain).
// You can use this without any limitation.
// https://creativecommons.org/publicdomain/zero/1.0/deed.en
// borrowed from https://proc-cpuinfo.fixstars.com/2019/02/cuda_smart_pointer/

#ifndef AUTOWARE__CUDA_UTILS__CUDA_UTILS_HPP_
#define AUTOWARE__CUDA_UTILS__CUDA_UTILS_HPP_

#include "autoware/cuda_utils/cuda_check_error.hpp"

#include <cuda_runtime_api.h>

namespace autoware::cuda_utils
{
template <typename T>
void clear_async(T * ptr, std::size_t num_elem, cudaStream_t stream)
{
  CHECK_CUDA_ERROR(::cudaMemsetAsync(ptr, 0, sizeof(T) * num_elem, stream));
}
}  // namespace autoware::cuda_utils

#endif  // AUTOWARE__CUDA_UTILS__CUDA_UTILS_HPP_

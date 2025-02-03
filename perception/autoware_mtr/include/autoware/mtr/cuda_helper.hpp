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

/*
 * This code is licensed under CC0 1.0 Universal (Public Domain).
 * You can use this without any limitation.
 * https://creativecommons.org/publicdomain/zero/1.0/deed.en
 */

#ifndef AUTOWARE__MTR__CUDA_HELPER_HPP_
#define AUTOWARE__MTR__CUDA_HELPER_HPP_

#include <cuda_runtime_api.h>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#define CHECK_CUDA_ERROR(e) (cuda::check_error(e, __FILE__, __LINE__))

namespace cuda
{
/**
 * @brief Throw if the input cuda error is not `cudaSuccess`.
 *
 * @param e The cuda error types.
 * @param f The file name.
 * @param n The line number.
 */
inline void check_error(const ::cudaError_t e, const char * f, int n)
{
  if (e != ::cudaSuccess) {
    ::std::stringstream s;
    s << ::cudaGetErrorName(e) << " (" << e << ")@" << f << "#L" << n << ": "
      << ::cudaGetErrorString(e);
    throw ::std::runtime_error{s.str()};
  }
}

struct deleter
{
  void operator()(void * p) const { CHECK_CUDA_ERROR(::cudaFree(p)); }
};

template <typename T>
using unique_ptr = ::std::unique_ptr<T, deleter>;

template <typename T>
typename ::std::enable_if<::std::is_array<T>::value, cuda::unique_ptr<T>>::type make_unique(
  const ::std::size_t n)
{
  using U = typename ::std::remove_extent<T>::type;
  U * p;
  CHECK_CUDA_ERROR(::cudaMalloc(reinterpret_cast<void **>(&p), sizeof(U) * n));
  return cuda::unique_ptr<T>{p};
}

template <typename T>
cuda::unique_ptr<T> make_unique()
{
  T * p;
  CHECK_CUDA_ERROR(::cudaMalloc(reinterpret_cast<void **>(&p), sizeof(T)));
  return cuda::unique_ptr<T>{p};
}

constexpr size_t CUDA_ALIGN = 256;

template <typename T>
inline size_t get_size_aligned(size_t num_elem)
{
  size_t size = num_elem * sizeof(T);
  size_t extra_align = 0;
  if (size % CUDA_ALIGN != 0) {
    extra_align = CUDA_ALIGN - size % CUDA_ALIGN;
  }
  return size + extra_align;
}

template <typename T>
inline T * get_next_ptr(size_t num_elem, void *& workspace, size_t & workspace_size)
{
  size_t size = get_size_aligned<T>(num_elem);
  if (size > workspace_size) {
    throw ::std::runtime_error("Workspace is too small!");
  }
  workspace_size -= size;
  T * ptr = reinterpret_cast<T *>(workspace);
  workspace = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(workspace) + size);
  return ptr;
}

class EventDebugger
{
public:
  void createEvent(cudaStream_t stream = 0)
  {
    CHECK_CUDA_ERROR(cudaEventCreate(&start_));
    CHECK_CUDA_ERROR(cudaEventCreate(&stop_));
    CHECK_CUDA_ERROR(cudaEventRecord(start_, stream));
    has_event_ = true;
  }

  void printElapsedTime(cudaStream_t stream = 0)
  {
    if (!has_event_) {
      std::cerr << "No event was found" << std::endl;
    } else {
      CHECK_CUDA_ERROR(cudaEventRecord(stop_, stream));
      CHECK_CUDA_ERROR(cudaEventSynchronize(stop_));
      float elapsed_time;
      CHECK_CUDA_ERROR(cudaEventElapsedTime(&elapsed_time, start_, stop_));
      std::cout << "Processing time: " << elapsed_time << "ms" << std::endl;
      cudaEventDestroy(start_);
      cudaEventDestroy(stop_);
      has_event_ = false;
    }
  }

private:
  cudaEvent_t start_, stop_;
  bool has_event_{false};
};

}  // namespace cuda

#endif  // AUTOWARE__MTR__CUDA_HELPER_HPP_

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

#ifndef COMMON__TRT_PLUGIN_HELPER_HPP_
#define COMMON__TRT_PLUGIN_HELPER_HPP_

#include <NvInferRuntime.h>

#include <iostream>

#define THREADS_PER_BLOCK 256
#define DIVUP(m, n) ((m) / (n) + ((m) % (n) > 0))

enum pluginStatus_t {
  STATUS_SUCCESS = 0,
  STATUS_FAILURE = 1,
  STATUS_BAD_PARAM = 2,
  STATUS_NOT_SUPPORTED = 3,
  STATUS_NOT_INITIALIZED = 4
};  // enum pluginStatus_t

#define PLUGIN_ASSERT(assertion)                                             \
  {                                                                          \
    if (!(assertion)) {                                                      \
      std::cerr << "#assertion" << __FILE__ << "," << __LINE__ << std::endl; \
      abort();                                                               \
    }                                                                        \
  }

#endif  // COMMON__TRT_PLUGIN_HELPER_HPP_

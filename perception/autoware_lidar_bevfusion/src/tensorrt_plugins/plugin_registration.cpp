// Copyright 2025 TIER IV, Inc.
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

#include "autoware/tensorrt_plugins/get_indice_pairs_implicit_gemm_plugin_creator.hpp"
#include "autoware/tensorrt_plugins/implicit_gemm_plugin_creator.hpp"
#include "autoware/tensorrt_plugins/quick_cumsum_cuda_plugin_creator.hpp"

#include <NvInferRuntime.h>

#include <cstdint>
#include <iostream>
#include <mutex>

class ThreadSafeLoggerFinder
{
public:
  ThreadSafeLoggerFinder() = default;

  // Set the logger finder.
  void setLoggerFinder(nvinfer1::ILoggerFinder * finder)
  {
    std::lock_guard<std::mutex> lk(mutex_);
    if (logger_finder_ == nullptr && finder != nullptr) {
      logger_finder_ = finder;
    }
  }

  // Get the logger.
  nvinfer1::ILogger * getLogger() noexcept
  {
    std::lock_guard<std::mutex> lk(mutex_);
    if (logger_finder_ != nullptr) {
      return logger_finder_->findLogger();
    }
    return nullptr;
  }

private:
  nvinfer1::ILoggerFinder * logger_finder_{nullptr};
  std::mutex mutex_;
};

ThreadSafeLoggerFinder glogger_finder;

extern "C" void setLoggerFinder(nvinfer1::ILoggerFinder * finder)
{
  glogger_finder.setLoggerFinder(finder);
}

extern "C" nvinfer1::IPluginCreatorInterface * const * getCreators(std::int32_t & num_creators)
{
  num_creators = 3;
  static nvinfer1::plugin::QuickCumsumCudaPluginCreator quick_cumsum_cuda_plugin_creator{};
  static nvinfer1::plugin::GetIndicePairsImplicitGemmPluginCreator
    get_indice_pairs_implicit_gemm_plugin_creator{};
  static nvinfer1::plugin::ImplicitGemmPluginCreator implicit_gemm_plugin_creator{};

  static nvinfer1::IPluginCreatorInterface * const plugin_creator_list[] = {
    &quick_cumsum_cuda_plugin_creator, &get_indice_pairs_implicit_gemm_plugin_creator,
    &implicit_gemm_plugin_creator};
  return plugin_creator_list;
}

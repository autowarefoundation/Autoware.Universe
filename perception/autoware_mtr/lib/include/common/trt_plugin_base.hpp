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

#ifndef COMMON__TRT_PLUGIN_BASE_HPP_
#define COMMON__TRT_PLUGIN_BASE_HPP_

#include "trt_plugin_helper.hpp"

#include <NvInferRuntime.h>
#include <NvInferVersion.h>
#include <cuda_runtime.h>
#include <cudnn.h>

#include <string>
#include <vector>

namespace autoware::trt_mtr
{
#if NV_TENSORRT_MAJOR > 7
#define TRT_NOEXCEPT noexcept
#else
#define TRT_NOEXCEPT
#endif

class TRTPluginBase : public nvinfer1::IPluginV2DynamicExt
{
public:
  explicit TRTPluginBase(const std::string & name) : mLayerName(name) {}

  /* IPluginV2 methods */

  /**
   * @brief Return the plugin version. Should match the plugin version returned by the corresponding
   * plugin creator.
   *
   * @return const char*
   */
  const char * getPluginVersion() const TRT_NOEXCEPT override { return "1"; }

  /**
   * @brief Initialize the layer for execution. This is called when the engine is created.
   *
   * @return int 0 for success, else non-zero (which will cause engine termination).
   */
  int initialize() TRT_NOEXCEPT override { return STATUS_SUCCESS; }

  /**
   * @brief Release resources acquired during plugin layer initialization. This is called when the
   * engine is destroyed.
   *
   */
  void terminate() TRT_NOEXCEPT override {}

  /**
   * @brief Destroy the plugin objects. This will be called when the network, builder or engine is
   * destroyed.
   *
   */
  void destroy() TRT_NOEXCEPT override { delete this; }

  /**
   * @brief Set the Plugin Namespace object
   *
   * @param pluginNamespace
   */
  void setPluginNamespace(const char * pluginNamespace) TRT_NOEXCEPT override
  {
    mNamespace = pluginNamespace;
  }

  /**
   * @brief Get the Plugin Namespace object
   *
   * @return const char*
   */
  const char * getPluginNamespace() const TRT_NOEXCEPT override { return mNamespace.c_str(); }

  /**
   * @brief
   *
   * @param inDesc
   * @param nbInputs
   * @param outDesc
   * @param nbOutputs
   */
  void configurePlugin(
    const nvinfer1::DynamicPluginTensorDesc *, int, const nvinfer1::DynamicPluginTensorDesc *,
    int) TRT_NOEXCEPT override
  {
  }

  /**
   * @brief Get the Workspace Size object
   *
   * @param inDesc
   * @param nbInputs
   * @param outDesc
   * @param nbOutputs
   * @return size_t
   */
  size_t getWorkspaceSize(
    const nvinfer1::PluginTensorDesc *, int, const nvinfer1::PluginTensorDesc *,
    int) const TRT_NOEXCEPT override
  {
    return 0;
  }

  /**
   * @brief
   *
   * @param cudnnCtx
   * @param cublasCtx
   * @param gpuAllocator
   */
  void attachToContext(cudnnContext *, cublasContext *, nvinfer1::IGpuAllocator *)
    TRT_NOEXCEPT override
  {
  }

  /**
   * @brief
   *
   */
  void detachFromContext() TRT_NOEXCEPT override {}

protected:
  const std::string mLayerName;
  std::string mNamespace;

#if NV_TENSORRT_MAJOR < 8
protected:
  // To prevent compiler warnings.
  using nvinfer1::IPluginV2DynamicExt::canBroadcastInputAcrossBatch;
  using nvinfer1::IPluginV2DynamicExt::enqueue;
  using nvinfer1::IPluginV2DynamicExt::getOutputDimensions;
  using nvinfer1::IPluginV2DynamicExt::isOutputBroadcastAcrossBatch;
  using nvinfer1::IPluginV2DynamicExt::supportsFormat;
#endif
};

class TRTPluginCreatorBase : public nvinfer1::IPluginCreator
{
public:
  const char * getPluginVersion() const TRT_NOEXCEPT override { return "1"; }

  const nvinfer1::PluginFieldCollection * getFieldNames() TRT_NOEXCEPT override { return &mFC; }

  void setPluginNamespace(const char * pluginNamespace) TRT_NOEXCEPT override
  {
    mNamespace = pluginNamespace;
  }

  const char * getPluginNamespace() const TRT_NOEXCEPT override { return mNamespace.c_str(); }

protected:
  nvinfer1::PluginFieldCollection mFC;
  std::vector<nvinfer1::PluginField> mPluginAttributes;
  std::string mNamespace;
};
}  // namespace autoware::trt_mtr

#endif  // COMMON__TRT_PLUGIN_BASE_HPP_

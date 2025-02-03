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

#include "autoware/tensorrt_plugins/quick_cumsum_cuda_plugin.hpp"

#include "autoware/bev_ops/bev_pool_cuda.h"
#include "autoware/tensorrt_plugins/plugin_utils.hpp"

#include <NvInferRuntime.h>
#include <NvInferRuntimePlugin.h>

#include <algorithm>  // NOTE(knzo25): temporary
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <vector>

namespace nvinfer1
{
namespace plugin
{

QuickCumsumCudaPlugin::QuickCumsumCudaPlugin(
  const std::string & name, QuickCumsumCudaParameters const & params)
: layer_name_{name}, params_{params}
{
  initFieldsToSerialize();
}

void QuickCumsumCudaPlugin::initFieldsToSerialize()
{
  data_to_serialize_.clear();
  data_to_serialize_.emplace_back("batch_size", &params_.batch_size, PluginFieldType::kINT32, 1);
  data_to_serialize_.emplace_back("dimension", &params_.dimension, PluginFieldType::kINT32, 1);
  data_to_serialize_.emplace_back("height", &params_.height, PluginFieldType::kINT32, 1);
  data_to_serialize_.emplace_back("width", &params_.width, PluginFieldType::kINT32, 1);

  fc_to_serialize_.nbFields = data_to_serialize_.size();
  fc_to_serialize_.fields = data_to_serialize_.data();
}

IPluginCapability * QuickCumsumCudaPlugin::getCapabilityInterface(
  PluginCapabilityType type) noexcept
{
  try {
    if (type == PluginCapabilityType::kBUILD) {
      return static_cast<IPluginV3OneBuild *>(this);
    }
    if (type == PluginCapabilityType::kRUNTIME) {
      return static_cast<IPluginV3OneRuntime *>(this);
    }
    PLUGIN_ASSERT(type == PluginCapabilityType::kCORE);
    return static_cast<IPluginV3OneCore *>(this);
  } catch (std::exception const & e) {
    caughtError(e);
  }
  return nullptr;
}

IPluginV3 * QuickCumsumCudaPlugin::clone() noexcept
{
  try {
    IPluginV3 * const plugin{new QuickCumsumCudaPlugin{layer_name_, params_}};
    return plugin;
  } catch (std::exception const & e) {
    caughtError(e);
  }
  return nullptr;
}

char const * QuickCumsumCudaPlugin::getPluginName() const noexcept
{
  return kQUICK_CUMSUM_CUDA_PLUGIN_NAME;
}

char const * QuickCumsumCudaPlugin::getPluginVersion() const noexcept
{
  return kQUICK_CUMSUM_CUDA_PLUGIN_VERSION;
}

char const * QuickCumsumCudaPlugin::getPluginNamespace() const noexcept
{
  return kQUICK_CUMSUM_CUDA_PLUGIN_NAMESPACE;
}

std::int32_t QuickCumsumCudaPlugin::getNbOutputs() const noexcept
{
  return 1;
}

std::int32_t QuickCumsumCudaPlugin::configurePlugin(
  DynamicPluginTensorDesc const * in, std::int32_t num_inputs, DynamicPluginTensorDesc const * out,
  std::int32_t num_outputs) noexcept
{
  // Validate input arguments.
  PLUGIN_ASSERT(num_inputs == 4);
  PLUGIN_ASSERT(num_outputs == 1);
  PLUGIN_ASSERT(in[0].desc.dims.nbDims == 2);
  PLUGIN_ASSERT(in[1].desc.dims.nbDims == 2);
  PLUGIN_ASSERT(in[2].desc.dims.nbDims == 1);
  PLUGIN_ASSERT(in[3].desc.dims.nbDims == 1);

  PLUGIN_ASSERT(out[0].desc.dims.nbDims == 5);

  PLUGIN_ASSERT(in[0].desc.dims.d[0] == -1);
  PLUGIN_ASSERT(in[1].desc.dims.d[0] == -1);
  PLUGIN_ASSERT(in[1].desc.dims.d[1] == 4);

  PLUGIN_ASSERT(in[2].desc.dims.d[0] == -1);
  PLUGIN_ASSERT(in[3].desc.dims.d[0] == -1);

  PLUGIN_ASSERT(out[0].desc.type == in[0].desc.type);

  return 0;
}

bool QuickCumsumCudaPlugin::supportsFormatCombination(
  std::int32_t pos, DynamicPluginTensorDesc const * in_out, std::int32_t num_inputs,
  std::int32_t num_outputs) noexcept
{
  PLUGIN_ASSERT(num_inputs == 4 && num_outputs == 1 && pos < num_inputs + num_outputs);
  bool valid;

  const std::int32_t INPUT_FEATURES_INDEX = 0;
  const std::int32_t INPUT_GEOM_FEATURES_INDEX = 1;
  const std::int32_t INPUT_INTERVAL_LENGTH_INDEX = 2;
  const std::int32_t INPUT_INTERVAL_START_INDEX = 3;
  const std::int32_t OUTPUT_INDEX = 4;

  switch (pos) {
    case INPUT_FEATURES_INDEX:
    case OUTPUT_INDEX:
      valid = in_out[pos].desc.format == nvinfer1::TensorFormat::kLINEAR &&
              in_out[pos].desc.type == nvinfer1::DataType::kFLOAT;
      break;
    case INPUT_GEOM_FEATURES_INDEX:
    case INPUT_INTERVAL_LENGTH_INDEX:
    case INPUT_INTERVAL_START_INDEX:
      valid = in_out[pos].desc.format == nvinfer1::TensorFormat::kLINEAR &&
              in_out[pos].desc.type == nvinfer1::DataType::kINT32;
      break;
    default:
      valid = false;
      break;
  }

  return valid;
}

std::int32_t QuickCumsumCudaPlugin::getOutputDataTypes(
  DataType * output_types, std::int32_t num_outputs, DataType const * input_types,
  std::int32_t num_inputs) const noexcept
{
  PLUGIN_ASSERT(num_inputs == 4);
  PLUGIN_ASSERT(num_outputs == 1);
  output_types[0] = input_types[0];
  return 0;
}

std::int32_t QuickCumsumCudaPlugin::getOutputShapes(
  DimsExprs const * inputs, std::int32_t num_inputs,
  [[maybe_unused]] DimsExprs const * shape_inputs, [[maybe_unused]] std::int32_t num_shape_inputs,
  DimsExprs * outputs, std::int32_t num_outputs, IExprBuilder & expr_builder) noexcept
{
  PLUGIN_ASSERT(num_inputs == 4);
  PLUGIN_ASSERT(num_outputs == 1);
  PLUGIN_ASSERT(inputs != nullptr);
  PLUGIN_ASSERT(inputs[0].nbDims == 2);

  outputs[0].nbDims = 5;
  outputs[0].d[0] = expr_builder.constant(static_cast<std::int32_t>(params_.batch_size));
  outputs[0].d[1] = expr_builder.constant(static_cast<std::int32_t>(params_.dimension));
  outputs[0].d[2] = expr_builder.constant(static_cast<std::int32_t>(params_.height));
  outputs[0].d[3] = expr_builder.constant(static_cast<std::int32_t>(params_.width));
  outputs[0].d[4] = inputs[0].d[1];

  return 0;
}

std::int32_t QuickCumsumCudaPlugin::enqueue(
  PluginTensorDesc const * input_desc, [[maybe_unused]] PluginTensorDesc const * output_desc,
  void const * const * inputs, void * const * outputs, [[maybe_unused]] void * workspace,
  cudaStream_t stream) noexcept
{
  const auto output_features_num = input_desc[0].dims.d[0];
  const auto output_features_dim = input_desc[0].dims.d[1];
  const auto intervals_num = input_desc[2].dims.d[0];

  cudaMemsetAsync(
    outputs[0], 0,
    params_.batch_size * params_.dimension * params_.height * params_.width * output_features_dim *
      sizeof(float),
    stream);

  bev_pool(
    static_cast<std::int32_t>(params_.batch_size), static_cast<std::int32_t>(params_.dimension),
    static_cast<std::int32_t>(params_.height), static_cast<std::int32_t>(params_.width),
    output_features_num, output_features_dim, intervals_num, static_cast<const float *>(inputs[0]),
    static_cast<const std::int32_t *>(inputs[1]), static_cast<const std::int32_t *>(inputs[3]),
    static_cast<const std::int32_t *>(inputs[2]), static_cast<float *>(outputs[0]), stream);

  return cudaSuccess;
}

std::int32_t QuickCumsumCudaPlugin::onShapeChange(
  [[maybe_unused]] PluginTensorDesc const * in, [[maybe_unused]] std::int32_t num_inputs,
  [[maybe_unused]] PluginTensorDesc const * out, [[maybe_unused]] std::int32_t num_outputs) noexcept
{
  return 0;
}

IPluginV3 * QuickCumsumCudaPlugin::attachToContext(
  [[maybe_unused]] IPluginResourceContext * context) noexcept
{
  return clone();
}

PluginFieldCollection const * QuickCumsumCudaPlugin::getFieldsToSerialize() noexcept
{
  return &fc_to_serialize_;
}

std::size_t QuickCumsumCudaPlugin::getWorkspaceSize(
  [[maybe_unused]] DynamicPluginTensorDesc const * inputs, [[maybe_unused]] std::int32_t num_inputs,
  [[maybe_unused]] DynamicPluginTensorDesc const * outputs,
  [[maybe_unused]] std::int32_t num_outputs) const noexcept
{
  return 0;
}

}  // namespace plugin
}  // namespace nvinfer1

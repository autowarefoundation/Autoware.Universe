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

#include "autoware/tensorrt_plugins/quick_cumsum_cuda_plugin_creator.hpp"

#include "autoware/tensorrt_plugins/plugin_utils.hpp"
#include "autoware/tensorrt_plugins/quick_cumsum_cuda_plugin.hpp"

#include <NvInferRuntimePlugin.h>

#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <mutex>
#include <sstream>

namespace nvinfer1
{
namespace plugin
{

REGISTER_TENSORRT_PLUGIN(QuickCumsumCudaPluginCreator);

QuickCumsumCudaPluginCreator::QuickCumsumCudaPluginCreator()
{
  plugin_attributes_.clear();
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("batch_size", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("dimension", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("height", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("width", nullptr, PluginFieldType::kINT32, 1));

  fc_.nbFields = plugin_attributes_.size();
  fc_.fields = plugin_attributes_.data();
}

nvinfer1::PluginFieldCollection const * QuickCumsumCudaPluginCreator::getFieldNames() noexcept
{
  // This is only used in the build phase.
  return &fc_;
}

IPluginV3 * QuickCumsumCudaPluginCreator::createPlugin(
  char const * name, PluginFieldCollection const * fc, TensorRTPhase phase) noexcept
{
  if (phase == TensorRTPhase::kBUILD || phase == TensorRTPhase::kRUNTIME) {
    try {
      nvinfer1::PluginField const * fields{fc->fields};
      std::int32_t num_fields{fc->nbFields};

      PLUGIN_VALIDATE(num_fields == 4);

      QuickCumsumCudaParameters parameters;

      for (std::int32_t i{0}; i < num_fields; ++i) {
        const std::string attr_name = fields[i].name;
        const nvinfer1::PluginFieldType type = fields[i].type;

        if (attr_name == "batch_size") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.batch_size = *static_cast<std::int32_t const *>(fields[i].data);
        } else if (attr_name == "dimension") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.dimension = *static_cast<std::int32_t const *>(fields[i].data);
        } else if (attr_name == "height") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.height = *static_cast<std::int32_t const *>(fields[i].data);
        } else if (attr_name == "width") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.width = *static_cast<std::int32_t const *>(fields[i].data);
        }
      }

      // Log the attributes parsed from ONNX node.
      std::stringstream ss;
      ss << name << " plugin Attributes:";
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "batch_size: " << parameters.batch_size;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "dimension: " << parameters.dimension;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "height: " << parameters.height;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "width: " << parameters.width;
      logDebug(ss.str().c_str());

      QuickCumsumCudaPlugin * const plugin{
        new QuickCumsumCudaPlugin{std::string(name), parameters}};
      return plugin;
    } catch (std::exception const & e) {
      caughtError(e);
    }
    return nullptr;
  } else if (phase == TensorRTPhase::kRUNTIME) {
    // The attributes from the serialized plugin will be passed via fc.
    try {
      nvinfer1::PluginField const * fields{fc->fields};
      std::int32_t num_fields{fc->nbFields};
      PLUGIN_VALIDATE(num_fields == 1);

      char const * attr_name = fields[0].name;
      PLUGIN_VALIDATE(!strcmp(attr_name, "parameters"));
      PLUGIN_VALIDATE(fields[0].type == nvinfer1::PluginFieldType::kUNKNOWN);
      PLUGIN_VALIDATE(fields[0].length == sizeof(QuickCumsumCudaParameters));
      QuickCumsumCudaParameters params{
        *(static_cast<QuickCumsumCudaParameters const *>(fields[0].data))};

      QuickCumsumCudaPlugin * const plugin{new QuickCumsumCudaPlugin{std::string(name), params}};
      return plugin;
    } catch (std::exception const & e) {
      caughtError(e);
    }
    return nullptr;
  } else {
    return nullptr;
  }
}

}  // namespace plugin
}  // namespace nvinfer1

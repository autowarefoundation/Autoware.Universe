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

#include "autoware/tensorrt_plugins/implicit_gemm_plugin_creator.hpp"

#include "autoware/tensorrt_plugins/implicit_gemm_plugin.hpp"
#include "autoware/tensorrt_plugins/plugin_utils.hpp"

#include <NvInferRuntimePlugin.h>

#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace nvinfer1
{
namespace plugin
{

REGISTER_TENSORRT_PLUGIN(ImplicitGemmPluginCreator);

ImplicitGemmPluginCreator::ImplicitGemmPluginCreator()
{
  std::cout << "ImplicitGemmPluginCreator::ImplicitGemmPluginCreator" << std::endl << std::flush;

  plugin_attributes_.clear();
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("act_alpha", nullptr, PluginFieldType::kFLOAT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("act_beta", nullptr, PluginFieldType::kFLOAT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("is_subm", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("is_train", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("output_add_scale", nullptr, PluginFieldType::kFLOAT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("output_scale", nullptr, PluginFieldType::kFLOAT32, 1));

  fc_.nbFields = plugin_attributes_.size();
  fc_.fields = plugin_attributes_.data();
}

nvinfer1::PluginFieldCollection const * ImplicitGemmPluginCreator::getFieldNames() noexcept
{
  // This is only used in the build phase.
  return &fc_;
}

IPluginV3 * ImplicitGemmPluginCreator::createPlugin(
  char const * name, PluginFieldCollection const * fc, TensorRTPhase phase) noexcept
{
  // The build phase and the deserialization phase are handled differently.
  if (phase == TensorRTPhase::kBUILD || phase == TensorRTPhase::kRUNTIME) {
    // The attributes from the ONNX node will be parsed and passed via fc.
    try {
      nvinfer1::PluginField const * fields{fc->fields};
      std::int32_t num_fields{fc->nbFields};

      PLUGIN_VALIDATE(num_fields == 6);

      ImplicitGemmParameters parameters;

      for (std::int32_t i{0}; i < num_fields; ++i) {
        const std::string attr_name = fields[i].name;
        const nvinfer1::PluginFieldType type = fields[i].type;

        if (attr_name == "act_alpha") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kFLOAT32);
          parameters.act_alpha = static_cast<float const *>(fields[i].data)[0];
        }

        if (attr_name == "act_beta") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kFLOAT32);
          parameters.act_beta = static_cast<float const *>(fields[i].data)[0];
        }

        if (attr_name == "is_subm") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.is_subm = static_cast<std::int32_t const *>(fields[i].data)[0];
        }

        if (attr_name == "is_train") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.is_train = static_cast<std::int32_t const *>(fields[i].data)[0];
        }

        if (attr_name == "output_add_scale") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kFLOAT32);
          parameters.output_add_scale = static_cast<float const *>(fields[i].data)[0];
        }

        if (attr_name == "output_scale") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kFLOAT32);
          parameters.output_scale = static_cast<float const *>(fields[i].data)[0];
        }
      }

      // Log the attributes parsed from ONNX node.
      std::stringstream ss;
      ss << name << " plugin Attributes:";
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "act_alpha: " << parameters.act_alpha;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "act_beta: " << parameters.act_beta;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "is_subm: " << parameters.is_subm;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "is_train: " << parameters.is_train;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "output_add_scale: " << parameters.output_add_scale;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "output_scale: " << parameters.output_scale;
      logDebug(ss.str().c_str());

      ImplicitGemmPlugin * const plugin{new ImplicitGemmPlugin{std::string(name), parameters}};
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
      PLUGIN_VALIDATE(fields[0].length == sizeof(ImplicitGemmParameters));
      ImplicitGemmParameters params{*(static_cast<ImplicitGemmParameters const *>(fields[0].data))};

      ImplicitGemmPlugin * const plugin{new ImplicitGemmPlugin{std::string(name), params}};
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

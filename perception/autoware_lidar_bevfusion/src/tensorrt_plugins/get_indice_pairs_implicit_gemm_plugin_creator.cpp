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

#include "autoware/tensorrt_plugins//get_indice_pairs_implicit_gemm_plugin_creator.hpp"

#include "autoware/tensorrt_plugins//get_indice_pairs_implicit_gemm_plugin.hpp"
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

REGISTER_TENSORRT_PLUGIN(GetIndicePairsImplicitGemmPluginCreator);

GetIndicePairsImplicitGemmPluginCreator::GetIndicePairsImplicitGemmPluginCreator()
{
  std::cout << "GetIndicePairsImplicitGemmPluginCreator::GetIndicePairsImplicitGemmPluginCreator"
            << std::endl
            << std::flush;

  plugin_attributes_.clear();
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("algo", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("batch_size", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("dilation", nullptr, PluginFieldType::kINT32, 3));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("is_train", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("ksize", nullptr, PluginFieldType::kINT32, 3));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("out_padding", nullptr, PluginFieldType::kINT32, 3));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("padding", nullptr, PluginFieldType::kINT32, 3));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("spatial_shape", nullptr, PluginFieldType::kINT32, 3));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("stride", nullptr, PluginFieldType::kINT32, 3));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("subm", nullptr, PluginFieldType::kINT32, 1));
  plugin_attributes_.emplace_back(
    nvinfer1::PluginField("transpose", nullptr, PluginFieldType::kINT32, 1));

  fc_.nbFields = plugin_attributes_.size();
  fc_.fields = plugin_attributes_.data();
}

nvinfer1::PluginFieldCollection const *
GetIndicePairsImplicitGemmPluginCreator::getFieldNames() noexcept
{
  // This is only used in the build phase.
  return &fc_;
}

IPluginV3 * GetIndicePairsImplicitGemmPluginCreator::createPlugin(
  char const * name, PluginFieldCollection const * fc, TensorRTPhase phase) noexcept
{
  // The build phase and the deserialization phase are handled differently.
  if (phase == TensorRTPhase::kBUILD || phase == TensorRTPhase::kRUNTIME) {
    // The attributes from the ONNX node will be parsed and passed via fc.
    try {
      nvinfer1::PluginField const * fields{fc->fields};
      std::int32_t num_fields{fc->nbFields};

      PLUGIN_VALIDATE(num_fields == 11);

      GetIndicePairsImplicitGemmParameters parameters;

      for (std::int32_t i{0}; i < num_fields; ++i) {
        const std::string attr_name = fields[i].name;
        const nvinfer1::PluginFieldType type = fields[i].type;

        if (attr_name == "batch_size") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.batch_size = static_cast<std::int32_t const *>(fields[i].data)[0];
        }
        if (attr_name == "algo") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.algo = static_cast<std::int32_t const *>(fields[i].data)[0];
        }
        if (attr_name == "is_train") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.is_train = static_cast<std::int32_t const *>(fields[i].data)[0];
        }
        if (attr_name == "dilation") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          std::int32_t const * const dilation_data{
            static_cast<std::int32_t const *>(fields[i].data)};

          parameters.dilation_dims.nbDims = fields[i].length;
          for (std::int32_t j{0}; j < fields[i].length; ++j) {
            parameters.dilation.push_back(dilation_data[j]);
            parameters.dilation_dims.d[j] = static_cast<std::int64_t>(dilation_data[j]);
          }
        }
        if (attr_name == "dilation_dims") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kDIMS);
          parameters.dilation_dims = static_cast<nvinfer1::Dims const *>(fields[i].data)[0];
          for (std::int32_t j{0}; j < parameters.dilation_dims.nbDims; ++j) {
            parameters.dilation.push_back(static_cast<std::int32_t>(parameters.dilation_dims.d[j]));
          }
        }
        if (attr_name == "ksize") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          std::int32_t const * const ksize_data{static_cast<std::int32_t const *>(fields[i].data)};
          parameters.ksize_dims.nbDims = fields[i].length;
          for (std::int32_t j{0}; j < fields[i].length; ++j) {
            parameters.ksize.push_back(ksize_data[j]);
            parameters.ksize_dims.d[j] = static_cast<std::int64_t>(ksize_data[j]);
          }
        }
        if (attr_name == "ksize_dims") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kDIMS);
          parameters.ksize_dims = static_cast<nvinfer1::Dims const *>(fields[i].data)[0];
          for (std::int32_t j{0}; j < parameters.ksize_dims.nbDims; ++j) {
            parameters.ksize.push_back(static_cast<std::int32_t>(parameters.ksize_dims.d[j]));
          }
        }
        if (attr_name == "out_padding") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          std::int32_t const * const out_padding_data{
            static_cast<std::int32_t const *>(fields[i].data)};
          parameters.out_padding_dims.nbDims = fields[i].length;
          for (std::int32_t j{0}; j < fields[i].length; ++j) {
            parameters.out_padding.push_back(out_padding_data[j]);
            parameters.out_padding_dims.d[j] = static_cast<std::int64_t>(out_padding_data[j]);
          }
        }
        if (attr_name == "out_padding_dims") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kDIMS);
          parameters.out_padding_dims = static_cast<nvinfer1::Dims const *>(fields[i].data)[0];
          for (std::int32_t j{0}; j < parameters.out_padding_dims.nbDims; ++j) {
            parameters.out_padding.push_back(
              static_cast<std::int32_t>(parameters.out_padding_dims.d[j]));
          }
        }
        if (attr_name == "padding") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          std::int32_t const * const padding_data{
            static_cast<std::int32_t const *>(fields[i].data)};
          parameters.padding_dims.nbDims = fields[i].length;
          for (std::int32_t j{0}; j < fields[i].length; ++j) {
            parameters.padding.push_back(padding_data[j]);
            parameters.padding_dims.d[j] = static_cast<std::int64_t>(padding_data[j]);
          }
        }
        if (attr_name == "padding_dims") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kDIMS);
          parameters.padding_dims = static_cast<nvinfer1::Dims const *>(fields[i].data)[0];
          for (std::int32_t j{0}; j < parameters.padding_dims.nbDims; ++j) {
            parameters.padding.push_back(static_cast<std::int32_t>(parameters.padding_dims.d[j]));
          }
        }
        if (attr_name == "spatial_shape") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          std::int32_t const * const spatial_shape_data{
            static_cast<std::int32_t const *>(fields[i].data)};
          parameters.spatial_shape_dims.nbDims = fields[i].length;
          for (std::int32_t j{0}; j < fields[i].length; ++j) {
            parameters.spatial_shape.push_back(spatial_shape_data[j]);
            parameters.spatial_shape_dims.d[j] = static_cast<std::int64_t>(spatial_shape_data[j]);
          }
        }
        if (attr_name == "spatial_shape_dims") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kDIMS);
          parameters.spatial_shape_dims = static_cast<nvinfer1::Dims const *>(fields[i].data)[0];
          for (std::int32_t j{0}; j < parameters.spatial_shape_dims.nbDims; ++j) {
            parameters.spatial_shape.push_back(
              static_cast<std::int32_t>(parameters.spatial_shape_dims.d[j]));
          }
        }
        if (attr_name == "stride") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          std::int32_t const * const stride_data{static_cast<std::int32_t const *>(fields[i].data)};
          parameters.stride_dims.nbDims = fields[i].length;
          for (std::int32_t j{0}; j < fields[i].length; ++j) {
            parameters.stride.push_back(stride_data[j]);
            parameters.stride_dims.d[j] = static_cast<std::int64_t>(stride_data[j]);
          }
        }
        if (attr_name == "stride_dims") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kDIMS);
          parameters.stride_dims = static_cast<nvinfer1::Dims const *>(fields[i].data)[0];
          for (std::int32_t j{0}; j < parameters.stride_dims.nbDims; ++j) {
            parameters.stride.push_back(static_cast<std::int32_t>(parameters.stride_dims.d[j]));
          }
        }
        if (attr_name == "subm") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.subm = static_cast<std::int32_t const *>(fields[i].data)[0];
        }
        if (attr_name == "transpose") {
          PLUGIN_VALIDATE(type == nvinfer1::PluginFieldType::kINT32);
          parameters.transpose = static_cast<std::int32_t const *>(fields[i].data)[0];
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
      ss << "algo: " << parameters.algo;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "is_train: " << parameters.is_train;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "dilation: ";
      for (auto const & val : parameters.dilation) {
        ss << val << " ";
      }
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "ksize: ";
      for (auto const & val : parameters.ksize) {
        ss << val << " ";
      }
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "out_padding: ";
      for (auto const & val : parameters.out_padding) {
        ss << val << " ";
      }
      logDebug(ss.str().c_str());

      logDebug(ss.str().c_str());

      ss.str("");
      ss << "padding: ";
      for (auto const & val : parameters.padding) {
        ss << val << " ";
      }
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "spatial_shape: ";
      for (auto const & val : parameters.spatial_shape) {
        ss << val << " ";
      }
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "stride: ";
      for (auto const & val : parameters.stride) {
        ss << val << " ";
      }
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "subm: " << parameters.subm;
      logDebug(ss.str().c_str());

      ss.str("");
      ss << "transpose: " << parameters.transpose;
      logDebug(ss.str().c_str());

      GetIndicePairsImplicitGemmPlugin * const plugin{
        new GetIndicePairsImplicitGemmPlugin{std::string(name), parameters}};
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
      PLUGIN_VALIDATE(fields[0].length == sizeof(GetIndicePairsImplicitGemmParameters));
      GetIndicePairsImplicitGemmParameters params{
        *(static_cast<GetIndicePairsImplicitGemmParameters const *>(fields[0].data))};

      GetIndicePairsImplicitGemmPlugin * const plugin{
        new GetIndicePairsImplicitGemmPlugin{std::string(name), params}};
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

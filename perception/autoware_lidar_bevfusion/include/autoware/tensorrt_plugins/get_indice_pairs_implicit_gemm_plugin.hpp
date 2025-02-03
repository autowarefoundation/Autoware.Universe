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

#ifndef AUTOWARE__TENSORRT_PLUGINS__GET_INDICE_PAIRS_IMPLICIT_GEMM_PLUGIN_HPP_
#define AUTOWARE__TENSORRT_PLUGINS__GET_INDICE_PAIRS_IMPLICIT_GEMM_PLUGIN_HPP_

#include <NvInferRuntime.h>
#include <NvInferRuntimePlugin.h>
#include <cuda_runtime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

constexpr char const * const kGET_INDICE_PAIRS_IMPLICIT_GEMM_PLUGIN_NAME{
  "GetIndicePairsImplicitGemm"};
constexpr char const * const kGET_INDICE_PAIRS_IMPLICIT_GEMM_PLUGIN_VERSION{"1"};
constexpr char const * const kGET_INDICE_PAIRS_IMPLICIT_GEMM_PLUGIN_NAMESPACE{""};

namespace nvinfer1
{
namespace plugin
{

struct GetIndicePairsImplicitGemmParameters
{
  std::int32_t batch_size;
  std::int32_t algo;
  std::int32_t is_train;
  std::vector<std::int32_t> dilation;
  std::vector<std::int32_t> ksize;
  std::vector<std::int32_t> out_padding;
  std::vector<std::int32_t> padding;
  std::vector<std::int32_t> spatial_shape;
  std::vector<std::int32_t> stride;
  std::int32_t subm;
  std::int32_t transpose;

  nvinfer1::Dims dilation_dims;
  nvinfer1::Dims ksize_dims;
  nvinfer1::Dims out_padding_dims;
  nvinfer1::Dims padding_dims;
  nvinfer1::Dims spatial_shape_dims;
  nvinfer1::Dims stride_dims;
};

class GetIndicePairsImplicitGemmPlugin : public IPluginV3,
                                         public IPluginV3OneCore,
                                         public IPluginV3OneBuild,
                                         public IPluginV3OneRuntime
{
public:
  GetIndicePairsImplicitGemmPlugin(
    const std::string & name, GetIndicePairsImplicitGemmParameters const & params);

  ~GetIndicePairsImplicitGemmPlugin() override = default;

  // IPluginV3 Methods

  IPluginCapability * getCapabilityInterface(PluginCapabilityType type) noexcept override;

  IPluginV3 * clone() noexcept override;

  // IPluginV3OneCore Methods

  char const * getPluginName() const noexcept override;

  char const * getPluginVersion() const noexcept override;

  char const * getPluginNamespace() const noexcept override;

  // IPluginV3OneBuild Methods

  std::int32_t getNbOutputs() const noexcept override;

  std::int32_t configurePlugin(
    DynamicPluginTensorDesc const * in, std::int32_t num_inputs,
    DynamicPluginTensorDesc const * out, std::int32_t num_outputs) noexcept override;

  bool supportsFormatCombination(
    std::int32_t pos, DynamicPluginTensorDesc const * in_out, std::int32_t num_inputs,
    std::int32_t num_outputs) noexcept override;

  std::int32_t getOutputDataTypes(
    DataType * output_types, std::int32_t num_outputs, DataType const * input_types,
    std::int32_t num_inputs) const noexcept override;

  std::int32_t getOutputShapes(
    DimsExprs const * inputs, std::int32_t num_inputs, DimsExprs const * shape_inputs,
    std::int32_t num_shape_inputs, DimsExprs * outputs, std::int32_t num_outputs,
    IExprBuilder & expr_builder) noexcept override;

  // IPluginV3OneRuntime Methods

  std::int32_t enqueue(
    PluginTensorDesc const * input_desc, PluginTensorDesc const * output_desc,
    void const * const * inputs, void * const * outputs, void * workspace,
    cudaStream_t stream) noexcept override;

  std::int32_t onShapeChange(
    PluginTensorDesc const * in, std::int32_t num_inputs, PluginTensorDesc const * out,
    std::int32_t num_outputs) noexcept override;

  IPluginV3 * attachToContext(IPluginResourceContext * context) noexcept override;

  PluginFieldCollection const * getFieldsToSerialize() noexcept override;

  std::size_t getWorkspaceSize(
    DynamicPluginTensorDesc const * inputs, std::int32_t num_inputs,
    DynamicPluginTensorDesc const * outputs, std::int32_t num_outputs) const noexcept override;

private:
  void initFieldsToSerialize();

  // upper bound of number of output indices. needed to bound memory usage.
  static constexpr int out_inds_num_limit_{256000};

  std::string layer_name_;
  GetIndicePairsImplicitGemmParameters params_;
  std::vector<nvinfer1::PluginField> data_to_serialize_;
  nvinfer1::PluginFieldCollection fc_to_serialize_;
};

}  // namespace plugin
}  // namespace nvinfer1

#endif  // AUTOWARE__TENSORRT_PLUGINS__GET_INDICE_PAIRS_IMPLICIT_GEMM_PLUGIN_HPP_

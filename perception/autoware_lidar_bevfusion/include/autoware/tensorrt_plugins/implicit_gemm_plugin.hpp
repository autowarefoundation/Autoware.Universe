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

#ifndef AUTOWARE__TENSORRT_PLUGINS__IMPLICIT_GEMM_PLUGIN_HPP_
#define AUTOWARE__TENSORRT_PLUGINS__IMPLICIT_GEMM_PLUGIN_HPP_

#include <NvInferRuntime.h>
#include <NvInferRuntimePlugin.h>
#include <cuda_runtime.h>
#include <spconvlib/spconv/csrc/sparse/convops/gemmops/GemmTunerSimple.h>
#include <spconvlib/spconv/csrc/sparse/convops/spops/ConvGemmOps.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

constexpr char const * const kIMPLICIT_GEMM_PLUGIN_NAME{"ImplicitGemm"};
constexpr char const * const kIMPLICIT_GEMM_PLUGIN_VERSION{"1"};
constexpr char const * const kIMPLICIT_GEMM_PLUGIN_NAMESPACE{""};

namespace nvinfer1
{
namespace plugin
{

struct ImplicitGemmParameters
{
  float act_alpha;
  float act_beta;
  std::int64_t is_subm;
  std::int64_t is_train;
  float output_add_scale;
  float output_scale;
};

class ImplicitGemmPlugin : public IPluginV3,
                           public IPluginV3OneCore,
                           public IPluginV3OneBuild,
                           public IPluginV3OneRuntime
{
public:
  using ConvTunerSimple = spconvlib::spconv::csrc::sparse::convops::spops::ConvTuner;
  ImplicitGemmPlugin(const std::string & name, ImplicitGemmParameters const & params);

  ~ImplicitGemmPlugin() override = default;

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
  static constexpr std::int32_t INOUT_IN_FEATURES_INDEX = 0;
  static constexpr std::int32_t INOUT_FILTERS_INDEX = 1;
  static constexpr std::int32_t INOUT_PAIR_FWD_INDEX = 2;
  static constexpr std::int32_t INOUT_PAIR_MASK_FWD_SPLITS_INDEX = 3;
  static constexpr std::int32_t INOUT_MASK_ARGSORT_FWD_SPLITS_INDEX = 4;
  static constexpr std::int32_t INOUT_OUT_FEATURES_INDEX = 5;

  void initFieldsToSerialize();

  std::string layer_name_;
  ImplicitGemmParameters params_;
  std::tuple<int, int> arch_;
  std::vector<nvinfer1::PluginField> data_to_serialize_;
  nvinfer1::PluginFieldCollection fc_to_serialize_;

  std::unique_ptr<ConvTunerSimple> tunner_ptr_{};
};

}  // namespace plugin
}  // namespace nvinfer1

#endif  // AUTOWARE__TENSORRT_PLUGINS__IMPLICIT_GEMM_PLUGIN_HPP_

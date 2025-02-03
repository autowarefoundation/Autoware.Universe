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

#ifndef ATTENTION__TRT_ATTN_WEIGHT_COMPUTATION_HPP_
#define ATTENTION__TRT_ATTN_WEIGHT_COMPUTATION_HPP_

#include "common/trt_plugin_base.hpp"

#include <string>

namespace autoware::trt_mtr
{
/**
 * @brief The function to compute attention weight.
 *
 * @param queryBatchCnt <int; [B]>
 * @param keyBatchCnt <int; [B]>
 * @param indexPairBatch <int; [Q]>
 * @param indexPair <int; [Q, L]>
 * @param queryFeature <float; [Q, numHead, headDim]>
 * @param keyFeature <float; [Q, numHead, headDim]>
 * @param output <float; [Q, L, numHead]>
 */
class AttentionWeightComputation : public TRTPluginBase
{
public:
  explicit AttentionWeightComputation(const std::string & name);
  AttentionWeightComputation(const std::string & name, const void * data, size_t length);
  ~AttentionWeightComputation() TRT_NOEXCEPT override;

  /* IPluginV2DynamicExt methods */
  nvinfer1::IPluginV2DynamicExt * clone() const TRT_NOEXCEPT override;

  nvinfer1::DimsExprs getOutputDimensions(
    int outputIndex, const nvinfer1::DimsExprs * inputs, int nbInputs,
    nvinfer1::IExprBuilder & exprBuilder) TRT_NOEXCEPT override;

  bool supportsFormatCombination(
    int pos, const nvinfer1::PluginTensorDesc * ioDesc, int nbInputs,
    int nbOutputs) TRT_NOEXCEPT override;

  void configurePlugin(
    const nvinfer1::DynamicPluginTensorDesc * inDesc, int nbInputs,
    const nvinfer1::DynamicPluginTensorDesc * outDesc, int nbOutputs) TRT_NOEXCEPT override;

  size_t getWorkspaceSize(
    const nvinfer1::PluginTensorDesc * inDesc, int nbInputs,
    const nvinfer1::PluginTensorDesc * outDesc, int nbOutputs) const TRT_NOEXCEPT override;

  int enqueue(
    const nvinfer1::PluginTensorDesc * inDesc, const nvinfer1::PluginTensorDesc * outDesc,
    const void * const * inputs, void * const * outputs, void * workspace,
    cudaStream_t stream) TRT_NOEXCEPT override;

  void attachToContext(
    cudnnContext * cudnnCtx, cublasContext * cublasCtx,
    nvinfer1::IGpuAllocator * gpuAllocator) TRT_NOEXCEPT override;

  void detachFromContext() TRT_NOEXCEPT override;

  /* IPluginV2Ext methods */
  nvinfer1::DataType getOutputDataType(
    int index, const nvinfer1::DataType * inTypes, int nbInputs) const TRT_NOEXCEPT override;

  /* IPluginV2 methods */
  const char * getPluginType() const TRT_NOEXCEPT override;
  const char * getPluginVersion() const TRT_NOEXCEPT override;
  int getNbOutputs() const TRT_NOEXCEPT override;
  size_t getSerializationSize() const TRT_NOEXCEPT override;
  void serialize(void * buffer) const TRT_NOEXCEPT override;
};  // class AttentionWeightComputation

class AttentionWeightComputationCreator : public TRTPluginCreatorBase
{
public:
  AttentionWeightComputationCreator();

  const char * getPluginName() const TRT_NOEXCEPT override;

  const char * getPluginVersion() const TRT_NOEXCEPT override;

  nvinfer1::IPluginV2DynamicExt * createPlugin(
    const char * name, const nvinfer1::PluginFieldCollection * fc) TRT_NOEXCEPT override;

  nvinfer1::IPluginV2DynamicExt * deserializePlugin(
    const char * name, const void * serialData, size_t serialLength) TRT_NOEXCEPT override;
};  // class AttentionWeightComputationCreator
REGISTER_TENSORRT_PLUGIN(AttentionWeightComputationCreator);
}  // namespace autoware::trt_mtr
#endif  // ATTENTION__TRT_ATTN_WEIGHT_COMPUTATION_HPP_

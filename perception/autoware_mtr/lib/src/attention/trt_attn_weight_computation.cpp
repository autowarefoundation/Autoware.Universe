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

#include "attention/trt_attn_weight_computation.hpp"

#include "attention/trt_attn_weight_computation_kernel.hpp"

#include <string>

namespace autoware::trt_mtr
{
namespace
{
static const char * PLUGIN_VERSION{"1"};
static const char * PLUGIN_NAME{"TRTAttentionWeightComputation"};
}  // namespace

AttentionWeightComputation::AttentionWeightComputation(const std::string & name)
: TRTPluginBase(name)
{
}

AttentionWeightComputation::AttentionWeightComputation(
  const std::string & name, const void *, size_t)
: TRTPluginBase(name)
{
}

AttentionWeightComputation::~AttentionWeightComputation() TRT_NOEXCEPT
{
}

nvinfer1::IPluginV2DynamicExt * AttentionWeightComputation::clone() const TRT_NOEXCEPT
{
  auto * plugin = new AttentionWeightComputation(mLayerName);
  plugin->setPluginNamespace(getPluginNamespace());
  return plugin;
}

nvinfer1::DimsExprs AttentionWeightComputation::getOutputDimensions(
  int, const nvinfer1::DimsExprs * inputs, int, nvinfer1::IExprBuilder &) TRT_NOEXCEPT
{
  nvinfer1::DimsExprs ret;
  ret.nbDims = 3;
  ret.d[0] = inputs[3].d[0];
  ret.d[1] = inputs[3].d[1];
  ret.d[2] = inputs[5].d[1];

  return ret;
}

bool AttentionWeightComputation::supportsFormatCombination(
  int pos, const nvinfer1::PluginTensorDesc * ioDesc, int, int) TRT_NOEXCEPT
{
  return ioDesc[pos].format == nvinfer1::TensorFormat::kLINEAR &&
         ioDesc[pos].type == (pos < 4 ? nvinfer1::DataType::kINT32 : nvinfer1::DataType::kFLOAT);
}

void AttentionWeightComputation::configurePlugin(
  const nvinfer1::DynamicPluginTensorDesc * inDesc, int nbInputs,
  const nvinfer1::DynamicPluginTensorDesc * outDesc, int nbOutputs) TRT_NOEXCEPT
{
  // Validate input arguments
  PLUGIN_ASSERT(nbInputs == 6);
  for (int pos = 0; pos < 6; ++pos) {
    PLUGIN_ASSERT(
      inDesc[pos].desc.format == nvinfer1::TensorFormat::kLINEAR &&
      inDesc[pos].desc.type == (pos < 4 ? nvinfer1::DataType::kINT32 : nvinfer1::DataType::kFLOAT));
  }

  PLUGIN_ASSERT(nbOutputs == 1);
  PLUGIN_ASSERT(
    outDesc[0].desc.format == nvinfer1::TensorFormat::kLINEAR &&
    outDesc[0].desc.type == nvinfer1::DataType::kFLOAT);
}

size_t AttentionWeightComputation::getWorkspaceSize(
  const nvinfer1::PluginTensorDesc *, int, const nvinfer1::PluginTensorDesc *,
  int) const TRT_NOEXCEPT
{
  return 0;
}

int AttentionWeightComputation::enqueue(
  const nvinfer1::PluginTensorDesc * inDesc, const nvinfer1::PluginTensorDesc * outDesc,
  const void * const * inputs, void * const * outputs, void *, cudaStream_t stream) TRT_NOEXCEPT
{
  // parse query_batch_cnt description
  const int32_t B = inDesc[0].dims.d[0];

  // parse index_pair description
  const int32_t Q = inDesc[3].dims.d[0];
  const int32_t L = inDesc[3].dims.d[1];

  // parse key_features description
  const int32_t K = inDesc[5].dims.d[0];
  const int32_t numHead = inDesc[5].dims.d[1];
  const int32_t headDim = inDesc[5].dims.d[2];

  const void * queryBatchCnt = inputs[0];
  const void * keyBatchCnt = inputs[1];
  const void * indexPairBatch = inputs[2];
  const void * indexPair = inputs[3];
  const void * queryFeature = inputs[4];
  const void * keyFeature = inputs[5];

  void * output = outputs[0];

  switch (outDesc[0].type) {
    case nvinfer1::DataType::kFLOAT:
      AttentionWeightComputationLauncher(
        B, Q, L, K, numHead, headDim, reinterpret_cast<const int *>(queryBatchCnt),
        reinterpret_cast<const int *>(keyBatchCnt), reinterpret_cast<const int *>(indexPairBatch),
        reinterpret_cast<const int *>(indexPair), reinterpret_cast<const float *>(queryFeature),
        reinterpret_cast<const float *>(keyFeature), reinterpret_cast<float *>(output), stream);
      break;

    default:
      break;
  }

  return 0;
}

void AttentionWeightComputation::attachToContext(
  cudnnContext *, cublasContext *, nvinfer1::IGpuAllocator *) TRT_NOEXCEPT
{
}

void AttentionWeightComputation::detachFromContext() TRT_NOEXCEPT
{
}

nvinfer1::DataType AttentionWeightComputation::getOutputDataType(
  int, const nvinfer1::DataType * inTypes, int) const TRT_NOEXCEPT
{
  return inTypes[4];
}

const char * AttentionWeightComputation::getPluginType() const TRT_NOEXCEPT
{
  return PLUGIN_NAME;
}

const char * AttentionWeightComputation::getPluginVersion() const TRT_NOEXCEPT
{
  return PLUGIN_VERSION;
}

int AttentionWeightComputation::getNbOutputs() const TRT_NOEXCEPT
{
  return 1;
}

size_t AttentionWeightComputation::getSerializationSize() const TRT_NOEXCEPT
{
  return 0;
}

void AttentionWeightComputation::serialize(void *) const TRT_NOEXCEPT
{
}

/* ====================== creator ====================== */
AttentionWeightComputationCreator::AttentionWeightComputationCreator()
{
  mPluginAttributes.clear();
  mFC.nbFields = mPluginAttributes.size();
  mFC.fields = mPluginAttributes.data();
}

const char * AttentionWeightComputationCreator::getPluginName() const TRT_NOEXCEPT
{
  return PLUGIN_NAME;
}

const char * AttentionWeightComputationCreator::getPluginVersion() const TRT_NOEXCEPT
{
  return PLUGIN_VERSION;
}

nvinfer1::IPluginV2DynamicExt * AttentionWeightComputationCreator::createPlugin(
  const char * name, const nvinfer1::PluginFieldCollection *) TRT_NOEXCEPT
{
  auto plugin = new AttentionWeightComputation(name);
  plugin->setPluginNamespace(getPluginNamespace());
  return plugin;
}

nvinfer1::IPluginV2DynamicExt * AttentionWeightComputationCreator::deserializePlugin(
  const char * name, const void * serialData, size_t serialLength) TRT_NOEXCEPT
{
  auto plugin = new AttentionWeightComputation(name, serialData, serialLength);
  plugin->setPluginNamespace(getPluginNamespace());
  return plugin;
}

}  // namespace autoware::trt_mtr

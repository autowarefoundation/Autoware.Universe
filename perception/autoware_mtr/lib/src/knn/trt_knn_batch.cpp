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

#include "knn/trt_knn_batch.hpp"

#include "common/trt_serialize.hpp"
#include "knn/trt_knn_batch_kernel.hpp"

#include <string>

namespace autoware::trt_mtr
{
namespace
{
static const char * PLUGIN_VERSION{"1"};
static const char * PLUGIN_NAME{"TRTKnnBatch"};
}  // namespace

KnnBatch::KnnBatch(const std::string & name, const int32_t top_k)
: TRTPluginBase(name), mTopK(top_k)
{
}

KnnBatch::KnnBatch(const std::string & name, const void * data, size_t length) : TRTPluginBase(name)
{
  deserialize_value(&data, &length, &mTopK);
}

KnnBatch::~KnnBatch() TRT_NOEXCEPT
{
}

nvinfer1::IPluginV2DynamicExt * KnnBatch::clone() const TRT_NOEXCEPT
{
  auto * plugin = new KnnBatch(mLayerName, mTopK);
  plugin->setPluginNamespace(getPluginNamespace());
  return plugin;
}

nvinfer1::DimsExprs KnnBatch::getOutputDimensions(
  int, const nvinfer1::DimsExprs * inputs, int, nvinfer1::IExprBuilder & exprBuilder) TRT_NOEXCEPT
{
  nvinfer1::DimsExprs ret;
  ret.nbDims = 2;
  ret.d[0] = inputs[0].d[0];
  ret.d[1] = exprBuilder.constant(mTopK);

  return ret;
}

bool KnnBatch::supportsFormatCombination(
  int pos, const nvinfer1::PluginTensorDesc * ioDesc, int, int) TRT_NOEXCEPT
{
  return ioDesc[pos].format == nvinfer1::TensorFormat::kLINEAR &&
         ioDesc[pos].type == (pos < 2 ? nvinfer1::DataType::kFLOAT : nvinfer1::DataType::kINT32);
}

void KnnBatch::configurePlugin(
  const nvinfer1::DynamicPluginTensorDesc * inDesc, int nbInputs,
  const nvinfer1::DynamicPluginTensorDesc * outDesc, int nbOutputs) TRT_NOEXCEPT
{
  // Validate input arguments
  PLUGIN_ASSERT(nbInputs == 4);
  for (int pos = 0; pos < 4; ++pos) {
    PLUGIN_ASSERT(
      inDesc[pos].desc.format == nvinfer1::TensorFormat::kLINEAR &&
      inDesc[pos].desc.type == (pos < 2 ? nvinfer1::DataType::kFLOAT : nvinfer1::DataType::kINT32));
  }

  PLUGIN_ASSERT(nbOutputs == 1);
  PLUGIN_ASSERT(
    outDesc[0].desc.format == nvinfer1::TensorFormat::kLINEAR &&
    outDesc[0].desc.type == nvinfer1::DataType::kINT32);
}

size_t KnnBatch::getWorkspaceSize(
  const nvinfer1::PluginTensorDesc *, int, const nvinfer1::PluginTensorDesc *,
  int) const TRT_NOEXCEPT
{
  return 0;
}

int KnnBatch::enqueue(
  const nvinfer1::PluginTensorDesc * inDesc, const nvinfer1::PluginTensorDesc * outDesc,
  const void * const * inputs, void * const * outputs, void *, cudaStream_t stream) TRT_NOEXCEPT
{
  const int32_t n = inDesc[0].dims.d[0];
  const int32_t m = inDesc[1].dims.d[0];

  const void * xyz = inputs[0];
  const void * query_xyz = inputs[1];
  const void * batch_idxs = inputs[2];
  const void * query_batch_offsets = inputs[3];

  void * output = outputs[0];

  switch (outDesc[0].type) {
    case nvinfer1::DataType::kINT32:
      KnnBatchLauncher(
        n, m, mTopK, reinterpret_cast<const float *>(xyz),
        reinterpret_cast<const float *>(query_xyz), reinterpret_cast<const int *>(batch_idxs),
        reinterpret_cast<const int *>(query_batch_offsets), reinterpret_cast<int *>(output),
        stream);
      break;

    default:
      break;
  }

  return 0;
}

void KnnBatch::attachToContext(cudnnContext *, cublasContext *, nvinfer1::IGpuAllocator *)
  TRT_NOEXCEPT
{
}

void KnnBatch::detachFromContext() TRT_NOEXCEPT
{
}

nvinfer1::DataType KnnBatch::getOutputDataType(int, const nvinfer1::DataType *, int) const
  TRT_NOEXCEPT
{
  return nvinfer1::DataType::kINT32;
}

const char * KnnBatch::getPluginType() const TRT_NOEXCEPT
{
  return PLUGIN_NAME;
}

const char * KnnBatch::getPluginVersion() const TRT_NOEXCEPT
{
  return PLUGIN_VERSION;
}

int KnnBatch::getNbOutputs() const TRT_NOEXCEPT
{
  return 1;
}

size_t KnnBatch::getSerializationSize() const TRT_NOEXCEPT
{
  return sizeof(mTopK);
}

void KnnBatch::serialize(void * buffer) const TRT_NOEXCEPT
{
  serialize_value(&buffer, mTopK);
}

/* ====================== creator ====================== */
KnnBatchCreator::KnnBatchCreator()
{
  mPluginAttributes.emplace_back(
    nvinfer1::PluginField("top_k", nullptr, nvinfer1::PluginFieldType::kINT32, 1));
  mFC.nbFields = mPluginAttributes.size();
  mFC.fields = mPluginAttributes.data();
}

const char * KnnBatchCreator::getPluginName() const TRT_NOEXCEPT
{
  return PLUGIN_NAME;
}

const char * KnnBatchCreator::getPluginVersion() const TRT_NOEXCEPT
{
  return PLUGIN_VERSION;
}

nvinfer1::IPluginV2DynamicExt * KnnBatchCreator::createPlugin(
  const char * name, const nvinfer1::PluginFieldCollection * fc) TRT_NOEXCEPT
{
  const nvinfer1::PluginField * fields = fc->fields;
  int32_t top_k = 0;
  for (int i = 0; i < fc->nbFields; ++i) {
    const char * attrName = fields[i].name;
    if (!strcmp(attrName, "top_k")) {
      PLUGIN_ASSERT(fields[i].type == nvinfer1::PluginFieldType::kINT32);
      top_k = *static_cast<const int32_t *>(fields[i].data);
    }
  }
  auto plugin = new KnnBatch(name, top_k);
  plugin->setPluginNamespace(getPluginNamespace());
  return plugin;
}

nvinfer1::IPluginV2DynamicExt * KnnBatchCreator::deserializePlugin(
  const char * name, const void * serialData, size_t serialLength) TRT_NOEXCEPT
{
  auto plugin = new KnnBatch(name, serialData, serialLength);
  plugin->setPluginNamespace(getPluginNamespace());
  return plugin;
}

}  // namespace autoware::trt_mtr

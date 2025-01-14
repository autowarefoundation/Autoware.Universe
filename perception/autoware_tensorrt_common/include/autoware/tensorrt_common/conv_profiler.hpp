// Copyright 2023 TIER IV, Inc.
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

#ifndef AUTOWARE__TENSORRT_COMMON__CONV_PROFILER_HPP_
#define AUTOWARE__TENSORRT_COMMON__CONV_PROFILER_HPP_

#include <autoware/tensorrt_common/profiler.hpp>

#include <NvInfer.h>

#include <map>
#include <string>
#include <vector>

namespace autoware
{
namespace tensorrt_common
{
/**
 * @struct ConvLayerInfo
 * @brief Information of a layer.
 */
struct ConvLayerInfo
{
  //! @brief Input channel.
  int in_c;
  //! @brief Output channel.
  int out_c;
  //! @brief Width.
  int w;
  //! @brief Height.
  int h;
  //! @brief Kernel size.
  int k;
  //! @brief Stride.
  int stride;
  //! @brief Number of groups.
  int groups;
  //! @brief Layer type.
  nvinfer1::LayerType type;
};

/**
 * @class ConvProfiler
 * @brief Collect per-layer profile information, assuming times are reported in the same order.
 */
class ConvProfiler : public tensorrt_common::Profiler
{
public:
  /**
   * @brief Construct Profiler for convolutional layers.
   *
   * @param[in] src_profilers Source profilers to merge.
   */
  explicit ConvProfiler(
    const std::vector<Profiler> & src_profilers = std::vector<tensorrt_common::Profiler>())
  : Profiler(src_profilers)
  {
  }

  /**
   * @brief Set per-layer profile information for model.
   *
   * @param[in] layer Layer to set profile information.
   */
  void setProfDict(nvinfer1::ILayer * const layer) noexcept final
  {
    if (const auto type = layer->getType(); type == nvinfer1::LayerType::kCONVOLUTION) {
      const auto name = layer->getName();
      auto conv = dynamic_cast<nvinfer1::IConvolutionLayer *>(layer);

      nvinfer1::ITensor * in = layer->getInput(0);
      nvinfer1::Dims in_dim = in->getDimensions();

      nvinfer1::ITensor * out = layer->getOutput(0);
      nvinfer1::Dims out_dim = out->getDimensions();

      nvinfer1::Dims k_dims = conv->getKernelSizeNd();
      nvinfer1::Dims s_dims = conv->getStrideNd();

      int32_t kernel = k_dims.d[0];
      int32_t stride = s_dims.d[0];
      int32_t groups = conv->getNbGroups();

      layer_dict_.insert_or_assign(
        name, ConvLayerInfo{
                in_dim.d[1], out_dim.d[1], in_dim.d[3], in_dim.d[2], kernel, stride, groups, type});
    }
  }

private:
  //! @brief Per-layer information.
  std::map<std::string, ConvLayerInfo> layer_dict_;
};
}  // namespace tensorrt_common
}  // namespace autoware
#endif  // AUTOWARE__TENSORRT_COMMON__CONV_PROFILER_HPP_

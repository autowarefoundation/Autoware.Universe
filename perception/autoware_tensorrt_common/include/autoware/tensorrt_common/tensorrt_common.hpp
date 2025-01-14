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

#ifndef AUTOWARE__TENSORRT_COMMON__TENSORRT_COMMON_HPP_
#define AUTOWARE__TENSORRT_COMMON__TENSORRT_COMMON_HPP_

#include "autoware/tensorrt_common/logger.hpp"
#include "autoware/tensorrt_common/profiler.hpp"
#include "autoware/tensorrt_common/utils.hpp"

#include <NvInfer.h>
#include <NvOnnxParser.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware
{
namespace tensorrt_common
{
template <typename T>
struct InferDeleter  // NOLINT
{
  void operator()(T * obj) const { delete obj; }
};

template <typename T>
using TrtUniquePtr = std::unique_ptr<T, InferDeleter<T>>;

using NetworkIOPtr = std::unique_ptr<std::vector<NetworkIO>>;
using ProfileDimsPtr = std::unique_ptr<std::vector<ProfileDims>>;
using TensorsVec = std::vector<std::pair<void *, nvinfer1::Dims>>;
using TensorsMap = std::unordered_map<const char *, std::pair<void *, nvinfer1::Dims>>;

/**
 * @class TrtCommon
 * @brief TensorRT common library.
 */
class TrtCommon  // NOLINT
{
public:
  /**
   * @brief Construct TrtCommon.
   *
   * @param[in] trt_config Base configuration with ONNX model path as minimum required.
   * parameter.
   * @param[in] profiler Per-layer profiler.
   * @param[in] plugin_paths Paths for TensorRT plugins.
   */
  TrtCommon(
    const TrtCommonConfig & trt_config,
    const std::shared_ptr<Profiler> & profiler = std::make_shared<Profiler>(),
    const std::vector<std::string> & plugin_paths = {});
  /**
   * @brief Deconstruct TrtCommon
   */
  ~TrtCommon();

  /**
   * @brief Setup for TensorRT execution including building and loading engine.
   *
   * @param[in] profile_dims Optimization profile of tensors for dynamic shapes.
   * @param[in] network_io Network input/output tensors information.
   * @return Whether setup is successful.
   */
  [[nodiscard]] virtual bool setup(
    ProfileDimsPtr profile_dims = nullptr, NetworkIOPtr network_io = nullptr);

  /**
   * @brief Get TensorRT engine precision.
   *
   * @return string representation of TensorRT engine precision.
   */
  [[nodiscard]] std::string getPrecision() const;

  /**
   * @brief Get tensor name by index from TensorRT engine with fallback from TensorRT network.
   *
   * @param[in] index Tensor index.
   * @return Tensor name.
   */
  [[nodiscard]] const char * getIOTensorName(const int32_t index) const;

  /**
   * @brief Get number of IO tensors from TensorRT engine with fallback from TensorRT network.
   *
   * @return Number of IO tensors.
   */
  [[nodiscard]] int32_t getNbIOTensors() const;

  /**
   * @brief Get tensor shape by index from TensorRT engine with fallback from TensorRT network.
   *
   * @param[in] index Tensor index.
   * @return Tensor shape.
   */
  [[nodiscard]] nvinfer1::Dims getTensorShape(const int32_t index) const;

  /**
   * @brief Get tensor shape by name from TensorRT engine.
   *
   * @param[in] tensor_name Tensor name.
   * @return Tensor shape.
   */
  [[nodiscard]] nvinfer1::Dims getTensorShape(const char * tensor_name) const;

  /**
   * @brief Get input tensor shape by index from TensorRT network.
   *
   * @param[in] index Tensor index.
   * @return Tensor shape.
   */
  [[nodiscard]] nvinfer1::Dims getInputDims(const int32_t index) const;

  /**
   * @brief Get output tensor shape by index from TensorRT network.
   *
   * @param[in] index Tensor index.
   * @return Tensor shape.
   */
  [[nodiscard]] nvinfer1::Dims getOutputDims(const int32_t index) const;

  /**
   * @brief Set tensor address by index via TensorRT context.
   *
   * @param[in] index Tensor index.
   * @param[in] data Tensor pointer.
   * @return Whether setting tensor address is successful.
   */
  bool setTensorAddress(const int32_t index, void * data);

  /**
   * @brief Set tensor address by name via TensorRT context.
   *
   * @param[in] tensor_name Tensor name.
   * @param[in] data Tensor pointer.
   * @return Whether setting tensor address is successful.
   */
  bool setTensorAddress(const char * tensor_name, void * data);

  /**
   * @brief Set tensors addresses by indices via TensorRT context.
   *
   * @param[in] tensors Tensors pointers.
   * @return Whether setting tensors addresses is successful.
   */
  bool setTensorsAddresses(std::vector<void *> & tensors);

  /**
   * @brief Set tensors addresses by names via TensorRT context.
   *
   * @param[in] tensors Tensors pointers.
   * @return Whether setting tensors addresses is successful.
   */
  bool setTensorsAddresses(std::unordered_map<const char *, void *> & tensors);

  /**
   * @brief Set input shape by index via TensorRT context.
   *
   * @param[in] index Tensor index.
   * @param[in] dimensions Tensor dimensions.
   * @return Whether setting input shape is successful.
   */
  bool setInputShape(const int32_t index, const nvinfer1::Dims & dimensions);

  /**
   * @brief Set input shape by name via TensorRT context.
   *
   * @param[in] tensor_name Tensor name.
   * @param[in] dimensions Tensor dimensions.
   * @return Whether setting input shape is successful.
   */
  bool setInputShape(const char * tensor_name, const nvinfer1::Dims & dimensions);

  /**
   * @brief Set inputs shapes by indices via TensorRT context.
   *
   * @param[in] dimensions Vector of tensor dimensions with corresponding indices.
   * @return Whether setting input shapes is successful.
   */
  bool setInputsShapes(const std::vector<nvinfer1::Dims> & dimensions);

  /**
   * @brief Set inputs shapes by names via TensorRT context.
   *
   * @param[in] dimensions Map of tensor dimensions with corresponding names as keys.
   * @return Whether setting input shapes is successful.
   */
  bool setInputsShapes(const std::unordered_map<const char *, nvinfer1::Dims> & dimensions);

  /**
   * @brief Set tensor (address and shape) by index via TensorRT context.
   *
   * @param[in] index Tensor index.
   * @param[in] data Tensor pointer.
   * @param[in] dimensions Tensor dimensions.
   * @return Whether setting tensor is successful.
   */
  bool setTensor(const int32_t index, void * data, nvinfer1::Dims dimensions = {});

  /**
   * @brief Set tensor (address and shape) by name via TensorRT context.
   *
   * @param[in] tensor_name Tensor name.
   * @param[in] data Tensor pointer.
   * @param[in] dimensions Tensor dimensions.
   * @return Whether setting tensor is successful.
   */
  bool setTensor(const char * tensor_name, void * data, nvinfer1::Dims dimensions = {});

  /**
   * @brief Set tensors (addresses and shapes) by indices via TensorRT context.
   *
   * @param[in] tensors Vector of tensor pointers and dimensions with corresponding indices.
   * @return Whether setting tensors is successful.
   */
  bool setTensors(TensorsVec & tensors);

  /**
   * @brief Set tensors (addresses and shapes) by names via TensorRT context.
   *
   * @param[in] tensors Map of tensor pointers and dimensions with corresponding names as keys.
   * @return Whether setting tensors is successful.
   */
  bool setTensors(TensorsMap & tensors);

  /**
   * @brief Get per-layer profiler for model.
   *
   * @return Per-layer profiler.
   */
  [[nodiscard]] std::shared_ptr<Profiler> getModelProfiler();

  /**
   * @brief Get per-layer profiler for host.
   *
   * @return Per-layer profiler.
   */
  [[nodiscard]] std::shared_ptr<Profiler> getHostProfiler();

  /**
   * @brief Get TensorRT common configuration.
   *
   * @return TensorRT common configuration.
   */
  [[nodiscard]] std::shared_ptr<TrtCommonConfig> getTrtCommonConfig();

  /**
   * @brief Get TensorRT builder configuration.
   *
   * @return TensorRT builder configuration.
   */
  [[nodiscard]] std::shared_ptr<nvinfer1::IBuilderConfig> getBuilderConfig();

  /**
   * @brief Get TensorRT network definition.
   *
   * @return TensorRT network definition.
   */
  [[nodiscard]] std::shared_ptr<nvinfer1::INetworkDefinition> getNetwork();

  /**
   * @brief Get TensorRT logger.
   *
   * @return TensorRT logger.
   */
  [[nodiscard]] std::shared_ptr<Logger> getLogger();

  /**
   * @brief Execute inference via TensorRT context.
   *
   * @param[in] stream CUDA stream.
   * @return Whether inference is successful.
   */
  bool enqueueV3(cudaStream_t stream);

  /**
   * @brief Print per-layer information.
   */
  void printProfiling() const;

private:
  /**
   * @brief Initialize TensorRT common.
   *
   * @return Whether initialization is successful.
   */
  [[nodiscard]] bool initialize();

  /**
   * @brief Get per-layer information for trt-engine-profiler.
   *
   * @param[in] format Format for layer information.
   * @return Layer information.
   */
  std::string getLayerInformation(nvinfer1::LayerInformationFormat format);

  /**

   * @brief Build TensorRT engine from ONNX.
   *
   * @return Whether building engine is successful.
   */
  bool buildEngineFromOnnx();

  /**
   * @brief Load TensorRT engine.
   *
   * @return Whether loading engine is successful.
   */
  bool loadEngine();

  /**
   * @brief Validate network input/output names and dimensions.
   *
   * @return Whether network input/output is valid.
   */
  bool validateNetworkIO();

  /**
   * @brief Validate optimization profile.
   *
   * @return Whether optimization profile is valid.
   */
  bool validateProfileDims();

  //! @brief TensorRT runtime.
  TrtUniquePtr<nvinfer1::IRuntime> runtime_;

  //! @brief TensorRT engine.
  TrtUniquePtr<nvinfer1::ICudaEngine> engine_;

  //! @brief TensorRT execution context.
  TrtUniquePtr<nvinfer1::IExecutionContext> context_;

  //! @brief TensorRT builder.
  TrtUniquePtr<nvinfer1::IBuilder> builder_;

  //! @brief TensorRT engine parser.
  TrtUniquePtr<nvonnxparser::IParser> parser_;

  //! @brief TensorRT builder configuration.
  std::shared_ptr<nvinfer1::IBuilderConfig> builder_config_;

  //! @brief TensorRT network definition.
  std::shared_ptr<nvinfer1::INetworkDefinition> network_;

  //! @brief TrtCommon library base configuration.
  std::shared_ptr<TrtCommonConfig> trt_config_;

  //! @brief TensorRT logger.
  std::shared_ptr<Logger> logger_;

  //! @brief Per-layer profiler for host.
  std::shared_ptr<Profiler> host_profiler_;

  //! @brief Per-layer profiler for model.
  std::shared_ptr<Profiler> model_profiler_;

  //! @brief Model optimization profile.
  ProfileDimsPtr profile_dims_;

  //! @brief Model network input/output tensors information.
  NetworkIOPtr network_io_;
};

}  // namespace tensorrt_common
}  // namespace autoware

#endif  // AUTOWARE__TENSORRT_COMMON__TENSORRT_COMMON_HPP_

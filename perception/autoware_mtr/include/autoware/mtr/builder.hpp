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

#ifndef AUTOWARE__MTR__BUILDER_HPP_
#define AUTOWARE__MTR__BUILDER_HPP_

#include <NvInfer.h>
#include <NvOnnxParser.h>

#include <filesystem>
namespace fs = ::std::filesystem;

#include <autoware/tensorrt_common/logger.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace autoware::mtr
{

template <typename T>
struct TrtDeleter
{
  void operator()(T * obj) const
  {
    if (obj) {
#if NV_TENSORRT_MAJOR >= 8
      delete obj;
#else
      obj->destroy();
#endif
    }
  }
};  // struct TrtDeleter

template <typename T>
using TrtUniquePtr = std::unique_ptr<T, TrtDeleter<T>>;

// Type names of precisions.
enum PrecisionType { FP32 = 0, FP16 = 1, INT8 = 2 };

// Return corresponding PrecisionType from string.
inline PrecisionType toPrecisionType(const std::string & name)
{
  if (name == "FP32") {
    return PrecisionType::FP32;
  }
  if (name == "FP16") {
    return PrecisionType::FP16;
  }
  if (name == "INT8") {
    return PrecisionType::INT8;
  }
  throw std::invalid_argument("Invalid precision name.");
}

// Type names of calibrations.
enum CalibrationType { ENTROPY = 0, LEGACY = 1, PERCENTILE = 2, MINMAX = 3 };

// Return corresponding CalibrationType from string.
inline CalibrationType toCalibrationType(const std::string & name)
{
  if (name == "ENTROPY") {
    return CalibrationType::ENTROPY;
  }
  if (name == "LEGACY") {
    return CalibrationType::LEGACY;
  }
  if (name == "PERCENTILE") {
    return CalibrationType::PERCENTILE;
  }
  if (name == "MINMAX") {
    return CalibrationType::MINMAX;
  }
  throw std::invalid_argument("Invalid calibration name.");
}

struct BatchOptConfig
{
  /**
   * @brief Construct a new OptimizationConfig for a static shape inference.
   *
   * @param value
   */
  explicit BatchOptConfig(const int32_t value) : k_min(value), k_opt(value), k_max(value) {}

  /**
   * @brief Construct a new OptimizationConfig for a dynamic shape inference.
   *
   * @param k_min
   * @param k_opt
   * @param k_max
   */
  BatchOptConfig(const int32_t k_min, const int32_t k_opt, const int32_t k_max)
  : k_min(k_min), k_opt(k_opt), k_max(k_max)
  {
  }

  int32_t k_min, k_opt, k_max;
};  // struct BatchOptConfig

struct BuildConfig
{
  // type of precision
  PrecisionType precision;

  // type for calibration
  CalibrationType calibration;

  BatchOptConfig batch_target;
  BatchOptConfig batch_agent;

  /**
   * @brief Construct a new instance with default configurations.
   */
  BuildConfig()
  : precision(PrecisionType::FP32),
    calibration(CalibrationType::MINMAX),
    batch_target(1, 10, 20),
    batch_agent(1, 30, 50),
    is_dynamic_(false)
  {
  }

  /**
   * @brief Construct a new build config.
   *
   * @param is_dynamic
   * @param precision
   * @param calibration
   */
  BuildConfig(
    const bool is_dynamic, const std::string & precision, const std::string & calibration,
    const BatchOptConfig & batch_target = BatchOptConfig(1, 10, 20),
    const BatchOptConfig & batch_agent = BatchOptConfig(1, 30, 50))
  : precision(toPrecisionType(precision)),
    calibration(toCalibrationType(calibration)),
    batch_target(batch_target),
    batch_agent(batch_agent),
    is_dynamic_(is_dynamic)
  {
  }

  bool is_dynamic() const { return is_dynamic_; }

private:
  bool is_dynamic_;
};  // struct BuildConfig

class MTRBuilder
{
public:
  /**
   * @brief Construct a new instance.
   *
   * @param model_path Path to engine or onnx file.
   * @param build_config The configuration of build.
   * @param max_workspace_size The max workspace size.
   */
  MTRBuilder(
    const std::string & model_path, const BuildConfig & build_config = BuildConfig(),
    const size_t max_workspace_size = (1ULL << 63));

  /**
   * @brief Setup engine for inference. After finishing setup successfully, `isInitialized` must
   * return `true`.
   */
  void setup();

  /**
   * @brief Check whether engine was initialized successfully.
   *
   * @return True if plugins were initialized successfully.
   */
  [[nodiscard]] bool isInitialized() const;

  // Return true if the model supports dynamic shape inference.
  [[nodiscard]] bool isDynamic() const;

  // Set binding dimensions for specified for dynamic shape inference.
  bool setBindingDimensions(int index, nvinfer1::Dims dimensions);

  /**
   * @brief A wrapper of `nvinfer1::IExecuteContext::enqueueV2`.
   *
   * @param bindings An array of pointers to input and output buffers for the network.
   * @param stream A cuda stream on which the inference kernels will be enqueued.
   * @param inputConsumed An optional event which will be signaled when the input buffers can be
   * refilled with new data.
   * @return True If the kernels were enqueued successfully.
   */
  bool enqueueV2(void ** bindings, cudaStream_t stream, cudaEvent_t * inputConsumed);

private:
  /**
   * @brief Load engin file.
   *
   * @param filepath Engine file path.
   * @return True if the engine were loaded successfully.
   */
  bool loadEngine(const std::string & filepath);

  // Create a cache path of engine file.
  [[nodiscard]] fs::path createEngineCachePath() const;

  /**
   * @brief Build engine from onnx file.
   *
   * @param filepath Onnx file path.
   * @param output_engine_filepath Output engine file path.
   * @return True if the engine were built successfully.
   */
  bool buildEngineFromOnnx(
    const std::string & filepath, const std::string & output_engine_filepath);

  autoware::tensorrt_common::Logger logger_;
  TrtUniquePtr<nvinfer1::IRuntime> runtime_;
  TrtUniquePtr<nvinfer1::ICudaEngine> engine_;
  TrtUniquePtr<nvinfer1::IExecutionContext> context_;

  fs::path model_filepath_;
  size_t max_workspace_size_;
  std::unique_ptr<const BuildConfig> build_config_;

  bool is_initialized_{false};
};  // class MTRBuilder
}  // namespace autoware::mtr
#endif  // AUTOWARE__MTR__BUILDER_HPP_

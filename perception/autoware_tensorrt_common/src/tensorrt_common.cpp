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

#include "autoware/tensorrt_common/tensorrt_common.hpp"

#include "autoware/tensorrt_common/logger.hpp"
#include "autoware/tensorrt_common/utils.hpp"

#include <NvInfer.h>
#include <NvInferPlugin.h>
#include <NvInferRuntimeBase.h>
#include <dlfcn.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware
{
namespace tensorrt_common
{

TrtCommon::TrtCommon(
  const TrtCommonConfig & trt_config, const std::shared_ptr<Profiler> & profiler,
  const std::vector<std::string> & plugin_paths)
: trt_config_(std::make_shared<TrtCommonConfig>(trt_config)),
  host_profiler_(profiler),
  model_profiler_(profiler)
{
  logger_ = std::make_shared<Logger>();
  for (const auto & plugin_path : plugin_paths) {
    int32_t flags{RTLD_LAZY};
// cspell: ignore asan
#if ENABLE_ASAN
    // https://github.com/google/sanitizers/issues/89
    // asan doesn't handle module unloading correctly and there are no plans on doing
    // so. In order to get proper stack traces, don't delete the shared library on
    // close so that asan can resolve the symbols correctly.
    flags |= RTLD_NODELETE;
#endif  // ENABLE_ASAN
    void * handle = dlopen(plugin_path.c_str(), flags);
    if (!handle) {
      logger_->log(nvinfer1::ILogger::Severity::kERROR, "Could not load plugin library");
    } else {
      logger_->log(
        nvinfer1::ILogger::Severity::kINFO, "Loaded plugin library: %s", plugin_path.c_str());
    }
  }
  runtime_ = TrtUniquePtr<nvinfer1::IRuntime>(nvinfer1::createInferRuntime(*logger_));
  if (trt_config_->dla_core_id != -1) {
    runtime_->setDLACore(trt_config_->dla_core_id);
  }
  initLibNvInferPlugins(&*logger_, "");

  if (!initialize()) {
    throw std::runtime_error("Failed to initialize TensorRT");
  }
}

TrtCommon::~TrtCommon() = default;

bool TrtCommon::setup(ProfileDimsPtr profile_dims, NetworkIOPtr network_io)
{
  profile_dims_ = std::move(profile_dims);
  network_io_ = std::move(network_io);

  // Set input profile
  if (profile_dims_ && !profile_dims_->empty()) {
    auto profile = builder_->createOptimizationProfile();
    for (auto & profile_dim : *profile_dims_) {
      if (profile_dim.tensor_name.empty()) {
        profile_dim.tensor_name = getIOTensorName(profile_dim.tensor_index);
      }
      logger_->log(
        nvinfer1::ILogger::Severity::kINFO, "Setting optimization profile for tensor: %s",
        profile_dim.toString().c_str());
      profile->setDimensions(
        profile_dim.tensor_name.c_str(), nvinfer1::OptProfileSelector::kMIN, profile_dim.min_dims);
      profile->setDimensions(
        profile_dim.tensor_name.c_str(), nvinfer1::OptProfileSelector::kOPT, profile_dim.opt_dims);
      profile->setDimensions(
        profile_dim.tensor_name.c_str(), nvinfer1::OptProfileSelector::kMAX, profile_dim.max_dims);
    }
    builder_config_->addOptimizationProfile(profile);
  }

  auto build_engine_with_log = [this]() -> bool {
    logger_->log(nvinfer1::ILogger::Severity::kINFO, "Starting to build engine");
    auto log_thread = logger_->log_throttle(
      nvinfer1::ILogger::Severity::kINFO,
      "Applying optimizations and building TensorRT CUDA engine. Please wait for a few minutes...",
      5);
    auto success = buildEngineFromOnnx();
    logger_->stop_throttle(log_thread);
    logger_->log(nvinfer1::ILogger::Severity::kINFO, "Engine build completed");
    return success;
  };

  // Load engine file if it exists
  if (fs::exists(trt_config_->engine_path)) {
    logger_->log(nvinfer1::ILogger::Severity::kINFO, "Loading engine");
    if (!loadEngine()) {
      return false;
    }
    logger_->log(nvinfer1::ILogger::Severity::kINFO, "Network validation");
    // Validate engine tensor shapes and optimization profile
    if (!validateNetworkIO() || !validateProfileDims()) {
      logger_->log(
        nvinfer1::ILogger::Severity::kWARNING,
        "Network validation failed for loaded engine from file. Rebuilding engine");
      // Rebuild engine if the tensor shapes or optimization profile mismatch
      if (!build_engine_with_log()) {
        return false;
      }
    }
  } else {
    // Build engine if engine has not been cached
    if (!build_engine_with_log()) {
      return false;
    }
  }

  // Validate engine nevertheless is loaded or rebuilt
  if (!validateNetworkIO() || !validateProfileDims()) {
    logger_->log(
      nvinfer1::ILogger::Severity::kERROR,
      "Final network validation failed. Possibly the input / output of the currently "
      "deployed model has changed. Check your configuration file with the current model.");
    return false;
  }

  logger_->log(nvinfer1::ILogger::Severity::kINFO, "Engine setup completed");
  return true;
}

std::string TrtCommon::getPrecision() const
{
  return trt_config_->precision;
}

const char * TrtCommon::getIOTensorName(const int32_t index) const
{
  if (!engine_) {
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Engine is not initialized. Retrieving data from network");
    if (!network_) {
      logger_->log(nvinfer1::ILogger::Severity::kERROR, "Network is not initialized");
      return nullptr;
    }
    auto num_inputs = network_->getNbInputs();
    auto num_outputs = network_->getNbOutputs();
    if (index < 0 || index >= num_inputs + num_outputs) {
      logger_->log(
        nvinfer1::ILogger::Severity::kERROR,
        "Invalid index for I/O tensor: %d. Total I/O tensors: %d", index, num_inputs + num_outputs);
      return nullptr;
    }
    if (index < num_inputs) {
      return network_->getInput(index)->getName();
    }
    return network_->getOutput(index - num_inputs)->getName();
  }

  return engine_->getIOTensorName(index);
}

int32_t TrtCommon::getNbIOTensors() const
{
  if (!engine_) {
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Engine is not initialized. Retrieving data from network");
    if (!network_) {
      logger_->log(nvinfer1::ILogger::Severity::kERROR, "Network is not initialized");
      return 0;
    }
    return network_->getNbInputs() + network_->getNbOutputs();
  }
  return engine_->getNbIOTensors();
}

nvinfer1::Dims TrtCommon::getTensorShape(const int32_t index) const
{
  if (!engine_) {
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Engine is not initialized. Retrieving data from network");
    if (!network_) {
      logger_->log(nvinfer1::ILogger::Severity::kERROR, "Network is not initialized");
      return nvinfer1::Dims{};
    }
    auto num_inputs = network_->getNbInputs();
    auto num_outputs = network_->getNbOutputs();
    if (index < 0 || index >= num_inputs + num_outputs) {
      logger_->log(
        nvinfer1::ILogger::Severity::kERROR,
        "Invalid index for I/O tensor: %d. Total I/O tensors: %d", index, num_inputs + num_outputs);
      return nvinfer1::Dims{};
    }
    if (index < num_inputs) {
      return network_->getInput(index)->getDimensions();
    }
    return network_->getOutput(index - num_inputs)->getDimensions();
  }
  auto const & name = getIOTensorName(index);
  return getTensorShape(name);
}

nvinfer1::Dims TrtCommon::getTensorShape(const char * tensor_name) const
{
  if (!engine_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Engine is not initialized");
    return nvinfer1::Dims{};
  }
  return engine_->getTensorShape(tensor_name);
}

nvinfer1::Dims TrtCommon::getInputDims(const int32_t index) const
{
  if (!network_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Network is not initialized");
    return {};
  }
  const auto input = network_->getInput(index);
  return input->getDimensions();
}

nvinfer1::Dims TrtCommon::getOutputDims(const int32_t index) const
{
  if (!network_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Network is not initialized");
    return {};
  }
  const auto output = network_->getOutput(index);
  return output->getDimensions();
}

bool TrtCommon::setTensorAddress(const int32_t index, void * data)
{
  auto const & name = getIOTensorName(index);
  return setTensorAddress(name, data);
}

bool TrtCommon::setTensorAddress(const char * tensor_name, void * data)
{
  if (!context_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Context is not initialized");
    return false;
  }
  auto success = context_->setTensorAddress(tensor_name, data);
  if (!success) {
    logger_->log(
      nvinfer1::ILogger::Severity::kERROR,
      "Failed to set tensor address for tensor: ", tensor_name);
  }
  return success;
}

bool TrtCommon::setTensorsAddresses(std::vector<void *> & tensors)
{
  bool success = true;
  for (std::size_t i = 0, e = tensors.size(); i < e; i++) {
    auto const name = getIOTensorName(i);
    success &= setTensorAddress(name, tensors.at(i));
  }
  return success;
}

bool TrtCommon::setTensorsAddresses(std::unordered_map<const char *, void *> & tensors)
{
  bool success = true;
  for (auto const & tensor : tensors) {
    success &= setTensorAddress(tensor.first, tensor.second);
  }
  return success;
}

bool TrtCommon::setInputShape(const int32_t index, const nvinfer1::Dims & dimensions)
{
  auto const & name = getIOTensorName(index);
  return setInputShape(name, dimensions);
}

bool TrtCommon::setInputShape(const char * tensor_name, const nvinfer1::Dims & dimensions)
{
  if (!context_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Context is not initialized");
    return false;
  }
  auto success = context_->setInputShape(tensor_name, dimensions);
  if (!success) {
    logger_->log(
      nvinfer1::ILogger::Severity::kERROR, "Failed to set input shape for tensor: ", tensor_name);
  }
  return success;
}

bool TrtCommon::setInputsShapes(const std::vector<nvinfer1::Dims> & dimensions)
{
  bool success = true;
  for (std::size_t i = 0, e = dimensions.size(); i < e; i++) {
    success &= setInputShape(i, dimensions.at(i));
  }
  return success;
}

bool TrtCommon::setInputsShapes(const std::unordered_map<const char *, nvinfer1::Dims> & dimensions)
{
  bool success = true;
  for (auto const & dim : dimensions) {
    success &= setInputShape(dim.first, dim.second);
  }
  return success;
}

bool TrtCommon::setTensor(const int32_t index, void * data, nvinfer1::Dims dimensions)
{
  auto success = setTensorAddress(index, data);
  if (dimensions.nbDims > 0) {
    success &= setInputShape(index, dimensions);
  }
  return success;
}

bool TrtCommon::setTensor(const char * tensor_name, void * data, nvinfer1::Dims dimensions)
{
  auto success = setTensorAddress(tensor_name, data);
  if (dimensions.nbDims > 0) {
    success &= setInputShape(tensor_name, dimensions);
  }
  return success;
}

bool TrtCommon::setTensors(TensorsVec & tensors)
{
  bool success = true;
  for (std::size_t i = 0, e = tensors.size(); i < e; i++) {
    success &= setTensor(i, tensors.at(i).first, tensors.at(i).second);
  }
  return success;
}

bool TrtCommon::setTensors(TensorsMap & tensors)
{
  bool success = true;
  for (auto const & tensor : tensors) {
    success &= setTensor(tensor.first, tensor.second.first, tensor.second.second);
  }
  return success;
}

std::shared_ptr<Profiler> TrtCommon::getModelProfiler()
{
  return model_profiler_;
}

std::shared_ptr<Profiler> TrtCommon::getHostProfiler()
{
  return host_profiler_;
}

std::shared_ptr<TrtCommonConfig> TrtCommon::getTrtCommonConfig()
{
  return trt_config_;
}

std::shared_ptr<nvinfer1::IBuilderConfig> TrtCommon::getBuilderConfig()
{
  return builder_config_;
}

std::shared_ptr<nvinfer1::INetworkDefinition> TrtCommon::getNetwork()
{
  return network_;
}

std::shared_ptr<Logger> TrtCommon::getLogger()
{
  return logger_;
}

bool TrtCommon::enqueueV3(cudaStream_t stream)
{
  if (!context_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Context is not initialized");
    return false;
  }
  if (trt_config_->profile_per_layer) {
    auto inference_start = std::chrono::high_resolution_clock::now();
    auto success = context_->enqueueV3(stream);
    auto inference_end = std::chrono::high_resolution_clock::now();
    host_profiler_->reportLayerTime(
      "inference_host",
      std::chrono::duration<float, std::milli>(inference_end - inference_start).count());
    return success;
  }
  return context_->enqueueV3(stream);
}

void TrtCommon::printProfiling() const
{
  logger_->log(
    nvinfer1::ILogger::Severity::kINFO, "Host Profiling\n", host_profiler_->toString().c_str());
  logger_->log(
    nvinfer1::ILogger::Severity::kINFO, "Model Profiling\n", model_profiler_->toString().c_str());
}

std::string TrtCommon::getLayerInformation(nvinfer1::LayerInformationFormat format)
{
  if (!context_ || !engine_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Context or engine are not initialized");
    return {};
  }
  auto inspector = std::unique_ptr<nvinfer1::IEngineInspector>(engine_->createEngineInspector());
  inspector->setExecutionContext(&(*context_));
  std::string result = inspector->getEngineInformation(format);
  return result;
}

bool TrtCommon::initialize()
{
  if (!fs::exists(trt_config_->onnx_path) || trt_config_->onnx_path.extension() != ".onnx") {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Invalid ONNX file path or extension");
    return false;
  }

  builder_ = TrtUniquePtr<nvinfer1::IBuilder>(nvinfer1::createInferBuilder(*logger_));
  if (!builder_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create builder");
    return false;
  }

#if (NV_TENSORRT_MAJOR * 1000) + (NV_TENSORRT_MINOR * 100) + NV_TENSOR_PATCH < 10000
  const auto explicit_batch =
    1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
  network_ = TrtUniquePtr<nvinfer1::INetworkDefinition>(builder_->createNetworkV2(explicit_batch));
#else
  network_ = TrtUniquePtr<nvinfer1::INetworkDefinition>(builder_->createNetworkV2(0));
#endif

  if (!network_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create network");
    return false;
  }

  builder_config_ = TrtUniquePtr<nvinfer1::IBuilderConfig>(builder_->createBuilderConfig());
  if (!builder_config_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create builder config");
    return false;
  }

  auto num_available_dla = builder_->getNbDLACores();
  if (trt_config_->dla_core_id != -1) {
    logger_->log(
      nvinfer1::ILogger::Severity::kINFO, "Number of DLAs supported: %d", num_available_dla);
    builder_config_->setDefaultDeviceType(nvinfer1::DeviceType::kDLA);
    builder_config_->setDLACore(trt_config_->dla_core_id);
    builder_config_->setFlag(nvinfer1::BuilderFlag::kPREFER_PRECISION_CONSTRAINTS);
    builder_config_->setFlag(nvinfer1::BuilderFlag::kGPU_FALLBACK);
  }
  if (trt_config_->precision == "fp16") {
    builder_config_->setFlag(nvinfer1::BuilderFlag::kFP16);
  } else if (trt_config_->precision == "int8") {
    builder_config_->setFlag(nvinfer1::BuilderFlag::kINT8);
  }
  builder_config_->setMemoryPoolLimit(
    nvinfer1::MemoryPoolType::kWORKSPACE, trt_config_->max_workspace_size);

  parser_ = TrtUniquePtr<nvonnxparser::IParser>(nvonnxparser::createParser(*network_, *logger_));
  if (!parser_->parseFromFile(
        trt_config_->onnx_path.c_str(),
        static_cast<int32_t>(nvinfer1::ILogger::Severity::kERROR))) {
    return false;
  }

  if (trt_config_->profile_per_layer) {
    builder_config_->setProfilingVerbosity(nvinfer1::ProfilingVerbosity::kDETAILED);
  }

  return true;
}

bool TrtCommon::buildEngineFromOnnx()
{
  // Build engine
  auto plan = TrtUniquePtr<nvinfer1::IHostMemory>(
    builder_->buildSerializedNetwork(*network_, *builder_config_));
  if (!plan) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create host memory");
    return false;
  }
  engine_ = TrtUniquePtr<nvinfer1::ICudaEngine>(
    runtime_->deserializeCudaEngine(plan->data(), plan->size()));

  if (!engine_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create engine");
    return false;
  }

  // Save engine
  std::ofstream file;
  file.open(trt_config_->engine_path, std::ios::binary | std::ios::out);
  if (!file.is_open()) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to open engine file");
    return false;
  }
  file.write(reinterpret_cast<const char *>(plan->data()), plan->size());  // NOLINT
  file.close();

  context_ = TrtUniquePtr<nvinfer1::IExecutionContext>(engine_->createExecutionContext());
  if (!context_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create context");
    return false;
  }

  if (trt_config_->profile_per_layer) {
    context_->setProfiler(&*model_profiler_);
  }

  fs::path json_path = trt_config_->engine_path.replace_extension(".json");
  auto ret = getLayerInformation(nvinfer1::LayerInformationFormat::kJSON);
  std::ofstream os(json_path, std::ofstream::trunc);
  os << ret << std::flush;
  os.close();

  return true;
}

bool TrtCommon::loadEngine()
{
  std::ifstream engine_file(trt_config_->engine_path);
  std::stringstream engine_buffer;
  engine_buffer << engine_file.rdbuf();
  std::string engine_str = engine_buffer.str();

  engine_ = TrtUniquePtr<nvinfer1::ICudaEngine>(runtime_->deserializeCudaEngine(
    reinterpret_cast<const void *>(  // NOLINT
      engine_str.data()),
    engine_str.size()));
  if (!engine_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create engine");
    return false;
  }

  context_ = TrtUniquePtr<nvinfer1::IExecutionContext>(engine_->createExecutionContext());
  if (!context_) {
    logger_->log(nvinfer1::ILogger::Severity::kERROR, "Fail to create context");
    return false;
  }

  if (trt_config_->profile_per_layer) {
    context_->setProfiler(&*model_profiler_);
  }

  return true;
}

bool TrtCommon::validateProfileDims()
{
  auto success = true;
  if (!profile_dims_ || profile_dims_->empty()) {
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Input profile is empty, skipping validation. If network has dynamic shapes, it might lead "
      "to undefined behavior.");
    return success;
  }
  if (engine_->getNbOptimizationProfiles() != 1) {
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Number of optimization profiles in the engine (%d) is not equal to 1. Selecting the first "
      "cached profile.",
      engine_->getNbOptimizationProfiles());
  }

  for (const auto & profile_dim : *profile_dims_) {
    nvinfer1::Dims min_dims = engine_->getProfileShape(
      profile_dim.tensor_name.c_str(), 0, nvinfer1::OptProfileSelector::kMIN);
    nvinfer1::Dims opt_dims = engine_->getProfileShape(
      profile_dim.tensor_name.c_str(), 0, nvinfer1::OptProfileSelector::kOPT);
    nvinfer1::Dims max_dims = engine_->getProfileShape(
      profile_dim.tensor_name.c_str(), 0, nvinfer1::OptProfileSelector::kMAX);
    ProfileDims profile_from_engine{profile_dim.tensor_name, min_dims, opt_dims, max_dims};
    if (profile_dim != profile_from_engine) {
      logger_->log(
        nvinfer1::ILogger::Severity::kWARNING,
        "Invalid profile. Current configuration: %s. Cached engine: %s",
        profile_dim.toString().c_str(), profile_from_engine.toString().c_str());
      success = false;
    }
  }
  return success;
}

bool TrtCommon::validateNetworkIO()
{
  auto success = true;
  if (!network_io_ || network_io_->empty()) {
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Network IO is empty, skipping validation. It might lead to undefined behavior");
    return success;
  }

  if (network_io_->size() != static_cast<size_t>(getNbIOTensors())) {
    std::string tensor_names = "[" + std::string(getIOTensorName(0));
    for (int32_t i = 1; i < getNbIOTensors(); ++i) {
      tensor_names += ", " + std::string(getIOTensorName(i));
    }
    tensor_names += "]";
    logger_->log(
      nvinfer1::ILogger::Severity::kWARNING,
      "Number of tensors in the engine (%d) does not match number of tensors in the config (%d). "
      "Tensors in the built engine: %s",
      getNbIOTensors(), network_io_->size(), tensor_names.c_str());
    success = false;
  }
  for (const auto & io : *network_io_) {
    NetworkIO tensor_from_engine{io.tensor_name, getTensorShape(io.tensor_name.c_str())};
    if (io != tensor_from_engine) {
      logger_->log(
        nvinfer1::ILogger::Severity::kERROR,
        "Invalid tensor. Current configuration: %s. Cached engine: %s", io.toString().c_str(),
        tensor_from_engine.toString().c_str());
      success = false;
    }
  }

  return success;
}

}  // namespace tensorrt_common
}  // namespace autoware

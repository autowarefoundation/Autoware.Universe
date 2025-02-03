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

#include "autoware/mtr/builder.hpp"

#include <fstream>
#include <sstream>

namespace autoware::mtr
{
namespace
{
/**
 * @brief Get the name of precision in string.
 *
 * @param type
 * @return std::string
 */
std::string getPrecisionName(const PrecisionType & type)
{
  switch (type) {
    case PrecisionType::FP32:
      return "FP32";
    case PrecisionType::FP16:
      return "FP16";
    case PrecisionType::INT8:
      return "INT8";
    default:
      throw std::runtime_error("Unsupported precision type.");
  }
}

/**
 * @brief Get the name of calibration in string.
 *
 * @param type
 * @return std::string
 */
std::string getCalibrationName(const CalibrationType & type)
{
  switch (type) {
    case CalibrationType::ENTROPY:
      return "ENTROPY";
    case CalibrationType::LEGACY:
      return "LEGACY";
    case CalibrationType::PERCENTILE:
      return "PERCENTILE";
    case CalibrationType::MINMAX:
      return "MINMAX";
    default:
      throw std::runtime_error("Unsupported calibration type.");
  }
}
}  // namespace

MTRBuilder::MTRBuilder(
  const std::string & model_filepath, const BuildConfig & build_config,
  const size_t max_workspace_size)
: model_filepath_(model_filepath), max_workspace_size_(max_workspace_size)
{
  build_config_ = std::make_unique<const BuildConfig>(build_config);
  runtime_ = TrtUniquePtr<nvinfer1::IRuntime>(nvinfer1::createInferRuntime(logger_));
}

void MTRBuilder::setup()
{
  if (!fs::exists(model_filepath_)) {
    is_initialized_ = false;
    return;
  }
  std::string engine_path = model_filepath_;
  if (model_filepath_.extension() == ".engine") {
    std::cout << "Loading... " << model_filepath_ << std::endl;
    loadEngine(model_filepath_);
  } else if (model_filepath_.extension() == ".onnx") {
    const auto engine_cache_path = createEngineCachePath();
    if (fs::exists(engine_cache_path)) {
      std::cout << "Loading cached engine... " << engine_cache_path << std::endl;
      if (!loadEngine(engine_cache_path)) {
        std::cerr << "Fail to load engine" << std::endl;
        is_initialized_ = false;
        return;
      }
    } else {
      std::cout << "Building... " << engine_cache_path << std::endl;
      logger_.log(nvinfer1::ILogger::Severity::kINFO, "start build engine");
      if (!buildEngineFromOnnx(model_filepath_, engine_cache_path)) {
        std::cerr << "Fail to build engine from onnx" << std::endl;
        is_initialized_ = false;
        return;
      }
      logger_.log(nvinfer1::ILogger::Severity::kINFO, "End build engine");
    }
    engine_path = engine_cache_path;
  } else {
    is_initialized_ = false;
    return;
  }

  context_ = TrtUniquePtr<nvinfer1::IExecutionContext>(engine_->createExecutionContext());
  if (!context_) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to create context");
    is_initialized_ = false;
    return;
  }

  is_initialized_ = true;
}

bool MTRBuilder::loadEngine(const std::string & filepath)
{
  try {
    std::ifstream engine_file(filepath);
    std::stringstream buffer;
    buffer << engine_file.rdbuf();
    std::string engine_str = buffer.str();
    engine_ = TrtUniquePtr<nvinfer1::ICudaEngine>(runtime_->deserializeCudaEngine(
      reinterpret_cast<const void *>(engine_str.data()), engine_str.size()));
    return true;
  } catch (std::exception & e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

fs::path MTRBuilder::createEngineCachePath() const
{
  fs::path cache_engine_path{model_filepath_};
  auto precision_name = getPrecisionName(build_config_->precision);
  auto calibration_name = build_config_->precision == PrecisionType::INT8
                            ? getCalibrationName(build_config_->calibration)
                            : "";
  cache_engine_path.replace_extension(calibration_name + precision_name + ".engine");
  return cache_engine_path;
}

bool MTRBuilder::buildEngineFromOnnx(
  const std::string & filepath, const std::string & output_engine_filepath)
{
  auto builder = TrtUniquePtr<nvinfer1::IBuilder>(nvinfer1::createInferBuilder(logger_));
  if (!builder) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to create builder");
    return false;
  }

  const auto explicit_batch =
    1U << static_cast<int32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);

  auto network =
    TrtUniquePtr<nvinfer1::INetworkDefinition>(builder->createNetworkV2(explicit_batch));
  if (!network) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to create network");
    return false;
  }

  auto config = TrtUniquePtr<nvinfer1::IBuilderConfig>(builder->createBuilderConfig());
  if (!config) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to create builder config");
    return false;
  }

  if (build_config_->precision == PrecisionType::FP16) {
    config->setFlag(nvinfer1::BuilderFlag::kFP16);
  } else if (build_config_->precision == PrecisionType::INT8) {
    config->setFlag(nvinfer1::BuilderFlag::kINT8);
  }

#if (NV_TENSORRT_MAJOR * 1000) + (NV_TENSORRT_MINOR * 100) + NV_TENSOR_PATCH >= 8400
  config->setMemoryPoolLimit(nvinfer1::MemoryPoolType::kWORKSPACE, max_workspace_size_);
#else
  config->setMaxWorkspaceSize(max_workspace_size_);
#endif

  auto parser = TrtUniquePtr<nvonnxparser::IParser>(nvonnxparser::createParser(*network, logger_));
  if (!parser->parseFromFile(
        filepath.c_str(), static_cast<int>(nvinfer1::ILogger::Severity::kERROR))) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to parse onnx file");
    return false;
  }

  if (isDynamic()) {
    auto profile = builder->createOptimizationProfile();
    const auto input0 = network->getInput(0);
    const auto input0_dims = input0->getDimensions();
    const auto num_past_frames = input0_dims.d[2];
    const auto num_agent_dims = input0_dims.d[3];

    const auto input2 = network->getInput(2);
    const auto input2_dims = input2->getDimensions();
    const auto num_polylines = input2_dims.d[1];
    const auto num_points = input2_dims.d[2];
    const auto num_polyline_dims = input2_dims.d[3];

    const auto & batch_target = build_config_->batch_target;
    const auto & batch_agent = build_config_->batch_agent;
    {  // trajectory
      auto name = network->getInput(0)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN,
        nvinfer1::Dims4{batch_target.k_min, batch_agent.k_min, num_past_frames, num_agent_dims});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT,
        nvinfer1::Dims4{batch_target.k_opt, batch_agent.k_opt, num_past_frames, num_agent_dims});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX,
        nvinfer1::Dims4{batch_target.k_max, batch_agent.k_max, num_past_frames, num_agent_dims});
    }
    {  // trajectory mask
      auto name = network->getInput(1)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN,
        nvinfer1::Dims3{batch_target.k_min, batch_agent.k_min, num_past_frames});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT,
        nvinfer1::Dims3{batch_target.k_opt, batch_agent.k_opt, num_past_frames});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX,
        nvinfer1::Dims3{batch_target.k_max, batch_agent.k_max, num_past_frames});
    }
    {  // polyline
      auto name = network->getInput(2)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN,
        nvinfer1::Dims4{batch_target.k_min, num_polylines, num_points, num_polyline_dims});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT,
        nvinfer1::Dims4{batch_target.k_opt, num_polylines, num_points, num_polyline_dims});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX,
        nvinfer1::Dims4{batch_target.k_max, num_polylines, num_points, num_polyline_dims});
    }
    {  // polyline mask
      auto name = network->getInput(3)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN,
        nvinfer1::Dims3{batch_target.k_min, num_polylines, num_points});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT,
        nvinfer1::Dims3{batch_target.k_opt, num_polylines, num_points});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX,
        nvinfer1::Dims3{batch_target.k_max, num_polylines, num_points});
    }
    {  // polyline center
      auto name = network->getInput(4)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN,
        nvinfer1::Dims3{batch_target.k_min, num_polylines, 3});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT,
        nvinfer1::Dims3{batch_target.k_opt, num_polylines, 3});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX,
        nvinfer1::Dims3{batch_target.k_max, num_polylines, 3});
    }
    {  // last pos
      auto name = network->getInput(5)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN,
        nvinfer1::Dims3{batch_target.k_min, batch_agent.k_min, 3});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT,
        nvinfer1::Dims3{batch_target.k_opt, batch_agent.k_opt, 3});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX,
        nvinfer1::Dims3{batch_target.k_max, batch_agent.k_max, 3});
    }
    {  // track index
      auto name = network->getInput(6)->getName();
      nvinfer1::Dims minDim, optDim, maxDim;
      minDim.nbDims = 1;
      minDim.d[0] = batch_target.k_min;
      profile->setDimensions(name, nvinfer1::OptProfileSelector::kMIN, minDim);
      optDim.nbDims = 1;
      optDim.d[0] = batch_target.k_opt;
      profile->setDimensions(name, nvinfer1::OptProfileSelector::kOPT, optDim);
      maxDim.nbDims = 1;
      maxDim.d[0] = batch_target.k_max;
      profile->setDimensions(name, nvinfer1::OptProfileSelector::kMAX, maxDim);
    }
    {
      // intention points
      auto name = network->getInput(7)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN, nvinfer1::Dims3{batch_target.k_min, 64, 2});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT, nvinfer1::Dims3{batch_target.k_opt, 64, 2});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX, nvinfer1::Dims3{batch_target.k_max, 64, 2});
    }
    {  // pred scores
      auto name = network->getOutput(0)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN, nvinfer1::Dims2{batch_target.k_min, 6});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT, nvinfer1::Dims2{batch_target.k_opt, 6});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX, nvinfer1::Dims2{batch_target.k_max, 6});
    }
    {  // pred trajs
      auto name = network->getOutput(1)->getName();
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMIN, nvinfer1::Dims4{batch_target.k_min, 6, 80, 7});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kOPT, nvinfer1::Dims4{batch_target.k_opt, 6, 80, 7});
      profile->setDimensions(
        name, nvinfer1::OptProfileSelector::kMAX, nvinfer1::Dims4{batch_target.k_max, 6, 80, 7});
    }
    config->addOptimizationProfile(profile);
  }

  if (isDynamic()) {
#if (NV_TENSORRT_MAJOR * 1000) + (NV_TENSORRT_MINOR * 100) + NV_TENSOR_PATCH >= 8200
    config->setProfilingVerbosity(nvinfer1::ProfilingVerbosity::kDETAILED);
#else
    config->setProfilingVerbosity(nvinfer1::ProfilingVerbosity::kVERBOSE);
#endif
  }

#if NV_TENSORRT_MAJOR >= 8
  auto plan =
    TrtUniquePtr<nvinfer1::IHostMemory>(builder->buildSerializedNetwork(*network, *config));
  if (!plan) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to create host memory");
    return false;
  }
  engine_ = TrtUniquePtr<nvinfer1::ICudaEngine>(
    runtime_->deserializeCudaEngine(plan->data(), plan->size()));
#else
  engine_ = TrtUniquePtr<nvinfer1::ICudaEngine>(builder->buildEngineWithConfig(*network, *config));
#endif

  if (!engine_) {
    logger_.log(nvinfer1::ILogger::Severity::kERROR, "Fail to create engine");
    return false;
  }

// save engine
#if NV_TENSORRT_MAJOR < 8
  auto data = TrtUniquePtr<nvinfer1::IHostMemory>(engine_->serialize());
#endif
  std::ofstream file;
  file.open(output_engine_filepath, std::ios::binary | std::ios::out);
  if (!file.is_open()) {
    return false;
  }
#if NV_TENSORRT_MAJOR < 8
  file.write(reinterpret_cast<const char *>(data->data()), data->size());
#else
  file.write(reinterpret_cast<const char *>(plan->data()), plan->size());
#endif

  file.close();

  return true;
}

bool MTRBuilder::isInitialized() const
{
  return is_initialized_;
}

bool MTRBuilder::isDynamic() const
{
  return build_config_->is_dynamic();
}

bool MTRBuilder::setBindingDimensions(int index, nvinfer1::Dims dimensions)
{
  if (isDynamic()) {
    return context_->setBindingDimensions(index, dimensions);
  } else {
    return true;
  }
}

bool MTRBuilder::enqueueV2(void ** bindings, cudaStream_t stream, cudaEvent_t * inputConsumed)
{
  return context_->enqueueV2(bindings, stream, inputConsumed);
}
}  // namespace autoware::mtr

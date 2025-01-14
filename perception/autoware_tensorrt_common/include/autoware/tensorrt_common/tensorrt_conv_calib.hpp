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

#ifndef AUTOWARE__TENSORRT_COMMON__TENSORRT_CONV_CALIB_HPP_
#define AUTOWARE__TENSORRT_COMMON__TENSORRT_CONV_CALIB_HPP_

#include <autoware/tensorrt_common/conv_profiler.hpp>
#include <autoware/tensorrt_common/tensorrt_common.hpp>
#include <autoware/tensorrt_common/utils.hpp>

#include <NvInfer.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware
{
namespace tensorrt_common
{
template <class T>
bool contain(const std::string & s, const T & v)
{
  return s.find(v) != std::string::npos;
}

using autoware::tensorrt_common::CalibrationConfig;
using autoware::tensorrt_common::NetworkIOPtr;
using autoware::tensorrt_common::ProfileDimsPtr;
using autoware::tensorrt_common::Profiler;
using autoware::tensorrt_common::TrtCommonConfig;

/**
 * @class TrtConvCalib
 * @brief TensorRT common library with calibration.
 */
class TrtConvCalib : public tensorrt_common::TrtCommon
{
public:
  /**
   * @brief Construct TrtCommonCalib.
   *
   * @param[in] trt_config base trt common configuration, ONNX model path is mandatory
   * @param[in] profiler per-layer profiler
   * @param[in] plugin_paths paths for TensorRT plugins
   */
  explicit TrtConvCalib(
    const TrtCommonConfig & trt_config,
    const std::shared_ptr<Profiler> & profiler = std::make_shared<Profiler>(),
    const std::vector<std::string> & plugin_paths = {})
  : TrtCommon(trt_config, profiler, plugin_paths)
  {
  }

  /**
   * @brief Setup for TensorRT execution including calibration, building and loading engine.
   *
   * @param[in] calibrator Pointer for any type of INT8 calibrator.
   * @param[in] calib_config Calibration configuration.
   * @param[in] profile_dims Optimization profile of input tensors for dynamic shapes.
   * @param[in] network_io Network input/output tensors information.
   * @return Whether setup is successful.
   */
  [[nodiscard]] bool setupWithCalibrator(
    std::unique_ptr<nvinfer1::IInt8Calibrator> calibrator, const CalibrationConfig & calib_config,
    ProfileDimsPtr profile_dims = nullptr, NetworkIOPtr network_io = nullptr)
  {
    calibrator_ = std::move(calibrator);
    calib_config_ = std::make_unique<const CalibrationConfig>(calib_config);

    auto builder_config = getBuilderConfig();
    builder_config->setFlag(nvinfer1::BuilderFlag::kPREFER_PRECISION_CONSTRAINTS);
    builder_config->setInt8Calibrator(calibrator_.get());

    // Model specific quantization
    auto logger = getLogger();
    auto quantization_log = quantization();
    logger->log(nvinfer1::ILogger::Severity::kINFO, quantization_log.c_str());

    return setup(std::move(profile_dims), std::move(network_io));
  }

private:
  /**
   * @brief Implicit quantization for TensorRT.
   *
   * @return Output log for TensorRT logger.
   */
  std::string quantization()
  {
    auto network = getNetwork();
    auto trt_config = getTrtCommonConfig();
    auto model_profiler = getModelProfiler();

    const int num = network->getNbLayers();
    bool first = calib_config_->quantize_first_layer;
    bool last = calib_config_->quantize_last_layer;
    std::stringstream ss;

    // Partial Quantization
    if (getPrecision() == "int8") {
      auto builder_config = getBuilderConfig();
      builder_config->setFlag(nvinfer1::BuilderFlag::kFP16);
      network->getInput(0)->setDynamicRange(0, 255.0);
      for (int i = 0; i < num; i++) {
        nvinfer1::ILayer * layer = network->getLayer(i);
        auto layer_type = layer->getType();
        std::string name = layer->getName();
        nvinfer1::ITensor * out = layer->getOutput(0);
        if (calib_config_->clip_value > 0.0) {
          ss << "Set max value for outputs: " << calib_config_->clip_value << "  " << name
             << std::endl;
          out->setDynamicRange(0.0, calib_config_->clip_value);
        }

        if (layer_type == nvinfer1::LayerType::kCONVOLUTION) {
          if (first) {
            layer->setPrecision(nvinfer1::DataType::kHALF);
            ss << "Set kHALF in " << name << std::endl;
            first = false;
          }
          if (last) {
            // cspell: ignore preds
            if (
              contain(name, "reg_preds") || contain(name, "cls_preds") ||
              contain(name, "obj_preds")) {
              layer->setPrecision(nvinfer1::DataType::kHALF);
              ss << "Set kHALF in " << name << std::endl;
            }
            for (int j = num - 1; j >= 0; j--) {
              nvinfer1::ILayer * inner_layer = network->getLayer(j);
              auto inner_layer_type = inner_layer->getType();
              std::string inner_name = inner_layer->getName();
              if (inner_layer_type == nvinfer1::LayerType::kCONVOLUTION) {
                inner_layer->setPrecision(nvinfer1::DataType::kHALF);
                ss << "Set kHALF in " << inner_name << std::endl;
                break;
              }
              if (inner_layer_type == nvinfer1::LayerType::kMATRIX_MULTIPLY) {
                inner_layer->setPrecision(nvinfer1::DataType::kHALF);
                ss << "Set kHALF in " << inner_name << std::endl;
                break;
              }
            }
          }
        }
      }
      builder_config->setFlag(nvinfer1::BuilderFlag::kINT8);
    }

    // Print layer information
    float total_gflops = 0.0;
    int total_params = 0;

    for (int i = 0; i < num; i++) {
      nvinfer1::ILayer * layer = network->getLayer(i);
      auto layer_type = layer->getType();
      if (trt_config->profile_per_layer) {
        model_profiler->setProfDict(layer);
      }
      if (layer_type == nvinfer1::LayerType::kCONSTANT) {
        continue;
      }
      nvinfer1::ITensor * in = layer->getInput(0);
      nvinfer1::Dims dim_in = in->getDimensions();
      nvinfer1::ITensor * out = layer->getOutput(0);
      nvinfer1::Dims dim_out = out->getDimensions();

      if (layer_type == nvinfer1::LayerType::kCONVOLUTION) {
        auto conv = dynamic_cast<nvinfer1::IConvolutionLayer *>(layer);
        nvinfer1::Dims k_dims = conv->getKernelSizeNd();
        nvinfer1::Dims s_dims = conv->getStrideNd();
        int groups = conv->getNbGroups();
        int stride = s_dims.d[0];
        int num_weights = (dim_in.d[1] / groups) * dim_out.d[1] * k_dims.d[0] * k_dims.d[1];
        float gflops = (2.0 * num_weights) * (static_cast<float>(dim_in.d[3]) / stride *
                                              static_cast<float>(dim_in.d[2]) / stride / 1e9);
        total_gflops += gflops;
        total_params += num_weights;
        ss << "L" << i << " [conv " << k_dims.d[0] << "x" << k_dims.d[1] << " (" << groups << ") "
           << ") " << "/" << s_dims.d[0] << "] " << dim_in.d[3] << "x" << dim_in.d[2] << "x"
           << " -> " << dim_out.d[3] << "x" << dim_out.d[2] << "x" << dim_out.d[1];
        ss << " weights:" << num_weights;
        ss << " GFLOPs:" << gflops;
        ss << std::endl;
      } else if (layer_type == nvinfer1::LayerType::kPOOLING) {
        auto pool = dynamic_cast<nvinfer1::IPoolingLayer *>(layer);
        auto p_type = pool->getPoolingType();
        nvinfer1::Dims dim_stride = pool->getStrideNd();
        nvinfer1::Dims dim_window = pool->getWindowSizeNd();

        ss << "L" << i << " [";
        if (p_type == nvinfer1::PoolingType::kMAX) {
          ss << "max ";
        } else if (p_type == nvinfer1::PoolingType::kAVERAGE) {
          ss << "avg ";
        } else if (p_type == nvinfer1::PoolingType::kMAX_AVERAGE_BLEND) {
          ss << "max avg blend ";
        }
        float gflops = static_cast<float>(dim_in.d[1]) *
                       (static_cast<float>(dim_window.d[0]) / static_cast<float>(dim_stride.d[0])) *
                       (static_cast<float>(dim_window.d[1]) / static_cast<float>(dim_stride.d[1])) *
                       static_cast<float>(dim_in.d[2]) * static_cast<float>(dim_in.d[3]) / 1e9;
        total_gflops += gflops;
        ss << "pool " << dim_window.d[0] << "x" << dim_window.d[1] << "]";
        ss << " GFLOPs:" << gflops;
        ss << std::endl;
      } else if (layer_type == nvinfer1::LayerType::kRESIZE) {
        ss << "L" << i << " [resize]" << std::endl;
      }
    }
    ss << "Total " << total_gflops << " GFLOPs" << std::endl;
    ss << "Total " << total_params / 1000.0 / 1000.0 << " M params" << std::endl;

    return ss.str();
  };

private:
  //!< Calibration configuration
  std::unique_ptr<const CalibrationConfig> calib_config_;

  //!< Calibrator used for implicit quantization
  std::unique_ptr<nvinfer1::IInt8Calibrator> calibrator_;
};

}  // namespace tensorrt_common
}  // namespace autoware

#endif  // AUTOWARE__TENSORRT_COMMON__TENSORRT_CONV_CALIB_HPP_

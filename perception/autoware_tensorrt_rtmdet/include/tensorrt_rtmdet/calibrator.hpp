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

#ifndef TENSORRT_RTMDET__CALIBRATOR_HPP_
#define TENSORRT_RTMDET__CALIBRATOR_HPP_
#include "cuda_utils/cuda_check_error.hpp"
#include "cuda_utils/cuda_unique_ptr.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>

#include <NvInfer.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace autoware::tensorrt_rtmdet
{
class ImageStream
{
public:
  ImageStream(
    uint32_t batch_size, nvinfer1::Dims input_dims,
    const std::vector<std::string> calibration_images)
  : batch_size_(batch_size),
    calibration_images_(calibration_images),
    current_batch_(0),
    max_batches_(calibration_images.size() / batch_size_),
    input_dims_(input_dims)
  {
    batch_.resize(batch_size_ * input_dims_.d[1] * input_dims_.d[2] * input_dims_.d[3]);
  }

  uint32_t getBatchSize() const { return batch_size_; }

  uint32_t getMaxBatches() const { return max_batches_; }

  float * getBatch() { return &batch_[0]; }

  nvinfer1::Dims getInputDims() { return input_dims_; }

  /**
   * @brief Preprocess in calibration
   * @param[in] images input images
   * @param[in] input_dims input dimensions
   * @param[in] mean mean value for each channel
   * @param[in] std std value for each channel
   * @return vector<float> preprocessed data
   */
  std::vector<float> preprocess(
    const std::vector<cv::Mat> & images, nvinfer1::Dims input_dims, const std::vector<float> & mean,
    const std::vector<float> & std)
  {
    std::vector<float> input_h;
    auto batch_size = static_cast<uint32_t>(images.size());
    input_dims.d[0] = batch_size;
    const auto input_chan = static_cast<uint32_t>(input_dims.d[1]);
    const auto input_height = static_cast<uint32_t>(input_dims.d[2]);
    const auto input_width = static_cast<uint32_t>(input_dims.d[3]);
    std::vector<cv::Mat> dst_images;
    std::vector<float> scales;
    uint32_t volume = batch_size * input_chan * input_height * input_width;
    scales.clear();
    input_h.resize(volume);
    for (const auto & image : images) {
      cv::Mat dst_image;
      cv::resize(image, dst_image, cv::Size(640, 640));
      dst_image.convertTo(dst_image, CV_32F);
      dst_image -= cv::Scalar(103.53, 116.28, 123.675);
      dst_image /= cv::Scalar(57.375, 57.12, 58.395);
      dst_images.emplace_back(dst_image);
    }

    // NHWC
    const size_t strides_cv[4] = {
      static_cast<size_t>(input_width * input_chan * input_height),
      static_cast<size_t>(input_width * input_chan), static_cast<size_t>(input_chan), 1};
    // NCHW
    const size_t strides[4] = {
      static_cast<size_t>(input_height * input_width * input_chan),
      static_cast<size_t>(input_height * input_width), static_cast<size_t>(input_width), 1};
    for (uint32_t n = 0; n < batch_size; n++) {
      for (uint32_t h = 0; h < input_height; h++) {
        for (uint32_t w = 0; w < input_width; w++) {
          for (uint32_t c = 0; c < input_chan; c++) {
            // NHWC (needs RBswap)
            const size_t offset_cv =
              h * strides_cv[1] + w * strides_cv[2] + (input_chan - c - 1) * strides_cv[3];
            // NCHW
            const size_t offset = n * strides[0] + (c)*strides[1] + h * strides[2] + w * strides[3];
            input_h[offset] =
              (static_cast<float>(dst_images[n].data[offset_cv]) - mean[c]) / std[c];
          }
        }
      }
    }
    return input_h;
  }

  /**
   * @brief Decode data in calibration
   * @param[in] scale normalization (0.0-1.0)
   * @return bool succ or fail
   */
  bool next(const std::vector<float> & mean, const std::vector<float> & std)
  {
    if (current_batch_ == max_batches_) {
      return false;
    }
    for (uint32_t i = 0; i < batch_size_; ++i) {
      auto image =
        cv::imread(calibration_images_[batch_size_ * current_batch_ + i].c_str(), cv::IMREAD_COLOR);
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"), "Preprocess %s",
        calibration_images_[batch_size_ * current_batch_ + i].c_str());
      auto input = preprocess({image}, input_dims_, mean, std);
      batch_.insert(
        batch_.begin() + i * input_dims_.d[1] * input_dims_.d[2] * input_dims_.d[3], input.begin(),
        input.end());
    }

    ++current_batch_;
    return true;
  }
  /**
   * @brief Reset calibration
   */
  void reset() { current_batch_ = 0; }

private:
  uint32_t batch_size_;
  std::vector<std::string> calibration_images_;
  uint32_t current_batch_;
  uint32_t max_batches_;
  nvinfer1::Dims input_dims_;
  std::vector<float> batch_;
};

/**Percentile calibration using legacy calibrator*/
/**
 * @class Int8LegacyCalibrator
 * @brief Calibrator for Percentile
 * @warning We are confirming bug on Tegra like Xavier and Orin. We recommend use EntropyV2 or
 * MinMax calibrator
 */
class Int8LegacyCalibrator : public nvinfer1::IInt8LegacyCalibrator
{
public:
  Int8LegacyCalibrator(
    ImageStream & stream, std::string calibration_cache_file, std::string histogram_cache_file,
    const std::vector<float> & mean = {123.675, 116.28, 103.53},
    const std::vector<float> & std = {58.395, 57.12, 57.375}, bool read_cache = true,
    double quantile = 0.999999, double cutoff = 0.999999)
  : stream_(stream),
    calibration_cache_file_(std::move(calibration_cache_file)),
    histogram_cache_file_(std::move(histogram_cache_file)),
    read_cache_(read_cache)
  {
    auto d = stream_.getInputDims();
    input_count_ = stream_.getBatchSize() * d.d[1] * d.d[2] * d.d[3];
    CHECK_CUDA_ERROR(cudaMalloc(&device_input_, input_count_ * sizeof(float)));
    m_std_ = std;
    m_mean_ = mean;
    m_quantile_ = quantile;
    m_cutoff_ = cutoff;
    auto alg_type = getAlgorithm();
    switch (alg_type) {
      case (nvinfer1::CalibrationAlgoType::kLEGACY_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kLEGACY_CALIBRATION");
        break;
      case (nvinfer1::CalibrationAlgoType::kENTROPY_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kENTROPY_CALIBRATION");
        break;
      case (nvinfer1::CalibrationAlgoType::kENTROPY_CALIBRATION_2):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kENTROPY_CALIBRATION_2");
        break;
      case (nvinfer1::CalibrationAlgoType::kMINMAX_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kMINMAX_CALIBRATION");
        break;
      default:
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"), "No CalibrationAlgType");
        break;
    }
  }
  int getBatchSize() const noexcept override { return stream_.getBatchSize(); }

  virtual ~Int8LegacyCalibrator() { CHECK_CUDA_ERROR(cudaFree(device_input_)); }

  bool getBatch(void * bindings[], const char * names[], int nb_bindings) noexcept override
  {
    (void)names;
    (void)nb_bindings;

    if (!stream_.next(m_mean_, m_std_)) {
      return false;
    }
    try {
      CHECK_CUDA_ERROR(cudaMemcpy(
        device_input_, stream_.getBatch(), input_count_ * sizeof(float), cudaMemcpyHostToDevice));
    } catch (const std::exception & e) {
      // Do nothing
    }
    bindings[0] = device_input_;
    return true;
  }

  const void * readCalibrationCache(size_t & length) noexcept override
  {
    calib_cache_.clear();
    std::ifstream input(calibration_cache_file_, std::ios::binary);
    input >> std::noskipws;
    if (read_cache_ && input.good()) {
      std::copy(
        std::istream_iterator<char>(input), std::istream_iterator<char>(),
        std::back_inserter(calib_cache_));
    }

    length = calib_cache_.size();
    if (length) {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "Using cached calibration table to build the engine");
    } else {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "New calibration table will be created to build the engine");
    }
    return length ? &calib_cache_[0] : nullptr;
  }

  void writeCalibrationCache(const void * cache, size_t length) noexcept override
  {
    std::ofstream output(calibration_cache_file_, std::ios::binary);
    output.write(reinterpret_cast<const char *>(cache), length);
  }

  double getQuantile() const noexcept
  {
    RCLCPP_INFO(
      rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"), "Quantile %f", m_quantile_);
    return m_quantile_;
  }

  double getRegressionCutoff(void) const noexcept
  {
    RCLCPP_INFO(rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"), "Cutoff %f", m_cutoff_);
    return m_cutoff_;
  }

  const void * readHistogramCache(std::size_t & length) noexcept
  {
    hist_cache_.clear();
    std::ifstream input(histogram_cache_file_, std::ios::binary);
    input >> std::noskipws;
    if (read_cache_ && input.good()) {
      std::copy(
        std::istream_iterator<char>(input), std::istream_iterator<char>(),
        std::back_inserter(hist_cache_));
    }

    length = hist_cache_.size();
    if (length) {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "Using cached histogram table to build the engine");
    } else {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "New histogram table will be created to build the engine");
    }
    return length ? &hist_cache_[0] : nullptr;
  }
  void writeHistogramCache(void const * ptr, std::size_t length) noexcept
  {
    std::ofstream output(histogram_cache_file_, std::ios::binary);
    output.write(reinterpret_cast<const char *>(ptr), length);
  }

private:
  ImageStream stream_;
  const std::string calibration_cache_file_;
  const std::string histogram_cache_file_;
  bool read_cache_{true};
  size_t input_count_;
  void * device_input_{nullptr};
  std::vector<char> calib_cache_;
  std::vector<char> hist_cache_;
  // mean for preprocessing
  std::vector<float> m_mean_;
  // std for preprocessing
  std::vector<float> m_std_;
  double m_quantile_;
  double m_cutoff_;
};

/**
 * @class Int8LegacyCalibrator
 * @brief Calibrator for Percentile
 *
 */
class Int8EntropyCalibrator : public nvinfer1::IInt8EntropyCalibrator2
{
public:
  Int8EntropyCalibrator(
    ImageStream & stream, std::string calibration_cache_file,
    const std::vector<float> & mean = {123.675, 116.28, 103.53},
    const std::vector<float> & std = {58.395, 57.12, 57.375}, bool read_cache = true)
  : stream_(stream),
    calibration_cache_file_(std::move(calibration_cache_file)),
    read_cache_(read_cache)
  {
    auto d = stream_.getInputDims();
    input_count_ = stream_.getBatchSize() * d.d[1] * d.d[2] * d.d[3];
    m_std_ = std;
    m_mean_ = mean;

    CHECK_CUDA_ERROR(cudaMalloc(&device_input_, input_count_ * sizeof(float)));
    auto alg_type = getAlgorithm();
    switch (alg_type) {
      case (nvinfer1::CalibrationAlgoType::kLEGACY_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kLEGACY_CALIBRATION");
        break;
      case (nvinfer1::CalibrationAlgoType::kENTROPY_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kENTROPY_CALIBRATION");
        break;
      case (nvinfer1::CalibrationAlgoType::kENTROPY_CALIBRATION_2):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kENTROPY_CALIBRATION_2");
        break;
      case (nvinfer1::CalibrationAlgoType::kMINMAX_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kMINMAX_CALIBRATION");
        break;
      default:
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"), "No CalibrationAlgType");
        break;
    }
  }
  int getBatchSize() const noexcept override { return stream_.getBatchSize(); }

  virtual ~Int8EntropyCalibrator() { CHECK_CUDA_ERROR(cudaFree(device_input_)); }

  bool getBatch(void * bindings[], const char * names[], int nb_bindings) noexcept override
  {
    (void)names;
    (void)nb_bindings;

    if (!stream_.next(m_mean_, m_std_)) {
      return false;
    }
    try {
      CHECK_CUDA_ERROR(cudaMemcpy(
        device_input_, stream_.getBatch(), input_count_ * sizeof(float), cudaMemcpyHostToDevice));
    } catch (const std::exception & e) {
      // Do nothing
    }
    bindings[0] = device_input_;
    return true;
  }

  const void * readCalibrationCache(size_t & length) noexcept override
  {
    calib_cache_.clear();
    std::ifstream input(calibration_cache_file_, std::ios::binary);
    input >> std::noskipws;
    if (read_cache_ && input.good()) {
      std::copy(
        std::istream_iterator<char>(input), std::istream_iterator<char>(),
        std::back_inserter(calib_cache_));
    }

    length = calib_cache_.size();
    if (length) {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "Using cached calibration table to build the engine");
    } else {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "New calibration table will be created to build the engine");
    }
    return length ? &calib_cache_[0] : nullptr;
  }

  void writeCalibrationCache(const void * cache, size_t length) noexcept override
  {
    std::ofstream output(calibration_cache_file_, std::ios::binary);
    output.write(reinterpret_cast<const char *>(cache), length);
  }

private:
  ImageStream stream_;
  const std::string calibration_cache_file_;
  bool read_cache_{true};
  size_t input_count_;
  void * device_input_{nullptr};
  std::vector<char> calib_cache_;
  std::vector<char> hist_cache_;
  // mean for preprocessing
  std::vector<float> m_mean_;
  // std for preprocessing
  std::vector<float> m_std_;
};

/**
 * @class Int8MinMaxCalibrator
 * @brief Calibrator for MinMax
 *
 */
class Int8MinMaxCalibrator : public nvinfer1::IInt8MinMaxCalibrator
{
public:
  Int8MinMaxCalibrator(
    ImageStream & stream, std::string calibration_cache_file,
    const std::vector<float> & mean = {123.675, 116.28, 103.53},
    const std::vector<float> & std = {58.395, 57.12, 57.375}, bool read_cache = true)
  : stream_(stream),
    calibration_cache_file_(std::move(calibration_cache_file)),
    read_cache_(read_cache)
  {
    auto d = stream_.getInputDims();
    input_count_ = stream_.getBatchSize() * d.d[1] * d.d[2] * d.d[3];
    m_std_ = std;
    m_mean_ = mean;
    CHECK_CUDA_ERROR(cudaMalloc(&device_input_, input_count_ * sizeof(float)));
    auto alg_type = getAlgorithm();
    switch (alg_type) {
      case (nvinfer1::CalibrationAlgoType::kLEGACY_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kLEGACY_CALIBRATION");
        break;
      case (nvinfer1::CalibrationAlgoType::kENTROPY_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kENTROPY_CALIBRATION");
        break;
      case (nvinfer1::CalibrationAlgoType::kENTROPY_CALIBRATION_2):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kENTROPY_CALIBRATION_2");
        break;
      case (nvinfer1::CalibrationAlgoType::kMINMAX_CALIBRATION):
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
          "CalibrationAlgoType : kMINMAX_CALIBRATION");
        break;
      default:
        RCLCPP_INFO(
          rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"), "No CalibrationAlgType");
        break;
    }
  }
  int getBatchSize() const noexcept override { return stream_.getBatchSize(); }

  virtual ~Int8MinMaxCalibrator() { CHECK_CUDA_ERROR(cudaFree(device_input_)); }

  bool getBatch(void * bindings[], const char * names[], int nb_bindings) noexcept override
  {
    (void)names;
    (void)nb_bindings;

    if (!stream_.next(m_mean_, m_std_)) {
      return false;
    }
    try {
      CHECK_CUDA_ERROR(cudaMemcpy(
        device_input_, stream_.getBatch(), input_count_ * sizeof(float), cudaMemcpyHostToDevice));
    } catch (const std::exception & e) {
      // Do nothing
    }
    bindings[0] = device_input_;
    return true;
  }

  const void * readCalibrationCache(size_t & length) noexcept override
  {
    calib_cache_.clear();
    std::ifstream input(calibration_cache_file_, std::ios::binary);
    input >> std::noskipws;
    if (read_cache_ && input.good()) {
      std::copy(
        std::istream_iterator<char>(input), std::istream_iterator<char>(),
        std::back_inserter(calib_cache_));
    }

    length = calib_cache_.size();
    if (length) {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "Using cached calibration table to build the engine");
    } else {
      RCLCPP_INFO(
        rclcpp::get_logger("autoware_tensorrt_rtmdet_calibrator"),
        "New calibration table will be created to build the engine");
    }
    return length ? &calib_cache_[0] : nullptr;
  }

  void writeCalibrationCache(const void * cache, size_t length) noexcept override
  {
    std::ofstream output(calibration_cache_file_, std::ios::binary);
    output.write(reinterpret_cast<const char *>(cache), length);
  }

private:
  ImageStream stream_;
  const std::string calibration_cache_file_;
  bool read_cache_{true};
  size_t input_count_;
  void * device_input_{nullptr};
  std::vector<char> calib_cache_;
  std::vector<char> hist_cache_;
  std::vector<float> m_mean_;
  std::vector<float> m_std_;
};
}  // namespace autoware::tensorrt_rtmdet

#endif  // TENSORRT_RTMDET__CALIBRATOR_HPP_

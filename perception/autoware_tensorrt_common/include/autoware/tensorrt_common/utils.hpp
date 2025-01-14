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

#ifndef AUTOWARE__TENSORRT_COMMON__UTILS_HPP_
#define AUTOWARE__TENSORRT_COMMON__UTILS_HPP_

#include <NvInfer.h>
#include <sys/types.h>

#if (defined(_MSC_VER) or (defined(__GNUC__) and (7 <= __GNUC_MAJOR__)))
#include <filesystem>
namespace fs = ::std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = ::std::experimental::filesystem;
#endif

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace autoware
{
namespace tensorrt_common
{
constexpr std::array<std::string_view, 3> valid_precisions = {"fp32", "fp16", "int8"};

/**
 * @struct TrtCommonConfig
 * @brief Configuration to provide fine control regarding TensorRT builder
 */
struct TrtCommonConfig
{
  /** @brief Construct TrtCommonConfig, ONNX model path is mandatory.
   *
   * @param[in] onnx_path ONNX model path
   * @param[in] precision precision for inference
   * @param[in] engine_path TensorRT engine path
   * @param[in] max_workspace_size maximum workspace size for TensorRT
   * @param[in] dla_core_id DLA core ID
   * @param[in] profile_per_layer flag for per-layer profiler using IProfiler
   */
  explicit TrtCommonConfig(
    const std::string onnx_path, const std::string precision = "fp16",
    const std::string engine_path = "", const size_t max_workspace_size = (1ULL << 30U),
    const int32_t dla_core_id = -1, const bool profile_per_layer = false)
  : onnx_path(onnx_path),
    precision(precision),
    engine_path(engine_path),
    max_workspace_size(max_workspace_size),
    dla_core_id(dla_core_id),
    profile_per_layer(profile_per_layer)
  {
    validatePrecision();

    if (engine_path.empty()) {
      this->engine_path = onnx_path;
      this->engine_path.replace_extension(".engine");
    }
  }

  /**
   * @brief Validate configured TensorRT engine precision.
   */
  void validatePrecision() const
  {
    if (
      std::find(valid_precisions.begin(), valid_precisions.end(), precision) ==
      valid_precisions.end()) {
      std::stringstream message;
      message << "Invalid precision was specified: " << precision << std::endl
              << "Valid string is one of: [" << valid_precisions[0];
      for (size_t i = 1; i < valid_precisions.size(); ++i) {
        message << ", " << valid_precisions[i];
      }
      message << "] (case sensitive)" << std::endl;

      throw std::runtime_error(message.str());
    }
  }

  //!< @brief ONNX model path.
  const fs::path onnx_path;

  //!< @brief Precision for inference.
  const std::string precision;

  //!< @brief TensorRT engine path.
  fs::path engine_path;

  //!< @brief TensorRT max workspace size.
  const size_t max_workspace_size;

  //!< @brief DLA core ID that the process uses.
  const int32_t dla_core_id;

  //!< @brief Flag for per-layer profiler using IProfiler.
  const bool profile_per_layer;
};

/**
 * @brief Represents a tensor with its name or index.
 */
struct TensorInfo
{
  /**
   * @brief Construct TensorInfo with tensor name.
   *
   * @param[in] name Tensor name.
   */
  explicit TensorInfo(std::string name) : tensor_name(std::move(name)), tensor_index(-1) {}

  /**
   * @brief Construct TensorInfo with tensor index.
   *
   * @param[in] index Tensor index.
   */
  explicit TensorInfo(int32_t index) : tensor_index(index) {}

  /**
   * @brief Check if dimensions are equal.
   *
   * @param lhs Left-hand side tensor dimensions.
   * @param rhs Right-hand side tensor dimensions.
   * @return Whether dimensions are equal.
   */
  static bool dimsEqual(const nvinfer1::Dims & lhs, const nvinfer1::Dims & rhs)
  {
    if (lhs.nbDims != rhs.nbDims) return false;
    return std::equal(lhs.d, lhs.d + lhs.nbDims, rhs.d);  // NOLINT
  }

  /**
   * @brief Get printable representation of tensor dimensions.
   *
   * @param[in] dims Tensor dimensions.
   * @return String representation of tensor dimensions.
   */
  static std::string dimsRepr(const nvinfer1::Dims & dims)
  {
    if (dims.nbDims == 0) return "[]";
    std::string repr = "[" + std::to_string(dims.d[0]);
    for (int i = 1; i < dims.nbDims; ++i) {
      repr += ", " + std::to_string(dims.d[i]);
    }
    repr += "]";
    return repr;
  }

  //!< @brief Tensor name.
  std::string tensor_name;

  //!< @brief Tensor index.
  int32_t tensor_index;
};

/**
 * @brief Represents a network input/output tensor with its dimensions.
 *
 * Example usage:
 * @code
 * nvinfer1::Dims tensor_dims = {3, {1, 2, 3}};
 * NetworkIO input("input_tensor", tensor_dims);
 * NetworkIO output("output_tensor", tensor_dims);
 * bool is_equal = input == output;  // false
 * @endcode
 */
struct NetworkIO : public TensorInfo
{
  /**
   * @brief Construct NetworkIO with tensor name and dimensions.
   *
   * @param[in] name Tensor name.
   * @param[in] tensor_dims Tensor dimensions.
   */
  NetworkIO(std::string name, const nvinfer1::Dims & tensor_dims)
  : TensorInfo(std::move(name)), dims(tensor_dims)
  {
  }

  /**
   * @brief Construct NetworkIO with tensor index and dimensions.
   *
   * @param[in] index Tensor index.
   * @param[in] tensor_dims Tensor dimensions.
   */
  NetworkIO(int32_t index, const nvinfer1::Dims & tensor_dims)
  : TensorInfo(index), dims(tensor_dims)
  {
  }

  /**
   * @brief Get printable representation of NetworkIO.
   *
   * @return String representation of NetworkIO.
   */
  [[nodiscard]] std::string toString() const
  {
    std::stringstream ss;
    ss << tensor_name << " {" << TensorInfo::dimsRepr(dims) << "}";
    return ss.str();
  }

  /**
   * @brief Check if NetworkIO is equal to another NetworkIO.
   *
   * @param rhs Right-hand side NetworkIO.
   * @return Whether NetworkIO is equal to another NetworkIO.
   */
  bool operator==(const NetworkIO & rhs) const
  {
    if (tensor_name != rhs.tensor_name) return false;
    return dimsEqual(dims, rhs.dims);
  }

  /**
   * @brief Check if NetworkIO is not equal to another NetworkIO.
   *
   * @param rhs Right-hand side NetworkIO.
   * @return Whether NetworkIO is not equal to another NetworkIO.
   */
  bool operator!=(const NetworkIO & rhs) const { return !(*this == rhs); }

  /**
   * @brief Output NetworkIO to ostream.
   *
   * @param os Output stream.
   * @param io NetworkIO.
   * @return Output stream.
   */
  friend std::ostream & operator<<(std::ostream & os, const NetworkIO & io)
  {
    os << io.toString();
    return os;
  }

  //!< @brief Tensor dimensions.
  nvinfer1::Dims dims;
};

/**
 * @brief Represents a tensor optimization profile with minimum, optimal, and maximum dimensions.
 *
 * Example usage:
 * @code
 * nvinfer1::Dims min = {3, {1, 2, 3}};
 * nvinfer1::Dims opt = {3, {1, 3, 4}};
 * nvinfer1::Dims max = {3, {1, 4, 5}};
 * ProfileDims entry_1("tensor_name", min, opt, max);
 * ProfileDims entry_2("tensor_name", {3, {1, 2, 3}}, {3, {1, 3, 4}}, {3, {1, 4, 5}});
 * bool is_equal = entry_1 == entry_2;  // true
 * @endcode
 */
struct ProfileDims : public TensorInfo
{
  /**
   * @brief Construct ProfileDims with tensor name and dimensions.
   *
   * @param[in] name Tensor name.
   * @param[in] min Minimum dimensions for optimization profile.
   * @param[in] opt Optimal dimensions for optimization profile.
   * @param[in] max Maximum dimensions for optimization profile.
   */
  ProfileDims(
    std::string name, const nvinfer1::Dims & min, const nvinfer1::Dims & opt,
    const nvinfer1::Dims & max)
  : TensorInfo(std::move(name)), min_dims(min), opt_dims(opt), max_dims(max)
  {
  }

  /**
   * @brief Construct ProfileDims with tensor index and dimensions.
   *
   * @param[in] index Tensor index.
   * @param[in] min Minimum dimensions for optimization profile.
   * @param[in] opt Optimal dimensions for optimization profile.
   * @param[in] max Maximum dimensions for optimization profile.
   */
  ProfileDims(
    int32_t index, const nvinfer1::Dims & min, const nvinfer1::Dims & opt,
    const nvinfer1::Dims & max)
  : TensorInfo(index), min_dims(min), opt_dims(opt), max_dims(max)
  {
  }

  /**
   * @brief Get printable representation of ProfileDims.
   *
   * @return String representation of ProfileDims.
   */
  [[nodiscard]] std::string toString() const
  {
    std::ostringstream oss;
    oss << tensor_name << " {min " << TensorInfo::dimsRepr(min_dims) << ", opt "
        << TensorInfo::dimsRepr(opt_dims) << ", max " << TensorInfo::dimsRepr(max_dims) << "}";
    return oss.str();
  }

  /**
   * @brief Check if ProfileDims is equal to another ProfileDims.
   *
   * @param rhs Right-hand side ProfileDims.
   * @return Whether ProfileDims is equal to another ProfileDims.
   */
  bool operator==(const ProfileDims & rhs) const
  {
    if (tensor_name != rhs.tensor_name) return false;
    return dimsEqual(min_dims, rhs.min_dims) && dimsEqual(opt_dims, rhs.opt_dims) &&
           dimsEqual(max_dims, rhs.max_dims);
  }

  /**
   * @brief Check if ProfileDims is not equal to another ProfileDims.
   *
   * @param rhs Right-hand side ProfileDims.
   * @return Whether ProfileDims is not equal to another ProfileDims.
   */
  bool operator!=(const ProfileDims & rhs) const { return !(*this == rhs); }

  /**
   * @brief Output ProfileDims to ostream.
   *
   * @param os Output stream.
   * @param profile ProfileDims.
   * @return Output stream.
   */
  friend std::ostream & operator<<(std::ostream & os, const ProfileDims & profile)
  {
    os << profile.toString();
    return os;
  }

  //!< @brief Minimum dimensions for optimization profile.
  nvinfer1::Dims min_dims;

  //!< @brief Optimal dimensions for optimization profile.
  nvinfer1::Dims opt_dims;

  //!< @brief Maximum dimensions for optimization profile.
  nvinfer1::Dims max_dims;
};

//!< @brief Valid calibration types for TensorRT.
constexpr std::array<std::string_view, 4> valid_calib_type = {
  "Entropy", "Legacy", "Percentile", "MinMax"};

/**
 * @brief Configuration for implicit calibration.
 */
struct CalibrationConfig
{
  /**
   * @brief Construct CalibrationConfig with its parameters.
   *
   * @param[in] calib_type_str Calibration type.
   * @param[in] quantize_first_layer Flag for partial quantization in first layer.
   * @param[in] quantize_last_layer Flag for partial quantization in last layer.
   * @param[in] clip_value Clip value for implicit quantization.
   */
  explicit CalibrationConfig(
    const std::string & calib_type_str = "MinMax", const bool quantize_first_layer = false,
    const bool quantize_last_layer = false, const double clip_value = 0.0)
  : calib_type_str(calib_type_str),
    quantize_first_layer(quantize_first_layer),
    quantize_last_layer(quantize_last_layer),
    clip_value(clip_value)
  {
    if (
      std::find(valid_calib_type.begin(), valid_calib_type.end(), calib_type_str) ==
      valid_calib_type.end()) {
      throw std::runtime_error(
        "Invalid calibration type was specified: " + std::string(calib_type_str) +
        "\nValid value is one of: [Entropy, (Legacy | Percentile), MinMax]");
    }
  }

  //!< @brief type of calibration
  const std::string calib_type_str;

  //!< @brief flag for partial quantization in first layer
  const bool quantize_first_layer;

  //!< @brief flag for partial quantization in last layer
  const bool quantize_last_layer;

  //!< @brief clip value for implicit quantization
  const double clip_value;
};

}  // namespace tensorrt_common
}  // namespace autoware

#endif  // AUTOWARE__TENSORRT_COMMON__UTILS_HPP_

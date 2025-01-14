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

#ifndef AUTOWARE__TENSORRT_COMMON__PROFILER_HPP_
#define AUTOWARE__TENSORRT_COMMON__PROFILER_HPP_

#include <NvInfer.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace autoware
{
namespace tensorrt_common
{

/**
 * @class Profiler
 * @brief Collect per-layer profile information, assuming times are reported in the same order.
 */
class Profiler : public nvinfer1::IProfiler
{
public:
  /**
   * @struct Record
   * @brief Record of layer profile information.
   */
  struct Record
  {
    float time{0};
    int count{0};
    float min_time{-1.0};
    int index;
  };

  /**
   * @brief Construct Profiler.
   *
   * @param[in] src_profilers Source profilers to merge.
   */
  explicit Profiler(const std::vector<Profiler> & src_profilers = std::vector<Profiler>());

  /**
   * @brief Report layer time.
   *
   * @param[in] layerName Layer name.
   * @param[in] ms Time in milliseconds.
   */
  void reportLayerTime(const char * layerName, float ms) noexcept final;

  /**
   * @brief Get printable representation of Profiler.
   *
   * @return String representation for current state of Profiler.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * @brief Set per-layer profile information for model.
   *
   * @param[in] layer Layer to set profile information.
   */
  virtual void setProfDict([[maybe_unused]] nvinfer1::ILayer * layer) noexcept {};

  /**
   * @brief Output Profiler to ostream.
   *
   * @param[out] out Output stream.
   * @param[in] value Profiler to output.
   * @return Output stream.
   */
  friend std::ostream & operator<<(std::ostream & out, const Profiler & value);

private:
  //!< @brief Profile information for layers.
  std::map<std::string, Record> profile_;

  //!< @brief Index for layer.
  int index_;
};
}  // namespace tensorrt_common
}  // namespace autoware
#endif  // AUTOWARE__TENSORRT_COMMON__PROFILER_HPP_

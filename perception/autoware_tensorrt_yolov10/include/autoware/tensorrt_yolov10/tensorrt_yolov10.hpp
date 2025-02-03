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

#ifndef AUTOWARE__TENSORRT_YOLOV10__TENSORRT_YOLOV10_HPP_
#define AUTOWARE__TENSORRT_YOLOV10__TENSORRT_YOLOV10_HPP_

#include <autoware/tensorrt_common/tensorrt_common.hpp>
#include <cuda_utils/cuda_unique_ptr.hpp>
#include <cuda_utils/stream_unique_ptr.hpp>
#include <opencv2/opencv.hpp>

#include <memory>
#include <string>
#include <vector>

namespace autoware::tensorrt_yolov10
{
using cuda_utils::CudaUniquePtr;
using cuda_utils::CudaUniquePtrHost;
using cuda_utils::makeCudaStream;
using cuda_utils::StreamUniquePtr;

struct Object
{
  int32_t x_offset;
  int32_t y_offset;
  int32_t height;
  int32_t width;
  float score;
  int32_t type;
};

using ObjectArray = std::vector<Object>;
using ObjectArrays = std::vector<ObjectArray>;

class TrtYolov10
{
public:
  TrtYolov10(
    const std::string & model_path, const std::string & precision, const int num_class = 8,
    const float score_threshold = 0.8,
    const tensorrt_common::BuildConfig build_config = tensorrt_common::BuildConfig(),
    const bool use_gpu_preprocess = false, const uint8_t gpu_id = 0,
    std::string calibration_image_list_file = std::string(), const double norm_factor = 1.0,
    [[maybe_unused]] const std::string & cache_dir = "",
    const tensorrt_common::BatchConfig & batch_config = {1, 1, 1},
    const size_t max_workspace_size = (1 << 30));

  ~TrtYolov10();

  bool setCudaDeviceId(const uint8_t gpu_id);

  /**
   * @brief return a flag for gpu initialization
   */
  bool isGPUInitialized() const { return is_gpu_initialized_; }

  /**
   * @brief run inference including pre-process and post-process
   * @param[out] objects results for object detection
   * @param[in] images batched images
   */
  bool doInference(const std::vector<cv::Mat> & images, ObjectArrays & objects);

  //   /**
  //    * @brief allocate buffer for preprocess on GPU
  //    * @param[in] width original image width
  //    * @param[in] height original image height
  //    * @warning if we don't allocate buffers using it, "preprocessGpu" allocates buffers at the
  //    * beginning
  //    */
  //   void initPreprocessBuffer(int width, int height);

  //   /**
  //    * @brief output TensorRT profiles for each layer
  //    */
  //   void printProfiling(void);

  void preProcess(cv::Mat * img, int length, float * factor, std::vector<float> & data);

  void preprocess(const std::vector<cv::Mat> & images, std::vector<float> & data);
  ObjectArray postprocess(float * result, float factor);

  //   /**
  //    * @brief run preprocess on GPU
  //    * @param[in] images batching images
  //    */
  //   void preprocessGpu(const std::vector<cv::Mat> & images);

  bool feedforward(const std::vector<cv::Mat> & images, ObjectArrays & objects);

  std::unique_ptr<tensorrt_common::TrtCommon> trt_common_;

  std::vector<float> input_h_;
  CudaUniquePtr<float[]> input_d_;

  size_t out_elem_num_;
  size_t out_elem_num_per_batch_;
  CudaUniquePtr<float[]> out_d_;
  CudaUniquePtrHost<float[]> out_h_;
  StreamUniquePtr stream_;

  int32_t max_detections_;
  std::vector<float> scales_;

  int num_class_;
  float score_threshold_;
  int batch_size_;

  // GPU id for inference
  const uint8_t gpu_id_;
  // flag for gpu initialization
  bool is_gpu_initialized_;
  // host buffer for preprocessing on GPU
  CudaUniquePtrHost<unsigned char[]> image_buf_h_;
  // device buffer for preprocessing on GPU
  CudaUniquePtr<unsigned char[]> image_buf_d_;

  int src_width_;
  int src_height_;

  CudaUniquePtr<unsigned char[]> argmax_buf_d_;
  double norm_factor_;
  int multitask_;
  bool use_gpu_preprocess_;

  float factor_ = 1.0;
};

}  // namespace autoware::tensorrt_yolov10

#endif  // AUTOWARE__TENSORRT_YOLOV10__TENSORRT_YOLOV10_HPP_

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

// cspell: ignore RTMDET
#ifndef AUTOWARE__TENSORRT_RTMDET__TENSORRT_RTMDET_HPP_
#define AUTOWARE__TENSORRT_RTMDET__TENSORRT_RTMDET_HPP_

#include <autoware/cuda_utils/cuda_unique_ptr.hpp>
#include <autoware/cuda_utils/stream_unique_ptr.hpp>
#include <autoware/tensorrt_common/tensorrt_common.hpp>
#include <autoware/tensorrt_common/tensorrt_conv_calib.hpp>
#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>

#include <cv_bridge/cv_bridge.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

class TrtRTMDetTest;

namespace autoware::tensorrt_rtmdet
{
using autoware::cuda_utils::CudaUniquePtr;
using autoware::cuda_utils::CudaUniquePtrHost;
using autoware::cuda_utils::makeCudaStream;
using autoware::cuda_utils::StreamUniquePtr;

/**
 * @brief Represents a detected object.
 *
 * This struct stores bounding box coordinates, class id, mask index, score of detected object.
 */
struct Object
{
  uint32_t x1;
  uint32_t y1;
  uint32_t x2;
  uint32_t y2;
  uint32_t class_id;
  uint32_t mask_index;
  float score;
};

using ObjectArray = std::vector<Object>;
using ObjectArrays = std::vector<ObjectArray>;

/**
 * @brief Represents a color for a class.
 *
 * This struct stores class name, color and label id for a label.
 * Read from a color map csv file.
 */
using LabelColor = struct LabelColor
{
  std::string class_name;
  cv::Vec3b color;
  uint8_t label_id;
};

using ColorMap = std::map<uint32_t, LabelColor>;
using autoware::tensorrt_common::CalibrationConfig;
using autoware::tensorrt_common::TrtCommonConfig;

class TrtRTMDet
{
public:
  TrtRTMDet(
    TrtCommonConfig & trt_config, const ColorMap & color_map, const float score_threshold = 0.3,
    const float nms_threshold = 0.7, const float mask_threshold = 200.0,
    const std::string & calibration_image_list_file_path = std::string(),
    const double norm_factor = 1.0, const std::vector<float> & mean = {103.53, 116.28, 123.675},
    const std::vector<float> & std = {57.375, 57.12, 58.395},
    [[maybe_unused]] const std::string & cache_dir = "",
    const std::vector<std::string> & plugin_paths = {},
    const CalibrationConfig & calib_config = CalibrationConfig{});

  ~TrtRTMDet() noexcept;

  /**
   * @brief Perform inference.
   *
   * This function calls preprocess and feedforward functions to perform inference.
   *
   * @param[in] images a vector of input images.
   * @param[out] objects a vector of detected objects.
   * @param[out] mask a segmentation mask.
   * @param[out] class_ids a vector of class ids.
   * @return true if inference is successful.
   */
  bool do_inference(
    const std::vector<cv::Mat> & images, ObjectArrays & objects, cv::Mat & mask,
    std::vector<uint8_t> & class_ids);

  /**
   * @brief Calculate intersection over union.
   *
   * This function calculates the intersection over union (IoU) between two bounding boxes.
   * Input bounding boxes contains the bounding box and class id.
   *
   * @param[in] a First object.
   * @param[in] b Second object.
   * @return IoU value.
   */
  [[nodiscard]] static float intersection_over_union(const Object & a, const Object & b);

  /**
   * @brief Perform non-maximum suppression.
   *
   * This function performs non-maximum suppression (NMS) on the detected objects.
   * The detected objects are sorted by score and the NMS is applied to remove overlapping boxes.
   *
   * Since the model architecture has own NMS layer, this function added for overlapped boxes which
   * have different class.
   *
   * @param[in] input_objects a vector of detected objects.
   * @param[out] output_objects a vector of detected objects after NMS.
   */
  static void nms_sorted_bboxes(
    const ObjectArray & input_objects, ObjectArray & output_objects, const float & nms_threshold);

  void print_profiling();

private:
  /**
   * @brief Preprocess input images on GPU.
   *
   * This function takes a vector of input images and preprocesses them on GPU.
   * The images are resized to the input size of the model, normalized and stored in a buffer.
   *
   * @param[in] images a vector of input images.
   */
  void preprocess_gpu(const std::vector<cv::Mat> & images);

  /**
   * @brief Inference with TensorRT.
   *
   * This function performs inference with TensorRT.
   * The input images are fed to the model and the output is stored in the output buffers.
   * The output buffers are then post-processed to get the detected objects and segmentation mask.
   *
   * @param[in] images a vector of input images.
   * @param[out] objects a vector of detected objects.
   * @param[out] mask a segmentation mask.
   * @param[out] class_ids a vector of class ids.
   * @return true if inference is successful.
   */
  bool feedforward(
    const std::vector<cv::Mat> & images, ObjectArrays & objects, cv::Mat & mask,
    std::vector<uint8_t> & class_ids);

  std::unique_ptr<autoware::tensorrt_common::TrtConvCalib> trt_common_;

  // Host and device pointers for inputs
  std::vector<float> input_h_;
  CudaUniquePtr<float[]> input_d_;

  // Device pointer for outputs
  CudaUniquePtr<float[]> out_detections_d_;
  CudaUniquePtr<int32_t[]> out_labels_d_;
  CudaUniquePtr<float[]> out_masks_d_;

  // Host pointer for outputs
  std::unique_ptr<float[]> out_detections_h_;
  std::unique_ptr<int32_t[]> out_labels_h_;
  std::unique_ptr<float[]> out_masks_h_;

  StreamUniquePtr stream_{makeCudaStream()};

  // scale factor for input image
  float scale_width_;
  float scale_height_;

  // size of input image for model
  uint32_t model_input_width_;
  uint32_t model_input_height_;

  const float score_threshold_;
  const float nms_threshold_;
  const float mask_threshold_;
  int batch_size_;

  // maximum number of detections, read from model
  uint32_t max_detections_;

  // host buffer for preprocessing on GPU
  CudaUniquePtrHost<unsigned char[]> image_buf_h_;
  // device buffer for preprocessing on GPU
  CudaUniquePtr<unsigned char[]> image_buf_d_;
  // normalization factor used for preprocessing
  const double norm_factor_;

  // size of input image for preprocessing, check if input image size is changed
  int32_t src_width_;
  int32_t src_height_;

  // mean and std for preprocessing
  const std::vector<float> mean_;
  const std::vector<float> std_;

  // Segmentation map, stores information for each class
  const ColorMap color_map_;

public:
  friend TrtRTMDetTest;
};
}  // namespace autoware::tensorrt_rtmdet

#endif  // AUTOWARE__TENSORRT_RTMDET__TENSORRT_RTMDET_HPP_

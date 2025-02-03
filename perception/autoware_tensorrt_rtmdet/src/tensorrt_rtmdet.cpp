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

#include "autoware/tensorrt_rtmdet/tensorrt_rtmdet.hpp"

#include "autoware/tensorrt_rtmdet/calibrator.hpp"
#include "autoware/tensorrt_rtmdet/preprocess.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Load calibration image list from a text file.
 *
 * @param filename Path to the text file.
 * @return List of image paths.
 */
std::vector<std::string> load_calibration_image_list(const std::string & filename)
{
  if (filename.empty()) {
    RCLCPP_ERROR(
      rclcpp::get_logger("autoware_tensorrt_rtmdet"),
      "Calibration image list is empty. Please set calibration_image_list_path.");
    return {};
  }
  if (!std::experimental::filesystem::exists(std::experimental::filesystem::path(filename))) {
    RCLCPP_ERROR(
      rclcpp::get_logger("autoware_tensorrt_rtmdet"), "failed to open %s", filename.c_str());
    return {};
  }

  std::ifstream txt_file(filename);

  std::string line;
  std::vector<std::string> image_list;
  while (std::getline(txt_file, line)) {
    image_list.push_back(line);
  }

  txt_file.close();
  return image_list;
}

namespace autoware::tensorrt_rtmdet
{
TrtRTMDet::TrtRTMDet(
  TrtCommonConfig & trt_config, const ColorMap & color_map, const float score_threshold,
  const float nms_threshold, const float mask_threshold,
  const std::string & calibration_image_list_file_path, const double norm_factor,
  const std::vector<float> & mean, const std::vector<float> & std,
  [[maybe_unused]] const std::string & cache_dir, const std::vector<std::string> & plugin_paths,
  const CalibrationConfig & calib_config)
: score_threshold_{score_threshold},
  nms_threshold_{nms_threshold},
  mask_threshold_{mask_threshold},
  norm_factor_{norm_factor},
  mean_{mean},
  std_{std},
  color_map_{color_map}
{
  src_width_ = -1;
  src_height_ = -1;
  scale_width_ = -1;
  scale_height_ = -1;

  trt_common_ = std::make_unique<autoware::tensorrt_common::TrtConvCalib>(
    trt_config, std::make_shared<autoware::tensorrt_common::Profiler>(), plugin_paths);

  const auto network_input_dims = trt_common_->getTensorShape(0);
  batch_size_ = network_input_dims.d[0];
  const auto input_channel = network_input_dims.d[1];
  const auto input_height = network_input_dims.d[2];
  const auto input_width = network_input_dims.d[3];

  auto profile_dims_ptr = std::make_unique<std::vector<autoware::tensorrt_common::ProfileDims>>();

  std::vector<autoware::tensorrt_common::ProfileDims> profile_dims{
    autoware::tensorrt_common::ProfileDims(
      0, {4, {batch_size_, input_channel, input_height, input_width}},
      {4, {batch_size_, input_channel, input_height, input_width}},
      {4, {batch_size_, input_channel, input_height, input_width}})};
  *profile_dims_ptr = profile_dims;

  if (trt_config.precision == "int8") {
    std::vector<std::string> calibration_images =
      load_calibration_image_list(calibration_image_list_file_path);

    tensorrt_rtmdet::ImageStream stream(batch_size_, network_input_dims, calibration_images);

    fs::path calibration_table{trt_config.onnx_path};
    std::string ext = "EntropyV2-calibration.table";
    calibration_table.replace_extension(ext);

    fs::path histogram_table{trt_config.onnx_path};
    ext = "histogram.table";
    histogram_table.replace_extension(ext);

    std::unique_ptr<nvinfer1::IInt8Calibrator> calibrator;
    calibrator = std::make_unique<tensorrt_rtmdet::Int8EntropyCalibrator>(
      stream, calibration_table, mean_, std_);

    if (!trt_common_->setupWithCalibrator(
          std::move(calibrator), calib_config, std::move((profile_dims_ptr)))) {
      throw std::runtime_error("Failed to setup TensorRT engine with calibrator");
    }
  } else {
    if (!trt_common_->setup(std::move(profile_dims_ptr))) {
      throw std::runtime_error("Failed to setup TensorRT engine");
    }
  }

  const auto input_dims = trt_common_->getTensorShape(0);
  const auto out_scores_dims = trt_common_->getTensorShape(3);

  max_detections_ = out_scores_dims.d[1];
  model_input_height_ = input_dims.d[2];
  model_input_width_ = input_dims.d[3];

  input_d_ = autoware::cuda_utils::make_unique<float[]>(
    batch_size_ * input_dims.d[1] * input_dims.d[2] * input_dims.d[3]);
  out_detections_d_ = autoware::cuda_utils::make_unique<float[]>(batch_size_ * max_detections_ * 5);
  out_labels_d_ = autoware::cuda_utils::make_unique<int32_t[]>(batch_size_ * max_detections_);
  out_masks_d_ = autoware::cuda_utils::make_unique<float[]>(
    batch_size_ * max_detections_ * model_input_width_ * model_input_height_);

  out_detections_h_ = std::make_unique<float[]>(batch_size_ * max_detections_ * 5);
  out_labels_h_ = std::make_unique<int32_t[]>(batch_size_ * max_detections_);
  out_masks_h_ = std::make_unique<float[]>(
    batch_size_ * max_detections_ * model_input_width_ * model_input_height_);
}

TrtRTMDet::~TrtRTMDet() noexcept
{
  if (image_buf_h_) {
    image_buf_h_.reset();
  }
  if (image_buf_d_) {
    image_buf_d_.reset();
  }
}

void TrtRTMDet::print_profiling()
{
  trt_common_->printProfiling();
}

void TrtRTMDet::preprocess_gpu(const std::vector<cv::Mat> & images)
{
  const auto batch_size = images.size();
  auto input_dims = trt_common_->getTensorShape(0);

  input_dims.d[0] = static_cast<int32_t>(batch_size);
  for (const auto & image : images) {
    // if size of source input has been changed...
    int width = image.cols;
    int height = image.rows;
    if (src_width_ != -1 || src_height_ != -1) {
      if (width != src_width_ || height != src_height_) {
        // Free cuda memory to reallocate
        if (image_buf_h_) {
          image_buf_h_.reset();
        }
        if (image_buf_d_) {
          image_buf_d_.reset();
        }
      }
    }
    src_width_ = width;
    src_height_ = height;
  }
  if (!image_buf_h_) {
    trt_common_->setInputShape(0, input_dims);
    scale_width_ = 0;
    scale_height_ = 0;
  }
  const auto input_height = static_cast<float>(input_dims.d[2]);
  const auto input_width = static_cast<float>(input_dims.d[3]);
  int b = 0;
  for (const auto & image : images) {
    if (!image_buf_h_) {
      scale_width_ = input_width / static_cast<float>(image.cols);
      scale_height_ = input_height / static_cast<float>(image.rows);
      image_buf_h_ = autoware::cuda_utils::make_unique_host<unsigned char[]>(
        image.cols * image.rows * 3 * batch_size, cudaHostAllocWriteCombined);
      image_buf_d_ = autoware::cuda_utils::make_unique<unsigned char[]>(
        image.cols * image.rows * 3 * batch_size);
    }
    int index = b * image.cols * image.rows * 3;
    // Copy into pinned memory
    memcpy(
      image_buf_h_.get() + index, &image.data[0],
      image.cols * image.rows * 3 * sizeof(unsigned char));
    b++;
  }
  // Copy into device memory
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    image_buf_d_.get(), image_buf_h_.get(),
    images[0].cols * images[0].rows * 3 * batch_size * sizeof(unsigned char),
    cudaMemcpyHostToDevice, *stream_));
  // Preprocess on GPU
  resize_bilinear_letterbox_nhwc_to_nchw32_batch_gpu(
    input_d_.get(), image_buf_d_.get(), static_cast<int32_t>(input_width),
    static_cast<int32_t>(input_height), images[0].cols, images[0].rows,
    static_cast<int32_t>(batch_size), mean_.data(), std_.data(), *stream_);
}

bool TrtRTMDet::do_inference(
  const std::vector<cv::Mat> & images, ObjectArrays & objects, cv::Mat & mask,
  std::vector<uint8_t> & class_ids)
{
  preprocess_gpu(images);

  return feedforward(images, objects, mask, class_ids);
}

bool TrtRTMDet::feedforward(
  const std::vector<cv::Mat> & images, ObjectArrays & objects, cv::Mat & mask,
  std::vector<uint8_t> & class_ids)
{
  trt_common_->enqueueV3(*stream_);

  const auto batch_size = images.size();
  out_detections_h_.reset(new float[batch_size_ * max_detections_ * 5]);
  out_labels_h_.reset(new int32_t[batch_size_ * max_detections_]);
  out_masks_h_.reset(new float[batch_size_ * 20 * model_input_width_ * model_input_height_]);

  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    out_detections_h_.get(), out_detections_d_.get(),
    sizeof(float) * batch_size_ * max_detections_ * 5, cudaMemcpyDeviceToHost, *stream_));
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    out_labels_h_.get(), out_labels_d_.get(), sizeof(int32_t) * batch_size_ * max_detections_,
    cudaMemcpyDeviceToHost, *stream_));
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    out_masks_h_.get(), out_masks_d_.get(),
    sizeof(float) * batch_size_ * 20 * model_input_width_ * model_input_height_,
    cudaMemcpyDeviceToHost, *stream_));

  cudaStreamSynchronize(*stream_);

  // POST PROCESSING
  objects.clear();
  for (size_t batch = 0; batch < batch_size; ++batch) {
    ObjectArray object_array;
    for (uint32_t index = 0; index < max_detections_; ++index) {
      if (out_detections_h_[(batch * max_detections_ * 5) + ((5 * index) + 4)] < score_threshold_) {
        break;
      }

      Object object{};
      object.mask_index = index;
      object.class_id = out_labels_h_[(batch * max_detections_) + index];
      object.x1 = static_cast<uint32_t>(
        out_detections_h_[(batch * max_detections_ * 5) + ((5 * index) + 0)] / scale_width_);
      object.y1 = static_cast<uint32_t>(
        out_detections_h_[(batch * max_detections_ * 5) + ((5 * index) + 1)] / scale_height_);
      object.x2 = static_cast<uint32_t>(
        out_detections_h_[(batch * max_detections_ * 5) + ((5 * index) + 2)] / scale_width_);
      object.y2 = static_cast<uint32_t>(
        out_detections_h_[(batch * max_detections_ * 5) + ((5 * index) + 3)] / scale_height_);
      object.score = out_detections_h_[(batch * max_detections_ * 5) + ((5 * index) + 4)];
      object_array.push_back(object);
    }
    ObjectArray nms_objects;
    nms_sorted_bboxes(object_array, nms_objects, nms_threshold_);

    objects.push_back(nms_objects);
  }

  // Create an instance segmentation mask.
  // The mask is an image with the same dimensions as the model input image,
  // where each pixel represents the class intensity.
  // The intensity of each pixel corresponds to the index of the class_array,
  // which stores the class IDs.
  mask = cv::Mat(
    static_cast<int32_t>(model_input_height_), static_cast<int32_t>(model_input_width_), CV_8UC1,
    cv::Scalar(0, 0, 0));
  uint8_t pixel_intensity = 1;  // 0 is reserved for background
  for (size_t batch = 0; batch < batch_size; ++batch) {
    for (const auto & object : objects.at(batch)) {
      cv::Mat object_mask(
        static_cast<int32_t>(model_input_height_), static_cast<int32_t>(model_input_width_), CV_32F,
        &out_masks_h_
          [(batch * 100 * model_input_width_ * model_input_height_) +
           (object.mask_index * model_input_width_ * model_input_height_)]);
      double min_val = 0;
      double max_val = 0;
      cv::minMaxLoc(object_mask, &min_val, &max_val);
      object_mask.convertTo(
        object_mask, CV_8U, 255.0 / (max_val - min_val), -min_val * 255.0 / (max_val - min_val));

      auto process_pixel = [&]([[maybe_unused]] cv::Vec3b & pixel, const int * position) -> void {
        int i = position[0];
        int j = position[1];

        if (object_mask.at<uchar>(i, j) > static_cast<int>(255 * mask_threshold_)) {
          mask.at<uint8_t>(i, j) = pixel_intensity;
        }
      };
      mask.forEach<cv::Vec3b>(process_pixel);
      class_ids.push_back(color_map_.at(object.class_id).label_id);
      pixel_intensity++;
    }
  }
  return true;
}

float TrtRTMDet::intersection_over_union(const Object & a, const Object & b)
{
  uint32_t x_left = std::max(a.x1, b.x1);
  uint32_t y_top = std::max(a.y1, b.y1);
  uint32_t x_right = std::min(a.x2, b.x2);
  uint32_t y_bottom = std::min(a.y2, b.y2);

  if (x_right < x_left || y_bottom < y_top) {
    return 0.0;
  }

  float intersection_area = ((x_right - x_left + 1) * (y_bottom - y_top + 1));
  float a_area = (a.x2 - a.x1 + 1) * (a.y2 - a.y1 + 1);
  float b_area = (b.x2 - b.x1 + 1) * (b.y2 - b.y1 + 1);

  return intersection_area / (a_area + b_area - intersection_area);
}

void TrtRTMDet::nms_sorted_bboxes(
  const ObjectArray & input_objects, ObjectArray & output_objects, const float & nms_threshold)
{
  std::vector<bool> suppressed(input_objects.size(), false);

  for (size_t i = 0; i < input_objects.size(); ++i) {
    if (suppressed.at(i)) continue;

    const Object & a = input_objects.at(i);
    output_objects.push_back(a);

    for (size_t j = i + 1; j < input_objects.size(); ++j) {
      if (suppressed.at(j)) continue;

      const Object & b = input_objects.at(j);
      if (intersection_over_union(a, b) > nms_threshold) {
        suppressed.at(j) = true;
      }
    }
  }
}
}  // namespace autoware::tensorrt_rtmdet

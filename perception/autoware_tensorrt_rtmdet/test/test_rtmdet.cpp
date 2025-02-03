// Copyright 2024 Autoware Foundation
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

// cspell: ignore libtrt
#include "ament_index_cpp/get_package_prefix.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include "autoware/tensorrt_rtmdet/tensorrt_rtmdet.hpp"
#include "autoware/tensorrt_rtmdet/tensorrt_rtmdet_node.hpp"

#include <autoware/cuda_utils/cuda_unique_ptr.hpp>
#include <autoware/cuda_utils/stream_unique_ptr.hpp>
#include <autoware/tensorrt_common/tensorrt_common.hpp>
#include <opencv2/opencv.hpp>

#include <dlfcn.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

template <typename T>
T get_pixel_value(const cv::Mat & image, int x, int y)
{
  if (x < 0 || x >= image.cols || y < 0 || y >= image.rows) {
    throw std::out_of_range("Coordinates are out of image bounds");
  }

  if (image.channels() == 1) {
    return image.at<T>(y, x);
  }
  return image.at<T>(y, x);
}

class TrtRTMDetTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    const auto home_path = std::string(getenv("HOME"));
    const auto autoware_data_path = home_path + "/autoware_data";

    const auto rtmdet_share_dir =
      ament_index_cpp::get_package_share_directory("autoware_tensorrt_rtmdet");
    const auto plugin_dir = ament_index_cpp::get_package_prefix("trt_batched_nms");

    const auto test_image_path = rtmdet_share_dir + "/data/test_image.jpg";
    const auto test_color_map_path = rtmdet_share_dir + "/data/test_color_map.csv";
    const auto plugin_path = plugin_dir + "/lib/libtrt_batched_nms_plugin.so";
    const auto model_path = autoware_data_path + "/tensorrt_rtmdet/rtmdet_x.onnx";

    const double norm_factor = 1.0;
    const std::string cache_dir;
    const std::vector<std::string> plugin_paths{plugin_path};

    test_image_ = cv::imread(test_image_path, cv::IMREAD_COLOR);

    autoware::tensorrt_common::TrtCommonConfig trt_config(
      model_path, "fp16", "", (1ULL << 30U), -1, false);

    autoware::tensorrt_common::CalibrationConfig calib_config("Entropy", false, false, false);

    // Test color map only includes 3 classes. It is enough for the test.
    color_map_ = autoware::tensorrt_rtmdet::TrtRTMDetNode::read_color_map_file(test_color_map_path);

    trt_rtmdet_ = std::make_unique<autoware::tensorrt_rtmdet::TrtRTMDet>(
      trt_config, color_map_, score_threshold_, nms_threshold_, mask_threshold_, "", norm_factor,
      mean_, std_, cache_dir, plugin_paths, calib_config);
  }

public:
  /**
   * @brief Wrapper function to test the autoware::tensorrt_rtmdet::TrtRTMDet::preprocess_gpu
   * function.
   *
   * @param input_image The input images.
   */
  void preprocess_gpu(const std::vector<cv::Mat> & input_images) const
  {
    trt_rtmdet_->preprocess_gpu(input_images);
  }

  /**
   * @brief Wrapper function to get the image buffer on the host.
   *
   * @return The image buffer on the host.
   */
  [[nodiscard]] const autoware::cuda_utils::CudaUniquePtrHost<unsigned char[]> &
  get_image_buffer_h() const
  {
    return trt_rtmdet_->image_buf_h_;
  }

  /**
   * @brief Wrapper function to get the image buffer on the device.
   *
   * @return The image buffer on the device.
   */
  [[nodiscard]] const autoware::cuda_utils::CudaUniquePtr<unsigned char[]> & get_image_buffer_d()
    const
  {
    return trt_rtmdet_->image_buf_d_;
  }

  /**
   * @brief Wrapper function to get the input dimensions.
   *
   * @return The input dimensions.
   */
  [[nodiscard]] nvinfer1::Dims get_input_dimensions() const
  {
    return trt_rtmdet_->trt_common_->getTensorShape(0);
  }

  std::unique_ptr<autoware::tensorrt_rtmdet::TrtRTMDet> trt_rtmdet_;

  // Common parameters to initialize the RTMDet model
  const std::vector<float> mean_{103.53, 116.28, 123.675};
  const std::vector<float> std_{57.375, 57.12, 58.395};
  const float score_threshold_{0.3};
  const float nms_threshold_{0.3};
  const float mask_threshold_{0.7};
  const std::string precision_{"fp16"};

  cv::Mat test_image_;

  autoware::tensorrt_rtmdet::ColorMap color_map_;
};

TEST_F(TrtRTMDetTest, TestPreprocessGPU)
{
  const auto input_image = test_image_.clone();
  const auto input_images = std::vector<cv::Mat>{input_image};

  ASSERT_NO_THROW(preprocess_gpu(input_images));

  // Validate CUDA memory setup by checking pointers.
  EXPECT_NE(get_image_buffer_h(), nullptr);
  EXPECT_NE(get_image_buffer_d(), nullptr);
}

TEST_F(TrtRTMDetTest, TestFeedForward)
{
  const auto input_image = test_image_.clone();
  const auto input_images = std::vector<cv::Mat>{input_image};
  preprocess_gpu(input_images);

  autoware::tensorrt_rtmdet::ObjectArrays object_arrays;
  cv::Mat mask;
  std::vector<uint8_t> class_ids;

  const auto success = trt_rtmdet_->do_inference(input_images, object_arrays, mask, class_ids);
  const auto object_size = object_arrays[0].size();

  // Check if the inference was successful
  EXPECT_TRUE(success);

  // Check if the object size is correct
  EXPECT_EQ(object_size, 1);

  // Check if the output objects class ids is correct (the label_id is derived from the COCO
  // dataset.)
  EXPECT_EQ(object_arrays.at(0).at(0).class_id, 2);

  // Check if the output objects positions are correct
  EXPECT_EQ(object_arrays.at(0).at(0).x1, 138);
  EXPECT_EQ(object_arrays.at(0).at(0).x2, 1026);
  EXPECT_EQ(object_arrays.at(0).at(0).y1, 201);
  EXPECT_EQ(object_arrays.at(0).at(0).y2, 509);

  // Check if the output objects class ids is correct (label_id, which is used to process the
  // outputs in Autoware.)
  EXPECT_EQ(class_ids.at(0), 1);

  // Check if the one of the mask pixel which is in the object is not zero (zero represents the
  // background.) There is a car in the test image, so the mask pixel value should not be zero.
  const auto mask_pixel_value = get_pixel_value<uint8_t>(mask, 320, 413);
  EXPECT_NE(mask_pixel_value, 0);
}

TEST_F(TrtRTMDetTest, TestIntersectionOverUnion)
{
  const autoware::tensorrt_rtmdet::Object obj1 = {10, 10, 50, 50, 1, 0, 0.8};
  const autoware::tensorrt_rtmdet::Object obj2 = {20, 20, 60, 60, 1, 0, 0.7};

  float iou = autoware::tensorrt_rtmdet::TrtRTMDet::intersection_over_union(obj1, obj2);
  EXPECT_NEAR(iou, 0.4, 0.01);
}

TEST_F(TrtRTMDetTest, TestNmsSortedBboxes)
{
  const autoware::tensorrt_rtmdet::Object obj1 = {10, 10, 50, 50, 0, 0, 0.8};
  const autoware::tensorrt_rtmdet::Object obj2 = {10, 10, 50, 50, 1, 0, 0.3};
  const autoware::tensorrt_rtmdet::Object obj3 = {12, 12, 52, 52, 1, 0, 0.6};
  const autoware::tensorrt_rtmdet::Object obj4 = {8, 8, 48, 48, 1, 0, 0.5};

  std::vector<autoware::tensorrt_rtmdet::Object> input_objects = {obj1, obj2, obj3, obj4};
  std::vector<autoware::tensorrt_rtmdet::Object> output_objects;
  const float nms_threshold = 0.5;

  autoware::tensorrt_rtmdet::TrtRTMDet::nms_sorted_bboxes(
    input_objects, output_objects, nms_threshold);

  // Check if the number of objects is correct after NMS
  EXPECT_EQ(output_objects.size(), 1);

  // After NMS, only the `obj1` should be kept.
  EXPECT_EQ(output_objects.at(0).x1, 10);
  EXPECT_EQ(output_objects.at(0).x2, 50);
  EXPECT_EQ(output_objects.at(0).y1, 10);
  EXPECT_EQ(output_objects.at(0).y2, 50);
  EXPECT_EQ(output_objects.at(0).class_id, 0);
  EXPECT_EQ(output_objects.at(0).score, static_cast<float>(0.8));
}

TEST_F(TrtRTMDetTest, TestPluginLoad)
{
  const auto plugin_dir = ament_index_cpp::get_package_prefix("trt_batched_nms");
  const auto plugin_path = plugin_dir + "/lib/libtrt_batched_nms_plugin.so";

  int32_t flags{RTLD_LAZY};
  void * handle = dlopen(plugin_path.c_str(), flags);

  EXPECT_NE(handle, nullptr);
}

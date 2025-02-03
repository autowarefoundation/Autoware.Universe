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
#ifndef AUTOWARE__TENSORRT_RTMDET__TENSORRT_RTMDET_NODE_HPP_
#define AUTOWARE__TENSORRT_RTMDET__TENSORRT_RTMDET_NODE_HPP_

#include "autoware/tensorrt_rtmdet/tensorrt_rtmdet.hpp"

#include <autoware/universe_utils/ros/debug_publisher.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <image_transport/image_transport.hpp>
#include <rclcpp/rclcpp.hpp>

#include "autoware_internal_perception_msgs/msg/segmentation_mask.hpp"
#include "tier4_perception_msgs/msg/detected_objects_with_feature.hpp"

#include <cv_bridge/cv_bridge.h>

#include <memory>
#include <string>
#include <vector>

namespace autoware::tensorrt_rtmdet
{
class TrtRTMDetNode : public rclcpp::Node
{
public:
  explicit TrtRTMDetNode(const rclcpp::NodeOptions & node_options);

  /**
   * @brief Read a color map file and return the color map.
   *
   * The color map file should be a csv file with the following format:
   * ```
   * unique_id, class_name, r, g, b, label_id
   * ```
   * where the label_id represents the class ID of the object, used to process the outputs in
   * Autoware.
   *
   * @param[in] color_map_path The path to the color map file.
   * @return A color map structure with the color information.
   */
  static ColorMap read_color_map_file(const std::string & color_map_path);

  /**
   * @brief Colorize the output mask.
   *
   * Take the output mask which includes the class ID of the objects and convert it to a colorized
   * mask.
   *
   * @param[in] color_map The color map structure.
   * @param[in] mask The output mask.
   * @param[out] color_mask The colorized mask.
   */
  static void get_colorized_mask(
    const ColorMap & color_map, const cv::Mat & mask, cv::Mat & color_mask);

  /**
   * @brief Draw the detected objects on the input image.
   *
   * @param[out] image
   * @param[in] mask
   * @param[in] objects
   * @param[in] color_map
   */
  static void draw_debug_image(
    cv::Mat & image, const cv::Mat & mask, const ObjectArrays & objects,
    const ColorMap & color_map);

  /**
   *  @brief Apply erosion to the mask.
   *
   *  With using this function, you can apply erosion to the mask and It overflowing the border.
   *
   * @param[in,out] mask
   * @param[in] erosion_size
   */
  static void apply_erosion(cv::Mat & mask, const int erosion_size);

private:
  /**
   * @brief Callback function to check are there any subscribers.
   */
  void on_connect();

  /**
   * @brief Callback function to run RTMDet model.
   *
   * @param[in] msg The input image message.
   */
  void on_image(const sensor_msgs::msg::Image::ConstSharedPtr msg);

  std::unique_ptr<tensorrt_rtmdet::TrtRTMDet> trt_rtmdet_;

  image_transport::Subscriber image_sub_;
  rclcpp::TimerBase::SharedPtr timer_;

  rclcpp::Publisher<tier4_perception_msgs::msg::DetectedObjectsWithFeature>::SharedPtr objects_pub_;
  rclcpp::Publisher<autoware_internal_perception_msgs::msg::SegmentationMask>::SharedPtr mask_pub_;

  image_transport::Publisher color_mask_pub_;
  image_transport::Publisher debug_image_pub_;

  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> debug_publisher_;

  const bool is_publish_color_mask_;
  const bool is_publish_debug_image_;

  // Color map to store label and color information.
  ColorMap color_map_;

  // Mean and std for normalization of input image.
  std::vector<float> mean_;
  std::vector<float> std_;

  // If true, apply erosion to the output mask.
  bool is_apply_erosion_;
};
}  // namespace autoware::tensorrt_rtmdet

#endif  // AUTOWARE__TENSORRT_RTMDET__TENSORRT_RTMDET_NODE_HPP_

// Copyright 2025 TIER IV, Inc.
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

#include "traffic_light_selector_node.hpp"

#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include <sensor_msgs/msg/region_of_interest.hpp>

#include <map>
#include <memory>
#include <vector>

namespace autoware::traffic_light
{

TrafficLightSelectorNode::TrafficLightSelectorNode(const rclcpp::NodeOptions & node_options)
: rclcpp::Node("traffic_light_selector_node", node_options),
  tf_buffer_(get_clock()),
  tf_listener_(tf_buffer_),
  detected_rois_sub_(this, "input/detected_rois", rclcpp::QoS{1}.get_rmw_qos_profile()),
  rough_rois_sub_(this, "input/rough_rois", rclcpp::QoS{1}.get_rmw_qos_profile()),
  expected_rois_sub_(this, "input/expect_rois", rclcpp::QoS{1}.get_rmw_qos_profile()),
  sync_(SyncPolicy(10), detected_rois_sub_, rough_rois_sub_, expected_rois_sub_)
{
  {
    stop_watch_ptr_ =
      std::make_unique<autoware::universe_utils::StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ =
      std::make_unique<autoware::universe_utils::DebugPublisher>(this, this->get_name());
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");
  }

  max_iou_threshold_ = declare_parameter<double>("max_iou_threshold");
  using std::placeholders::_1;
  using std::placeholders::_2;
  using std::placeholders::_3;
  sync_.registerCallback(std::bind(&TrafficLightSelectorNode::objectsCallback, this, _1, _2, _3));

  camera_info_sub_ = create_subscription<sensor_msgs::msg::CameraInfo>(
    "input/camera_info", rclcpp::SensorDataQoS(),
    std::bind(&TrafficLightSelectorNode::cameraInfoCallback, this, _1));
  // Publisher
  pub_traffic_light_rois_ =
    create_publisher<TrafficLightRoiArray>("output/traffic_light_rois", rclcpp::QoS{1});
}

void TrafficLightSelectorNode::cameraInfoCallback(
  const sensor_msgs::msg::CameraInfo::ConstSharedPtr input_msg)
{
  if (camera_info_subscribed_) {
    return;
  }
  RCLCPP_INFO(get_logger(), "camera_info received");
  image_width_ = input_msg->width;
  image_height_ = input_msg->height;
  camera_info_subscribed_ = true;
}

void TrafficLightSelectorNode::objectsCallback(
  const DetectedObjectsWithFeature::ConstSharedPtr & detected_traffic_light_msg,
  const TrafficLightRoiArray::ConstSharedPtr & rough_rois_msg,
  const TrafficLightRoiArray::ConstSharedPtr & expected_rois_msg)
{
  stop_watch_ptr_->toc("processing_time", true);
  if (!camera_info_subscribed_) {
    return;
  }
  std::map<int, sensor_msgs::msg::RegionOfInterest> rough_rois_map;
  for (const auto & roi : rough_rois_msg->rois) {
    rough_rois_map[roi.traffic_light_id] = roi.roi;
  }
  // TODO(badai-nguyen): implement this function on CUDA or refactor the code

  TrafficLightRoiArray output;
  output.header = detected_traffic_light_msg->header;
  float max_matching_score = 0.0;
  int det_roi_shift_x = 0;
  int det_roi_shift_y = 0;
  std::vector<sensor_msgs::msg::RegionOfInterest> det_rois;
  std::vector<sensor_msgs::msg::RegionOfInterest> expect_rois;
  for (const auto & detected_roi : detected_traffic_light_msg->feature_objects) {
    det_rois.push_back(detected_roi.feature.roi);
  }
  for (const auto & expected_roi : expected_rois_msg->rois) {
    expect_rois.push_back(expected_roi.roi);
  }
  cv::Mat expect_roi_img =
    utils::createBinaryImageFromRois(expect_rois, cv::Size(image_width_, image_height_));
  cv::Mat det_roi_img =
    utils::createBinaryImageFromRois(det_rois, cv::Size(image_width_, image_height_));
  // for (const auto expect_roi : expect_rois) {
  for (const auto & expect_traffic_roi : expected_rois_msg->rois) {
    const auto & expect_roi = expect_traffic_roi.roi;
    auto traffic_light_id = expect_traffic_roi.traffic_light_id;
    const auto & rough_roi = rough_rois_map[traffic_light_id];

    for (const auto & detected_roi : det_rois) {
      // check if the detected roi is inside the rough roi
      if (!utils::isInsideRoughRoi(detected_roi, rough_roi)) {
        continue;
      }
      int dx = static_cast<int>(detected_roi.x_offset) - static_cast<int>(expect_roi.x_offset);
      int dy = static_cast<int>(detected_roi.y_offset) - static_cast<int>(expect_roi.y_offset);
      cv::Mat det_roi_shifted = utils::shiftAndPaddingImage(det_roi_img, dx, dy);
      double iou = utils::getIoUOf2BinaryImages(expect_roi_img, det_roi_shifted);
      if (iou > max_matching_score) {
        max_matching_score = iou;
        det_roi_shift_x = dx;
        det_roi_shift_y = dy;
      }
    }
  }

  for (const auto & expect_roi : expected_rois_msg->rois) {
    // check max IOU after shift
    double max_iou = -1.0;
    sensor_msgs::msg::RegionOfInterest max_iou_roi;
    for (const auto & detected_roi : detected_traffic_light_msg->feature_objects) {
      // shift detected roi by det_roi_shift_x, det_roi_shift_y and calculate IOU
      sensor_msgs::msg::RegionOfInterest detected_roi_shifted = detected_roi.feature.roi;
      // fit top lef corner of detected roi to inside of image
      detected_roi_shifted.x_offset = std::clamp(
        static_cast<int>(detected_roi.feature.roi.x_offset) - det_roi_shift_x, 0,
        static_cast<int>(image_width_ - detected_roi.feature.roi.width));
      detected_roi_shifted.y_offset = std::clamp(
        static_cast<int>(detected_roi.feature.roi.y_offset) - det_roi_shift_y, 0,
        static_cast<int>(image_height_ - detected_roi.feature.roi.height));

      double iou = utils::getGenIoU(expect_roi.roi, detected_roi_shifted);
      if (iou > max_iou) {
        max_iou = iou;
        max_iou_roi = detected_roi.feature.roi;
      }
    }
    if (max_iou > max_iou_threshold_) {
      TrafficLightRoi traffic_light_roi;
      traffic_light_roi.traffic_light_id = expect_roi.traffic_light_id;
      traffic_light_roi.traffic_light_type = expect_roi.traffic_light_type;
      traffic_light_roi.roi = max_iou_roi;
      output.rois.push_back(traffic_light_roi);
    } else {
      TrafficLightRoi traffic_light_roi;
      traffic_light_roi.traffic_light_id = expect_roi.traffic_light_id;
      traffic_light_roi.traffic_light_type = expect_roi.traffic_light_type;
      output.rois.push_back(traffic_light_roi);
    }
  }
  pub_traffic_light_rois_->publish(output);
  if (debug_publisher_) {
    const double processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    const double cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    const double pipeline_latency_ms =
      std::chrono::duration<double, std::milli>(
        std::chrono::nanoseconds((this->get_clock()->now() - output.header.stamp).nanoseconds()))
        .count();
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/pipeline_latency_ms", pipeline_latency_ms);
  }
  return;
}
}  // namespace autoware::traffic_light

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::traffic_light::TrafficLightSelectorNode)

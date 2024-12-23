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

#include "traffic_light_selector_node.hpp"

#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include "sensor_msgs/msg/region_of_interest.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
bool isInsideRoughRoi(
  const sensor_msgs::msg::RegionOfInterest & detected_roi,
  const sensor_msgs::msg::RegionOfInterest & rough_roi)
{
  // check if tl or br of detected roi is inside the rough roi
  auto tl_x = detected_roi.x_offset;
  auto tl_y = detected_roi.y_offset;
  auto br_x = detected_roi.x_offset + detected_roi.width;
  auto br_y = detected_roi.y_offset + detected_roi.height;
  bool is_tl_inside = rough_roi.x_offset <= tl_x && tl_x <= rough_roi.x_offset + rough_roi.width &&
                      rough_roi.y_offset <= tl_y && tl_y <= rough_roi.y_offset + rough_roi.height;
  if (is_tl_inside) {
    return true;
  }

  bool is_br_inside = rough_roi.x_offset <= br_x && br_x <= rough_roi.x_offset + rough_roi.width &&
                      rough_roi.y_offset <= br_y && br_y <= rough_roi.y_offset + rough_roi.height;
  if (is_br_inside) {
    return true;
  }
  return false;
}

float calIou(
  const sensor_msgs::msg::RegionOfInterest & bbox1,
  const sensor_msgs::msg::RegionOfInterest & bbox2)
{
  int x1 = std::max(bbox1.x_offset, bbox2.x_offset);
  int x2 = std::min(bbox1.x_offset + bbox1.width, bbox2.x_offset + bbox2.width);
  int y1 = std::max(bbox1.y_offset, bbox2.y_offset);
  int y2 = std::min(bbox1.y_offset + bbox1.height, bbox2.y_offset + bbox2.height);
  int area1 = std::max(x2 - x1, 0) * std::max(y2 - y1, 0);
  int area2 = bbox1.width * bbox1.height + bbox2.width * bbox2.height - area1;
  if (area2 == 0) {
    return 0.0;
  }
  return static_cast<float>(area1) / static_cast<float>(area2);
}

// create and function of 2 binary image
int andOf2Images(cv::Mat & img1, cv::Mat & img2)
{
  cv::Mat img_and;
  cv::bitwise_and(img1, img2, img_and);
  return cv::countNonZero(img_and);
}

// create OR function of 2 binary image
int orOf2Images(cv::Mat & img1, cv::Mat & img2)
{
  cv::Mat img_or;
  cv::bitwise_or(img1, img2, img_or);
  return cv::countNonZero(img_or);
}

// create the overal IOU of 2 binary image
double getIoUOf2BinaryImages(cv::Mat & img1, cv::Mat & img2)
{
  int and_area = andOf2Images(img1, img2);
  int or_area = orOf2Images(img1, img2);
  return static_cast<double>(and_area) / static_cast<double>(or_area);
}

// create binary image from list of rois
cv::Mat createBinaryImageFromRois(
  const std::vector<sensor_msgs::msg::RegionOfInterest> & rois, const cv::Size & size)
{
  cv::Mat img = cv::Mat::zeros(size, CV_8UC1);
  for (const auto & roi : rois) {
    // check roi is inside the image
    cv::Rect rect(roi.x_offset, roi.y_offset, roi.width, roi.height);
    cv::rectangle(img, rect, cv::Scalar(255), cv::FILLED);
  }
  return img;
}
// shift and padding image by dx, dy
cv::Mat shiftAndPaddingImage(cv::Mat & img, int dx, int dy)
{
  cv::Mat img_shifted = cv::Mat::zeros(img.size(), img.type());
  uint32_t tl_x = static_cast<uint32_t>(std::max(0, dx));
  uint32_t tl_y = static_cast<uint32_t>(std::max(0, dy));
  uint32_t br_x = std::min(img.cols, (static_cast<int>(img.cols) + dx));
  uint32_t br_y = std::min(img.rows, (static_cast<int>(img.rows) + dy));
  if (br_x <= tl_x || br_y <= tl_y) {
    return img_shifted;
  }
  cv::Rect img_rect(tl_x, tl_y, br_x - tl_x, br_y - tl_y);
  img(img_rect).copyTo(img_shifted(img_rect));
  return img_shifted;
}

}  // namespace

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
  max_iou_threshold_ = declare_parameter<double>("max_iou_threshold", 0.0);
  RCLCPP_INFO(get_logger(), "max_iou_threshold: %f", max_iou_threshold_);
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
    createBinaryImageFromRois(expect_rois, cv::Size(image_width_, image_height_));
  cv::Mat det_roi_img = createBinaryImageFromRois(det_rois, cv::Size(image_width_, image_height_));
  // for (const auto expect_roi : expect_rois) {
  for (const auto & expect_traffic_roi : expected_rois_msg->rois) {
    const auto & expect_roi = expect_traffic_roi.roi;
    auto traffic_light_id = expect_traffic_roi.traffic_light_id;
    const auto & rough_roi = rough_rois_map[traffic_light_id];

    for (const auto & detected_roi : det_rois) {
      // check if the detected roi is inside the rough roi
      if (!isInsideRoughRoi(detected_roi, rough_roi)) {
        continue;
      }
      int dx = static_cast<int>(detected_roi.x_offset) - static_cast<int>(expect_roi.x_offset);
      int dy = static_cast<int>(detected_roi.y_offset) - static_cast<int>(expect_roi.y_offset);
      cv::Mat det_roi_shifted = shiftAndPaddingImage(det_roi_img, dx, dy);
      double iou = getIoUOf2BinaryImages(expect_roi_img, det_roi_shifted);
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

      double iou = calIou(expect_roi.roi, detected_roi_shifted);
      if (iou > max_iou) {
        max_iou = iou;
        max_iou_roi = detected_roi.feature.roi;
      }
    }
    if (max_iou > max_iou_threshold_) {
      TrafficLightRoi traffic_light_roi;
      traffic_light_roi.traffic_light_id = expect_roi.traffic_light_id;
      traffic_light_roi.roi = max_iou_roi;
      output.rois.push_back(traffic_light_roi);
    }
  }
  pub_traffic_light_rois_->publish(output);
  return;
}
}  // namespace autoware::traffic_light

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::traffic_light::TrafficLightSelectorNode)

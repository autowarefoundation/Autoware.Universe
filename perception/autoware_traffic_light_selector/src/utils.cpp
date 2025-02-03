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

#include "utils.hpp"

#include <algorithm>
#include <vector>
namespace autoware::traffic_light
{
namespace utils
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

float calIOU(
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

double getGenIoU(
  const sensor_msgs::msg::RegionOfInterest & roi1, const sensor_msgs::msg::RegionOfInterest & roi2)
{
  int rect1_x_min = static_cast<int>(roi1.x_offset);
  int rect1_x_max = static_cast<int>(roi1.x_offset + roi1.width);
  int rect1_y_min = static_cast<int>(roi1.y_offset);
  int rect1_y_max = static_cast<int>(roi1.y_offset + roi1.height);
  int rect2_x_min = static_cast<int>(roi2.x_offset);
  int rect2_x_max = static_cast<int>(roi2.x_offset + roi2.width);
  int rect2_y_min = static_cast<int>(roi2.y_offset);
  int rect2_y_max = static_cast<int>(roi2.y_offset + roi2.height);
  int rect1_area = roi1.width * roi1.height;
  int rect2_area = roi2.width * roi2.height;
  int x_min = std::max(rect1_x_min, rect2_x_min);
  int y_min = std::max(rect1_y_min, rect2_y_min);
  int x_max = std::min(rect1_x_max, rect2_x_max);
  int y_max = std::min(rect1_y_max, rect2_y_max);

  auto w = std::max(0, x_max - x_min);
  auto h = std::max(0, y_max - y_min);
  auto intersect = w * h;

  auto union_area = rect1_area + rect2_area - intersect;

  double iou = static_cast<double>(intersect) / static_cast<double>(union_area);

  // convex shape area

  auto con_x_min = std::min(rect1_x_min, rect2_x_min);
  auto con_y_min = std::min(rect1_y_min, rect2_y_min);
  auto con_x_max = std::max(rect1_x_max, rect2_x_max);
  auto con_y_max = std::max(rect1_y_max, rect2_y_max);

  auto con_area = (con_x_max - con_x_min + 1) * (con_y_max - con_y_min + 1);

  // GIoU calc
  double giou = iou - static_cast<double>(con_area - union_area) / static_cast<double>(con_area);

  return giou;
}

}  // namespace utils
}  // namespace autoware::traffic_light

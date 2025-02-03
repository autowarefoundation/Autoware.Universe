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

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include <sensor_msgs/msg/region_of_interest.hpp>

#include <vector>

namespace autoware::traffic_light
{
namespace utils
{
// create and function of 2 binary image
inline int andOf2Images(cv::Mat & img1, cv::Mat & img2)
{
  cv::Mat img_and;
  cv::bitwise_and(img1, img2, img_and);
  return cv::countNonZero(img_and);
}

// create OR function of 2 binary image
inline int orOf2Images(cv::Mat & img1, cv::Mat & img2)
{
  cv::Mat img_or;
  cv::bitwise_or(img1, img2, img_or);
  return cv::countNonZero(img_or);
}

// create the overall IOU of 2 binary image
inline double getIoUOf2BinaryImages(cv::Mat & img1, cv::Mat & img2)
{
  int and_area = andOf2Images(img1, img2);
  int or_area = orOf2Images(img1, img2);
  return static_cast<double>(and_area) / static_cast<double>(or_area);
}

bool isInsideRoughRoi(
  const sensor_msgs::msg::RegionOfInterest & detected_roi,
  const sensor_msgs::msg::RegionOfInterest & rough_roi);

float calIOU(
  const sensor_msgs::msg::RegionOfInterest & bbox1,
  const sensor_msgs::msg::RegionOfInterest & bbox2);

double getGenIoU(
  const sensor_msgs::msg::RegionOfInterest & roi1, const sensor_msgs::msg::RegionOfInterest & roi2);
// create binary image from list of rois
cv::Mat createBinaryImageFromRois(
  const std::vector<sensor_msgs::msg::RegionOfInterest> & rois, const cv::Size & size);

// shift and padding image by dx, dy
cv::Mat shiftAndPaddingImage(cv::Mat & img, int dx, int dy);

}  // namespace utils
}  // namespace autoware::traffic_light

#endif  // UTILS_HPP_

// Copyright 2024 AutoCore, Inc.
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

// cspell:ignore BEVDET, bevdet

#ifndef AUTOWARE__TENSORRT_BEVDET__ROS_UTILS_HPP_
#define AUTOWARE__TENSORRT_BEVDET__ROS_UTILS_HPP_

#include <opencv2/opencv.hpp>

#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <geometry_msgs/msg/point32.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <bevdet.h>

#include <string>
#include <vector>
namespace autoware::tensorrt_bevdet
{
uint8_t getSemanticType(const std::string & class_name);

void box3DToDetectedObjects(
  const std::vector<Box> & boxes, autoware_perception_msgs::msg::DetectedObjects & objects,
  const std::vector<std::string> & class_names, float score_thre, const bool has_twist);

// Get the rotation and translation from a geometry_msgs::msg::TransformStamped
void getTransform(
  const geometry_msgs::msg::TransformStamped & transform, Eigen::Quaternion<float> & rot,
  Eigen::Translation3f & translation);

// Get the camera intrinsics from a sensor_msgs::msg::CameraInfo
void getCameraIntrinsics(
  const sensor_msgs::msg::CameraInfo::SharedPtr msg, Eigen::Matrix3f & intrinsics);

// Convert images from OpenCV's cv:: Mat format to a specific format
void imageTransport(std::vector<cv::Mat> imgs, uchar * out_imgs, size_t width, size_t height);

}  // namespace autoware::tensorrt_bevdet
#endif  // AUTOWARE__TENSORRT_BEVDET__ROS_UTILS_HPP_

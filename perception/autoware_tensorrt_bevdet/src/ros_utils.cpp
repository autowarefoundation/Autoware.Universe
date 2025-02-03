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

// cspell:ignore bevdet, RGBHWC, BGRCHW

#include "autoware/tensorrt_bevdet/ros_utils.hpp"

#include <autoware_perception_msgs/msg/detected_object_kinematics.hpp>
#include <autoware_perception_msgs/msg/object_classification.hpp>
#include <autoware_perception_msgs/msg/shape.hpp>

#include <preprocess.h>

namespace autoware::tensorrt_bevdet
{
using Label = autoware_perception_msgs::msg::ObjectClassification;

uint8_t getSemanticType(const std::string & class_name)
{
  if (class_name == "car") {
    return Label::CAR;
  } else if (class_name == "truck") {
    return Label::TRUCK;
  } else if (class_name == "bus") {
    return Label::BUS;
  } else if (class_name == "trailer") {
    return Label::TRAILER;
  } else if (class_name == "bicycle") {
    return Label::BICYCLE;
  } else if (class_name == "motorcycle") {
    return Label::MOTORCYCLE;
  } else if (class_name == "pedestrian") {
    return Label::PEDESTRIAN;
  } else {
    return Label::UNKNOWN;
  }
}

void box3DToDetectedObjects(
  const std::vector<Box> & boxes, autoware_perception_msgs::msg::DetectedObjects & bevdet_objects,
  const std::vector<std::string> & class_names, float score_thre, const bool has_twist = true)
{
  for (auto b : boxes) {
    if (b.score < score_thre) continue;
    autoware_perception_msgs::msg::DetectedObject obj;

    Eigen::Vector3f center(b.x, b.y, b.z + b.h / 2.);

    obj.existence_probability = b.score;
    // classification
    autoware_perception_msgs::msg::ObjectClassification classification;
    classification.probability = 1.0f;
    if (b.label >= 0 && static_cast<size_t>(b.label) < class_names.size()) {
      classification.label = getSemanticType(class_names[b.label]);
    } else {
      classification.label = Label::UNKNOWN;
    }
    obj.classification.emplace_back(classification);

    // pose and shape
    geometry_msgs::msg::Point p;
    p.x = center.x();
    p.y = center.y();
    p.z = center.z();
    obj.kinematics.pose_with_covariance.pose.position = p;

    tf2::Quaternion q;
    q.setRPY(0, 0, b.r);
    obj.kinematics.pose_with_covariance.pose.orientation = tf2::toMsg(q);

    obj.shape.type = autoware_perception_msgs::msg::Shape::BOUNDING_BOX;

    geometry_msgs::msg::Vector3 v;
    v.x = b.l;
    v.y = b.w;
    v.z = b.h;
    obj.shape.dimensions = v;
    if (has_twist) {
      float vel_x = b.vx;
      float vel_y = b.vy;
      geometry_msgs::msg::Twist twist;
      twist.linear.x = std::sqrt(std::pow(vel_x, 2) + std::pow(vel_y, 2));
      twist.angular.z = 2 * (std::atan2(vel_y, vel_x) - b.r);
      obj.kinematics.twist_with_covariance.twist = twist;
      obj.kinematics.has_twist = has_twist;
    }

    bevdet_objects.objects.emplace_back(obj);
  }
}

void getTransform(
  const geometry_msgs::msg::TransformStamped & transform, Eigen::Quaternion<float> & rot,
  Eigen::Translation3f & translation)
{
  rot = Eigen::Quaternion<float>(
    transform.transform.rotation.w, transform.transform.rotation.x, transform.transform.rotation.y,
    transform.transform.rotation.z);
  translation = Eigen::Translation3f(
    transform.transform.translation.x, transform.transform.translation.y,
    transform.transform.translation.z);
}

void getCameraIntrinsics(
  const sensor_msgs::msg::CameraInfo::SharedPtr msg, Eigen::Matrix3f & intrinsics)
{
  intrinsics << msg->k[0], msg->k[1], msg->k[2], msg->k[3], msg->k[4], msg->k[5], msg->k[6],
    msg->k[7], msg->k[8];
}

void imageTransport(std::vector<cv::Mat> imgs, uchar * out_imgs, size_t width, size_t height)
{
  uchar * temp = new uchar[width * height * 3];
  uchar * temp_gpu = nullptr;
  CHECK_CUDA(cudaMalloc(&temp_gpu, width * height * 3));

  for (size_t i = 0; i < imgs.size(); i++) {
    cv::cvtColor(imgs[i], imgs[i], cv::COLOR_BGR2RGB);
    CHECK_CUDA(cudaMemcpy(temp_gpu, imgs[i].data, width * height * 3, cudaMemcpyHostToDevice));
    convert_RGBHWC_to_BGRCHW(temp_gpu, out_imgs + i * width * height * 3, 3, height, width);
  }
  delete[] temp;
  CHECK_CUDA(cudaFree(temp_gpu));
}

}  // namespace autoware::tensorrt_bevdet

// Copyright 2022 TIER IV, Inc.
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

#include "autoware/tensorrt_yolov10/tensorrt_yolov10_node.hpp"

#include "autoware/object_recognition_utils/object_classification.hpp"
#include "perception_utils/run_length_encoder.hpp"

#include <autoware_perception_msgs/msg/object_classification.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define PRINT_DEBUG_INFO printf("line:%d\n", __LINE__);

namespace autoware::tensorrt_yolov10
{
TrtYolov10Node::TrtYolov10Node(const rclcpp::NodeOptions & node_options)
: Node("tensorrt_yolov10", node_options)
{
  {
    stop_watch_ptr_ =
      std::make_unique<autoware::universe_utils::StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ =
      std::make_unique<autoware::universe_utils::DebugPublisher>(this, this->get_name());
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");
  }
  using std::placeholders::_1;
  using std::chrono_literals::operator""ms;

  const std::string model_path = this->declare_parameter<std::string>("model_path");
  const std::string precision = this->declare_parameter<std::string>("precision", "fp16");
  const uint8_t gpu_id = this->declare_parameter<uint8_t>("gpu_id", 0);
  const std::string label_path = this->declare_parameter<std::string>("label_path");

  trt_yolov10_ = std::make_unique<tensorrt_yolov10::TrtYolov10>(model_path, precision);

  if (!readLabelFile(label_path)) {
    RCLCPP_ERROR(this->get_logger(), "Could not find label file");
    rclcpp::shutdown();
  }
  replaceLabelMap();

  if (!trt_yolov10_->isGPUInitialized()) {
    RCLCPP_ERROR(this->get_logger(), "GPU %d does not exist or is not suitable.", gpu_id);
    rclcpp::shutdown();
    return;
  }
  RCLCPP_INFO(this->get_logger(), "GPU %d is selected for the inference!", gpu_id);

  timer_ =
    rclcpp::create_timer(this, get_clock(), 100ms, std::bind(&TrtYolov10Node::onConnect, this));

  objects_pub_ = this->create_publisher<tier4_perception_msgs::msg::DetectedObjectsWithFeature>(
    "~/out/objects", 1);
  image_pub_ = image_transport::create_publisher(this, "~/out/image");
}

void TrtYolov10Node::onConnect()
{
  using std::placeholders::_1;
  if (
    objects_pub_->get_subscription_count() == 0 &&
    objects_pub_->get_intra_process_subscription_count() == 0 &&
    image_pub_.getNumSubscribers() == 0) {
    // printf("no subscribers who sub from objects_pub_ or image_pub_,shut down image_sub_\n");
    image_sub_.shutdown();
  } else if (!image_sub_) {
    image_sub_ = image_transport::create_subscription(
      this, "~/in/image", std::bind(&TrtYolov10Node::onImage, this, _1), "raw",
      rmw_qos_profile_sensor_data);
  }
}

void TrtYolov10Node::onImage(const sensor_msgs::msg::Image::ConstSharedPtr msg)
{
  stop_watch_ptr_->toc("processing_time", true);
  tier4_perception_msgs::msg::DetectedObjectsWithFeature out_objects;

  cv_bridge::CvImagePtr in_image_ptr;
  try {
    in_image_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
  } catch (cv_bridge::Exception & e) {
    RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
    return;
  }
  const auto width = in_image_ptr->image.cols;
  const auto height = in_image_ptr->image.rows;

  tensorrt_yolov10::ObjectArrays objects;

  if (!trt_yolov10_->doInference({in_image_ptr->image}, objects)) {
    RCLCPP_WARN(this->get_logger(), "Fail to inference");
    return;
  }

  for (const auto & yolov10_object : objects.at(0)) {
    tier4_perception_msgs::msg::DetectedObjectWithFeature object;
    object.feature.roi.x_offset = yolov10_object.x_offset;
    object.feature.roi.y_offset = yolov10_object.y_offset;
    object.feature.roi.width = yolov10_object.width;
    object.feature.roi.height = yolov10_object.height;
    object.object.existence_probability = yolov10_object.score;
    object.object.classification =
      object_recognition_utils::toObjectClassifications(label_map_[yolov10_object.type], 1.0f);
    out_objects.feature_objects.push_back(object);
    const auto left = std::max(0, static_cast<int>(object.feature.roi.x_offset));
    const auto top = std::max(0, static_cast<int>(object.feature.roi.y_offset));
    const auto right =
      std::min(static_cast<int>(object.feature.roi.x_offset + object.feature.roi.width), width);
    const auto bottom =
      std::min(static_cast<int>(object.feature.roi.y_offset + object.feature.roi.height), height);
    cv::rectangle(
      in_image_ptr->image, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255), 3,
      8, 0);

    cv::putText(
      in_image_ptr->image, label_map_[yolov10_object.type], cv::Point(left, top),
      cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 255), 1, 8, 0);
  }

  image_pub_.publish(in_image_ptr->toImageMsg());
  out_objects.header = msg->header;
  objects_pub_->publish(out_objects);

  if (debug_publisher_) {
    const double processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    const double cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    const double pipeline_latency_ms =
      std::chrono::duration<double, std::milli>(
        std::chrono::nanoseconds(
          (this->get_clock()->now() - out_objects.header.stamp).nanoseconds()))
        .count();
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/pipeline_latency_ms", pipeline_latency_ms);
  }
}

bool TrtYolov10Node::readLabelFile(const std::string & label_path)
{
  std::ifstream label_file(label_path);
  if (!label_file.is_open()) {
    RCLCPP_ERROR(this->get_logger(), "Could not open label file. [%s]", label_path.c_str());
    return false;
  }
  int label_index{};
  std::string label;
  while (getline(label_file, label)) {
    std::transform(
      label.begin(), label.end(), label.begin(), [](auto c) { return std::toupper(c); });
    label_map_.insert({label_index, label});
    ++label_index;
  }
  return true;
}

// we need this because autoware::object_recognition_utils::toLabel(const std::string & class_name)
// limit label type
void TrtYolov10Node::replaceLabelMap()
{
  for (std::size_t i = 0; i < label_map_.size(); ++i) {
    auto & label = label_map_[i];
    if (label == "PERSON") {
      label = "PEDESTRIAN";
    } else if (label == "MOTORBIKE") {
      label = "MOTORCYCLE";
    } else if (
      label != "CAR" && label != "PEDESTRIAN" && label != "BUS" && label != "TRUCK" &&
      label != "BICYCLE" && label != "MOTORCYCLE") {
      label = "UNKNOWN";
    }
  }
}

}  // namespace autoware::tensorrt_yolov10

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::tensorrt_yolov10::TrtYolov10Node)

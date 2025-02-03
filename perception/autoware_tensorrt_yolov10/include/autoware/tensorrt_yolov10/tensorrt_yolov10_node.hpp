// Copyright 2022 Tier IV, Inc.
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

#ifndef AUTOWARE__TENSORRT_YOLOV10__TENSORRT_YOLOV10_NODE_HPP_
#define AUTOWARE__TENSORRT_YOLOV10__TENSORRT_YOLOV10_NODE_HPP_

#include "autoware/object_recognition_utils/object_recognition_utils.hpp"

#include <autoware/tensorrt_yolov10/tensorrt_yolov10.hpp>
#include <autoware/universe_utils/ros/debug_publisher.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <image_transport/image_transport.hpp>
#include <opencv2/opencv.hpp>
#include <rclcpp/rclcpp.hpp>

#include <sensor_msgs/msg/image.hpp>
#include <std_msgs/msg/header.hpp>
#include <tier4_perception_msgs/msg/detected_objects_with_feature.hpp>
#include <tier4_perception_msgs/msg/semantic.hpp>

#if __has_include(<cv_bridge/cv_bridge.hpp>)
#include <cv_bridge/cv_bridge.hpp>
#else
#include <cv_bridge/cv_bridge.h>
#endif

#include <chrono>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::tensorrt_yolov10
{
// cspell: ignore Semseg
class TrtYolov10Node : public rclcpp::Node
{
public:
  explicit TrtYolov10Node(const rclcpp::NodeOptions & node_options);

private:
  void onConnect();
  void onImage(const sensor_msgs::msg::Image::ConstSharedPtr msg);

  bool readLabelFile(const std::string & label_path);
  void replaceLabelMap();

  std::unique_ptr<tensorrt_yolov10::TrtYolov10> trt_yolov10_;

  image_transport::Subscriber image_sub_;

  rclcpp::TimerBase::SharedPtr timer_;
  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> debug_publisher_;
  rclcpp::Publisher<tier4_perception_msgs::msg::DetectedObjectsWithFeature>::SharedPtr objects_pub_;
  image_transport::Publisher image_pub_;

  std::map<int, std::string> label_map_;
};

}  // namespace autoware::tensorrt_yolov10

#endif  // AUTOWARE__TENSORRT_YOLOV10__TENSORRT_YOLOV10_NODE_HPP_

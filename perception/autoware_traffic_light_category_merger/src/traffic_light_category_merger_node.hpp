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

#ifndef TRAFFIC_LIGHT_CATEGORY_MERGER_NODE_HPP_
#define TRAFFIC_LIGHT_CATEGORY_MERGER_NODE_HPP_

#include <autoware/universe_utils/ros/transform_listener.hpp>
#include <rclcpp/rclcpp.hpp>

#include <tier4_perception_msgs/msg/traffic_light_array.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/synchronizer.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace autoware::traffic_light
{
using tier4_perception_msgs::msg::TrafficLightArray;

class TrafficLightCategoryMergerNode : public rclcpp::Node
{
public:
  explicit TrafficLightCategoryMergerNode(const rclcpp::NodeOptions & node_options);

private:
  // Subscriber
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  message_filters::Subscriber<TrafficLightArray> car_signal_sub_;
  message_filters::Subscriber<TrafficLightArray> pedestrian_signal_sub_;

  typedef message_filters::sync_policies::ApproximateTime<TrafficLightArray, TrafficLightArray>
    SyncPolicy;
  message_filters::Synchronizer<SyncPolicy> sync_;

  void signalsCallback(
    const TrafficLightArray::ConstSharedPtr & car_signals_msg,
    const TrafficLightArray::ConstSharedPtr & pedestrian_signals_msg);

  rclcpp::Publisher<TrafficLightArray>::SharedPtr pub_traffic_light_signals_;
};

}  // namespace autoware::traffic_light

#endif  // TRAFFIC_LIGHT_CATEGORY_MERGER_NODE_HPP_

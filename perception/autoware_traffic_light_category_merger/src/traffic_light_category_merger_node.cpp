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

#include "traffic_light_category_merger_node.hpp"

#include <memory>
#include <string>
#include <vector>

namespace autoware::traffic_light
{

TrafficLightCategoryMergerNode::TrafficLightCategoryMergerNode(
  const rclcpp::NodeOptions & node_options)
: rclcpp::Node("traffic_light_category_merger_node", node_options),
  tf_buffer_(get_clock()),
  tf_listener_(tf_buffer_),
  car_signal_sub_(this, "input/car_signals", rclcpp::QoS{1}.get_rmw_qos_profile()),
  pedestrian_signal_sub_(this, "input/pedestrian_signals", rclcpp::QoS{1}.get_rmw_qos_profile()),
  sync_(SyncPolicy(10), car_signal_sub_, pedestrian_signal_sub_)
{
  using std::placeholders::_1;
  using std::placeholders::_2;
  sync_.registerCallback(std::bind(&TrafficLightCategoryMergerNode::signalsCallback, this, _1, _2));
  pub_traffic_light_signals_ =
    create_publisher<TrafficLightArray>("output/traffic_light_signals", rclcpp::QoS{1});
}

void TrafficLightCategoryMergerNode::signalsCallback(
  const TrafficLightArray::ConstSharedPtr & car_signals_msg,
  const TrafficLightArray::ConstSharedPtr & pedestrian_signals_msg)
{
  TrafficLightArray output;
  output.header = car_signals_msg->header;
  output.signals.insert(
    output.signals.end(), car_signals_msg->signals.begin(), car_signals_msg->signals.end());
  output.signals.insert(
    output.signals.end(), pedestrian_signals_msg->signals.begin(),
    pedestrian_signals_msg->signals.end());
  pub_traffic_light_signals_->publish(output);
}

}  // namespace autoware::traffic_light

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::traffic_light::TrafficLightCategoryMergerNode)

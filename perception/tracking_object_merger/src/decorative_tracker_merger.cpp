// Copyright 2020 Tier IV, Inc.
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

#include "tracking_object_merger/decorative_tracker_merger.hpp"

#include "perception_utils/perception_utils.hpp"
#include "tier4_autoware_utils/tier4_autoware_utils.hpp"
#include "tracking_object_merger/data_association/solver/successive_shortest_path.hpp"
#include "tracking_object_merger/utils/utils.hpp"

#include <boost/optional.hpp>

#include <chrono>
#include <unordered_map>

#define EIGEN_MPL2_ONLY
#include <Eigen/Core>
#include <Eigen/Geometry>

using Label = autoware_auto_perception_msgs::msg::ObjectClassification;

namespace tracking_object_merger
{

using autoware_auto_perception_msgs::msg::TrackedObjects;

// get unix time from header
double getUnixTime(const std_msgs::msg::Header & header)
{
  return header.stamp.sec + header.stamp.nanosec * 1e-9;
}

DecorativeTrackerMergerNode::DecorativeTrackerMergerNode(const rclcpp::NodeOptions & node_options)
: rclcpp::Node("decorative_object_merger_node", node_options),
  tf_buffer_(get_clock()),
  tf_listener_(tf_buffer_)
{
  // Subscriber
  sub_main_objects_ = create_subscription<TrackedObjects>(
    "input/main_object", rclcpp::QoS{1},
    std::bind(&DecorativeTrackerMergerNode::mainObjectsCallback, this, std::placeholders::_1));
  sub_sub_objects_ = create_subscription<TrackedObjects>(
    "input/sub_object", rclcpp::QoS{1},
    std::bind(&DecorativeTrackerMergerNode::subObjectsCallback, this, std::placeholders::_1));

  // merged object publisher
  merged_object_pub_ = create_publisher<TrackedObjects>("output/object", rclcpp::QoS{1});

  // Parameters
  base_link_frame_id_ = declare_parameter<std::string>("base_link_frame_id", "base_link");
  // priority_mode_ = static_cast<PriorityMode>(
  //   declare_parameter<int>("priority_mode", static_cast<int>(PriorityMode::Confidence)));
  // remove_overlapped_unknown_objects_ =
  //   declare_parameter<bool>("remove_overlapped_unknown_objects", true);
  overlapped_judge_param_.precision_threshold =
    declare_parameter<double>("precision_threshold_to_judge_overlapped");
  overlapped_judge_param_.recall_threshold =
    declare_parameter<double>("recall_threshold_to_judge_overlapped", 0.5);
  overlapped_judge_param_.generalized_iou_threshold =
    declare_parameter<double>("generalized_iou_threshold");
  sub_object_timeout_sec_ = declare_parameter<double>("sub_object_timeout_sec", 0.5);
}

void DecorativeTrackerMergerNode::mainObjectsCallback(
  const TrackedObjects::ConstSharedPtr & main_objects)
{
  // if there are no sub objects, publish main objects as it is
  if (sub_objects_buffer_.empty()) {
    merged_object_pub_->publish(*main_objects);
    return;
  }

  // else, merge main objects and sub objects

  // get newest sub objects which timestamp is earlier to main objects
  TrackedObjects::ConstSharedPtr closest_sub_objects;
  TrackedObjects::ConstSharedPtr closest_sub_objects_later;
  for (const auto & sub_object : sub_objects_buffer_) {
    if (getUnixTime(sub_object->header) < getUnixTime(main_objects->header)) {
      closest_sub_objects = sub_object;
    } else {
      closest_sub_objects_later = sub_object;
      break;
    }
  }

  // There three possible patterns
  // 1. closest_sub_objects is nullptr
  // 2. closest_sub_objects_later is nullptr
  // 3. both closest_sub_objects and closest_sub_objects_later are not nullptr

  // finally publish merged objects
}

void DecorativeTrackerMergerNode::subObjectsCallback(const TrackedObjects::ConstSharedPtr & msg)
{
  sub_objects_buffer_.push_back(msg);
  // remove old sub objects
  const auto now = get_clock()->now();
  const auto remove_itr = std::remove_if(
    sub_objects_buffer_.begin(), sub_objects_buffer_.end(), [now, this](const auto & sub_object) {
      return (now - sub_object->header.stamp).seconds() > sub_object_timeout_sec_;
    });
  sub_objects_buffer_.erase(remove_itr, sub_objects_buffer_.end());
}

// TrackedObjects DecorativeTrackerMergerNode::

}  // namespace tracking_object_merger

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(tracking_object_merger::DecorativeTrackerMergerNode)

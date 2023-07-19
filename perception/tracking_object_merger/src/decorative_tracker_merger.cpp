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

using autoware_auto_perception_msgs::msg::TrackedObject;
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
  time_sync_threshold_ = declare_parameter<double>("time_sync_threshold", 0.05);

  // init association
  const auto tmp = this->declare_parameter<std::vector<int64_t>>("can_assign_matrix");
  const std::vector<int> can_assign_matrix(tmp.begin(), tmp.end());
  const auto max_dist_matrix = this->declare_parameter<std::vector<double>>("max_dist_matrix");
  const auto max_rad_matrix = this->declare_parameter<std::vector<double>>("max_rad_matrix");
  const auto min_iou_matrix = this->declare_parameter<std::vector<double>>("min_iou_matrix");
  data_association_ = std::make_unique<DataAssociation>(
    can_assign_matrix, max_dist_matrix, max_rad_matrix, min_iou_matrix);
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

TrackedObjects DecorativeTrackerMergerNode::decorativeMerger(
  const TrackedObjects::ConstSharedPtr & main_objects)
{
  // get interpolated sub objects
  // get newest sub objects which timestamp is earlier to main objects
  TrackedObjects::ConstSharedPtr closest_time_sub_objects;
  TrackedObjects::ConstSharedPtr closest_time_sub_objects_later;
  for (const auto & sub_object : sub_objects_buffer_) {
    if (getUnixTime(sub_object->header) < getUnixTime(main_objects->header)) {
      closest_time_sub_objects = sub_object;
    } else {
      closest_time_sub_objects_later = sub_object;
      break;
    }
  }
  const auto interpolated_sub_objects = interpolateObjectState(
    closest_time_sub_objects, closest_time_sub_objects_later, main_objects->header);

  // define output object
  TrackedObjects output_objects;

  // if there are no sub objects, return main objects as it is
  if (!interpolated_sub_objects) {
    return *main_objects;
  }

  // else, merge main objects and sub objects
  // first, do association
  /* global nearest neighbor */
  std::unordered_map<int, int> direct_assignment, reverse_assignment;
  const auto & objects0 = main_objects->objects;
  const auto & objects1 = interpolated_sub_objects->objects;
  Eigen::MatrixXd score_matrix =
    data_association_->calcScoreMatrix(*main_objects, *interpolated_sub_objects);
  data_association_->assign(score_matrix, direct_assignment, reverse_assignment);

  for (size_t object0_idx = 0; object0_idx < objects0.size(); ++object0_idx) {
    const auto & object0 = objects0.at(object0_idx);
    if (direct_assignment.find(object0_idx) != direct_assignment.end()) {  // found and merge
      const auto & object1 = objects1.at(direct_assignment.at(object0_idx));
      // merge object0 and object1
      TrackedObject merged_object = object0;
      merged_object.kinematics = merger_utils::objectKinematicsVXMerger(
        object0, object1, merger_utils::MergePolicy::OVERWRITE);
      merged_object.shape =
        merger_utils::shapeMerger(object0, object1, merger_utils::MergePolicy::SKIP);
      merged_object.existence_probability = merger_utils::probabilityMerger(
        object0.existence_probability, object1.existence_probability,
        merger_utils::MergePolicy::SKIP);
      auto merged_classification =
        merger_utils::objectClassificationMerger(object0, object1, merger_utils::MergePolicy::SKIP);
      merged_object.classification = merged_classification.classification;
      output_objects.objects.push_back(merged_object);
    } else {  // not found
      output_objects.objects.push_back(object0);
    }
  }
  for (size_t object1_idx = 0; object1_idx < objects1.size(); ++object1_idx) {
    const auto & object1 = objects1.at(object1_idx);
    if (reverse_assignment.find(object1_idx) != reverse_assignment.end()) {  // found
    } else {                                                                 // not found
      output_objects.objects.push_back(object1);
    }
  }

  return *main_objects;
}

std::optional<TrackedObjects> DecorativeTrackerMergerNode::interpolateObjectState(
  const TrackedObjects::ConstSharedPtr & former_msg,
  const TrackedObjects::ConstSharedPtr & latter_msg, const std_msgs::msg::Header & output_header)
{
  // Assumption: output_header must be newer than former_msg and older than latter_msg
  // There three possible patterns
  // 1. both msg is nullptr
  // 2. former_msg is nullptr
  // 3. latter_msg is nullptr
  // 4. both msg is not nullptr

  // 1. both msg is nullptr
  if (former_msg == nullptr && latter_msg == nullptr) {
    // return null optional
    return std::nullopt;
  }

  // 2. former_msg is nullptr
  if (former_msg == nullptr) {
    // depends on header stamp difference
    if (
      (rclcpp::Time(latter_msg->header.stamp) - rclcpp::Time(output_header.stamp)).seconds() >
      time_sync_threshold_) {
      // do nothing
      return std::nullopt;
    } else {  // else, return latter_msg
      return *latter_msg;
    }

    // 3. latter_msg is nullptr
  } else if (latter_msg == nullptr) {
    // depends on header stamp difference
    if (
      (rclcpp::Time(output_header.stamp) - rclcpp::Time(former_msg->header.stamp)).seconds() >
      time_sync_threshold_) {
      // do nothing
      return std::nullopt;
    } else {
      // else, return former_msg
      return *former_msg;
      // (TODO) do prediction in here
    }

    // 4. both msg is not nullptr
  } else {
    // do the interpolation
    TrackedObjects interpolated_msg =
      utils::interpolateTrackedObjects(*former_msg, *latter_msg, output_header);
    return interpolated_msg;
  }
}

}  // namespace tracking_object_merger

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(tracking_object_merger::DecorativeTrackerMergerNode)

// Copyright 2024 TIER IV, Inc.
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

#ifndef PROCESSOR__PROCESSOR_HPP_
#define PROCESSOR__PROCESSOR_HPP_

#include "autoware/multi_object_tracker/object_model/types.hpp"
#include "autoware/multi_object_tracker/tracker/model/tracker_base.hpp"

#include <rclcpp/rclcpp.hpp>

#include "autoware_perception_msgs/msg/detected_objects.hpp"
#include "autoware_perception_msgs/msg/tracked_objects.hpp"

#include <list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace autoware::multi_object_tracker
{
using LabelType = autoware_perception_msgs::msg::ObjectClassification::_label_type;

struct TrackerProcessorConfig
{
  std::map<LabelType, std::string> tracker_map;
  size_t channel_size;
  float tracker_lifetime;                              // [s]
  float min_known_object_removal_iou;                  // ratio [0, 1]
  float min_unknown_object_removal_iou;                // ratio [0, 1]
  double distance_threshold;                           // [m]
  std::map<LabelType, int> confident_count_threshold;  // [count]
};

class TrackerProcessor
{
public:
  explicit TrackerProcessor(const TrackerProcessorConfig & config);

  const std::list<std::shared_ptr<Tracker>> & getListTracker() const { return list_tracker_; }
  // tracker processes
  void predict(const rclcpp::Time & time);
  void update(
    const types::DynamicObjectList & detected_objects,
    const geometry_msgs::msg::Transform & self_transform,
    const std::unordered_map<int, int> & direct_assignment);
  void spawn(
    const types::DynamicObjectList & detected_objects,
    const std::unordered_map<int, int> & reverse_assignment);
  void prune(const rclcpp::Time & time);

  // output
  bool isConfidentTracker(const std::shared_ptr<Tracker> & tracker) const;
  void getTrackedObjects(
    const rclcpp::Time & time,
    autoware_perception_msgs::msg::TrackedObjects & tracked_objects) const;
  void getTentativeObjects(
    const rclcpp::Time & time,
    autoware_perception_msgs::msg::TrackedObjects & tentative_objects) const;

  void getExistenceProbabilities(std::vector<std::vector<float>> & existence_vectors) const;

private:
  TrackerProcessorConfig config_;
  std::list<std::shared_ptr<Tracker>> list_tracker_;
  void removeOldTracker(const rclcpp::Time & time);
  void removeOverlappedTracker(const rclcpp::Time & time);
  std::shared_ptr<Tracker> createNewTracker(
    const types::DynamicObject & object, const rclcpp::Time & time) const;
};

}  // namespace autoware::multi_object_tracker

#endif  // PROCESSOR__PROCESSOR_HPP_

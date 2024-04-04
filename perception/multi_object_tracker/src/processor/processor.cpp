// Copyright 2024 Tier IV, Inc.
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
//
//

#include "multi_object_tracker/processor/processor.hpp"

#include "multi_object_tracker/tracker/tracker.hpp"
#include "object_recognition_utils/object_recognition_utils.hpp"

#include <autoware_auto_perception_msgs/msg/tracked_objects.hpp>

#include <iterator>

using Label = autoware_auto_perception_msgs::msg::ObjectClassification;

TrackerProcessor::TrackerProcessor(const std::map<std::uint8_t, std::string> & tracker_map)
: tracker_map_(tracker_map)
{
  // tracker lifetime parameters
  max_elapsed_time_ = 1.0;  // [s]

  // tracker overlap remover parameters
  min_iou_ = 0.1;                       // [ratio]
  min_iou_for_unknown_object_ = 0.001;  // [ratio]
  distance_threshold_ = 5.0;            // [m]

  // tracker confidence threshold
  confident_count_threshold_ = 3;  // [count]
}

void TrackerProcessor::predict(const rclcpp::Time & time)
{
  /* tracker prediction */
  for (auto itr = list_tracker_.begin(); itr != list_tracker_.end(); ++itr) {
    (*itr)->predict(time);
  }
}

void TrackerProcessor::update(
  const autoware_auto_perception_msgs::msg::DetectedObjects & detected_objects,
  const geometry_msgs::msg::Transform & self_transform,
  const std::unordered_map<int, int> & direct_assignment)
{
  int tracker_idx = 0;
  const auto & time = detected_objects.header.stamp;
  for (auto tracker_itr = list_tracker_.begin(); tracker_itr != list_tracker_.end();
       ++tracker_itr, ++tracker_idx) {
    if (direct_assignment.find(tracker_idx) != direct_assignment.end()) {  // found
      const auto & associated_object =
        detected_objects.objects.at(direct_assignment.find(tracker_idx)->second);
      (*(tracker_itr))->updateWithMeasurement(associated_object, time, self_transform);
    } else {  // not found
      (*(tracker_itr))->updateWithoutMeasurement();
    }
  }
}

void TrackerProcessor::spawn(
  const autoware_auto_perception_msgs::msg::DetectedObjects & detected_objects,
  const geometry_msgs::msg::Transform & self_transform,
  const std::unordered_map<int, int> & reverse_assignment)
{
  const auto & time = detected_objects.header.stamp;
  for (size_t i = 0; i < detected_objects.objects.size(); ++i) {
    if (reverse_assignment.find(i) != reverse_assignment.end()) {  // found
      continue;
    }
    const auto & new_object = detected_objects.objects.at(i);
    std::shared_ptr<Tracker> tracker = createNewTracker(new_object, time, self_transform);
    if (tracker) list_tracker_.push_back(tracker);
  }
}

std::shared_ptr<Tracker> TrackerProcessor::createNewTracker(
  const autoware_auto_perception_msgs::msg::DetectedObject & object, const rclcpp::Time & time,
  const geometry_msgs::msg::Transform & self_transform) const
{
  const std::uint8_t label = object_recognition_utils::getHighestProbLabel(object.classification);
  if (tracker_map_.count(label) != 0) {
    const auto tracker = tracker_map_.at(label);
    if (tracker == "bicycle_tracker")
      return std::make_shared<BicycleTracker>(time, object, self_transform);
    if (tracker == "big_vehicle_tracker")
      return std::make_shared<BigVehicleTracker>(time, object, self_transform);
    if (tracker == "multi_vehicle_tracker")
      return std::make_shared<MultipleVehicleTracker>(time, object, self_transform);
    if (tracker == "normal_vehicle_tracker")
      return std::make_shared<NormalVehicleTracker>(time, object, self_transform);
    if (tracker == "pass_through_tracker")
      return std::make_shared<PassThroughTracker>(time, object, self_transform);
    if (tracker == "pedestrian_and_bicycle_tracker")
      return std::make_shared<PedestrianAndBicycleTracker>(time, object, self_transform);
    if (tracker == "pedestrian_tracker")
      return std::make_shared<PedestrianTracker>(time, object, self_transform);
  }
  return std::make_shared<UnknownTracker>(time, object, self_transform);
}

void TrackerProcessor::prune(const rclcpp::Time & time)
{
  // check lifetime: if the tracker is old, delete it
  removeOldTracker(time);
  // check overlap: if the tracker is overlapped, delete the one with lower IOU
  removeOverlappedTracker(time);
}

void TrackerProcessor::removeOldTracker(const rclcpp::Time & time)
{
  // check elapsed time from last update
  for (auto itr = list_tracker_.begin(); itr != list_tracker_.end(); ++itr) {
    const bool is_old = max_elapsed_time_ < (*itr)->getElapsedTimeFromLastUpdate(time);
    // if the tracker is old, delete it
    if (is_old) {
      auto erase_itr = itr;
      --itr;
      list_tracker_.erase(erase_itr);
    }
  }
}

void TrackerProcessor::removeOverlappedTracker(const rclcpp::Time & time)
{
  // check overlap between trackers
  // if the overlap is too large, delete one of them
  for (auto itr1 = list_tracker_.begin(); itr1 != list_tracker_.end(); ++itr1) {
    autoware_auto_perception_msgs::msg::TrackedObject object1;
    if (!(*itr1)->getTrackedObject(time, object1)) continue;
    for (auto itr2 = std::next(itr1); itr2 != list_tracker_.end(); ++itr2) {
      autoware_auto_perception_msgs::msg::TrackedObject object2;
      if (!(*itr2)->getTrackedObject(time, object2)) continue;

      // if the distance is too large, skip
      const double distance = std::hypot(
        object1.kinematics.pose_with_covariance.pose.position.x -
          object2.kinematics.pose_with_covariance.pose.position.x,
        object1.kinematics.pose_with_covariance.pose.position.y -
          object2.kinematics.pose_with_covariance.pose.position.y);
      if (distance > distance_threshold_) {
        continue;
      }

      // Check IoU between two objects
      const double min_union_iou_area = 1e-2;
      const auto iou = object_recognition_utils::get2dIoU(object1, object2, min_union_iou_area);
      const auto & label1 = (*itr1)->getHighestProbLabel();
      const auto & label2 = (*itr2)->getHighestProbLabel();
      bool should_delete_tracker1 = false;
      bool should_delete_tracker2 = false;

      // If at least one of them is UNKNOWN, delete the younger tracker. Because the UNKNOWN
      // objects are not reliable.
      if (label1 == Label::UNKNOWN || label2 == Label::UNKNOWN) {
        if (iou > min_iou_for_unknown_object_) {
          if (label1 == Label::UNKNOWN && label2 == Label::UNKNOWN) {
            if ((*itr1)->getTotalMeasurementCount() < (*itr2)->getTotalMeasurementCount()) {
              should_delete_tracker1 = true;
            } else {
              should_delete_tracker2 = true;
            }
          } else if (label1 == Label::UNKNOWN) {
            should_delete_tracker1 = true;
          } else if (label2 == Label::UNKNOWN) {
            should_delete_tracker2 = true;
          }
        }
      } else {  // If neither is UNKNOWN, delete the younger tracker
        if (iou > min_iou_) {
          if ((*itr1)->getTotalMeasurementCount() < (*itr2)->getTotalMeasurementCount()) {
            should_delete_tracker1 = true;
          } else {
            should_delete_tracker2 = true;
          }
        }
      }

      // delete the tracker
      if (should_delete_tracker1) {
        itr1 = list_tracker_.erase(itr1);
        --itr1;
        break;
      }
      if (should_delete_tracker2) {
        itr2 = list_tracker_.erase(itr2);
        --itr2;
      }
    }
  }
}

bool TrackerProcessor::isConfidentTracker(const std::shared_ptr<Tracker> & tracker) const
{
  // confidence is measured by counting the number of measurements
  // if the number of measurements is same or more than the threshold, the tracker is confident
  return tracker->getTotalMeasurementCount() >= confident_count_threshold_;
}

void TrackerProcessor::getTrackedObjects(
  const rclcpp::Time & time,
  autoware_auto_perception_msgs::msg::TrackedObjects & tracked_objects) const
{
  tracked_objects.header.stamp = time;
  for (const auto & tracker : list_tracker_) {
    // skip if the tracker is not confident
    if (!isConfidentTracker(tracker)) continue;
    // get the tracked object, extrapolated to the given time
    autoware_auto_perception_msgs::msg::TrackedObject tracked_object;
    if (tracker->getTrackedObject(time, tracked_object)) {
      tracked_objects.objects.push_back(tracked_object);
    }
  }
}

void TrackerProcessor::getTentativeObjects(
  const rclcpp::Time & time,
  autoware_auto_perception_msgs::msg::TrackedObjects & tentative_objects) const
{
  tentative_objects.header.stamp = time;
  for (const auto & tracker : list_tracker_) {
    if (!isConfidentTracker(tracker)) {
      autoware_auto_perception_msgs::msg::TrackedObject tracked_object;
      if (tracker->getTrackedObject(time, tracked_object)) {
        tentative_objects.objects.push_back(tracked_object);
      }
    }
  }
}

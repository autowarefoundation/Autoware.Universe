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

#ifndef MULTI_OBJECT_TRACKER__DEBUGGER__DEBUG_OBJECT_HPP_
#define MULTI_OBJECT_TRACKER__DEBUGGER__DEBUG_OBJECT_HPP_

#include "multi_object_tracker/tracker/model/tracker_base.hpp"

#include <rclcpp/rclcpp.hpp>
#include <tier4_autoware_utils/ros/uuid_helper.hpp>

#include "unique_identifier_msgs/msg/uuid.hpp"
#include <autoware_auto_perception_msgs/msg/detected_objects.hpp>
#include <autoware_auto_perception_msgs/msg/tracked_objects.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct ObjectData
{
  rclcpp::Time time;

  // object uuid
  boost::uuids::uuid uuid;
  int uuid_int;
  std::string uuid_str;

  // association link, pair of coordinates
  // tracker to detection
  geometry_msgs::msg::Point tracker_point;
  geometry_msgs::msg::Point detection_point;
  bool is_associated{false};

  // existence probabilities
  std::vector<float> existence_vector;

  // detection channel id
  uint channel_id;
};

class TrackerObjectDebugger
{
public:
  explicit TrackerObjectDebugger(std::string frame_id);

private:
  bool is_initialized_{false};
  std::string frame_id_;
  visualization_msgs::msg::MarkerArray markers_;
  std::unordered_set<int> current_ids_;
  std::unordered_set<int> previous_ids_;
  rclcpp::Time message_time_;

  std::vector<ObjectData> object_data_list_;
  std::list<int32_t> unused_marker_ids_;
  int32_t marker_id_ = 0;
  std::vector<std::vector<ObjectData>> object_data_groups_;

  std::vector<std::string> channel_names_;

public:
  void setChannelNames(const std::vector<std::string> & channel_names)
  {
    channel_names_ = channel_names;
  }
  void collect(
    const rclcpp::Time & message_time, const std::list<std::shared_ptr<Tracker>> & list_tracker,
    const uint & channel_index,
    const autoware_auto_perception_msgs::msg::DetectedObjects & detected_objects,
    const std::unordered_map<int, int> & direct_assignment,
    const std::unordered_map<int, int> & reverse_assignment);

  void reset();
  void draw(
    const std::vector<std::vector<ObjectData>> object_data_groups,
    visualization_msgs::msg::MarkerArray & marker_array) const;
  void process();
  void getMessage(visualization_msgs::msg::MarkerArray & marker_array) const;

private:
  std::string uuid_to_string(const unique_identifier_msgs::msg::UUID & u) const
  {
    std::stringstream ss;
    for (auto i = 0; i < 16; ++i) {
      ss << std::hex << std::setfill('0') << std::setw(2) << +u.uuid[i];
    }
    return ss.str();
  }

  boost::uuids::uuid to_boost_uuid(const unique_identifier_msgs::msg::UUID & uuid_msg)
  {
    const std::string uuid_str = uuid_to_string(uuid_msg);
    boost::uuids::string_generator gen;
    boost::uuids::uuid uuid = gen(uuid_str);
    return uuid;
  }

  int32_t uuid_to_int(const boost::uuids::uuid & uuid) { return boost::uuids::hash_value(uuid); }
};

#endif  // MULTI_OBJECT_TRACKER__DEBUGGER__DEBUG_OBJECT_HPP_

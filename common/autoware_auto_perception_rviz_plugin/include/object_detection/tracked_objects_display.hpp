// Copyright 2021 Apex.AI, Inc.
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
#ifndef OBJECT_DETECTION__TRACKED_OBJECTS_DISPLAY_HPP_
#define OBJECT_DETECTION__TRACKED_OBJECTS_DISPLAY_HPP_

#include <object_detection/object_polygon_display_base.hpp>

#include <autoware_auto_perception_msgs/msg/tracked_objects.hpp>
#include <autoware_auto_perception_msgs/msg/tracked_object.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <list>
#include <map>
#include <string>
#include <vector>

namespace autoware
{
namespace rviz_plugins
{
namespace object_detection
{
/// \brief Class defining rviz plugin to visualize TrackedObjects
class AUTOWARE_AUTO_PERCEPTION_RVIZ_PLUGIN_PUBLIC TrackedObjectsDisplay
: public ObjectPolygonDisplayBase<autoware_auto_perception_msgs::msg::TrackedObjects>
{
  Q_OBJECT

public:
  using TrackedObjects = autoware_auto_perception_msgs::msg::TrackedObjects;

  TrackedObjectsDisplay();

private:
  void processMessage(TrackedObjects::ConstSharedPtr msg) override;
  void onInitialize() override;
  // void pointCloudCallback(const sensor_msgs::msg::PointCloud2 input_pointcloud_msg) override;

  boost::uuids::uuid to_boost_uuid(const unique_identifier_msgs::msg::UUID & uuid_msg)
  {
    const std::string uuid_str = uuid_to_string(uuid_msg);
    boost::uuids::string_generator gen;
    boost::uuids::uuid uuid = gen(uuid_str);
    return uuid;
  }

  void update_id_map(const TrackedObjects::ConstSharedPtr & msg)
  {
    std::vector<boost::uuids::uuid> new_uuids;
    std::vector<boost::uuids::uuid> tracked_uuids;
    new_uuids.reserve(msg->objects.size());
    tracked_uuids.reserve(msg->objects.size());
    for (const auto & object : msg->objects) {
      const auto uuid = to_boost_uuid(object.object_id);
      ((id_map.find(uuid) != id_map.end()) ? tracked_uuids : new_uuids).push_back(uuid);
    }

    auto itr = id_map.begin();
    while (itr != id_map.end()) {
      if (
        std::find(tracked_uuids.begin(), tracked_uuids.end(), itr->first) == tracked_uuids.end()) {
        unused_marker_ids.push_back(itr->second);
        itr = id_map.erase(itr);
      } else {
        ++itr;
      }
    }

    for (const auto & new_uuid : new_uuids) {
      if (unused_marker_ids.empty()) {
        id_map.emplace(new_uuid, marker_id);
        marker_id++;
      } else {
        id_map.emplace(new_uuid, unused_marker_ids.front());
        unused_marker_ids.pop_front();
      }
    }
  }

  int32_t uuid_to_marker_id(const unique_identifier_msgs::msg::UUID & uuid_msg)
  {
    auto uuid = to_boost_uuid(uuid_msg);
    return id_map.at(uuid);
  }

  void onObjectsAndObstaclePointCloud(const TrackedObjects::ConstSharedPtr & input_objs_msg,
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr & input_pointcloud_msg);

  std::map<boost::uuids::uuid, int32_t> id_map;
  std::list<int32_t> unused_marker_ids;
  int32_t marker_id = 0;

  typedef message_filters::sync_policies::ApproximateTime<TrackedObjects, sensor_msgs::msg::PointCloud2>SyncPolicy;
  typedef message_filters::Synchronizer<SyncPolicy> Sync;
  typename std::shared_ptr<Sync> sync_ptr_;

  // std::string objects_frame_id_;
  // std::vector<autoware_auto_perception_msgs::msg::TrackedObject> objs_buffer;
};

}  // namespace object_detection
}  // namespace rviz_plugins
}  // namespace autoware

#endif  // OBJECT_DETECTION__TRACKED_OBJECTS_DISPLAY_HPP_

// Copyright 2023 TIER IV, Inc.
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

// Copyright 2024 AutoCore, Inc.
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

#include <bytetrack3D/bytetrack3D.hpp>

#include <algorithm>
#include <fstream>
#include <functional>

namespace bytetrack3D
{
ByteTrack3D::ByteTrack3D(const int track_buffer_length)
{
  // Tracker initialization
  tracker_ = std::make_unique<ByteTracker>(track_buffer_length);
}

bool ByteTrack3D::do_inference(ObjectArray & objects)
{
  // Re-format data
  std::vector<ByteTrackObject> bytetrack_objects;
  for (const auto & obj : objects) {
    ByteTrackObject bytetrack_obj;
    bytetrack_obj.x = obj.x;
    bytetrack_obj.y = obj.y;
    bytetrack_obj.z = obj.z;
    bytetrack_obj.yaw = obj.yaw;
    bytetrack_obj.l = obj.l;
    bytetrack_obj.w = obj.w;
    bytetrack_obj.h = obj.h;
    bytetrack_obj.prob = obj.score;
    bytetrack_obj.label = obj.type;
    bytetrack_objects.emplace_back(bytetrack_obj);
  }

  // cspell: ignore stracks tlwh
  // Update tracker
  std::vector<STrack> output_stracks = tracker_->update(bytetrack_objects);

  // Pack results
  latest_objects_.clear();
  for (const auto & tracking_result : output_stracks) {
    Object object{};
    std::vector<float> pose = tracking_result.pose;
    std::vector<float> lwh = tracking_result.lwh;
    std::vector<float> velocity = tracking_result.velocity;
    object.x = pose[0];
    object.y = pose[1];
    object.z = pose[2];
    object.yaw = pose[3];
    object.l = lwh[0];
    object.w = lwh[1];
    object.h = lwh[2];
    object.vx = velocity[0];
    object.vy = velocity[1];
    object.vz = velocity[2];
    object.vyaw = velocity[3];
    object.score = tracking_result.score;
    object.type = tracking_result.label;
    object.track_id = tracking_result.track_id;
    object.unique_id = tracking_result.unique_id;
    latest_objects_.emplace_back(object);
  }

  return true;
}

ObjectArray ByteTrack3D::update_tracker(ObjectArray & input_objects)
{
  do_inference(input_objects);
  return latest_objects_;
}
}  // namespace bytetrack3D

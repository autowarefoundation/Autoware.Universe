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

#ifndef BYTETRACK3D__BYTETRACK3D_HPP_
#define BYTETRACK3D__BYTETRACK3D_HPP_

#include "byte_tracker.h"
#include "strack.h"

#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include <boost/uuid/uuid.hpp>

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

namespace bytetrack3D
{
struct Object
{
  float x, y, z, yaw;
  float l, w, h;
  float vx, vy, vz, vyaw;
  float score;
  int32_t type;
  int32_t track_id;
  boost::uuids::uuid unique_id;
};

using ObjectArray = std::vector<Object>;

class ByteTrack3D
{
public:
  explicit ByteTrack3D(const int track_buffer_length = 30);

  bool do_inference(ObjectArray & objects);
  ObjectArray update_tracker(ObjectArray & input_objects);

private:
  std::unique_ptr<ByteTracker> tracker_;
  ObjectArray latest_objects_;
};

}  // namespace bytetrack3D

#endif  // BYTETRACK3D__BYTETRACK3D_HPP_

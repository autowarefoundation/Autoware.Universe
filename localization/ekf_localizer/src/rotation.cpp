// Copyright 2023  Autoware Foundation
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

#include "ekf_localizer/rotation.hpp"

Eigen::Vector3d quaternionToEulerXYZ(const tf2::Quaternion & q)
{
  double roll = 0.0, pitch = 0.0, yaw = 0.0;
  tf2::Matrix3x3(q).getRPY(roll, pitch, yaw);
  return Eigen::Vector3d(roll, pitch, yaw);
}

Eigen::Vector3d quaternionToEulerXYZ(
  const geometry_msgs::msg::Quaternion & quaternion)
{
  tf2::Quaternion q;
  tf2::fromMsg(quaternion, q);
  return quaternionToEulerXYZ(q);
}

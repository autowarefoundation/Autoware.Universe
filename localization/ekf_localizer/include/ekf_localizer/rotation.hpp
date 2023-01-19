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

#ifndef EKF_LOCALIZER__ROTATION_HPP_
#define EKF_LOCALIZER__ROTATION_HPP_

#include <Eigen/Core>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <geometry_msgs/msg/quaternion.hpp>


Eigen::Vector3d quaternionToEulerXYZ(const tf2::Quaternion & q);
Eigen::Vector3d quaternionToEulerXYZ(const geometry_msgs::msg::Quaternion & orientation);

#endif  // EKF_LOCALIZER__ROTATION_HPP_

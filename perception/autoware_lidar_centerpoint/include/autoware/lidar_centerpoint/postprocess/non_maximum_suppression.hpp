// Copyright 2022 TIER IV, Inc.
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

#ifndef AUTOWARE__LIDAR_CENTERPOINT__POSTPROCESS__NON_MAXIMUM_SUPPRESSION_HPP_
#define AUTOWARE__LIDAR_CENTERPOINT__POSTPROCESS__NON_MAXIMUM_SUPPRESSION_HPP_

#include "autoware/lidar_centerpoint/ros_utils.hpp"

#include <Eigen/Eigen>

#include "autoware_perception_msgs/msg/detected_object.hpp"

#include <string>
#include <vector>

namespace autoware::lidar_centerpoint
{
using autoware_perception_msgs::msg::DetectedObject;

struct NMSParams
{
  double search_distance_2d_{};
  double iou_threshold_{};
};

class NonMaximumSuppression
{
public:
  void setParameters(const NMSParams &);

  std::vector<DetectedObject> apply(const std::vector<DetectedObject> &);

private:
  bool isTargetPairObject(const DetectedObject &, const DetectedObject &);

  Eigen::MatrixXd generateIoUMatrix(const std::vector<DetectedObject> &);

  NMSParams params_{};

  double search_distance_2d_sq_{};
};

}  // namespace autoware::lidar_centerpoint

#endif  // AUTOWARE__LIDAR_CENTERPOINT__POSTPROCESS__NON_MAXIMUM_SUPPRESSION_HPP_

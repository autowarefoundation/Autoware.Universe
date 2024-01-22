// Copyright 2023 Autoware Foundation
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

#ifndef LOCALIZATION__POSE_ESTIMATOR_ARBITER__EXAMPLE_RULE__POSE_ESTIMATOR_ARBITER__RULE_HELPER__AR_TAG_POSITION_HPP_
#define LOCALIZATION__POSE_ESTIMATOR_ARBITER__EXAMPLE_RULE__POSE_ESTIMATOR_ARBITER__RULE_HELPER__AR_TAG_POSITION_HPP_

#include "pose_estimator_arbiter/shared_data.hpp"

#include <rclcpp/logger.hpp>
#include <rclcpp/node.hpp>

#include <autoware_auto_mapping_msgs/msg/had_map_bin.hpp>
#include <geometry_msgs/msg/pose.hpp>

#include <memory>
#include <string>

namespace pose_estimator_arbiter::rule_helper
{
// This class finds the distance to the nearest landmark for AR tag based localization.
class ArTagPosition
{
  using HADMapBin = autoware_auto_mapping_msgs::msg::HADMapBin;
  using Pose = geometry_msgs::msg::Pose;

public:
  explicit ArTagPosition(rclcpp::Node * node);

  double distance_to_nearest_ar_tag_around_ego(const geometry_msgs::msg::Point & ego_point) const;

  void init(HADMapBin::ConstSharedPtr msg);

private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
  rclcpp::Logger logger_;
};
}  // namespace pose_estimator_arbiter::rule_helper

#endif  // LOCALIZATION__POSE_ESTIMATOR_ARBITER__EXAMPLE_RULE__POSE_ESTIMATOR_ARBITER__RULE_HELPER__AR_TAG_POSITION_HPP_

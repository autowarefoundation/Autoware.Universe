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

#ifndef POSE_ESTIMATOR_ARBITER__RULE_HELPER__POSE_ESTIMATOR_AREA_HPP_
#define POSE_ESTIMATOR_ARBITER__RULE_HELPER__POSE_ESTIMATOR_AREA_HPP_

#include <rclcpp/logger.hpp>
#include <rclcpp/node.hpp>

#include <autoware_auto_mapping_msgs/msg/had_map_bin.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <memory>
#include <string>

namespace pose_estimator_arbiter::rule_helper
{
class PoseEstimatorArea
{
  using HADMapBin = autoware_auto_mapping_msgs::msg::HADMapBin;
  using Marker = visualization_msgs::msg::Marker;
  using MarkerArray = visualization_msgs::msg::MarkerArray;

public:
  explicit PoseEstimatorArea(rclcpp::Node * node);
  explicit PoseEstimatorArea(const rclcpp::Logger & logger);

  // This is assumed to be called only once in the vector map callback.
  void init(const HADMapBin::ConstSharedPtr msg);

  bool within(
    const geometry_msgs::msg::Point & point, const std::string & pose_estimator_name) const;

  MarkerArray debug_marker_array() const;

private:
  struct Impl;
  std::shared_ptr<Impl> impl_{nullptr};
  rclcpp::Logger logger_;
};

}  // namespace pose_estimator_arbiter::rule_helper

#endif  // POSE_ESTIMATOR_ARBITER__RULE_HELPER__POSE_ESTIMATOR_AREA_HPP_

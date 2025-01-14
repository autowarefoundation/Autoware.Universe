// Copyright 2020 Tier IV, Inc.
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
// Author: v1.0 Yukihiro Saito
//

#ifndef AUTOWARE__MULTI_OBJECT_TRACKER__TRACKER__MODEL__VEHICLE_TRACKER_HPP_
#define AUTOWARE__MULTI_OBJECT_TRACKER__TRACKER__MODEL__VEHICLE_TRACKER_HPP_

#include "autoware/multi_object_tracker/object_model/object_model.hpp"
#include "autoware/multi_object_tracker/object_model/types.hpp"
#include "autoware/multi_object_tracker/tracker/model/tracker_base.hpp"
#include "autoware/multi_object_tracker/tracker/motion_model/bicycle_motion_model.hpp"

#include <autoware/kalman_filter/kalman_filter.hpp>

namespace autoware::multi_object_tracker
{

class VehicleTracker : public Tracker
{
private:
  object_model::ObjectModel object_model_;
  rclcpp::Logger logger_;

  double velocity_deviation_threshold_;

  types::DynamicObject object_;
  double z_;

  struct BoundingBox
  {
    double length;
    double width;
    double height;
  };
  BoundingBox bounding_box_;
  Eigen::Vector2d tracking_offset_;

  BicycleMotionModel motion_model_;
  using IDX = BicycleMotionModel::IDX;

public:
  VehicleTracker(
    const object_model::ObjectModel & object_model, const rclcpp::Time & time,
    const types::DynamicObject & object, const size_t channel_size);

  bool predict(const rclcpp::Time & time) override;
  bool measure(
    const types::DynamicObject & object, const rclcpp::Time & time,
    const geometry_msgs::msg::Transform & self_transform) override;
  bool measureWithPose(const types::DynamicObject & object);
  bool measureWithShape(const types::DynamicObject & object);
  bool getTrackedObject(const rclcpp::Time & time, types::DynamicObject & object) const override;

private:
  types::DynamicObject getUpdatingObject(
    const types::DynamicObject & object, const geometry_msgs::msg::Transform & self_transform);
};

}  // namespace autoware::multi_object_tracker

#endif  // AUTOWARE__MULTI_OBJECT_TRACKER__TRACKER__MODEL__VEHICLE_TRACKER_HPP_

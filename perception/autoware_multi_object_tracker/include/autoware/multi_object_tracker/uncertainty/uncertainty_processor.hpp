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
//
//
// Author: v1.0 Taekjin Lee

#ifndef AUTOWARE__MULTI_OBJECT_TRACKER__UNCERTAINTY__UNCERTAINTY_PROCESSOR_HPP_
#define AUTOWARE__MULTI_OBJECT_TRACKER__UNCERTAINTY__UNCERTAINTY_PROCESSOR_HPP_

#include "autoware/multi_object_tracker/object_model/object_model.hpp"
#include "autoware/multi_object_tracker/object_model/types.hpp"

#include <autoware/object_recognition_utils/object_recognition_utils.hpp>

#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <nav_msgs/msg/odometry.hpp>

namespace autoware::multi_object_tracker
{

namespace uncertainty
{

using autoware::multi_object_tracker::object_model::ObjectModel;
using autoware_perception_msgs::msg::ObjectClassification;
using nav_msgs::msg::Odometry;

ObjectModel decodeObjectModel(const ObjectClassification & object_class);

types::DynamicObjectList modelUncertainty(const types::DynamicObjectList & detected_objects);

object_model::StateCovariance covarianceFromObjectClass(
  const types::DynamicObject & detected_object, const ObjectClassification & object_class);

void normalizeUncertainty(types::DynamicObjectList & detected_objects);

void addOdometryUncertainty(const Odometry & odometry, types::DynamicObjectList & detected_objects);
}  // namespace uncertainty

}  // namespace autoware::multi_object_tracker

#endif  // AUTOWARE__MULTI_OBJECT_TRACKER__UNCERTAINTY__UNCERTAINTY_PROCESSOR_HPP_

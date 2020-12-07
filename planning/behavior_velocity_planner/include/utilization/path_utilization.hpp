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
#pragma once

#include "rclcpp/rclcpp.hpp"

#include "autoware_planning_msgs/msg/path.hpp"

autoware_planning_msgs::msg::Path interpolatePath(
  const autoware_planning_msgs::msg::Path & path, const double length,
  const rclcpp::Logger & logger);
autoware_planning_msgs::msg::Path filterLitterPathPoint(
  const autoware_planning_msgs::msg::Path & path);
autoware_planning_msgs::msg::Path filterStopPathPoint(
  const autoware_planning_msgs::msg::Path & path);

// Copyright 2024 TIER IV
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

#ifndef AUTOWARE_TEST_UTILS__MOCK_DATA_PARSER_HPP_
#define AUTOWARE_TEST_UTILS__MOCK_DATA_PARSER_HPP_

#include <builtin_interfaces/msg/duration.hpp>
#include <builtin_interfaces/msg/time.hpp>

#include <autoware_adapi_v1_msgs/msg/operation_mode_state.hpp>
#include <autoware_internal_planning_msgs/msg/path_with_lane_id.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/tracked_objects.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group_array.hpp>
#include <autoware_planning_msgs/msg/lanelet_primitive.hpp>
#include <autoware_planning_msgs/msg/lanelet_route.hpp>
#include <autoware_planning_msgs/msg/lanelet_segment.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/pose_with_covariance.hpp>
#include <geometry_msgs/msg/twist_with_covariance.hpp>
#include <nav_msgs/msg/odometry.hpp>

#include <yaml-cpp/yaml.h>

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace autoware::test_utils
{
using autoware_adapi_v1_msgs::msg::OperationModeState;
using autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObject;
using autoware_perception_msgs::msg::PredictedObjectKinematics;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_perception_msgs::msg::PredictedPath;
using autoware_perception_msgs::msg::Shape;
using autoware_perception_msgs::msg::TrackedObject;
using autoware_perception_msgs::msg::TrackedObjectKinematics;
using autoware_perception_msgs::msg::TrackedObjects;
using autoware_perception_msgs::msg::TrafficLightElement;
using autoware_perception_msgs::msg::TrafficLightGroup;
using autoware_perception_msgs::msg::TrafficLightGroupArray;

using autoware_internal_planning_msgs::msg::PathPointWithLaneId;
using autoware_internal_planning_msgs::msg::PathWithLaneId;
using autoware_planning_msgs::msg::LaneletPrimitive;
using autoware_planning_msgs::msg::LaneletRoute;
using autoware_planning_msgs::msg::LaneletSegment;
using autoware_planning_msgs::msg::PathPoint;
using builtin_interfaces::msg::Duration;
using builtin_interfaces::msg::Time;
using geometry_msgs::msg::Accel;
using geometry_msgs::msg::AccelWithCovariance;
using geometry_msgs::msg::AccelWithCovarianceStamped;
using geometry_msgs::msg::Point;
using geometry_msgs::msg::Pose;
using geometry_msgs::msg::PoseWithCovariance;
using geometry_msgs::msg::Twist;
using geometry_msgs::msg::TwistWithCovariance;
using nav_msgs::msg::Odometry;
using std_msgs::msg::Header;
using unique_identifier_msgs::msg::UUID;

/**
 * @brief Parses a YAML node and converts it into an object of type T.
 *
 * This function extracts data from the given YAML node and converts it into an object of type T.
 * If no specialization exists for T, it will result in a compile-time error.
 *
 * @tparam T The type of object to parse the node contents into.
 * @param node The YAML node to be parsed.
 * @return T An object of type T containing the parsed data.
 */
template <typename T>
T parse(const YAML::Node & node);

template <>
Header parse(const YAML::Node & node);

template <>
Duration parse(const YAML::Node & node);

template <>
Time parse(const YAML::Node & node);

template <>
Point parse(const YAML::Node & node);

template <>
std::vector<Point> parse(const YAML::Node & node);

template <>
std::array<double, 36> parse(const YAML::Node & node);

template <>
Pose parse(const YAML::Node & node);

template <>
PoseWithCovariance parse(const YAML::Node & node);

template <>
Twist parse(const YAML::Node & node);

template <>
TwistWithCovariance parse(const YAML::Node & node);

template <>
Odometry parse(const YAML::Node & node);

template <>
Accel parse(const YAML::Node & node);

template <>
AccelWithCovariance parse(const YAML::Node & node);

template <>
AccelWithCovarianceStamped parse(const YAML::Node & node);

template <>
LaneletPrimitive parse(const YAML::Node & node);

template <>
std::vector<LaneletPrimitive> parse(const YAML::Node & node);

template <>
std::vector<LaneletSegment> parse(const YAML::Node & node);

template <>
LaneletRoute parse(const YAML::Node & node);

template <>
std::vector<PathPointWithLaneId> parse(const YAML::Node & node);

template <>
UUID parse(const YAML::Node & node);

template <>
PathPoint parse(const YAML::Node & node);

template <>
PathPointWithLaneId parse(const YAML::Node & node);

template <>
PathWithLaneId parse(const YAML::Node & node);

template <>
PredictedPath parse(const YAML::Node & node);

template <>
ObjectClassification parse(const YAML::Node & node);

template <>
Shape parse(const YAML::Node & node);

template <>
PredictedObjectKinematics parse(const YAML::Node & node);

template <>
PredictedObject parse(const YAML::Node & node);

template <>
PredictedObjects parse(const YAML::Node & node);

template <>
TrackedObjectKinematics parse(const YAML::Node & node);

template <>
TrackedObject parse(const YAML::Node & node);

template <>
TrackedObjects parse(const YAML::Node & node);

template <>
TrafficLightGroupArray parse(const YAML::Node & node);

template <>
TrafficLightGroup parse(const YAML::Node & node);

template <>
TrafficLightElement parse(const YAML::Node & node);

template <>
OperationModeState parse(const YAML::Node & node);

/**
 * @brief Parses a YAML file and converts it into an object of type T.
 *
 * This function reads the specified YAML file and converts its contents into an object of the given
 * type T. If no specialization exists for T, it will result in a compile-time error.
 *
 * @tparam T The type of object to parse the file contents into.
 * @param filename The path to the YAML file to be parsed.
 * @return T An object of type T containing the parsed data.
 */
template <typename T>
T parse(const std::string & filename);

template <>
std::optional<LaneletRoute> parse(const std::string & filename);

template <>
std::optional<PathWithLaneId> parse(const std::string & filename);

template <typename MessageType>
auto create_const_shared_ptr(MessageType && payload)
{
  using UnqualifiedType = typename std::decay_t<MessageType>;
  return std::make_shared<const UnqualifiedType>(std::forward<MessageType>(payload));
}

}  // namespace autoware::test_utils

#endif  // AUTOWARE_TEST_UTILS__MOCK_DATA_PARSER_HPP_

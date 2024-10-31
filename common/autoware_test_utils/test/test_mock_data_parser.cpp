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

#include <gtest/gtest.h>

// Assuming the parseRouteFile function is in 'RouteHandler.h'
#include "ament_index_cpp/get_package_share_directory.hpp"
#include "autoware_test_utils/mock_data_parser.hpp"

#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

namespace autoware::test_utils
{
using tier4_planning_msgs::msg::PathWithLaneId;

// Example YAML structure as a string for testing
const char g_complete_yaml[] = R"(
start_pose:
  position:
    x: 1.0
    y: 2.0
    z: 3.0
  orientation:
    x: 0.1
    y: 0.2
    z: 0.3
    w: 0.4
goal_pose:
  position:
    x: 4.0
    y: 5.0
    z: 6.0
  orientation:
    x: 0.5
    y: 0.6
    z: 0.7
    w: 0.8
segments:
- preferred_primitive:
    id: 11
    primitive_type: ''
  primitives:
    - id: 22
      primitive_type: lane
    - id: 33
      primitive_type: lane
self_odometry:
  header:
    stamp:
      sec: 100
      nanosec: 100
    frame_id: map
  child_frame_id: base_link
  pose:
    pose:
      position:
        x: 100
        y: 200
        z: 300
      orientation:
        x: 0.00000
        y: 0.00000
        z: 0.282884
        w: 0.959154
    covariance:
      - 0.000100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.000100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
  twist:
    twist:
      linear:
        x: 1.00000
        y: 2.00000
        z: 3.00000
      angular:
        x: 1.00000
        y: 2.00000
        z: 3.00000
    covariance:
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
self_acceleration:
  header:
    stamp:
      sec: 100
      nanosec: 100
    frame_id: /base_link
  accel:
    accel:
      linear:
        x: 1.00000
        y: 2.00000
        z: 3.00000
      angular:
        x: 1.00000
        y: 2.00000
        z: 3.00000
    covariance:
      - 0.00100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00100000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00000
      - 0.00100000
dynamic_object:
  header:
    stamp:
      sec: 348
      nanosec: 334918846
    frame_id: map
  objects:
    - object_id:
        uuid:
          - 233
          - 198
          - 185
          - 212
          - 242
          - 49
          - 117
          - 203
          - 59
          - 251
          - 182
          - 168
          - 212
          - 44
          - 72
          - 132
      existence_probability: 0.00000
      classification:
        - label: 1
          probability: 1.00000
      kinematics:
        initial_pose_with_covariance:
          pose:
            position:
              x: 89616.1
              y: 42266.7
              z: 7.68325
            orientation:
              x: 0.00000
              y: 0.00000
              z: 0.882204
              w: 0.470868
          covariance:
            - 0.0554278
            - -0.0187974
            - 0.000225879
            - 0.00000
            - 0.00000
            - 0.00000
            - -0.0187974
            - 0.0707268
            - -0.00540388
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.000225879
            - -0.00540388
            - 0.0100000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.0100000
            - 1.40223e-05
            - 0.000549930
            - 0.00000
            - 0.00000
            - 0.00000
            - 1.40223e-05
            - 0.0100000
            - 0.00310481
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.000549930
            - 0.00310481
            - 0.00264476
        initial_twist_with_covariance:
          twist:
            linear:
              x: 8.01698
              y: -0.00152468
              z: 0.00000
            angular:
              x: 0.00000
              y: 0.00000
              z: -0.00138607
          covariance:
            - 0.618731
            - -0.000104739
            - 0.00000
            - 0.00000
            - 0.00000
            - -9.52170e-05
            - -0.000104739
            - 0.00158706
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00144278
            - 0.00000
            - 0.00000
            - 0.0100000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.0100000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.0100000
            - 0.00000
            - -9.52170e-05
            - 0.00144278
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00131162
        initial_acceleration_with_covariance:
          accel:
            linear:
              x: 0.00000
              y: 0.00000
              z: 0.00000
            angular:
              x: 0.00000
              y: 0.00000
              z: 0.00000
          covariance:
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
            - 0.00000
        predicted_paths:
          - path:
              - position:
                  x: 89557.0
                  y: 42366.9
                  z: 6.82636
                orientation:
                  x: 0.00000
                  y: 0.00000
                  z: 0.871073
                  w: 0.491154
              - position:
                  x: 89554.9
                  y: 42370.3
                  z: 6.81617
                orientation:
                  x: 0.00000
                  y: 0.00000
                  z: 0.870056
                  w: 0.492952
            time_step:
              sec: 0
              nanosec: 500000000
            confidence: 0.0476190
          - path:
              - position:
                  x: 89616.1
                  y: 42266.7
                  z: 7.68325
                orientation:
                  x: 0.00000
                  y: 0.00000
                  z: 0.882204
                  w: 0.470868
              - position:
                  x: 89613.8
                  y: 42270.0
                  z: 7.27790
                orientation:
                  x: 0.00000
                  y: 0.00000
                  z: 0.882135
                  w: 0.470997
            time_step:
              sec: 0
              nanosec: 500000000
            confidence: 0.158730
      shape:
        type: 0
        footprint:
          points: []
        dimensions:
          x: 4.40000
          y: 1.85564
          z: 1.74263
traffic_signal:
  stamp:
    sec: 1730184609
    nanosec: 816275300
  traffic_light_groups:
    - traffic_light_group_id: 10352
      elements:
        - color: 3
          shape: 1
          status: 2
          confidence: 1.00000
operation_mode:
  stamp:
    sec: 0
    nanosec: 204702031
  mode: 1
  is_autoware_control_enabled: true
  is_in_transition: false
  is_stop_mode_available: true
  is_autonomous_mode_available: true
  is_local_mode_available: true
  is_remote_mode_available: true
)";

TEST(ParseFunctions, CompleteYAMLTest)
{
  YAML::Node config = YAML::Load(g_complete_yaml);

  // Test parsing of start_pose and goal_pose
  Pose start_pose = parse<Pose>(config["start_pose"]);
  Pose goal_pose = parse<Pose>(config["goal_pose"]);

  EXPECT_DOUBLE_EQ(start_pose.position.x, 1.0);
  EXPECT_DOUBLE_EQ(start_pose.position.y, 2.0);
  EXPECT_DOUBLE_EQ(start_pose.position.z, 3.0);

  EXPECT_DOUBLE_EQ(start_pose.orientation.x, 0.1);
  EXPECT_DOUBLE_EQ(start_pose.orientation.y, 0.2);
  EXPECT_DOUBLE_EQ(start_pose.orientation.z, 0.3);
  EXPECT_DOUBLE_EQ(start_pose.orientation.w, 0.4);

  EXPECT_DOUBLE_EQ(goal_pose.position.x, 4.0);
  EXPECT_DOUBLE_EQ(goal_pose.position.y, 5.0);
  EXPECT_DOUBLE_EQ(goal_pose.position.z, 6.0);
  EXPECT_DOUBLE_EQ(goal_pose.orientation.x, 0.5);
  EXPECT_DOUBLE_EQ(goal_pose.orientation.y, 0.6);
  EXPECT_DOUBLE_EQ(goal_pose.orientation.z, 0.7);
  EXPECT_DOUBLE_EQ(goal_pose.orientation.w, 0.8);

  // Test parsing of segments
  const auto segments = parse<std::vector<LaneletSegment>>(config["segments"]);
  ASSERT_EQ(
    segments.size(), uint64_t(1));  // Assuming only one segment in the provided YAML for this test

  const auto & segment0 = segments[0];
  EXPECT_EQ(segment0.preferred_primitive.id, 11);
  EXPECT_EQ(segment0.primitives.size(), uint64_t(2));
  EXPECT_EQ(segment0.primitives[0].id, 22);
  EXPECT_EQ(segment0.primitives[0].primitive_type, "lane");
  EXPECT_EQ(segment0.primitives[1].id, 33);
  EXPECT_EQ(segment0.primitives[1].primitive_type, "lane");

  const auto self_odometry = parse<Odometry>(config["self_odometry"]);
  EXPECT_DOUBLE_EQ(self_odometry.header.stamp.sec, 100);
  EXPECT_DOUBLE_EQ(self_odometry.header.stamp.nanosec, 100);
  EXPECT_EQ(self_odometry.header.frame_id, "map");
  EXPECT_EQ(self_odometry.child_frame_id, "base_link");
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.position.x, 100);
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.position.y, 200);
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.position.z, 300);
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.orientation.x, 0.0);
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.orientation.y, 0.0);
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.orientation.z, 0.282884);
  EXPECT_DOUBLE_EQ(self_odometry.pose.pose.orientation.w, 0.959154);
  EXPECT_DOUBLE_EQ(self_odometry.pose.covariance[0], 0.0001);
  EXPECT_DOUBLE_EQ(self_odometry.twist.twist.linear.x, 1.0);
  EXPECT_DOUBLE_EQ(self_odometry.twist.twist.linear.y, 2.0);
  EXPECT_DOUBLE_EQ(self_odometry.twist.twist.linear.z, 3.0);
  EXPECT_DOUBLE_EQ(self_odometry.twist.twist.angular.x, 1.0);
  EXPECT_DOUBLE_EQ(self_odometry.twist.twist.angular.y, 2.0);
  EXPECT_DOUBLE_EQ(self_odometry.twist.twist.angular.z, 3.0);

  const auto self_acceleration = parse<AccelWithCovarianceStamped>(config["self_acceleration"]);
  EXPECT_DOUBLE_EQ(self_acceleration.header.stamp.sec, 100);
  EXPECT_DOUBLE_EQ(self_acceleration.header.stamp.nanosec, 100);
  EXPECT_EQ(self_acceleration.header.frame_id, "/base_link");
  EXPECT_DOUBLE_EQ(self_acceleration.accel.accel.linear.x, 1.00);
  EXPECT_DOUBLE_EQ(self_acceleration.accel.accel.linear.y, 2.00);
  EXPECT_DOUBLE_EQ(self_acceleration.accel.accel.linear.z, 3.00);
  EXPECT_DOUBLE_EQ(self_acceleration.accel.accel.angular.x, 1.00);
  EXPECT_DOUBLE_EQ(self_acceleration.accel.accel.angular.y, 2.00);
  EXPECT_DOUBLE_EQ(self_acceleration.accel.accel.angular.z, 3.00);
  EXPECT_DOUBLE_EQ(self_acceleration.accel.covariance[0], 0.001);

  const auto dynamic_object = parse<PredictedObjects>(config["dynamic_object"]);
  EXPECT_EQ(dynamic_object.header.stamp.sec, 348);
  EXPECT_EQ(dynamic_object.header.stamp.nanosec, 334918846);
  EXPECT_EQ(dynamic_object.header.frame_id, "map");
  EXPECT_EQ(dynamic_object.objects.at(0).object_id.uuid.at(0), 233);
  EXPECT_EQ(dynamic_object.objects.at(0).classification.at(0).label, 1);
  EXPECT_DOUBLE_EQ(dynamic_object.objects.at(0).classification.at(0).probability, 1.0);
  EXPECT_DOUBLE_EQ(
    dynamic_object.objects.at(0).kinematics.initial_pose_with_covariance.pose.position.x, 89616.1);
  EXPECT_DOUBLE_EQ(
    dynamic_object.objects.at(0).kinematics.initial_twist_with_covariance.twist.linear.x, 8.01698);
  EXPECT_DOUBLE_EQ(
    dynamic_object.objects.at(0).kinematics.predicted_paths.at(0).path.at(0).position.x, 89557.0);
  EXPECT_EQ(dynamic_object.objects.at(0).kinematics.predicted_paths.at(0).time_step.sec, 0);
  EXPECT_EQ(
    dynamic_object.objects.at(0).kinematics.predicted_paths.at(0).time_step.nanosec, 500000000);
  EXPECT_FLOAT_EQ(
    dynamic_object.objects.at(0).kinematics.predicted_paths.at(0).confidence, 0.0476190);
  EXPECT_EQ(dynamic_object.objects.at(0).shape.type, 0);
  EXPECT_DOUBLE_EQ(dynamic_object.objects.at(0).shape.dimensions.x, 4.40000);
  EXPECT_DOUBLE_EQ(dynamic_object.objects.at(0).shape.dimensions.y, 1.85564);

  const auto traffic_signal = parse<TrafficLightGroupArray>(config["traffic_signal"]);
  EXPECT_EQ(traffic_signal.stamp.sec, 1730184609);
  EXPECT_EQ(traffic_signal.stamp.nanosec, 816275300);
  EXPECT_EQ(traffic_signal.traffic_light_groups.at(0).traffic_light_group_id, 10352);
  EXPECT_EQ(traffic_signal.traffic_light_groups.at(0).elements.at(0).color, 3);
  EXPECT_EQ(traffic_signal.traffic_light_groups.at(0).elements.at(0).shape, 1);
  EXPECT_EQ(traffic_signal.traffic_light_groups.at(0).elements.at(0).status, 2);
  EXPECT_FLOAT_EQ(traffic_signal.traffic_light_groups.at(0).elements.at(0).confidence, 1.0);

  const auto operation_mode = parse<OperationModeState>(config["operation_mode"]);
  EXPECT_EQ(operation_mode.stamp.sec, 0);
  EXPECT_EQ(operation_mode.stamp.nanosec, 204702031);
  EXPECT_EQ(operation_mode.mode, 1);
  EXPECT_EQ(operation_mode.is_autoware_control_enabled, true);
  EXPECT_EQ(operation_mode.is_in_transition, false);
  EXPECT_EQ(operation_mode.is_stop_mode_available, true);
  EXPECT_EQ(operation_mode.is_autonomous_mode_available, true);
  EXPECT_EQ(operation_mode.is_local_mode_available, true);
  EXPECT_EQ(operation_mode.is_remote_mode_available, true);
}

TEST(ParseFunction, CompleteFromFilename)
{
  const auto autoware_test_utils_dir =
    ament_index_cpp::get_package_share_directory("autoware_test_utils");
  const auto parser_test_route =
    autoware_test_utils_dir + "/test_data/lanelet_route_parser_test.yaml";

  const auto lanelet_route = parse<LaneletRoute>(parser_test_route);
  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.position.x, 1.0);
  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.position.y, 2.0);
  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.position.z, 3.0);

  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.orientation.x, 0.1);
  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.orientation.y, 0.2);
  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.orientation.z, 0.3);
  EXPECT_DOUBLE_EQ(lanelet_route.start_pose.orientation.w, 0.4);

  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.position.x, 4.0);
  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.position.y, 5.0);
  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.position.z, 6.0);
  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.orientation.x, 0.5);
  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.orientation.y, 0.6);
  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.orientation.z, 0.7);
  EXPECT_DOUBLE_EQ(lanelet_route.goal_pose.orientation.w, 0.8);

  ASSERT_EQ(
    lanelet_route.segments.size(),
    uint64_t(2));  // Assuming only one segment in the provided YAML for this test
  const auto & segment1 = lanelet_route.segments[1];
  EXPECT_EQ(segment1.preferred_primitive.id, 44);
  EXPECT_EQ(segment1.primitives.size(), uint64_t(4));
  EXPECT_EQ(segment1.primitives[0].id, 55);
  EXPECT_EQ(segment1.primitives[0].primitive_type, "lane");
  EXPECT_EQ(segment1.primitives[1].id, 66);
  EXPECT_EQ(segment1.primitives[1].primitive_type, "lane");
  EXPECT_EQ(segment1.primitives[2].id, 77);
  EXPECT_EQ(segment1.primitives[2].primitive_type, "lane");
  EXPECT_EQ(segment1.primitives[3].id, 88);
  EXPECT_EQ(segment1.primitives[3].primitive_type, "lane");
}

TEST(ParseFunction, ParsePathWithLaneID)
{
  const auto autoware_test_utils_dir =
    ament_index_cpp::get_package_share_directory("autoware_test_utils");
  const auto parser_test_path =
    autoware_test_utils_dir + "/test_data/path_with_lane_id_parser_test.yaml";

  const auto path = parse<PathWithLaneId>(parser_test_path);
  EXPECT_EQ(path.header.stamp.sec, 20);
  EXPECT_EQ(path.header.stamp.nanosec, 5);

  const auto path_points = path.points;
  const auto & p1 = path_points.front();
  EXPECT_DOUBLE_EQ(p1.point.pose.position.x, 12.9);
  EXPECT_DOUBLE_EQ(p1.point.pose.position.y, 3.8);
  EXPECT_DOUBLE_EQ(p1.point.pose.position.z, 4.7);
  EXPECT_DOUBLE_EQ(p1.point.pose.orientation.x, 1.0);
  EXPECT_DOUBLE_EQ(p1.point.pose.orientation.y, 2.0);
  EXPECT_DOUBLE_EQ(p1.point.pose.orientation.z, 3.0);
  EXPECT_DOUBLE_EQ(p1.point.pose.orientation.w, 4.0);
  EXPECT_FLOAT_EQ(p1.point.longitudinal_velocity_mps, 1.2);
  EXPECT_FLOAT_EQ(p1.point.lateral_velocity_mps, 3.4);
  EXPECT_FLOAT_EQ(p1.point.heading_rate_rps, 5.6);
  EXPECT_TRUE(p1.point.is_final);
  EXPECT_EQ(p1.lane_ids.front(), 912);

  const auto & p2 = path_points.back();
  EXPECT_DOUBLE_EQ(p2.point.pose.position.x, 0.0);
  EXPECT_DOUBLE_EQ(p2.point.pose.position.y, 20.5);
  EXPECT_DOUBLE_EQ(p2.point.pose.position.z, 90.11);
  EXPECT_DOUBLE_EQ(p2.point.pose.orientation.x, 4.0);
  EXPECT_DOUBLE_EQ(p2.point.pose.orientation.y, 3.0);
  EXPECT_DOUBLE_EQ(p2.point.pose.orientation.z, 2.0);
  EXPECT_DOUBLE_EQ(p2.point.pose.orientation.w, 1.0);
  EXPECT_FLOAT_EQ(p2.point.longitudinal_velocity_mps, 2.1);
  EXPECT_FLOAT_EQ(p2.point.lateral_velocity_mps, 4.3);
  EXPECT_FLOAT_EQ(p2.point.heading_rate_rps, 6.5);
  EXPECT_FALSE(p2.point.is_final);
  EXPECT_EQ(p2.lane_ids.front(), 205);

  EXPECT_DOUBLE_EQ(path.left_bound.front().x, 55.0);
  EXPECT_DOUBLE_EQ(path.left_bound.front().y, 66.0);
  EXPECT_DOUBLE_EQ(path.left_bound.front().z, 77.0);

  EXPECT_DOUBLE_EQ(path.right_bound.front().x, 0.55);
  EXPECT_DOUBLE_EQ(path.right_bound.front().y, 0.66);
  EXPECT_DOUBLE_EQ(path.right_bound.front().z, 0.77);
}
}  // namespace autoware::test_utils

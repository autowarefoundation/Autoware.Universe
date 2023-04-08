// Copyright 2023 Tier IV, Inc.
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

#include <planning_interface_test_manager/planning_interface_test_manager.hpp>
#include <planning_interface_test_manager/planning_interface_test_manager_utils.hpp>

namespace planning_test_utils
{

PlanningIntefaceTestManager::PlanningIntefaceTestManager()
{
  test_node_ = std::make_shared<rclcpp::Node>("planning_interface_test_node");
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(test_node_->get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);
}

void PlanningIntefaceTestManager::declareVehicleInfoParams(rclcpp::NodeOptions & node_options)
{
  // for vehicle info
  node_options.append_parameter_override("wheel_radius", 0.5);
  node_options.append_parameter_override("wheel_width", 0.2);
  node_options.append_parameter_override("wheel_base", 3.0);
  node_options.append_parameter_override("wheel_tread", 2.0);
  node_options.append_parameter_override("front_overhang", 1.0);
  node_options.append_parameter_override("rear_overhang", 1.0);
  node_options.append_parameter_override("left_overhang", 0.5);
  node_options.append_parameter_override("right_overhang", 0.5);
  node_options.append_parameter_override("vehicle_height", 1.5);
  node_options.append_parameter_override("max_steer_angle", 0.7);
}

void PlanningIntefaceTestManager::declareNearestSearchDistanceParams(
  rclcpp::NodeOptions & node_options)
{
  node_options.append_parameter_override("ego_nearest_dist_threshold", 3.0);
  node_options.append_parameter_override("ego_nearest_yaw_threshold", 1.046);
}

void PlanningIntefaceTestManager::publishOdometry(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<Odometry>(test_node_, target_node, topic_name, odom_pub_);
}

void PlanningIntefaceTestManager::publishMaxVelocity(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<VelocityLimit>(test_node_, target_node, topic_name, max_velocity_pub_);
}

void PlanningIntefaceTestManager::publishPointCloud(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<PointCloud2>(test_node_, target_node, topic_name, point_cloud_pub_);
}

void PlanningIntefaceTestManager::publishAcceleration(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<AccelWithCovarianceStamped>(
    test_node_, target_node, topic_name, acceleration_pub_);
}

void PlanningIntefaceTestManager::publishPredictedObjects(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<PredictedObjects>(
    test_node_, target_node, topic_name, predicted_objects_pub_);
}

void PlanningIntefaceTestManager::publishExpandStopRange(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<ExpandStopRange>(
    test_node_, target_node, topic_name, expand_stop_range_pub_);
}

void PlanningIntefaceTestManager::publishOccupancyGrid(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<OccupancyGrid>(test_node_, target_node, topic_name, occupancy_grid_pub_);
}

void PlanningIntefaceTestManager::publishCostMap(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<OccupancyGrid>(test_node_, target_node, topic_name, cost_map_pub_);
}

void PlanningIntefaceTestManager::publishMap(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<HADMapBin>(test_node_, target_node, topic_name, map_pub_);
}

void PlanningIntefaceTestManager::publishLaneDrivingScenario(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishScenarioData(
    test_node_, target_node, topic_name, lane_driving_scenario_pub_, Scenario::LANEDRIVING);
}

void PlanningIntefaceTestManager::publishParkingScenario(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishScenarioData(
    test_node_, target_node, topic_name, parking_scenario_pub_, Scenario::PARKING);
}

void PlanningIntefaceTestManager::publishInitialPose(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  publishInitialPoseData(target_node, topic_name);
}

void PlanningIntefaceTestManager::publishParkingState(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<std_msgs::msg::Bool>(
    test_node_, target_node, topic_name, parking_state_pub_);
}

void PlanningIntefaceTestManager::publishTrajectory(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<Trajectory>(test_node_, target_node, topic_name, trajectory_pub_);
}

void PlanningIntefaceTestManager::publishRoute(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<LaneletRoute>(test_node_, target_node, topic_name, route_pub_);
}

void PlanningIntefaceTestManager::publishTF(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<TFMessage>(test_node_, target_node, topic_name, TF_pub_);
}

void PlanningIntefaceTestManager::publishInitialPoseTF(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{

  const double position_x = 3722.16015625;
  const double position_y = 73723.515625;
  const double quaternion_x = 0.;
  const double quaternion_y = 0.;
  const double quaternion_z = 0.23311256049418302;
  const double quaternion_w = 0.9724497591854532;
  geometry_msgs::msg::Quaternion quaternion;
  quaternion.x = quaternion_x;
  quaternion.y = quaternion_y;
  quaternion.z = quaternion_z;
  quaternion.w = quaternion_w;

  
  TransformStamped tf;
  tf.header.stamp = target_node->get_clock()->now();
  tf.header.frame_id = "odom";
  tf.child_frame_id = "base_link";
  tf.transform.translation.x = position_x;
  tf.transform.translation.y = position_y;
  tf.transform.translation.z = 0;
  tf.transform.rotation = quaternion;

  tf2_msgs::msg::TFMessage tf_msg{};
  tf_msg.transforms.emplace_back(std::move(tf));

  test_utils::setPublisher(test_node_, topic_name, initial_pose_tf_pub_);
  initial_pose_tf_pub_->publish(tf_msg);
  test_utils::spinSomeNodes(test_node_, target_node);
}

void PlanningIntefaceTestManager::publishLateralOffset(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<LateralOffset>(test_node_, target_node, topic_name, lateral_offset_pub_);
}

void PlanningIntefaceTestManager::publishOperationModeState(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<OperationModeState>(
    test_node_, target_node, topic_name, operation_mode_state_pub_);
}

void PlanningIntefaceTestManager::publishTrafficSignals(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<TrafficSignalArray>(
    test_node_, target_node, topic_name, traffic_signals_pub_);
}
void PlanningIntefaceTestManager::publishExternalTrafficSignals(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<TrafficSignalArray>(
    test_node_, target_node, topic_name, external_traffic_signals_pub_);
}
void PlanningIntefaceTestManager::publishVirtualTrafficLightState(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<VirtualTrafficLightStateArray>(
    test_node_, target_node, topic_name, virtual_traffic_light_states_pub_);
}
void PlanningIntefaceTestManager::publishExternalCrosswalkStates(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<CrosswalkStatus>(
    test_node_, target_node, topic_name, external_crosswalk_states_pub_);
}
void PlanningIntefaceTestManager::publishExternalIntersectionStates(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::publishData<IntersectionStatus>(
    test_node_, target_node, topic_name, external_intersection_states_pub_);
}

void PlanningIntefaceTestManager::setTrajectoryInputTopicName(std::string topic_name)
{
  input_trajectory_name_ = topic_name;
}

void PlanningIntefaceTestManager::setParkingTrajectoryInputTopicName(std::string topic_name)
{
  input_parking_trajectory_name_ = topic_name;
}

void PlanningIntefaceTestManager::setLaneDrivingTrajectoryInputTopicName(std::string topic_name)
{
  input_lane_driving_trajectory_name_ = topic_name;
}

void PlanningIntefaceTestManager::setRouteInputTopicName(std::string topic_name)
{
  input_route_name_ = topic_name;
}

void PlanningIntefaceTestManager::publishNominalTrajectory(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::setPublisher(test_node_, topic_name, normal_trajectory_pub_);
  normal_trajectory_pub_->publish(test_utils::generateTrajectory<Trajectory>(10, 1.0));
  test_utils::spinSomeNodes(test_node_, target_node);
}

void PlanningIntefaceTestManager::publishNominalRoute(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::setPublisher(test_node_, topic_name, normal_route_pub_);
  normal_route_pub_->publish(test_utils::makeNormalRoute());
  test_utils::spinSomeNodes(test_node_, target_node);
}

void PlanningIntefaceTestManager::publishBehaviorNominalRoute(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  test_utils::setPublisher(test_node_, topic_name, behavior_normal_route_pub_);
  normal_route_pub_->publish(test_utils::makeBehaviorNormalRoute());
  test_utils::spinSomeNodes(test_node_, target_node);
}

void PlanningIntefaceTestManager::setTrajectorySubscriber(std::string topic_name)
{
  test_utils::setSubscriber(test_node_, topic_name, traj_sub_, count_);
}

void PlanningIntefaceTestManager::setRouteSubscriber(std::string topic_name)
{
  test_utils::setSubscriber(test_node_, topic_name, route_sub_, count_);
}
void PlanningIntefaceTestManager::setScenarioSubscriber(std::string topic_name)
{
  test_utils::setSubscriber(test_node_, topic_name, scenario_sub_, count_);
}

void PlanningIntefaceTestManager::setPathWithLaneIdSubscriber(std::string topic_name)
{
  test_utils::setSubscriber(test_node_, topic_name, path_with_lane_id_sub_, count_);
}

// test for normal working
void PlanningIntefaceTestManager::testWithNominalTrajectory(rclcpp::Node::SharedPtr target_node)
{
  publishNominalTrajectory(target_node, input_trajectory_name_);
  test_utils::spinSomeNodes(test_node_, target_node, 2);
}

// check to see if target node is dead.
void PlanningIntefaceTestManager::testWithAbnormalTrajectory(rclcpp::Node::SharedPtr target_node)
{
  ASSERT_NO_THROW(publishAbnormalTrajectory(target_node, Trajectory{}));
  ASSERT_NO_THROW(
    publishAbnormalTrajectory(target_node, test_utils::generateTrajectory<Trajectory>(1, 0.0)));
  ASSERT_NO_THROW(publishAbnormalTrajectory(
    target_node, test_utils::generateTrajectory<Trajectory>(10, 0.0, 0.0, 0.0, 0.0, 1)));
}

void PlanningIntefaceTestManager::publishAbnormalTrajectory(
  rclcpp::Node::SharedPtr target_node, const Trajectory & abnormal_trajectory)
{
  test_utils::setPublisher(test_node_, input_trajectory_name_, abnormal_trajectory_pub_);
  abnormal_trajectory_pub_->publish(abnormal_trajectory);
  test_utils::spinSomeNodes(test_node_, target_node);
}

// test for normal working
void PlanningIntefaceTestManager::testWithNominalRoute(rclcpp::Node::SharedPtr target_node)
{
  std::cerr << "print debug " << __FILE__ << __LINE__ << std::endl;
  publishNominalRoute(target_node, input_route_name_);
  std::cerr << "print debug " << __FILE__ << __LINE__ << std::endl;
  test_utils::spinSomeNodes(test_node_, target_node, 5);
}

// test for normal working
void PlanningIntefaceTestManager::testWithBehaviorNominalRoute(rclcpp::Node::SharedPtr target_node)
{
  std::cerr << "print debug " << __FILE__ << __LINE__ << std::endl;
  publishBehaviorNominalRoute(target_node, input_route_name_);
  std::cerr << "print debug " << __FILE__ << __LINE__ << std::endl;
  test_utils::spinSomeNodes(test_node_, target_node, 5);
}

// check to see if target node is dead.
void PlanningIntefaceTestManager::testWithAbnormalRoute(rclcpp::Node::SharedPtr target_node)
{
  ASSERT_NO_THROW(publishAbnormalRoute(target_node, LaneletRoute{}));
  // ASSERT_NO_THROW(
  //   publishAbnormalRoute(target_node, test_utils::generateRoute<LaneletRoute>(1, 0.0)));
  // ASSERT_NO_THROW(publishAbnormalRoute(
  //   target_node, test_utils::generateRoute<LaneletRoute>(10, 0.0, 0.0, 0.0, 0.0, 1)));
}

void PlanningIntefaceTestManager::publishAbnormalRoute(
  rclcpp::Node::SharedPtr target_node, const LaneletRoute & abnormal_route)
{
  test_utils::setPublisher(test_node_, input_route_name_, abnormal_route_pub_);
  abnormal_route_pub_->publish(abnormal_route);
  test_utils::spinSomeNodes(test_node_, target_node);
}

int PlanningIntefaceTestManager::getReceivedTopicNum()
{
  return count_;
}

void PlanningIntefaceTestManager::publishInitialPoseData(
  rclcpp::Node::SharedPtr target_node, std::string topic_name)
{
  const double position_x = 3722.16015625;
  const double position_y = 73723.515625;
  const double quaternion_x = 0.;
  const double quaternion_y = 0.;
  const double quaternion_z = 0.23311256049418302;
  const double quaternion_w = 0.9724497591854532;
  geometry_msgs::msg::Quaternion quaternion;
  quaternion.x = quaternion_x;
  quaternion.y = quaternion_y;
  quaternion.z = quaternion_z;
  quaternion.w = quaternion_w;
  const double yaw = tf2::getYaw(quaternion);

  std::shared_ptr<Odometry> current_odometry = std::make_shared<Odometry>();
  const std::array<double, 4> start_pose{position_x, position_y, yaw};
  current_odometry->pose.pose = test_utils::create_pose_msg(start_pose);
  current_odometry->header.frame_id = "map";
  // std::string origin_frame_id = "odom";


  std::cerr << "print debug " << __FILE__ << __LINE__ << std::endl;
  // set_initial_state_with_transform(current_odometry);
  std::cerr << "print debug " << __FILE__ << __LINE__ << std::endl;

  test_utils::setPublisher(test_node_, topic_name, initial_pose_pub_);
  initial_pose_pub_->publish(*current_odometry);
  test_utils::spinSomeNodes(test_node_, target_node);
}

void PlanningIntefaceTestManager::set_initial_state_with_transform(Odometry::SharedPtr & odometry)
{
  auto transform = get_transform_msg("odom", odometry->header.frame_id);
  odometry->pose.pose.position.x =
    odometry->pose.pose.position.x + transform.transform.translation.x;
  odometry->pose.pose.position.y =
    odometry->pose.pose.position.y + transform.transform.translation.y;
  odometry->pose.pose.position.z =
    odometry->pose.pose.position.z + transform.transform.translation.z;
}

TransformStamped PlanningIntefaceTestManager::get_transform_msg(
  const std::string parent_frame, const std::string child_frame)
{
  TransformStamped transform;
  while (true) {
    try {
      const auto time_point = tf2::TimePoint(std::chrono::milliseconds(0));
      transform = tf_buffer_->lookupTransform(
        parent_frame, child_frame, time_point, tf2::durationFromSec(0.0));
      break;
    } catch (tf2::TransformException & ex) {
      rclcpp::sleep_for(std::chrono::milliseconds(500));
    }
  }
  return transform;
}

}  // namespace planning_test_utils

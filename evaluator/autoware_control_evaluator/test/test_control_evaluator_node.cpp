// Copyright 2024 Tier IV, Inc.
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

#include "ament_index_cpp/get_package_share_directory.hpp"
#include "gtest/gtest.h"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/time.hpp"
#include "tf2_ros/transform_broadcaster.h"

#include <autoware/control_evaluator/control_evaluator_node.hpp>

#include "autoware_planning_msgs/msg/trajectory.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include <tier4_metric_msgs/msg/metric_array.hpp>

#include "boost/lexical_cast.hpp"

#include <tf2/LinearMath/Quaternion.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

using EvalNode = control_diagnostics::ControlEvaluatorNode;
using Trajectory = autoware_planning_msgs::msg::Trajectory;
using TrajectoryPoint = autoware_planning_msgs::msg::TrajectoryPoint;
using MetricArrayMsg = tier4_metric_msgs::msg::MetricArray;
using geometry_msgs::msg::AccelWithCovarianceStamped;
using nav_msgs::msg::Odometry;

constexpr double epsilon = 1e-6;

class EvalTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);

    rclcpp::NodeOptions options;
    const auto share_dir =
      ament_index_cpp::get_package_share_directory("autoware_control_evaluator");

    dummy_node = std::make_shared<rclcpp::Node>("control_evaluator_test_node");
    eval_node = std::make_shared<EvalNode>(options);
    // Enable all logging in the node
    auto ret = rcutils_logging_set_logger_level(
      dummy_node->get_logger().get_name(), RCUTILS_LOG_SEVERITY_DEBUG);
    if (ret != RCUTILS_RET_OK) {
      std::cerr << "Failed to set logging severity to DEBUG\n";
    }
    ret = rcutils_logging_set_logger_level(
      eval_node->get_logger().get_name(), RCUTILS_LOG_SEVERITY_DEBUG);
    if (ret != RCUTILS_RET_OK) {
      std::cerr << "Failed to set logging severity to DEBUG\n";
    }
    traj_pub_ =
      rclcpp::create_publisher<Trajectory>(dummy_node, "/control_evaluator/input/trajectory", 1);
    odom_pub_ =
      rclcpp::create_publisher<Odometry>(dummy_node, "/control_evaluator/input/odometry", 1);
    acc_pub_ = rclcpp::create_publisher<AccelWithCovarianceStamped>(
      dummy_node, "/control_evaluator/input/acceleration", 1);
    tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(dummy_node);
    publishEgoPose(0.0, 0.0, 0.0);
  }

  ~EvalTest() override { rclcpp::shutdown(); }

  void setTargetMetric(const std::string & metric_str)
  {
    const auto is_target_metric = [metric_str](const auto & metric) {
      return metric.name == metric_str;
    };
    metric_sub_ = rclcpp::create_subscription<MetricArrayMsg>(
      dummy_node, "/control_evaluator/metrics", 1, [=](const MetricArrayMsg::ConstSharedPtr msg) {
        const auto it =
          std::find_if(msg->metric_array.begin(), msg->metric_array.end(), is_target_metric);
        if (it != msg->metric_array.end()) {
          metric_value_ = boost::lexical_cast<double>(it->value);
          metric_updated_ = true;
        }
      });
  }

  Trajectory makeTrajectory(const std::vector<std::pair<double, double>> & traj)
  {
    Trajectory t;
    t.header.frame_id = "map";
    TrajectoryPoint p;
    for (const std::pair<double, double> & point : traj) {
      p.pose.position.x = point.first;
      p.pose.position.y = point.second;
      t.points.push_back(p);
    }
    return t;
  }

  void publishTrajectory(const Trajectory & traj)
  {
    traj_pub_->publish(traj);
    spin_some();
  }

  double publishTrajectoryAndGetMetric(const Trajectory & traj)
  {
    metric_updated_ = false;
    traj_pub_->publish(traj);
    while (!metric_updated_) {
      spin_some();
    }
    return metric_value_;
  }

  void publishEgoPose(const double x, const double y, const double yaw)
  {
    Odometry odom;
    odom.header.frame_id = "map";
    odom.header.stamp = dummy_node->now();
    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, yaw);
    odom.pose.pose.orientation.x = q.x();
    odom.pose.pose.orientation.y = q.y();
    odom.pose.pose.orientation.z = q.z();
    odom.pose.pose.orientation.w = q.w();
    odom_pub_->publish(odom);
    spin_some();
  }

  void publishEgoAcc(const double acc1)
  {
    AccelWithCovarianceStamped acc_msg;
    acc_msg.header.frame_id = "baselink";
    acc_msg.header.stamp = dummy_node->now();
    acc_msg.accel.accel.linear.x = acc1;
    acc_pub_->publish(acc_msg);
    spin_some();
  }

  double publishEgoAccAndGetMetric(const double acc)
  {
    metric_updated_ = false;
    AccelWithCovarianceStamped acc_msg;
    acc_msg.header.frame_id = "baselink";

    acc_msg.header.stamp = dummy_node->now();
    acc_msg.accel.accel.linear.x = acc;
    acc_pub_->publish(acc_msg);
    while (!metric_updated_) {
      spin_some();
    }
    return metric_value_;
  }
  void spin_some()
  {
    rclcpp::spin_some(eval_node);
    rclcpp::spin_some(dummy_node);
    rclcpp::sleep_for(std::chrono::milliseconds(100));
  }

  // Latest metric value
  bool metric_updated_ = false;
  double metric_value_;
  // Node
  rclcpp::Node::SharedPtr dummy_node;
  EvalNode::SharedPtr eval_node;
  // publishers
  rclcpp::Publisher<Trajectory>::SharedPtr traj_pub_;
  rclcpp::Publisher<Odometry>::SharedPtr odom_pub_;
  rclcpp::Publisher<AccelWithCovarianceStamped>::SharedPtr acc_pub_;
  rclcpp::Subscription<MetricArrayMsg>::SharedPtr metric_sub_;
  // TF broadcaster
  std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

TEST_F(EvalTest, TestYawDeviation)
{
  auto setYaw = [](geometry_msgs::msg::Quaternion & msg, const double yaw_rad) {
    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, yaw_rad);
    msg.x = q.x();
    msg.y = q.y();
    msg.z = q.z();
    msg.w = q.w();
  };
  setTargetMetric("yaw_deviation");
  Trajectory t = makeTrajectory({{0.0, 0.0}, {1.0, 0.0}});
  for (auto & p : t.points) {
    setYaw(p.pose.orientation, M_PI);
  }

  publishEgoPose(0.0, 0.0, M_PI);
  EXPECT_NEAR(publishTrajectoryAndGetMetric(t), 0.0, epsilon);

  publishEgoPose(0.0, 0.0, 0.0);
  EXPECT_NEAR(publishTrajectoryAndGetMetric(t), M_PI, epsilon);

  publishEgoPose(0.0, 0.0, -M_PI);
  EXPECT_NEAR(publishTrajectoryAndGetMetric(t), 0.0, epsilon);
}

TEST_F(EvalTest, TestLateralDeviation)
{
  setTargetMetric("lateral_deviation");
  Trajectory t = makeTrajectory({{0.0, 0.0}, {1.0, 0.0}});

  publishEgoPose(0.0, 0.0, 0.0);
  EXPECT_NEAR(publishTrajectoryAndGetMetric(t), 0.0, epsilon);

  publishEgoPose(1.0, 1.0, 0.0);
  EXPECT_NEAR(publishTrajectoryAndGetMetric(t), 1.0, epsilon);
}

TEST_F(EvalTest, TestKinematicStateAcc)
{
  setTargetMetric("kinematic_state/acc");
  Trajectory t = makeTrajectory({{0.0, 0.0}, {1.0, 0.0}});
  publishTrajectory(t);
  publishEgoPose(0.0, 0.0, 0.0);
  EXPECT_NEAR(publishEgoAccAndGetMetric(2.0), 2.0, epsilon);
}

TEST_F(EvalTest, TestKinematicStateJerk)
{
  setTargetMetric("kinematic_state/jerk");
  Trajectory t = makeTrajectory({{0.0, 0.0}, {1.0, 0.0}});
  publishTrajectory(t);
  publishEgoPose(0.0, 0.0, 0.0);
  // there is about 0.1 sec delay in spin_some
  publishEgoAcc(0.0);
  EXPECT_NEAR(publishEgoAccAndGetMetric(1), 10.0, 0.5);
}

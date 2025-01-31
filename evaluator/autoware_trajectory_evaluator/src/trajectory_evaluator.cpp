// Copyright 2025 TIER IV, Inc.
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

#include "autoware/trajectory_evaluator/trajectory_evaluator.hpp"

namespace trajectory_evaluator
{

void store_trajectory(
  const autoware_planning_msgs::msg::Trajectory::ConstSharedPtr traj_msg,
  const nav_msgs::msg::Odometry::ConstSharedPtr odom_msg,
  std::vector<TrajectoryWithTimestamp> & trajectory_history, size_t traj_history_limit)
{
  if (traj_msg != nullptr) {
    TrajectoryWithTimestamp trajectory_with_timestamp;
    const auto & stamp = odom_msg->header.stamp;
    trajectory_with_timestamp.time_stamp =
      static_cast<double>(stamp.sec) + static_cast<double>(stamp.nanosec) * 1e-9;
    std::vector<TrajectoryPointWithTime> trajectory_with_time;
    for (const auto & point : traj_msg->points) {
      TrajectoryPointWithTime point_with_time(point, 0, trajectory_with_timestamp.time_stamp);
      trajectory_with_timestamp.trajectory_points.push_back(point_with_time);
    }

    trajectory_history.push_back(trajectory_with_timestamp);
    if (trajectory_history.size() > traj_history_limit) {
      trajectory_history.erase(trajectory_history.begin());
    }
  }
}

void on_kinematic_state(
  const nav_msgs::msg::Odometry::ConstSharedPtr odom_msg,
  std::vector<TrajectoryWithTimestamp> & trajectory_history,
  std::vector<TimeErrorData> & time_errors, MetricArrayMsg & metrics_msg)
{
  if (odom_msg != nullptr) {
    const double velocity = std::sqrt(
      std::pow(odom_msg->twist.twist.linear.x, 2) + std::pow(odom_msg->twist.twist.linear.y, 2) +
      std::pow(odom_msg->twist.twist.linear.z, 2));

    if (velocity <= 1e-3) {
      return;
    }

    const auto & stamp = odom_msg->header.stamp;
    auto current_time = static_cast<double>(stamp.sec) + static_cast<double>(stamp.nanosec) * 1e-9;
    auto time_at_pose = 0.0;
    double time_error = 0.0;
    for (auto & trajectory : trajectory_history) {
      double min_distance = std::numeric_limits<double>::max();

      auto & trajectory_points = trajectory.trajectory_points;
      std::vector<autoware_planning_msgs::msg::TrajectoryPoint> trajectory_points_sequence;
      for (const auto & traj_point_with_time : trajectory_points) {
        trajectory_points_sequence.push_back(traj_point_with_time.trajectory_point);
      }

      size_t closest_index = 0;
      const geometry_msgs::msg::Pose & pose = odom_msg->pose.pose;
      for (size_t i = 0; i < trajectory.trajectory_points.size(); ++i) {
        const auto & trajectory_point_pose = trajectory.trajectory_points[i].trajectory_point.pose;
        double distance = autoware::universe_utils::calcDistance2d(trajectory_point_pose, pose);

        if (distance < min_distance) {
          min_distance = distance;
          closest_index = i;
        }
      }

      auto closest_point = trajectory.trajectory_points[closest_index];
      const auto & next_closest_point = trajectory.trajectory_points[closest_index + 1];

      geometry_msgs::msg::Point segment_vector;
      segment_vector.x = next_closest_point.trajectory_point.pose.position.x -
                         closest_point.trajectory_point.pose.position.x;
      segment_vector.y = next_closest_point.trajectory_point.pose.position.y -
                         closest_point.trajectory_point.pose.position.y;
      segment_vector.z = next_closest_point.trajectory_point.pose.position.z -
                         closest_point.trajectory_point.pose.position.z;

      geometry_msgs::msg::Point vehicle_vector;
      vehicle_vector.x = pose.position.x - closest_point.trajectory_point.pose.position.x;
      vehicle_vector.y = pose.position.y - closest_point.trajectory_point.pose.position.y;
      vehicle_vector.z = pose.position.z - closest_point.trajectory_point.pose.position.z;

      double progress = vehicle_vector.x * segment_vector.x + vehicle_vector.y * segment_vector.y +
                        vehicle_vector.z * segment_vector.z;

      double segment_length_squared = segment_vector.x * segment_vector.x +
                                      segment_vector.y * segment_vector.y +
                                      segment_vector.z * segment_vector.z;

      if (progress >= 0 && progress <= segment_length_squared) {
        double distance =
          autoware::universe_utils::calcDistance2d(pose, next_closest_point.trajectory_point.pose);
        const double time_delta = distance / velocity;
        closest_point.time_stamp = current_time;
        closest_point.time_from_start = time_delta;
        if (!time_errors.empty()) {
          time_at_pose = current_time - time_errors.back().stamp;
          time_error = std::abs(time_at_pose - time_errors.back().expected_time);
        } else {
          time_at_pose = current_time;
        }

        time_errors.push_back(TimeErrorData{
          current_time,                                  // stamp
          closest_index,                                 // trajectory_index
          closest_point.trajectory_point.pose.position,  // position
          closest_point.time_from_start,                 // expected_time
          time_at_pose,                                  // actual_time
          time_error                                     // time_error
        });

        MetricMsg expected_time_metric;
        expected_time_metric.name = "trajectory_metrics/expected_time";
        expected_time_metric.value = closest_point.time_from_start;
        metrics_msg.metric_array.push_back(expected_time_metric);

        MetricMsg actual_time_metric;
        actual_time_metric.name = "trajectory_metrics/actual_time";
        actual_time_metric.value = time_at_pose;
        metrics_msg.metric_array.push_back(actual_time_metric);

        MetricMsg time_error_metric;
        time_error_metric.name = "trajectory_metrics/time_error";
        time_error_metric.value = time_error;
        metrics_msg.metric_array.push_back(time_error_metric);

        MetricMsg closest_traj_x;
        closest_traj_x.name = "trajectory_metrics/closest_traj_x";
        closest_traj_x.value = closest_point.trajectory_point.pose.position.x;
        metrics_msg.metric_array.push_back(closest_traj_x);

        MetricMsg closest_traj_y;
        closest_traj_y.name = "trajectory_metrics/closest_traj_y";
        closest_traj_y.value = closest_point.trajectory_point.pose.position.y;
        metrics_msg.metric_array.push_back(closest_traj_y);

        MetricMsg closest_traj_z;
        closest_traj_z.name = "trajectory_metrics/closest_traj_z";
        closest_traj_z.value = closest_point.trajectory_point.pose.position.z;
        metrics_msg.metric_array.push_back(closest_traj_z);

        MetricMsg current_pose_x;
        current_pose_x.name = "trajectory_metrics/current_pose_x";
        current_pose_x.value = pose.position.x;
        metrics_msg.metric_array.push_back(closest_traj_x);

        MetricMsg current_pose_y;
        current_pose_y.name = "trajectory_metrics/current_pose_y";
        current_pose_y.value = pose.position.y;
        metrics_msg.metric_array.push_back(closest_traj_y);

        MetricMsg current_pose_z;
        current_pose_z.name = "trajectory_metrics/current_pose_z";
        current_pose_z.value = pose.position.z;
        metrics_msg.metric_array.push_back(current_pose_z);
      }
    }
  }
}

}  // namespace trajectory_evaluator

// Copyright 2022 TIER IV, Inc.
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

#ifndef DYNAMIC_OBSTACLE_HPP_
#define DYNAMIC_OBSTACLE_HPP_

#include "debug.hpp"
#include "utils.hpp"

#include <autoware/behavior_velocity_planner_common/planner_data.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/path_utilization.hpp>
#include <autoware/behavior_velocity_planner_common/utilization/util.hpp>

#include <autoware_perception_msgs/msg/predicted_object.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/exact_time.h>
#include <message_filters/synchronizer.h>
#include <pcl/common/transforms.h>
#include <pcl_conversions/pcl_conversions.h>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_eigen/tf2_eigen.h>
#else
#include <tf2_eigen/tf2_eigen.hpp>
#endif

#include <tf2_ros/transform_listener.h>

#include <memory>
#include <string>
#include <vector>

namespace autoware::behavior_velocity_planner
{
using autoware_internal_planning_msgs::msg::PathPointWithLaneId;
using autoware_internal_planning_msgs::msg::PathWithLaneId;
using autoware_perception_msgs::msg::ObjectClassification;
using autoware_perception_msgs::msg::PredictedObjects;
using autoware_perception_msgs::msg::Shape;
using run_out_utils::DynamicObstacle;
using run_out_utils::DynamicObstacleData;
using run_out_utils::DynamicObstacleParam;
using run_out_utils::PlannerParam;
using run_out_utils::PredictedPath;
using PathPointsWithLaneId = std::vector<autoware_internal_planning_msgs::msg::PathPointWithLaneId>;

/**
 * @brief base class for creating dynamic obstacles from multiple types of input
 */
class DynamicObstacleCreator
{
public:
  using PointCloud2 = sensor_msgs::msg::PointCloud2;
  using ExactTimeSyncPolicy = message_filters::sync_policies::ExactTime<PointCloud2, PointCloud2>;
  using ExactTimeSynchronizer = message_filters::Synchronizer<ExactTimeSyncPolicy>;

  explicit DynamicObstacleCreator(
    rclcpp::Node & node, std::shared_ptr<RunOutDebug> & debug_ptr,
    const DynamicObstacleParam & param)
  : node_(node), debug_ptr_(debug_ptr), param_(param)
  {
  }
  virtual ~DynamicObstacleCreator() = default;
  virtual std::vector<DynamicObstacle> createDynamicObstacles() = 0;
  void setData(
    const PlannerData & planner_data, const PlannerParam & planner_param,
    const PathWithLaneId & path, const PathWithLaneId & smoothed_path)
  {
    std::lock_guard<std::mutex> lock(mutex_);

    dynamic_obstacle_data_.predicted_objects = *planner_data.predicted_objects;
    dynamic_obstacle_data_.path = path;

    // detection area is used only when detection target is Points
    if (planner_param.run_out.detection_method == "Points") {
      dynamic_obstacle_data_.detection_area =
        createDetectionAreaPolygon(smoothed_path, planner_data, planner_param);
      for (const auto & poly : dynamic_obstacle_data_.detection_area) {
        debug_ptr_->pushDetectionAreaPolygons(poly);
      }

      if (param_.use_mandatory_area) {
        dynamic_obstacle_data_.mandatory_detection_area =
          createMandatoryDetectionAreaPolygon(smoothed_path, planner_data, planner_param);
        for (const auto & poly : dynamic_obstacle_data_.mandatory_detection_area) {
          debug_ptr_->pushMandatoryDetectionAreaPolygons(poly);
        }
      }
    }
  }

protected:
  rclcpp::Node & node_;
  std::shared_ptr<RunOutDebug> debug_ptr_;
  DynamicObstacleParam param_;
  DynamicObstacleData dynamic_obstacle_data_;

  // mutex for dynamic_obstacle_data_
  std::mutex mutex_;
};

/**
 * @brief create dynamic obstacles from predicted objects
 */
class DynamicObstacleCreatorForObject : public DynamicObstacleCreator
{
public:
  explicit DynamicObstacleCreatorForObject(
    rclcpp::Node & node, std::shared_ptr<RunOutDebug> & debug_ptr,
    const DynamicObstacleParam & param);
  std::vector<DynamicObstacle> createDynamicObstacles() override;
};

/**
 * @brief create dynamic obstacles from predicted objects, but overwrite the path to be normal to
 *        the path of ego vehicle.
 */
class DynamicObstacleCreatorForObjectWithoutPath : public DynamicObstacleCreator
{
public:
  explicit DynamicObstacleCreatorForObjectWithoutPath(
    rclcpp::Node & node, std::shared_ptr<RunOutDebug> & debug_ptr,
    const DynamicObstacleParam & param);
  std::vector<DynamicObstacle> createDynamicObstacles() override;
};

/**
 * @brief create dynamic obstacles from points.
 *        predicted path is created to be normal to the path of ego vehicle.
 */
class DynamicObstacleCreatorForPoints : public DynamicObstacleCreator
{
public:
  explicit DynamicObstacleCreatorForPoints(
    rclcpp::Node & node, std::shared_ptr<RunOutDebug> & debug_ptr,
    const DynamicObstacleParam & param);
  std::vector<DynamicObstacle> createDynamicObstacles() override;

private:
  void onCompareMapFilteredPointCloud(const sensor_msgs::msg::PointCloud2::ConstSharedPtr msg);

  void onSynchronizedPointCloud(
    const PointCloud2::ConstSharedPtr compare_map_filtered_points,
    const PointCloud2::ConstSharedPtr vector_map_filtered_points);

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr
    sub_compare_map_filtered_pointcloud_;

  // synchronized subscribers
  message_filters::Subscriber<PointCloud2> sub_compare_map_filtered_pointcloud_sync_;
  message_filters::Subscriber<PointCloud2> sub_vector_map_inside_area_filtered_pointcloud_sync_;
  std::unique_ptr<ExactTimeSynchronizer> exact_time_synchronizer_;

  // tf
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  // obstacle points
  pcl::PointCloud<pcl::PointXYZ> obstacle_points_map_filtered_;
};

geometry_msgs::msg::Quaternion createQuaternionFacingToTrajectory(
  const PathPointsWithLaneId & path_points, const geometry_msgs::msg::Point & point);

std::vector<geometry_msgs::msg::Pose> createPredictedPath(
  const geometry_msgs::msg::Pose & initial_pose, const float time_step,
  const float max_velocity_mps, const float max_prediction_time);

pcl::PointCloud<pcl::PointXYZ> applyVoxelGridFilter(
  const pcl::PointCloud<pcl::PointXYZ> & input_points);

pcl::PointCloud<pcl::PointXYZ> applyVoxelGridFilter(
  const sensor_msgs::msg::PointCloud2 & input_points);

bool isAheadOf(
  const geometry_msgs::msg::Point & target_point, const geometry_msgs::msg::Pose & base_pose);

pcl::PointCloud<pcl::PointXYZ> extractObstaclePointsWithinPolygon(
  const pcl::PointCloud<pcl::PointXYZ> & input_points, const Polygons2d & polys);

std::vector<pcl::PointCloud<pcl::PointXYZ>> groupPointsWithNearestSegmentIndex(
  const pcl::PointCloud<pcl::PointXYZ> & input_points, const PathPointsWithLaneId & path_points);

pcl::PointXYZ calculateLateralNearestPoint(
  const pcl::PointCloud<pcl::PointXYZ> & input_points, const geometry_msgs::msg::Pose & base_pose);

pcl::PointCloud<pcl::PointXYZ> selectLateralNearestPoints(
  const std::vector<pcl::PointCloud<pcl::PointXYZ>> & points_with_index,
  const PathPointsWithLaneId & path_points);

pcl::PointCloud<pcl::PointXYZ> extractLateralNearestPoints(
  const pcl::PointCloud<pcl::PointXYZ> & input_points, const PathWithLaneId & path,
  const float interval);

std::optional<Eigen::Affine3f> getTransformMatrix(
  const tf2_ros::Buffer & tf_buffer, const std::string & target_frame_id,
  const std::string & source_frame_id, const builtin_interfaces::msg::Time & stamp);

pcl::PointCloud<pcl::PointXYZ> transformPointCloud(
  const PointCloud2 & input_pointcloud, const Eigen::Affine3f & transform_matrix);

PointCloud2 concatPointCloud(
  const pcl::PointCloud<pcl::PointXYZ> & cloud1, const pcl::PointCloud<pcl::PointXYZ> & cloud2);

void calculateMinAndMaxVelFromCovariance(
  const geometry_msgs::msg::TwistWithCovariance & twist_with_covariance,
  const double std_dev_multiplier, run_out_utils::DynamicObstacle & dynamic_obstacle);

double convertDurationToDouble(const builtin_interfaces::msg::Duration & duration);

std::vector<geometry_msgs::msg::Pose> createPathToPredictionTime(
  const autoware_perception_msgs::msg::PredictedPath & predicted_path, double prediction_time);

}  // namespace autoware::behavior_velocity_planner

#endif  // DYNAMIC_OBSTACLE_HPP_

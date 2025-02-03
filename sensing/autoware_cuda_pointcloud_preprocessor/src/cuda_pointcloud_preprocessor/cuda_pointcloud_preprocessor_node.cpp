// Copyright 2024 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "autoware/cuda_pointcloud_preprocessor/cuda_pointcloud_preprocessor_node.hpp"

#include "autoware/cuda_pointcloud_preprocessor/point_types.hpp"

#include <autoware/point_types/types.hpp>

#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <cuda_runtime.h>
#include <pcl/pcl_base.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <chrono>
#include <string>
#include <vector>

namespace autoware::cuda_pointcloud_preprocessor
{
using sensor_msgs::msg::PointCloud2;

CudaPointcloudPreprocessorNode::CudaPointcloudPreprocessorNode(
  const rclcpp::NodeOptions & node_options)
: Node("cuda_pointcloud_preprocessor", node_options),
  tf2_buffer_(this->get_clock()),
  tf2_listener_(tf2_buffer_)
{
  using std::placeholders::_1;

  // Parameters
  base_frame_ = declare_parameter<std::string>("base_frame");

  RingOutlierFilterParameters ring_outlier_filter_parameters;
  ring_outlier_filter_parameters.distance_ratio = declare_parameter<float>("distance_ratio");
  ring_outlier_filter_parameters.object_length_threshold =
    declare_parameter<float>("object_length_threshold");
  ring_outlier_filter_parameters.num_points_threshold =
    declare_parameter<int>("num_points_threshold");

  const auto crop_box_min_x_vector = declare_parameter<std::vector<double>>("crop_box.min_x");
  const auto crop_box_min_y_vector = declare_parameter<std::vector<double>>("crop_box.min_y");
  const auto crop_box_min_z_vector = declare_parameter<std::vector<double>>("crop_box.min_z");

  const auto crop_box_max_x_vector = declare_parameter<std::vector<double>>("crop_box.max_x");
  const auto crop_box_max_y_vector = declare_parameter<std::vector<double>>("crop_box.max_y");
  const auto crop_box_max_z_vector = declare_parameter<std::vector<double>>("crop_box.max_z");

  if (
    crop_box_min_x_vector.size() != crop_box_min_y_vector.size() ||
    crop_box_min_x_vector.size() != crop_box_min_z_vector.size() ||
    crop_box_min_x_vector.size() != crop_box_max_x_vector.size() ||
    crop_box_min_x_vector.size() != crop_box_max_y_vector.size() ||
    crop_box_min_x_vector.size() != crop_box_max_z_vector.size()) {
    throw std::runtime_error("Crop box parameters must have the same size");
  }

  std::vector<CropBoxParameters> crop_box_parameters;

  for (std::size_t i = 0; i < crop_box_min_x_vector.size(); i++) {
    CropBoxParameters parameters;
    parameters.min_x = crop_box_min_x_vector[i];
    parameters.min_y = crop_box_min_y_vector[i];
    parameters.min_z = crop_box_min_z_vector[i];
    parameters.max_x = crop_box_max_x_vector[i];
    parameters.max_y = crop_box_max_y_vector[i];
    parameters.max_z = crop_box_max_z_vector[i];
    crop_box_parameters.push_back(parameters);
  }

  bool use_3d_undistortion = declare_parameter<bool>("use_3d_distortion_correction");
  bool use_imu = declare_parameter<bool>("use_imu");

  // Subscriber
  sub_ =
    std::make_unique<cuda_blackboard::CudaBlackboardSubscriber<cuda_blackboard::CudaPointCloud2>>(
      *this, "~/input/pointcloud", false,
      std::bind(&CudaPointcloudPreprocessorNode::pointcloudCallback, this, _1));

  // Publisher
  pub_ =
    std::make_unique<cuda_blackboard::CudaBlackboardPublisher<cuda_blackboard::CudaPointCloud2>>(
      *this, "~/output/pointcloud");
  twist_sub_ = this->create_subscription<geometry_msgs::msg::TwistWithCovarianceStamped>(
    "~/input/twist", 10,
    std::bind(&CudaPointcloudPreprocessorNode::twistCallback, this, std::placeholders::_1));

  if (use_imu) {
    imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
      "~/input/imu", 10,
      std::bind(&CudaPointcloudPreprocessorNode::imuCallback, this, std::placeholders::_1));
  }

  cuda_pointcloud_preprocessor_ = std::make_unique<CudaPointcloudPreprocessor>();
  cuda_pointcloud_preprocessor_->setRingOutlierFilterParameters(ring_outlier_filter_parameters);
  cuda_pointcloud_preprocessor_->setCropBoxParameters(crop_box_parameters);
  cuda_pointcloud_preprocessor_->set3DUndistortion(use_3d_undistortion);

  // initialize debug tool
  {
    using autoware::universe_utils::DebugPublisher;
    using autoware::universe_utils::StopWatch;

    stop_watch_ptr_ = std::make_unique<StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ = std::make_unique<DebugPublisher>(this, "cuda_pointcloud_preprocessor");
    stop_watch_ptr_->tic("processing_time");
  }
}

bool CudaPointcloudPreprocessorNode::getTransform(
  const std::string & target_frame, const std::string & source_frame,
  tf2::Transform * tf2_transform_ptr)
{
  if (target_frame == source_frame) {
    tf2_transform_ptr->setOrigin(tf2::Vector3(0.0, 0.0, 0.0));
    tf2_transform_ptr->setRotation(tf2::Quaternion(0.0, 0.0, 0.0, 1.0));
    return true;
  }

  try {
    const auto transform_msg =
      tf2_buffer_.lookupTransform(target_frame, source_frame, tf2::TimePointZero);
    tf2::convert(transform_msg.transform, *tf2_transform_ptr);
  } catch (const tf2::TransformException & ex) {
    RCLCPP_WARN(get_logger(), "%s", ex.what());
    RCLCPP_ERROR(
      get_logger(), "Please publish TF %s to %s", target_frame.c_str(), source_frame.c_str());

    tf2_transform_ptr->setOrigin(tf2::Vector3(0.0, 0.0, 0.0));
    tf2_transform_ptr->setRotation(tf2::Quaternion(0.0, 0.0, 0.0, 1.0));
    return false;
  }
  return true;
}

void CudaPointcloudPreprocessorNode::twistCallback(
  const geometry_msgs::msg::TwistWithCovarianceStamped::ConstSharedPtr twist_msg_ptr)
{
  twist_queue_.push_back(*twist_msg_ptr);

  while (!twist_queue_.empty()) {
    // for replay rosbag
    if (
      rclcpp::Time(twist_queue_.front().header.stamp) > rclcpp::Time(twist_msg_ptr->header.stamp)) {
      twist_queue_.pop_front();
    } else if (  // NOLINT
      rclcpp::Time(twist_queue_.front().header.stamp) <
      rclcpp::Time(twist_msg_ptr->header.stamp) - rclcpp::Duration::from_seconds(1.0)) {
      twist_queue_.pop_front();
    }
    break;
  }
}

void CudaPointcloudPreprocessorNode::imuCallback(
  const sensor_msgs::msg::Imu::ConstSharedPtr imu_msg)
{
  tf2::Transform tf2_imu_link_to_base_link{};
  getTransform(base_frame_, imu_msg->header.frame_id, &tf2_imu_link_to_base_link);
  geometry_msgs::msg::TransformStamped::SharedPtr tf_base2imu_ptr =
    std::make_shared<geometry_msgs::msg::TransformStamped>();
  tf_base2imu_ptr->transform.rotation = tf2::toMsg(tf2_imu_link_to_base_link.getRotation());

  geometry_msgs::msg::Vector3Stamped angular_velocity;
  angular_velocity.vector = imu_msg->angular_velocity;

  geometry_msgs::msg::Vector3Stamped transformed_angular_velocity;
  tf2::doTransform(angular_velocity, transformed_angular_velocity, *tf_base2imu_ptr);
  transformed_angular_velocity.header = imu_msg->header;
  angular_velocity_queue_.push_back(transformed_angular_velocity);

  while (!angular_velocity_queue_.empty()) {
    // for rosbag replay
    if (
      rclcpp::Time(angular_velocity_queue_.front().header.stamp) >
      rclcpp::Time(imu_msg->header.stamp)) {
      angular_velocity_queue_.pop_front();
    } else if (  // NOLINT
      rclcpp::Time(angular_velocity_queue_.front().header.stamp) <
      rclcpp::Time(imu_msg->header.stamp) - rclcpp::Duration::from_seconds(1.0)) {
      angular_velocity_queue_.pop_front();
    }
    break;
  }
}

void CudaPointcloudPreprocessorNode::pointcloudCallback(
  const std::shared_ptr<const cuda_blackboard::CudaPointCloud2> input_pointcloud_msg_ptr)
{
  static_assert(
    sizeof(InputPointType) == sizeof(autoware::point_types::PointXYZIRCAEDT),
    "PointStruct and PointXYZIRCAEDT must have the same size");

  stop_watch_ptr_->toc("processing_time", true);

  // Make sure that the first twist is newer than the first point
  InputPointType first_point;
  cudaMemcpy(
    &first_point, input_pointcloud_msg_ptr->data.get(), sizeof(InputPointType),
    cudaMemcpyDeviceToHost);
  double first_point_stamp = input_pointcloud_msg_ptr->header.stamp.sec +
                             input_pointcloud_msg_ptr->header.stamp.nanosec * 1e-9 +
                             first_point.time_stamp * 1e-9;

  while (twist_queue_.size() > 1 &&
         rclcpp::Time(twist_queue_.front().header.stamp).seconds() < first_point_stamp) {
    twist_queue_.pop_front();
  }

  while (angular_velocity_queue_.size() > 1 &&
         rclcpp::Time(angular_velocity_queue_.front().header.stamp).seconds() < first_point_stamp) {
    angular_velocity_queue_.pop_front();
  }

  // Obtain the base link to input pointcloud transform
  geometry_msgs::msg::TransformStamped transform_msg;

  try {
    transform_msg = tf2_buffer_.lookupTransform(
      base_frame_, input_pointcloud_msg_ptr->header.frame_id, tf2::TimePointZero);
  } catch (const tf2::TransformException & ex) {
    RCLCPP_WARN(get_logger(), "%s", ex.what());
    return;
  }

  auto output_pointcloud_ptr = cuda_pointcloud_preprocessor_->process(
    *input_pointcloud_msg_ptr, transform_msg, twist_queue_, angular_velocity_queue_);
  output_pointcloud_ptr->header.frame_id = base_frame_;

  // Publish
  pub_->publish(std::move(output_pointcloud_ptr));

  if (debug_publisher_) {
    const double processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);

    double now_stamp_seconds = rclcpp::Time(this->get_clock()->now()).seconds();
    double cloud_stamp_seconds = rclcpp::Time(input_pointcloud_msg_ptr->header.stamp).seconds();

    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/latency_ms", 1000.f * (now_stamp_seconds - cloud_stamp_seconds));
  }

  // Preallocate for next iteration
  cuda_pointcloud_preprocessor_->preallocateOutput();
}

}  // namespace autoware::cuda_pointcloud_preprocessor

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(
  autoware::cuda_pointcloud_preprocessor::CudaPointcloudPreprocessorNode)

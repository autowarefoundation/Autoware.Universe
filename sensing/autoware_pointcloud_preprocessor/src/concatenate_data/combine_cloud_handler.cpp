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

#include "autoware/pointcloud_preprocessor/concatenate_data/combine_cloud_handler.hpp"

#ifdef USE_CUDA
#include "autoware/pointcloud_preprocessor/concatenate_data/combine_cloud_handler_kernel.hpp"
#endif

#include <pcl_ros/transforms.hpp>

#include <pcl_conversions/pcl_conversions.h>

#ifdef USE_CUDA
#include <cuda_blackboard/cuda_pointcloud2.hpp>

#include <cuda_runtime.h>
#endif

#include <algorithm>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef USE_CUDA

#define CHECK_OFFSET(structure1, structure2, field)             \
  static_assert(                                                \
    offsetof(structure1, field) == offsetof(structure2, field), \
    "Offset of " #field " in " #structure1 " does not match expected offset.")

static_assert(
  sizeof(autoware::pointcloud_preprocessor::PointTypeStruct) ==
  sizeof(autoware::point_types::PointXYZIRC));

CHECK_OFFSET(
  autoware::pointcloud_preprocessor::PointTypeStruct, autoware::point_types::PointXYZIRCAEDT, x);
CHECK_OFFSET(
  autoware::pointcloud_preprocessor::PointTypeStruct, autoware::point_types::PointXYZIRCAEDT, y);
CHECK_OFFSET(
  autoware::pointcloud_preprocessor::PointTypeStruct, autoware::point_types::PointXYZIRCAEDT, z);
CHECK_OFFSET(
  autoware::pointcloud_preprocessor::PointTypeStruct, autoware::point_types::PointXYZIRCAEDT,
  intensity);
CHECK_OFFSET(
  autoware::pointcloud_preprocessor::PointTypeStruct, autoware::point_types::PointXYZIRCAEDT,
  return_type);
CHECK_OFFSET(
  autoware::pointcloud_preprocessor::PointTypeStruct, autoware::point_types::PointXYZIRCAEDT,
  channel);

#endif

namespace autoware::pointcloud_preprocessor
{

CombineCloudHandlerBase::CombineCloudHandlerBase(
  rclcpp::Node & node, std::string output_frame, bool is_motion_compensated,
  bool publish_synchronized_pointcloud, bool keep_input_frame_in_synchronized_pointcloud,
  bool has_static_tf_only)
: node_(node),
  output_frame_(output_frame),
  is_motion_compensated_(is_motion_compensated),
  publish_synchronized_pointcloud_(publish_synchronized_pointcloud),
  keep_input_frame_in_synchronized_pointcloud_(keep_input_frame_in_synchronized_pointcloud),
  managed_tf_buffer_(
    std::make_unique<autoware::universe_utils::ManagedTransformBuffer>(&node_, has_static_tf_only))
{
}

void CombineCloudHandlerBase::process_twist(
  const geometry_msgs::msg::TwistWithCovarianceStamped::ConstSharedPtr & twist_msg)
{
  geometry_msgs::msg::TwistStamped msg;
  msg.header = twist_msg->header;
  msg.twist = twist_msg->twist.twist;

  // If time jumps backwards (e.g. when a rosbag restarts), clear buffer
  if (!twist_queue_.empty()) {
    if (rclcpp::Time(twist_queue_.front().header.stamp) > rclcpp::Time(msg.header.stamp)) {
      twist_queue_.clear();
    }
  }

  // Twist data in the queue that is older than the current twist by 1 second will be cleared.
  auto cutoff_time = rclcpp::Time(msg.header.stamp) - rclcpp::Duration::from_seconds(1.0);

  while (!twist_queue_.empty()) {
    if (rclcpp::Time(twist_queue_.front().header.stamp) > cutoff_time) {
      break;
    }
    twist_queue_.pop_front();
  }

  twist_queue_.push_back(msg);
}

void CombineCloudHandlerBase::process_odometry(
  const nav_msgs::msg::Odometry::ConstSharedPtr & odometry_msg)
{
  geometry_msgs::msg::TwistStamped msg;
  msg.header = odometry_msg->header;
  msg.twist = odometry_msg->twist.twist;

  // If time jumps backwards (e.g. when a rosbag restarts), clear buffer
  if (!twist_queue_.empty()) {
    if (rclcpp::Time(twist_queue_.front().header.stamp) > rclcpp::Time(msg.header.stamp)) {
      twist_queue_.clear();
    }
  }

  // Twist data in the queue that is older than the current twist by 1 second will be cleared.
  auto cutoff_time = rclcpp::Time(msg.header.stamp) - rclcpp::Duration::from_seconds(1.0);

  while (!twist_queue_.empty()) {
    if (rclcpp::Time(twist_queue_.front().header.stamp) > cutoff_time) {
      break;
    }
    twist_queue_.pop_front();
  }

  twist_queue_.push_back(msg);
}

std::deque<geometry_msgs::msg::TwistStamped> CombineCloudHandlerBase::get_twist_queue()
{
  return twist_queue_;
}

Eigen::Matrix4f CombineCloudHandlerBase::compute_transform_to_adjust_for_old_timestamp(
  const rclcpp::Time & old_stamp, const rclcpp::Time & new_stamp)
{
  // return identity if no twist is available
  if (twist_queue_.empty()) {
    RCLCPP_WARN_STREAM_THROTTLE(
      node_.get_logger(), *node_.get_clock(), std::chrono::milliseconds(10000).count(),
      "No twist is available. Please confirm twist topic and timestamp. Leaving point cloud "
      "untransformed.");
    return Eigen::Matrix4f::Identity();
  }

  auto old_twist_it = std::lower_bound(
    std::begin(twist_queue_), std::end(twist_queue_), old_stamp,
    [](const geometry_msgs::msg::TwistStamped & x, const rclcpp::Time & t) {
      return rclcpp::Time(x.header.stamp) < t;
    });
  old_twist_it = old_twist_it == twist_queue_.end() ? (twist_queue_.end() - 1) : old_twist_it;

  auto new_twist_it = std::lower_bound(
    std::begin(twist_queue_), std::end(twist_queue_), new_stamp,
    [](const geometry_msgs::msg::TwistStamped & x, const rclcpp::Time & t) {
      return rclcpp::Time(x.header.stamp) < t;
    });
  new_twist_it = new_twist_it == twist_queue_.end() ? (twist_queue_.end() - 1) : new_twist_it;

  auto prev_time = old_stamp;
  double x = 0.0;
  double y = 0.0;
  double yaw = 0.0;
  tf2::Quaternion baselink_quat{};
  for (auto twist_it = old_twist_it; twist_it != new_twist_it + 1; ++twist_it) {
    const double dt =
      (twist_it != new_twist_it)
        ? (rclcpp::Time((*twist_it).header.stamp) - rclcpp::Time(prev_time)).seconds()
        : (rclcpp::Time(new_stamp) - rclcpp::Time(prev_time)).seconds();

    if (std::fabs(dt) > 0.1) {
      RCLCPP_WARN_STREAM_THROTTLE(
        node_.get_logger(), *node_.get_clock(), std::chrono::milliseconds(10000).count(),
        "Time difference is too large. Cloud not interpolate. Please confirm twist topic and "
        "timestamp");
      break;
    }

    const double distance = (*twist_it).twist.linear.x * dt;
    yaw += (*twist_it).twist.angular.z * dt;
    x += distance * std::cos(yaw);
    y += distance * std::sin(yaw);
    prev_time = (*twist_it).header.stamp;
  }

  Eigen::Matrix4f transformation_matrix = Eigen::Matrix4f::Identity();

  float cos_yaw = std::cos(yaw);
  float sin_yaw = std::sin(yaw);

  transformation_matrix(0, 3) = x;
  transformation_matrix(1, 3) = y;
  transformation_matrix(0, 0) = cos_yaw;
  transformation_matrix(0, 1) = -sin_yaw;
  transformation_matrix(1, 0) = sin_yaw;
  transformation_matrix(1, 1) = cos_yaw;

  return transformation_matrix;
}

CombineCloudHandler<sensor_msgs::msg::PointCloud2>::CombineCloudHandler(
  rclcpp::Node & node, std::string output_frame, bool is_motion_compensated,
  bool publish_synchronized_pointcloud, bool keep_input_frame_in_synchronized_pointcloud,
  bool has_static_tf_only)
: CombineCloudHandlerBase(
    node, output_frame, is_motion_compensated, publish_synchronized_pointcloud,
    keep_input_frame_in_synchronized_pointcloud, has_static_tf_only)
{
}

void CombineCloudHandler<sensor_msgs::msg::PointCloud2>::convert_to_xyzirc_cloud(
  const typename sensor_msgs::msg::PointCloud2::ConstSharedPtr & input_cloud,
  typename sensor_msgs::msg::PointCloud2::SharedPtr & xyzirc_cloud)
{
  xyzirc_cloud->header = input_cloud->header;

  PointCloud2Modifier<PointXYZIRC, autoware::point_types::PointXYZIRCGenerator> output_modifier{
    *xyzirc_cloud, input_cloud->header.frame_id};
  output_modifier.reserve(input_cloud->width);

  bool has_valid_intensity =
    std::any_of(input_cloud->fields.begin(), input_cloud->fields.end(), [](const auto & field) {
      return field.name == "intensity" && field.datatype == sensor_msgs::msg::PointField::UINT8;
    });

  bool has_valid_return_type =
    std::any_of(input_cloud->fields.begin(), input_cloud->fields.end(), [](const auto & field) {
      return field.name == "return_type" && field.datatype == sensor_msgs::msg::PointField::UINT8;
    });

  bool has_valid_channel =
    std::any_of(input_cloud->fields.begin(), input_cloud->fields.end(), [](const auto & field) {
      return field.name == "channel" && field.datatype == sensor_msgs::msg::PointField::UINT16;
    });

  sensor_msgs::PointCloud2ConstIterator<float> it_x(*input_cloud, "x");
  sensor_msgs::PointCloud2ConstIterator<float> it_y(*input_cloud, "y");
  sensor_msgs::PointCloud2ConstIterator<float> it_z(*input_cloud, "z");

  if (has_valid_intensity && has_valid_return_type && has_valid_channel) {
    sensor_msgs::PointCloud2ConstIterator<std::uint8_t> it_i(*input_cloud, "intensity");
    sensor_msgs::PointCloud2ConstIterator<std::uint8_t> it_r(*input_cloud, "return_type");
    sensor_msgs::PointCloud2ConstIterator<std::uint16_t> it_c(*input_cloud, "channel");

    for (; it_x != it_x.end(); ++it_x, ++it_y, ++it_z, ++it_i, ++it_r, ++it_c) {
      PointXYZIRC point;
      point.x = *it_x;
      point.y = *it_y;
      point.z = *it_z;
      point.intensity = *it_i;
      point.return_type = *it_r;
      point.channel = *it_c;
      output_modifier.push_back(std::move(point));
    }
  } else {
    for (; it_x != it_x.end(); ++it_x, ++it_y, ++it_z) {
      PointXYZIRC point;
      point.x = *it_x;
      point.y = *it_y;
      point.z = *it_z;
      output_modifier.push_back(std::move(point));
    }
  }
}

void CombineCloudHandler<sensor_msgs::msg::PointCloud2>::correct_pointcloud_motion(
  const std::shared_ptr<sensor_msgs::msg::PointCloud2> & transformed_cloud_ptr,
  const std::vector<rclcpp::Time> & pc_stamps,
  std::unordered_map<rclcpp::Time, Eigen::Matrix4f, RclcppTimeHash> & transform_memo,
  std::shared_ptr<sensor_msgs::msg::PointCloud2> transformed_delay_compensated_cloud_ptr)
{
  Eigen::Matrix4f adjust_to_old_data_transform = Eigen::Matrix4f::Identity();
  rclcpp::Time current_cloud_stamp = rclcpp::Time(transformed_cloud_ptr->header.stamp);
  for (const auto & stamp : pc_stamps) {
    if (stamp >= current_cloud_stamp) continue;

    Eigen::Matrix4f new_to_old_transform;
    if (transform_memo.find(stamp) != transform_memo.end()) {
      new_to_old_transform = transform_memo[stamp];
    } else {
      new_to_old_transform =
        compute_transform_to_adjust_for_old_timestamp(stamp, current_cloud_stamp);
      transform_memo[stamp] = new_to_old_transform;
    }
    adjust_to_old_data_transform = new_to_old_transform * adjust_to_old_data_transform;
    current_cloud_stamp = stamp;
  }
  pcl_ros::transformPointCloud(
    adjust_to_old_data_transform, *transformed_cloud_ptr, *transformed_delay_compensated_cloud_ptr);
}

/* template<> */
ConcatenatedCloudResult<sensor_msgs::msg::PointCloud2>
CombineCloudHandler<sensor_msgs::msg::PointCloud2>::combine_pointclouds(
  std::unordered_map<std::string, sensor_msgs::msg::PointCloud2::ConstSharedPtr> &
    topic_to_cloud_map)
{
  ConcatenatedCloudResult<sensor_msgs::msg::PointCloud2> concatenate_cloud_result;

  std::vector<rclcpp::Time> pc_stamps;
  for (const auto & [topic, cloud] : topic_to_cloud_map) {
    pc_stamps.emplace_back(cloud->header.stamp);
  }
  std::sort(pc_stamps.begin(), pc_stamps.end(), std::greater<rclcpp::Time>());
  const auto oldest_stamp = pc_stamps.back();

  std::unordered_map<rclcpp::Time, Eigen::Matrix4f, RclcppTimeHash> transform_memo;

  // Before combining the pointclouds, initialize and reserve space for the concatenated pointcloud
  concatenate_cloud_result.concatenate_cloud_ptr =
    std::make_shared<sensor_msgs::msg::PointCloud2>();

  // Reserve space based on the total size of the pointcloud data to speed up the concatenation
  // process
  size_t total_data_size = 0;
  for (const auto & [topic, cloud] : topic_to_cloud_map) {
    total_data_size += cloud->data.size();
  }
  concatenate_cloud_result.concatenate_cloud_ptr->data.reserve(total_data_size);

  for (const auto & [topic, cloud] : topic_to_cloud_map) {
    // convert to XYZIRC pointcloud if pointcloud is not empty
    auto xyzirc_cloud = std::make_shared<sensor_msgs::msg::PointCloud2>();
    convert_to_xyzirc_cloud(cloud, xyzirc_cloud);

    auto transformed_cloud_ptr = std::make_shared<sensor_msgs::msg::PointCloud2>();
    managed_tf_buffer_->transformPointcloud(output_frame_, *xyzirc_cloud, *transformed_cloud_ptr);

    concatenate_cloud_result.topic_to_original_stamp_map[topic] =
      rclcpp::Time(cloud->header.stamp).seconds();

    // compensate pointcloud
    std::shared_ptr<sensor_msgs::msg::PointCloud2> transformed_delay_compensated_cloud_ptr;
    if (is_motion_compensated_) {
      transformed_delay_compensated_cloud_ptr = std::make_shared<sensor_msgs::msg::PointCloud2>();
      correct_pointcloud_motion(
        transformed_cloud_ptr, pc_stamps, transform_memo, transformed_delay_compensated_cloud_ptr);
    } else {
      transformed_delay_compensated_cloud_ptr = transformed_cloud_ptr;
    }

    pcl::concatenatePointCloud(
      *concatenate_cloud_result.concatenate_cloud_ptr, *transformed_delay_compensated_cloud_ptr,
      *concatenate_cloud_result.concatenate_cloud_ptr);

    if (publish_synchronized_pointcloud_) {
      if (!concatenate_cloud_result.topic_to_transformed_cloud_map) {
        // Initialize the map if it is not present
        concatenate_cloud_result.topic_to_transformed_cloud_map =
          std::unordered_map<std::string, typename sensor_msgs::msg::PointCloud2::SharedPtr>();
      }
      // convert to original sensor frame if necessary
      bool need_transform_to_sensor_frame = (cloud->header.frame_id != output_frame_);
      if (keep_input_frame_in_synchronized_pointcloud_ && need_transform_to_sensor_frame) {
        auto transformed_cloud_ptr_in_sensor_frame =
          std::make_shared<sensor_msgs::msg::PointCloud2>();
        managed_tf_buffer_->transformPointcloud(
          cloud->header.frame_id, *transformed_delay_compensated_cloud_ptr,
          *transformed_cloud_ptr_in_sensor_frame);
        transformed_cloud_ptr_in_sensor_frame->header.stamp = oldest_stamp;
        transformed_cloud_ptr_in_sensor_frame->header.frame_id = cloud->header.frame_id;

        (*concatenate_cloud_result.topic_to_transformed_cloud_map)[topic] =
          transformed_cloud_ptr_in_sensor_frame;
      } else {
        transformed_delay_compensated_cloud_ptr->header.stamp = oldest_stamp;
        transformed_delay_compensated_cloud_ptr->header.frame_id = output_frame_;
        (*concatenate_cloud_result.topic_to_transformed_cloud_map)[topic] =
          transformed_delay_compensated_cloud_ptr;
      }
    }
  }
  concatenate_cloud_result.concatenate_cloud_ptr->header.stamp = oldest_stamp;

  return concatenate_cloud_result;
}

#ifdef USE_CUDA

CombineCloudHandler<cuda_blackboard::CudaPointCloud2>::CombineCloudHandler(
  rclcpp::Node & node, const std::vector<std::string> & input_topics, std::string output_frame,
  bool is_motion_compensated, bool publish_synchronized_pointcloud,
  bool keep_input_frame_in_synchronized_pointcloud, bool has_static_tf_only)
: CombineCloudHandlerBase(
    node, output_frame, is_motion_compensated, publish_synchronized_pointcloud,
    keep_input_frame_in_synchronized_pointcloud, has_static_tf_only),
  input_topics_(input_topics)
{
  for (const auto & topic : input_topics_) {
    CudaConcatStruct cuda_concat_struct;
    cudaStreamCreate(&cuda_concat_struct.stream);
    cuda_concat_struct_map_[topic] = std::move(cuda_concat_struct);
  }
}

void CombineCloudHandler<cuda_blackboard::CudaPointCloud2>::allocate_pointclouds()
{
  std::lock_guard<std::mutex> lock(mutex_);

  for (const auto & topic : input_topics_) {
    auto & concat_struct = cuda_concat_struct_map_[topic];
    concat_struct.cloud_ptr = std::make_unique<cuda_blackboard::CudaPointCloud2>();
    concat_struct.cloud_ptr->data =
      cuda_blackboard::make_unique<std::uint8_t[]>(concat_struct.max_pointcloud_size_);
  }

  concatenated_cloud_ptr = std::make_unique<cuda_blackboard::CudaPointCloud2>();
  concatenated_cloud_ptr->data = cuda_blackboard::make_unique<std::uint8_t[]>(
    max_concat_pointcloud_size_ * input_topics_.size());
}

ConcatenatedCloudResult<cuda_blackboard::CudaPointCloud2>
CombineCloudHandler<cuda_blackboard::CudaPointCloud2>::combine_pointclouds(
  std::unordered_map<std::string, cuda_blackboard::CudaPointCloud2::ConstSharedPtr> &
    topic_to_cloud_map)
{
  ConcatenatedCloudResult<cuda_blackboard::CudaPointCloud2> concatenate_cloud_result;
  std::lock_guard<std::mutex> lock(mutex_);

  std::vector<rclcpp::Time> pc_stamps;
  for (const auto & [topic, cloud] : topic_to_cloud_map) {
    pc_stamps.emplace_back(cloud->header.stamp);
  }
  std::sort(pc_stamps.begin(), pc_stamps.end(), std::greater<rclcpp::Time>());
  auto oldest_stamp = pc_stamps.back();

  // Before combining the pointclouds, initialize and reserve space for the concatenated pointcloud
  concatenate_cloud_result.concatenate_cloud_ptr =
    std::make_unique<cuda_blackboard::CudaPointCloud2>();

  // Reserve space based on the total size of the pointcloud data to speed up the concatenation
  // process
  size_t total_data_size = 0;
  size_t total_points = 0;
  for (const auto & [topic, cloud] : topic_to_cloud_map) {
    total_data_size += (cloud->height * cloud->row_step);
    total_points += (cloud->height * cloud->width);
  }

  const auto point_fields = topic_to_cloud_map.begin()->second->fields;

  if (total_data_size > max_concat_pointcloud_size_ || !concatenated_cloud_ptr) {
    max_concat_pointcloud_size_ = (total_data_size + 1024) / 1024 * 1024;
    concatenated_cloud_ptr = std::make_unique<cuda_blackboard::CudaPointCloud2>();
    concatenated_cloud_ptr->data = cuda_blackboard::make_unique<std::uint8_t[]>(
      max_concat_pointcloud_size_ * input_topics_.size());
  }

  concatenate_cloud_result.concatenate_cloud_ptr = std::move(concatenated_cloud_ptr);

  PointTypeStruct * output_points =
    reinterpret_cast<PointTypeStruct *>(concatenate_cloud_result.concatenate_cloud_ptr->data.get());
  std::size_t concatenated_start_index = 0;

  for (const auto & [topic, cloud] : topic_to_cloud_map) {
    const std::size_t num_points = cloud->height * cloud->width;

    // Compute motion compensation transform
    Eigen::Matrix4f transform = Eigen::Matrix4f::Identity();

    // Transform if needed
    managed_tf_buffer_->getTransform(output_frame_, cloud->header.frame_id, transform);

    rclcpp::Time current_cloud_stamp = rclcpp::Time(cloud->header.stamp);

    if (is_motion_compensated_) {
      transform = compute_transform_to_adjust_for_old_timestamp(oldest_stamp, current_cloud_stamp) *
                  transform;
    }

    TransformStruct transform_struct;
    transform_struct.translation_x = transform(0, 3);
    transform_struct.translation_y = transform(1, 3);
    transform_struct.translation_z = transform(2, 3);
    transform_struct.m11 = transform(0, 0);
    transform_struct.m12 = transform(0, 1);
    transform_struct.m13 = transform(0, 2);
    transform_struct.m21 = transform(1, 0);
    transform_struct.m22 = transform(1, 1);
    transform_struct.m23 = transform(1, 2);
    transform_struct.m31 = transform(2, 0);
    transform_struct.m32 = transform(2, 1);
    transform_struct.m33 = transform(2, 2);

    auto & stream = cuda_concat_struct_map_[topic].stream;

    cudaMemset(output_points + concatenated_start_index, 222, num_points);

    // Apply the kernel to the pointclouds
    transform_launch(
      reinterpret_cast<PointTypeStruct *>(cloud->data.get()), num_points, transform_struct,
      output_points + concatenated_start_index, stream);
    concatenated_start_index += num_points;
  }

  concatenate_cloud_result.concatenate_cloud_ptr->header.frame_id = output_frame_;
  concatenate_cloud_result.concatenate_cloud_ptr->width = concatenated_start_index;
  concatenate_cloud_result.concatenate_cloud_ptr->height = 1;
  concatenate_cloud_result.concatenate_cloud_ptr->point_step = sizeof(PointTypeStruct);
  concatenate_cloud_result.concatenate_cloud_ptr->row_step =
    concatenated_start_index * sizeof(PointTypeStruct);
  concatenate_cloud_result.concatenate_cloud_ptr->fields = point_fields;
  concatenate_cloud_result.concatenate_cloud_ptr->is_bigendian = false;
  concatenate_cloud_result.concatenate_cloud_ptr->is_dense = true;

  // Second round is for when we need to publish sync pointclouds
  if (publish_synchronized_pointcloud_) {
    if (!concatenate_cloud_result.topic_to_transformed_cloud_map) {
      // Initialize the map if it is not present
      concatenate_cloud_result.topic_to_transformed_cloud_map =
        std::unordered_map<std::string, cuda_blackboard::CudaPointCloud2::UniquePtr>();
    }

    concatenated_start_index = 0;

    for (const auto & [topic, cloud] : topic_to_cloud_map) {
      const std::size_t num_points = cloud->height * cloud->width;
      const std::size_t data_size = cloud->height * cloud->row_step;

      auto & concat_struct = cuda_concat_struct_map_[topic];

      if (data_size > concat_struct.max_pointcloud_size_ || !concat_struct.cloud_ptr) {
        concat_struct.max_pointcloud_size_ = (data_size + 1024) / 1024 * 1024;
        concat_struct.cloud_ptr = std::make_unique<cuda_blackboard::CudaPointCloud2>();
        concat_struct.cloud_ptr->data = cuda_blackboard::make_unique<std::uint8_t[]>(data_size);
      }
      // convert to original sensor frame if necessary

      auto & output_cloud = (*concatenate_cloud_result.topic_to_transformed_cloud_map)[topic];
      bool need_transform_to_sensor_frame = (cloud->header.frame_id != output_frame_);

      output_cloud = std::move(concat_struct.cloud_ptr);

      auto & stream = cuda_concat_struct_map_[topic].stream;

      if (keep_input_frame_in_synchronized_pointcloud_ && need_transform_to_sensor_frame) {
        Eigen::Matrix4f transform;
        managed_tf_buffer_->getTransform(cloud->header.frame_id, output_frame_, transform);

        TransformStruct transform_struct;
        transform_struct.translation_x = transform(0, 3);
        transform_struct.translation_y = transform(1, 3);
        transform_struct.translation_z = transform(2, 3);
        transform_struct.m11 = transform(0, 0);
        transform_struct.m12 = transform(0, 1);
        transform_struct.m13 = transform(0, 2);
        transform_struct.m21 = transform(1, 0);
        transform_struct.m22 = transform(1, 1);
        transform_struct.m23 = transform(1, 2);
        transform_struct.m31 = transform(2, 0);
        transform_struct.m32 = transform(2, 1);
        transform_struct.m33 = transform(2, 2);

        transform_launch(
          output_points + concatenated_start_index, num_points, transform_struct,
          reinterpret_cast<PointTypeStruct *>(output_cloud->data.get()), stream);
        output_cloud->header.frame_id = cloud->header.frame_id;
      } else {
        cudaMemcpyAsync(
          output_cloud->data.get(), output_points + concatenated_start_index, data_size,
          cudaMemcpyDeviceToDevice, stream);
        output_cloud->header.frame_id = output_frame_;
      }

      output_cloud->header.stamp = cloud->header.stamp;
      output_cloud->width = cloud->width;
      output_cloud->height = cloud->height;
      output_cloud->point_step = sizeof(PointTypeStruct);
      output_cloud->row_step = cloud->width * sizeof(PointTypeStruct);
      output_cloud->fields = point_fields;
      output_cloud->is_bigendian = false;
      output_cloud->is_dense = true;

      concatenated_start_index += cloud->height * cloud->width;
    }
  }

  // Sync all streams
  for (const auto & [topic, cuda_concat_struct] : cuda_concat_struct_map_) {
    cudaStreamSynchronize(cuda_concat_struct.stream);
  }

  concatenate_cloud_result.concatenate_cloud_ptr->header.stamp = oldest_stamp;

  return concatenate_cloud_result;
}
#endif

}  // namespace autoware::pointcloud_preprocessor

template class autoware::pointcloud_preprocessor::CombineCloudHandler<
  sensor_msgs::msg::PointCloud2>;

#ifdef USE_CUDA
template class autoware::pointcloud_preprocessor::CombineCloudHandler<
  cuda_blackboard::CudaPointCloud2>;
#endif

// Copyright 2023 TIER IV, Inc.
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

#include "autoware/image_projection_based_fusion/segmentation_pointcloud_fusion/node.hpp"

#include "autoware/image_projection_based_fusion/utils/geometry.hpp"
#include "autoware/image_projection_based_fusion/utils/utils.hpp"

#include <autoware/universe_utils/system/time_keeper.hpp>
#include <perception_utils/run_length_encoder.hpp>

#include <memory>
#include <vector>

#ifdef ROS_DISTRO_GALACTIC
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2_sensor_msgs/tf2_sensor_msgs.h>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tf2_sensor_msgs/tf2_sensor_msgs.hpp>
#endif

namespace autoware::image_projection_based_fusion
{
using autoware::universe_utils::ScopedTimeTrack;

SegmentPointCloudFusionNode::SegmentPointCloudFusionNode(const rclcpp::NodeOptions & options)
: FusionNode<PointCloudMsgType, Image, PointCloudMsgType>("segmentation_pointcloud_fusion", options)
{
  filter_distance_threshold_ = declare_parameter<float>("filter_distance_threshold");
  for (auto & item : filter_semantic_label_target_list_) {
    item.second = declare_parameter<bool>("filter_semantic_label_target." + item.first);
  }
  for (const auto & item : filter_semantic_label_target_list_) {
    RCLCPP_INFO(
      this->get_logger(), "filter_semantic_label_target: %s %d", item.first.c_str(), item.second);
  }
  is_publish_debug_mask_ = declare_parameter<bool>("is_publish_debug_mask");
  pub_debug_mask_ptr_ = image_transport::create_publisher(this, "~/debug/mask");

  // publisher
  pub_ptr_ = this->create_publisher<PointCloudMsgType>("output", rclcpp::QoS{1});
}

void SegmentPointCloudFusionNode::preprocess(__attribute__((unused))
                                             PointCloudMsgType & pointcloud_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  return;
}

void SegmentPointCloudFusionNode::fuseOnSingleImage(
  const PointCloudMsgType & input_pointcloud_msg, const Det2dStatus<Image> & det2d,
  [[maybe_unused]] const Image & input_mask,
  __attribute__((unused)) PointCloudMsgType & output_cloud)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  if (input_pointcloud_msg.data.empty()) {
    return;
  }
  if (input_mask.height == 0 || input_mask.width == 0) {
    return;
  }

  const sensor_msgs::msg::CameraInfo & camera_info = det2d.camera_projector_ptr->getCameraInfo();
  std::vector<uint8_t> mask_data(input_mask.data.begin(), input_mask.data.end());
  cv::Mat mask = perception_utils::runLengthDecoder(mask_data, input_mask.height, input_mask.width);

  // publish debug mask
  if (is_publish_debug_mask_) {
    sensor_msgs::msg::Image::SharedPtr debug_mask_msg =
      cv_bridge::CvImage(std_msgs::msg::Header(), "mono8", mask).toImageMsg();
    debug_mask_msg->header = input_mask.header;
    pub_debug_mask_ptr_.publish(debug_mask_msg);
  }
  const int orig_width = camera_info.width;
  const int orig_height = camera_info.height;
  // resize mask to the same size as the camera image
  cv::resize(mask, mask, cv::Size(orig_width, orig_height), 0, 0, cv::INTER_NEAREST);

  geometry_msgs::msg::TransformStamped transform_stamped;
  // transform pointcloud from frame id to camera optical frame id
  {
    const auto transform_stamped_optional = getTransformStamped(
      tf_buffer_, input_mask.header.frame_id, input_pointcloud_msg.header.frame_id,
      input_pointcloud_msg.header.stamp);
    if (!transform_stamped_optional) {
      return;
    }
    transform_stamped = transform_stamped_optional.value();
  }

  PointCloudMsgType transformed_cloud;
  tf2::doTransform(input_pointcloud_msg, transformed_cloud, transform_stamped);

  int point_step = input_pointcloud_msg.point_step;
  int x_offset = input_pointcloud_msg.fields[pcl::getFieldIndex(input_pointcloud_msg, "x")].offset;
  int y_offset = input_pointcloud_msg.fields[pcl::getFieldIndex(input_pointcloud_msg, "y")].offset;
  int z_offset = input_pointcloud_msg.fields[pcl::getFieldIndex(input_pointcloud_msg, "z")].offset;

  for (size_t global_offset = 0; global_offset < transformed_cloud.data.size();
       global_offset += point_step) {
    float transformed_x =
      *reinterpret_cast<float *>(&transformed_cloud.data[global_offset + x_offset]);
    float transformed_y =
      *reinterpret_cast<float *>(&transformed_cloud.data[global_offset + y_offset]);
    float transformed_z =
      *reinterpret_cast<float *>(&transformed_cloud.data[global_offset + z_offset]);
    // skip filtering pointcloud behind the camera or too far from camera
    if (transformed_z <= 0.0 || transformed_z > filter_distance_threshold_) {
      continue;
    }

    Eigen::Vector2d projected_point;
    if (!det2d.camera_projector_ptr->calcImageProjectedPoint(
          cv::Point3d(transformed_x, transformed_y, transformed_z), projected_point)) {
      continue;
    }

    // skip filtering pointcloud where semantic id out of the defined list
    uint8_t semantic_id = mask.at<uint8_t>(
      static_cast<uint16_t>(projected_point.y()), static_cast<uint16_t>(projected_point.x()));
    if (
      static_cast<size_t>(semantic_id) >= filter_semantic_label_target_list_.size() ||
      !filter_semantic_label_target_list_.at(semantic_id).second) {
      continue;
    }

    filter_global_offset_set_.insert(global_offset);
  }
}

void SegmentPointCloudFusionNode::postprocess(
  const PointCloudMsgType & pointcloud_msg, PointCloudMsgType & output_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  output_msg.header = pointcloud_msg.header;
  output_msg.data.clear();
  output_msg.data.resize(pointcloud_msg.data.size());
  const int point_step = pointcloud_msg.point_step;

  size_t output_pointcloud_size = 0;
  for (size_t global_offset = 0; global_offset < pointcloud_msg.data.size();
       global_offset += point_step) {
    if (filter_global_offset_set_.find(global_offset) == filter_global_offset_set_.end()) {
      copyPointCloud(pointcloud_msg, point_step, global_offset, output_msg, output_pointcloud_size);
    }
  }

  output_msg.data.resize(output_pointcloud_size);
  output_msg.row_step = output_pointcloud_size / output_msg.height;
  output_msg.width = output_pointcloud_size / output_msg.point_step / output_msg.height;

  filter_global_offset_set_.clear();
  return;
}

}  // namespace autoware::image_projection_based_fusion

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(
  autoware::image_projection_based_fusion::SegmentPointCloudFusionNode)

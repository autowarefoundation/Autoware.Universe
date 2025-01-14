// Copyright 2020 TIER IV, Inc.
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

#include "autoware/image_projection_based_fusion/roi_cluster_fusion/node.hpp"

#include <autoware/image_projection_based_fusion/utils/geometry.hpp>
#include <autoware/image_projection_based_fusion/utils/utils.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
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

RoiClusterFusionNode::RoiClusterFusionNode(const rclcpp::NodeOptions & options)
: FusionNode<ClusterMsgType, RoiMsgType, ClusterMsgType>("roi_cluster_fusion", options)
{
  trust_object_iou_mode_ = declare_parameter<std::string>("trust_object_iou_mode");
  non_trust_object_iou_mode_ = declare_parameter<std::string>("non_trust_object_iou_mode");
  use_cluster_semantic_type_ = declare_parameter<bool>("use_cluster_semantic_type");
  only_allow_inside_cluster_ = declare_parameter<bool>("only_allow_inside_cluster");
  roi_scale_factor_ = declare_parameter<double>("roi_scale_factor");
  iou_threshold_ = declare_parameter<double>("iou_threshold");
  unknown_iou_threshold_ = declare_parameter<double>("unknown_iou_threshold");
  remove_unknown_ = declare_parameter<bool>("remove_unknown");
  fusion_distance_ = declare_parameter<double>("fusion_distance");
  trust_object_distance_ = declare_parameter<double>("trust_object_distance");

  // publisher
  pub_ptr_ = this->create_publisher<ClusterMsgType>("output", rclcpp::QoS{1});
}

void RoiClusterFusionNode::preprocess(ClusterMsgType & output_cluster_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  // reset cluster semantic type
  if (!use_cluster_semantic_type_) {
    for (auto & feature_object : output_cluster_msg.feature_objects) {
      feature_object.object.classification.front().label =
        autoware_perception_msgs::msg::ObjectClassification::UNKNOWN;
      feature_object.object.existence_probability = 0.0;
    }
  }
}

void RoiClusterFusionNode::fuseOnSingleImage(
  const ClusterMsgType & input_cluster_msg, const Det2dStatus<RoiMsgType> & det2d,
  const RoiMsgType & input_roi_msg, ClusterMsgType & output_cluster_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  const sensor_msgs::msg::CameraInfo & camera_info = det2d.camera_projector_ptr->getCameraInfo();

  // get transform from cluster frame id to camera optical frame id
  geometry_msgs::msg::TransformStamped transform_stamped;
  {
    const auto transform_stamped_optional = getTransformStamped(
      tf_buffer_, /*target*/ camera_info.header.frame_id,
      /*source*/ input_cluster_msg.header.frame_id, camera_info.header.stamp);
    if (!transform_stamped_optional) {
      RCLCPP_WARN_STREAM(
        get_logger(), "Failed to get transform from " << input_cluster_msg.header.frame_id << " to "
                                                      << camera_info.header.frame_id);
      return;
    }
    transform_stamped = transform_stamped_optional.value();
  }

  std::map<std::size_t, RegionOfInterest> m_cluster_roi;

  for (std::size_t i = 0; i < input_cluster_msg.feature_objects.size(); ++i) {
    if (input_cluster_msg.feature_objects.at(i).feature.cluster.data.empty()) {
      continue;
    }

    if (is_far_enough(input_cluster_msg.feature_objects.at(i), fusion_distance_)) {
      continue;
    }

    // filter point out of scope
    if (debugger_ && out_of_scope(input_cluster_msg.feature_objects.at(i))) {
      continue;
    }

    sensor_msgs::msg::PointCloud2 transformed_cluster;
    tf2::doTransform(
      input_cluster_msg.feature_objects.at(i).feature.cluster, transformed_cluster,
      transform_stamped);

    int min_x(camera_info.width), min_y(camera_info.height), max_x(0), max_y(0);
    std::vector<Eigen::Vector2d> projected_points;
    projected_points.reserve(transformed_cluster.data.size());
    for (sensor_msgs::PointCloud2ConstIterator<float> iter_x(transformed_cluster, "x"),
         iter_y(transformed_cluster, "y"), iter_z(transformed_cluster, "z");
         iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z) {
      if (*iter_z <= 0.0) {
        continue;
      }

      Eigen::Vector2d projected_point;
      if (det2d.camera_projector_ptr->calcImageProjectedPoint(
            cv::Point3d(*iter_x, *iter_y, *iter_z), projected_point)) {
        const int px = static_cast<int>(projected_point.x());
        const int py = static_cast<int>(projected_point.y());

        min_x = std::min(px, min_x);
        min_y = std::min(py, min_y);
        max_x = std::max(px, max_x);
        max_y = std::max(py, max_y);

        projected_points.push_back(projected_point);
        if (debugger_) debugger_->obstacle_points_.push_back(projected_point);
      }
    }
    if (projected_points.empty()) {
      continue;
    }

    sensor_msgs::msg::RegionOfInterest roi;
    roi.x_offset = min_x;
    roi.y_offset = min_y;
    roi.width = max_x - min_x;
    roi.height = max_y - min_y;
    m_cluster_roi.insert(std::make_pair(i, roi));
    if (debugger_) debugger_->obstacle_rois_.push_back(roi);
  }

  for (const auto & feature_obj : input_roi_msg.feature_objects) {
    int index = -1;
    bool associated = false;
    double max_iou = 0.0;
    const bool is_roi_label_known =
      feature_obj.object.classification.front().label != ObjectClassification::UNKNOWN;
    for (const auto & cluster_map : m_cluster_roi) {
      double iou(0.0);
      bool is_use_non_trust_object_iou_mode = is_far_enough(
        input_cluster_msg.feature_objects.at(cluster_map.first), trust_object_distance_);
      auto image_roi = feature_obj.feature.roi;
      auto cluster_roi = cluster_map.second;
      sanitizeROI(image_roi, camera_info.width, camera_info.height);
      sanitizeROI(cluster_roi, camera_info.width, camera_info.height);
      if (is_use_non_trust_object_iou_mode || is_roi_label_known) {
        iou = cal_iou_by_mode(cluster_roi, image_roi, non_trust_object_iou_mode_);
      } else {
        iou = cal_iou_by_mode(cluster_roi, image_roi, trust_object_iou_mode_);
      }

      const bool passed_inside_cluster_gate =
        only_allow_inside_cluster_ ? is_inside(image_roi, cluster_roi, roi_scale_factor_) : true;
      if (max_iou < iou && passed_inside_cluster_gate) {
        index = cluster_map.first;
        max_iou = iou;
        associated = true;
      }
    }

    if (!associated) {
      continue;
    }

    if (!output_cluster_msg.feature_objects.empty()) {
      auto & fused_object = output_cluster_msg.feature_objects.at(index).object;
      const bool is_roi_existence_prob_higher =
        fused_object.existence_probability <= feature_obj.object.existence_probability;
      const bool is_roi_iou_over_threshold =
        (is_roi_label_known && iou_threshold_ < max_iou) ||
        (!is_roi_label_known && unknown_iou_threshold_ < max_iou);

      if (is_roi_iou_over_threshold && is_roi_existence_prob_higher) {
        fused_object.classification = feature_obj.object.classification;
        // Update existence_probability for fused objects
        fused_object.existence_probability =
          std::clamp(feature_obj.object.existence_probability, min_roi_existence_prob_, 1.0f);
      }
    }
    if (debugger_) debugger_->image_rois_.push_back(feature_obj.feature.roi);
    if (debugger_) debugger_->max_iou_for_image_rois_.push_back(max_iou);
  }

  // note: debug objects are safely cleared in fusion_node.cpp
  if (debugger_) {
    debugger_->publishImage(det2d.id, input_roi_msg.header.stamp);
  }
}

bool RoiClusterFusionNode::out_of_scope(const DetectedObjectWithFeature & obj)
{
  auto cluster = obj.feature.cluster;
  bool is_out = false;
  auto valid_point = [](float p, float min_num, float max_num) -> bool {
    return (p > min_num) && (p < max_num);
  };

  for (sensor_msgs::PointCloud2ConstIterator<float> iter_x(cluster, "x"), iter_y(cluster, "y"),
       iter_z(cluster, "z");
       iter_x != iter_x.end(); ++iter_x, ++iter_y, ++iter_z) {
    if (!valid_point(*iter_x, filter_scope_min_x_, filter_scope_max_x_)) {
      is_out = true;
      break;
    }

    if (!valid_point(*iter_y, filter_scope_min_y_, filter_scope_max_y_)) {
      is_out = true;
      break;
    }

    if (!valid_point(*iter_z, filter_scope_min_z_, filter_scope_max_z_)) {
      is_out = true;
      break;
    }
  }

  return is_out;
}

bool RoiClusterFusionNode::is_far_enough(
  const DetectedObjectWithFeature & obj, const double distance_threshold)
{
  const auto & position = obj.object.kinematics.pose_with_covariance.pose.position;
  return position.x * position.x + position.y * position.y >
         distance_threshold * distance_threshold;
}

double RoiClusterFusionNode::cal_iou_by_mode(
  const sensor_msgs::msg::RegionOfInterest & roi_1,
  const sensor_msgs::msg::RegionOfInterest & roi_2, const std::string iou_mode)
{
  switch (IOU_MODE_MAP.at(iou_mode)) {
    case 0 /* use iou mode */:
      return calcIoU(roi_1, roi_2);

    case 1 /* use iou_x mode */:
      return calcIoUX(roi_1, roi_2);

    case 2 /* use iou_y mode */:
      return calcIoUY(roi_1, roi_2);
    default:
      return 0.0;
  }
}

void RoiClusterFusionNode::postprocess(
  const ClusterMsgType & processing_msg, ClusterMsgType & output_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  output_msg = processing_msg;

  if (remove_unknown_) {
    // filter by object classification and existence probability
    output_msg.feature_objects.clear();
    for (const auto & feature_object : processing_msg.feature_objects) {
      if (
        feature_object.object.classification.front().label !=
          autoware_perception_msgs::msg::ObjectClassification::UNKNOWN ||
        feature_object.object.existence_probability >= min_roi_existence_prob_) {
        output_msg.feature_objects.push_back(feature_object);
      }
    }
  }
}

}  // namespace autoware::image_projection_based_fusion

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::image_projection_based_fusion::RoiClusterFusionNode)

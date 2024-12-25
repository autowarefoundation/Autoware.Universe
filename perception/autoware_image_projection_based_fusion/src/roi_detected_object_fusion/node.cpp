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

#include "autoware/image_projection_based_fusion/roi_detected_object_fusion/node.hpp"

#include "autoware/object_recognition_utils/object_recognition_utils.hpp"

#include <autoware/image_projection_based_fusion/utils/geometry.hpp>
#include <autoware/image_projection_based_fusion/utils/utils.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace autoware::image_projection_based_fusion
{
using autoware::universe_utils::ScopedTimeTrack;

RoiDetectedObjectFusionNode::RoiDetectedObjectFusionNode(const rclcpp::NodeOptions & options)
: FusionNode<DetectedObjects, DetectedObject, DetectedObjectsWithFeature>(
    "roi_detected_object_fusion", options)
{
  fusion_params_.passthrough_lower_bound_probability_thresholds =
    declare_parameter<std::vector<double>>("passthrough_lower_bound_probability_thresholds");
  fusion_params_.min_iou_threshold = declare_parameter<double>("min_iou_threshold");
  fusion_params_.trust_distances = declare_parameter<std::vector<double>>("trust_distances");
  fusion_params_.use_roi_probability = declare_parameter<bool>("use_roi_probability");
  fusion_params_.roi_probability_threshold = declare_parameter<double>("roi_probability_threshold");
  {
    const auto can_assign_vector_tmp = declare_parameter<std::vector<int64_t>>("can_assign_matrix");
    std::vector<int> can_assign_vector(can_assign_vector_tmp.begin(), can_assign_vector_tmp.end());
    const int label_num = static_cast<int>(std::sqrt(can_assign_vector.size()));
    Eigen::Map<Eigen::MatrixXi> can_assign_matrix_tmp(
      can_assign_vector.data(), label_num, label_num);
    fusion_params_.can_assign_matrix = can_assign_matrix_tmp.transpose();
  }
}

void RoiDetectedObjectFusionNode::preprocess(DetectedObjects & output_msg)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  std::vector<bool> passthrough_object_flags, fused_object_flags, ignored_object_flags;
  passthrough_object_flags.resize(output_msg.objects.size());
  fused_object_flags.resize(output_msg.objects.size());
  ignored_object_flags.resize(output_msg.objects.size());
  for (std::size_t obj_i = 0; obj_i < output_msg.objects.size(); ++obj_i) {
    const auto & object = output_msg.objects.at(obj_i);
    const auto label =
      autoware::object_recognition_utils::getHighestProbLabel(object.classification);
    const auto pos = autoware::object_recognition_utils::getPose(object).position;
    const auto object_sqr_dist = pos.x * pos.x + pos.y * pos.y;
    const auto prob_threshold =
      fusion_params_.passthrough_lower_bound_probability_thresholds.at(label);
    const auto trust_sqr_dist =
      fusion_params_.trust_distances.at(label) * fusion_params_.trust_distances.at(label);
    if (object.existence_probability > prob_threshold || object_sqr_dist > trust_sqr_dist) {
      passthrough_object_flags.at(obj_i) = true;
    }
  }

  int64_t timestamp_nsec =
    output_msg.header.stamp.sec * static_cast<int64_t>(1e9) + output_msg.header.stamp.nanosec;
  passthrough_object_flags_map_.insert(std::make_pair(timestamp_nsec, passthrough_object_flags));
  fused_object_flags_map_.insert(std::make_pair(timestamp_nsec, fused_object_flags));
  ignored_object_flags_map_.insert(std::make_pair(timestamp_nsec, ignored_object_flags));
}

void RoiDetectedObjectFusionNode::fuseOnSingleImage(
  const DetectedObjects & input_object_msg, const std::size_t image_id,
  const DetectedObjectsWithFeature & input_roi_msg,
  DetectedObjects & output_object_msg __attribute__((unused)))
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  Eigen::Affine3d object2camera_affine;
  {
    const auto transform_stamped_optional = getTransformStamped(
      tf_buffer_, /*target*/ input_roi_msg.header.frame_id,
      /*source*/ input_object_msg.header.frame_id, input_roi_msg.header.stamp);
    if (!transform_stamped_optional) {
      return;
    }
    object2camera_affine = transformToEigen(transform_stamped_optional.value().transform);
  }

  const auto object_roi_map =
    generateDetectedObjectRoIs(input_object_msg, image_id, object2camera_affine);
  fuseObjectsOnImage(input_object_msg, input_roi_msg.feature_objects, object_roi_map);

  if (debugger_) {
    debugger_->image_rois_.reserve(input_roi_msg.feature_objects.size());
    for (std::size_t roi_i = 0; roi_i < input_roi_msg.feature_objects.size(); ++roi_i) {
      debugger_->image_rois_.push_back(input_roi_msg.feature_objects.at(roi_i).feature.roi);
    }
    debugger_->publishImage(image_id, input_roi_msg.header.stamp);
  }
}

std::map<std::size_t, DetectedObjectWithFeature>
RoiDetectedObjectFusionNode::generateDetectedObjectRoIs(
  const DetectedObjects & input_object_msg, const std::size_t & image_id,
  const Eigen::Affine3d & object2camera_affine)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  std::map<std::size_t, DetectedObjectWithFeature> object_roi_map;
  int64_t timestamp_nsec = input_object_msg.header.stamp.sec * static_cast<int64_t>(1e9) +
                           input_object_msg.header.stamp.nanosec;
  if (passthrough_object_flags_map_.size() == 0) {
    return object_roi_map;
  }
  if (passthrough_object_flags_map_.count(timestamp_nsec) == 0) {
    return object_roi_map;
  }
  const auto & passthrough_object_flags = passthrough_object_flags_map_.at(timestamp_nsec);
  const sensor_msgs::msg::CameraInfo & camera_info = camera_projectors_[image_id].getCameraInfo();
  const double image_width = static_cast<double>(camera_info.width);
  const double image_height = static_cast<double>(camera_info.height);

  for (std::size_t obj_i = 0; obj_i < input_object_msg.objects.size(); ++obj_i) {
    std::vector<Eigen::Vector3d> vertices_camera_coord;
    const auto & object = input_object_msg.objects.at(obj_i);

    if (passthrough_object_flags.at(obj_i)) {
      continue;
    }

    // filter point out of scope
    if (debugger_ && out_of_scope(object)) {
      continue;
    }

    {
      std::vector<Eigen::Vector3d> vertices;
      objectToVertices(object.kinematics.pose_with_covariance.pose, object.shape, vertices);
      transformPoints(vertices, object2camera_affine, vertices_camera_coord);
    }

    double min_x(image_width), min_y(image_height), max_x(0.0), max_y(0.0);
    std::size_t point_on_image_cnt = 0;
    for (const auto & point : vertices_camera_coord) {
      if (point.z() <= 0.0) {
        continue;
      }

      Eigen::Vector2d proj_point;
      if (camera_projectors_[image_id].calcImageProjectedPoint(
            cv::Point3d(point.x(), point.y(), point.z()), proj_point)) {
        const double px = proj_point.x();
        const double py = proj_point.y();

        min_x = std::min(px, min_x);
        min_y = std::min(py, min_y);
        max_x = std::max(px, max_x);
        max_y = std::max(py, max_y);

        point_on_image_cnt++;

        if (debugger_) {
          debugger_->obstacle_points_.push_back(proj_point);
        }
      }
    }
    if (point_on_image_cnt < 3) {
      continue;
    }

    const uint32_t idx_min_x = std::floor(std::max(min_x, 0.0));
    const uint32_t idx_min_y = std::floor(std::max(min_y, 0.0));
    const uint32_t idx_max_x = std::ceil(std::min(max_x, image_width));
    const uint32_t idx_max_y = std::ceil(std::min(max_y, image_height));

    DetectedObjectWithFeature object_roi;
    object_roi.feature.roi.x_offset = idx_min_x;
    object_roi.feature.roi.y_offset = idx_min_y;
    object_roi.feature.roi.width = idx_max_x - idx_min_x;
    object_roi.feature.roi.height = idx_max_y - idx_min_y;
    object_roi.object = object;
    object_roi_map.insert(std::make_pair(obj_i, object_roi));

    if (debugger_) {
      debugger_->obstacle_rois_.push_back(object_roi.feature.roi);
    }
  }

  return object_roi_map;
}

void RoiDetectedObjectFusionNode::fuseObjectsOnImage(
  const DetectedObjects & input_object_msg,
  const std::vector<DetectedObjectWithFeature> & image_rois,
  const std::map<std::size_t, DetectedObjectWithFeature> & object_roi_map)
{
  std::unique_ptr<ScopedTimeTrack> st_ptr;
  if (time_keeper_) st_ptr = std::make_unique<ScopedTimeTrack>(__func__, *time_keeper_);

  int64_t timestamp_nsec = input_object_msg.header.stamp.sec * static_cast<int64_t>(1e9) +
                           input_object_msg.header.stamp.nanosec;
  if (fused_object_flags_map_.size() == 0 || ignored_object_flags_map_.size() == 0) {
    return;
  }
  if (
    fused_object_flags_map_.count(timestamp_nsec) == 0 ||
    ignored_object_flags_map_.count(timestamp_nsec) == 0) {
    return;
  }
  auto & fused_object_flags = fused_object_flags_map_.at(timestamp_nsec);
  auto & ignored_object_flags = ignored_object_flags_map_.at(timestamp_nsec);

  for (const auto & object_pair : object_roi_map) {
    const auto & obj_i = object_pair.first;
    if (fused_object_flags.at(obj_i)) {
      continue;
    }

    float roi_prob = 0.0f;
    float max_iou = 0.0f;
    for (const auto & image_roi : image_rois) {
      const auto & object_roi = object_pair.second;
      const auto object_roi_label =
        autoware::object_recognition_utils::getHighestProbLabel(object_roi.object.classification);
      const auto image_roi_label =
        autoware::object_recognition_utils::getHighestProbLabel(image_roi.object.classification);
      if (!fusion_params_.can_assign_matrix(object_roi_label, image_roi_label)) {
        continue;
      }

      const double iou = calcIoU(object_roi.feature.roi, image_roi.feature.roi);
      if (iou > max_iou) {
        max_iou = iou;
        roi_prob = image_roi.object.existence_probability;
      }
    }

    if (max_iou > fusion_params_.min_iou_threshold) {
      if (fusion_params_.use_roi_probability) {
        if (roi_prob > fusion_params_.roi_probability_threshold) {
          fused_object_flags.at(obj_i) = true;
        } else {
          ignored_object_flags.at(obj_i) = true;
        }
      } else {
        fused_object_flags.at(obj_i) = true;
      }
    } else {
      ignored_object_flags.at(obj_i) = true;
    }
  }
}

bool RoiDetectedObjectFusionNode::out_of_scope(const DetectedObject & obj)
{
  bool is_out = true;
  auto pose = obj.kinematics.pose_with_covariance.pose;

  auto valid_point = [](float p, float min_num, float max_num) -> bool {
    return (p > min_num) && (p < max_num);
  };

  if (!valid_point(pose.position.x, filter_scope_min_x_, filter_scope_max_x_)) {
    return is_out;
  }
  if (!valid_point(pose.position.y, filter_scope_min_y_, filter_scope_max_y_)) {
    return is_out;
  }
  if (!valid_point(pose.position.z, filter_scope_min_z_, filter_scope_max_z_)) {
    return is_out;
  }

  is_out = false;
  return is_out;
}

void RoiDetectedObjectFusionNode::publish(const DetectedObjects & output_msg)
{
  if (pub_ptr_->get_subscription_count() < 1) {
    return;
  }

  int64_t timestamp_nsec =
    output_msg.header.stamp.sec * static_cast<int64_t>(1e9) + output_msg.header.stamp.nanosec;
  if (
    passthrough_object_flags_map_.size() == 0 || fused_object_flags_map_.size() == 0 ||
    ignored_object_flags_map_.size() == 0) {
    return;
  }
  if (
    passthrough_object_flags_map_.count(timestamp_nsec) == 0 ||
    fused_object_flags_map_.count(timestamp_nsec) == 0 ||
    ignored_object_flags_map_.count(timestamp_nsec) == 0) {
    return;
  }
  auto & passthrough_object_flags = passthrough_object_flags_map_.at(timestamp_nsec);
  auto & fused_object_flags = fused_object_flags_map_.at(timestamp_nsec);
  auto & ignored_object_flags = ignored_object_flags_map_.at(timestamp_nsec);

  DetectedObjects output_objects_msg, debug_fused_objects_msg, debug_ignored_objects_msg;
  output_objects_msg.header = output_msg.header;
  debug_fused_objects_msg.header = output_msg.header;
  debug_ignored_objects_msg.header = output_msg.header;
  for (std::size_t obj_i = 0; obj_i < output_msg.objects.size(); ++obj_i) {
    const auto & obj = output_msg.objects.at(obj_i);
    if (passthrough_object_flags.at(obj_i)) {
      output_objects_msg.objects.emplace_back(obj);
    }
    if (fused_object_flags.at(obj_i)) {
      output_objects_msg.objects.emplace_back(obj);
      debug_fused_objects_msg.objects.emplace_back(obj);
    }
    if (ignored_object_flags.at(obj_i)) {
      debug_ignored_objects_msg.objects.emplace_back(obj);
    }
  }

  pub_ptr_->publish(output_objects_msg);

  debug_publisher_->publish<DetectedObjects>("debug/fused_objects", debug_fused_objects_msg);
  debug_publisher_->publish<DetectedObjects>("debug/ignored_objects", debug_ignored_objects_msg);

  passthrough_object_flags_map_.erase(timestamp_nsec);
  fused_object_flags_map_.erase(timestamp_nsec);
  ignored_object_flags_map_.erase(timestamp_nsec);
}

}  // namespace autoware::image_projection_based_fusion

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(
  autoware::image_projection_based_fusion::RoiDetectedObjectFusionNode)

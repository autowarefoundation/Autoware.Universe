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

#ifndef AUTOWARE__LIDAR_BEVFUSION__BEVFUSION_CONFIG_HPP_
#define AUTOWARE__LIDAR_BEVFUSION__BEVFUSION_CONFIG_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace autoware::lidar_bevfusion
{

class BEVFusionConfig
{
public:
  // cSpell:ignore dbound, xbound, ybound, zbound
  BEVFusionConfig(
    const bool sensor_fusion, const std::string & plugins_path, const std::int64_t out_size_factor,
    const std::int64_t cloud_capacity, const std::int64_t max_points_per_voxel,
    const std::vector<std::int64_t> & voxels_num, const std::vector<float> & point_cloud_range,
    const std::vector<float> & voxel_size, const std::vector<float> & dbound,
    const std::vector<float> & xbound, const std::vector<float> & ybound,
    const std::vector<float> & zbound, const std::int64_t num_cameras,
    const std::int64_t raw_image_height, const std::int64_t raw_image_width,
    const float img_aug_scale_x, const float img_aug_scale_y, const std::int64_t roi_height,
    const std::int64_t roi_width, const std::int64_t features_height,
    const std::int64_t features_width, const std::int64_t num_depth_features,
    const std::int64_t num_proposals, const float circle_nms_dist_threshold,
    const std::vector<double> & yaw_norm_thresholds, const float score_threshold)
  {
    sensor_fusion_ = sensor_fusion;
    plugins_path_ = plugins_path;

    out_size_factor_ = out_size_factor;

    cloud_capacity_ = cloud_capacity;
    max_points_per_voxel_ = max_points_per_voxel;

    if (voxels_num.size() == 3) {
      min_num_voxels_ = voxels_num[0];
      max_num_voxels_ = voxels_num[2];

      voxels_num_[0] = voxels_num[0];
      voxels_num_[1] = voxels_num[1];
      voxels_num_[2] = voxels_num[2];
    }
    if (point_cloud_range.size() == 6) {
      min_x_range_ = point_cloud_range[0];
      min_y_range_ = point_cloud_range[1];
      min_z_range_ = point_cloud_range[2];
      max_x_range_ = point_cloud_range[3];
      max_y_range_ = point_cloud_range[4];
      max_z_range_ = point_cloud_range[5];
    }
    if (voxel_size.size() == 3) {
      voxel_x_size_ = voxel_size[0];
      voxel_y_size_ = voxel_size[1];
      voxel_z_size_ = voxel_size[2];
    }
    if (dbound.size() == 3 && xbound.size() == 3 && ybound.size() == 3 && zbound.size() == 3) {
      dbound_ = dbound;
      xbound_ = xbound;
      ybound_ = ybound;
      zbound_ = zbound;
    }

    num_cameras_ = num_cameras;
    raw_image_height_ = raw_image_height;
    raw_image_width_ = raw_image_width;
    img_aug_scale_x_ = img_aug_scale_x;
    img_aug_scale_y_ = img_aug_scale_y;
    roi_height_ = roi_height;
    roi_width_ = roi_width;
    features_height_ = features_height;
    features_width_ = features_width;
    num_depth_features_ = num_depth_features;
    resized_height_ = raw_image_height_ * img_aug_scale_y_;
    resized_width_ = raw_image_width_ * img_aug_scale_x_;

    if (num_proposals > 0) {
      num_proposals_ = num_proposals;
    }
    if (score_threshold > 0.0) {
      score_threshold_ = score_threshold;
    }
    if (circle_nms_dist_threshold > 0.0) {
      circle_nms_dist_threshold_ = circle_nms_dist_threshold;
    }
    yaw_norm_thresholds_ =
      std::vector<float>(yaw_norm_thresholds.begin(), yaw_norm_thresholds.end());
    for (auto & yaw_norm_threshold : yaw_norm_thresholds_) {
      yaw_norm_threshold =
        (yaw_norm_threshold >= 0.0 && yaw_norm_threshold < 1.0) ? yaw_norm_threshold : 0.0;
    }
    grid_x_size_ = static_cast<std::int64_t>((max_x_range_ - min_x_range_) / voxel_x_size_);
    grid_y_size_ = static_cast<std::int64_t>((max_y_range_ - min_y_range_) / voxel_y_size_);
    grid_z_size_ = static_cast<std::int64_t>((max_z_range_ - min_z_range_) / voxel_z_size_);
  }

  ///// MODALITY /////
  bool sensor_fusion_{};

  // CUDA parameters
  const std::uint32_t threads_per_block_{256};  // threads number for a block

  // TensorRT parameters
  std::string plugins_path_{};

  ///// NETWORK PARAMETERS /////

  // Common network parameters
  std::int64_t out_size_factor_{};

  std::int64_t cloud_capacity_{};
  std::int64_t min_num_voxels_{};
  std::int64_t max_num_voxels_{};
  std::int64_t max_points_per_voxel_;
  const std::int64_t num_point_feature_size_{5};  // x, y, z, intensity, lag

  // Pointcloud range in meters
  float min_x_range_{};
  float max_x_range_{};
  float min_y_range_{};
  float max_y_range_{};
  float min_z_range_{};
  float max_z_range_{};

  // Voxel size in meters
  float voxel_x_size_{};
  float voxel_y_size_{};
  float voxel_z_size_{};

  // Grid size
  std::int64_t grid_x_size_{};
  std::int64_t grid_y_size_{};
  std::int64_t grid_z_size_{};

  // Camera branch parameters
  std::vector<float> dbound_{};
  std::vector<float> xbound_{};
  std::vector<float> ybound_{};
  std::vector<float> zbound_{};

  std::int64_t num_cameras_{};
  std::int64_t raw_image_height_{};
  std::int64_t raw_image_width_{};

  float img_aug_scale_x_{};
  float img_aug_scale_y_{};

  std::int64_t roi_height_{};
  std::int64_t roi_width_{};

  std::int64_t resized_height_{};
  std::int64_t resized_width_{};

  std::int64_t features_height_{};
  std::int64_t features_width_{};
  std::int64_t num_depth_features_{};

  // Head parameters
  std::int64_t num_proposals_{};
  const std::size_t num_classes_{5};

  // Post processing parameters

  // the score threshold for classification
  float score_threshold_{};

  float circle_nms_dist_threshold_{};
  std::vector<float> yaw_norm_thresholds_{};
  // the detected boxes result decode by (x, y, z, w, l, h, yaw, vx, vy)
  const std::int64_t num_box_values_{10};

  ///// RUNTIME DIMENSIONS /////
  std::array<std::int64_t, 3> voxels_num_{};
};

}  // namespace autoware::lidar_bevfusion

#endif  // AUTOWARE__LIDAR_BEVFUSION__BEVFUSION_CONFIG_HPP_

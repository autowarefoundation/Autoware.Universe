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

#ifndef AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__CAMERA_PROJECTION_HPP_
#define AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__CAMERA_PROJECTION_HPP_

#define EIGEN_MPL2_ONLY

#include <Eigen/Core>
#include <opencv2/core/core.hpp>

#include <sensor_msgs/msg/camera_info.hpp>

#include <image_geometry/pinhole_camera_model.h>

#include <memory>

namespace autoware::image_projection_based_fusion
{
struct PixelPos
{
  float x;
  float y;
};

class CameraProjection
{
public:
  explicit CameraProjection(
    const sensor_msgs::msg::CameraInfo & camera_info, const float grid_cell_width,
    const float grid_cell_height, const bool unrectify, const bool use_approximation);
  CameraProjection() : cell_width_(1.0), cell_height_(1.0), unrectify_(false) {}
  void initialize();
  std::function<bool(const cv::Point3d &, Eigen::Vector2d &)> calcImageProjectedPoint;
  sensor_msgs::msg::CameraInfo getCameraInfo();
  bool isOutsideHorizontalView(const float px, const float pz);
  bool isOutsideVerticalView(const float py, const float pz);
  bool isOutsideFOV(const float px, const float py, const float pz);

protected:
  bool calcRectifiedImageProjectedPoint(
    const cv::Point3d & point3d, Eigen::Vector2d & projected_point);
  bool calcRawImageProjectedPoint(const cv::Point3d & point3d, Eigen::Vector2d & projected_point);
  bool calcRawImageProjectedPointWithApproximation(
    const cv::Point3d & point3d, Eigen::Vector2d & projected_point);
  void initializeCache();

  sensor_msgs::msg::CameraInfo camera_info_;
  uint32_t image_height_, image_width_;
  double tan_h_x_, tan_h_y_;
  double fov_left_, fov_right_, fov_top_, fov_bottom_;

  uint32_t cache_size_;
  float cell_width_;
  float cell_height_;
  float inv_cell_width_;
  float inv_cell_height_;
  int grid_width_;
  int grid_height_;

  bool unrectify_;
  bool use_approximation_;

  std::unique_ptr<PixelPos[]> projection_cache_;
  image_geometry::PinholeCameraModel camera_model_;
};

}  // namespace autoware::image_projection_based_fusion

#endif  // AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__CAMERA_PROJECTION_HPP_

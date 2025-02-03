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

#ifndef AUTOWARE__LIDAR_BEVFUSION__PREPROCESS__PRECOMPUTED_FEATURES_HPP_
#define AUTOWARE__LIDAR_BEVFUSION__PREPROCESS__PRECOMPUTED_FEATURES_HPP_

#include "autoware/lidar_bevfusion/bevfusion_config.hpp"

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

#include <sensor_msgs/msg/camera_info.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <vector>

namespace autoware::lidar_bevfusion
{

using Tensor1D = Eigen::Tensor<float, 1, Eigen::RowMajor>;
using Tensor2D = Eigen::Tensor<float, 2, Eigen::RowMajor>;
using Tensor3D = Eigen::Tensor<float, 3, Eigen::RowMajor>;
using Tensor4D = Eigen::Tensor<float, 4, Eigen::RowMajor>;
using Tensor5D = Eigen::Tensor<float, 5, Eigen::RowMajor>;
using Tensor6D = Eigen::Tensor<float, 6, Eigen::RowMajor>;
using Tensor7D = Eigen::Tensor<float, 7, Eigen::RowMajor>;

using TensorMap1D = Eigen::TensorMap<Tensor1D>;
using TensorMap2D = Eigen::TensorMap<Tensor2D>;
using TensorMap3D = Eigen::TensorMap<Tensor3D>;
using TensorMap4D = Eigen::TensorMap<Tensor4D>;
using TensorMap5D = Eigen::TensorMap<Tensor5D>;
using TensorMap6D = Eigen::TensorMap<Tensor6D>;
using TensorMap7D = Eigen::TensorMap<Tensor7D>;

using Matrix4fRowM = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using Matrix3fRowM = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;

using Vector3fRowM = Eigen::Matrix<float, 1, 3, Eigen::RowMajor>;

Tensor4D create_frustum(const BEVFusionConfig & config);

Tensor5D get_geometry(
  const Tensor4D & frustum,             // [D, H, W, 3]
  const Tensor3D & camera2lidar_rots,   // [N, 3, 3]
  const Tensor2D & camera2lidar_trans,  // [N, 3]
  const Tensor3D & intrins_inverse,     // [N, 3, 3]
  const Tensor3D & post_rots_inverse,   // [N, 3, 3]
  const Tensor2D & post_trans           // [N, 3]
);

// Define the function
std::tuple<
  Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor>,
  Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic, Eigen::RowMajor>,
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor>,
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor> >
bev_pool_aux(const Tensor5D & geom_feats_input, const BEVFusionConfig & config);

std::tuple<
  Eigen::VectorXf,  // lidar2image
  Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor>,
  Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic, Eigen::RowMajor>,
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor>,
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor> >
precompute_features(
  const std::vector<Matrix4fRowM> & lidar2camera_transforms,
  const std::vector<Matrix4fRowM> & camera_aug_matrices,
  const std::vector<sensor_msgs::msg::CameraInfo> & camera_info_vector,
  const BEVFusionConfig & config);

}  // namespace autoware::lidar_bevfusion

#endif  // AUTOWARE__LIDAR_BEVFUSION__PREPROCESS__PRECOMPUTED_FEATURES_HPP_

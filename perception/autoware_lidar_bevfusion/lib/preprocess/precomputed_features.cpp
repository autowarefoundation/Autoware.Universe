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

#include "autoware/lidar_bevfusion/preprocess/precomputed_features.hpp"

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <tuple>
#include <utility>
#include <vector>

namespace autoware::lidar_bevfusion
{

Tensor4D create_frustum(const BEVFusionConfig & config)
{
  const float dbound_start = config.dbound_[0];
  const float dbound_end = config.dbound_[1];
  const float dbound_step = config.dbound_[2];
  const int roi_height = static_cast<int>(config.roi_height_);            // Image height
  const int roi_width = static_cast<int>(config.roi_width_);              // Image width
  const int features_height = static_cast<int>(config.features_height_);  // Feature height
  const int features_width = static_cast<int>(config.features_width_);    // Feature width

  // Compute D (number of depth layers)
  int D = static_cast<Eigen::Index>(std::floor((dbound_end - dbound_start) / dbound_step));
  assert(static_cast<std::int64_t>(D) == config.num_depth_features_);

  Tensor1D ds_vec(D);
  for (Eigen::Index i = 0; i < D; ++i) {
    ds_vec(i) = dbound_start + i * dbound_step;
  }

  // Create xs_lin and ys_lin tensors
  Tensor1D xs_lin(features_width);
  for (Eigen::Index i = 0; i < features_width; ++i) {
    xs_lin(i) = static_cast<double>(i) * (roi_width - 1) / (features_width - 1);
  }

  Tensor1D ys_lin(features_height);
  for (Eigen::Index i = 0; i < features_height; ++i) {
    ys_lin(i) = static_cast<double>(i) * (roi_height - 1) / (features_height - 1);
  }

  // Reshape and broadcast ds_vec to (D, features_height, features_width)
  Tensor3D ds_tensor = ds_vec.reshape(Eigen::array<Eigen::Index, 3>{D, 1, 1});
  Tensor3D ds_broadcast =
    ds_tensor.broadcast(Eigen::array<Eigen::Index, 3>{1, features_height, features_width});

  // Reshape and broadcast xs_lin to (1, 1, features_width)
  Tensor3D xs_tensor = xs_lin.reshape(Eigen::array<Eigen::Index, 3>{1, 1, features_width});
  Tensor3D xs_broadcast = xs_tensor.broadcast(Eigen::array<Eigen::Index, 3>{D, features_height, 1});

  // Reshape and broadcast ys_lin to (1, features_height, 1)
  Tensor3D ys_tensor = ys_lin.reshape(Eigen::array<Eigen::Index, 3>{1, features_height, 1});
  Tensor3D ys_broadcast = ys_tensor.broadcast(Eigen::array<Eigen::Index, 3>{D, 1, features_width});

  // Stack xs, ys, ds along the last dimension to form frustum
  Tensor4D frustum(D, features_height, features_width, 3);
  frustum.chip(0, 3) = xs_broadcast;
  frustum.chip(1, 3) = ys_broadcast;
  frustum.chip(2, 3) = ds_broadcast;

  return frustum;
}

Tensor5D get_geometry(
  const Tensor4D & frustum,             // [D, H, W, 3]
  const Tensor3D & camera2lidar_rots,   // [N, 3, 3]
  const Tensor2D & camera2lidar_trans,  // [N, 3]
  const Tensor3D & intrins_inverse,     // [N, 3, 3]
  const Tensor3D & post_rots_inverse,   // [N, 3, 3]
  const Tensor2D & post_trans           // [N, 3]
)
{
  // Get dimensions
  int N = camera2lidar_trans.dimension(0);
  int D = frustum.dimension(0);
  int H = frustum.dimension(1);
  int W = frustum.dimension(2);

  // Reshape and broadcast frustum to [N, D, H, W, 3]
  Tensor5D frustum_broadcast = frustum.reshape(Eigen::array<Eigen::Index, 5>{1, D, H, W, 3})
                                 .broadcast(Eigen::array<Eigen::Index, 5>{N, 1, 1, 1, 1});

  // Reshape post_trans to [N, 1, 1, 1, 3]
  Tensor5D post_trans_broadcast = post_trans.reshape(Eigen::array<Eigen::Index, 5>{N, 1, 1, 1, 3})
                                    .broadcast(Eigen::array<Eigen::Index, 5>{1, D, H, W, 1});

  // Subtract post_trans from frustum
  Tensor5D points = frustum_broadcast - post_trans_broadcast;

  // Unsqueeze points to [N, D, H, W, 3, 1]
  Tensor6D points_unsqueezed = points.reshape(Eigen::array<Eigen::Index, 6>{N, D, H, W, 3, 1});

  Tensor5D points_rotated(N, D, H, W, 3);

  for (Eigen::Index camera_index = 0; camera_index < N; camera_index++) {
    Tensor2D post_rot_inverse = post_rots_inverse.chip(camera_index, 0);
    Tensor4D points_sliced = points.chip(camera_index, 0);

    Eigen::array<Eigen::IndexPair<int>, 1> contract_dims = {Eigen::IndexPair<int>(3, 0)};
    Eigen::array<int, 2> shuffling({1, 0});
    Tensor2D post_rot_inverse_transposed = post_rot_inverse.shuffle(shuffling);

    Tensor4D points_sliced_rotated =
      points_sliced.contract(post_rot_inverse_transposed, contract_dims);
    points_rotated.chip(camera_index, 0) = points_sliced_rotated;
  }

  // Remove the last dimension (size 1)
  Tensor5D points_squeezed = points_rotated;

  Tensor5D points_xy = points_squeezed.slice(
    Eigen::array<Eigen::Index, 5>{0, 0, 0, 0, 0}, Eigen::array<Eigen::Index, 5>{N, D, H, W, 2});
  Tensor5D points_z = points_squeezed.slice(
    Eigen::array<Eigen::Index, 5>{0, 0, 0, 0, 2}, Eigen::array<Eigen::Index, 5>{N, D, H, W, 1});
  Tensor5D points_xy_scaled =
    points_xy * points_z.broadcast(Eigen::array<Eigen::Index, 5>{1, 1, 1, 1, 2});

  // Concatenate points_xy_scaled and points_z along the last dimension
  Tensor5D points_cat(points_squeezed.dimensions());
  points_cat.slice(
    Eigen::array<Eigen::Index, 5>{0, 0, 0, 0, 0}, Eigen::array<Eigen::Index, 5>{N, D, H, W, 2}) =
    points_xy_scaled;
  points_cat.slice(
    Eigen::array<Eigen::Index, 5>{0, 0, 0, 0, 2}, Eigen::array<Eigen::Index, 5>{N, D, H, W, 1}) =
    points_z;

  Tensor3D combine(intrins_inverse.dimensions());

  for (Eigen::Index camera_index = 0; camera_index < N; camera_index++) {
    Tensor2D camera2lidar_rot = camera2lidar_rots.chip(camera_index, 0);
    Tensor2D intrins_inv = intrins_inverse.chip(camera_index, 0);

    Eigen::array<Eigen::IndexPair<int>, 1> contract_dims = {Eigen::IndexPair<int>(1, 0)};

    Tensor2D combine_sliced = camera2lidar_rot.contract(intrins_inv, contract_dims);
    combine.chip(camera_index, 0) = combine_sliced;
  }

  // Reshape combine to [N, 1, 1, 1, 3, 3]
  Tensor6D combine_reshaped = combine.reshape(Eigen::array<Eigen::Index, 6>{N, 1, 1, 1, 3, 3});

  Tensor5D points_transformed(points_cat.dimensions());

  for (Eigen::Index camera_index = 0; camera_index < N; camera_index++) {
    Tensor2D combine_sliced = combine.chip(camera_index, 0);
    Tensor4D points_sliced = points_cat.chip(camera_index, 0);

    Eigen::array<Eigen::IndexPair<int>, 1> contract_dims = {Eigen::IndexPair<int>(3, 0)};
    Eigen::array<int, 2> shuffling({1, 0});
    Tensor2D combined_transposed = combine_sliced.shuffle(shuffling);

    Tensor4D points_transformed_sliced = points_sliced.contract(combined_transposed, contract_dims);
    points_transformed.chip(camera_index, 0) = points_transformed_sliced;
  }

  // Squeeze the last dimension
  Tensor5D points_squeezed_final =
    points_transformed.reshape(Eigen::array<Eigen::Index, 5>{N, D, H, W, 3});

  // Add camera2lidar_trans
  Tensor5D camera2lidar_trans_broacast =
    camera2lidar_trans.reshape(Eigen::array<Eigen::Index, 5>{N, 1, 1, 1, 3})
      .broadcast(Eigen::array<Eigen::Index, 5>{1, D, H, W, 1});
  ;
  Tensor5D points_final = points_squeezed_final + camera2lidar_trans_broacast;

  return points_final;
}

// Define the function
std::tuple<
    Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor>,  // geom_feats
    Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic, Eigen::RowMajor>,  // kept
    Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor>,  // ranks
    Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor>   // indices
> bev_pool_aux(
    const Tensor5D& geom_feats_input,
    const BEVFusionConfig & config)
{
  Eigen::Vector3f dx(config.xbound_[2], config.ybound_[2], config.zbound_[2]);
  Eigen::Vector3f bx(
    config.xbound_[0] + config.xbound_[2] / 2.0, config.ybound_[0] + config.ybound_[2] / 2.0,
    config.zbound_[0] + config.zbound_[2] / 2.0);
  Eigen::Vector3i nx(
    static_cast<int>((config.xbound_[1] - config.xbound_[0]) / config.xbound_[2]),
    static_cast<int>((config.ybound_[1] - config.ybound_[0]) / config.ybound_[2]),
    static_cast<int>((config.zbound_[1] - config.zbound_[0]) / config.zbound_[2]));

  // Get dimensions
  Eigen::Index N = geom_feats_input.dimension(0);
  Eigen::Index D = geom_feats_input.dimension(1);
  Eigen::Index H = geom_feats_input.dimension(2);
  Eigen::Index W = geom_feats_input.dimension(3);
  Eigen::Index C = geom_feats_input.dimension(4);
  assert(C == 3);

  Eigen::Index Nprime = N * D * H * W;

  TensorMap1D dx_map(dx.data(), 3);
  TensorMap1D bx_map(bx.data(), 3);

  Tensor5D dx_broadcast = dx_map.reshape(Eigen::array<Eigen::Index, 5>{1, 1, 1, 1, C})
                            .broadcast(Eigen::array<Eigen::Index, 5>{N, D, H, W, 1});

  Tensor5D bx_broadcast = bx_map.reshape(Eigen::array<Eigen::Index, 5>{1, 1, 1, 1, 3})
                            .broadcast(Eigen::array<Eigen::Index, 5>{N, D, H, W, 1});

  Tensor5D geom_feats_aux = (geom_feats_input - (bx_broadcast - 0.5 * dx_broadcast)) / dx_broadcast;

  Eigen::Map<const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>> geom_feats_map(
    geom_feats_aux.data(), Nprime, C);
  Eigen::Matrix<std::int32_t, Eigen::Dynamic, 3, Eigen::RowMajor> geom_feats_int =
    geom_feats_map.cast<std::int32_t>();

  // Concatenate geom_feats_int and batch_ix along column
  Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor> geom_feats_cat(Nprime, 4);
  geom_feats_cat.block(0, 0, Nprime, C) = geom_feats_int;
  geom_feats_cat.col(3).setZero();

  // Filter out points outside box
  Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic, Eigen::RowMajor> kept(Nprime);
  for (Eigen::Index i = 0; i < Nprime; ++i) {
    int32_t x = geom_feats_cat(i, 0);
    int32_t y = geom_feats_cat(i, 1);
    int32_t z = geom_feats_cat(i, 2);

    bool keep = (x >= 0) && (x < nx(0)) && (y >= 0) && (y < nx(1)) && (z >= 0) && (z < nx(2));
    kept(i) = keep;
  }

  // Collect indices where kept is true
  std::vector<std::int64_t> kept_indices;
  for (Eigen::Index i = 0; i < Nprime; ++i) {
    if (kept(i)) {
      kept_indices.push_back(i);
    }
  }

  // Create geom_feats_kept
  Eigen::Index N_kept = kept_indices.size();
  Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor> geom_feats_kept(N_kept, 4);
  for (Eigen::Index i = 0; i < N_kept; ++i) {
    std::int64_t idx = kept_indices[i];
    geom_feats_kept.row(i) = geom_feats_cat.row(idx);
  }

  // Compute ranks
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor> ranks(N_kept);
  std::int64_t factor0 = static_cast<std::int64_t>(W) * D;
  std::int64_t factor1 = static_cast<std::int64_t>(D);
  std::int64_t factor2 = 1;

  for (Eigen::Index i = 0; i < N_kept; ++i) {
    std::int32_t x = geom_feats_kept(i, 0);
    std::int32_t y = geom_feats_kept(i, 1);
    std::int32_t z = geom_feats_kept(i, 2);
    std::int64_t batch = geom_feats_kept(i, 3);

    std::int64_t rank = x * factor0 + y * factor1 + z * factor2 + batch;
    ranks(i) = rank;
  }

  //  Sort ranks and get indices
  std::vector<std::pair<std::int64_t, std::int64_t>> rank_idx_pairs(N_kept);
  for (Eigen::Index i = 0; i < N_kept; ++i) {
    rank_idx_pairs[i] = std::make_pair(ranks(i), i);
  }
  std::sort(rank_idx_pairs.begin(), rank_idx_pairs.end());

  //  Create sorted ranks and indices
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor> ranks_sorted(N_kept);
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor> indices(N_kept);
  Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor> geom_feats_sorted(N_kept, 4);
  for (Eigen::Index i = 0; i < N_kept; ++i) {
    std::int64_t rank = rank_idx_pairs[i].first;
    std::int64_t idx = rank_idx_pairs[i].second;

    ranks_sorted(i) = rank;
    indices(i) = idx;
    geom_feats_sorted.row(i) = geom_feats_kept.row(idx);
  }

  //  Return geom_feats_sorted, kept, ranks_sorted, indices
  return std::make_tuple(geom_feats_sorted, kept, ranks_sorted, indices);
}

std::tuple<
  Eigen::VectorXf, Eigen::Matrix<std::int32_t, Eigen::Dynamic, 4, Eigen::RowMajor>,
  Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic, Eigen::RowMajor>,
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor>,
  Eigen::Matrix<std::int64_t, 1, Eigen::Dynamic, Eigen::RowMajor>>
precompute_features(
  const std::vector<Matrix4fRowM> & lidar2camera_transforms,
  const std::vector<Matrix4fRowM> & camera_aug_matrices,
  const std::vector<sensor_msgs::msg::CameraInfo> & camera_info_vector,
  const BEVFusionConfig & config)
{
  Eigen::VectorXf lidar2images_flat(config.num_cameras_ * 4 * 4);

  Tensor4D frustum = autoware::lidar_bevfusion::create_frustum(config);

  Tensor3D camera2lidar_rotations(config.num_cameras_, 3, 3);
  Tensor2D camera2lidar_translations(config.num_cameras_, 3);
  Tensor3D intrins_inverse(config.num_cameras_, 3, 3);
  Tensor3D post_rots_inverse(config.num_cameras_, 3, 3);
  Tensor2D post_trans(config.num_cameras_, 3);

  for (std::int64_t camera_id = 0; camera_id < config.num_cameras_; camera_id++) {
    Matrix4fRowM cam2image = Matrix4fRowM::Identity();
    cam2image(0, 0) = camera_info_vector[camera_id].p[0];
    cam2image(1, 1) = camera_info_vector[camera_id].p[5];
    cam2image(0, 2) = camera_info_vector[camera_id].p[2];
    cam2image(1, 2) = camera_info_vector[camera_id].p[6];

    Matrix4fRowM img_aug_matrix = camera_aug_matrices[camera_id];
    Matrix4fRowM img_aug_matrix_inverse = img_aug_matrix.inverse().eval();

    Matrix4fRowM lidar2camera = lidar2camera_transforms[camera_id];
    Matrix4fRowM camera2lidar = lidar2camera.inverse().eval();

    Matrix4fRowM lidar2image = cam2image * lidar2camera;
    Eigen::VectorXf lidar2image_flat(Eigen::Map<Eigen::VectorXf>(lidar2image.data(), 16));
    lidar2images_flat.segment(16 * camera_id, 16) = lidar2image_flat;

    Matrix3fRowM camera2lidar_rot = camera2lidar.block<3, 3>(0, 0);
    Vector3fRowM camera2lidar_trans = camera2lidar.block<3, 1>(0, 3);
    Matrix3fRowM intrins_inv = cam2image.inverse().eval().block<3, 3>(0, 0);
    Matrix3fRowM post_rots_inv = img_aug_matrix_inverse.block<3, 3>(0, 0);
    Vector3fRowM post_trans_vec = img_aug_matrix.block<3, 1>(0, 3);

    TensorMap2D camera2lidar_rot_tensor(camera2lidar_rot.data(), 3, 3);
    TensorMap1D camera2lidar_trans_tensor(camera2lidar_trans.data(), 3);
    TensorMap2D intrins_inv_tensor(intrins_inv.data(), 3, 3);
    TensorMap2D post_rots_inv_tensor(post_rots_inv.data(), 3, 3);
    TensorMap1D post_trans_tensor(post_trans_vec.data(), 3);

    camera2lidar_rotations.chip(camera_id, 0) = camera2lidar_rot_tensor;
    camera2lidar_translations.chip(camera_id, 0) = camera2lidar_trans_tensor;
    intrins_inverse.chip(camera_id, 0) = intrins_inv_tensor;
    post_rots_inverse.chip(camera_id, 0) = post_rots_inv_tensor;
    post_trans.chip(camera_id, 0) = post_trans_tensor;
  }

  Tensor5D geometry = get_geometry(
    frustum,                    // [D, H, W, 3]
    camera2lidar_rotations,     // [N, 3, 3]
    camera2lidar_translations,  // [N, 3]
    intrins_inverse,            // [N, 3, 3]
    post_rots_inverse,          // [N, 3, 3]
    post_trans                  // [N, 3]
  );

  auto [geom_feats, kept, ranks, indices] = bev_pool_aux(geometry, config);

  return std::make_tuple(lidar2images_flat, geom_feats, kept, ranks, indices);
}

}  // namespace autoware::lidar_bevfusion

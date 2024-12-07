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

#include "autoware/ndt_scan_matcher/ndt_omp/estimate_covariance.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <memory>
#include <utility>
#include <vector>

namespace pclomp
{

Eigen::Matrix2d estimate_xy_covariance_by_laplace_approximation(
  const Eigen::Matrix<double, 6, 6> & hessian)
{
  const Eigen::Matrix2d hessian_xy = hessian.block<2, 2>(0, 0);
  Eigen::Matrix2d covariance_xy = -hessian_xy.inverse();
  return covariance_xy;
}

ResultOfMultiNdtCovarianceEstimation estimate_xy_covariance_by_multi_ndt(
  const NdtResult & ndt_result,
  const std::shared_ptr<
    pclomp::MultiGridNormalDistributionsTransform<pcl::PointXYZ, pcl::PointXYZ>> & ndt_ptr,
  const std::vector<Eigen::Matrix4f> & poses_to_search)
{
  // initialize by the main result
  const Eigen::Vector2d ndt_pose_2d(ndt_result.pose(0, 3), ndt_result.pose(1, 3));
  std::vector<Eigen::Vector2d> ndt_pose_2d_vec{ndt_pose_2d};

  // multiple searches
  std::vector<NdtResult> ndt_results;
  for (const Eigen::Matrix4f & curr_pose : poses_to_search) {
    auto sub_output_cloud = std::make_shared<pcl::PointCloud<pcl::PointXYZ>>();
    ndt_ptr->align(*sub_output_cloud, curr_pose);
    const NdtResult sub_ndt_result = ndt_ptr->getResult();
    ndt_results.push_back(sub_ndt_result);

    const Eigen::Matrix4f sub_ndt_pose = sub_ndt_result.pose;
    const Eigen::Vector2d sub_ndt_pose_2d = sub_ndt_pose.topRightCorner<2, 1>().cast<double>();
    ndt_pose_2d_vec.emplace_back(sub_ndt_pose_2d);
  }

  // calculate the weights
  const int n = static_cast<int>(ndt_results.size()) + 1;
  const std::vector<double> weight_vec(n, 1.0 / n);

  // calculate mean and covariance
  auto [mean, covariance] = calculate_weighted_mean_and_cov(ndt_pose_2d_vec, weight_vec);

  // unbiased covariance
  covariance *= static_cast<double>(n - 1) / n;

  return {mean, covariance, poses_to_search, ndt_results};
}

ResultOfMultiNdtCovarianceEstimation estimate_xy_covariance_by_multi_ndt_score(
  const NdtResult & ndt_result,
  const std::shared_ptr<
    pclomp::MultiGridNormalDistributionsTransform<pcl::PointXYZ, pcl::PointXYZ>> & ndt_ptr,
  const std::vector<Eigen::Matrix4f> & poses_to_search, const double temperature)
{
  // initialize by the main result
  const Eigen::Vector2d ndt_pose_2d(ndt_result.pose(0, 3), ndt_result.pose(1, 3));
  std::vector<Eigen::Vector2d> ndt_pose_2d_vec{ndt_pose_2d};
  std::vector<double> score_vec{ndt_result.nearest_voxel_transformation_likelihood};

  const pcl::PointCloud<pcl::PointXYZ>::ConstPtr input_cloud = ndt_ptr->getInputCloud();
  pcl::PointCloud<pcl::PointXYZ> trans_cloud;

  // multiple searches
  std::vector<NdtResult> ndt_results;
  for (const Eigen::Matrix4f & curr_pose : poses_to_search) {
    const Eigen::Vector2d sub_ndt_pose_2d = curr_pose.topRightCorner<2, 1>().cast<double>();
    ndt_pose_2d_vec.emplace_back(sub_ndt_pose_2d);

    transformPointCloud(*input_cloud, trans_cloud, curr_pose);
    const double nvtl = ndt_ptr->calculateNearestVoxelTransformationLikelihood(trans_cloud);
    score_vec.emplace_back(nvtl);

    NdtResult sub_ndt_result{};
    sub_ndt_result.pose = curr_pose;
    sub_ndt_result.iteration_num = 0;
    sub_ndt_result.nearest_voxel_transformation_likelihood = static_cast<float>(nvtl);
    ndt_results.push_back(sub_ndt_result);
  }

  // calculate the weights
  const std::vector<double> weight_vec = calc_weight_vec(score_vec, temperature);

  // calculate mean and covariance
  const auto [mean, covariance] = calculate_weighted_mean_and_cov(ndt_pose_2d_vec, weight_vec);
  return {mean, covariance, poses_to_search, ndt_results};
}

std::vector<Eigen::Matrix4f> propose_poses_to_search(
  const NdtResult & ndt_result, const std::vector<double> & offset_x,
  const std::vector<double> & offset_y)
{
  assert(offset_x.size() == offset_y.size());
  const Eigen::Matrix4f & center_pose = ndt_result.pose;
  const Eigen::Matrix2d rot = ndt_result.pose.topLeftCorner<2, 2>().cast<double>();
  std::vector<Eigen::Matrix4f> poses_to_search;
  for (int i = 0; i < static_cast<int>(offset_x.size()); i++) {
    const Eigen::Vector2d pose_offset(offset_x[i], offset_y[i]);
    const Eigen::Vector2d rotated_pose_offset_2d = rot * pose_offset;
    Eigen::Matrix4f curr_pose = center_pose;
    curr_pose(0, 3) += static_cast<float>(rotated_pose_offset_2d.x());
    curr_pose(1, 3) += static_cast<float>(rotated_pose_offset_2d.y());
    poses_to_search.emplace_back(curr_pose);
  }
  return poses_to_search;
}

std::vector<double> calc_weight_vec(const std::vector<double> & score_vec, double temperature)
{
  const int n = static_cast<int>(score_vec.size());
  const double max_score = *std::max_element(score_vec.begin(), score_vec.end());
  std::vector<double> exp_score_vec(n);
  double exp_score_sum = 0.0;
  for (int i = 0; i < n; i++) {
    exp_score_vec[i] = std::exp((score_vec[i] - max_score) / temperature);
    exp_score_sum += exp_score_vec[i];
  }
  for (int i = 0; i < n; i++) {
    exp_score_vec[i] /= exp_score_sum;
  }
  return exp_score_vec;
}

std::pair<Eigen::Vector2d, Eigen::Matrix2d> calculate_weighted_mean_and_cov(
  const std::vector<Eigen::Vector2d> & pose_2d_vec, const std::vector<double> & weight_vec)
{
  const int n = static_cast<int>(pose_2d_vec.size());
  Eigen::Vector2d mean = Eigen::Vector2d::Zero();
  for (int i = 0; i < n; i++) {
    mean += weight_vec[i] * pose_2d_vec[i];
  }
  Eigen::Matrix2d covariance = Eigen::Matrix2d::Zero();
  for (int i = 0; i < n; i++) {
    const Eigen::Vector2d diff = pose_2d_vec[i] - mean;
    covariance += weight_vec[i] * diff * diff.transpose();
  }
  return {mean, covariance};
}

Eigen::Matrix2d rotate_covariance_to_base_link(
  const Eigen::Matrix2d & covariance, const Eigen::Matrix4f & pose)
{
  const Eigen::Matrix2d rot = pose.topLeftCorner<2, 2>().cast<double>();
  return rot.transpose() * covariance * rot;
}

Eigen::Matrix2d rotate_covariance_to_map(
  const Eigen::Matrix2d & covariance, const Eigen::Matrix4f & pose)
{
  const Eigen::Matrix2d rot = pose.topLeftCorner<2, 2>().cast<double>();
  return rot * covariance * rot.transpose();
}

Eigen::Matrix2d adjust_diagonal_covariance(
  const Eigen::Matrix2d & covariance, const Eigen::Matrix4f & pose, const double fixed_cov00,
  const double fixed_cov11)
{
  Eigen::Matrix2d cov_base_link = rotate_covariance_to_base_link(covariance, pose);
  cov_base_link(0, 0) = std::max(cov_base_link(0, 0), fixed_cov00);
  cov_base_link(1, 1) = std::max(cov_base_link(1, 1), fixed_cov11);
  Eigen::Matrix2d cov_map = rotate_covariance_to_map(cov_base_link, pose);
  return cov_map;
}

}  // namespace pclomp

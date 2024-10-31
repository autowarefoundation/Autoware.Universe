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

/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2011, Willow Garage, Inc.
 *  Copyright (c) 2012-, Open Perception, Inc.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the copyright holder(s) nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 *
 */

#ifndef NDT_OMP__MULTIGRID_NDT_OMP_IMPL_HPP_
#define NDT_OMP__MULTIGRID_NDT_OMP_IMPL_HPP_

// cspell:ignore multigrid, nnvn, colj

#include "autoware/ndt_scan_matcher/ndt_omp/multigrid_ndt_omp.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace pclomp
{

template <typename PointSource, typename PointTarget>
MultiGridNormalDistributionsTransform<PointSource, PointTarget>::
  MultiGridNormalDistributionsTransform(const MultiGridNormalDistributionsTransform & other)
: BaseRegType(other), target_cells_(other.target_cells_)
{
  params_ = other.params_;
  outlier_ratio_ = other.outlier_ratio_;
  gauss_d1_ = other.gauss_d1_;
  gauss_d2_ = other.gauss_d2_;
  gauss_d3_ = other.gauss_d3_;
  trans_probability_ = other.trans_probability_;
  // No need to copy j_ang_ and h_ang_, as those matrices are re-computed on every
  // computeDerivatives() call

  hessian_ = other.hessian_;
  transformation_array_ = other.transformation_array_;
  transform_probability_array_ = other.transform_probability_array_;
  nearest_voxel_transformation_likelihood_array_ =
    other.nearest_voxel_transformation_likelihood_array_;
  nearest_voxel_transformation_likelihood_ = other.nearest_voxel_transformation_likelihood_;

  regularization_pose_ = other.regularization_pose_;
  regularization_pose_translation_ = other.regularization_pose_translation_;
}

template <typename PointSource, typename PointTarget>
MultiGridNormalDistributionsTransform<PointSource, PointTarget>::
  MultiGridNormalDistributionsTransform(MultiGridNormalDistributionsTransform && other) noexcept
: BaseRegType(std::move(other)),
  target_cells_(std::move(other.target_cells_)),
  params_(std::move(other.params_))
{
  outlier_ratio_ = other.outlier_ratio_;
  gauss_d1_ = other.gauss_d1_;
  gauss_d2_ = other.gauss_d2_;
  gauss_d3_ = other.gauss_d3_;
  trans_probability_ = other.trans_probability_;

  hessian_ = other.hessian_;
  transformation_array_ = other.transformation_array_;
  transform_probability_array_ = other.transform_probability_array_;
  nearest_voxel_transformation_likelihood_array_ =
    other.nearest_voxel_transformation_likelihood_array_;
  nearest_voxel_transformation_likelihood_ = other.nearest_voxel_transformation_likelihood_;

  regularization_pose_ = other.regularization_pose_;
  regularization_pose_translation_ = other.regularization_pose_translation_;
}

template <typename PointSource, typename PointTarget>
MultiGridNormalDistributionsTransform<PointSource, PointTarget> &
MultiGridNormalDistributionsTransform<PointSource, PointTarget>::operator=(
  const MultiGridNormalDistributionsTransform & other)
{
  target_cells_ = other.target_cells_;
  params_ = other.params_;

  outlier_ratio_ = other.outlier_ratio_;
  gauss_d1_ = other.gauss_d1_;
  gauss_d2_ = other.gauss_d2_;
  gauss_d3_ = other.gauss_d3_;
  trans_probability_ = other.trans_probability_;

  hessian_ = other.hessian_;
  transformation_array_ = other.transformation_array_;
  transform_probability_array_ = other.transform_probability_array_;
  nearest_voxel_transformation_likelihood_array_ =
    other.nearest_voxel_transformation_likelihood_array_;
  nearest_voxel_transformation_likelihood_ = other.nearest_voxel_transformation_likelihood_;

  regularization_pose_ = other.regularization_pose_;
  regularization_pose_translation_ = other.regularization_pose_translation_;

  BaseRegType::operator=(other);

  return *this;
}

template <typename PointSource, typename PointTarget>
MultiGridNormalDistributionsTransform<PointSource, PointTarget> &
MultiGridNormalDistributionsTransform<PointSource, PointTarget>::operator=(
  MultiGridNormalDistributionsTransform && other) noexcept
{
  target_cells_ = std::move(other.target_cells_);
  params_ = std::move(other.params_);

  outlier_ratio_ = other.outlier_ratio_;
  gauss_d1_ = other.gauss_d1_;
  gauss_d2_ = other.gauss_d2_;
  gauss_d3_ = other.gauss_d3_;
  trans_probability_ = other.trans_probability_;

  hessian_ = other.hessian_;
  transformation_array_ = other.transformation_array_;
  transform_probability_array_ = other.transform_probability_array_;
  nearest_voxel_transformation_likelihood_array_ =
    other.nearest_voxel_transformation_likelihood_array_;
  nearest_voxel_transformation_likelihood_ = other.nearest_voxel_transformation_likelihood_;

  regularization_pose_ = other.regularization_pose_;
  regularization_pose_translation_ = other.regularization_pose_translation_;

  BaseRegType::operator=(std::move(other));

  return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
MultiGridNormalDistributionsTransform<
  PointSource, PointTarget>::MultiGridNormalDistributionsTransform()
: target_cells_(),
  outlier_ratio_(0.55),
  gauss_d1_(),
  gauss_d2_(),
  gauss_d3_(),
  trans_probability_(),
  regularization_pose_(boost::none)
{
  reg_name_ = "MultiGridNormalDistributionsTransform";

  params_.trans_epsilon = 0.1;
  params_.step_size = 0.1;
  params_.resolution = 1.0f;
  params_.max_iterations = 35;
  params_.search_method = KDTREE;  // Only KDTREE is supported in multigrid_ndt_omp
  params_.num_threads = omp_get_max_threads();
  params_.regularization_scale_factor = 0.0f;
  params_.use_line_search = false;

  // Initializes the gaussian fitting parameters (eq. 6.8) [Magnusson 2009]
  double gauss_c1 = 10.0 * (1 - outlier_ratio_);
  double gauss_c2 = outlier_ratio_ / pow(params_.resolution, 3);
  gauss_d3_ = -log(gauss_c2);
  gauss_d1_ = -log(gauss_c1 + gauss_c2) - gauss_d3_;
  gauss_d2_ = -2 * log((-log(gauss_c1 * exp(-0.5) + gauss_c2) - gauss_d3_) / gauss_d1_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void MultiGridNormalDistributionsTransform<PointSource, PointTarget>::computeTransformation(
  PointCloudSource & output, const Eigen::Matrix4f & guess)
{
  nr_iterations_ = 0;
  converged_ = false;

  // Initializes the gaussian fitting parameters (eq. 6.8) [Magnusson 2009]
  double gauss_c1 = 10 * (1 - outlier_ratio_);
  double gauss_c2 = outlier_ratio_ / pow(params_.resolution, 3);
  gauss_d3_ = -log(gauss_c2);
  gauss_d1_ = -log(gauss_c1 + gauss_c2) - gauss_d3_;
  gauss_d2_ = -2.0 * log((-log(gauss_c1 * exp(-0.5) + gauss_c2) - gauss_d3_) / gauss_d1_);

  if (guess != Eigen::Matrix4f::Identity()) {
    // Initialise final transformation to the guessed one
    final_transformation_ = guess;
    // Apply guessed transformation prior to search for neighbours
    transformPointCloud(output, output, guess);
  }

  Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> eig_transformation;

  eig_transformation.matrix() = final_transformation_;
  transformation_array_.clear();
  transformation_array_.push_back(final_transformation_);

  transform_probability_array_.clear();
  nearest_voxel_transformation_likelihood_array_.clear();

  // Convert initial guess matrix to 6 element transformation vector
  Eigen::Matrix<double, 6, 1> p;
  Eigen::Matrix<double, 6, 1> delta_p;
  Eigen::Matrix<double, 6, 1> score_gradient;
  Eigen::Vector3f init_translation = eig_transformation.translation();
  Eigen::Vector3f init_rotation = eig_transformation.rotation().eulerAngles(0, 1, 2);

  p << init_translation(0), init_translation(1), init_translation(2), init_rotation(0),
    init_rotation(1), init_rotation(2);

  Eigen::Matrix<double, 6, 6> hessian;

  double score = 0;

  if (regularization_pose_) {
    Eigen::Transform<float, 3, Eigen::Affine, Eigen::ColMajor> regularization_pose_transformation;
    regularization_pose_transformation.matrix() = regularization_pose_.get();
    regularization_pose_translation_ = regularization_pose_transformation.translation();
  }

  // Calculate derivatives of initial transform vector, subsequent derivative calculations are done
  // in the step length determination.
  score = computeDerivatives(score_gradient, hessian, output, p);

  while (!converged_) {
    // Store previous transformation
    previous_transformation_ = transformation_;

    // Solve for decent direction using newton method, line 23 in Algorithm 2 [Magnusson 2009]
    Eigen::JacobiSVD<Eigen::Matrix<double, 6, 6>> sv(
      hessian, Eigen::ComputeFullU | Eigen::ComputeFullV);
    // Negative for maximization as opposed to minimization
    delta_p = sv.solve(-score_gradient);

    // Calculate step length with guaranteed sufficient decrease [More, Thuente 1994]
    double delta_p_norm = delta_p.norm();

    if (delta_p_norm == 0 || delta_p_norm != delta_p_norm) {
      if (input_->empty()) {
        trans_probability_ = 0.0f;
      } else {
        trans_probability_ = score / static_cast<double>(input_->size());
      }

      converged_ = delta_p_norm == delta_p_norm;
      return;
    }

    delta_p.normalize();
    delta_p_norm = computeStepLengthMT(
      p, delta_p, delta_p_norm, params_.step_size, params_.trans_epsilon / 2.0, score,
      score_gradient, hessian, output);
    delta_p *= delta_p_norm;

    transformation_ =
      (Eigen::Translation<float, 3>(
         static_cast<float>(delta_p(0)), static_cast<float>(delta_p(1)),
         static_cast<float>(delta_p(2))) *
       Eigen::AngleAxis<float>(static_cast<float>(delta_p(3)), Eigen::Vector3f::UnitX()) *
       Eigen::AngleAxis<float>(static_cast<float>(delta_p(4)), Eigen::Vector3f::UnitY()) *
       Eigen::AngleAxis<float>(static_cast<float>(delta_p(5)), Eigen::Vector3f::UnitZ()))
        .matrix();

    transformation_array_.push_back(final_transformation_);

    p = p + delta_p;

    // Update Visualizer (untested)
    if (update_visualizer_ != 0)
      update_visualizer_(output, std::vector<int>(), *target_, std::vector<int>());

    nr_iterations_++;

    if (
      nr_iterations_ >= params_.max_iterations ||
      (nr_iterations_ && (std::fabs(delta_p_norm) < params_.trans_epsilon))) {
      converged_ = true;
    }
  }

  // Store transformation probability. The relative differences within each scan registration are
  // accurate but the normalization constants need to be modified for it to be globally accurate
  if (input_->empty()) {
    trans_probability_ = 0.0f;
  } else {
    trans_probability_ = score / static_cast<double>(input_->size());
  }

  hessian_ = hessian;
}

#ifndef _OPENMP
int omp_get_max_threads()
{
  return 1;
}
int omp_get_thread_num()
{
  return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
double MultiGridNormalDistributionsTransform<PointSource, PointTarget>::computeDerivatives(
  Eigen::Matrix<double, 6, 1> & score_gradient, Eigen::Matrix<double, 6, 6> & hessian,
  PointCloudSource & trans_cloud, Eigen::Matrix<double, 6, 1> & p, bool compute_hessian)
{
  score_gradient.setZero();
  hessian.setZero();

  double score = 0;
  int total_neighborhood_count = 0;
  double nearest_voxel_score = 0;
  size_t found_neighborhood_voxel_num = 0;

  std::vector<double> scores(params_.num_threads);
  std::vector<double> nearest_voxel_scores(params_.num_threads);
  std::vector<size_t> found_neighborhood_voxel_nums(params_.num_threads);
  std::vector<Eigen::Matrix<double, 6, 1>, Eigen::aligned_allocator<Eigen::Matrix<double, 6, 1>>>
    score_gradients(params_.num_threads);
  std::vector<Eigen::Matrix<double, 6, 6>, Eigen::aligned_allocator<Eigen::Matrix<double, 6, 6>>>
    hessians(params_.num_threads);
  std::vector<int> neighborhood_counts(params_.num_threads);

  // Pre-allocate thread-wise point derivative matrices to avoid reallocate too many times
  std::vector<Eigen::Matrix<double, 4, 6>> t_point_gradients(params_.num_threads);
  std::vector<Eigen::Matrix<double, 24, 6>> t_point_hessians(params_.num_threads);

  for (int i = 0; i < params_.num_threads; ++i) {
    scores[i] = 0;
    nearest_voxel_scores[i] = 0;
    found_neighborhood_voxel_nums[i] = 0;
    score_gradients[i].setZero();
    hessians[i].setZero();
    neighborhood_counts[i] = 0;

    // Initialize point derivatives
    t_point_gradients[i].setZero();
    t_point_gradients[i].block<3, 3>(0, 0).setIdentity();
    t_point_hessians[i].setZero();
  }

  // Precompute Angular Derivatives (eq. 6.19 and 6.21)[Magnusson 2009]
  computeAngleDerivatives(p);

  // Update gradient and hessian for each point, line 17 in Algorithm 2 [Magnusson 2009]
#pragma omp parallel for num_threads(params_.num_threads) schedule(guided, 8)
  for (size_t idx = 0; idx < input_->size(); ++idx) {
    int tid = omp_get_thread_num();
    // Searching for neighbors of the current transformed point
    auto & x_trans_pt = trans_cloud[idx];
    std::vector<TargetGridLeafConstPtr> neighborhood;

    // Neighborhood search method other than kdtree is disabled in multigrid_ndt_omp
    target_cells_.radiusSearch(x_trans_pt, params_.resolution, neighborhood);

    if (neighborhood.empty()) {
      continue;
    }

    // Original Point
    auto & x_pt = (*input_)[idx];
    // Original Point and Transformed Point (for math)
    Eigen::Vector3d x(x_pt.x, x_pt.y, x_pt.z);
    // Current Point Gradient and Hessian
    auto & point_gradient = t_point_gradients[tid];
    auto & point_hessian = t_point_hessians[tid];

    // Compute derivative of transform function w.r.t. transform vector, J_E and H_E in
    // Equations 6.18 and 6.20 [Magnusson 2009]
    computePointDerivatives(x, point_gradient, point_hessian);

    // Denorm point, x_k' in Equations 6.12 and 6.13 [Magnusson 2009]
    const Eigen::Vector3d x_trans(x_trans_pt.x, x_trans_pt.y, x_trans_pt.z);

    double sum_score_pt = 0;
    double nearest_voxel_score_pt = 0;
    auto & score_gradient_pt = score_gradients[tid];
    auto & hessian_pt = hessians[tid];

    for (auto & cell : neighborhood) {
      // Update score, gradient and hessian, lines 19-21 in Algorithm 2, according to
      // Equations 6.10, 6.12 and 6.13, respectively [Magnusson 2009]
      double score_pt = updateDerivatives(
        score_gradient_pt, hessian_pt, point_gradient, point_hessian, x_trans - cell->getMean(),
        cell->getInverseCov(), compute_hessian);
      sum_score_pt += score_pt;

      if (score_pt > nearest_voxel_score_pt) {
        nearest_voxel_score_pt = score_pt;
      }
    }

    ++found_neighborhood_voxel_nums[tid];

    scores[tid] += sum_score_pt;
    nearest_voxel_scores[tid] += nearest_voxel_score_pt;
    neighborhood_counts[tid] += neighborhood.size();
  }

  // Ensure that the result is invariant against the summing up order
  for (int i = 0; i < params_.num_threads; ++i) {
    score += scores[i];
    nearest_voxel_score += nearest_voxel_scores[i];
    found_neighborhood_voxel_num += found_neighborhood_voxel_nums[i];
    score_gradient += score_gradients[i];
    hessian += hessians[i];
    total_neighborhood_count += neighborhood_counts[i];
  }

  if (regularization_pose_) {
    float regularization_score = 0.0f;
    Eigen::Matrix<double, 6, 1> regularization_gradient = Eigen::Matrix<double, 6, 1>::Zero();
    Eigen::Matrix<double, 6, 6> regularization_hessian = Eigen::Matrix<double, 6, 6>::Zero();

    const float dx = regularization_pose_translation_(0) - static_cast<float>(p(0, 0));
    const float dy = regularization_pose_translation_(1) - static_cast<float>(p(1, 0));
    const auto sin_yaw = static_cast<float>(sin(p(5, 0)));
    const auto cos_yaw = static_cast<float>(cos(p(5, 0)));
    const float longitudinal_distance = dy * sin_yaw + dx * cos_yaw;
    const auto neighborhood_count_weight = static_cast<float>(total_neighborhood_count);

    regularization_score = -params_.regularization_scale_factor * neighborhood_count_weight *
                           longitudinal_distance * longitudinal_distance;

    regularization_gradient(0, 0) = params_.regularization_scale_factor *
                                    neighborhood_count_weight * 2.0f * cos_yaw *
                                    longitudinal_distance;
    regularization_gradient(1, 0) = params_.regularization_scale_factor *
                                    neighborhood_count_weight * 2.0f * sin_yaw *
                                    longitudinal_distance;

    regularization_hessian(0, 0) =
      -params_.regularization_scale_factor * neighborhood_count_weight * 2.0f * cos_yaw * cos_yaw;
    regularization_hessian(0, 1) =
      -params_.regularization_scale_factor * neighborhood_count_weight * 2.0f * cos_yaw * sin_yaw;
    regularization_hessian(1, 1) =
      -params_.regularization_scale_factor * neighborhood_count_weight * 2.0f * sin_yaw * sin_yaw;
    regularization_hessian(1, 0) = regularization_hessian(0, 1);

    score += regularization_score;
    score_gradient += regularization_gradient;
    hessian += regularization_hessian;
  }

  if (found_neighborhood_voxel_num != 0) {
    nearest_voxel_transformation_likelihood_ =
      nearest_voxel_score / static_cast<double>(found_neighborhood_voxel_num);
  } else {
    nearest_voxel_transformation_likelihood_ = 0.0;
  }
  nearest_voxel_transformation_likelihood_array_.push_back(
    nearest_voxel_transformation_likelihood_);
  const float transform_probability =
    (input_->points.empty() ? 0.0f : score / static_cast<double>(input_->points.size()));
  transform_probability_array_.push_back(transform_probability);
  return (score);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void MultiGridNormalDistributionsTransform<PointSource, PointTarget>::computeAngleDerivatives(
  Eigen::Matrix<double, 6, 1> & p, bool compute_hessian)
{
  // Simplified math for near 0 angles
  double cx = 1.0;
  double cy = 1.0;
  double cz = 1.0;
  double sx = 0.0;
  double sy = 0.0;
  double sz = 0.0;
  if (fabs(p(3)) < 10e-5) {
    // p(3) = 0;
    cx = 1.0;
    sx = 0.0;
  } else {
    cx = cos(p(3));
    sx = sin(p(3));
  }
  if (fabs(p(4)) < 10e-5) {
    // p(4) = 0;
    cy = 1.0;
    sy = 0.0;
  } else {
    cy = cos(p(4));
    sy = sin(p(4));
  }

  if (fabs(p(5)) < 10e-5) {
    // p(5) = 0;
    cz = 1.0;
    sz = 0.0;
  } else {
    cz = cos(p(5));
    sz = sin(p(5));
  }

  // Precomputed angular gradient components. Letters correspond to Equation 6.19 [Magnusson 2009]
  j_ang_.setZero();

  j_ang_.row(0) << (-sx * sz + cx * sy * cz), (-sx * cz - cx * sy * sz), (-cx * cy), 0.0f;
  j_ang_.row(1) << (cx * sz + sx * sy * cz), (cx * cz - sx * sy * sz), (-sx * cy), 0.0f;
  j_ang_.row(2) << (-sy * cz), sy * sz, cy, 0.0f;
  j_ang_.row(3) << sx * cy * cz, (-sx * cy * sz), sx * sy, 0.0f;
  j_ang_.row(4) << (-cx * cy * cz), cx * cy * sz, (-cx * sy), 0.0f;
  j_ang_.row(5) << (-cy * sz), (-cy * cz), 0.0f, 0.0f;
  j_ang_.row(6) << (cx * cz - sx * sy * sz), (-cx * sz - sx * sy * cz), 0.0f, 0.0f;
  j_ang_.row(7) << (sx * cz + cx * sy * sz), (cx * sy * cz - sx * sz), 0.0f, 0.0f;

  if (compute_hessian) {
    // Precomputed angular hessian components. Letters correspond to Equation 6.21 and numbers
    // correspond to row index [Magnusson 2009]
    h_ang_.setZero();

    h_ang_.row(0) << (-cx * sz - sx * sy * cz), (-cx * cz + sx * sy * sz), sx * cy, 0.0f;     // a2
    h_ang_.row(1) << (-sx * sz + cx * sy * cz), (-cx * sy * sz - sx * cz), (-cx * cy), 0.0f;  // a3

    h_ang_.row(2) << (cx * cy * cz), (-cx * cy * sz), (cx * sy), 0.0f;  // b2
    h_ang_.row(3) << (sx * cy * cz), (-sx * cy * sz), (sx * sy), 0.0f;  // b3

    h_ang_.row(4) << (-sx * cz - cx * sy * sz), (sx * sz - cx * sy * cz), 0.0f, 0.0f;  // c2
    h_ang_.row(5) << (cx * cz - sx * sy * sz), (-sx * sy * cz - cx * sz), 0.0f, 0.0f;  // c3

    h_ang_.row(6) << (-cy * cz), (cy * sz), (sy), 0.0f;                  // d1
    h_ang_.row(7) << (-sx * sy * cz), (sx * sy * sz), (sx * cy), 0.0f;   // d2
    h_ang_.row(8) << (cx * sy * cz), (-cx * sy * sz), (-cx * cy), 0.0f;  // d3

    h_ang_.row(9) << (sy * sz), (sy * cz), 0.0f, 0.0f;               // e1
    h_ang_.row(10) << (-sx * cy * sz), (-sx * cy * cz), 0.0f, 0.0f;  // e2
    h_ang_.row(11) << (cx * cy * sz), (cx * cy * cz), 0.0f, 0.0f;    // e3

    h_ang_.row(12) << (-cy * cz), (cy * sz), 0.0f, 0.0f;                                 // f1
    h_ang_.row(13) << (-cx * sz - sx * sy * cz), (-cx * cz + sx * sy * sz), 0.0f, 0.0f;  // f2
    h_ang_.row(14) << (-sx * sz + cx * sy * cz), (-cx * sy * sz - sx * cz), 0.0f, 0.0f;  // f3
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void MultiGridNormalDistributionsTransform<PointSource, PointTarget>::computePointDerivatives(
  Eigen::Vector3d & x, Eigen::Matrix<double, 4, 6> & point_gradient,
  Eigen::Matrix<double, 24, 6> & point_hessian, bool compute_hessian) const
{
  Eigen::Vector4d x4(x[0], x[1], x[2], 0.0f);

  // Calculate first derivative of Transformation Equation 6.17 w.r.t. transform vector p.
  // Derivative w.r.t. ith element of transform vector corresponds to column i, Equation 6.18
  // and 6.19 [Magnusson 2009]
  auto x_j_ang = j_ang_ * x4;

  point_gradient(1, 3) = x_j_ang[0];
  point_gradient(2, 3) = x_j_ang[1];
  point_gradient(0, 4) = x_j_ang[2];
  point_gradient(1, 4) = x_j_ang[3];
  point_gradient(2, 4) = x_j_ang[4];
  point_gradient(0, 5) = x_j_ang[5];
  point_gradient(1, 5) = x_j_ang[6];
  point_gradient(2, 5) = x_j_ang[7];

  if (compute_hessian) {
    auto x_h_ang = h_ang_ * x4;

    // Vectors from Equation 6.21 [Magnusson 2009]
    Eigen::Vector4d a(0.0f, x_h_ang[0], x_h_ang[1], 0.0f);
    Eigen::Vector4d b(0.0f, x_h_ang[2], x_h_ang[3], 0.0f);
    Eigen::Vector4d c(0.0f, x_h_ang[4], x_h_ang[5], 0.0f);
    Eigen::Vector4d d(x_h_ang[6], x_h_ang[7], x_h_ang[8], 0.0f);
    Eigen::Vector4d e(x_h_ang[9], x_h_ang[10], x_h_ang[11], 0.0f);
    Eigen::Vector4d f(x_h_ang[12], x_h_ang[13], x_h_ang[14], 0.0f);

    // Calculate second derivative of Transformation Equation 6.17 w.r.t. transform vector p.
    // Derivative w.r.t. ith and jth elements of transform vector corresponds to the 3x1 block
    // matrix starting at (3i,j), Equation 6.20 and 6.21 [Magnusson 2009]
    point_hessian.block<4, 1>(12, 3) = a;
    point_hessian.block<4, 1>(16, 3) = b;
    point_hessian.block<4, 1>(20, 3) = c;
    point_hessian.block<4, 1>(12, 4) = b;
    point_hessian.block<4, 1>(16, 4) = d;
    point_hessian.block<4, 1>(20, 4) = e;
    point_hessian.block<4, 1>(12, 5) = c;
    point_hessian.block<4, 1>(16, 5) = e;
    point_hessian.block<4, 1>(20, 5) = f;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
double MultiGridNormalDistributionsTransform<PointSource, PointTarget>::updateDerivatives(
  Eigen::Matrix<double, 6, 1> & score_gradient, Eigen::Matrix<double, 6, 6> & hessian,
  const Eigen::Matrix<double, 4, 6> & point_gradient,
  const Eigen::Matrix<double, 24, 6> & point_hessian, const Eigen::Vector3d & x_trans,
  const Eigen::Matrix3d & c_inv, bool compute_hessian) const
{
  Eigen::Matrix<double, 1, 4> x_trans4(x_trans[0], x_trans[1], x_trans[2], 0.0f);
  Eigen::Matrix4d c_inv4 = Eigen::Matrix4d::Zero();

  c_inv4.topLeftCorner(3, 3) = c_inv;

  // e^(-d_2/2 * (x_k - mu_k)^T Sigma_k^-1 (x_k - mu_k)) Equation 6.9 [Magnusson 2009]
  double e_x_cov_x = exp(-gauss_d2_ * x_trans4.dot(x_trans4 * c_inv4) * 0.5f);
  // Calculate probability of transformed points existence, Equation 6.9 [Magnusson 2009]
  double score_inc = -gauss_d1_ * e_x_cov_x;

  e_x_cov_x = gauss_d2_ * e_x_cov_x;

  // Error checking for invalid values.
  if (e_x_cov_x > 1 || e_x_cov_x < 0 || e_x_cov_x != e_x_cov_x) return (0);

  // Reusable portion of Equation 6.12 and 6.13 [Magnusson 2009]
  e_x_cov_x *= gauss_d1_;

  Eigen::Matrix<double, 4, 6> c_inv4_x_point_gradient4 = c_inv4 * point_gradient;
  Eigen::Matrix<double, 6, 1> x_trans4_dot_c_inv4_x_point_gradient4 =
    x_trans4 * c_inv4_x_point_gradient4;

  score_gradient.noalias() += (e_x_cov_x * x_trans4_dot_c_inv4_x_point_gradient4).cast<double>();

  if (compute_hessian) {
    Eigen::Matrix<double, 1, 4> x_trans4_x_c_inv4 = x_trans4 * c_inv4;
    Eigen::Matrix<double, 6, 6> point_gradient4_colj_dot_c_inv4_x_point_gradient4_col_i =
      point_gradient.transpose() * c_inv4_x_point_gradient4;
    Eigen::Matrix<double, 6, 1> x_trans4_dot_c_inv4_x_ext_point_hessian_4ij;

    for (int i = 0; i < 6; ++i) {
      // Sigma_k^-1 d(T(x,p))/dpi, Reusable portion of Equation 6.12 and 6.13 [Magnusson 2009]
      // Update gradient, Equation 6.12 [Magnusson 2009]
      x_trans4_dot_c_inv4_x_ext_point_hessian_4ij.noalias() =
        x_trans4_x_c_inv4 * point_hessian.block<4, 6>(i * 4, 0);

      for (int j = 0; j < hessian.cols(); j++) {
        // Update hessian, Equation 6.13 [Magnusson 2009]
        hessian(i, j) +=
          e_x_cov_x * (-gauss_d2_ * x_trans4_dot_c_inv4_x_point_gradient4(i) *
                         x_trans4_dot_c_inv4_x_point_gradient4(j) +
                       x_trans4_dot_c_inv4_x_ext_point_hessian_4ij(j) +
                       point_gradient4_colj_dot_c_inv4_x_point_gradient4_col_i(j, i));
      }
    }
  }

  return (score_inc);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void MultiGridNormalDistributionsTransform<PointSource, PointTarget>::computeHessian(
  Eigen::Matrix<double, 6, 6> & hessian, PointCloudSource & trans_cloud,
  Eigen::Matrix<double, 6, 1> &)
{
  // Initialize Point Gradient and Hessian
  // Pre-allocate thread-wise point gradients and point hessians
  std::vector<Eigen::Matrix<double, 4, 6>> t_point_gradients(params_.num_threads);
  std::vector<Eigen::Matrix<double, 24, 6>> t_point_hessians(params_.num_threads);
  std::vector<Eigen::Matrix<double, 6, 6>> t_hessians(params_.num_threads);

  for (int i = 0; i < params_.num_threads; ++i) {
    t_point_gradients[i].setZero();
    t_point_gradients[i].block<3, 3>(0, 0).setIdentity();
    t_point_hessians[i].setZero();
    t_hessians[i].setZero();
  }

  hessian.setZero();

  // Precompute Angular Derivatives unnecessary because only used after regular derivative
  // calculation

  // Update hessian for each point, line 17 in Algorithm 2 [Magnusson 2009]
#pragma omp parallel for num_threads(params_.num_threads) schedule(guided, 8)
  for (size_t idx = 0; idx < input_->size(); ++idx) {
    int tid = omp_get_thread_num();
    auto & x_trans_pt = trans_cloud[idx];

    // Find neighbors (Radius search has been experimentally faster than direct neighbor checking.
    std::vector<TargetGridLeafConstPtr> neighborhood;

    // Neighborhood search method other than kdtree is disabled in multigrid_ndt_omp
    target_cells_.radiusSearch(x_trans_pt, params_.resolution, neighborhood);

    if (neighborhood.empty()) {
      continue;
    }

    auto & x_pt = (*input_)[idx];
    // For math
    Eigen::Vector3d x(x_pt.x, x_pt.y, x_pt.z);
    const Eigen::Vector3d x_trans(x_trans_pt.x, x_trans_pt.y, x_trans_pt.z);

    auto & point_gradient = t_point_gradients[tid];
    auto & point_hessian = t_point_hessians[tid];
    auto & tmp_hessian = t_hessians[tid];

    // Compute derivative of transform function w.r.t. transform vector, J_E and H_E in
    // Equations 6.18 and 6.20 [Magnusson 2009]
    computePointDerivatives(x, point_gradient, point_hessian);

    for (const auto & cell : neighborhood) {
      // Update hessian, lines 21 in Algorithm 2, according to Equations 6.10, 6.12 and 6.13,
      // respectively [Magnusson 2009]
      updateHessian(
        tmp_hessian, point_gradient, point_hessian, x_trans - cell->getMean(),
        cell->getInverseCov());
    }
  }

  // Sum over t_hessians
  for (const auto & th : t_hessians) {
    hessian += th;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
void MultiGridNormalDistributionsTransform<PointSource, PointTarget>::updateHessian(
  Eigen::Matrix<double, 6, 6> & hessian, const Eigen::Matrix<double, 4, 6> & point_gradient,
  const Eigen::Matrix<double, 24, 6> & point_hessian, const Eigen::Vector3d & x_trans,
  const Eigen::Matrix3d & c_inv) const
{
  Eigen::Vector3d cov_dxd_pi;
  // e^(-d_2/2 * (x_k - mu_k)^T Sigma_k^-1 (x_k - mu_k)) Equation 6.9 [Magnusson 2009]
  double e_x_cov_x = gauss_d2_ * exp(-gauss_d2_ * x_trans.dot(c_inv * x_trans) / 2);

  // Error checking for invalid values.
  if (e_x_cov_x > 1 || e_x_cov_x < 0 || e_x_cov_x != e_x_cov_x) return;

  // Reusable portion of Equation 6.12 and 6.13 [Magnusson 2009]
  e_x_cov_x *= gauss_d1_;

  for (int i = 0; i < 6; ++i) {
    // Sigma_k^-1 d(T(x,p))/dpi, Reusable portion of Equation 6.12 and 6.13 [Magnusson 2009]
    cov_dxd_pi = c_inv * point_gradient.block<3, 1>(0, i);

    for (int j = 0; j < hessian.cols(); j++) {
      // Update hessian, Equation 6.13 [Magnusson 2009]
      Eigen::Vector3d pg_col = point_gradient.block<3, 1>(0, j);

      hessian(i, j) +=
        e_x_cov_x *
        (-gauss_d2_ * x_trans.dot(cov_dxd_pi) * x_trans.dot(c_inv * pg_col) +
         x_trans.dot(c_inv * point_hessian.block<3, 1>(3 * i, j)) + pg_col.dot(cov_dxd_pi));
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
bool MultiGridNormalDistributionsTransform<PointSource, PointTarget>::updateIntervalMT(
  double & a_l, double & f_l, double & g_l, double & a_u, double & f_u, double & g_u, double a_t,
  double f_t, double g_t)
{
  // Case U1 in Update Algorithm and Case a in Modified Update Algorithm [More, Thuente 1994]
  if (f_t > f_l) {
    a_u = a_t;
    f_u = f_t;
    g_u = g_t;
    return (false);
  }
  // Case U2 in Update Algorithm and Case b in Modified Update Algorithm [More, Thuente 1994]
  if (g_t * (a_l - a_t) > 0) {
    a_l = a_t;
    f_l = f_t;
    g_l = g_t;
    return (false);
  }
  // Case U3 in Update Algorithm and Case c in Modified Update Algorithm [More, Thuente 1994]
  if (g_t * (a_l - a_t) < 0) {
    a_u = a_l;
    f_u = f_l;
    g_u = g_l;

    a_l = a_t;
    f_l = f_t;
    g_l = g_t;
    return (false);
  }
  // Interval Converged
  return (true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
double MultiGridNormalDistributionsTransform<PointSource, PointTarget>::trialValueSelectionMT(
  double a_l, double f_l, double g_l, double a_u, double f_u, double g_u, double a_t, double f_t,
  double g_t)
{
  // Case 1 in Trial Value Selection [More, Thuente 1994]
  if (f_t > f_l) {
    // Calculate the minimizer of the cubic that interpolates f_l, f_t, g_l and g_t
    // Equation 2.4.52 [Sun, Yuan 2006]
    double z = 3 * (f_t - f_l) / (a_t - a_l) - g_t - g_l;
    double w = std::sqrt(z * z - g_t * g_l);
    // Equation 2.4.56 [Sun, Yuan 2006]
    double a_c = a_l + (a_t - a_l) * (w - g_l - z) / (g_t - g_l + 2 * w);

    // Calculate the minimizer of the quadratic that interpolates f_l, f_t and g_l
    // Equation 2.4.2 [Sun, Yuan 2006]
    double a_q = a_l - 0.5 * (a_l - a_t) * g_l / (g_l - (f_l - f_t) / (a_l - a_t));

    if (std::fabs(a_c - a_l) < std::fabs(a_q - a_l)) {
      return (a_c);
    }
    return (0.5 * (a_q + a_c));
  }
  // Case 2 in Trial Value Selection [More, Thuente 1994]
  if (g_t * g_l < 0) {
    // Calculate the minimizer of the cubic that interpolates f_l, f_t, g_l and g_t
    // Equation 2.4.52 [Sun, Yuan 2006]
    double z = 3 * (f_t - f_l) / (a_t - a_l) - g_t - g_l;
    double w = std::sqrt(z * z - g_t * g_l);
    // Equation 2.4.56 [Sun, Yuan 2006]
    double a_c = a_l + (a_t - a_l) * (w - g_l - z) / (g_t - g_l + 2 * w);

    // Calculate the minimizer of the quadratic that interpolates f_l, g_l and g_t
    // Equation 2.4.5 [Sun, Yuan 2006]
    double a_s = a_l - (a_l - a_t) / (g_l - g_t) * g_l;

    if (std::fabs(a_c - a_t) >= std::fabs(a_s - a_t)) {
      return (a_c);
    }
    return (a_s);
  }
  // Case 3 in Trial Value Selection [More, Thuente 1994]
  if (std::fabs(g_t) <= std::fabs(g_l)) {
    // Calculate the minimizer of the cubic that interpolates f_l, f_t, g_l and g_t
    // Equation 2.4.52 [Sun, Yuan 2006]
    double z = 3 * (f_t - f_l) / (a_t - a_l) - g_t - g_l;
    double w = std::sqrt(z * z - g_t * g_l);
    double a_c = a_l + (a_t - a_l) * (w - g_l - z) / (g_t - g_l + 2 * w);

    // Calculate the minimizer of the quadratic that interpolates g_l and g_t
    // Equation 2.4.5 [Sun, Yuan 2006]
    double a_s = a_l - (a_l - a_t) / (g_l - g_t) * g_l;

    double a_t_next = 0.0;

    if (std::fabs(a_c - a_t) < std::fabs(a_s - a_t))
      a_t_next = a_c;
    else
      a_t_next = a_s;

    if (a_t > a_l) {
      return (std::min(a_t + 0.66 * (a_u - a_t), a_t_next));
    }
    return (std::max(a_t + 0.66 * (a_u - a_t), a_t_next));
  }
  // Case 4 in Trial Value Selection [More, Thuente 1994]
  // Calculate the minimizer of the cubic that interpolates f_u, f_t, g_u and g_t
  // Equation 2.4.52 [Sun, Yuan 2006]
  double z = 3 * (f_t - f_u) / (a_t - a_u) - g_t - g_u;
  double w = std::sqrt(z * z - g_t * g_u);
  // Equation 2.4.56 [Sun, Yuan 2006]
  return (a_u + (a_t - a_u) * (w - g_u - z) / (g_t - g_u + 2 * w));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename PointSource, typename PointTarget>
double MultiGridNormalDistributionsTransform<PointSource, PointTarget>::computeStepLengthMT(
  const Eigen::Matrix<double, 6, 1> & x, Eigen::Matrix<double, 6, 1> & step_dir, double step_init,
  double step_max, double step_min, double & score, Eigen::Matrix<double, 6, 1> & score_gradient,
  Eigen::Matrix<double, 6, 6> & hessian, PointCloudSource & trans_cloud)
{
  // Set the value of phi'(0), Equation 1.3 [More, Thuente 1994]
  double d_phi_0 = -(score_gradient.dot(step_dir));

  Eigen::Matrix<double, 6, 1> x_t;

  if (d_phi_0 >= 0) {
    // Not a decent direction
    if (d_phi_0 == 0) return 0;

    // Reverse step direction and calculate optimal step.
    d_phi_0 *= -1;
    step_dir *= -1;
  }

  double a_t = step_init;
  a_t = std::min(a_t, step_max);
  a_t = std::max(a_t, step_min);

  x_t = x + step_dir * a_t;

  final_transformation_ =
    (Eigen::Translation<float, 3>(
       static_cast<float>(x_t(0)), static_cast<float>(x_t(1)), static_cast<float>(x_t(2))) *
     Eigen::AngleAxis<float>(static_cast<float>(x_t(3)), Eigen::Vector3f::UnitX()) *
     Eigen::AngleAxis<float>(static_cast<float>(x_t(4)), Eigen::Vector3f::UnitY()) *
     Eigen::AngleAxis<float>(static_cast<float>(x_t(5)), Eigen::Vector3f::UnitZ()))
      .matrix();

  // New transformed point cloud
  transformPointCloud(*input_, trans_cloud, final_transformation_);

  // Updates score, gradient and hessian.  Hessian calculation is unnecessary but testing showed
  // that most step calculations use the initial step suggestion and recalculation the reusable
  // portions of the hessian would entail more computation time.
  score = computeDerivatives(score_gradient, hessian, trans_cloud, x_t, true);

  // --------------------------------------------------------------------------------------------------------------------------------
  // FIXME(YamatoAndo):
  // Currently, using LineSearch causes the following problems:
  // It may converge to a local solution.
  // There are cases where a loop process is executed without changing the variable values,
  // resulting in unnecessary processing time. As better convergence results are obtained without
  // using LineSearch, we have decided to disable this feature. If you have any suggestions on how
  // to solve these issues, we would appreciate it if you could contribute.
  // --------------------------------------------------------------------------------------------------------------------------------
  int step_iterations = 0;

  if (params_.use_line_search) {
    // Set the value of phi(0), Equation 1.3 [More, Thuente 1994]
    double phi_0 = -score;

    // The Search Algorithm for T(mu) [More, Thuente 1994]

    int max_step_iterations = 10;

    // Sufficient decrease constant, Equation 1.1 [More, Thuete 1994]
    double mu = 1.e-4;
    // Curvature condition constant, Equation 1.2 [More, Thuete 1994]
    double nu = 0.9;

    // Initial endpoints of Interval I,
    double a_l = 0;
    double a_u = 0;

    // Auxiliary function psi is used until I is determined ot be a closed interval, Equation 2.1
    // [More, Thuente 1994]
    double f_l = auxiliaryFunction_PsiMT(a_l, phi_0, phi_0, d_phi_0, mu);
    double g_l = auxiliaryFunction_dPsiMT(d_phi_0, d_phi_0, mu);

    double f_u = auxiliaryFunction_PsiMT(a_u, phi_0, phi_0, d_phi_0, mu);
    double g_u = auxiliaryFunction_dPsiMT(d_phi_0, d_phi_0, mu);

    // Check used to allow More-Thuente step length calculation to be skipped by making step_min ==
    // step_max
    bool interval_converged = (step_max - step_min) < 0;
    bool open_interval = true;

    // Calculate phi(alpha_t)
    double phi_t = -score;
    // Calculate phi'(alpha_t)
    double d_phi_t = -(score_gradient.dot(step_dir));

    // Calculate psi(alpha_t)
    double psi_t = auxiliaryFunction_PsiMT(a_t, phi_t, phi_0, d_phi_0, mu);
    // Calculate psi'(alpha_t)
    double d_psi_t = auxiliaryFunction_dPsiMT(d_phi_t, d_phi_0, mu);

    // Iterate until max number of iterations, interval convergence or a value satisfies the
    // sufficient decrease, Equation 1.1, and curvature condition, Equation 1.2 [More, Thuente 1994]
    while (
      !interval_converged && step_iterations < max_step_iterations &&
      !(psi_t <= 0 /*Sufficient Decrease*/ && d_phi_t <= -nu * d_phi_0 /*Curvature Condition*/)) {
      // Use auxiliary function if interval I is not closed
      if (open_interval) {
        a_t = trialValueSelectionMT(a_l, f_l, g_l, a_u, f_u, g_u, a_t, psi_t, d_psi_t);
      } else {
        a_t = trialValueSelectionMT(a_l, f_l, g_l, a_u, f_u, g_u, a_t, phi_t, d_phi_t);
      }

      a_t = std::min(a_t, step_max);
      a_t = std::max(a_t, step_min);

      x_t = x + step_dir * a_t;

      final_transformation_ =
        (Eigen::Translation<float, 3>(
           static_cast<float>(x_t(0)), static_cast<float>(x_t(1)), static_cast<float>(x_t(2))) *
         Eigen::AngleAxis<float>(static_cast<float>(x_t(3)), Eigen::Vector3f::UnitX()) *
         Eigen::AngleAxis<float>(static_cast<float>(x_t(4)), Eigen::Vector3f::UnitY()) *
         Eigen::AngleAxis<float>(static_cast<float>(x_t(5)), Eigen::Vector3f::UnitZ()))
          .matrix();

      // New transformed point cloud
      // Done on final cloud to prevent wasted computation
      transformPointCloud(*input_, trans_cloud, final_transformation_);

      // Updates score, gradient. Values stored to prevent wasted computation.
      score = computeDerivatives(score_gradient, hessian, trans_cloud, x_t, false);

      // Calculate phi(alpha_t+)
      phi_t = -score;
      // Calculate phi'(alpha_t+)
      d_phi_t = -(score_gradient.dot(step_dir));

      // Calculate psi(alpha_t+)
      psi_t = auxiliaryFunction_PsiMT(a_t, phi_t, phi_0, d_phi_0, mu);
      // Calculate psi'(alpha_t+)
      d_psi_t = auxiliaryFunction_dPsiMT(d_phi_t, d_phi_0, mu);

      // Check if I is now a closed interval
      if (open_interval && (psi_t <= 0 && d_psi_t >= 0)) {
        open_interval = false;

        // Converts f_l and g_l from psi to phi
        f_l = f_l + phi_0 - mu * d_phi_0 * a_l;
        g_l = g_l + mu * d_phi_0;

        // Converts f_u and g_u from psi to phi
        f_u = f_u + phi_0 - mu * d_phi_0 * a_u;
        g_u = g_u + mu * d_phi_0;
      }

      if (open_interval) {
        // Update interval end points using Updating Algorithm [More, Thuente 1994]
        interval_converged = updateIntervalMT(a_l, f_l, g_l, a_u, f_u, g_u, a_t, psi_t, d_psi_t);
      } else {
        // Update interval end points using Modified Updating Algorithm [More, Thuente 1994]
        interval_converged = updateIntervalMT(a_l, f_l, g_l, a_u, f_u, g_u, a_t, phi_t, d_phi_t);
      }

      step_iterations++;
    }
  }

  // If inner loop was run then hessian needs to be calculated.
  // Hessian is unnecessary for step length determination but gradients are required
  // so derivative and transform data is stored for the next iteration.
  if (step_iterations) computeHessian(hessian, trans_cloud, x_t);

  return (a_t);
}

template <typename PointSource, typename PointTarget>
double
MultiGridNormalDistributionsTransform<PointSource, PointTarget>::calculateTransformationProbability(
  const PointCloudSource & trans_cloud) const
{
  double score = 0;

  // Score per thread
  std::vector<double> t_scores(params_.num_threads, 0);

#pragma omp parallel for num_threads(params_.num_threads) schedule(guided, 8)
  for (size_t idx = 0; idx < trans_cloud.size(); ++idx) {
    int tid = omp_get_thread_num();
    PointSource x_trans_pt = trans_cloud[idx];

    // Find neighbors (Radius search has been experimentally faster than direct neighbor checking.
    std::vector<TargetGridLeafConstPtr> neighborhood;

    // Neighborhood search method other than kdtree is disabled in multigrid_ndt_omp
    target_cells_.radiusSearch(x_trans_pt, params_.resolution, neighborhood);

    if (neighborhood.empty()) {
      continue;
    }

    double tmp_score = 0;

    for (auto & cell : neighborhood) {
      Eigen::Vector3d x_trans(x_trans_pt.x, x_trans_pt.y, x_trans_pt.z);

      // Denorm point, x_k' in Equations 6.12 and 6.13 [Magnusson 2009]
      x_trans -= cell->getMean();

      // Calculate probability of transformed points existence, Equation 6.9 [Magnusson 2009]
      // e^(-d_2/2 * (x_k - mu_k)^T Sigma_k^-1 (x_k - mu_k)) Equation 6.9 [Magnusson 2009]
      tmp_score -= gauss_d1_ * exp(-gauss_d2_ * x_trans.dot(cell->getInverseCov() * x_trans) / 2.0);
    }

    t_scores[tid] += tmp_score;
  }

  // Sum the point-wise scores
  for (const auto & ts : t_scores) {
    score += ts;
  }

  double output_score = 0;
  if (!trans_cloud.empty()) {
    output_score = (score) / static_cast<double>(trans_cloud.size());
  }
  return output_score;
}

template <typename PointSource, typename PointTarget>
double MultiGridNormalDistributionsTransform<PointSource, PointTarget>::
  calculateNearestVoxelTransformationLikelihood(const PointCloudSource & trans_cloud) const
{
  double nearest_voxel_score = 0;
  size_t found_neighborhood_voxel_num = 0;

  // Thread-wise results
  std::vector<double> t_nvs(params_.num_threads);
  std::vector<size_t> t_found_nnvn(params_.num_threads);

  for (int i = 0; i < params_.num_threads; ++i) {
    t_nvs[i] = 0;
    t_found_nnvn[i] = 0;
  }

#pragma omp parallel for num_threads(params_.num_threads) schedule(guided, 8)
  for (size_t idx = 0; idx < trans_cloud.size(); ++idx) {
    int tid = omp_get_thread_num();
    PointSource x_trans_pt = trans_cloud[idx];

    // Find neighbors (Radius search has been experimentally faster than direct neighbor checking.
    std::vector<TargetGridLeafConstPtr> neighborhood;

    // Neighborhood search method other than kdtree is disabled in multigrid_ndt_omp
    target_cells_.radiusSearch(x_trans_pt, params_.resolution, neighborhood);

    if (neighborhood.empty()) {
      continue;
    }

    double nearest_voxel_score_pt = 0;

    for (auto & cell : neighborhood) {
      Eigen::Vector3d x_trans(x_trans_pt.x, x_trans_pt.y, x_trans_pt.z);

      // Denorm point, x_k' in Equations 6.12 and 6.13 [Magnusson 2009]
      x_trans -= cell->getMean();

      // e^(-d_2/2 * (x_k - mu_k)^T Sigma_k^-1 (x_k - mu_k)) Equation 6.9 [Magnusson 2009]
      double e_x_cov_x = exp(-gauss_d2_ * x_trans.dot(cell->getInverseCov() * x_trans) / 2.0);
      // Calculate probability of transformed points existence, Equation 6.9 [Magnusson 2009]
      double score_inc = -gauss_d1_ * e_x_cov_x;

      if (score_inc > nearest_voxel_score_pt) {
        nearest_voxel_score_pt = score_inc;
      }
    }

    t_nvs[tid] += nearest_voxel_score_pt;
    ++t_found_nnvn[tid];
  }

  // Sum up point-wise scores
  for (int idx = 0; idx < params_.num_threads; ++idx) {
    found_neighborhood_voxel_num += t_found_nnvn[idx];
    nearest_voxel_score += t_nvs[idx];
  }

  double output_score = 0;

  if (found_neighborhood_voxel_num != 0) {
    output_score = nearest_voxel_score / static_cast<double>(found_neighborhood_voxel_num);
  }
  return output_score;
}

template <typename PointSource, typename PointTarget>
pcl::PointCloud<pcl::PointXYZI> MultiGridNormalDistributionsTransform<PointSource, PointTarget>::
  calculateNearestVoxelScoreEachPoint(const PointCloudSource & trans_cloud) const
{
  // Thread-wise results
  std::vector<pcl::PointCloud<pcl::PointXYZI>> threads_pc(params_.num_threads);
  pcl::PointCloud<pcl::PointXYZI> score_points;

#pragma omp parallel for num_threads(params_.num_threads) schedule(guided, 8)
  for (size_t idx = 0; idx < trans_cloud.size(); ++idx) {
    int tid = omp_get_thread_num();
    PointSource x_trans_pt = trans_cloud[idx];

    // Find neighbors (Radius search has been experimentally faster than direct neighbor checking.
    std::vector<TargetGridLeafConstPtr> neighborhood;

    // Neighborhood search method other than kdtree is disabled in multigrid_ndt_omp
    target_cells_.radiusSearch(x_trans_pt, params_.resolution, neighborhood);

    if (neighborhood.empty()) {
      continue;
    }

    double nearest_voxel_score_pt = 0;

    for (auto & cell : neighborhood) {
      Eigen::Vector3d x_trans(x_trans_pt.x, x_trans_pt.y, x_trans_pt.z);

      // Denorm point, x_k' in Equations 6.12 and 6.13 [Magnusson 2009]
      x_trans -= cell->getMean();

      // e^(-d_2/2 * (x_k - mu_k)^T Sigma_k^-1 (x_k - mu_k)) Equation 6.9 [Magnusson 2009]
      double e_x_cov_x = exp(-gauss_d2_ * x_trans.dot(cell->getInverseCov() * x_trans) / 2.0);
      // Calculate probability of transformed points existence, Equation 6.9 [Magnusson 2009]
      double score_inc = -gauss_d1_ * e_x_cov_x;

      if (score_inc > nearest_voxel_score_pt) {
        nearest_voxel_score_pt = score_inc;
      }
    }

    pcl::PointXYZI sensor_point_score;
    sensor_point_score.x = trans_cloud.points[idx].x;
    sensor_point_score.y = trans_cloud.points[idx].y;
    sensor_point_score.z = trans_cloud.points[idx].z;
    sensor_point_score.intensity = nearest_voxel_score_pt;
    threads_pc[tid].points.push_back(sensor_point_score);
  }

  // Sum up point-wise scores
  for (int idx = 0; idx < params_.num_threads; ++idx) {
    for (size_t p_idx = 0; p_idx < threads_pc[idx].size(); ++p_idx) {
      score_points.points.push_back(threads_pc[idx].points[p_idx]);
    }
  }

  return score_points;
}

}  // namespace pclomp

#endif  // NDT_OMP__MULTIGRID_NDT_OMP_IMPL_HPP_

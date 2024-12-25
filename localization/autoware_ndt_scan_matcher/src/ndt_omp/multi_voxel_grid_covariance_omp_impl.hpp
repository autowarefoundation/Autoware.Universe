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
 */

#ifndef NDT_OMP__MULTI_VOXEL_GRID_COVARIANCE_OMP_IMPL_HPP_
#define NDT_OMP__MULTI_VOXEL_GRID_COVARIANCE_OMP_IMPL_HPP_

// cspell:ignore evecs, evals, covar, eigvalue, futs

#include "autoware/ndt_scan_matcher/ndt_omp/multi_voxel_grid_covariance_omp.h"

#include <pcl/common/common.h>
#include <pcl/filters/boost.h>

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace pclomp
{

template <typename PointT>
MultiVoxelGridCovariance<PointT>::MultiVoxelGridCovariance(const MultiVoxelGridCovariance & other)
: pcl::VoxelGrid<PointT>(other),
  sid_to_iid_(other.sid_to_iid_),
  grid_list_(other.grid_list_),
  kdtree_(other.kdtree_),
  leaf_ptrs_(other.leaf_ptrs_)
{
  min_points_per_voxel_ = other.min_points_per_voxel_;
  min_covar_eigvalue_mult_ = other.min_covar_eigvalue_mult_;

  // Deep copy
  voxel_centroids_ptr_.reset(new PointCloud);

  if (other.voxel_centroids_ptr_) {
    *voxel_centroids_ptr_ = *other.voxel_centroids_ptr_;
  }

  setThreadNum(other.thread_num_);
  last_check_tid_ = -1;
}

template <typename PointT>
MultiVoxelGridCovariance<PointT>::MultiVoxelGridCovariance(
  MultiVoxelGridCovariance && other) noexcept
: pcl::VoxelGrid<PointT>(std::move(other)),
  voxel_centroids_ptr_(std::move(other.voxel_centroids_ptr_)),
  sid_to_iid_(std::move(other.sid_to_iid_)),
  grid_list_(std::move(other.grid_list_)),
  kdtree_(std::move(other.kdtree_)),
  leaf_ptrs_(std::move(other.leaf_ptrs_))
{
  min_points_per_voxel_ = other.min_points_per_voxel_;
  min_covar_eigvalue_mult_ = other.min_covar_eigvalue_mult_;

  setThreadNum(other.thread_num_);
  last_check_tid_ = -1;
}

template <typename PointT>
MultiVoxelGridCovariance<PointT> & pclomp::MultiVoxelGridCovariance<PointT>::operator=(
  const MultiVoxelGridCovariance & other)
{
  pcl::VoxelGrid<PointT>::operator=(other);
  voxel_centroids_ptr_ = other.voxel_centroids_ptr_;
  sid_to_iid_ = other.sid_to_iid_;
  grid_list_ = other.grid_list_;
  kdtree_ = other.kdtree_;
  leaf_ptrs_ = other.leaf_ptrs_;
  min_points_per_voxel_ = other.min_points_per_voxel_;
  min_covar_eigvalue_mult_ = other.min_covar_eigvalue_mult_;

  // Deep copy
  voxel_centroids_ptr_.reset(new PointCloud);

  if (other.voxel_centroids_ptr_) {
    *voxel_centroids_ptr_ = *other.voxel_centroids_ptr_;
  }

  setThreadNum(other.thread_num_);
  last_check_tid_ = -1;

  return *this;
}

template <typename PointT>
MultiVoxelGridCovariance<PointT> & pclomp::MultiVoxelGridCovariance<PointT>::operator=(
  MultiVoxelGridCovariance && other) noexcept
{
  voxel_centroids_ptr_ = std::move(other.voxel_centroids_ptr_);
  sid_to_iid_ = std::move(other.sid_to_iid_);
  grid_list_ = std::move(other.grid_list_);
  kdtree_ = std::move(other.kdtree_);
  leaf_ptrs_ = std::move(other.leaf_ptrs_);

  min_points_per_voxel_ = other.min_points_per_voxel_;
  min_covar_eigvalue_mult_ = other.min_covar_eigvalue_mult_;

  setThreadNum(other.thread_num_);
  last_check_tid_ = -1;

  pcl::VoxelGrid<PointT>::operator=(std::move(other));

  return *this;
}

template <typename PointT>
void MultiVoxelGridCovariance<PointT>::setInputCloudAndFilter(
  const PointCloudConstPtr & cloud, const std::string & grid_id)
{
  // Creat a new grid and push it to the back of the vector grid_list_
  // The vector grid_list_ will be rebuilt after we remove obsolete grids
  GridNodePtr new_grid(new GridNodeType);

  grid_list_.push_back(new_grid);

  sid_to_iid_[grid_id] = grid_list_.size() - 1;

  // If single thread, no parallel processing
  if (thread_num_ == 1) {
    apply_filter(cloud, *new_grid);
  } else {
    int idle_tid = get_idle_tid();

    processing_inputs_[idle_tid] = cloud;
    thread_futs_[idle_tid] = std::async(
      std::launch::async, &MultiVoxelGridCovariance<PointT>::apply_filter_thread, this, idle_tid,
      std::ref(*new_grid));
  }
}

template <typename PointT>
void MultiVoxelGridCovariance<PointT>::removeCloud(const std::string & grid_id)
{
  auto iid = sid_to_iid_.find(grid_id);

  if (iid == sid_to_iid_.end()) {
    return;
  }

  // Set the pointer corresponding to the specified grid to null
  grid_list_[iid->second].reset();
  // Remove the specified grid from the conversion map
  sid_to_iid_.erase(iid);
}

template <typename PointT>
void MultiVoxelGridCovariance<PointT>::createKdtree()
{
  // Wait for all threads to finish
  sync();

  // Rebuild the grid_list_ and sid_to_iid_ to delete the data related to the removed clouds
  const int new_grid_num = sid_to_iid_.size();
  std::vector<GridNodePtr> new_grid_list(new_grid_num);
  int new_pos = 0;
  int total_leaf_num = 0;

  for (auto & it : sid_to_iid_) {
    int & old_pos = it.second;
    auto & grid_ptr = grid_list_[old_pos];

    new_grid_list[new_pos] = grid_ptr;
    old_pos = new_pos;
    ++new_pos;
    total_leaf_num = grid_ptr->size();
  }

  grid_list_ = std::move(new_grid_list);

  // Rebuild the voxel_centroids_ptr_
  voxel_centroids_ptr_.reset(new PointCloud);
  voxel_centroids_ptr_->reserve(total_leaf_num);
  leaf_ptrs_.clear();
  leaf_ptrs_.reserve(total_leaf_num);

  for (const auto & grid_ptr : grid_list_) {
    for (const auto & leaf : *grid_ptr) {
      PointT new_leaf;

      new_leaf.x = leaf.centroid_[0];
      new_leaf.y = leaf.centroid_[1];
      new_leaf.z = leaf.centroid_[2];
      voxel_centroids_ptr_->push_back(new_leaf);
      leaf_ptrs_.push_back(&leaf);
    }
  }

  // Rebuild the kdtree_ of leaves
  if (voxel_centroids_ptr_->size() > 0) {
    kdtree_.setInputCloud(voxel_centroids_ptr_);
  }
}

template <typename PointT>
int MultiVoxelGridCovariance<PointT>::radiusSearch(
  const PointT & point, double radius, std::vector<LeafConstPtr> & k_leaves,
  unsigned int max_nn) const
{
  k_leaves.clear();

  // Search from the kdtree to find neighbors of @point
  std::vector<float> k_sqr_distances;
  std::vector<int> k_indices;
  const int k = kdtree_.radiusSearch(point, radius, k_indices, k_sqr_distances, max_nn);

  if (k <= 0) {
    return 0;
  }

  k_leaves.reserve(k);

  for (auto & nn_idx : k_indices) {
    k_leaves.push_back(leaf_ptrs_[nn_idx]);
  }

  return k_leaves.size();
}

template <typename PointT>
int MultiVoxelGridCovariance<PointT>::radiusSearch(
  const PointCloud & cloud, int index, double radius, std::vector<LeafConstPtr> & k_leaves,
  unsigned int max_nn) const
{
  if (index >= static_cast<int>(cloud.size()) || index < 0) {
    return 0;
  }

  return (radiusSearch(cloud[index], radius, k_leaves, max_nn));
}

template <typename PointT>
typename MultiVoxelGridCovariance<PointT>::PointCloud
MultiVoxelGridCovariance<PointT>::getVoxelPCD() const
{
  return *voxel_centroids_ptr_;
}

template <typename PointT>
std::vector<std::string> MultiVoxelGridCovariance<PointT>::getCurrentMapIDs() const
{
  std::vector<std::string> output;

  output.reserve(sid_to_iid_.size());

  for (const auto & element : sid_to_iid_) {
    output.push_back(element.first);
  }

  return output;
}

//////////////////////////////////////////////////////////////////////////////////////////
template <typename PointT>
void MultiVoxelGridCovariance<PointT>::apply_filter(
  const PointCloudConstPtr & input, GridNodeType & node)
{
  // Has the input dataset been set already?
  if (!input) {
    PCL_WARN("[pcl::%s::apply_filter] No input dataset given!\n", getClassName().c_str());
    return;
  }

  Eigen::Vector4f min_p;
  Eigen::Vector4f max_p;
  pcl::getMinMax3D<PointT>(*input, min_p, max_p);

  // Check that the leaf size is not too small, given the size of the data
  int64_t dx = static_cast<int64_t>((max_p[0] - min_p[0]) * inverse_leaf_size_[0]) + 1;
  int64_t dy = static_cast<int64_t>((max_p[1] - min_p[1]) * inverse_leaf_size_[1]) + 1;
  int64_t dz = static_cast<int64_t>((max_p[2] - min_p[2]) * inverse_leaf_size_[2]) + 1;

  if ((dx * dy * dz) > std::numeric_limits<int32_t>::max()) {
    PCL_WARN(
      "[pcl::%s::apply_filter] Leaf size is too small for the input dataset. Integer indices would "
      "overflow.",
      getClassName().c_str());
    return;
  }

  // Compute the minimum and maximum bounding box values
  BoundingBox bbox;
  bbox.min[0] = static_cast<int>(floor(min_p[0] * inverse_leaf_size_[0]));
  bbox.max[0] = static_cast<int>(floor(max_p[0] * inverse_leaf_size_[0]));
  bbox.min[1] = static_cast<int>(floor(min_p[1] * inverse_leaf_size_[1]));
  bbox.max[1] = static_cast<int>(floor(max_p[1] * inverse_leaf_size_[1]));
  bbox.min[2] = static_cast<int>(floor(min_p[2] * inverse_leaf_size_[2]));
  bbox.max[2] = static_cast<int>(floor(max_p[2] * inverse_leaf_size_[2]));

  // Compute the number of divisions needed along all axis
  Eigen::Vector4i div_b = bbox.max - bbox.min + Eigen::Vector4i::Ones();
  div_b[3] = 0;

  // Clear the leaves
  node.clear();

  // Set up the division multiplier
  bbox.div_mul = Eigen::Vector4i(1, div_b[0], div_b[0] * div_b[1], 0);

  int centroid_size = 4;
  std::map<int64_t, Leaf> map_leaves;

  // First pass: go over all points and insert them into the right leaf
  for (auto & p : *input) {
    if (!input->is_dense) {
      // Check if the point is invalid
      if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) continue;
    }
    int64_t lid = getLeafID(p, bbox);
    Leaf & leaf = map_leaves[lid];
    updateLeaf(p, centroid_size, leaf);
  }

  // Second pass: go over all leaves and compute centroids and covariance matrices

  // Eigen values and vectors calculated to prevent near singular matrices
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigensolver;
  Eigen::Vector3d pt_sum;

  node.reserve(map_leaves.size());

  // Eigen values less than a threshold of max eigen value are inflated to a set fraction of the max
  // eigen value.
  for (auto & it : map_leaves) {
    // Skip leaves that do not have enough points
    if (it.second.nr_points_ < min_points_per_voxel_) {
      continue;
    }

    // Append qualified leaves to the end of the output vector
    node.push_back(it.second);

    // Normalize the centroid
    Leaf & leaf = node.back();

    // Normalize the centroid
    leaf.centroid_ /= static_cast<float>(leaf.nr_points_);
    // Point sum used for single pass covariance calculation
    pt_sum = leaf.mean_;
    // Normalize mean
    leaf.mean_ /= leaf.nr_points_;
    // Compute covariance matrices
    computeLeafParams(pt_sum, eigensolver, leaf);
  }
}

template <typename PointT>
int64_t pclomp::MultiVoxelGridCovariance<PointT>::getLeafID(
  const PointT & point, const BoundingBox & bbox) const
{
  int ijk0 =
    static_cast<int>(floor(point.x * inverse_leaf_size_[0]) - static_cast<float>(bbox.min[0]));
  int ijk1 =
    static_cast<int>(floor(point.y * inverse_leaf_size_[1]) - static_cast<float>(bbox.min[1]));
  int ijk2 =
    static_cast<int>(floor(point.z * inverse_leaf_size_[2]) - static_cast<float>(bbox.min[2]));

  return ijk0 * bbox.div_mul[0] + ijk1 * bbox.div_mul[1] + ijk2 * bbox.div_mul[2];
}

template <typename PointT>
void pclomp::MultiVoxelGridCovariance<PointT>::updateLeaf(
  const PointT & point, const int & centroid_size, Leaf & leaf) const
{
  if (leaf.nr_points_ == 0) {
    leaf.centroid_.resize(centroid_size);
    leaf.centroid_.setZero();
  }

  Eigen::Vector3d pt3d(point.x, point.y, point.z);
  // Accumulate point sum for centroid calculation
  leaf.mean_ += pt3d;
  // Accumulate x*xT for single pass covariance calculation
  leaf.cov_ += pt3d * pt3d.transpose();

  Eigen::Vector4f pt(point.x, point.y, point.z, 0);
  leaf.centroid_.template head<4>() += pt;
  ++leaf.nr_points_;
}

template <typename PointT>
void pclomp::MultiVoxelGridCovariance<PointT>::computeLeafParams(
  const Eigen::Vector3d & pt_sum, Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> & eigensolver,
  Leaf & leaf) const
{
  // Single pass covariance calculation
  leaf.cov_ = (leaf.cov_ - 2 * (pt_sum * leaf.mean_.transpose())) / leaf.nr_points_ +
              leaf.mean_ * leaf.mean_.transpose();
  leaf.cov_ *= (leaf.nr_points_ - 1.0) / leaf.nr_points_;

  // Normalize Eigen Val such that max no more than 100x min.
  eigensolver.compute(leaf.cov_);
  Eigen::Matrix3d eigen_val = eigensolver.eigenvalues().asDiagonal();
  Eigen::Matrix3d eigen_vec = eigensolver.eigenvectors();

  if (eigen_val(0, 0) < 0 || eigen_val(1, 1) < 0 || eigen_val(2, 2) <= 0) {
    leaf.nr_points_ = -1;
    return;
  }

  // Avoids matrices near singularities (eq 6.11)[Magnusson 2009]
  double min_covar_eigvalue = min_covar_eigvalue_mult_ * eigen_val(2, 2);
  if (eigen_val(0, 0) < min_covar_eigvalue) {
    eigen_val(0, 0) = min_covar_eigvalue;

    if (eigen_val(1, 1) < min_covar_eigvalue) {
      eigen_val(1, 1) = min_covar_eigvalue;
    }

    leaf.cov_ = eigen_vec * eigen_val * eigen_vec.inverse();
  }

  leaf.icov_ = leaf.cov_.inverse();
  if (
    leaf.icov_.maxCoeff() == std::numeric_limits<float>::infinity() ||
    leaf.icov_.minCoeff() == -std::numeric_limits<float>::infinity()) {
    leaf.nr_points_ = -1;
  }
}

}  // namespace pclomp

#define PCL_INSTANTIATE_VoxelGridCovariance(T) \
  template class PCL_EXPORTS pcl::VoxelGridCovariance<T>;

#endif  // NDT_OMP__MULTI_VOXEL_GRID_COVARIANCE_OMP_IMPL_HPP_

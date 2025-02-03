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

/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2011-2025 Jose Luis Blanco (joseluisblancoc@gmail.com).
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/
#ifndef AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__KDTREE_NANOFLANN_HPP_
#define AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__KDTREE_NANOFLANN_HPP_

// cspell:ignore nanoflann, dists

#include "autoware/ndt_scan_matcher/ndt_omp/nanoflann.hpp"

#include <pcl/pcl_macros.h>
// clang-format on

#include <pcl/filters/boost.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/point_types.h>

#include <memory>
#include <vector>

template <typename PointT>
class KdTreeNanoflann
{
  typedef typename pcl::Filter<PointT>::PointCloud PointCloud;
  typedef typename PointCloud::Ptr PointCloudPtr;

  struct PointCloudNanoflann
  {
    PointCloudPtr cloud_;
    explicit PointCloudNanoflann(PointCloudPtr cloud) { cloud_ = cloud; }

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return cloud_->size(); }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate
    // value, the
    //  "if/else's" are actually solved at compile time.
    inline float kdtree_get_pt(const size_t idx, const size_t dim) const
    {
      if (dim == 0)
        return (*cloud_)[idx].x;
      else if (dim == 1)
        return (*cloud_)[idx].y;
      else
        return (*cloud_)[idx].z;
    }

    // Optional bounding-box computation: return false to default to a standard
    // bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned
    //   in "bb" so it can be avoided to redo it again. Look at bb.size() to
    //   find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX & /* bb */) const
    {
      return false;
    }
  };

  using kdtree_t = nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<float, PointCloudNanoflann>, PointCloudNanoflann, 3 /* dim */
    >;

  std::shared_ptr<kdtree_t> index_ptr_;
  std::shared_ptr<PointCloudNanoflann> cloud_ptr_;

public:
  KdTreeNanoflann() : index_ptr_(), cloud_ptr_() {}

  KdTreeNanoflann(const KdTreeNanoflann & other)
  : index_ptr_(other.index_ptr_), cloud_ptr_(other.cloud_ptr_)
  {
  }

  KdTreeNanoflann & operator=(const KdTreeNanoflann & other)
  {
    index_ptr_ = other.index_ptr_;
    cloud_ptr_ = other.cloud_ptr_;
    return *this;
  }

  void setInputCloud(PointCloudPtr cloud)
  {
    cloud_ptr_ = std::make_shared<PointCloudNanoflann>(cloud);
    index_ptr_ = std::make_shared<kdtree_t>(
      3, *cloud_ptr_, nanoflann::KDTreeSingleIndexAdaptorParams(15 /* max leaf */));
  }

  int radiusSearch(
    const PointT & point, double radius,
    std::vector<nanoflann::ResultItem<size_t, float>> & indices_dists,
    [[maybe_unused]] unsigned int max_nn) const
  {
    const float sqr_radius = radius * radius;
    float query_pt[3] = {point.x, point.y, point.z};
    nanoflann::RadiusResultSet<float, size_t> resultSet(sqr_radius, indices_dists);
    index_ptr_->findNeighbors(resultSet, query_pt);
    int k = resultSet.size();
    return k;
  }
};
#endif  // AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__KDTREE_NANOFLANN_HPP_

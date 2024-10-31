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
/*
 * Software License Agreement (BSD License)
 *
 *  Point Cloud Library (PCL) - www.pointclouds.org
 *  Copyright (c) 2010-2012, Willow Garage, Inc.
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

#ifndef AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__NDT_STRUCT_HPP_
#define AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__NDT_STRUCT_HPP_

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <vector>

namespace pclomp
{

enum NeighborSearchMethod { KDTREE, DIRECT26, DIRECT7, DIRECT1 };

struct NdtResult
{
  Eigen::Matrix4f pose;
  float transform_probability;
  float nearest_voxel_transformation_likelihood;
  int iteration_num;
  std::vector<Eigen::Matrix4f, Eigen::aligned_allocator<Eigen::Matrix4f>> transformation_array;
  Eigen::Matrix<double, 6, 6> hessian;
  std::vector<float> transform_probability_array;
  std::vector<float> nearest_voxel_transformation_likelihood_array;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  friend std::ostream & operator<<(std::ostream & os, const NdtResult & val)
  {
    os << "Pose: " << std::endl << val.pose << std::endl;
    os << "TP: " << val.transform_probability << std::endl;
    os << "NVTL: " << val.nearest_voxel_transformation_likelihood << std::endl;
    os << "Iteration num: " << val.iteration_num << std::endl;
    os << "Hessian: " << std::endl << val.hessian << std::endl;
    return os;
  }
};

struct NdtParams
{
  double trans_epsilon{};
  double step_size{};
  double resolution{};
  int max_iterations{};
  NeighborSearchMethod search_method{};
  int num_threads{};
  float regularization_scale_factor{};

  // line search is false by default
  // "use_lines_search = true" is not tested well
  bool use_line_search = false;
};

}  // namespace pclomp

#endif  // AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__NDT_STRUCT_HPP_

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

#ifndef AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__MULTIGRID_NDT_OMP_H_
#define AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__MULTIGRID_NDT_OMP_H_

// cspell:ignore multigrid, Magnusson, Thuente, Todor, Stoyanov, Okorn

#include "multi_voxel_grid_covariance_omp.h"
#include "ndt_struct.hpp"

#include <pcl/search/impl/search.hpp>
#include <unsupported/Eigen/NonLinearOptimization>

#include "boost/optional.hpp"

#include <pcl/registration/registration.h>

#include <string>
#include <vector>

namespace pclomp
{

/** \brief A 3D Normal Distribution Transform registration implementation for point cloud data.
 * \note For more information please see
 * <b>Magnusson, M. (2009). The Three-Dimensional Normal-Distributions Transform —
 * an Efficient Representation for Registration, Surface Analysis, and Loop Detection.
 * PhD thesis, Orebro University. Orebro Studies in Technology 36.</b>,
 * <b>More, J., and Thuente, D. (1994). Line Search Algorithm with Guaranteed Sufficient Decrease
 * In ACM Transactions on Mathematical Software.</b> and
 * Sun, W. and Yuan, Y, (2006) Optimization Theory and Methods: Nonlinear Programming. 89-100
 * \note Math refactored by Todor Stoyanov.
 * \author Brian Okorn (Space and Naval Warfare Systems Center Pacific)
 */
template <typename PointSource, typename PointTarget>
class MultiGridNormalDistributionsTransform : public pcl::Registration<PointSource, PointTarget>
{
protected:
  typedef pcl::Registration<PointSource, PointTarget> BaseRegType;
  typedef typename BaseRegType::PointCloudSource PointCloudSource;
  typedef typename PointCloudSource::Ptr PointCloudSourcePtr;
  typedef typename PointCloudSource::ConstPtr PointCloudSourceConstPtr;

  typedef typename BaseRegType::PointCloudTarget PointCloudTarget;
  typedef typename PointCloudTarget::Ptr PointCloudTargetPtr;
  typedef typename PointCloudTarget::ConstPtr PointCloudTargetConstPtr;

  /** \brief Typename of searchable voxel grid containing mean and covariance. */
  typedef pclomp::MultiVoxelGridCovariance<PointTarget> TargetGrid;
  /** \brief Typename of const pointer to searchable voxel grid leaf. */
  typedef typename TargetGrid::LeafConstPtr TargetGridLeafConstPtr;

public:
#if PCL_VERSION >= PCL_VERSION_CALC(1, 10, 0)
  typedef pcl::shared_ptr<MultiGridNormalDistributionsTransform<PointSource, PointTarget>> Ptr;
  typedef pcl::shared_ptr<const MultiGridNormalDistributionsTransform<PointSource, PointTarget>>
    ConstPtr;
#else
  typedef boost::shared_ptr<MultiGridNormalDistributionsTransform<PointSource, PointTarget>> Ptr;
  typedef boost::shared_ptr<const MultiGridNormalDistributionsTransform<PointSource, PointTarget>>
    ConstPtr;
#endif

  /** \brief Constructor.
   * Sets \ref outlier_ratio_ to 0.35, \ref params_.step_size to 0.05 and \ref params_.resolution
   * to 1.0
   */
  MultiGridNormalDistributionsTransform();

  // Copy & move constructor
  MultiGridNormalDistributionsTransform(const MultiGridNormalDistributionsTransform & other);
  MultiGridNormalDistributionsTransform(MultiGridNormalDistributionsTransform && other) noexcept;

  // Copy & move assignments
  MultiGridNormalDistributionsTransform & operator=(
    const MultiGridNormalDistributionsTransform & other);
  MultiGridNormalDistributionsTransform & operator=(
    MultiGridNormalDistributionsTransform && other) noexcept;

  /** \brief Empty destructor */
  virtual ~MultiGridNormalDistributionsTransform() {}

  void setNumThreads(int n)
  {
    params_.num_threads = n;

    target_cells_.setThreadNum(params_.num_threads);
  }

  inline int getNumThreads() const { return params_.num_threads; }

  inline void setInputSource(const PointCloudSourceConstPtr & input)
  {
    // This is to avoid segmentation fault when setting null input
    // No idea why PCL does not check the nullity of input
    if (input) {
      BaseRegType::setInputSource(input);
    } else {
      std::cerr << "Error: Null input source cloud is not allowed" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  inline void setInputTarget(const PointCloudTargetConstPtr & cloud)
  {
    const std::string default_target_id = "default";
    addTarget(cloud, default_target_id);
    createVoxelKdtree();
  }

  /** \brief Provide a pointer to the input target (e.g., the point cloud that we want to align the
   * input source to). \param[in] cloud the input point cloud target
   */
  inline void addTarget(const PointCloudTargetConstPtr & cloud, const std::string & target_id)
  {
    BaseRegType::setInputTarget(cloud);
    target_cells_.setLeafSize(params_.resolution, params_.resolution, params_.resolution);
    target_cells_.setInputCloudAndFilter(cloud, target_id);
  }

  inline void removeTarget(const std::string & target_id) { target_cells_.removeCloud(target_id); }

  inline void createVoxelKdtree()
  {
    target_cells_.createKdtree();
    target_cloud_updated_ = false;
  }

  /** \brief Set/change the voxel grid resolution.
   * \param[in] resolution side length of voxels
   */
  inline void setResolution(float resolution)
  {
    // Prevents unnecessary voxel initiations
    if (params_.resolution != resolution) {
      params_.resolution = resolution;
      if (input_) init();
    }
  }

  /** \brief Get voxel grid resolution.
   * \return side length of voxels
   */
  inline float getResolution() const { return (params_.resolution); }

  /** \brief Get the newton line search maximum step length.
   * \return maximum step length
   */
  inline double getStepSize() const { return (params_.step_size); }

  /** \brief Set/change the newton line search maximum step length.
   * \param[in] step_size maximum step length
   */
  inline void setStepSize(double step_size) { params_.step_size = step_size; }

  /** \brief Get the point cloud outlier ratio.
   * \return outlier ratio
   */
  inline double getOutlierRatio() const { return (outlier_ratio_); }

  /** \brief Set/change the point cloud outlier ratio.
   * \param[in] outlier_ratio outlier ratio
   */
  inline void setOutlierRatio(double outlier_ratio) { outlier_ratio_ = outlier_ratio; }

  /** \brief Get the registration alignment probability.
   * \return transformation probability
   */
  inline double getTransformationProbability() const { return (trans_probability_); }

  inline double getNearestVoxelTransformationLikelihood() const
  {
    return nearest_voxel_transformation_likelihood_;
  }

  /** \brief Get the number of iterations required to calculate alignment.
   * \return final number of iterations
   */
  inline int getFinalNumIteration() const { return (nr_iterations_); }

  /** \brief Return the hessian matrix */
  inline Eigen::Matrix<double, 6, 6> getHessian() const { return hessian_; }

  /** \brief Return the transformation array */
  inline const std::vector<Eigen::Matrix4f, Eigen::aligned_allocator<Eigen::Matrix4f>> &
  getFinalTransformationArray() const
  {
    return transformation_array_;
  }

  /** \brief Convert 6 element transformation vector to affine transformation.
   * \param[in] x transformation vector of the form [x, y, z, roll, pitch, yaw]
   * \param[out] trans affine transform corresponding to given transformation vector
   */
  static void convertTransform(const Eigen::Matrix<double, 6, 1> & x, Eigen::Affine3f & trans)
  {
    trans = Eigen::Translation<float, 3>(
              static_cast<float>(x(0)), static_cast<float>(x(1)), static_cast<float>(x(2))) *
            Eigen::AngleAxis<float>(static_cast<float>(x(3)), Eigen::Vector3f::UnitX()) *
            Eigen::AngleAxis<float>(static_cast<float>(x(4)), Eigen::Vector3f::UnitY()) *
            Eigen::AngleAxis<float>(static_cast<float>(x(5)), Eigen::Vector3f::UnitZ());
  }

  /** \brief Convert 6 element transformation vector to transformation matrix.
   * \param[in] x transformation vector of the form [x, y, z, roll, pitch, yaw]
   * \param[out] trans 4x4 transformation matrix corresponding to given transformation vector
   */
  static void convertTransform(const Eigen::Matrix<double, 6, 1> & x, Eigen::Matrix4f & trans)
  {
    Eigen::Affine3f _affine;
    convertTransform(x, _affine);
    trans = _affine.matrix();
  }

  // negative log likelihood function
  // lower is better
  double calculateTransformationProbability(const PointCloudSource & cloud) const;
  double calculateNearestVoxelTransformationLikelihood(const PointCloudSource & cloud) const;
  pcl::PointCloud<pcl::PointXYZI> calculateNearestVoxelScoreEachPoint(
    const PointCloudSource & cloud) const;

  inline void setRegularizationScaleFactor(float regularization_scale_factor)
  {
    params_.regularization_scale_factor = regularization_scale_factor;
  }

  inline void setRegularizationPose(Eigen::Matrix4f regularization_pose)
  {
    regularization_pose_ = regularization_pose;
  }

  inline void unsetRegularizationPose() { regularization_pose_ = boost::none; }

  NdtResult getResult()
  {
    NdtResult ndt_result;
    ndt_result.pose = this->getFinalTransformation();
    ndt_result.transformation_array = getFinalTransformationArray();
    ndt_result.transform_probability = getTransformationProbability();
    ndt_result.transform_probability_array = transform_probability_array_;
    ndt_result.nearest_voxel_transformation_likelihood = getNearestVoxelTransformationLikelihood();
    ndt_result.nearest_voxel_transformation_likelihood_array =
      nearest_voxel_transformation_likelihood_array_;
    ndt_result.iteration_num = getFinalNumIteration();
    ndt_result.hessian = getHessian();
    return ndt_result;
  }

  /** \brief Set the transformation epsilon (maximum allowable translation squared
   * difference between two consecutive transformations) in order for an optimization to
   * be considered as having converged to the final solution. \param[in] epsilon the
   * transformation epsilon in order for an optimization to be considered as having
   * converged to the final solution.
   */
  inline void setTransformationEpsilon(double epsilon) { params_.trans_epsilon = epsilon; }

  /** \brief Get the transformation epsilon (maximum allowable translation squared
   * difference between two consecutive transformations) as set by the user.
   */
  inline double getTransformationEpsilon() { return (params_.trans_epsilon); }

  inline void setMaximumIterations(int max_iterations)
  {
    params_.max_iterations = max_iterations;
    max_iterations_ = params_.max_iterations;
  }

  inline int getMaxIterations() const { return params_.max_iterations; }

  inline int getMaxIterations() { return params_.max_iterations; }

  void setParams(const NdtParams & ndt_params)
  {
    params_ = ndt_params;
    max_iterations_ = params_.max_iterations;

    target_cells_.setThreadNum(params_.num_threads);
  }

  NdtParams getParams() const { return params_; }

  pcl::PointCloud<PointTarget> getVoxelPCD() const { return target_cells_.getVoxelPCD(); }

  std::vector<std::string> getCurrentMapIDs() const { return target_cells_.getCurrentMapIDs(); }

protected:
  using BaseRegType::converged_;
  using BaseRegType::final_transformation_;
  using BaseRegType::input_;
  using BaseRegType::max_iterations_;
  using BaseRegType::nr_iterations_;
  using BaseRegType::previous_transformation_;
  using BaseRegType::reg_name_;
  using BaseRegType::target_;
  using BaseRegType::target_cloud_updated_;
  using BaseRegType::transformation_;
  using BaseRegType::transformation_epsilon_;

  using BaseRegType::update_visualizer_;

  /** \brief Estimate the transformation and returns the transformed source (input) as output.
   * \param[out] output the resultant input transformed point cloud dataset
   */
  virtual void computeTransformation(PointCloudSource & output)
  {
    computeTransformation(output, Eigen::Matrix4f::Identity());
  }

  /** \brief Estimate the transformation and returns the transformed source (input) as output.
   * \param[out] output the resultant input transformed point cloud dataset
   * \param[in] guess the initial gross estimation of the transformation
   */
  virtual void computeTransformation(PointCloudSource & output, const Eigen::Matrix4f & guess);

  /** \brief Initiate covariance voxel structure. */
  void inline init() {}

  /** \brief Compute derivatives of probability function w.r.t. the transformation vector.
   * \note Equation 6.10, 6.12 and 6.13 [Magnusson 2009].
   * \param[out] score_gradient the gradient vector of the probability function w.r.t. the
   * transformation vector \param[out] hessian the hessian matrix of the probability function w.r.t.
   * the transformation vector \param[in] trans_cloud transformed point cloud \param[in] p the
   * current transform vector \param[in] compute_hessian flag to calculate hessian, unnecessary for
   * step calculation.
   */
  double computeDerivatives(
    Eigen::Matrix<double, 6, 1> & score_gradient, Eigen::Matrix<double, 6, 6> & hessian,
    PointCloudSource & trans_cloud, Eigen::Matrix<double, 6, 1> & p, bool compute_hessian = true);

  /** \brief Compute individual point contributions to derivatives of probability function w.r.t.
   * the transformation vector. \note Equation 6.10, 6.12 and 6.13 [Magnusson 2009]. \param[in,out]
   * score_gradient the gradient vector of the probability function w.r.t. the transformation vector
   * \param[in,out] hessian the hessian matrix of the probability function w.r.t. the transformation
   * vector \param[in] x_trans transformed point minus mean of occupied covariance voxel \param[in]
   * c_inv covariance of occupied covariance voxel \param[in] compute_hessian flag to calculate
   * hessian, unnecessary for step calculation.
   */
  double updateDerivatives(
    Eigen::Matrix<double, 6, 1> & score_gradient, Eigen::Matrix<double, 6, 6> & hessian,
    const Eigen::Matrix<double, 4, 6> & point_gradient,
    const Eigen::Matrix<double, 24, 6> & point_hessian, const Eigen::Vector3d & x_trans,
    const Eigen::Matrix3d & c_inv, bool compute_hessian = true) const;

  /** \brief Precompute angular components of derivatives.
   * \note Equation 6.19 and 6.21 [Magnusson 2009].
   * \param[in] p the current transform vector
   * \param[in] compute_hessian flag to calculate hessian, unnecessary for step calculation.
   */
  void computeAngleDerivatives(Eigen::Matrix<double, 6, 1> & p, bool compute_hessian = true);

  /** \brief Compute point derivatives.
   * \note Equation 6.18-21 [Magnusson 2009].
   * \param[in] x point from the input cloud
   * \param[in] compute_hessian flag to calculate hessian, unnecessary for step calculation.
   */
  void computePointDerivatives(
    Eigen::Vector3d & x, Eigen::Matrix<double, 4, 6> & point_gradient,
    Eigen::Matrix<double, 24, 6> & point_hessian, bool compute_hessian = true) const;

  /** \brief Compute hessian of probability function w.r.t. the transformation vector.
   * \note Equation 6.13 [Magnusson 2009].
   * \param[out] hessian the hessian matrix of the probability function w.r.t. the transformation
   * vector \param[in] trans_cloud transformed point cloud \param[in] p the current transform vector
   */
  void computeHessian(
    Eigen::Matrix<double, 6, 6> & hessian, PointCloudSource & trans_cloud,
    Eigen::Matrix<double, 6, 1> & p);

  /** \brief Compute individual point contributions to hessian of probability function w.r.t. the
   * transformation vector. \note Equation 6.13 [Magnusson 2009]. \param[in,out] hessian the hessian
   * matrix of the probability function w.r.t. the transformation vector \param[in] x_trans
   * transformed point minus mean of occupied covariance voxel \param[in] c_inv covariance of
   * occupied covariance voxel
   */
  void updateHessian(
    Eigen::Matrix<double, 6, 6> & hessian, const Eigen::Matrix<double, 4, 6> & point_gradient,
    const Eigen::Matrix<double, 24, 6> & point_hessian, const Eigen::Vector3d & x_trans,
    const Eigen::Matrix3d & c_inv) const;

  /** \brief Compute line search step length and update transform and probability derivatives using
   * More-Thuente method. \note Search Algorithm [More, Thuente 1994] \param[in] x initial
   * transformation vector, \f$ x \f$ in Equation 1.3 (Moore, Thuente 1994) and \f$ \vec{p} \f$ in
   * Algorithm 2 [Magnusson 2009] \param[in] step_dir descent direction, \f$ p \f$ in Equation 1.3
   * (Moore, Thuente 1994) and \f$ \delta \vec{p} \f$ normalized in Algorithm 2 [Magnusson 2009]
   * \param[in] step_init initial step length estimate, \f$ \alpha_0 \f$ in Moore-Thuente (1994) and
   * the normal of \f$ \delta \vec{p} \f$ in Algorithm 2 [Magnusson 2009] \param[in] step_max
   * maximum step length, \f$ \alpha_max \f$ in Moore-Thuente (1994) \param[in] step_min minimum
   * step length, \f$ \alpha_min \f$ in Moore-Thuente (1994) \param[out] score final score function
   * value, \f$ f(x + \alpha p) \f$ in Equation 1.3 (Moore, Thuente 1994) and \f$ score \f$ in
   * Algorithm 2 [Magnusson 2009] \param[in,out] score_gradient gradient of score function w.r.t.
   * transformation vector, \f$ f'(x + \alpha p) \f$ in Moore-Thuente (1994) and \f$ \vec{g} \f$ in
   * Algorithm 2 [Magnusson 2009] \param[out] hessian hessian of score function w.r.t.
   * transformation vector, \f$ f''(x + \alpha p) \f$ in Moore-Thuente (1994) and \f$ H \f$ in
   * Algorithm 2 [Magnusson 2009] \param[in,out] trans_cloud transformed point cloud, \f$ X \f$
   * transformed by \f$ T(\vec{p},\vec{x}) \f$ in Algorithm 2 [Magnusson 2009] \return final step
   * length
   */
  double computeStepLengthMT(
    const Eigen::Matrix<double, 6, 1> & x, Eigen::Matrix<double, 6, 1> & step_dir, double step_init,
    double step_max, double step_min, double & score, Eigen::Matrix<double, 6, 1> & score_gradient,
    Eigen::Matrix<double, 6, 6> & hessian, PointCloudSource & trans_cloud);

  /** \brief Update interval of possible step lengths for More-Thuente method, \f$ I \f$ in
   * More-Thuente (1994) \note Updating Algorithm until some value satisfies \f$ \psi(\alpha_k) \leq
   * 0 \f$ and \f$ \phi'(\alpha_k) \geq 0 \f$ and Modified Updating Algorithm from then on [More,
   * Thuente 1994]. \param[in,out] a_l first endpoint of interval \f$ I \f$, \f$ \alpha_l \f$ in
   * Moore-Thuente (1994) \param[in,out] f_l value at first endpoint, \f$ f_l \f$ in Moore-Thuente
   * (1994), \f$ \psi(\alpha_l) \f$ for Update Algorithm and \f$ \phi(\alpha_l) \f$ for Modified
   * Update Algorithm \param[in,out] g_l derivative at first endpoint, \f$ g_l \f$ in Moore-Thuente
   * (1994), \f$ \psi'(\alpha_l) \f$ for Update Algorithm and \f$ \phi'(\alpha_l) \f$ for Modified
   * Update Algorithm \param[in,out] a_u second endpoint of interval \f$ I \f$, \f$ \alpha_u \f$ in
   * Moore-Thuente (1994) \param[in,out] f_u value at second endpoint, \f$ f_u \f$ in Moore-Thuente
   * (1994), \f$ \psi(\alpha_u) \f$ for Update Algorithm and \f$ \phi(\alpha_u) \f$ for Modified
   * Update Algorithm \param[in,out] g_u derivative at second endpoint, \f$ g_u \f$ in Moore-Thuente
   * (1994), \f$ \psi'(\alpha_u) \f$ for Update Algorithm and \f$ \phi'(\alpha_u) \f$ for Modified
   * Update Algorithm \param[in] a_t trial value, \f$ \alpha_t \f$ in Moore-Thuente (1994)
   * \param[in] f_t value at trial value, \f$ f_t \f$ in Moore-Thuente (1994), \f$ \psi(\alpha_t)
   * \f$ for Update Algorithm and \f$ \phi(\alpha_t) \f$ for Modified Update Algorithm \param[in]
   * g_t derivative at trial value, \f$ g_t \f$ in Moore-Thuente (1994), \f$ \psi'(\alpha_t) \f$ for
   * Update Algorithm and \f$ \phi'(\alpha_t) \f$ for Modified Update Algorithm \return if interval
   * converges
   */
  bool updateIntervalMT(
    double & a_l, double & f_l, double & g_l, double & a_u, double & f_u, double & g_u, double a_t,
    double f_t, double g_t);

  /** \brief Select new trial value for More-Thuente method.
   * \note Trial Value Selection [More, Thuente 1994], \f$ \psi(\alpha_k) \f$ is used for \f$ f_k
   * \f$ and \f$ g_k \f$ until some value satisfies the test \f$ \psi(\alpha_k) \leq 0 \f$ and \f$
   * \phi'(\alpha_k) \geq 0 \f$ then \f$ \phi(\alpha_k) \f$ is used from then on. \note
   * Interpolation Minimizer equations from Optimization Theory and Methods: Nonlinear Programming
   * By Wenyu Sun, Ya-xiang Yuan (89-100). \param[in] a_l first endpoint of interval \f$ I \f$, \f$
   * \alpha_l \f$ in Moore-Thuente (1994) \param[in] f_l value at first endpoint, \f$ f_l \f$ in
   * Moore-Thuente (1994) \param[in] g_l derivative at first endpoint, \f$ g_l \f$ in Moore-Thuente
   * (1994) \param[in] a_u second endpoint of interval \f$ I \f$, \f$ \alpha_u \f$ in Moore-Thuente
   * (1994) \param[in] f_u value at second endpoint, \f$ f_u \f$ in Moore-Thuente (1994) \param[in]
   * g_u derivative at second endpoint, \f$ g_u \f$ in Moore-Thuente (1994) \param[in] a_t previous
   * trial value, \f$ \alpha_t \f$ in Moore-Thuente (1994) \param[in] f_t value at previous trial
   * value, \f$ f_t \f$ in Moore-Thuente (1994) \param[in] g_t derivative at previous trial value,
   * \f$ g_t \f$ in Moore-Thuente (1994) \return new trial value
   */
  double trialValueSelectionMT(
    double a_l, double f_l, double g_l, double a_u, double f_u, double g_u, double a_t, double f_t,
    double g_t);

  /** \brief Auxiliary function used to determine endpoints of More-Thuente interval.
   * \note \f$ \psi(\alpha) \f$ in Equation 1.6 (Moore, Thuente 1994)
   * \param[in] a the step length, \f$ \alpha \f$ in More-Thuente (1994)
   * \param[in] f_a function value at step length a, \f$ \phi(\alpha) \f$ in More-Thuente (1994)
   * \param[in] f_0 initial function value, \f$ \phi(0) \f$ in Moore-Thuente (1994)
   * \param[in] g_0 initial function gradient, \f$ \phi'(0) \f$ in More-Thuente (1994)
   * \param[in] mu the step length, constant \f$ \mu \f$ in Equation 1.1 [More, Thuente 1994]
   * \return sufficient decrease value
   */
  inline double auxiliaryFunction_PsiMT(
    double a, double f_a, double f_0, double g_0, double mu = 1.e-4)
  {
    return (f_a - f_0 - mu * g_0 * a);
  }

  /** \brief Auxiliary function derivative used to determine endpoints of More-Thuente interval.
   * \note \f$ \psi'(\alpha) \f$, derivative of Equation 1.6 (Moore, Thuente 1994)
   * \param[in] g_a function gradient at step length a, \f$ \phi'(\alpha) \f$ in More-Thuente (1994)
   * \param[in] g_0 initial function gradient, \f$ \phi'(0) \f$ in More-Thuente (1994)
   * \param[in] mu the step length, constant \f$ \mu \f$ in Equation 1.1 [More, Thuente 1994]
   * \return sufficient decrease derivative
   */
  inline double auxiliaryFunction_dPsiMT(double g_a, double g_0, double mu = 1.e-4)
  {
    return (g_a - mu * g_0);
  }

  /** \brief The voxel grid generated from target cloud containing point means and covariances. */
  TargetGrid target_cells_;

  // double fitness_epsilon_;

  /** \brief The ratio of outliers of points w.r.t. a normal distribution, Equation 6.7 [Magnusson
   * 2009]. */
  double outlier_ratio_;

  /** \brief The normalization constants used fit the point distribution to a normal distribution,
   * Equation 6.8 [Magnusson 2009]. */
  double gauss_d1_, gauss_d2_, gauss_d3_;

  /** \brief The probability score of the transform applied to the input cloud, Equation 6.9
   * and 6.10 [Magnusson 2009]. */
  double trans_probability_;

  /** \brief Precomputed Angular Gradient
   *
   * The precomputed angular derivatives for the jacobian of a transformation vector, Equation 6.19
   * [Magnusson 2009].
   */
  Eigen::Matrix<double, 8, 4> j_ang_;

  /** \brief Precomputed Angular Hessian
   *
   * The precomputed angular derivatives for the hessian of a transformation vector, Equation 6.19
   * [Magnusson 2009].
   */
  Eigen::Matrix<double, 16, 4> h_ang_;

  Eigen::Matrix<double, 6, 6> hessian_;
  std::vector<Eigen::Matrix4f, Eigen::aligned_allocator<Eigen::Matrix4f>> transformation_array_;
  std::vector<float> transform_probability_array_;
  std::vector<float> nearest_voxel_transformation_likelihood_array_;
  double nearest_voxel_transformation_likelihood_{};

  boost::optional<Eigen::Matrix4f> regularization_pose_;
  Eigen::Vector3f regularization_pose_translation_;

  NdtParams params_;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}  // namespace pclomp

#endif  // AUTOWARE__NDT_SCAN_MATCHER__NDT_OMP__MULTIGRID_NDT_OMP_H_

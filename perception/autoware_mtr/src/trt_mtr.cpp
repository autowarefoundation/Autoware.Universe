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

#include "autoware/mtr/trt_mtr.hpp"

#include "autoware/mtr/cuda_helper.hpp"
#include "autoware/mtr/intention_point.hpp"
#include "autoware/mtr/trajectory.hpp"
#include "postprocess/postprocess_kernel.cuh"
#include "preprocess/agent_preprocess_kernel.cuh"
#include "preprocess/polyline_preprocess_kernel.cuh"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace autoware::mtr
{
TrtMTR::TrtMTR(
  const std::string & model_path, const MTRConfig & config, const BuildConfig & build_config,
  const size_t max_workspace_size)
: config_(config),
  intention_point_(IntentionPoint::from_file(
    config_.num_intention_point_cluster, config_.intention_point_filepath))
{
  max_num_polyline_ = config_.max_num_polyline;
  num_mode_ = config_.num_mode;
  num_future_ = config_.num_future;
  num_intention_point_ = config_.num_intention_point_cluster;

  // build engine
  builder_ = std::make_unique<MTRBuilder>(model_path, build_config, max_workspace_size);
  builder_->setup();

  if (!builder_->isInitialized()) {
    return;
  }

  CHECK_CUDA_ERROR(cudaStreamCreate(&stream_));
}

bool TrtMTR::doInference(
  const AgentData & agent_data, const PolylineData & polyline_data,
  std::vector<PredictedTrajectory> & trajectories)
{
  initCudaPtr(agent_data, polyline_data);

  if (!preProcess(agent_data, polyline_data)) {
    std::cerr << "Fail to preprocess" << std::endl;
    return false;
  }

  std::vector<void *> buffer = {d_in_trajectory_.get(),      d_in_trajectory_mask_.get(),
                                d_in_polyline_.get(),        d_in_polyline_mask_.get(),
                                d_in_polyline_center_.get(), d_in_last_pos_.get(),
                                d_target_index_.get(),       d_intention_point_.get(),
                                d_out_trajectory_.get(),     d_out_score_.get()};

  if (!builder_->enqueueV2(buffer.data(), stream_, nullptr)) {
    std::cerr << "Fail to do inference" << std::endl;
    return false;
  }

  if (!postProcess(agent_data, trajectories)) {
    std::cerr << "Fail to postprocess" << std::endl;
    return false;
  }

  return true;
}

void TrtMTR::initCudaPtr(const AgentData & agent_data, const PolylineData & polyline_data)
{
  num_target_ = agent_data.num_target();
  num_agent_ = agent_data.num_agent();
  num_timestamp_ = agent_data.time_length();
  num_agent_attr_ = agent_data.input_dim();
  num_polyline_ = polyline_data.num_polyline();
  num_point_ = polyline_data.num_point();
  num_point_attr_ = polyline_data.input_dim();

  // source data
  d_target_index_ = cuda::make_unique<int[]>(num_target_);
  d_label_index_ = cuda::make_unique<int[]>(num_agent_);
  d_timestamp_ = cuda::make_unique<float[]>(num_timestamp_);
  d_trajectory_ = cuda::make_unique<float[]>(agent_data.size());
  d_target_state_ = cuda::make_unique<float[]>(num_target_ * agent_data.state_dim());
  d_intention_point_ = cuda::make_unique<float[]>(num_target_ * intention_point_.size());
  d_polyline_ = cuda::make_unique<float[]>(polyline_data.size());

  // preprocessed input
  d_in_trajectory_ =
    cuda::make_unique<float[]>(num_target_ * num_agent_ * num_timestamp_ * num_agent_attr_);
  d_in_trajectory_mask_ = cuda::make_unique<bool[]>(num_target_ * num_agent_ * num_timestamp_);
  d_in_last_pos_ = cuda::make_unique<float[]>(num_target_ * num_agent_ * 3);
  d_in_polyline_ =
    cuda::make_unique<float[]>(num_target_ * max_num_polyline_ * num_point_ * num_point_attr_);
  d_in_polyline_mask_ = cuda::make_unique<bool[]>(num_target_ * max_num_polyline_ * num_point_);
  d_in_polyline_center_ = cuda::make_unique<float[]>(num_target_ * max_num_polyline_ * 3);

  if (max_num_polyline_ < num_polyline_) {
    d_tmp_polyline_ =
      cuda::make_unique<float[]>(num_target_ * num_polyline_ * num_point_ * num_point_attr_);
    d_tmp_polyline_mask_ = cuda::make_unique<bool[]>(num_target_ * num_polyline_ * num_point_);
    d_tmp_distance_ = cuda::make_unique<float[]>(num_target_ * num_polyline_);
  }

  // outputs
  d_out_score_ = cuda::make_unique<float[]>(num_target_ * num_mode_);
  d_out_trajectory_ =
    cuda::make_unique<float[]>(num_target_ * num_mode_ * num_future_ * PredictedStateDim);

  if (builder_->isDynamic()) {
    // trajectory: (B, N, T, Da)
    builder_->setBindingDimensions(
      0, nvinfer1::Dims4{num_target_, num_agent_, num_timestamp_, num_agent_attr_});
    // trajectory mask: (B, N, T)
    builder_->setBindingDimensions(1, nvinfer1::Dims3{num_target_, num_agent_, num_timestamp_});
    // polyline: (B, K, P, Dp)
    builder_->setBindingDimensions(
      2, nvinfer1::Dims4{num_target_, max_num_polyline_, num_point_, num_point_attr_});
    // polyline mask: (B, K, P)
    builder_->setBindingDimensions(3, nvinfer1::Dims3{num_target_, max_num_polyline_, num_point_});
    // polyline center: (B, K, 3)
    builder_->setBindingDimensions(4, nvinfer1::Dims3{num_target_, max_num_polyline_, 3});
    // agent last position: (B, N, 3)
    builder_->setBindingDimensions(5, nvinfer1::Dims3{num_target_, num_agent_, 3});
    // target indices: (B,)
    nvinfer1::Dims targetIdxDim;
    targetIdxDim.nbDims = 1;
    targetIdxDim.d[0] = num_target_;
    builder_->setBindingDimensions(6, targetIdxDim);
    // intention points: (B, I, 2)
    builder_->setBindingDimensions(7, nvinfer1::Dims3{num_target_, num_intention_point_, 2});
  }
}

bool TrtMTR::preProcess(const AgentData & agent_data, const PolylineData & polyline_data)
{
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_target_index_.get(), agent_data.target_indices().data(), sizeof(int) * num_target_,
    cudaMemcpyHostToDevice, stream_));
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_label_index_.get(), agent_data.label_ids().data(), sizeof(int) * num_agent_,
    cudaMemcpyHostToDevice, stream_));
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_timestamp_.get(), agent_data.timestamps().data(), sizeof(float) * num_timestamp_,
    cudaMemcpyHostToDevice, stream_));
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_trajectory_.get(), agent_data.data_ptr(), sizeof(float) * agent_data.size(),
    cudaMemcpyHostToDevice, stream_));
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_target_state_.get(), agent_data.current_target_data_ptr(),
    sizeof(float) * num_target_ * agent_data.state_dim(), cudaMemcpyHostToDevice, stream_));

  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_polyline_.get(), polyline_data.data_ptr(), sizeof(float) * polyline_data.size(),
    cudaMemcpyHostToDevice, stream_));

  const auto target_label_names = getLabelNames(agent_data.target_label_ids());
  const auto intention_point = intention_point_.as_array(target_label_names);
  CHECK_CUDA_ERROR(cudaMemcpyAsync(
    d_intention_point_.get(), intention_point.data(),
    sizeof(float) * num_target_ * intention_point_.size(), cudaMemcpyHostToDevice, stream_));

  CHECK_CUDA_ERROR(agentPreprocessLauncher(
    num_target_, num_agent_, num_timestamp_, agent_data.state_dim(), agent_data.num_class(),
    agent_data.ego_index(), d_target_index_.get(), d_label_index_.get(), d_timestamp_.get(),
    d_trajectory_.get(), d_in_trajectory_.get(), d_in_trajectory_mask_.get(), d_in_last_pos_.get(),
    stream_));

  if (max_num_polyline_ < num_polyline_) {
    CHECK_CUDA_ERROR(polylinePreprocessWithTopkLauncher(
      max_num_polyline_, num_polyline_, num_point_, polyline_data.state_dim(), d_polyline_.get(),
      num_target_, agent_data.state_dim(), d_target_state_.get(), d_tmp_polyline_.get(),
      d_tmp_polyline_mask_.get(), d_tmp_distance_.get(), d_in_polyline_.get(),
      d_in_polyline_mask_.get(), d_in_polyline_center_.get(), stream_));
  } else {
    CHECK_CUDA_ERROR(polylinePreprocessLauncher(
      num_polyline_, num_point_, polyline_data.state_dim(), d_polyline_.get(), num_target_,
      agent_data.state_dim(), d_target_state_.get(), d_in_polyline_.get(),
      d_in_polyline_mask_.get(), d_in_polyline_center_.get(), stream_));
  }
  return true;
}

bool TrtMTR::postProcess(
  const AgentData & agent_data, std::vector<PredictedTrajectory> & trajectories)
{
  CHECK_CUDA_ERROR(postprocessLauncher(
    num_target_, num_mode_, num_future_, agent_data.state_dim(), d_target_state_.get(),
    PredictedStateDim, d_out_trajectory_.get(), stream_));

  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  h_out_score_.clear();
  h_out_trajectory_.clear();
  h_out_score_.resize(num_target_ * num_mode_);
  h_out_trajectory_.resize(num_target_ * num_mode_ * num_future_ * PredictedStateDim);

  CHECK_CUDA_ERROR(cudaMemcpy(
    h_out_score_.data(), d_out_score_.get(), sizeof(float) * num_target_ * num_mode_,
    cudaMemcpyDeviceToHost));
  CHECK_CUDA_ERROR(cudaMemcpy(
    h_out_trajectory_.data(), d_out_trajectory_.get(),
    sizeof(float) * num_target_ * num_mode_ * num_future_ * PredictedStateDim,
    cudaMemcpyDeviceToHost));

  trajectories.clear();
  trajectories.reserve(num_target_);
  for (auto b = 0; b < num_target_; ++b) {
    const auto score_itr = h_out_score_.cbegin() + b * num_mode_;
    const std::vector<double> scores(score_itr, score_itr + num_mode_);
    const auto mode_itr =
      h_out_trajectory_.cbegin() + b * num_mode_ * num_future_ * PredictedStateDim;
    std::vector<double> modes(mode_itr, mode_itr + num_mode_ * num_future_ * PredictedStateDim);
    trajectories.emplace_back(scores, modes, num_mode_, num_future_);
  }
  return true;
}
}  // namespace autoware::mtr

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

#ifndef AUTOWARE__MTR__TRT_MTR_HPP_
#define AUTOWARE__MTR__TRT_MTR_HPP_

#include "attention/trt_attn_value_computation.hpp"
#include "attention/trt_attn_weight_computation.hpp"
#include "autoware/mtr/agent.hpp"
#include "autoware/mtr/builder.hpp"
#include "autoware/mtr/cuda_helper.hpp"
#include "autoware/mtr/intention_point.hpp"
#include "autoware/mtr/polyline.hpp"
#include "autoware/mtr/trajectory.hpp"
#include "knn/trt_knn_batch.hpp"
#include "knn/trt_knn_batch_mlogk_kernel.hpp"

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::mtr
{
/**
 * @brief A configuration of MTR.
 */
struct MTRConfig
{
  /**
   * @brief Construct a new instance.
   *
   * @param target_labels An array of target label names.
   * @param num_past The number of past timestamps.
   * @param num_mode The number of modes.
   * @param num_future The number of future time step length predicted by MTR.
   * @param max_num_polyline The max number of polylines which can be contained in a single input.
   * @param max_num_point The max number of points contained in each polyline.
   * @param intention_point_filepath The path to intention points file.
   * @param num_intention_point_cluster The number of clusters for intension points.
   */
  explicit MTRConfig(
    const std::vector<std::string> & target_labels = {"VEHICLE", "PEDESTRIAN", "CYCLIST"},
    const size_t num_past = 11, const size_t num_mode = 6, const size_t num_future = 80,
    const size_t max_num_polyline = 768, const size_t max_num_point = 20,
    const float point_break_distance = 1.0f,
    std::string intention_point_filepath = "./data/intention_point.csv",
    const size_t num_intention_point_cluster = 64)
  : target_labels(target_labels),
    num_class(target_labels.size()),
    num_past(num_past),
    num_mode(num_mode),
    num_future(num_future),
    max_num_polyline(max_num_polyline),
    max_num_point(max_num_point),
    point_break_distance(point_break_distance),
    intention_point_filepath(std::move(intention_point_filepath)),
    num_intention_point_cluster(num_intention_point_cluster)
  {
  }

  std::vector<std::string> target_labels;
  size_t num_class;
  size_t num_past;
  size_t num_mode;
  size_t num_future;
  size_t max_num_polyline;
  size_t max_num_point;
  float point_break_distance;
  std::string intention_point_filepath;
  size_t num_intention_point_cluster;
};  // struct MTRConfig

/**
 * @brief A class to inference with MTR.
 */
class TrtMTR
{
public:
  /**
   * @brief Construct a new instance.
   *
   * @param model_path The path to engine or onnx file.
   * @param config The configuration of model.
   * @param build_config The configuration of build.
   * @param max_workspace_size The max size of workspace.
   * @param build_config The configuration of build.
   */
  explicit TrtMTR(
    const std::string & model_path, const MTRConfig & config = MTRConfig(),
    const BuildConfig & build_config = BuildConfig(),
    const size_t max_workspace_size = (1ULL << 30));

  /**
   * @brief Execute inference.
   *
   * @param agent_data The agent data to input.
   * @param polyline_data The polyline data to input.
   * @param trajectories A container to store predicted trajectories.
   * @return True if the inference finishes successfully.
   */
  bool doInference(
    const AgentData & agent_data, const PolylineData & polyline_data,
    std::vector<PredictedTrajectory> & trajectories);

  /**
   * @brief Return the model configuration.
   *
   * @return const MtrConfig& The model configuration which can not be updated.
   */
  const MTRConfig & config() const { return config_; }

private:
  /**
   * @brief Initialize pointers of cuda containers.
   *
   * @param agent_data The input agent data.
   * @param polyline_data The input polyline data.
   */
  void initCudaPtr(const AgentData & agent_data, const PolylineData & polyline_data);

  /**
   * @brief Execute pre-process.
   *
   * @param agent_data The input agent data.
   * @param polyline_data The input polyline data.
   * @return True if the pre-process finishes successfully.
   */
  bool preProcess(const AgentData & agent_data, const PolylineData & polyline_data);

  /**
   * @brief Execute post-process.
   *
   * @param agent_data The input agent data.
   * @param trajectories A container to store predicted trajectories.
   * @return True if the post-process finishes successfully.
   */
  bool postProcess(const AgentData & agent_data, std::vector<PredictedTrajectory> & trajectories);

  // model parameters
  MTRConfig config_;

  std::unique_ptr<MTRBuilder> builder_;
  cudaStream_t stream_{nullptr};

  IntentionPoint intention_point_;

  // data size
  // load from input data
  int32_t num_target_, num_agent_, num_timestamp_, num_agent_attr_;
  int32_t num_polyline_, num_point_, num_point_dim_, num_point_attr_;
  // load from config
  int32_t max_num_polyline_, num_mode_, num_future_, num_intention_point_;

  // source data
  cuda::unique_ptr<int[]> d_target_index_{nullptr};
  cuda::unique_ptr<int[]> d_label_index_{nullptr};
  cuda::unique_ptr<float[]> d_timestamp_{nullptr};
  cuda::unique_ptr<float[]> d_trajectory_{nullptr};
  cuda::unique_ptr<float[]> d_target_state_{nullptr};
  cuda::unique_ptr<float[]> d_intention_point_{nullptr};
  cuda::unique_ptr<float[]> d_polyline_{nullptr};

  // preprocessed inputs
  cuda::unique_ptr<float[]> d_in_trajectory_{nullptr};
  cuda::unique_ptr<bool[]> d_in_trajectory_mask_{nullptr};
  cuda::unique_ptr<float[]> d_in_last_pos_{nullptr};
  cuda::unique_ptr<float[]> d_in_polyline_{nullptr};
  cuda::unique_ptr<bool[]> d_in_polyline_mask_{nullptr};
  cuda::unique_ptr<float[]> d_in_polyline_center_{nullptr};
  // only used for topk extraction
  cuda::unique_ptr<float[]> d_tmp_polyline_{nullptr};
  cuda::unique_ptr<bool[]> d_tmp_polyline_mask_{nullptr};
  cuda::unique_ptr<float[]> d_tmp_distance_{nullptr};

  // outputs
  cuda::unique_ptr<float[]> d_out_score_{nullptr};
  cuda::unique_ptr<float[]> d_out_trajectory_{nullptr};
  std::vector<float> h_out_score_;
  std::vector<float> h_out_trajectory_;
};  // class TrtMTR
}  // namespace autoware::mtr
#endif  // AUTOWARE__MTR__TRT_MTR_HPP_

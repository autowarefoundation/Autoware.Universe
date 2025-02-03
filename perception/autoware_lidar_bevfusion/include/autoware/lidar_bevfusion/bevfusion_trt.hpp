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

#ifndef AUTOWARE__LIDAR_BEVFUSION__BEVFUSION_TRT_HPP_
#define AUTOWARE__LIDAR_BEVFUSION__BEVFUSION_TRT_HPP_

#include "autoware/lidar_bevfusion/postprocess/postprocess_kernel.hpp"
#include "autoware/lidar_bevfusion/preprocess/pointcloud_densification.hpp"
#include "autoware/lidar_bevfusion/preprocess/preprocess_kernel.hpp"
#include "autoware/lidar_bevfusion/preprocess/voxel_generator.hpp"
#include "autoware/lidar_bevfusion/utils.hpp"
#include "autoware/lidar_bevfusion/visibility_control.hpp"

#include <autoware/cuda_utils/cuda_check_error.hpp>
#include <autoware/cuda_utils/cuda_unique_ptr.hpp>
#include <autoware/tensorrt_common/tensorrt_common.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>

#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace autoware::lidar_bevfusion
{

using autoware::cuda_utils::CudaUniquePtr;

class NetworkParam
{
public:
  NetworkParam(std::string onnx_path, std::string engine_path, std::string trt_precision)
  : onnx_path_(std::move(onnx_path)),
    engine_path_(std::move(engine_path)),
    trt_precision_(std::move(trt_precision))
  {
  }

  std::string onnx_path() const { return onnx_path_; }
  std::string engine_path() const { return engine_path_; }
  std::string trt_precision() const { return trt_precision_; }

private:
  std::string onnx_path_;
  std::string engine_path_;
  std::string trt_precision_;
};

class LIDAR_BEVFUSION_PUBLIC BEVFusionTRT
{
public:
  using Matrix4fRowM = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;

  explicit BEVFusionTRT(
    const tensorrt_common::TrtCommonConfig & trt_config,
    const DensificationParam & densification_param, const BEVFusionConfig & config);
  virtual ~BEVFusionTRT();

  bool detect(
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr & msg,
    const std::vector<sensor_msgs::msg::Image::ConstSharedPtr> & image_msgs,
    const std::vector<float> & camera_masks, const tf2_ros::Buffer & tf_buffer,
    std::vector<Box3D> & det_boxes3d, std::unordered_map<std::string, double> & proc_timing);

  void setIntrinsicsExtrinsics(
    std::vector<sensor_msgs::msg::CameraInfo> & camera_info_vector,
    std::vector<Matrix4fRowM> & lidar2camera_vector);

protected:
  void initPtr();
  void initTrt(const tensorrt_common::TrtCommonConfig & trt_config);

  bool preProcess(
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr & pc_msg,
    const std::vector<sensor_msgs::msg::Image::ConstSharedPtr> & image_msgs,
    const std::vector<float> & camera_masks, const tf2_ros::Buffer & tf_buffer);

  bool inference();

  bool postProcess(std::vector<Box3D> & det_boxes3d);

  std::unique_ptr<autoware::tensorrt_common::TrtCommon> network_trt_ptr_{nullptr};
  std::unique_ptr<VoxelGenerator> vg_ptr_{nullptr};
  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_{
    nullptr};
  std::unique_ptr<PreprocessCuda> pre_ptr_{nullptr};
  std::unique_ptr<PostprocessCuda> post_ptr_{nullptr};
  cudaStream_t stream_{nullptr};
  std::vector<cudaStream_t> camera_streams_{};

  BEVFusionConfig config_;
  std::vector<int> roi_start_y_vector_;

  // pre-process inputs

  unsigned int voxel_features_size_{0};
  unsigned int voxel_coords_size_{0};
  unsigned int bbox_pred_size_{0};

  // lidar buffers
  CudaUniquePtr<float[]> points_d_{nullptr};
  CudaUniquePtr<float[]> voxel_features_d_{nullptr};
  CudaUniquePtr<std::int32_t[]> voxel_coords_d_{nullptr};
  CudaUniquePtr<std::int32_t[]> num_points_per_voxel_d_{nullptr};

  // pre computed tensors
  std::int64_t num_geom_feats_{};
  std::int64_t num_kept_{};
  std::int64_t num_ranks_{};
  std::int64_t num_indices_{};
  CudaUniquePtr<float_t[]> lidar2image_d_{};
  CudaUniquePtr<std::int32_t[]> geom_feats_d_{};
  CudaUniquePtr<std::uint8_t[]> kept_d_{};
  CudaUniquePtr<std::int64_t[]> ranks_d_{};
  CudaUniquePtr<std::int64_t[]> indices_d_{};

  // image buffers
  CudaUniquePtr<std::uint8_t[]> roi_tensor_d_{nullptr};
  std::vector<CudaUniquePtr<std::uint8_t[]>> image_buffers_d_{};
  CudaUniquePtr<float[]> camera_masks_d_{nullptr};

  // output buffers
  CudaUniquePtr<std::int64_t[]> label_pred_output_d_{nullptr};
  CudaUniquePtr<float[]> bbox_pred_output_d_{nullptr};
  CudaUniquePtr<float[]> score_output_d_{nullptr};
};

}  // namespace autoware::lidar_bevfusion

#endif  // AUTOWARE__LIDAR_BEVFUSION__BEVFUSION_TRT_HPP_

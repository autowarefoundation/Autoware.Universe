// Copyright 2021 TIER IV, Inc.
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

#ifndef AUTOWARE__LIDAR_CENTERPOINT__CENTERPOINT_TRT_HPP_
#define AUTOWARE__LIDAR_CENTERPOINT__CENTERPOINT_TRT_HPP_

#include "autoware/lidar_centerpoint/cuda_utils.hpp"
#include "autoware/lidar_centerpoint/postprocess/postprocess_kernel.hpp"
#include "autoware/lidar_centerpoint/preprocess/voxel_generator.hpp"

#include <autoware/tensorrt_common/tensorrt_common.hpp>
#include <cuda_blackboard/cuda_pointcloud2.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::lidar_centerpoint
{
using autoware::tensorrt_common::ProfileDims;
using autoware::tensorrt_common::TrtCommonConfig;

class CenterPointTRT
{
public:
  explicit CenterPointTRT(
    const TrtCommonConfig & encoder_param, const TrtCommonConfig & head_param,
    const DensificationParam & densification_param, const CenterPointConfig & config);

  virtual ~CenterPointTRT();

  bool detect(
    const std::shared_ptr<const cuda_blackboard::CudaPointCloud2> & input_pointcloud_msg_ptr,
    const tf2_ros::Buffer & tf_buffer, std::vector<Box3D> & det_boxes3d,
    bool & is_num_pillars_within_range);

protected:
  void initPtr();
  void initTrt(const TrtCommonConfig & encoder_param, const TrtCommonConfig & head_param);

  virtual bool preprocess(
    const std::shared_ptr<const cuda_blackboard::CudaPointCloud2> & input_pointcloud_msg_ptr,
    const tf2_ros::Buffer & tf_buffer);

  void inference();

  void postProcess(std::vector<Box3D> & det_boxes3d);

  std::unique_ptr<VoxelGeneratorTemplate> vg_ptr_{nullptr};
  std::unique_ptr<tensorrt_common::TrtCommon> encoder_trt_ptr_{nullptr};
  std::unique_ptr<tensorrt_common::TrtCommon> head_trt_ptr_{nullptr};
  std::unique_ptr<PostProcessCUDA> post_proc_ptr_{nullptr};
  cudaStream_t stream_{nullptr};

  std::size_t class_size_{0};
  CenterPointConfig config_;
  std::size_t encoder_in_feature_size_{0};
  std::size_t spatial_features_size_{0};
  std::size_t voxels_buffer_size_{0};
  std::size_t mask_size_{0};
  std::size_t voxels_size_{0};
  std::size_t coordinates_size_{0};
  std::vector<float> points_;
  cuda::unique_ptr<float[]> voxels_d_{nullptr};
  cuda::unique_ptr<int[]> coordinates_d_{nullptr};
  cuda::unique_ptr<float[]> num_points_per_voxel_d_{nullptr};
  cuda::unique_ptr<float[]> encoder_in_features_d_{nullptr};
  cuda::unique_ptr<float[]> pillar_features_d_{nullptr};
  cuda::unique_ptr<float[]> spatial_features_d_{nullptr};
  cuda::unique_ptr<float[]> head_out_heatmap_d_{nullptr};
  cuda::unique_ptr<float[]> head_out_offset_d_{nullptr};
  cuda::unique_ptr<float[]> head_out_z_d_{nullptr};
  cuda::unique_ptr<float[]> head_out_dim_d_{nullptr};
  cuda::unique_ptr<float[]> head_out_rot_d_{nullptr};
  cuda::unique_ptr<float[]> head_out_vel_d_{nullptr};
  cuda::unique_ptr<float[]> points_d_{nullptr};
  cuda::unique_ptr<float[]> points_aux_d_{nullptr};
  cuda::unique_ptr<float[]> voxels_buffer_d_{nullptr};
  cuda::unique_ptr<unsigned int[]> mask_d_{nullptr};
  cuda::unique_ptr<unsigned int[]> num_voxels_d_{nullptr};
  cuda::unique_ptr<unsigned int[]> shuffle_indices_d_{nullptr};
};

}  // namespace autoware::lidar_centerpoint

#endif  // AUTOWARE__LIDAR_CENTERPOINT__CENTERPOINT_TRT_HPP_

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

#ifndef AUTOWARE__LIDAR_BEVFUSION__PREPROCESS__VOXEL_GENERATOR_HPP_
#define AUTOWARE__LIDAR_BEVFUSION__PREPROCESS__VOXEL_GENERATOR_HPP_

#include "autoware/lidar_bevfusion/bevfusion_config.hpp"
#include "autoware/lidar_bevfusion/preprocess/pointcloud_densification.hpp"
#include "autoware/lidar_bevfusion/preprocess/preprocess_kernel.hpp"
#include "autoware/lidar_bevfusion/ros_utils.hpp"

#include <autoware/cuda_utils/cuda_check_error.hpp>
#include <autoware/cuda_utils/cuda_unique_ptr.hpp>
#include <autoware/point_types/types.hpp>
#include <tf2_eigen/tf2_eigen.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace autoware::lidar_bevfusion
{

class VoxelGenerator
{
public:
  explicit VoxelGenerator(
    const DensificationParam & densification_param, const BEVFusionConfig & config,
    cudaStream_t & stream);
  std::size_t generateSweepPoints(CudaUniquePtr<float[]> & points_d);
  bool enqueuePointCloud(
    const sensor_msgs::msg::PointCloud2 & msg, const tf2_ros::Buffer & tf_buffer);
  void initCloudInfo(const sensor_msgs::msg::PointCloud2 & msg);
  std::tuple<const std::uint32_t, const std::uint8_t, const std::uint8_t> getFieldInfo(
    const sensor_msgs::msg::PointCloud2 & msg, const std::string & field_name);

private:
  std::unique_ptr<PointCloudDensification> pd_ptr_{nullptr};
  std::unique_ptr<PreprocessCuda> pre_ptr_{nullptr};
  BEVFusionConfig config_;
  CudaUniquePtr<float[]> affine_past2current_d_{nullptr};
  std::vector<float> points_;
  cudaStream_t stream_;
};

}  // namespace autoware::lidar_bevfusion

#endif  // AUTOWARE__LIDAR_BEVFUSION__PREPROCESS__VOXEL_GENERATOR_HPP_

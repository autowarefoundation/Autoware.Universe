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

#include "autoware/lidar_bevfusion/preprocess/voxel_generator.hpp"

#include "autoware/lidar_bevfusion/preprocess/preprocess_kernel.hpp"

#include <sensor_msgs/point_cloud2_iterator.hpp>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>

namespace autoware::lidar_bevfusion
{

VoxelGenerator::VoxelGenerator(
  const DensificationParam & densification_param, const BEVFusionConfig & config,
  cudaStream_t & stream)
: config_(config), stream_(stream)
{
  pd_ptr_ = std::make_unique<PointCloudDensification>(densification_param, stream_);

  pre_ptr_ = std::make_unique<PreprocessCuda>(config_, stream_, false);

  affine_past2current_d_ =
    autoware::cuda_utils::make_unique<float[]>(Eigen::Affine3f::MatrixType::SizeAtCompileTime);
}

bool VoxelGenerator::enqueuePointCloud(
  const sensor_msgs::msg::PointCloud2 & msg, const tf2_ros::Buffer & tf_buffer)
{
  return pd_ptr_->enqueuePointCloud(msg, tf_buffer);
}

std::size_t VoxelGenerator::generateSweepPoints(CudaUniquePtr<float[]> & points_d)
{
  std::size_t point_counter{0};
  CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

  for (auto pc_cache_iter = pd_ptr_->getPointCloudCacheIter(); !pd_ptr_->isCacheEnd(pc_cache_iter);
       pc_cache_iter++) {
    auto sweep_num_points = pc_cache_iter->num_points;
    auto output_offset = point_counter * config_.num_point_feature_size_;

    if (point_counter + sweep_num_points >= static_cast<std::size_t>(config_.cloud_capacity_)) {
      RCLCPP_WARN_STREAM(
        rclcpp::get_logger("lidar_bevfusion"), "Exceeding cloud capacity. Used "
                                                 << pd_ptr_->getIdx(pc_cache_iter) << " out of "
                                                 << pd_ptr_->getCacheSize() << " sweep(s)");
      break;
    }

    auto affine_past2current =
      pd_ptr_->getAffineWorldToCurrent() * pc_cache_iter->affine_past2world;
    static_assert(std::is_same<decltype(affine_past2current.matrix()), Eigen::Matrix4f &>::value);
    static_assert(!Eigen::Matrix4f::IsRowMajor, "matrices should be col-major.");

    float time_lag = static_cast<float>(
      pd_ptr_->getCurrentTimestamp() - rclcpp::Time(pc_cache_iter->header.stamp).seconds());

    CHECK_CUDA_ERROR(cudaMemcpyAsync(
      affine_past2current_d_.get(), affine_past2current.data(),
      Eigen::Affine3f::MatrixType::SizeAtCompileTime * sizeof(float), cudaMemcpyHostToDevice,
      stream_));
    CHECK_CUDA_ERROR(cudaStreamSynchronize(stream_));

    pre_ptr_->generateSweepPoints_launch(
      pc_cache_iter->data_d.get(), sweep_num_points, time_lag, affine_past2current_d_.get(),
      points_d.get() + output_offset);
    point_counter += sweep_num_points;
  }

  return point_counter;
}

}  // namespace autoware::lidar_bevfusion

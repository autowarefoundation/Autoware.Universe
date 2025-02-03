// Copyright 2021 Tier IV, Inc.
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

#ifndef AUTOWARE__PROBABILISTIC_OCCUPANCY_GRID_MAP__UPDATER__BINARY_BAYES_FILTER_UPDATER_HPP_
#define AUTOWARE__PROBABILISTIC_OCCUPANCY_GRID_MAP__UPDATER__BINARY_BAYES_FILTER_UPDATER_HPP_

#ifdef USE_CUDA
#include "autoware/cuda_utils/cuda_unique_ptr.hpp"
#endif

#include "autoware/probabilistic_occupancy_grid_map/updater/ogm_updater_interface.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace autoware::occupancy_grid_map
{
namespace costmap_2d
{
class OccupancyGridMapBBFUpdater : public OccupancyGridMapUpdaterInterface
{
public:
  enum Index : size_t { OCCUPIED = 0U, FREE = 1U, NUM_STATES = 2U };
  OccupancyGridMapBBFUpdater(
    const bool use_cuda, const unsigned int cells_size_x, const unsigned int cells_size_y,
    const float resolution);
  bool update(const OccupancyGridMapInterface & single_frame_occupancy_grid_map) override;
  void initRosParam(rclcpp::Node & node) override;

private:
  inline unsigned char applyBBF(const unsigned char & z, const unsigned char & o);
  Eigen::Matrix2f probability_matrix_;
  double v_ratio_;

#ifdef USE_CUDA
  autoware::cuda_utils::CudaUniquePtr<float[]> device_probability_matrix_;
#endif
};

}  // namespace costmap_2d
}  // namespace autoware::occupancy_grid_map

#endif  // AUTOWARE__PROBABILISTIC_OCCUPANCY_GRID_MAP__UPDATER__BINARY_BAYES_FILTER_UPDATER_HPP_

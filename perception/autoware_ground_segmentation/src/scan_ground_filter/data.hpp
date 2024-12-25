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

#ifndef SCAN_GROUND_FILTER__DATA_HPP_
#define SCAN_GROUND_FILTER__DATA_HPP_

#include <sensor_msgs/msg/point_cloud2.hpp>

#include <pcl_conversions/pcl_conversions.h>

namespace autoware::ground_segmentation
{
using PointCloud2ConstPtr = sensor_msgs::msg::PointCloud2::ConstSharedPtr;

class PclDataAccessor
{
public:
  PclDataAccessor() = default;
  ~PclDataAccessor() = default;

  bool isInitialized() const { return data_offset_initialized_; }

  void setField(const PointCloud2ConstPtr & input)
  {
    data_offset_x_ = input->fields[pcl::getFieldIndex(*input, "x")].offset;
    data_offset_y_ = input->fields[pcl::getFieldIndex(*input, "y")].offset;
    data_offset_z_ = input->fields[pcl::getFieldIndex(*input, "z")].offset;
    int intensity_index = pcl::getFieldIndex(*input, "intensity");
    if (intensity_index != -1) {
      data_offset_intensity_ = input->fields[intensity_index].offset;
      intensity_type_ = input->fields[intensity_index].datatype;
    } else {
      data_offset_intensity_ = -1;
    }
    data_offset_initialized_ = true;
  }

  inline void getPoint(
    const PointCloud2ConstPtr & input, const size_t data_index, pcl::PointXYZ & point) const
  {
    point.x = *reinterpret_cast<const float *>(&input->data[data_index + data_offset_x_]);
    point.y = *reinterpret_cast<const float *>(&input->data[data_index + data_offset_y_]);
    point.z = *reinterpret_cast<const float *>(&input->data[data_index + data_offset_z_]);
  }

private:
  // data field offsets
  int data_offset_x_ = 0;
  int data_offset_y_ = 0;
  int data_offset_z_ = 0;
  int data_offset_intensity_ = 0;
  int intensity_type_ = 0;
  bool data_offset_initialized_ = false;
};

}  // namespace autoware::ground_segmentation

#endif  // SCAN_GROUND_FILTER__DATA_HPP_

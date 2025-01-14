// Copyright 2023 TIER IV, Inc.
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

#ifndef AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__SEGMENTATION_POINTCLOUD_FUSION__NODE_HPP_
#define AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__SEGMENTATION_POINTCLOUD_FUSION__NODE_HPP_

#include "autoware/image_projection_based_fusion/fusion_node.hpp"

#include <autoware/image_projection_based_fusion/utils/utils.hpp>
#include <image_transport/image_transport.hpp>

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#if __has_include(<cv_bridge/cv_bridge.hpp>)
#include <cv_bridge/cv_bridge.hpp>
#else
#include <cv_bridge/cv_bridge.h>
#endif

namespace autoware::image_projection_based_fusion
{
class SegmentPointCloudFusionNode : public FusionNode<PointCloudMsgType, Image, PointCloudMsgType>
{
public:
  explicit SegmentPointCloudFusionNode(const rclcpp::NodeOptions & options);

private:
  void preprocess(PointCloudMsgType & pointcloud_msg) override;

  void fuseOnSingleImage(
    const PointCloudMsgType & input_pointcloud_msg, const Det2dStatus<Image> & det2d,
    const Image & input_mask, PointCloudMsgType & output_pointcloud_msg) override;

  inline void copyPointCloud(
    const PointCloudMsgType & input, const int point_step, const size_t global_offset,
    PointCloudMsgType & output, size_t & output_pointcloud_size)
  {
    std::memcpy(&output.data[output_pointcloud_size], &input.data[global_offset], point_step);
    output_pointcloud_size += point_step;
  }

  void postprocess(
    const PointCloudMsgType & pointcloud_msg, PointCloudMsgType & output_msg) override;

  // debug
  image_transport::Publisher pub_debug_mask_ptr_;

  std::vector<bool> filter_semantic_label_target_;
  float filter_distance_threshold_;
  // declare list of semantic label target, depend on trained data of yolox segmentation model
  std::vector<std::pair<std::string, bool>> filter_semantic_label_target_list_ = {
    {"UNKNOWN", false},       {"BUILDING", false},     {"WALL", false},       {"OBSTACLE", false},
    {"TRAFFIC_LIGHT", false}, {"TRAFFIC_SIGN", false}, {"PERSON", false},     {"VEHICLE", false},
    {"BIKE", false},          {"ROAD", false},         {"SIDEWALK", false},   {"ROAD_PAINT", false},
    {"CURBSTONE", false},     {"CROSSWALK", false},    {"VEGETATION", false}, {"SKY", false}};

  bool is_publish_debug_mask_;
  std::unordered_set<size_t> filter_global_offset_set_;
};

}  // namespace autoware::image_projection_based_fusion
#endif  // AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__SEGMENTATION_POINTCLOUD_FUSION__NODE_HPP_

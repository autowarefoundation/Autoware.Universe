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

#ifndef AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__ROI_POINTCLOUD_FUSION__NODE_HPP_
#define AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__ROI_POINTCLOUD_FUSION__NODE_HPP_

#include "autoware/image_projection_based_fusion/fusion_node.hpp"

#include <autoware/image_projection_based_fusion/utils/utils.hpp>

#include <memory>
#include <string>
#include <vector>

namespace autoware::image_projection_based_fusion
{
class RoiPointCloudFusionNode : public FusionNode<PointCloudMsgType, RoiMsgType, ClusterMsgType>
{
public:
  explicit RoiPointCloudFusionNode(const rclcpp::NodeOptions & options);

private:
  rclcpp::Publisher<PointCloudMsgType>::SharedPtr point_pub_ptr_;
  rclcpp::Publisher<PointCloudMsgType>::SharedPtr cluster_debug_pub_;

  void fuseOnSingleImage(
    const PointCloudMsgType & input_pointcloud_msg, const Det2dStatus<RoiMsgType> & det2d,
    const RoiMsgType & input_roi_msg, PointCloudMsgType & output_pointcloud_msg) override;

  void postprocess(const PointCloudMsgType & pointcloud_msg, ClusterMsgType & output_msg) override;

  void publish(const ClusterMsgType & output_msg) override;

  int min_cluster_size_{1};
  int max_cluster_size_{20};
  bool fuse_unknown_only_{true};
  double cluster_2d_tolerance_;

  std::vector<ClusterObjType> output_fused_objects_;
};

}  // namespace autoware::image_projection_based_fusion
#endif  // AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__ROI_POINTCLOUD_FUSION__NODE_HPP_

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

#pragma once

#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <tier4_perception_msgs/msg/detected_objects_with_feature.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace autoware::image_projection_based_fusion
{
using autoware_perception_msgs::msg::DetectedObject;
using autoware_perception_msgs::msg::DetectedObjects;
using sensor_msgs::msg::CameraInfo;
using sensor_msgs::msg::Image;
using PointCloudMsgType = sensor_msgs::msg::PointCloud2;
using RoiMsgType = tier4_perception_msgs::msg::DetectedObjectsWithFeature;
using ClusterMsgType = tier4_perception_msgs::msg::DetectedObjectsWithFeature;
using ClusterObjType = tier4_perception_msgs::msg::DetectedObjectWithFeature;
using tier4_perception_msgs::msg::DetectedObjectWithFeature;
using PointCloud = pcl::PointCloud<pcl::PointXYZ>;
using autoware_perception_msgs::msg::ObjectClassification;
}  // namespace autoware::image_projection_based_fusion

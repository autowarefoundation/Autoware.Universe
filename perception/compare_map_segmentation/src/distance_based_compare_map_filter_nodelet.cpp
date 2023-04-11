// Copyright 2020 Tier IV, Inc.
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

#include "compare_map_segmentation/distance_based_compare_map_filter_nodelet.hpp"

#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/search/kdtree.h>
#include <pcl/segmentation/segment_differences.h>

#include <vector>

namespace compare_map_segmentation
{

// ******************************Static Map Loader ***************************************
void DistanceBasedStaticMapLoader::onMapCallback(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr map)
{
  pcl::PointCloud<pcl::PointXYZ> map_pcl;
  pcl::fromROSMsg<pcl::PointXYZ>(*map, map_pcl);
  const auto map_pcl_ptr = pcl::make_shared<pcl::PointCloud<pcl::PointXYZ>>(map_pcl);

  (*mutex_ptr_).lock();
  map_ptr_ = map_pcl_ptr;
  *tf_map_input_frame_ = map_ptr_->header.frame_id;
  if (!tree_) {
    if (map_ptr_->isOrganized()) {
      tree_.reset(new pcl::search::OrganizedNeighbor<pcl::PointXYZ>());
    } else {
      tree_.reset(new pcl::search::KdTree<pcl::PointXYZ>(false));
    }
  }
  tree_->setInputCloud(map_ptr_);

  (*mutex_ptr_).unlock();
}

bool DistanceBasedStaticMapLoader::is_close_to_map(
  const pcl::PointXYZ & point, const double distance_threshold)
{
  if (map_ptr_ == NULL) {
    return false;
  }
  if (tree_ == NULL) {
    return false;
  }

  std::vector<int> nn_indices(1);
  std::vector<float> nn_distances(1);
  if (!isFinite(point)) {
    return false;
  }
  if (!tree_->nearestKSearch(point, 1, nn_indices, nn_distances)) {
    return false;
  }
  if (nn_distances[0] > distance_threshold) {
    return false;
  }
  return true;
}

// ******************************  Dynamic Map Loader ************************************
bool DistanceBasedDynamicMapLoader::is_close_to_map(
  const pcl::PointXYZ & point, const double distance_threshold)
{
  return true;
}

// ****************************************************************************************

DistanceBasedCompareMapFilterComponent::DistanceBasedCompareMapFilterComponent(
  const rclcpp::NodeOptions & options)
: Filter("DistanceBasedCompareMapFilter", options)
{  // initialize debug tool
  {
    using tier4_autoware_utils::DebugPublisher;
    using tier4_autoware_utils::StopWatch;
    stop_watch_ptr_ = std::make_unique<StopWatch<std::chrono::milliseconds>>();
    debug_publisher_ =
      std::make_unique<DebugPublisher>(this, "voxel_based_approximate_compare_map_filter");
    stop_watch_ptr_->tic("cyclic_time");
    stop_watch_ptr_->tic("processing_time");
  }

  distance_threshold_ = static_cast<double>(declare_parameter("distance_threshold", 0.3));
  bool use_dynamic_map_loading = declare_parameter<bool>("use_dynamic_map_loading");
  if (use_dynamic_map_loading) {
    rclcpp::CallbackGroup::SharedPtr main_callback_group;
    main_callback_group = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    distance_based_map_loader_ = std::make_unique<DistanceBasedDynamicMapLoader>(
      this, distance_threshold_, &tf_input_frame_, &mutex_, main_callback_group);
  } else {
    distance_based_map_loader_ = std::make_unique<DistanceBasedStaticMapLoader>(
      this, distance_threshold_, &tf_input_frame_, &mutex_);
  }
}

void DistanceBasedCompareMapFilterComponent::filter(
  const PointCloud2ConstPtr & input, [[maybe_unused]] const IndicesPtr & indices,
  PointCloud2 & output)
{
  std::scoped_lock lock(mutex_);
  stop_watch_ptr_->toc("processing_time", true);

  pcl::PointCloud<pcl::PointXYZ>::Ptr pcl_input(new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PointCloud<pcl::PointXYZ>::Ptr pcl_output(new pcl::PointCloud<pcl::PointXYZ>);
  pcl::fromROSMsg(*input, *pcl_input);
  pcl_output->points.reserve(pcl_input->points.size());

  for (size_t i = 0; i < pcl_input->points.size(); ++i) {
    if (distance_based_map_loader_->is_close_to_map(pcl_input->points.at(i), distance_threshold_)) {
      continue;
    }
    pcl_output->points.push_back(pcl_input->points.at(i));
  }

  pcl::toROSMsg(*pcl_output, output);
  output.header = input->header;

  // add processing time for debug
  if (debug_publisher_) {
    const double cyclic_time_ms = stop_watch_ptr_->toc("cyclic_time", true);
    const double processing_time_ms = stop_watch_ptr_->toc("processing_time", true);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/cyclic_time_ms", cyclic_time_ms);
    debug_publisher_->publish<tier4_debug_msgs::msg::Float64Stamped>(
      "debug/processing_time_ms", processing_time_ms);
  }
}

}  // namespace compare_map_segmentation

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(compare_map_segmentation::DistanceBasedCompareMapFilterComponent)

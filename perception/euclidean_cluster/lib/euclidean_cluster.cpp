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

#include "euclidean_cluster/euclidean_cluster.hpp"

#include <pcl/kdtree/kdtree.h>
#include <pcl/segmentation/extract_clusters.h>

namespace euclidean_cluster
{
EuclideanCluster::EuclideanCluster() {}

EuclideanCluster::EuclideanCluster(bool use_height, int min_cluster_size, int max_cluster_size)
: EuclideanClusterInterface(use_height, min_cluster_size, max_cluster_size)
{
}

EuclideanCluster::EuclideanCluster(
  bool use_height, int min_cluster_size, int max_cluster_size, float tolerance)
: EuclideanClusterInterface(use_height, min_cluster_size, max_cluster_size, tolerance)
{
}

bool EuclideanCluster::cluster(
  const pcl::PointCloud<pcl::PointXYZ>::ConstPtr & pointcloud,
  std::vector<pcl::PointCloud<pcl::PointXYZ>> & clusters)
{
  // convert 2d pointcloud
  pcl::PointCloud<pcl::PointXYZ>::ConstPtr pointcloud_ptr(new pcl::PointCloud<pcl::PointXYZ>);
  setPointcloud(pointcloud, pointcloud_ptr);

  pcl::EuclideanClusterExtraction<pcl::PointXYZ> pcl_euclidean_cluster;
  std::vector<pcl::PointIndices> cluster_indices;
  solveEuclideanClustering(pcl_euclidean_cluster, cluster_indices, pointcloud_ptr);

  // build output
  {
    for (const auto & cluster : cluster_indices) {
      pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster(new pcl::PointCloud<pcl::PointXYZ>);
      for (const auto & point_idx : cluster.indices) {
        cloud_cluster->points.emplace_back(pointcloud->points[point_idx]);
      }
      clusters.emplace_back(*cloud_cluster);
      clusters.back().width = cloud_cluster->points.size();
      clusters.back().height = 1;
      clusters.back().is_dense = false;
    }
  }
  return true;
}

void EuclideanCluster::setPointcloud(
  const pcl::PointCloud<pcl::PointXYZ>::ConstPtr & pointcloud,
  pcl::PointCloud<pcl::PointXYZ>::ConstPtr & pointcloud_ptr)
{
  if (!params_.use_height) {
    pcl::PointCloud<pcl::PointXYZ>::Ptr pointcloud_2d_ptr(new pcl::PointCloud<pcl::PointXYZ>);
    for (const auto & point : pointcloud->points) {
      pcl::PointXYZ point2d;
      point2d.x = point.x;
      point2d.y = point.y;
      point2d.z = 0.0;
      pointcloud_2d_ptr->emplace_back(point2d);
    }
    pointcloud_ptr = pointcloud_2d_ptr;
  } else {
    pointcloud_ptr = pointcloud;
  }
}

}  // namespace euclidean_cluster

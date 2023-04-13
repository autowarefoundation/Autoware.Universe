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

#pragma once
#include <Eigen/StdVector>
#include <opencv4/opencv2/core.hpp>
#include <pcdless_common/gamma_converter.hpp>
#include <rclcpp/node.hpp>

#include <visualization_msgs/msg/marker_array.hpp>

#include <boost/functional/hash.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace pcdless::ekf_corrector
{
struct Area
{
  Area() {}
  Area(const Eigen::Vector2f & v)
  {
    if (unit_length_ < 0) throw_error();
    x = static_cast<long>(std::floor(v.x() / unit_length_));
    y = static_cast<long>(std::floor(v.y() / unit_length_));
  }

  Eigen::Vector2f real_scale() const { return {x * unit_length_, y * unit_length_}; };

  std::array<Eigen::Vector2f, 2> real_scale_boundary() const
  {
    std::array<Eigen::Vector2f, 2> boundary;
    boundary.at(0) = real_scale();
    boundary.at(1) = real_scale() + Eigen::Vector2f(unit_length_, unit_length_);
    return boundary;
  };

  void throw_error() const
  {
    std::cerr << "Area::unit_length_ is not initialized" << std::endl;
    throw std::runtime_error("invalid Area::unit_length");
  }
  int x, y;
  static float unit_length_;
  static float image_size_;

  friend bool operator==(const Area & one, const Area & other)
  {
    return one.x == other.x && one.y == other.y;
  }
  friend bool operator!=(const Area & one, const Area & other) { return !(one == other); }
  size_t operator()(const Area & index) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, index.x);
    boost::hash_combine(seed, index.y);
    return seed;
  }
};

struct CostMapValue
{
  CostMapValue(float intensity, int angle, bool unmapped)
  : intensity(intensity), angle(angle), unmapped(unmapped)
  {
  }
  float intensity;  // 0~1
  int angle;        // 0~180
  bool unmapped;    // true/false
};

class HierarchicalCostMap
{
public:
  using Marker = visualization_msgs::msg::Marker;
  using MarkerArray = visualization_msgs::msg::MarkerArray;
  using Pose = geometry_msgs::msg::Pose;

  using BgPoint = boost::geometry::model::d2::point_xy<double>;
  using BgPolygon = boost::geometry::model::polygon<BgPoint>;

  HierarchicalCostMap(rclcpp::Node * node);

  void set_cloud(const pcl::PointCloud<pcl::PointNormal> & cloud);
  void set_bounding_box(const pcl::PointCloud<pcl::PointXYZL> & cloud);

  /**
   * Get pixel value at specified pixel
   *
   * @param[in] position Real scale position at world frame
   * @return The combination of intensity (0-1), angle (0-180), unmapped flag (0, 1)
   */
  CostMapValue at(const Eigen::Vector2f & position);

  MarkerArray show_map_range() const;

  cv::Mat get_map_image(const Pose & pose);

  void erase_obsolete();

  void set_height(float height);

private:
  const float max_range_;
  const float image_size_;
  const size_t max_map_count_;
  rclcpp::Logger logger_;
  std::optional<float> height_{std::nullopt};

  common::GammaConverter gamma_converter{4.0f};

  std::unordered_map<Area, bool, Area> map_accessed_;

  std::list<Area> generated_map_history_;
  std::optional<pcl::PointCloud<pcl::PointNormal>> cloud_;
  std::vector<BgPolygon> bounding_boxes_;
  std::unordered_map<Area, cv::Mat, Area> cost_maps_;

  cv::Point to_cv_point(const Area & are, const Eigen::Vector2f) const;
  void build_map(const Area & area);

  cv::Mat create_available_area_image(const Area & area) const;
};
}  // namespace pcdless::ekf_corrector
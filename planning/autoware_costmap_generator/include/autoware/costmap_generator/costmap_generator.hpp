// Copyright 2020-2024 Tier IV, Inc.
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

/*
 *  Copyright (c) 2018, Nagoya University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Autoware nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************/

#ifndef AUTOWARE__COSTMAP_GENERATOR__COSTMAP_GENERATOR_HPP_
#define AUTOWARE__COSTMAP_GENERATOR__COSTMAP_GENERATOR_HPP_

#include "autoware/costmap_generator/utils/objects_to_costmap.hpp"
#include "autoware/costmap_generator/utils/points_to_costmap.hpp"
#include "costmap_generator_node_parameters.hpp"

#include <autoware/universe_utils/ros/polling_subscriber.hpp>
#include <autoware/universe_utils/ros/processing_time_publisher.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>
#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <grid_map_ros/GridMapRosConverter.hpp>
#include <grid_map_ros/grid_map_ros.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_map_msgs/msg/lanelet_map_bin.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <tier4_debug_msgs/msg/float64_stamped.hpp>
#include <tier4_planning_msgs/msg/scenario.hpp>

#include <grid_map_msgs/msg/grid_map.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <memory>
#include <vector>

class TestCostmapGenerator;

namespace autoware::costmap_generator
{
using autoware_perception_msgs::msg::PredictedObjects;

class CostmapGenerator : public rclcpp::Node
{
public:
  explicit CostmapGenerator(const rclcpp::NodeOptions & node_options);

private:
  std::shared_ptr<::costmap_generator_node::ParamListener> param_listener_;
  std::shared_ptr<::costmap_generator_node::Params> param_;
  geometry_msgs::msg::PoseStamped::ConstSharedPtr current_pose_;

  lanelet::LaneletMapPtr lanelet_map_;
  PredictedObjects::ConstSharedPtr objects_;
  sensor_msgs::msg::PointCloud2::ConstSharedPtr points_;

  grid_map::GridMap costmap_;
  std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_;

  rclcpp::Publisher<grid_map_msgs::msg::GridMap>::SharedPtr pub_costmap_;
  rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr pub_occupancy_grid_;
  rclcpp::Publisher<autoware::universe_utils::ProcessingTimeDetail>::SharedPtr pub_processing_time_;
  rclcpp::Publisher<tier4_debug_msgs::msg::Float64Stamped>::SharedPtr pub_processing_time_ms_;

  rclcpp::Subscription<autoware_map_msgs::msg::LaneletMapBin>::SharedPtr sub_lanelet_bin_map_;
  autoware::universe_utils::InterProcessPollingSubscriber<sensor_msgs::msg::PointCloud2>
    sub_points_{this, "~/input/points_no_ground", autoware::universe_utils::SingleDepthSensorQoS()};
  autoware::universe_utils::InterProcessPollingSubscriber<PredictedObjects> sub_objects_{
    this, "~/input/objects"};
  autoware::universe_utils::InterProcessPollingSubscriber<tier4_planning_msgs::msg::Scenario>
    sub_scenario_{this, "~/input/scenario"};

  rclcpp::TimerBase::SharedPtr timer_;

  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  std::vector<geometry_msgs::msg::Polygon> primitives_polygons_;

  PointsToCostmap points2costmap_{};
  ObjectsToCostmap objects2costmap_;

  tier4_planning_msgs::msg::Scenario::ConstSharedPtr scenario_;

  struct LayerName
  {
    static constexpr const char * objects = "objects";
    static constexpr const char * points = "points";
    static constexpr const char * primitives = "primitives";
    static constexpr const char * combined = "combined";
  };

  /// \brief wait for lanelet2 map to load and build routing graph
  void initLaneletMap();

  /// \brief callback for loading lanelet2 map
  void onLaneletMapBin(const autoware_map_msgs::msg::LaneletMapBin::ConstSharedPtr msg);

  void update_data();

  void set_current_pose();

  void onTimer();

  bool isActive();

  /// \brief initialize gridmap parameters based on rosparam
  void initGridmap();

  /// \brief publish ros msg: grid_map::GridMap, and nav_msgs::OccupancyGrid
  /// \param[in] gridmap with calculated cost
  void publishCostmap(const grid_map::GridMap & costmap);

  /// \brief fill a vector with road area polygons
  /// \param [in] lanelet_map input lanelet map
  /// \param [out] area_polygons polygon vector to fill
  static void loadRoadAreasFromLaneletMap(
    const lanelet::LaneletMapPtr lanelet_map,
    std::vector<geometry_msgs::msg::Polygon> & area_polygons);

  /// \brief fill a vector with parking-area polygons
  /// \param [in] lanelet_map input lanelet map
  /// \param [out] area_polygons polygon vector to fill
  static void loadParkingAreasFromLaneletMap(
    const lanelet::LaneletMapPtr lanelet_map,
    std::vector<geometry_msgs::msg::Polygon> & area_polygons);

  /// \brief calculate cost from pointcloud data
  /// \param[in] in_points: subscribed pointcloud data
  /// \param[in] vehicle_to_map_z: z value of the ego vehicle in the costmap frame
  grid_map::Matrix generatePointsCostmap(
    const sensor_msgs::msg::PointCloud2::ConstSharedPtr & in_points, const double vehicle_to_map_z);

  /// \brief calculate cost from DynamicObjectArray
  /// \param[in] in_objects: subscribed DynamicObjectArray
  grid_map::Matrix generateObjectsCostmap(const PredictedObjects::ConstSharedPtr in_objects);

  /// \brief calculate cost from lanelet2 map
  grid_map::Matrix generatePrimitivesCostmap();

  /// \brief calculate cost for final output
  grid_map::Matrix generateCombinedCostmap();

  /// \brief measure processing time
  autoware::universe_utils::StopWatch<std::chrono::milliseconds> stop_watch;

  friend class ::TestCostmapGenerator;
};
}  // namespace autoware::costmap_generator

#endif  // AUTOWARE__COSTMAP_GENERATOR__COSTMAP_GENERATOR_HPP_

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
 *  * Redistributions of source code must retain the above copyright notice, private_node
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    private_node list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Autoware nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    private_node software without specific prior written permission.
 *
 *  private_node SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF private_node SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************/

#include "autoware/costmap_generator/costmap_generator.hpp"

#include "autoware/costmap_generator/utils/object_map_utils.hpp"

#include <autoware_lanelet2_extension/utility/message_conversion.hpp>
#include <autoware_lanelet2_extension/utility/query.hpp>
#include <autoware_lanelet2_extension/utility/utilities.hpp>
#include <autoware_lanelet2_extension/visualization/visualization.hpp>
#include <pcl_ros/transforms.hpp>
#include <rclcpp/logging.hpp>
#include <tf2_eigen/tf2_eigen.hpp>

#include <lanelet2_core/Forward.h>
#include <lanelet2_core/geometry/Polygon.h>
#include <tf2/time.h>
#include <tf2/utils.h>

#include <memory>
#include <string>
#include <vector>

namespace
{

// Copied from scenario selector
geometry_msgs::msg::PoseStamped::ConstSharedPtr getCurrentPose(
  const tf2_ros::Buffer & tf_buffer, const rclcpp::Logger & logger)
{
  geometry_msgs::msg::TransformStamped tf_current_pose;

  try {
    tf_current_pose = tf_buffer.lookupTransform("map", "base_link", tf2::TimePointZero);
  } catch (tf2::TransformException & ex) {
    RCLCPP_ERROR(logger, "%s", ex.what());
    return nullptr;
  }

  geometry_msgs::msg::PoseStamped::SharedPtr p(new geometry_msgs::msg::PoseStamped());
  p->header = tf_current_pose.header;
  p->pose.orientation = tf_current_pose.transform.rotation;
  p->pose.position.x = tf_current_pose.transform.translation.x;
  p->pose.position.y = tf_current_pose.transform.translation.y;
  p->pose.position.z = tf_current_pose.transform.translation.z;

  return geometry_msgs::msg::PoseStamped::ConstSharedPtr(p);
}

// copied from scenario selector
std::shared_ptr<lanelet::ConstPolygon3d> findNearestParkinglot(
  const std::shared_ptr<lanelet::LaneletMap> & lanelet_map_ptr,
  const lanelet::BasicPoint2d & current_position)
{
  auto linked_parking_lot = std::make_shared<lanelet::ConstPolygon3d>();
  const auto result = lanelet::utils::query::getLinkedParkingLot(
    current_position, lanelet_map_ptr, linked_parking_lot.get());

  if (result) {
    return linked_parking_lot;
  }
  return {};
}

// copied from scenario selector
bool isInParkingLot(
  const std::shared_ptr<lanelet::LaneletMap> & lanelet_map_ptr,
  const geometry_msgs::msg::Pose & current_pose)
{
  const auto & p = current_pose.position;
  const lanelet::Point3d search_point(lanelet::InvalId, p.x, p.y, p.z);

  const auto nearest_parking_lot =
    findNearestParkinglot(lanelet_map_ptr, search_point.basicPoint2d());

  if (!nearest_parking_lot) {
    return false;
  }

  return lanelet::geometry::within(search_point, nearest_parking_lot->basicPolygon());
}

pcl::PointCloud<pcl::PointXYZ> getTransformedPointCloud(
  const sensor_msgs::msg::PointCloud2 & pointcloud_msg,
  const geometry_msgs::msg::Transform & transform)
{
  const Eigen::Matrix4f transform_matrix = tf2::transformToEigen(transform).matrix().cast<float>();

  sensor_msgs::msg::PointCloud2 transformed_msg;
  pcl_ros::transformPointCloud(transform_matrix, pointcloud_msg, transformed_msg);

  pcl::PointCloud<pcl::PointXYZ> transformed_pointcloud;
  pcl::fromROSMsg(transformed_msg, transformed_pointcloud);

  return transformed_pointcloud;
}

std::vector<geometry_msgs::msg::Polygon> getTransformedPrimitives(
  const std::vector<geometry_msgs::msg::Polygon> & in_polygons,
  const geometry_msgs::msg::TransformStamped & transform)
{
  std::vector<geometry_msgs::msg::Polygon> out_polygons;
  for (const auto & polygon : in_polygons) {
    geometry_msgs::msg::Polygon transformed_polygon;
    tf2::doTransform(polygon, transformed_polygon, transform);
    out_polygons.emplace_back(transformed_polygon);
  }
  return out_polygons;
}

}  // namespace

namespace autoware::costmap_generator
{
CostmapGenerator::CostmapGenerator(const rclcpp::NodeOptions & node_options)
: Node("costmap_generator", node_options), tf_buffer_(this->get_clock()), tf_listener_(tf_buffer_)
{
  param_listener_ = std::make_shared<::costmap_generator_node::ParamListener>(
    this->get_node_parameters_interface());
  param_ = std::make_shared<::costmap_generator_node::Params>(param_listener_->get_params());

  // Lanelet map subscriber
  sub_lanelet_bin_map_ = this->create_subscription<autoware_map_msgs::msg::LaneletMapBin>(
    "~/input/vector_map", rclcpp::QoS{1}.transient_local(),
    std::bind(&CostmapGenerator::onLaneletMapBin, this, std::placeholders::_1));

  // Publishers
  pub_costmap_ = this->create_publisher<grid_map_msgs::msg::GridMap>("~/output/grid_map", 1);
  pub_occupancy_grid_ =
    this->create_publisher<nav_msgs::msg::OccupancyGrid>("~/output/occupancy_grid", 1);
  pub_processing_time_ =
    create_publisher<autoware::universe_utils::ProcessingTimeDetail>("processing_time", 1);
  time_keeper_ = std::make_shared<autoware::universe_utils::TimeKeeper>(pub_processing_time_);
  pub_processing_time_ms_ =
    this->create_publisher<tier4_debug_msgs::msg::Float64Stamped>("~/debug/processing_time_ms", 1);

  // Timer
  const auto period_ns = rclcpp::Rate(param_->update_rate).period();
  timer_ =
    rclcpp::create_timer(this, get_clock(), period_ns, std::bind(&CostmapGenerator::onTimer, this));

  // Initialize
  initGridmap();
}

void CostmapGenerator::loadRoadAreasFromLaneletMap(
  const lanelet::LaneletMapPtr lanelet_map,
  std::vector<geometry_msgs::msg::Polygon> & area_polygons)
{
  // use all lanelets in map of subtype road to give way area
  lanelet::ConstLanelets all_lanelets = lanelet::utils::query::laneletLayer(lanelet_map);
  lanelet::ConstLanelets road_lanelets = lanelet::utils::query::roadLanelets(all_lanelets);

  // convert lanelets to polygons and put into area_points array
  for (const auto & ll : road_lanelets) {
    geometry_msgs::msg::Polygon poly;
    geometry_msgs::msg::Point32 pt;
    for (const auto & p : ll.polygon3d().basicPolygon()) {
      lanelet::utils::conversion::toGeomMsgPt32(p, &pt);
      poly.points.push_back(pt);
    }
    area_polygons.push_back(poly);
  }
}

void CostmapGenerator::loadParkingAreasFromLaneletMap(
  const lanelet::LaneletMapPtr lanelet_map,
  std::vector<geometry_msgs::msg::Polygon> & area_polygons)
{
  // Parking lots
  lanelet::ConstPolygons3d all_parking_lots = lanelet::utils::query::getAllParkingLots(lanelet_map);
  for (const auto & ll_poly : all_parking_lots) {
    geometry_msgs::msg::Polygon poly;
    lanelet::utils::conversion::toGeomMsgPoly(ll_poly, &poly);
    area_polygons.push_back(poly);
  }

  // Parking spaces
  lanelet::ConstLineStrings3d all_parking_spaces =
    lanelet::utils::query::getAllParkingSpaces(lanelet_map);
  for (const auto & parking_space : all_parking_spaces) {
    lanelet::ConstPolygon3d ll_poly;
    lanelet::utils::lineStringWithWidthToPolygon(parking_space, &ll_poly);
    geometry_msgs::msg::Polygon poly;
    lanelet::utils::conversion::toGeomMsgPoly(ll_poly, &poly);
    area_polygons.push_back(poly);
  }
}

void CostmapGenerator::onLaneletMapBin(
  const autoware_map_msgs::msg::LaneletMapBin::ConstSharedPtr msg)
{
  lanelet_map_ = std::make_shared<lanelet::LaneletMap>();
  lanelet::utils::conversion::fromBinMsg(*msg, lanelet_map_);

  if (param_->use_wayarea) {
    loadRoadAreasFromLaneletMap(lanelet_map_, primitives_polygons_);
  }

  if (param_->use_parkinglot) {
    loadParkingAreasFromLaneletMap(lanelet_map_, primitives_polygons_);
  }
}

void CostmapGenerator::update_data()
{
  objects_ = sub_objects_.takeData();
  points_ = sub_points_.takeData();
  scenario_ = sub_scenario_.takeData();
}

void CostmapGenerator::set_current_pose()
{
  current_pose_ = getCurrentPose(tf_buffer_, this->get_logger());
}

void CostmapGenerator::onTimer()
{
  update_data();

  autoware::universe_utils::ScopedTimeTrack scoped_time_track(__func__, *time_keeper_);
  stop_watch.tic();

  if (!param_->activate_by_scenario) set_current_pose();

  if (!isActive()) {
    // Publish ProcessingTime
    tier4_debug_msgs::msg::Float64Stamped processing_time_msg;
    processing_time_msg.stamp = get_clock()->now();
    processing_time_msg.data = stop_watch.toc();
    pub_processing_time_ms_->publish(processing_time_msg);
    return;
  }

  // Get current pose
  time_keeper_->start_track("lookupTransform");
  geometry_msgs::msg::TransformStamped tf;
  try {
    tf =
      tf_buffer_.lookupTransform(param_->costmap_frame, param_->vehicle_frame, tf2::TimePointZero);
  } catch (tf2::TransformException & ex) {
    RCLCPP_ERROR(this->get_logger(), "%s", ex.what());
    return;
  }
  time_keeper_->end_track("lookupTransform");

  // Set grid center
  grid_map::Position p;
  p.x() = tf.transform.translation.x;
  p.y() = tf.transform.translation.y;
  costmap_.setPosition(p);

  if ((param_->use_wayarea || param_->use_parkinglot) && lanelet_map_) {
    autoware::universe_utils::ScopedTimeTrack st("generatePrimitivesCostmap()", *time_keeper_);
    costmap_[LayerName::primitives] = generatePrimitivesCostmap();
  }

  if (param_->use_objects && objects_) {
    autoware::universe_utils::ScopedTimeTrack st("generateObjectsCostmap()", *time_keeper_);
    costmap_[LayerName::objects] = generateObjectsCostmap(objects_);
  }

  if (param_->use_points && points_) {
    autoware::universe_utils::ScopedTimeTrack st("generatePointsCostmap()", *time_keeper_);
    costmap_[LayerName::points] = generatePointsCostmap(points_, tf.transform.translation.z);
  }

  {
    autoware::universe_utils::ScopedTimeTrack st("generateCombinedCostmap()", *time_keeper_);
    costmap_[LayerName::combined] = generateCombinedCostmap();
  }

  publishCostmap(costmap_);
}

bool CostmapGenerator::isActive()
{
  if (!lanelet_map_) {
    return false;
  }

  if (param_->activate_by_scenario) {
    if (!scenario_) return false;
    const auto & s = scenario_->activating_scenarios;
    return std::any_of(s.begin(), s.end(), [](const auto scen) {
      return scen == tier4_planning_msgs::msg::Scenario::PARKING;
    });
  }

  if (!current_pose_) return false;
  return isInParkingLot(lanelet_map_, current_pose_->pose);
}

void CostmapGenerator::initGridmap()
{
  costmap_.setFrameId(param_->costmap_frame);
  costmap_.setGeometry(
    grid_map::Length(param_->grid_length_x, param_->grid_length_y), param_->grid_resolution,
    grid_map::Position(param_->grid_position_x, param_->grid_position_y));

  costmap_.add(LayerName::points, param_->grid_min_value);
  costmap_.add(LayerName::objects, param_->grid_min_value);
  costmap_.add(LayerName::primitives, param_->grid_min_value);
  costmap_.add(LayerName::combined, param_->grid_min_value);
}

grid_map::Matrix CostmapGenerator::generatePointsCostmap(
  const sensor_msgs::msg::PointCloud2::ConstSharedPtr & in_points, const double vehicle_to_map_z)
{
  geometry_msgs::msg::TransformStamped points2costmap;
  try {
    points2costmap = tf_buffer_.lookupTransform(
      param_->costmap_frame, in_points->header.frame_id, tf2::TimePointZero);
  } catch (const tf2::TransformException & ex) {
    RCLCPP_ERROR_THROTTLE(
      rclcpp::get_logger("costmap_generator"), *get_clock(), 1000, "%s", ex.what());
  }

  const auto transformed_points = getTransformedPointCloud(*in_points, points2costmap.transform);

  const auto maximum_height_thres = param_->maximum_lidar_height_thres + vehicle_to_map_z;
  const auto minimum_height_thres = param_->minimum_lidar_height_thres + vehicle_to_map_z;
  grid_map::Matrix points_costmap = points2costmap_.makeCostmapFromPoints(
    maximum_height_thres, minimum_height_thres, param_->grid_min_value, param_->grid_max_value,
    costmap_, LayerName::points, transformed_points);

  return points_costmap;
}

PredictedObjects::ConstSharedPtr transformObjects(
  const tf2_ros::Buffer & tf_buffer, const PredictedObjects::ConstSharedPtr in_objects,
  const std::string & target_frame_id, const std::string & src_frame_id)
{
  auto objects = std::make_shared<PredictedObjects>();
  *objects = *in_objects;
  objects->header.frame_id = target_frame_id;

  geometry_msgs::msg::TransformStamped objects2costmap;
  try {
    objects2costmap = tf_buffer.lookupTransform(target_frame_id, src_frame_id, tf2::TimePointZero);
  } catch (const tf2::TransformException & ex) {
    RCLCPP_ERROR(rclcpp::get_logger("costmap_generator"), "%s", ex.what());
  }

  for (auto & object : objects->objects) {
    geometry_msgs::msg::PoseStamped output_stamped;
    geometry_msgs::msg::PoseStamped input_stamped;
    input_stamped.pose = object.kinematics.initial_pose_with_covariance.pose;
    tf2::doTransform(input_stamped, output_stamped, objects2costmap);
    object.kinematics.initial_pose_with_covariance.pose = output_stamped.pose;
  }

  return PredictedObjects::ConstSharedPtr(objects);
}

grid_map::Matrix CostmapGenerator::generateObjectsCostmap(
  const PredictedObjects::ConstSharedPtr in_objects)
{
  const auto object_frame = in_objects->header.frame_id;
  const auto transformed_objects =
    transformObjects(tf_buffer_, in_objects, param_->costmap_frame, object_frame);

  grid_map::Matrix objects_costmap = objects2costmap_.makeCostmapFromObjects(
    costmap_, param_->expand_polygon_size, param_->size_of_expansion_kernel, transformed_objects);

  return objects_costmap;
}

grid_map::Matrix CostmapGenerator::generatePrimitivesCostmap()
{
  grid_map::GridMap lanelet2_costmap = costmap_;
  if (primitives_polygons_.empty()) {
    return lanelet2_costmap[LayerName::primitives];
  }

  geometry_msgs::msg::TransformStamped primitives2costmap;
  try {
    primitives2costmap =
      tf_buffer_.lookupTransform(param_->costmap_frame, param_->map_frame, tf2::TimePointZero);
  } catch (const tf2::TransformException & ex) {
    RCLCPP_ERROR(rclcpp::get_logger("costmap_generator"), "%s", ex.what());
  }

  const auto transformed_primitives =
    getTransformedPrimitives(primitives_polygons_, primitives2costmap);

  object_map::fill_polygon_areas(
    lanelet2_costmap, transformed_primitives, LayerName::primitives, param_->grid_max_value,
    param_->grid_min_value);

  return lanelet2_costmap[LayerName::primitives];
}

grid_map::Matrix CostmapGenerator::generateCombinedCostmap()
{
  // assuming combined_costmap is calculated by element wise max operation
  grid_map::GridMap combined_costmap = costmap_;

  combined_costmap[LayerName::combined].setConstant(param_->grid_min_value);

  combined_costmap[LayerName::combined] =
    combined_costmap[LayerName::combined].cwiseMax(combined_costmap[LayerName::points]);

  combined_costmap[LayerName::combined] =
    combined_costmap[LayerName::combined].cwiseMax(combined_costmap[LayerName::primitives]);

  combined_costmap[LayerName::combined] =
    combined_costmap[LayerName::combined].cwiseMax(combined_costmap[LayerName::objects]);

  return combined_costmap[LayerName::combined];
}

void CostmapGenerator::publishCostmap(const grid_map::GridMap & costmap)
{
  // Set header
  std_msgs::msg::Header header;
  header.frame_id = param_->costmap_frame;
  header.stamp = this->now();

  // Publish OccupancyGrid
  nav_msgs::msg::OccupancyGrid out_occupancy_grid;
  grid_map::GridMapRosConverter::toOccupancyGrid(
    costmap, LayerName::combined, param_->grid_min_value, param_->grid_max_value,
    out_occupancy_grid);
  out_occupancy_grid.header = header;
  pub_occupancy_grid_->publish(out_occupancy_grid);

  // Publish GridMap
  auto out_gridmap_msg = grid_map::GridMapRosConverter::toMessage(costmap);
  out_gridmap_msg->header = header;
  pub_costmap_->publish(*out_gridmap_msg);

  // Publish ProcessingTime
  tier4_debug_msgs::msg::Float64Stamped processing_time_msg;
  processing_time_msg.stamp = get_clock()->now();
  processing_time_msg.data = stop_watch.toc();
  pub_processing_time_ms_->publish(processing_time_msg);
}
}  // namespace autoware::costmap_generator

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::costmap_generator::CostmapGenerator)

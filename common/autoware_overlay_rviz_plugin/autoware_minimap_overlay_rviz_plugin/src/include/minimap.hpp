// Copyright 2024 The Autoware Contributors
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
#ifndef MINIMAP_HPP_
#define MINIMAP_HPP_

#include "goal_pose.hpp"
#include "overlay_utils.hpp"
#include "path_overlay.hpp"
#include "rviz_common/properties/color_property.hpp"
#include "rviz_common/properties/enum_property.hpp"
#include "rviz_common/properties/float_property.hpp"
#include "rviz_common/properties/int_property.hpp"
#include "rviz_common/properties/ros_topic_property.hpp"
#include "rviz_common/properties/string_property.hpp"
#include "tile_field.hpp"
#include "tile_provider.hpp"  // Include the TileProvider header

#include <GeographicLib/MGRS.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <QImage>
#include <QVBoxLayout>
#include <rclcpp/rclcpp.hpp>
#include <rviz_common/display.hpp>
#include <rviz_common/display_context.hpp>
#include <rviz_common/logging.hpp>
#include <rviz_common/render_panel.hpp>
#include <rviz_common/ros_topic_display.hpp>
#include <rviz_common/view_manager.hpp>
#include <rviz_common/window_manager_interface.hpp>
#include <rviz_rendering/render_window.hpp>

#include <autoware_adapi_v1_msgs/msg/route_state.hpp>
#include <autoware_adapi_v1_msgs/msg/vehicle_kinematics.hpp>
#include <autoware_planning_msgs/msg/trajectory.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <tier4_map_msgs/msg/map_projector_info.hpp>

#include <OgreColourValue.h>
#include <OgreMaterial.h>
#include <OgreTexture.h>
#include <OgreTextureManager.h>

#include <cmath>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

namespace autoware_minimap_overlay_rviz_plugin
{

class VehicleMapDisplay : public rviz_common::Display
{
  Q_OBJECT

public:
  VehicleMapDisplay();
  virtual ~VehicleMapDisplay();

  void onInitialize() override;
  void reset() override;
  void update(float, float) override;

protected Q_SLOTS:
  void updateOverlaySize();
  void updateOverlayPosition();
  void onTilesUpdated();
  void updateZoomLevel();
  void updateLatitude();
  void updateLongitude();
  void updateGoalPose();
  void updateMapPosition();
  void updateTileProvider();  // Add this slot

protected:
  void onEnable() override;
  void onDisable() override;

private:
  void drawWidget(QImage & hud);
  void drawCircle(QPainter & painter, const QRectF & backgroundRect);
  void goalPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg);
  void poseCallback(const autoware_adapi_v1_msgs::msg::VehicleKinematics::SharedPtr msg);
  void routeStateCallback(const autoware_adapi_v1_msgs::msg::RouteState::SharedPtr msg);
  void routePointsCallback(const autoware_planning_msgs::msg::Trajectory::SharedPtr msg);
  void mapProjectorInfoCallback(const tier4_map_msgs::msg::MapProjectorInfo::SharedPtr msg);

  std::pair<double, double> localXYZToLatLon(double local_x, double local_y);
  void smoothUpdate();

  autoware_minimap_overlay_rviz_plugin::OverlayObject::SharedPtr overlay_;

  std::mutex mutex_;
  std::mutex property_mutex_;
  std::mutex tile_mutex_;

  rviz_common::properties::IntProperty * property_width_;
  rviz_common::properties::IntProperty * property_height_;

  rviz_common::properties::IntProperty * property_horizontal_margin_;
  rviz_common::properties::IntProperty * property_vertical_margin_;

  rviz_common::properties::EnumProperty * property_tile_provider_;
  rviz_common::properties::EnumProperty * property_anchor_vertical_;
  rviz_common::properties::EnumProperty * property_anchor_horizontal_;
  rviz_common::properties::IntProperty * property_border_radius_;

  rviz_common::properties::IntProperty * property_zoom_;
  rviz_common::properties::FloatProperty * alpha_property_;
  rviz_common::properties::FloatProperty * property_latitude_;
  rviz_common::properties::FloatProperty * property_longitude_;
  rviz_common::properties::FloatProperty * property_goal_lat;
  rviz_common::properties::FloatProperty * property_goal_lon;
  rviz_common::properties::FloatProperty * property_origin_lat_;
  rviz_common::properties::FloatProperty * property_origin_lon_;

  int zoom_;

  std::unique_ptr<TileField> tile_field_;
  rclcpp::Node::SharedPtr node_;
  rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr nav_sat_fix_sub_;

  int center_x_tile_;
  int center_y_tile_;

  double latitude_;
  double longitude_;

  // subscription ptr and msg ptr
  rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr goal_pose_sub_;
  geometry_msgs::msg::PoseStamped::SharedPtr goal_pose_msg_;

  // subscription ptr and msg ptr
  rclcpp::Subscription<autoware_adapi_v1_msgs::msg::VehicleKinematics>::SharedPtr pose_sub_;
  autoware_adapi_v1_msgs::msg::VehicleKinematics::SharedPtr pose_msg_;

  // subscription ptr and msg ptr
  rclcpp::Subscription<autoware_adapi_v1_msgs::msg::RouteState>::SharedPtr route_state_sub_;
  autoware_adapi_v1_msgs::msg::RouteState::SharedPtr route_state_msg_;

  // subscription ptr and msg ptr
  rclcpp::Subscription<autoware_planning_msgs::msg::Trajectory>::SharedPtr route_points_sub_;
  autoware_planning_msgs::msg::Trajectory::SharedPtr route_points_msg_;

  // subscription ptr and msg ptr
  rclcpp::Subscription<tier4_map_msgs::msg::MapProjectorInfo>::SharedPtr map_projector_info_sub_;
  tier4_map_msgs::msg::MapProjectorInfo::SharedPtr map_projector_info_msg_;

  GoalPose goal_pose_;
  PathOverlay path_overlay_;

  // Previous and target positions
  double prev_latitude_;
  double prev_longitude_;
  double target_latitude_;
  double target_longitude_;

  void drawGoalPose(QPainter & painter, const QRectF & backgroundRect);
  std::pair<double, double> localXYZToLatLonUTM(double x, double y);
  std::pair<double, double> localXYZToLatLonMGRS(double x, double y);

  std::string projector_type_;
  std::string mgrs_grid_;

  TileProvider::Provider current_tile_provider_;  // Add this line
};

}  // namespace autoware_minimap_overlay_rviz_plugin

#endif  // MINIMAP_HPP_

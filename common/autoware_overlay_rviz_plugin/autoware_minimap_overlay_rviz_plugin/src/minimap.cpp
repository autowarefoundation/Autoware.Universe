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
#include "include/minimap.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QTransform>
#include <QVBoxLayout>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <rviz_rendering/render_system.hpp>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <qimage.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/utils.h>

#include <cmath>

namespace autoware_minimap_overlay_rviz_plugin
{

VehicleMapDisplay::VehicleMapDisplay()
: rviz_common::Display(),
  overlay_(nullptr),
  prev_latitude_(0.0),
  prev_longitude_(0.0),
  target_latitude_(0.0),
  target_longitude_(0.0),
  current_tile_provider_(TileProvider::OpenStreetMap)
{
  property_width_ = new rviz_common::properties::IntProperty(
    "Width", 256, "Width of the overlay max:500", this, SLOT(updateOverlaySize()));

  property_width_->setMax(500);
  property_width_->setShouldBeSaved(true);

  property_height_ = new rviz_common::properties::IntProperty(
    "Height", 256, "Height of the overlay max:500", this, SLOT(updateOverlaySize()));

  property_height_->setMax(500);
  property_height_->setShouldBeSaved(true);

  property_vertical_margin_ = new rviz_common::properties::IntProperty(
    "Vertical Margin", 10, "Vertical margin of the overlay", this, SLOT(updateOverlayPosition()));
  property_horizontal_margin_ = new rviz_common::properties::IntProperty(
    "Horizontal Margin", 10, "Horizontal margin of the overlay", this,
    SLOT(updateOverlayPosition()));

  property_anchor_vertical_ = new rviz_common::properties::EnumProperty(
    "Vertical Anchor", "Bottom", "Vertical anchor of the overlay", this,
    SLOT(updateOverlayPosition()));

  property_anchor_vertical_->addOption(
    "Top", static_cast<int>(autoware_minimap_overlay_rviz_plugin::VerticalAlignment::TOP));
  property_anchor_vertical_->addOption(
    "Center", static_cast<int>(autoware_minimap_overlay_rviz_plugin::VerticalAlignment::CENTER));
  property_anchor_vertical_->addOption(
    "Bottom", static_cast<int>(autoware_minimap_overlay_rviz_plugin::VerticalAlignment::BOTTOM));

  property_anchor_horizontal_ = new rviz_common::properties::EnumProperty(
    "Horizontal Anchor", "Left", "Horizontal anchor of the overlay", this,
    SLOT(updateOverlayPosition()));

  property_anchor_horizontal_->addOption(
    "Left", static_cast<int>(autoware_minimap_overlay_rviz_plugin::HorizontalAlignment::LEFT));
  property_anchor_horizontal_->addOption(
    "Center", static_cast<int>(autoware_minimap_overlay_rviz_plugin::HorizontalAlignment::CENTER));
  property_anchor_horizontal_->addOption(
    "Right", static_cast<int>(autoware_minimap_overlay_rviz_plugin::HorizontalAlignment::RIGHT));

  property_border_radius_ = new rviz_common::properties::IntProperty(
    "Border Radius", property_height_->getInt() / 2.0, "Border radius of the overlay", this,
    SLOT(updateOverlaySize()));

  alpha_property_ = new rviz_common::properties::FloatProperty(
    "Alpha", 1.0, "Amount of transparency to apply to the overlay.", this,
    SLOT(updateOverlaySize()));

  property_tile_provider_ = new rviz_common::properties::EnumProperty(
    "Tile Provider", "OpenStreetMap", "Select the tile provider", this, SLOT(updateTileProvider()));

  property_tile_provider_->addOption("OpenStreetMap", TileProvider::OpenStreetMap);

  property_zoom_ = new rviz_common::properties::IntProperty(
    "Zoom", 15, "Zoom level of the map 15-18", this, SLOT(updateZoomLevel()));

  property_zoom_->setMin(15);
  property_zoom_->setMax(18);
  property_zoom_->setShouldBeSaved(true);

  property_latitude_ = new rviz_common::properties::FloatProperty(
    "Latitude", 0.0, "Latitude of the ego", this, SLOT(updateLatitude()));

  property_longitude_ = new rviz_common::properties::FloatProperty(
    "Longitude", 0.0, "Longitude of the ego", this, SLOT(updateLongitude()));

  property_goal_lat = new rviz_common::properties::FloatProperty(
    "Goal Latitude", 0.0, "Goal pose Latitude", this, SLOT(updateGoalPose()));

  property_goal_lon = new rviz_common::properties::FloatProperty(
    "Goal Longitude", 0.0, "Goal pose Longitude", this, SLOT(updateGoalPose()));

  property_origin_lat_ = new rviz_common::properties::FloatProperty(
    "Map Origin Latitude", 35.23808753540768, "Latitude of the map origin position", this,
    SLOT(updateLatitude()));

  property_origin_lon_ = new rviz_common::properties::FloatProperty(
    "Map Origin Longitude", 139.9009591876285, "Longitude of the map origin position", this,
    SLOT(updateLongitude()));

  zoom_ = property_zoom_->getInt();

  latitude_ = property_latitude_->getFloat();
  longitude_ = property_longitude_->getFloat();

  tile_field_ = std::make_unique<TileField>(this);
  tile_field_->setUrlTemplate(
    TileProvider::getUrlTemplate(current_tile_provider_));  // Set the URL template

  connect(tile_field_.get(), &TileField::tilesUpdated, this, &VehicleMapDisplay::onTilesUpdated);
}

VehicleMapDisplay::~VehicleMapDisplay()
{
}

void VehicleMapDisplay::onInitialize()
{
  rviz_rendering::RenderSystem::get()->prepareOverlays(scene_manager_);
  static int count = 0;
  std::stringstream ss;
  ss << "AerialMapDisplayObject" << count++;
  overlay_.reset(new autoware_minimap_overlay_rviz_plugin::OverlayObject(ss.str()));
  overlay_->show();
  updateOverlaySize();
  updateOverlayPosition();

  node_ = context_->getRosNodeAbstraction().lock()->get_raw_node();

  // Initialize the goal pose subscriber
  goal_pose_sub_ = node_->create_subscription<geometry_msgs::msg::PoseStamped>(
    "/planning/mission_planning/echo_back_goal_pose", 10,
    std::bind(&VehicleMapDisplay::goalPoseCallback, this, std::placeholders::_1));

  // pose_sub_ = node_->create_subscription<nav_msgs::msg::Odometry>(
  //   "/localization/kinematic_state", 10,
  //   std::bind(&VehicleMapDisplay::poseCallback, this, std::placeholders::_1));

  pose_sub_ = node_->create_subscription<autoware_adapi_v1_msgs::msg::VehicleKinematics>(
    "/api/vehicle/kinematics", rclcpp::QoS(10).best_effort(),
    std::bind(&VehicleMapDisplay::poseCallback, this, std::placeholders::_1));

  route_state_sub_ = node_->create_subscription<autoware_adapi_v1_msgs::msg::RouteState>(
    "/api/routing/state", 10,
    std::bind(&VehicleMapDisplay::routeStateCallback, this, std::placeholders::_1));

  route_points_sub_ = node_->create_subscription<autoware_planning_msgs::msg::Trajectory>(
    "/planning/scenario_planning/trajectory", 10,
    std::bind(&VehicleMapDisplay::routePointsCallback, this, std::placeholders::_1));

  // Subscribe to the map projector info topic
  rclcpp::QoS qos_settings = rclcpp::QoS(1).reliable().transient_local();

  map_projector_info_sub_ = node_->create_subscription<tier4_map_msgs::msg::MapProjectorInfo>(
    "/map/map_projector_info", qos_settings,
    std::bind(&VehicleMapDisplay::mapProjectorInfoCallback, this, std::placeholders::_1));

  // Timer for smooth update
  node_->create_wall_timer(
    std::chrono::milliseconds(16), std::bind(&VehicleMapDisplay::smoothUpdate, this));
}

void VehicleMapDisplay::mapProjectorInfoCallback(
  const tier4_map_msgs::msg::MapProjectorInfo::SharedPtr msg)
{
  map_projector_info_msg_ = msg;

  if (
    msg->projector_type != tier4_map_msgs::msg::MapProjectorInfo::MGRS &&
    msg->projector_type != tier4_map_msgs::msg::MapProjectorInfo::LOCAL_CARTESIAN_UTM) {
    RCLCPP_ERROR_STREAM(
      node_->get_logger(), "Minimap plugin only supports MGRS and UTM for projector type");
  }
  projector_type_ = msg->projector_type;
  mgrs_grid_ = msg->mgrs_grid;

  // For some reason this always returns 0,0 so we need to set it manually in the properties
  // according to the map used
  // property_origin_lat_->setFloat(msg->map_origin.latitude);
  // property_origin_lon_->setFloat(msg->map_origin.longitude);

  goal_pose_.setProjectionInfo(projector_type_, mgrs_grid_);
  path_overlay_.setProjectionInfo(projector_type_, mgrs_grid_);

  updateGoalPose();
  updateMapPosition();
}

void VehicleMapDisplay::reset()
{
}

void VehicleMapDisplay::onEnable()
{
  if (overlay_) {
    overlay_->show();
  }
}

void VehicleMapDisplay::onDisable()
{
  if (overlay_) {
    overlay_->hide();
  }
}

void VehicleMapDisplay::update(float, float)
{
  if (!overlay_) {
    return;
  }

  autoware_minimap_overlay_rviz_plugin::ScopedPixelBuffer buffer = overlay_->getBuffer();
  QImage hud = buffer.getQImage(*overlay_);
  hud.fill(Qt::transparent);
  drawWidget(hud);
}

void VehicleMapDisplay::updateTileProvider()
{
  // Update the current tile provider based on the selected option
  current_tile_provider_ =
    static_cast<TileProvider::Provider>(property_tile_provider_->getOptionInt());

  // Get the URL template based on the selected tile provider
  std::string url_template = TileProvider::getUrlTemplate(current_tile_provider_);

  tile_field_->setUrlTemplate(url_template);

  updateMapPosition();
  queueRender();
}

void VehicleMapDisplay::updateOverlaySize()
{
  if (!overlay_) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  overlay_->updateTextureSize(property_width_->getInt(), property_height_->getInt());
  overlay_->setDimensions(overlay_->getTextureWidth(), overlay_->getTextureHeight());
  queueRender();
}

void VehicleMapDisplay::updateOverlayPosition()
{
  if (!overlay_) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex_);
  overlay_->setPosition(
    property_horizontal_margin_->getInt(), property_vertical_margin_->getInt(),
    static_cast<HorizontalAlignment>(property_anchor_horizontal_->getOptionInt()),
    static_cast<VerticalAlignment>(property_anchor_vertical_->getOptionInt()));
  queueRender();
}

void VehicleMapDisplay::drawWidget(QImage & hud)
{
  std::lock_guard<std::mutex> lock(property_mutex_);

  if (!overlay_->isVisible()) {
    return;
  }

  QPainter painter(&hud);
  painter.setRenderHint(QPainter::Antialiasing, true);

  QRectF backgroundRect(0, 0, hud.width(), hud.height());
  drawCircle(painter, backgroundRect);

  painter.end();
}

void VehicleMapDisplay::drawCircle(QPainter & painter, const QRectF & backgroundRect)
{
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setOpacity(alpha_property_->getFloat());

  // Define the visible rectangle
  QRectF visibleRect(
    backgroundRect.width() / 2 - property_width_->getInt() / 2,
    backgroundRect.height() / 2 - property_height_->getInt() / 2, property_width_->getInt(),
    property_height_->getInt());

  // Define the circular clipping path
  QPainterPath path;
  path.setFillRule(Qt::WindingFill);

  // Define the rectangle dimensions and radius for the bottom-left corner
  QRectF rect = backgroundRect;
  qreal radius = property_border_radius_->getInt();

  // Add a rounded rectangle covering the entire area with all corners rounded
  path.addRoundedRect(rect, radius, radius);
  painter.setClipPath(path.simplified());

  // Draw the background
  painter.setPen(Qt::NoPen);
  painter.drawRect(backgroundRect);

  // Get the tile field image
  QImage tile_field_image = tile_field_->getTileFieldImage();

  // Calculate the target rectangle for the tile field image
  auto [x_pixel, y_pixel] = tile_field_->getTileOffsets(latitude_, longitude_);

  // Adjust the target rectangle such that the position icon is centered in the
  // visible rect
  QRectF target(
    backgroundRect.width() / 2 - x_pixel - 128, backgroundRect.height() / 2 - y_pixel - 128,
    tile_field_image.width(), tile_field_image.height());

  QRectF source(0, 0, tile_field_image.width(), tile_field_image.height());

  // Draw the tile field image
  painter.drawImage(target, tile_field_image, source);

  // Draw the position icon
  std::string package_path =
    ament_index_cpp::get_package_share_directory("autoware_minimap_overlay_rviz_plugin");
  std::string image_path = package_path + "/icons/pos.png";
  QPixmap pos_pixmap(image_path.c_str());
  pos_pixmap = pos_pixmap.scaled(25, 25, Qt::KeepAspectRatio);

  QPointF positionInOverlay =
    backgroundRect.center() - QPointF(pos_pixmap.width() / 4, pos_pixmap.height() / 4);

  if (route_state_msg_ && route_state_msg_->state == autoware_adapi_v1_msgs::msg::RouteState::SET) {
    goal_pose_.draw(painter, backgroundRect, zoom_);
    path_overlay_.draw(painter, backgroundRect, zoom_);
  }
  if (pose_msg_) {  // Assuming pose_msg_ is your pose message
    tf2::Quaternion q(
      pose_msg_->pose.pose.pose.orientation.x, pose_msg_->pose.pose.pose.orientation.y,
      pose_msg_->pose.pose.pose.orientation.z, pose_msg_->pose.pose.pose.orientation.w);
    tf2::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    yaw = -yaw + M_PI / 2;  // Adjust the yaw to match the image orientation

    // Save the painter state
    painter.save();

    // Translate the painter to the center of the position icon
    painter.translate(
      positionInOverlay.x() + pos_pixmap.width() / 2,
      positionInOverlay.y() + pos_pixmap.height() / 2);

    // Rotate the painter around the center of the position icon
    painter.rotate(yaw * 180.0 / M_PI);  // Convert radians to degrees

    // Translate the painter back
    painter.translate(-pos_pixmap.width() / 2, -pos_pixmap.height() / 2);

    // Draw the pixmap
    painter.drawPixmap(0, 0, pos_pixmap);

    // Restore the painter state
    painter.restore();

  } else {
    painter.drawPixmap(positionInOverlay, pos_pixmap);
  }

  queueRender();
}

void VehicleMapDisplay::smoothUpdate()
{
  const double smoothing_factor = 0.08;

  latitude_ = prev_latitude_ + smoothing_factor * (target_latitude_ - prev_latitude_);
  longitude_ = prev_longitude_ + smoothing_factor * (target_longitude_ - prev_longitude_);

  prev_latitude_ = latitude_;
  prev_longitude_ = longitude_;

  updateMapPosition();
}

void VehicleMapDisplay::onTilesUpdated()
{
  queueRender();
}

void VehicleMapDisplay::updateZoomLevel()
{
  zoom_ = property_zoom_->getInt();  // Update the zoom level

  // Update the map position based on the new zoom level
  if (pose_msg_) {
    poseCallback(pose_msg_);
  }

  if (goal_pose_msg_) {
    goalPoseCallback(goal_pose_msg_);
  }

  if (route_points_msg_) {
    routePointsCallback(route_points_msg_);
  }

  tile_field_->fetchTiles(zoom_, center_x_tile_, center_y_tile_);
  updateMapPosition();
  queueRender();  // Request re-rendering
}

void VehicleMapDisplay::updateLatitude()
{
  latitude_ = property_latitude_->getFloat();
  target_latitude_ = latitude_;
  int new_center_x_tile = tile_field_->long_to_tile_x(longitude_, zoom_);
  int new_center_y_tile = tile_field_->lat_to_tile_y(latitude_, zoom_);
  center_x_tile_ = new_center_x_tile;
  center_y_tile_ = new_center_y_tile;
  tile_field_->fetchTiles(zoom_, center_x_tile_, center_y_tile_);
  queueRender();
}

void VehicleMapDisplay::updateLongitude()
{
  longitude_ = property_longitude_->getFloat();
  target_longitude_ = longitude_;
  int new_center_x_tile = tile_field_->long_to_tile_x(longitude_, zoom_);
  int new_center_y_tile = tile_field_->lat_to_tile_y(latitude_, zoom_);
  center_x_tile_ = new_center_x_tile;
  center_y_tile_ = new_center_y_tile;
  tile_field_->fetchTiles(zoom_, center_x_tile_, center_y_tile_);
  queueRender();
}

void VehicleMapDisplay::updateMapPosition()
{
  int new_center_x_tile = tile_field_->long_to_tile_x(longitude_, zoom_);
  int new_center_y_tile = tile_field_->lat_to_tile_y(latitude_, zoom_);
  center_x_tile_ = new_center_x_tile;
  center_y_tile_ = new_center_y_tile;
  tile_field_->fetchTiles(zoom_, new_center_x_tile, new_center_y_tile);
  queueRender();
}

void VehicleMapDisplay::updateGoalPose()
{
  double goal_x = property_goal_lat->getFloat();
  double goal_y = property_goal_lon->getFloat();
  double origin_lat = property_origin_lat_->getFloat();
  double origin_lon = property_origin_lon_->getFloat();
  goal_pose_.setGoalPosition(goal_x, goal_y, origin_lat, origin_lon);
  if (!goal_pose_msg_) {
    return;
  }
  goalPoseCallback(goal_pose_msg_);

  if (route_points_msg_) {
    routePointsCallback(route_points_msg_);
  }
  queueRender();
}

void VehicleMapDisplay::routePointsCallback(
  const autoware_planning_msgs::msg::Trajectory::SharedPtr msg)
{
  route_points_msg_ = msg;

  // Convert the route points to local coordinates and set them in the RouteOverlay
  std::vector<PathPoint> path_points;
  for (const auto & pose : msg->points) {
    PathPoint point;
    point.x = pose.pose.position.x;
    point.y = pose.pose.position.y;
    path_points.push_back(point);
  }

  double origin_lat = property_origin_lat_->getFloat();
  double origin_lon = property_origin_lon_->getFloat();
  path_overlay_.setPathPoints(path_points, origin_lat, origin_lon);
  path_overlay_.setVehiclePosition(latitude_, longitude_);

  queueRender();
}

void VehicleMapDisplay::routeStateCallback(
  const autoware_adapi_v1_msgs::msg::RouteState::SharedPtr msg)
{
  route_state_msg_ = msg;

  queueRender();
}

// void VehicleMapDisplay::poseCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
void VehicleMapDisplay::poseCallback(
  const autoware_adapi_v1_msgs::msg::VehicleKinematics::SharedPtr msg)
{
  pose_msg_ = msg;

  latitude_ = msg->geographic_pose.position.latitude;
  longitude_ = msg->geographic_pose.position.longitude;
  target_latitude_ = latitude_;
  target_longitude_ = longitude_;

  property_longitude_->setFloat(target_longitude_);
  property_latitude_->setFloat(target_latitude_);

  goal_pose_.setVehiclePosition(target_latitude_, target_longitude_);
  path_overlay_.setVehiclePosition(target_latitude_, target_longitude_);

  updateMapPosition();  // Method to update map position based on new latitude and longitude
}

void VehicleMapDisplay::goalPoseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg)
{
  goal_pose_msg_ = msg;

  double origin_lat = property_origin_lat_->getFloat();
  double origin_lon = property_origin_lon_->getFloat();
  goal_pose_.setGoalPosition(msg->pose.position.x, msg->pose.position.y, origin_lat, origin_lon);

  // Set the vehicle position in the goal pose (in case it was not set yet)
  goal_pose_.setVehiclePosition(target_latitude_, target_longitude_);
  path_overlay_.setVehiclePosition(target_latitude_, target_longitude_);

  property_goal_lat->setFloat(goal_pose_.getGoalLatitude());
  property_goal_lon->setFloat(goal_pose_.getGoalLongitude());
  queueRender();
}

std::pair<double, double> VehicleMapDisplay::localXYZToLatLon(double x, double y)
{
  if (projector_type_ == "MGRS") {
    return localXYZToLatLonMGRS(x, y);
  } else {
    return localXYZToLatLonUTM(x, y);
  }
}

std::pair<double, double> VehicleMapDisplay::localXYZToLatLonUTM(double x, double y)
{
  int zone;
  bool north_p;
  double origin_lat = property_origin_lat_->getFloat();
  double origin_lon = property_origin_lon_->getFloat();
  double origin_x, origin_y, gamma, k;

  // Convert origin to UTM coordinates
  GeographicLib::UTMUPS::Forward(
    origin_lat, origin_lon, zone, north_p, origin_x, origin_y, gamma, k);

  // Calculate global UTM coordinates by adding local offsets
  double global_x = origin_x + x;
  double global_y = origin_y + y;

  // Convert back to geographic coordinates
  double lat, lon;
  GeographicLib::UTMUPS::Reverse(zone, north_p, global_x, global_y, lat, lon);

  return {lat, lon};
}

std::pair<double, double> VehicleMapDisplay::localXYZToLatLonMGRS(double x, double y)
{
  // Assuming we have the origin_lat_ and origin_lon_ set
  int zone;
  bool north_p;
  double origin_x, origin_y, gamma, k;

  // Convert origin to UTM coordinates
  GeographicLib::UTMUPS::Forward(
    property_origin_lat_->getFloat(), property_origin_lon_->getFloat(), zone, north_p, origin_x,
    origin_y, gamma, k);

  // Calculate global UTM coordinates by adding local offsets
  double global_x = origin_x + x;
  double global_y = origin_y + y;

  // Convert global UTM coordinates to MGRS
  std::string mgrs;
  GeographicLib::MGRS::Forward(zone, north_p, global_x, global_y, 5, mgrs);

  // Convert MGRS coordinates to geographic coordinates
  double lat, lon;
  int prec;
  bool northp;
  GeographicLib::MGRS::Reverse(mgrs, zone, northp, lat, lon, prec);

  return {lat, lon};
}

}  // namespace autoware_minimap_overlay_rviz_plugin

PLUGINLIB_EXPORT_CLASS(
  autoware_minimap_overlay_rviz_plugin::VehicleMapDisplay, rviz_common::Display)

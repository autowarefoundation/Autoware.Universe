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
#include "include/goal_pose.hpp"

#include <QDebug>

#include <qpoint.h>

GoalPose::GoalPose()
{
  std::string package_path =
    ament_index_cpp::get_package_share_directory("autoware_minimap_overlay_rviz_plugin");
  std::string goal_image_path = package_path + "/icons/goal.png";
  goal_image_ = QImage(goal_image_path.c_str());
  goal_image_ = goal_image_.scaled(20, 20, Qt::KeepAspectRatio);
}

void GoalPose::setGoalPosition(double local_x, double local_y, double origin_lat, double origin_lon)
{
  std::tie(goal_lat_, goal_lon_) = localToGeographic(local_x, local_y, origin_lat, origin_lon);
}

void GoalPose::setProjectionInfo(const std::string & projector_type, const std::string & mgrs_grid)
{
  projector_type_ = projector_type;
  mgrs_grid_ = mgrs_grid;
}

std::pair<double, double> GoalPose::localToGeographic(
  double local_x, double local_y, double origin_lat, double origin_lon)
{
  if (projector_type_ == tier4_map_msgs::msg::MapProjectorInfo::MGRS) {
    return localToGeographicMGRS(local_x, local_y, mgrs_grid_);
  } else if (projector_type_ == tier4_map_msgs::msg::MapProjectorInfo::LOCAL_CARTESIAN_UTM) {
    return localToGeographicUTM(local_x, local_y, origin_lat, origin_lon);
  }
  // Return a default pair if none of the conditions match
  return std::make_pair(0.0, 0.0);  // Adjust as necessary
}

std::pair<double, double> GoalPose::localToGeographicUTM(
  double local_x, double local_y, double origin_lat, double origin_lon)
{
  int zone;
  bool north_p;
  double origin_x, origin_y, gamma, k;

  // Convert origin coordinates to UTM
  GeographicLib::UTMUPS::Forward(
    origin_lat, origin_lon, zone, north_p, origin_x, origin_y, gamma, k);

  // Add local coordinates to origin UTM coordinates
  double goal_x = origin_x + local_x;
  double goal_y = origin_y + local_y;

  // Convert UTM coordinates back to geographic coordinates
  double goal_lat, goal_lon;
  GeographicLib::UTMUPS::Reverse(zone, north_p, goal_x, goal_y, goal_lat, goal_lon);

  return {goal_lat, goal_lon};
}

std::pair<double, double> GoalPose::localToGeographicMGRS(
  double local_x, double local_y, const std::string & mgrs_grid)
{
  int zone;
  bool north_p;
  double mgrs_origin_x, mgrs_origin_y;
  int prec;

  // Convert MGRS grid string to UTM coordinates
  GeographicLib::MGRS::Reverse(mgrs_grid, zone, north_p, mgrs_origin_x, mgrs_origin_y, prec, false);

  // Calculate the global UTM coordinates by adding local offsets
  double utm_x = mgrs_origin_x + local_x;
  double utm_y = mgrs_origin_y + local_y;

  // Convert global UTM coordinates to geographic coordinates
  double goal_lat, goal_lon;
  GeographicLib::UTMUPS::Reverse(zone, north_p, utm_x, utm_y, goal_lat, goal_lon);

  return {goal_lat, goal_lon};
}

std::pair<int, int> GoalPose::getTileOffsets(
  double lat, double lon, int zoom, const QRectF & backgroundRect)
{
  double x = (lon + 180.0) / 360.0 * std::pow(2.0, zoom);
  double y =
    (1.0 - std::log(std::tan(lat * M_PI / 180.0) + 1.0 / std::cos(lat * M_PI / 180.0)) / M_PI) /
    2.0 * std::pow(2.0, zoom);

  int x_tile = static_cast<int>(std::floor(x));
  int y_tile = static_cast<int>(std::floor(y));

  int x_offset = static_cast<int>((x - x_tile) * 256);
  int y_offset = static_cast<int>((y - y_tile) * 256);

  // Adjusting to align with the center of the background rectangle
  int centerX = static_cast<int>(backgroundRect.width() / 2);
  int centerY = static_cast<int>(backgroundRect.height() / 2);

  return {centerX + x_offset, centerY + y_offset - 100};
}

void GoalPose::draw(QPainter & painter, const QRectF & backgroundRect, int zoom)
{
  // Get the tile offsets for the vehicle position
  auto [vehicle_x_pixel, vehicle_y_pixel] =
    getTileOffsets(vehicle_lat_, vehicle_lon_, zoom, backgroundRect);

  auto [goal_x_pixel, goal_y_pixel] = getTileOffsets(goal_lat_, goal_lon_, zoom, backgroundRect);

  // Calculate the adjusted goal position relative to the vehicle position
  int adjusted_goal_x = goal_x_pixel - vehicle_x_pixel + backgroundRect.width() / 2;
  int adjusted_goal_y = goal_y_pixel - vehicle_y_pixel + backgroundRect.height() / 2;

  QPointF goalPositionInOverlay(
    QPointF(adjusted_goal_x, adjusted_goal_y) -
    QPointF(goal_image_.width() / 2.0, goal_image_.height() - 10));

  painter.drawImage(goalPositionInOverlay, goal_image_);
}

// Getter methods for the goal latitude and longitude
double GoalPose::getGoalLatitude() const
{
  return goal_lat_;
}

double GoalPose::getGoalLongitude() const
{
  return goal_lon_;
}

// Add the implementation of the new method
void GoalPose::setVehiclePosition(double lat, double lon)
{
  vehicle_lat_ = lat;
  vehicle_lon_ = lon;
}

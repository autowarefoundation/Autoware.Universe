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
#include "include/path_overlay.hpp"

#include <QDebug>

#include <iomanip>
#include <sstream>

PathOverlay::PathOverlay() : vehicle_lat_(0.0), vehicle_lon_(0.0)
{
}

void PathOverlay::setVehiclePosition(double lat, double lon)
{
  vehicle_lat_ = lat;
  vehicle_lon_ = lon;
}

void PathOverlay::setPathPoints(
  const std::vector<PathPoint> & path_points, double origin_lat, double origin_lon)
{
  path_points_geo_.clear();
  for (const auto & point : path_points) {
    auto geo_point = localToGeographic(point.x, point.y, origin_lat, origin_lon);
    path_points_geo_.emplace_back(geo_point);
  }
}

void PathOverlay::setProjectionInfo(
  const std::string & projector_type, const std::string & mgrs_grid)
{
  projector_type_ = projector_type;
  mgrs_grid_ = mgrs_grid;
}

std::pair<double, double> PathOverlay::localToGeographic(
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

std::pair<double, double> PathOverlay::localToGeographicUTM(
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

std::pair<double, double> PathOverlay::localToGeographicMGRS(
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

std::pair<int, int> PathOverlay::getTileOffsets(
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

void PathOverlay::draw(QPainter & painter, const QRectF & backgroundRect, int zoom)
{
  if (path_points_geo_.empty()) {
    return;
  }

  // Get the tile offsets for the vehicle position
  auto [vehicle_x_pixel, vehicle_y_pixel] =
    getTileOffsets(vehicle_lat_, vehicle_lon_, zoom, backgroundRect);

  QPointF previous_point;
  bool first_point = true;

  int line_width = zoom == 15 ? 1 : zoom == 16 ? 2 : 3;

  painter.setPen(QPen(QColor("#00E678"), line_width));  // Set pen color and width for drawing

  // Create a larger clipping path to allow drawing out of bounds
  QPainterPath clipPath;
  clipPath.addEllipse(backgroundRect);
  painter.setClipPath(clipPath);

  for (size_t i = 2; i < path_points_geo_.size() - 2; ++i) {
    const auto & point_geo = path_points_geo_[i];

    auto [point_x_pixel, point_y_pixel] =
      getTileOffsets(point_geo.first, point_geo.second, zoom, backgroundRect);

    // Calculate the adjusted point position relative to the vehicle position
    int adjusted_x = point_x_pixel - vehicle_x_pixel + backgroundRect.width() / 2 + 5;
    int adjusted_y = point_y_pixel - vehicle_y_pixel + backgroundRect.height() / 2 + 5;

    // Skip points that are too far out of bounds
    if (
      adjusted_x < backgroundRect.width() * 0.25 || adjusted_x > backgroundRect.width() * 0.75 ||
      adjusted_y < backgroundRect.height() * 0.25 || adjusted_y > backgroundRect.height() * 0.75) {
      first_point = true;
      continue;
    }

    QPointF current_point(adjusted_x, adjusted_y);

    if (!first_point) {
      painter.drawLine(previous_point, current_point);
    } else {
      first_point = false;
    }

    previous_point = current_point;
  }
}

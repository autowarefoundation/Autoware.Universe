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
#ifndef PATH_OVERLAY_HPP_
#define PATH_OVERLAY_HPP_

#include "GeographicLib/MGRS.hpp"
#include "GeographicLib/UTMUPS.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QRectF>

#include "tier4_map_msgs/msg/map_projector_info.hpp"

#include <string>
#include <tuple>
#include <utility>
#include <vector>
struct PathPoint
{
  double x;
  double y;
};

class PathOverlay
{
public:
  PathOverlay();
  void setVehiclePosition(double lat, double lon);
  void setPathPoints(
    const std::vector<PathPoint> & path_points, double origin_lat, double origin_lon);
  void draw(QPainter & painter, const QRectF & backgroundRect, int zoom);
  void setProjectionInfo(const std::string & projector_type, const std::string & mgrs_grid);

private:
  double vehicle_lat_;
  double vehicle_lon_;
  std::vector<std::pair<double, double>> path_points_geo_;

  std::string projector_type_;
  std::string mgrs_grid_;

  std::pair<double, double> localToGeographic(
    double local_x, double local_y, double origin_lat, double origin_lon);
  std::pair<double, double> localToGeographicUTM(
    double local_x, double local_y, double origin_lat, double origin_lon);
  std::pair<double, double> localToGeographicMGRS(
    double local_x, double local_y, const std::string & mgrs_grid);
  std::pair<int, int> getTileOffsets(
    double lat, double lon, int zoom, const QRectF & backgroundRect);
};

#endif  // PATH_OVERLAY_HPP_

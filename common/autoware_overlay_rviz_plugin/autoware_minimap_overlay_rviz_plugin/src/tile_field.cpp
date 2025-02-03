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
#include "include/tile_field.hpp"

#include <QPainter>
#include <QTimer>
#include <QUrl>

#include <cmath>
#include <stdexcept>

TileField::TileField(QObject * parent) : QObject(parent), center_x_tile_(0), center_y_tile_(0)
{
  url_template_ =
    TileProvider::getUrlTemplate(TileProvider::OpenStreetMap);  // Default URL template
}

TileField::~TileField()
{
  std::lock_guard<std::mutex> lock(tile_mutex_);
  clearTiles();
}

void TileField::clearTiles()
{
  std::lock_guard<std::mutex> lock(tile_mutex_);
  for (auto & tile : tiles_) {
    if (tile.second) {
      disconnect(tile.second.get(), &Tile::tileFetched, this, &TileField::onTileFetched);
      tile.second.reset();
    }
  }
  tiles_.clear();
}

void TileField::fetchTiles(int zoom, int center_x_tile, int center_y_tile)
{
  std::lock_guard<std::mutex> lock(tile_mutex_);
  zoom_ = zoom != 0 ? zoom : zoom_;
  center_x_tile_ = center_x_tile;
  center_y_tile_ = center_y_tile;

  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      int x_tile = center_x_tile_ + dx;
      int y_tile = center_y_tile_ + dy;
      // std::string tile_key =
      //   std::to_string(zoom) + "/" + std::to_string(x_tile) + "/" + std::to_string(y_tile) +
      //   ".png";
      std::string tile_key = getTileKey(zoom_, x_tile, y_tile, url_template_);

      if (tiles_.find(tile_key) == tiles_.end()) {
        // Tile * tile = new Tile(zoom, x_tile, y_tile);
        // tiles_[tile_key] = tile;
        auto tile = std::make_unique<Tile>(zoom, x_tile, y_tile);

        connect(tile.get(), &Tile::tileFetched, this, &TileField::onTileFetched);
        tile->setUrlTemplate(url_template_);

        tile->fetch();
        tiles_[tile_key] = std::move(tile);
      }
    }
  }
}

void TileField::initializeTiles(int center_x_tile, int center_y_tile)
{
  std::lock_guard<std::mutex> lock(tile_mutex_);
  center_x_tile_ = center_x_tile;
  center_y_tile_ = center_y_tile;
  updateTiles(center_x_tile_, center_y_tile_);
}

void TileField::updateTiles(int new_center_x_tile, int new_center_y_tile)
{
  std::lock_guard<std::mutex> lock(tile_mutex_);
  if (new_center_x_tile == center_x_tile_ && new_center_y_tile == center_y_tile_) {
    return;
  }

  center_x_tile_ = new_center_x_tile;
  center_y_tile_ = new_center_y_tile;

  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      int x_tile = center_x_tile_ + dx;
      int y_tile = center_y_tile_ + dy;

      // auto tile_key = QString("%1/%2/%3.png").arg(zoom_).arg(x_tile).arg(y_tile).toStdString();
      std::string tile_key = getTileKey(zoom_, x_tile, y_tile, url_template_);

      if (tiles_.find(tile_key) == tiles_.end()) {
        // Tile * tile = new Tile(zoom_, x_tile, y_tile);
        // tiles_[tile_key] = tile;
        auto tile = std::make_unique<Tile>(zoom_, x_tile, y_tile);

        connect(tile.get(), &Tile::tileFetched, this, &TileField::onTileFetched);
        tile->setUrlTemplate(url_template_);

        tile->fetch();
        tiles_[tile_key] = std::move(tile);
      }
    }
  }
}

void TileField::setUrlTemplate(const std::string & url_template)
{
  std::lock_guard<std::mutex> lock(tile_mutex_);

  if (url_template_ == url_template) {
    return;
  }

  updating_url_template_ = true;  // Indicate that the URL template is being updated

  // Clear existing tiles before fetching new ones, this crashes rviz
  // clearTiles();

  url_template_ = url_template;

  updating_url_template_ = false;  // Indicate that the URL template update is complete

  // Re-fetch tiles with the new URL template after a short delay
  QTimer::singleShot(100, this, [this]() { fetchTiles(zoom_, center_x_tile_, center_y_tile_); });
}

std::string TileField::getTileKey(int zoom, int x, int y, const std::string & url_template) const
{
  return url_template + "/" + std::to_string(zoom) + "/" + std::to_string(x) + "/" +
         std::to_string(y) + ".png";
}

QImage TileField::getTileFieldImage()
{
  std::lock_guard<std::mutex> lock(tile_mutex_);
  if (tile_field_image_.isNull()) {
    int tile_size = 256;  // Assuming tile size is 256x256 pixels
    tile_field_image_ = QImage(tile_size * 3, tile_size * 3, QImage::Format_ARGB32);
  }

  QPainter painter(&tile_field_image_);
  painter.fillRect(tile_field_image_.rect(), Qt::transparent);

  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      int x_tile = center_x_tile_ + dx;
      int y_tile = center_y_tile_ + dy;

      // auto tile_key = QString("%1/%2/%3.png").arg(zoom_).arg(x_tile).arg(y_tile).toStdString();
      std::string tile_key = getTileKey(zoom_, x_tile, y_tile, url_template_);

      float lon = tile_x_to_long(x_tile, zoom_);
      // Round the longitude to 2 decimal places
      lon = std::round(lon * 100) / 100;
      float lat = tile_y_to_lat(y_tile, zoom_);
      // Round the latitude to 2 decimal places
      lat = std::round(lat * 100) / 100;

      QString tile_text = QString("Lat: %1\nLon: %2").arg(lat).arg(lon);

      auto tile_it = tiles_.find(tile_key);
      if (tile_it != tiles_.end() && !tile_it->second->getImage().isNull()) {
        QRectF target((dx + 1) * 256, (dy + 1) * 256, 256, 256);
        QRectF source(0, 0, 256, 256);
        painter.drawImage(target, tile_it->second->getImage(), source);
        // Draw the latitude and longitude on the tile
        // painter.drawText(target, Qt::AlignCenter, tile_text);
      }
    }
  }

  return tile_field_image_;
}

void TileField::onTileFetched()
{
  Q_EMIT tilesUpdated();
}

int TileField::long_to_tile_x(double lon, int z)
{
  return static_cast<int>(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int TileField::lat_to_tile_y(double lat, int z)
{
  double lat_radius = lat * M_PI / 180.0;
  return static_cast<int>(floor((1.0 - asinh(tan(lat_radius)) / M_PI) / 2.0 * (1 << z)));
}

double TileField::tile_x_to_long(int x, int z)
{
  return x / static_cast<double>(1 << z) * 360.0 - 180;
}

double TileField::tile_y_to_lat(int y, int z)
{
  double n = M_PI - 2.0 * M_PI * y / static_cast<double>(1 << z);
  return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

std::pair<int, int> TileField::getTileOffsets(double lat, double lon)
{
  double x = (lon + 180.0) / 360.0 * std::pow(2.0, zoom_);
  double y =
    (1.0 - std::log(std::tan(lat * M_PI / 180.0) + 1.0 / std::cos(lat * M_PI / 180.0)) / M_PI) /
    2.0 * std::pow(2.0, zoom_);

  int x_tile = static_cast<int>(std::floor(x));
  int y_tile = static_cast<int>(std::floor(y));

  int x_offset = static_cast<int>((x - x_tile) * 256);
  int y_offset = static_cast<int>((y - y_tile) * 256);

  return {128 + x_offset, 128 + y_offset};
}

std::pair<double, double> TileField::latLonToTile(double lat, double lon, int zoom)
{
  double x = (lon + 180.0) / 360.0 * std::pow(2.0, zoom);
  double y =
    (1.0 - std::log(std::tan(lat * M_PI / 180.0) + 1.0 / std::cos(lat * M_PI / 180.0)) / M_PI) /
    2.0 * std::pow(2.0, zoom);

  return {x, y};
}

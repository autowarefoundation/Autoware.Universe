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
#ifndef TILE_FIELD_HPP_
#define TILE_FIELD_HPP_

#include "tile.hpp"
#include "tile_provider.hpp"

#include <QImage>
#include <QObject>

#include <cmath>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

class TileField : public QObject
{
  Q_OBJECT

public:
  explicit TileField(QObject * parent = nullptr);
  ~TileField();

  void clearTiles();
  void resetTileImages();
  void setUrlTemplate(const std::string & url_template);
  void initializeTiles(int center_x_tile, int center_y_tile);
  void fetchTiles(int zoom, int center_x_tile, int center_y_tile);
  void updateTiles(int center_x_tile, int center_y_tile);
  QImage getTileFieldImage();
  std::string getTileKey(int zoom, int x, int y, const std::string & url_template) const;

  std::pair<int, int> getTileOffsets(double lat, double lon);
  std::pair<double, double> latLonToTile(double lat, double lon, int zoom);

  int long_to_tile_x(double lon, int z);
  int lat_to_tile_y(double lat, int z);
  double tile_x_to_long(int x, int z);
  double tile_y_to_lat(int y, int z);

Q_SIGNALS:
  void tilesUpdated();

private Q_SLOTS:
  void onTileFetched();

private:
  int zoom_ = 15;
  int center_x_tile_;
  int center_y_tile_;
  std::map<std::string, std::unique_ptr<Tile>> tiles_;
  QImage tile_field_image_;
  std::mutex tile_mutex_;
  std::string url_template_;
  bool updating_url_template_ = false;  // Flag to indicate URL template is being updated
};

#endif  // TILE_FIELD_HPP_

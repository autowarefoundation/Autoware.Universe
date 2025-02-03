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
#ifndef TILE_HPP_
#define TILE_HPP_

#include "tile_provider.hpp"

#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

#include <future>
#include <string>

class Tile : public QObject
{
  Q_OBJECT

public:
  Tile(int zoom, int x, int y, QObject * parent = nullptr);
  ~Tile();
  void fetch();
  QImage getImage() const;
  bool isValidUrl(const std::string & url) const;
  void setUrlTemplate(const std::string & url_template);
  void resetImage();
  void markStale() { stale_ = true; }

  int getZoom() const { return zoom_; }
  int getX() const { return x_; }
  int getY() const { return y_; }

Q_SIGNALS:
  void tileFetched();

private Q_SLOTS:
  void onTileFetched(QNetworkReply * reply);

private:
  int zoom_;
  int x_;
  int y_;
  std::string last_url_;
  QImage image_;
  QNetworkAccessManager * network_manager_;

  std::string url_template_;
  bool stale_ = false;

  std::promise<QImage> tile_promise_;

  std::future<QImage> requestRemote();
};

#endif  // TILE_HPP_

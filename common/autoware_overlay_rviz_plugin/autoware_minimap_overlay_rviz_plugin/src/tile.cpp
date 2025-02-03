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
#include "include/tile.hpp"

#include <QDebug>
#include <QImageReader>
#include <QNetworkRequest>

Tile::Tile(int zoom, int x, int y, QObject * parent)
: QObject(parent), zoom_(zoom), x_(x), y_(y), network_manager_(new QNetworkAccessManager(this))
{
  connect(network_manager_, &QNetworkAccessManager::finished, this, &Tile::onTileFetched);
  url_template_ =
    TileProvider::getUrlTemplate(TileProvider::OpenStreetMap);  // Default URL template
}

Tile::~Tile()
{
}

void Tile::resetImage()
{
  image_ = QImage();  // Reset the image to an empty QImage
}

void Tile::fetch()
{
  if (stale_ || image_.isNull()) {
    stale_ = false;  // Reset the stale flag
    requestRemote();
  }
}

bool Tile::isValidUrl(const std::string & url) const
{
  return last_url_ == url;
}

QImage Tile::getImage() const
{
  return image_;
}

void Tile::setUrlTemplate(const std::string & url_template)
{
  url_template_ = url_template;
}

std::future<QImage> Tile::requestRemote()
{
  QString formatted_url = QString::fromStdString(url_template_)
                            .replace("{z}", QString::number(zoom_))
                            .replace("{x}", QString::number(x_))
                            .replace("{y}", QString::number(y_));
  QUrl url(formatted_url);
  QNetworkRequest request(url);
  char constexpr agent[] = "autoware_minimap_overlay_rviz_plugin";
  request.setHeader(QNetworkRequest::UserAgentHeader, agent);

  last_url_ = url.toString().toStdString();  // Save the URL for validation

  std::promise<QImage> tile_promise;
  this->tile_promise_ = std::move(tile_promise);

  // qDebug() << "Fetching tile from URL:" << url;
  network_manager_->get(request);

  return this->tile_promise_.get_future();
}

void Tile::onTileFetched(QNetworkReply * reply)
{
  // qDebug() << "Tile fetch completed.";
  if (reply->error() == QNetworkReply::NoError) {
    // qDebug() << "Reply received successfully.";
    QImageReader reader(reply);
    if (reader.canRead()) {
      // qDebug() << "ImageReader can read the reply.";
      image_ = reader.read();
      if (image_.isNull()) {
        // qDebug() << "Failed to decode the image.";
      } else {
        // qDebug() << "Image successfully fetched. Image size: " << image_.size();
        tile_promise_.set_value(image_);
        Q_EMIT tileFetched();
      }
    } else {
      // qDebug() << "ImageReader cannot read the reply.";
      tile_promise_.set_value(QImage());
    }
  } else {
    // qDebug() << "Error fetching tile:" << reply->errorString();
    tile_promise_.set_value(QImage());
  }
  reply->deleteLater();
}

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
#include "include/custom_slider.hpp"

CustomSlider::CustomSlider(Qt::Orientation orientation, QWidget * parent)
: QSlider(orientation, parent)
{
  setMinimumHeight(40);  // Ensure there's enough space for the custom track
}

void CustomSlider::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);

  QStyleOptionSlider opt;
  initStyleOption(&opt);

  QRectF rect_groove =
    style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
  double groove_y = rect_groove.center().y();
  QRectF rect_handle =
    style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

  int value = this->value();
  int val_min = this->minimum();
  int val_max = this->maximum();

  double track_height = 10.0;
  double gap_handle_x = 5.0;

  // active track
  if (value > val_min + 1) {
    QRectF rect_before(
      rect_groove.x(), groove_y - track_height / 2,
      rect_handle.center().x() - rect_groove.x() - gap_handle_x, track_height);

    QColor color_track_active(autoware::state_rviz_plugin::colors::default_colors.primary.c_str());
    painter.setBrush(color_track_active);
    // first half of the track with high radius
    painter.drawRoundedRect(
      QRectF(
        rect_before.left(), groove_y - track_height / 2, rect_before.width() / 2 + track_height / 2,
        track_height),
      track_height / 2, track_height / 2);

    // second half of the track with low radius
    painter.drawRoundedRect(
      QRectF(
        rect_before.center().x(), groove_y - track_height / 2, rect_before.width() / 2,
        track_height),
      2.0, 2.0);
  }

  // inactive track
  if (value < val_max - 1) {
    QRectF rect_after(
      rect_handle.center().x() + gap_handle_x, groove_y - track_height / 2,
      rect_groove.right() - rect_handle.center().x() - gap_handle_x, track_height);

    QColor color_track_inactive(
      autoware::state_rviz_plugin::colors::default_colors.primary_container.c_str());
    painter.setBrush(color_track_inactive);
    // second half of the track with high radius
    painter.drawRoundedRect(
      QRectF(
        rect_after.center().x() - track_height / 2, groove_y - track_height / 2,
        rect_after.width() / 2 + track_height / 2, track_height),
      track_height / 2, track_height / 2);

    // first half of the track with low radius
    painter.drawRoundedRect(
      QRectF(rect_after.left(), groove_y - track_height / 2, rect_after.width() / 2, track_height),
      2.0, 2.0);
  }

  double handle_height = 28.0;
  double handle_width = 3.0;
  double handle_corner_radius = 2.0;
  QColor color_handle(autoware::state_rviz_plugin::colors::default_colors.primary.c_str());
  painter.setBrush(color_handle);
  painter.drawRoundedRect(
    QRectF(
      rect_handle.center().x() - handle_width / 2, groove_y - handle_height / 2, handle_width,
      handle_height),
    handle_corner_radius, handle_corner_radius);
}

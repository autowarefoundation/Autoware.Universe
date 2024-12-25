// Copyright 2024 TIER IV
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

// NOTE(soblin): this file is intentionally inline to deal with link issue

#ifndef AUTOWARE_TEST_UTILS__VISUALIZATION_HPP_
#define AUTOWARE_TEST_UTILS__VISUALIZATION_HPP_

#include <autoware/pyplot/patches.hpp>
#include <autoware/pyplot/pyplot.hpp>

#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <pybind11/stl.h>

#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace autoware::test_utils
{

struct PointConfig
{
  std::optional<std::string> color{};
  std::optional<std::string> marker{};
  std::optional<double> marker_size{};
};

struct LineConfig
{
  static constexpr const char * default_color = "blue";
  static LineConfig defaults()
  {
    return LineConfig{std::string(default_color), 1.0, "solid", std::nullopt};
  }
  std::optional<std::string> color{};
  std::optional<double> linewidth{};
  std::optional<std::string> linestyle{};
  std::optional<std::string> label{};
};

struct LaneConfig
{
  static LaneConfig defaults() { return LaneConfig{std::nullopt, LineConfig::defaults(), true}; }

  std::optional<std::string> label{};
  std::optional<LineConfig> line_config{};
  bool plot_centerline = true;  //<! if true, centerline is plotted in the same style as line_config
                                // except for {"style"_a = "dashed"}
};

struct PolygonConfig
{
  std::optional<double> alpha{};
  std::optional<std::string> color{};
  bool fill{true};
  std::optional<double> linewidth{};
  std::optional<PointConfig> point_config{};
};

/**
 * @brief plot the linestring by `axes.plot()`
 * @param [in] config_opt argument for plotting the linestring. if valid, each field is used as the
 * kwargs
 */
inline void plot_lanelet2_object(
  const lanelet::ConstLineString3d & line, autoware::pyplot::Axes & axes,
  const std::optional<LineConfig> & config_opt = std::nullopt)
{
  py::dict kwargs{};
  if (config_opt) {
    const auto & config = config_opt.value();
    if (config.color) {
      kwargs["color"] = config.color.value();
    }
    if (config.linewidth) {
      kwargs["linewidth"] = config.linewidth.value();
    }
    if (config.linestyle) {
      kwargs["linestyle"] = config.linestyle.value();
    }
    if (config.label) {
      kwargs["label"] = config.label.value();
    }
  }
  std::vector<double> xs;
  for (const auto & p : line) {
    xs.emplace_back(p.x());
  }
  std::vector<double> ys;
  for (const auto & p : line) {
    ys.emplace_back(p.y());
  }
  axes.plot(Args(xs, ys), kwargs);
}

/**
 * @brief plot the left/right bounds and optionally centerline
 * @param [in] args used for plotting the left/right bounds as
 */
inline void plot_lanelet2_object(
  const lanelet::ConstLanelet & lanelet, autoware::pyplot::Axes & axes,
  const std::optional<LaneConfig> & config_opt = std::nullopt)
{
  const auto left = lanelet.leftBound();
  const auto right = lanelet.rightBound();

  const auto line_config = [&]() -> std::optional<LineConfig> {
    if (!config_opt) {
      return LineConfig{std::string(LineConfig::default_color)};
    }
    return config_opt.value().line_config;
  }();

  if (config_opt) {
    const auto & config = config_opt.value();

    // plot lanelet centerline
    if (config.plot_centerline) {
      auto centerline_config = [&]() -> LineConfig {
        if (!config.line_config) {
          return LineConfig{"k", std::nullopt, "dashed"};
        }
        auto cfg = config.line_config.value();
        cfg.color = "k";
        cfg.linestyle = "dashed";
        return cfg;
      }();
      plot_lanelet2_object(
        lanelet.centerline(), axes, std::make_optional<LineConfig>(std::move(centerline_config)));
    }

    // plot lanelet-id
    const auto center = (left.front().basicPoint2d() + left.back().basicPoint2d() +
                         right.front().basicPoint2d() + right.back().basicPoint2d()) /
                        4.0;
    axes.text(Args(center.x(), center.y(), std::to_string(lanelet.id())));
  }

  if (config_opt && config_opt.value().label) {
    auto left_line_config_for_legend = line_config ? line_config.value() : LineConfig::defaults();
    left_line_config_for_legend.label = config_opt.value().label.value();

    // plot left
    plot_lanelet2_object(lanelet.leftBound(), axes, left_line_config_for_legend);

    // plot right
    plot_lanelet2_object(lanelet.rightBound(), axes, line_config);
  } else {
    // plot left
    plot_lanelet2_object(lanelet.leftBound(), axes, line_config);

    // plot right
    plot_lanelet2_object(lanelet.rightBound(), axes, line_config);
  }

  // plot centerline direction
  const auto centerline_size = lanelet.centerline().size();
  const auto mid_index = centerline_size / 2;
  const auto before = static_cast<size_t>(std::max<int>(0, mid_index - 1));
  const auto after = static_cast<size_t>(std::min<int>(centerline_size - 1, mid_index + 1));
  const auto p_before = lanelet.centerline()[before];
  const auto p_after = lanelet.centerline()[after];
  const double azimuth = std::atan2(p_after.y() - p_before.y(), p_after.x() - p_before.x());
  const auto & mid = lanelet.centerline()[mid_index];
  axes.quiver(
    Args(mid.x(), mid.y(), std::cos(azimuth), std::sin(azimuth)), Kwargs("units"_a = "width"));
}

/**
 * @brief plot the polygon as matplotlib.patches.Polygon
 * @param [in] config_opt argument for plotting the polygon. if valid, each field is used as the
 * kwargs
 */
inline void plot_lanelet2_object(
  const lanelet::ConstPolygon3d & polygon, autoware::pyplot::Axes & axes,
  const std::optional<PolygonConfig> & config_opt = std::nullopt)
{
  std::vector<std::vector<double>> xy(polygon.size());
  for (unsigned i = 0; i < polygon.size(); ++i) {
    xy.at(i) = std::vector<double>({polygon[i].x(), polygon[i].y()});
  }
  py::dict kwargs;
  if (config_opt) {
    const auto & config = config_opt.value();
    if (config.alpha) {
      kwargs["alpha"] = config.alpha.value();
    }
    if (config.color) {
      kwargs["color"] = config.color.value();
    }
    kwargs["fill"] = config.fill;
    if (config.linewidth) {
      kwargs["linewidth"] = config.linewidth.value();
    }
  }
  auto poly = autoware::pyplot::Polygon(Args(xy), kwargs);
  axes.add_patch(Args(poly.unwrap()));
}

/**
 * @brief plot the point by `axes.plot()`
 * @param [in] config_opt argument for plotting the point. if valid, each field is used as the
 * kwargs
 */
/*
void plot_lanelet2_point(
const lanelet::ConstPoint3d & point, autoware::pyplot::Axes & axes,
const std::optional<PointConfig> & config_opt = std::nullopt);
*/
}  // namespace autoware::test_utils

#endif  // AUTOWARE_TEST_UTILS__VISUALIZATION_HPP_

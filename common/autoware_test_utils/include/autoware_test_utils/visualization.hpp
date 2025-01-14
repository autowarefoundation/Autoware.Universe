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
#include <autoware/universe_utils/geometry/geometry.hpp>

#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

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
    axes.text(
      Args(center.x(), center.y(), std::to_string(lanelet.id())), Kwargs("clip_on"_a = true));
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

struct DrivableAreaConfig
{
  static DrivableAreaConfig defaults() { return {"turquoise", 2.0}; }
  std::optional<std::string> color{};
  std::optional<double> linewidth{};
};

struct PathWithLaneIdConfig
{
  static PathWithLaneIdConfig defaults()
  {
    return {std::nullopt, "k", 1.0, std::nullopt, false, 1.0};
  }
  std::optional<std::string> label{};
  std::optional<std::string> color{};
  std::optional<double> linewidth{};
  std::optional<DrivableAreaConfig> da{};
  bool lane_id{};           //<! flag to plot lane_id text
  double quiver_size{1.0};  //<! quiver color is same as `color` or "k" if it is null
};

/**
 * @brief plot path_with_lane_id
 * @param [in] config_opt if null, only the path points & quiver are plotted with "k" color.
 */
inline void plot_autoware_object(
  const tier4_planning_msgs::msg::PathWithLaneId & path, autoware::pyplot::Axes & axes,
  const std::optional<PathWithLaneIdConfig> & config_opt = std::nullopt)
{
  py::dict kwargs{};
  if (config_opt) {
    const auto & config = config_opt.value();
    if (config.label) {
      kwargs["label"] = config.label.value();
    }
    if (config.color) {
      kwargs["color"] = config.color.value();
    }
    if (config.linewidth) {
      kwargs["linewidth"] = config.linewidth.value();
    }
  }
  std::vector<double> xs;
  std::vector<double> ys;
  std::vector<double> yaw_cos;
  std::vector<double> yaw_sin;
  std::vector<std::vector<lanelet::Id>> ids;
  const bool plot_lane_id = config_opt ? config_opt.value().lane_id : false;
  for (const auto & point : path.points) {
    xs.push_back(point.point.pose.position.x);
    ys.push_back(point.point.pose.position.y);
    const auto th = autoware::universe_utils::getRPY(point.point.pose.orientation).z;
    yaw_cos.push_back(std::cos(th));
    yaw_sin.push_back(std::sin(th));
    if (plot_lane_id) {
      ids.emplace_back();
      for (const auto & id : point.lane_ids) {
        ids.back().push_back(id);
      }
    }
  }
  // plot centerline
  axes.plot(Args(xs, ys), kwargs);
  const auto quiver_scale =
    config_opt ? config_opt.value().quiver_size : PathWithLaneIdConfig::defaults().quiver_size;
  const auto quiver_color =
    config_opt ? (config_opt.value().color ? config_opt.value().color.value() : "k") : "k";
  axes.quiver(
    Args(xs, ys, yaw_cos, yaw_sin), Kwargs(
                                      "angles"_a = "xy", "scale_units"_a = "xy",
                                      "scale"_a = quiver_scale, "color"_a = quiver_color));
  if (plot_lane_id) {
    for (size_t i = 0; i < xs.size(); ++i) {
      std::stringstream ss;
      const char * delimiter = "";
      for (const auto id : ids[i]) {
        ss << std::exchange(delimiter, ",") << id;
      }
      axes.text(Args(xs[i], ys[i], ss.str()), Kwargs("clip_on"_a = true));
    }
  }
  // plot drivable area
  if (config_opt && config_opt.value().da) {
    auto plot_boundary = [&](const decltype(path.left_bound) & points) {
      std::vector<double> xs;
      std::vector<double> ys;
      for (const auto & point : points) {
        xs.push_back(point.x);
        ys.push_back(point.y);
      }
      const auto & cfg = config_opt.value().da.value();
      py::dict kwargs{};
      if (cfg.color) {
        kwargs["color"] = cfg.color.value();
      }
      if (cfg.linewidth) {
        kwargs["linewidth"] = cfg.linewidth.value();
      }
      axes.plot(Args(xs, ys), kwargs);
    };
    plot_boundary(path.left_bound);
    plot_boundary(path.right_bound);
  }
}

}  // namespace autoware::test_utils

#endif  // AUTOWARE_TEST_UTILS__VISUALIZATION_HPP_

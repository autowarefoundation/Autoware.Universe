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

#ifndef AUTOWARE__DYNAMIC_LANELET_PROVIDER__DYNAMIC_LANELET_PROVIDER_HPP_
#define AUTOWARE__DYNAMIC_LANELET_PROVIDER__DYNAMIC_LANELET_PROVIDER_HPP_

#include <component_interface_specs/map.hpp>
#include <component_interface_utils/rclcpp.hpp>
#include <rclcpp/rclcpp.hpp>

#include "autoware_map_msgs/msg/lanelet_map_meta_data.hpp"
#include "autoware_map_msgs/srv/get_selected_lanelet2_map.hpp"
#include <geometry_msgs/msg/point.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tier4_map_msgs/msg/map_projector_info.hpp>

#include <optional>
#include <string>
#include <vector>

namespace autoware
{
namespace dynamic_lanelet_provider
{

struct Lanelet2FileMetaData
{
  std::string id;
  double min_x;
  double max_x;
  double min_y;
  double max_y;
};

class DynamicLaneletProviderNode : public rclcpp::Node
{
public:
  explicit DynamicLaneletProviderNode(const rclcpp::NodeOptions & options);

private:
  void onOdometry(const nav_msgs::msg::Odometry::ConstSharedPtr & msg);
  void mapUpdateTimerCallback();

  void updateMap(const geometry_msgs::msg::Point & pose);
  bool should_update_map() const;

  rclcpp::Publisher<autoware_map_msgs::msg::LaneletMapBin>::SharedPtr dynamic_map_pub_;

  rclcpp::Client<autoware_map_msgs::srv::GetSelectedLanelet2Map>::SharedPtr map_loader_client_;

  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odometry_sub_;

  component_interface_utils::Subscription<map_interface::LaneletMapMetaData>::SharedPtr
    lanelet_map_meta_data_sub_;

  rclcpp::TimerBase::SharedPtr map_update_timer_;

  rclcpp::CallbackGroup::SharedPtr client_callback_group_;
  rclcpp::CallbackGroup::SharedPtr timer_callback_group_;

  std::string map_frame_;

  std::optional<geometry_msgs::msg::Point> last_update_position_ = std::nullopt;
  std::optional<geometry_msgs::msg::Point> current_position_ = std::nullopt;

  const double dynamic_map_loading_update_distance_;
  const double dynamic_map_loading_map_radius_;

  std::vector<Lanelet2FileMetaData> lanelet_map_meta_data_list_;
};
}  // namespace dynamic_lanelet_provider
}  // namespace autoware

#endif  // AUTOWARE__DYNAMIC_LANELET_PROVIDER__DYNAMIC_LANELET_PROVIDER_HPP_

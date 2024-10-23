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

#include "autoware/dynamic_lanelet_provider/dynamic_lanelet_provider.hpp"

namespace autoware
{
namespace dynamic_lanelet_provider
{

// Define a helper function to get x and y
template <typename T>
auto getX(const T & point) -> decltype(point.x)
{
  return point.x;
}

template <typename T>
auto getY(const T & point) -> decltype(point.y)
{
  return point.y;
}

// Define a helper function to get x() and y()
template <typename T>
auto getX(const T & point) -> decltype(point.x())
{
  return point.x();
}

template <typename T>
auto getY(const T & point) -> decltype(point.y())
{
  return point.y();
}

template <typename T, typename U>
double norm_xy(const T & p1, const U & p2)
{
  double dx = getX(p1) - getX(p2);
  double dy = getY(p1) - getY(p2);
  return std::hypot(dx, dy);
}

template <typename T>
bool is_inside_region(
  const double & min_x, const double & max_x, const double & min_y, const double & max_y,
  const T & point)
{
  return min_x <= getX(point) && getX(point) <= max_x && min_y <= getY(point) &&
         getY(point) <= max_y;
}

DynamicLaneletProviderNode::DynamicLaneletProviderNode(const rclcpp::NodeOptions & options)
: Node("dynamic_lanelet_provider", options),
  map_frame_("map"),
  dynamic_map_loading_update_distance_(
    declare_parameter<double>("dynamic_map_loading_update_distance")),
  dynamic_map_loading_map_radius_(declare_parameter<double>("dynamic_map_loading_map_radius"))
{
  client_callback_group_ =
    this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  timer_callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  dynamic_map_pub_ = this->create_publisher<autoware_map_msgs::msg::LaneletMapBin>(
    "output/lanelet2_map", rclcpp::QoS{1}.transient_local());

  map_loader_client_ = this->create_client<autoware_map_msgs::srv::GetSelectedLanelet2Map>(
    "service/get_differential_lanelet_map", rmw_qos_profile_services_default,
    client_callback_group_);

  odometry_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
    "input/odometry", 1,
    std::bind(&DynamicLaneletProviderNode::onOdometry, this, std::placeholders::_1));

  const auto metadata_adaptor = component_interface_utils::NodeAdaptor(this);
  metadata_adaptor.init_sub(
    lanelet_map_meta_data_sub_,
    [this](const autoware_map_msgs::msg::LaneletMapMetaData::SharedPtr msg) {
      for (const auto & data : msg->metadata_list) {
        Lanelet2FileMetaData metadata;
        metadata.id = data.cell_id;
        metadata.min_x = data.min_x;
        metadata.max_x = data.max_x;
        metadata.min_y = data.min_y;
        metadata.max_y = data.max_y;
        lanelet_map_meta_data_list_.push_back(metadata);
      }
    });

  double map_update_dt = 1.0;
  auto period_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(map_update_dt));
  map_update_timer_ = rclcpp::create_timer(
    this, get_clock(), period_ns,
    std::bind(&DynamicLaneletProviderNode::mapUpdateTimerCallback, this), timer_callback_group_);
}

void DynamicLaneletProviderNode::mapUpdateTimerCallback()
{
  if (current_position_ == std::nullopt) {
    RCLCPP_ERROR_STREAM_THROTTLE(
      get_logger(), *get_clock(), 1,
      "Cannot find the reference position for lanelet map update. Please check if the EKF odometry "
      "is provided to behavior planner map update module.");
    return;
  }

  if (lanelet_map_meta_data_list_.empty()) {
    RCLCPP_ERROR_ONCE(get_logger(), "Check your lanelet map metadata and projector info.");
    return;
  }

  if (should_update_map()) {
    RCLCPP_INFO(get_logger(), "Start updating lanelet map (timer callback)");

    last_update_position_ = current_position_;
    updateMap(current_position_.value());
  }
}

void DynamicLaneletProviderNode::updateMap(const geometry_msgs::msg::Point & pose)
{
  std::vector<std::string> cache_ids;
  for (const auto & metadata : lanelet_map_meta_data_list_) {
    geometry_msgs::msg::Point point;
    point.x = (metadata.min_x + metadata.max_x) / 2;
    point.y = (metadata.min_y + metadata.max_y) / 2;

    if (is_inside_region(metadata.min_x, metadata.max_x, metadata.min_y, metadata.max_y, pose)) {
      cache_ids.push_back(metadata.id);
      continue;
    }

    double distance = norm_xy(point, pose);
    if (distance < dynamic_map_loading_map_radius_) {
      cache_ids.push_back(metadata.id);
    }
  }

  if (cache_ids.empty()) {
    RCLCPP_ERROR(get_logger(), "No lanelet map is found in the radius.");
    return;
  }

  auto request = std::make_shared<autoware_map_msgs::srv::GetSelectedLanelet2Map::Request>();
  request->cell_ids.insert(request->cell_ids.end(), cache_ids.begin(), cache_ids.end());

  while (!map_loader_client_->wait_for_service(std::chrono::seconds(1)) && rclcpp::ok()) {
    RCLCPP_INFO(get_logger(), "Waiting for lanelet loader service");
  }

  auto result{map_loader_client_->async_send_request(
    request, [](rclcpp::Client<autoware_map_msgs::srv::GetSelectedLanelet2Map>::SharedFuture) {})};

  std::future_status status = result.wait_for(std::chrono::seconds(0));
  while (status != std::future_status::ready) {
    switch (status) {
      case std::future_status::ready:
        RCLCPP_INFO(get_logger(), "The future status is (ready).");
        break;
      case std::future_status::timeout:
        RCLCPP_INFO(get_logger(), "The future status is (timed out).");
        break;
      case std::future_status::deferred:
        RCLCPP_INFO(get_logger(), "The future status is (deferred).");
        break;
    }
    RCLCPP_INFO(get_logger(), "waiting response from lanelet loader service.");
    if (!rclcpp::ok()) {
      return;
    }
    status = result.wait_for(std::chrono::seconds(1));
  }

  dynamic_map_pub_->publish(result.get()->lanelet2_cells);
}

void DynamicLaneletProviderNode::onOdometry(const nav_msgs::msg::Odometry::ConstSharedPtr & msg)
{
  current_position_ = msg->pose.pose.position;
}

bool DynamicLaneletProviderNode::should_update_map() const
{
  if (last_update_position_ == std::nullopt) {
    return true;
  }

  double distance = norm_xy(current_position_.value(), last_update_position_.value());
  return distance > dynamic_map_loading_update_distance_;
}

}  // namespace dynamic_lanelet_provider
}  // namespace autoware

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(autoware::dynamic_lanelet_provider::DynamicLaneletProviderNode)

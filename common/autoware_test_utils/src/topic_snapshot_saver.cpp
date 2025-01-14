// Copyright 2024 TIER IV, Inc.
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

#include <autoware_test_utils/autoware_test_utils.hpp>
#include <autoware_test_utils/mock_data_parser.hpp>
#include <rclcpp/logger.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_adapi_v1_msgs/msg/operation_mode_state.hpp>
#include <autoware_perception_msgs/msg/predicted_objects.hpp>
#include <autoware_perception_msgs/msg/tracked_objects.hpp>
#include <autoware_perception_msgs/msg/traffic_light_group_array.hpp>
#include <autoware_planning_msgs/msg/lanelet_route.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <std_srvs/srv/empty.hpp>
#include <tier4_planning_msgs/msg/path_with_lane_id.hpp>

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using MessageType = std::variant<
  nav_msgs::msg::Odometry,                                // 0
  geometry_msgs::msg::AccelWithCovarianceStamped,         // 1
  autoware_perception_msgs::msg::PredictedObjects,        // 2
  autoware_adapi_v1_msgs::msg::OperationModeState,        // 3
  autoware_planning_msgs::msg::LaneletRoute,              // 4
  autoware_perception_msgs::msg::TrafficLightGroupArray,  // 5
  autoware_perception_msgs::msg::TrackedObjects,          // 6
  tier4_planning_msgs::msg::PathWithLaneId                // 7
  >;

std::optional<size_t> get_topic_index(const std::string & name)
{
  if (name == "Odometry") {
    return 0;
  }
  if (name == "AccelWithCovarianceStamped") {
    return 1;
  }
  if (name == "PredictedObjects") {
    return 2;
  }
  if (name == "OperationModeState") {
    return 3;
  }
  if (name == "LaneletRoute") {
    return 4;
  }
  if (name == "TrafficLightGroupArray") {
    return 5;
  }
  if (name == "TrackedObjects") {
    return 6;
  }
  if (name == "PathWithLaneId") {
    return 7;
  }
  return std::nullopt;
}

std::mutex g_mutex;

template <typename Message>
class CallbackHandler
{
public:
  explicit CallbackHandler(std::shared_ptr<MessageType> buffer) : buffer_(buffer) {}

  void on_callback(const typename Message::SharedPtr msg)
  {
    std::lock_guard guard(g_mutex);
    *buffer_ = *msg;
  }

private:
  std::shared_ptr<MessageType> buffer_;
};

namespace detail
{
template <typename T>
struct CallbackHandlerTypeFunctor;

template <typename... Ts>
struct CallbackHandlerTypeFunctor<std::variant<Ts...>>
{
  using type = std::variant<CallbackHandler<Ts>...>;
};

template <size_t TypeIndex>
struct RosMsgTypeFunctor
{
  static_assert(TypeIndex < std::variant_size_v<MessageType>);
  using type = std::variant_alternative_t<TypeIndex, MessageType>;
};

}  // namespace detail

/**
 * @brief convert std::variant<T1...Tn> to std::variant<CallbackHandler<T1>...CallbackHandler<Tn>>
 */
template <typename T>
using CallbackHandlerType = typename detail::CallbackHandlerTypeFunctor<T>::type;

/**
 * @brief get n-th type of MessageType
 */
template <size_t TypeIndex>
using RosMsgType = typename detail::RosMsgTypeFunctor<TypeIndex>::type;

template <size_t TypeIndex, typename Callback>
typename rclcpp::SubscriptionBase::SharedPtr create_subscriber(
  const std::string & topic_name, rclcpp::Node & node, Callback && callback)
{
  return node.create_subscription<RosMsgType<TypeIndex>>(
    topic_name, 1, std::forward<Callback>(callback));
}

class TopicSnapShotSaver
{
public:
  TopicSnapShotSaver(
    const std::string & map_path, const std::string & map_path_uri, const std::string & config_path,
    rclcpp::Node & node)
  : map_path_(map_path), map_path_uri_(map_path_uri), config_path_(config_path), node_(node)
  {
    std::lock_guard guard(g_mutex);

    server_ = node_.create_service<std_srvs::srv::Empty>(
      "/autoware_test_utils/topic_snapshot_saver",
      std::bind(
        &TopicSnapShotSaver::on_service, this, std::placeholders::_1, std::placeholders::_2));

    // setup subscribers and callbacks
    const auto config = YAML::LoadFile(config_path);
    for (const auto field : config["fields"]) {
      const auto name = field["name"].as<std::string>();
      const auto type_index_opt = get_topic_index(field["type"].as<std::string>());
      if (!type_index_opt) {
        continue;
      }
      const auto type_index = type_index_opt.value();
      const auto topic = field["topic"].as<std::string>();
      field_2_topic_type_[name] = type_index;

      /**
       * NOTE: for a specific topic-type, only one topic-name is allowed
       *
       * `message_buffer_` holds buffer for each topic-type for the lifetime duration of this class
       *
       * `subscribers_` holds a subscriber for each topic-type for the lifetime duration of this
       * class
       *
       * `handlers_` holds a handler for each topic-type for the lifetime duration of this
       * class
       *
       */

#define REGISTER_CALLBACK(arg)                                                                     \
  if ((arg) == type_index) {                                                                       \
    using RosMsgTypeI = RosMsgType<(arg)>;                                                         \
    RosMsgTypeI payload{};                                                                         \
    auto msg = std::make_shared<MessageType>(payload);                                             \
    message_buffer_[arg] = msg;                                                                    \
    auto handler =                                                                                 \
      std::make_shared<CallbackHandlerType<MessageType>>(CallbackHandler<RosMsgTypeI>(msg));       \
    handlers_.emplace_back(handler);                                                               \
    auto & handler_ref = std::get<CallbackHandler<RosMsgTypeI>>(*handler);                         \
    subscribers_[arg] = create_subscriber<arg>(                                                    \
      topic, node,                                                                                 \
      std::bind(&CallbackHandler<RosMsgTypeI>::on_callback, &handler_ref, std::placeholders::_1)); \
  }

      handlers_.reserve(std::variant_size_v<MessageType>);
      REGISTER_CALLBACK(0);
      REGISTER_CALLBACK(1);
      REGISTER_CALLBACK(2);
      REGISTER_CALLBACK(3);
      REGISTER_CALLBACK(4);
      REGISTER_CALLBACK(5);
      REGISTER_CALLBACK(6);
      REGISTER_CALLBACK(7);
    }
  }

private:
  const std::string map_path_;
  const std::string map_path_uri_;
  const std::string config_path_;
  rclcpp::Node & node_;

  rclcpp::Service<std_srvs::srv::Empty>::SharedPtr server_;

  std::unordered_map<size_t, rclcpp::SubscriptionBase::SharedPtr> subscribers_;
  std::vector<std::shared_ptr<CallbackHandlerType<MessageType>>> handlers_;
  std::unordered_map<size_t, std::shared_ptr<MessageType>> message_buffer_;
  std::unordered_map<std::string, size_t> field_2_topic_type_;
  void on_service(
    [[maybe_unused]] const std::shared_ptr<std_srvs::srv::Empty::Request> req,
    [[maybe_unused]] std::shared_ptr<std_srvs::srv::Empty::Response> res)
  {
    std::lock_guard guard(g_mutex);

    YAML::Node yaml;

    yaml["format_version"] = 1;

    yaml["map_path_uri"] = map_path_uri_;

    for (const auto & [field, type_index] : field_2_topic_type_) {
      // instantiate for each type

#define REGISTER_WRITE_TYPE(arg)                                   \
  if ((arg) == type_index) {                                       \
    const auto it = message_buffer_.find(arg);                     \
    if (it == message_buffer_.end()) {                             \
      continue;                                                    \
    }                                                              \
    const auto & msg = std::get<RosMsgType<(arg)>>(*(it->second)); \
    yaml[field] = YAML::Load(to_yaml(msg));                        \
  }

      REGISTER_WRITE_TYPE(0);
      REGISTER_WRITE_TYPE(1);
      REGISTER_WRITE_TYPE(2);
      REGISTER_WRITE_TYPE(3);
      REGISTER_WRITE_TYPE(4);
      REGISTER_WRITE_TYPE(5);
      REGISTER_WRITE_TYPE(6);
      REGISTER_WRITE_TYPE(7);
    }

    const std::string desc = std::string(R"(#
# AUTO GENERATED by autoware_test_utils::topic_snapshot_saver
# format1:
#
# format_version: <format-major-version, int>
# map_path_uri: package://<package-name>/<resource-path>
# fields(this is array)
#   - name: <field-name-for-your-yaml-of-this-topic, str>
#     type: either {Odometry | AccelWithCovarianceStamped | PredictedObjects | OperationModeState | LaneletRoute | TrafficLightGroupArray | TrackedObjects | PathWithLaneId | TBD}
#     topic: <topic-name, str>
#
)");

    std::ofstream ofs("topic_snapshot.yaml");
    ofs << desc;
    ofs << yaml;

    std::cout << "saved planner_data" << std::endl;
  }
};

class TopicSnapShotSaverFrontEnd : public rclcpp::Node
{
public:
  TopicSnapShotSaverFrontEnd() : Node("topic_snapshot_saver_frontend")
  {
    const auto map_path_uri = declare_parameter<std::string>("map_path", "none");
    const auto config_path_uri = declare_parameter<std::string>("config_path", "none");
    if (map_path_uri == "none" || config_path_uri == "none") {
      RCLCPP_ERROR(get_logger(), "map_path_uri and/or config_path_uri are not provided");
      return;
    }
    const auto map_path = autoware::test_utils::resolve_pkg_share_uri(map_path_uri);
    const auto config_path = autoware::test_utils::resolve_pkg_share_uri(config_path_uri);
    if (!map_path) {
      RCLCPP_ERROR(
        get_logger(),
        "failed to resolve %s. expected form is package://<package-name>/<resource-path>",
        map_path_uri.c_str());
    } else if (!config_path) {
      RCLCPP_ERROR(
        get_logger(),
        "failed to resolve %s. expected form is package://<package-name>/<resource-path>",
        config_path_uri.c_str());
    } else {
      snap_shot_saver_ = std::make_shared<TopicSnapShotSaver>(
        map_path.value(), map_path_uri, config_path.value(), *this);
    }
  }

private:
  std::shared_ptr<TopicSnapShotSaver> snap_shot_saver_{nullptr};
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor exec;

  auto node = std::make_shared<TopicSnapShotSaverFrontEnd>();

  exec.add_node(node);

  exec.spin();

  rclcpp::shutdown();
}

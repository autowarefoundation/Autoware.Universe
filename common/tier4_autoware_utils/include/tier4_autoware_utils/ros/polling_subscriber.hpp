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

#ifndef TIER4_AUTOWARE_UTILS__ROS__POLLING_SUBSCRIBER_HPP_
#define TIER4_AUTOWARE_UTILS__ROS__POLLING_SUBSCRIBER_HPP_

#include <rclcpp/rclcpp.hpp>

#include <string>

namespace tier4_autoware_utils
{

template <typename T>
class InterProcessPollingSubscriber
{
private:
  typename rclcpp::Subscription<T>::SharedPtr subscriber_;
  std::optional<T> data_;

public:
  explicit InterProcessPollingSubscriber(
    rclcpp::Node * node, const std::string & topic_name, const rclcpp::QoS & qos = rclcpp::QoS{1})
  {
    auto noexec_callback_group =
      node->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive, false);
    auto noexec_subscription_options = rclcpp::SubscriptionOptions();
    noexec_subscription_options.callback_group = noexec_callback_group;

    subscriber_ = node->create_subscription<T>(
      topic_name, qos,
      [node]([[maybe_unused]] const typename T::ConstSharedPtr msg) { assert(false); },
      noexec_subscription_options);
    if (qos.get_rmw_qos_profile().depth > 1) {
      RCLCPP_WARN(
        node->get_logger(),
        "InterProcessPollingSubscriber will be used with depth > 1, which may cause inefficient "
        "serialization while updateLatestData()");
    }
  };
  bool updateLatestData()
  {
    rclcpp::MessageInfo message_info;
    T tmp;
    // The queue size (QoS) must be 1 to get the last message data.
    bool is_latest_message_consumed = false;
    // pop the queue until latest data
    while (subscriber_->take(tmp, message_info)) {
      is_latest_message_consumed = true;
    }
    if (is_latest_message_consumed) {
      data_ = tmp;
    }
    return data_.has_value();
  };
  const T & getData() const
  {
    if (!data_.has_value()) {
      throw std::runtime_error("Bad_optional_access in class InterProcessPollingSubscriber");
    }
    return data_.value();
  };
};

}  // namespace tier4_autoware_utils

#endif  // TIER4_AUTOWARE_UTILS__ROS__POLLING_SUBSCRIBER_HPP_

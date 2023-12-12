// Copyright 2023 Autoware Foundation
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

#ifndef POSE_ESTIMATOR_ARBITER__SUB_ARBITER__SUB_ARBITER_YABLOC_HPP_
#define POSE_ESTIMATOR_ARBITER__SUB_ARBITER__SUB_ARBITER_YABLOC_HPP_
#include "pose_estimator_arbiter/base_pose_estimator_sub_arbiter.hpp"

#include <sensor_msgs/msg/image.hpp>
#include <std_srvs/srv/set_bool.hpp>

#include <memory>

namespace pose_estimator_arbiter::sub_arbiter
{
class SubArbiterYabLoc : public BasePoseEstimatorSubArbiter
{
public:
  using Image = sensor_msgs::msg::Image;
  using SetBool = std_srvs::srv::SetBool;

  explicit SubArbiterYabLoc(
    rclcpp::Node * node, const std::shared_ptr<const SharedData> shared_data)
  : BasePoseEstimatorSubArbiter(node, shared_data)
  {
    yabloc_is_enabled_ = true;
    pub_image_ = node->create_publisher<Image>("~/output/yabloc/image", 5);

    using namespace std::literals::chrono_literals;
    service_callback_group_ =
      node->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    enable_service_client_ = node->create_client<SetBool>(
      "~/yabloc_suspend_srv", rmw_qos_profile_services_default, service_callback_group_);
    while (!enable_service_client_->wait_for_service(1s) && rclcpp::ok()) {
      RCLCPP_INFO_STREAM(node->get_logger(), "Waiting for service... ");
    }

    // Register callback
    shared_data_->yabloc_input_image.set_callback([this](Image::ConstSharedPtr msg) -> void {
      if (yabloc_is_enabled_) {
        pub_image_->publish(*msg);
      }
    });
  }

  void set_enable(bool enabled) override
  {
    if (yabloc_is_enabled_ != enabled) {
      request_service(enabled);
    }
    yabloc_is_enabled_ = enabled;
  }

protected:
  rclcpp::CallbackGroup::SharedPtr service_callback_group_;

  void request_service(bool flag)
  {
    using namespace std::literals::chrono_literals;
    auto request = std::make_shared<SetBool::Request>();
    request->data = flag;

    auto result_future = enable_service_client_->async_send_request(request);
    std::future_status status = result_future.wait_for(1000ms);
    if (status == std::future_status::ready) {
      RCLCPP_INFO_STREAM(logger_, "yabloc_arbiter dis/enableing service exited successfully");
    } else {
      RCLCPP_ERROR_STREAM(logger_, "yabloc_arbiter dis/enableing service exited unexpectedly");
    }
  }

private:
  bool yabloc_is_enabled_;
  rclcpp::Client<SetBool>::SharedPtr enable_service_client_;
  rclcpp::Publisher<Image>::SharedPtr pub_image_;
};
}  // namespace pose_estimator_arbiter::sub_arbiter
#endif  // POSE_ESTIMATOR_ARBITER__SUB_ARBITER__SUB_ARBITER_YABLOC_HPP_

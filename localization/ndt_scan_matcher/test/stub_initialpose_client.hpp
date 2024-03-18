// Copyright 2024 Autoware Foundation
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

#ifndef NDT_SCAN_MATCHER__TEST__STUB_INITIALPOSE_CLIENT_HPP_
#define NDT_SCAN_MATCHER__TEST__STUB_INITIALPOSE_CLIENT_HPP_

#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>

class StubInitialposeClient : public rclcpp::Node
{
  using AlignSrv = tier4_localization_msgs::srv::PoseWithCovarianceStamped;

public:
  StubInitialposeClient() : Node("stub_initialpose_client")
  {
    align_service_client_ = this->create_client<AlignSrv>("/ndt_align_srv");
  }

  geometry_msgs::msg::PoseWithCovarianceStamped send_initialpose(
    const geometry_msgs::msg::PoseWithCovarianceStamped & initialpose)
  {
    // wait for the service to be available
    while (!align_service_client_->wait_for_service(std::chrono::seconds(1))) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "Interrupted while waiting for the service.");
        EXIT_FAILURE;
      }
      RCLCPP_INFO(this->get_logger(), "Waiting for service...");
    }

    // send the request
    std::shared_ptr<AlignSrv::Request> request = std::make_shared<AlignSrv::Request>();
    request->pose_with_covariance = initialpose;
    auto result = align_service_client_->async_send_request(request);

    // wait for the response
    std::future_status status = result.wait_for(std::chrono::seconds(0));
    while (status != std::future_status::ready) {
      if (!rclcpp::ok()) {
        EXIT_FAILURE;
      }
      status = result.wait_for(std::chrono::seconds(1));
    }

    return result.get()->pose_with_covariance;
  }

private:
  rclcpp::Client<AlignSrv>::SharedPtr align_service_client_;
};

#endif  // NDT_SCAN_MATCHER__TEST__STUB_INITIALPOSE_CLIENT_HPP_

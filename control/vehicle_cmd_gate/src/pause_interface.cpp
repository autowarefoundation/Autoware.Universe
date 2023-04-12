// Copyright 2022 TIER IV, Inc.
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

#include "pause_interface.hpp"

namespace vehicle_cmd_gate
{

PauseInterface::PauseInterface(rclcpp::Node * node) : node_(node)
{
  const auto adaptor = component_interface_utils::NodeAdaptor(node);
  adaptor.init_srv(srv_set_pause_, this, &PauseInterface::on_pause);
  adaptor.init_pub(pub_is_paused_);
  adaptor.init_pub(pub_is_start_requested_);

  is_paused_ = false;
  is_start_requested_ = false;
  publish();
}

bool PauseInterface::is_paused()
{
  return is_paused_;
}

void PauseInterface::publish()
{
  if (prev_is_paused_ != is_paused_) {
    IsPaused::Message msg;
    msg.stamp = node_->now();
    msg.data = is_paused_;
    std::cout << "* [pub_is_paused] = " << msg.data << " @ "
              << node_->get_clock()->now().nanoseconds() / 1e6 << "ms\n";
    pub_is_paused_->publish(msg);
    prev_is_paused_ = is_paused_;
  }

  if (prev_is_start_requested_ != is_start_requested_) {
    IsStartRequested::Message msg;
    msg.stamp = node_->now();
    msg.data = is_start_requested_;
    std::cout << "* [pub_is_start_requested_] = " << msg.data << " @ "
              << node_->get_clock()->now().nanoseconds() / 1e6 << "ms\n";
    pub_is_start_requested_->publish(msg);
    prev_is_start_requested_ = is_start_requested_;
  }
}

void PauseInterface::update(const AckermannControlCommand & control)
{
  is_start_requested_ = eps < std::abs(control.longitudinal.speed);
  std::cout << "* [update] is_start_requested_ = " << is_start_requested_ << " @ "
            << node_->get_clock()->now().nanoseconds() / 1e6 << "ms\n";
}

void PauseInterface::on_pause(
  const SetPause::Service::Request::SharedPtr req, const SetPause::Service::Response::SharedPtr res)
{
  is_paused_ = req->pause;
  res->status.success = true;
  std::cout << "* [on_pause] is_paused_ = " << req->pause << " @ "
            << node_->get_clock()->now().nanoseconds() / 1e6 << "ms\n";
}

}  // namespace vehicle_cmd_gate

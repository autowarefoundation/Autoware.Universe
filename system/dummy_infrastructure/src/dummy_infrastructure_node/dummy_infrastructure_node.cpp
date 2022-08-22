// Copyright 2021 Tier IV
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

#include "dummy_infrastructure/dummy_infrastructure_node.hpp"

#include <boost/optional.hpp>

#include <memory>
#include <string>
#include <vector>

using namespace std::literals;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::placeholders::_1;

namespace dummy_infrastructure
{
namespace
{
template <class T>
bool update_param(
  const std::vector<rclcpp::Parameter> & params, const std::string & name, T & value)
{
  const auto itr = std::find_if(
    params.cbegin(), params.cend(),
    [&name](const rclcpp::Parameter & p) { return p.get_name() == name; });

  // Not found
  if (itr == params.cend()) {
    return false;
  }

  value = itr->template get_value<T>();
  return true;
}

boost::optional<InfrastructureCommand> findCommand(
  const InfrastructureCommandArray & command_array, const std::string & instrument_id,
  const bool use_first_command, const bool use_instrument_id)
{
  if (use_first_command && !command_array.commands.empty()) {
    return command_array.commands.front();
  }

  if (use_instrument_id) {
    for (const auto & command : command_array.commands) {
      if (command.id == instrument_id) {
        return command;
      }
    }
  }

  for (const auto & command : command_array.commands) {
    if (command.state >= 1) {
      return command;
    }
  }

  return {};
}
}  // namespace

DummyInfrastructureNode::DummyInfrastructureNode(const rclcpp::NodeOptions & node_options)
: Node("dummy_infrastructure", node_options)
{
  // Parameter Server
  set_param_res_ =
    this->add_on_set_parameters_callback(std::bind(&DummyInfrastructureNode::onSetParam, this, _1));

  // Parameter
  node_param_.update_rate_hz = declare_parameter<double>("update_rate_hz", 10.0);
  node_param_.use_first_command = declare_parameter<bool>("use_first_command", true);
  node_param_.use_instrument_id = declare_parameter<bool>("use_instrument_id", false);
  node_param_.instrument_id = declare_parameter<std::string>("instrument_id", "");
  node_param_.approval = declare_parameter<bool>("approval", false);
  node_param_.is_finalized = declare_parameter<bool>("is_finalized", false);

  // Subscriber
  sub_command_array_ = create_subscription<InfrastructureCommandArray>(
    "~/input/command_array", rclcpp::QoS{1},
    std::bind(&DummyInfrastructureNode::onCommandArray, this, _1));

  // Publisher
  pub_state_array_ = create_publisher<VirtualTrafficLightStateArray>("~/output/state_array", 1);

  // Timer
  const auto update_period_ns = rclcpp::Rate(node_param_.update_rate_hz).period();
  timer_ = rclcpp::create_timer(
    this, get_clock(), update_period_ns, std::bind(&DummyInfrastructureNode::onTimer, this));
}

void DummyInfrastructureNode::onCommandArray(const InfrastructureCommandArray::ConstSharedPtr msg)
{
  if (!msg->commands.empty()) {
    command_array_ = msg;
  }
}

rcl_interfaces::msg::SetParametersResult DummyInfrastructureNode::onSetParam(
  const std::vector<rclcpp::Parameter> & params)
{
  rcl_interfaces::msg::SetParametersResult result;

  try {
    // Copy to local variable
    auto p = node_param_;

    // Update params
    update_param(params, "update_rate_hz", p.update_rate_hz);
    update_param(params, "use_first_command", p.use_first_command);
    update_param(params, "use_instrument_id", p.use_instrument_id);
    update_param(params, "instrument_id", p.instrument_id);
    update_param(params, "approval", p.approval);
    update_param(params, "is_finalized", p.is_finalized);

    // Copy back to member variable
    node_param_ = p;
  } catch (const rclcpp::exceptions::InvalidParameterTypeException & e) {
    result.successful = false;
    result.reason = e.what();
    return result;
  }

  result.successful = true;
  result.reason = "success";
  return result;
}

bool DummyInfrastructureNode::isDataReady()
{
  if (!command_array_) {
    RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 1000, "waiting for command_array msg...");
    return false;
  }

  return true;
}

void DummyInfrastructureNode::onTimer()
{
  if (!isDataReady()) {
    return;
  }

  const auto command = findCommand(
    *command_array_, node_param_.instrument_id, node_param_.use_first_command,
    node_param_.use_instrument_id);

  VirtualTrafficLightState state;
  state.stamp = get_clock()->now();
  state.id = command ? command->id : node_param_.instrument_id;
  state.type = command ? command->type : "dummy_infrastructure";
  state.approval = command ? node_param_.approval : false;
  state.is_finalized = node_param_.is_finalized;

  VirtualTrafficLightStateArray state_array;
  state_array.stamp = get_clock()->now();
  state_array.states.push_back(state);
  pub_state_array_->publish(state_array);
}

}  // namespace dummy_infrastructure

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(dummy_infrastructure::DummyInfrastructureNode)

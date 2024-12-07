// Copyright 2023 Tier IV, Inc.
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

#include "scenario_simulator_v2_adapter/converter_node.hpp"

#include <regex>
#include <string>
#include <vector>

namespace
{
std::string removeInvalidTopicString(const std::string & input_string)
{
  std::regex pattern{R"([a-zA-Z0-9/_]+)"};

  std::string result;
  for (std::sregex_iterator itr(std::begin(input_string), std::end(input_string), pattern), end;
       itr != end; ++itr) {
    result += itr->str();
  }
  return result;
}
}  // namespace

namespace scenario_simulator_v2_adapter
{
MetricConverter::MetricConverter(const rclcpp::NodeOptions & node_options)
: Node("scenario_simulator_v2_adapter", node_options)
{
  using std::placeholders::_1;

  size_t sub_counter = 0;
  std::vector<std::string> metric_topic_list;
  declare_parameter<std::vector<std::string>>("metric_topic_list", std::vector<std::string>());
  get_parameter<std::vector<std::string>>("metric_topic_list", metric_topic_list);
  for (const std::string & metric_topic : metric_topic_list) {
    // std::function required with multiple arguments https://answers.ros.org/question/289207
    const std::function<void(const MetricArray::ConstSharedPtr)> fn =
      std::bind(&MetricConverter::onMetrics, this, _1, sub_counter++, metric_topic);
    metrics_sub_.push_back(create_subscription<MetricArray>(metric_topic, 1, fn));
  }
  params_pub_.resize(metrics_sub_.size());
}

void MetricConverter::onMetrics(
  const MetricArray::ConstSharedPtr metrics_msg, const size_t topic_idx,
  const std::string & base_topic_name)
{
  for (const auto & metric : metrics_msg->metric_array) {
    std::string metric_name = base_topic_name + (metric.name.empty() ? "" : "/" + metric.name);
    const auto valid_topic_name = removeInvalidTopicString(metric_name);
    getPublisher(valid_topic_name, topic_idx)->publish(createUserDefinedValue(metric));
  }
}

UserDefinedValue MetricConverter::createUserDefinedValue(const Metric & metric) const
{
  UserDefinedValue param_msg;
  param_msg.type.data = UserDefinedValueType::DOUBLE;
  param_msg.value = metric.value;
  return param_msg;
}

rclcpp::Publisher<UserDefinedValue>::SharedPtr MetricConverter::getPublisher(
  const std::string & topic_name, const size_t topic_idx)
{
  auto & pubs = params_pub_[topic_idx];
  if (pubs.count(topic_name) == 0) {
    pubs[topic_name] = create_publisher<UserDefinedValue>(topic_name, 1);
  }
  return pubs.at(topic_name);
}
}  // namespace scenario_simulator_v2_adapter

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(scenario_simulator_v2_adapter::MetricConverter)

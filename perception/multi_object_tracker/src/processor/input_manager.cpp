// Copyright 2024 Tier IV, Inc.
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

#include "multi_object_tracker/processor/input_manager.hpp"

#include <cassert>
#include <functional>

InputStream::InputStream(rclcpp::Node & node) : node_(node)
{
}

void InputStream::init(
  const std::string & input_topic, const std::string & long_name, const std::string & short_name)
{
  // Initialize parameters
  input_topic_ = input_topic;
  long_name_ = long_name;
  short_name_ = short_name;

  // Initialize queue
  objects_que_.clear();

  // Initialize latency statistics
  expected_rate_ = 10.0;
  latency_mean_ = 0.10;  // [s]
  latency_var_ = 0.0;
  interval_mean_ = 1 / expected_rate_;
  interval_var_ = 0.0;

  latest_measurement_time_ = node_.now();
  latest_message_time_ = node_.now();
}

bool InputStream::getTimestamps(
  rclcpp::Time & latest_measurement_time, rclcpp::Time & latest_message_time) const
{
  if (!is_time_initialized_) {
    return false;
  }
  latest_measurement_time = latest_measurement_time_;
  latest_message_time = latest_message_time_;
  return true;
}

void InputStream::setObjects(
  const autoware_auto_perception_msgs::msg::DetectedObjects::ConstSharedPtr msg)
{
  // // debug message
  // RCLCPP_INFO(
  //   node_.get_logger(), "InputStream::setObjects Received %s message from %s at %d.%d",
  //   long_name_.c_str(), input_topic_.c_str(), msg->header.stamp.sec, msg->header.stamp.nanosec);

  const autoware_auto_perception_msgs::msg::DetectedObjects object = *msg;
  objects_que_.push_back(object);
  if (objects_que_.size() > que_size_) {
    objects_que_.pop_front();
  }

  // RCLCPP_INFO(node_.get_logger(), "InputStream::setObjects Que size: %zu", objects_que_.size());

  // Filter parameters
  constexpr double gain = 0.05;
  const auto now = node_.now();

  // Calculate interval, Update interval statistics
  if (is_time_initialized_) {
    const double interval = (now - latest_message_time_).seconds();
    interval_mean_ = (1.0 - gain) * interval_mean_ + gain * interval;
    const double interval_delta = interval - interval_mean_;
    interval_var_ = (1.0 - gain) * interval_var_ + gain * interval_delta * interval_delta;
  }

  // Update time
  latest_message_time_ = now;
  latest_measurement_time_ = object.header.stamp;
  if (!is_time_initialized_) is_time_initialized_ = true;

  // Calculate latency
  const double latency = (latest_message_time_ - latest_measurement_time_).seconds();

  // Update latency statistics
  latency_mean_ = (1.0 - gain) * latency_mean_ + gain * latency;
  const double latency_delta = latency - latency_mean_;
  latency_var_ = (1.0 - gain) * latency_var_ + gain * latency_delta * latency_delta;
}

void InputStream::getObjectsOlderThan(
  const rclcpp::Time & object_latest_time, const rclcpp::Time & object_oldest_time,
  std::vector<autoware_auto_perception_msgs::msg::DetectedObjects> & objects)
{
  // objects.clear();

  assert(object_latest_time.nanoseconds() > object_oldest_time.nanoseconds());

  for (const auto & object : objects_que_) {
    const rclcpp::Time object_time = rclcpp::Time(object.header.stamp);

    // remove objects older than the specified duration
    if (object_time < object_oldest_time) {
      objects_que_.pop_front();
      continue;
    }

    // Add the object if the object is older than the specified latest time
    if (object_latest_time >= object_time) {
      objects.push_back(object);
      // remove the object from the queue
      objects_que_.pop_front();
    }
  }
  RCLCPP_INFO(
    node_.get_logger(), "InputStream::getObjectsOlderThan %s gives %zu objects", long_name_.c_str(),
    objects.size());
}

InputManager::InputManager(rclcpp::Node & node) : node_(node)
{
  latest_object_time_ = node_.now();
}

void InputManager::init(
  const std::vector<std::string> & input_topics, const std::vector<std::string> & long_names,
  const std::vector<std::string> & short_names)
{
  // Check input sizes
  input_size_ = input_topics.size();
  if (input_size_ == 0) {
    RCLCPP_ERROR(node_.get_logger(), "InputManager::init No input streams");
    return;
  }
  assert(input_size_ == long_names.size());
  assert(input_size_ == short_names.size());

  sub_objects_array_.resize(input_size_);

  for (size_t i = 0; i < input_size_; i++) {
    InputStream input_stream(node_);
    input_stream.init(input_topics.at(i), long_names.at(i), short_names.at(i));
    input_streams_.push_back(std::make_shared<InputStream>(input_stream));

    // Set subscription
    RCLCPP_INFO(
      node_.get_logger(), "InputManager::init Initializing %s input stream from %s",
      long_names.at(i).c_str(), input_topics.at(i).c_str());
    std::function<void(
      const autoware_auto_perception_msgs::msg::DetectedObjects::ConstSharedPtr msg)>
      func = std::bind(&InputStream::setObjects, input_streams_.at(i), std::placeholders::_1);
    sub_objects_array_.at(i) =
      node_.create_subscription<autoware_auto_perception_msgs::msg::DetectedObjects>(
        input_topics.at(i), rclcpp::QoS{1}, func);
  }

  is_initialized_ = true;
}

bool InputManager::isInputsReady() const
{
  if (!is_initialized_) {
    RCLCPP_INFO(
      node_.get_logger(), "InputManager::isMajorInputReady Input manager is not initialized");
    return false;
  }

  // Check if the major input stream (index of 0) is ready
  return input_streams_.at(0)->getObjectsCount() > 0;
}

bool InputManager::getObjects(
  const rclcpp::Time & now,
  std::vector<autoware_auto_perception_msgs::msg::DetectedObjects> & objects)
{
  if (!is_initialized_) {
    RCLCPP_INFO(node_.get_logger(), "InputManager::getObjects Input manager is not initialized");
    return false;
  }

  objects.clear();

  // ANALYSIS: Get the streams statistics
  std::string long_name, short_name;
  double latency_mean, latency_var, interval_mean, interval_var;
  rclcpp::Time latest_measurement_time, latest_message_time;
  for (const auto & input_stream : input_streams_) {
    input_stream->getNames(long_name, short_name);
    input_stream->getTimeStatistics(latency_mean, latency_var, interval_mean, interval_var);
    if (!input_stream->getTimestamps(latest_measurement_time, latest_message_time)) {
      continue;
    }
    double latency_message = (now - latest_message_time).seconds();
    double latency_measurement = (now - latest_measurement_time).seconds();
    RCLCPP_INFO(
      node_.get_logger(),
      "InputManager::getObjects %s: latency mean: %f, std: %f, interval mean: "
      "%f, std: %f, latest measurement latency: %f, latest message latency: %f",
      long_name.c_str(), latency_mean, std::sqrt(latency_var), interval_mean,
      std::sqrt(interval_var), latency_measurement, latency_message);
  }

  // Get proper latency
  constexpr double target_latency = 0.15;  // [s], measurement to tracking latency, as much as the
                                           // detector latency, the less total latency
  constexpr double acceptable_latency =
    0.35;  // [s], acceptable band from the target latency, larger than the target latency
  const rclcpp::Time object_latest_time = now - rclcpp::Duration::from_seconds(target_latency);
  rclcpp::Time object_oldest_time = now - rclcpp::Duration::from_seconds(acceptable_latency);

  // if the object_oldest_time is older than the latest object time, set it to the latest object
  // time
  object_oldest_time =
    object_oldest_time > latest_object_time_ ? object_oldest_time : latest_object_time_;

  // Get objects from all input streams
  for (const auto & input_stream : input_streams_) {
    // std::vector<autoware_auto_perception_msgs::msg::DetectedObjects> objects_tmp;
    // input_stream->getObjectsOlderThan(object_latest_time, object_oldest_time, objects_tmp);
    // objects.insert(objects.end(), objects_tmp.begin(), objects_tmp.end());
    input_stream->getObjectsOlderThan(object_latest_time, object_oldest_time, objects);
  }

  // Sort objects by timestamp
  std::sort(
    objects.begin(), objects.end(),
    [](
      const autoware_auto_perception_msgs::msg::DetectedObjects & a,
      const autoware_auto_perception_msgs::msg::DetectedObjects & b) {
      return (rclcpp::Time(a.header.stamp) - rclcpp::Time(b.header.stamp)).seconds() < 0;
    });

  // // ANALYSIS: Obtained object time range
  // if (!objects.empty()) {
  //   rclcpp::Time oldest_time(objects.front().header.stamp);
  //   rclcpp::Time latest_time(objects.back().header.stamp);
  //   RCLCPP_INFO(
  //     node_.get_logger(), "InputManager::getObjects Object time range: %f - %f",
  //     (now - latest_time).seconds(), (now - oldest_time).seconds());
  // }

  RCLCPP_INFO(
    node_.get_logger(), "InputManager::getObjects Got %zu objects from input streams",
    objects.size());

  // Update the latest object time
  if (!objects.empty()) {
    latest_object_time_ = rclcpp::Time(objects.back().header.stamp);
  }

  return !objects.empty();
}

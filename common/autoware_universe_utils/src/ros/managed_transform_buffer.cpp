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

#include "autoware/universe_utils/ros/managed_transform_buffer.hpp"

#include <pcl_ros/transforms.hpp>
#include <tf2_ros/qos.hpp>

#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <pcl_conversions/pcl_conversions.h>
#include <tf2/convert.h>
#include <tf2/exceptions.h>
#include <tf2_ros/create_timer_ros.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>

namespace autoware::universe_utils
{

ManagedTransformBuffer::ManagedTransformBuffer(rclcpp::Node * node, bool managed) : node_(node)
{
  get_transform_ = [this, managed](
                     const std::string & target_frame, const std::string & source_frame,
                     const rclcpp::Time & time,
                     const rclcpp::Duration & timeout) -> std::optional<TransformStamped> {
    if (managed) {
      return getUnknownTransform(target_frame, source_frame, time, timeout);
    }
    return getDynamicTransform(target_frame, source_frame, time, timeout);
  };

  static_tf_buffer_ = std::make_unique<TFMap>();
  tf_tree_ = std::make_unique<TreeMap>();
  tf_buffer_ = std::make_unique<tf2_ros::Buffer>(node_->get_clock());
  auto timer_interface = std::make_shared<tf2_ros::CreateTimerROS>(
    node_->get_node_base_interface(), node_->get_node_timers_interface());
  tf_buffer_->setCreateTimerInterface(timer_interface);
  tf_options_ = tf2_ros::detail::get_default_transform_listener_sub_options();
  tf_static_options_ = tf2_ros::detail::get_default_transform_listener_static_sub_options();
  cb_ = std::bind(&ManagedTransformBuffer::tfCallback, this, std::placeholders::_1, false);
  cb_static_ = std::bind(&ManagedTransformBuffer::tfCallback, this, std::placeholders::_1, true);
  std::stringstream sstream;
  sstream << "managed_tf_listener_impl_" << std::hex << reinterpret_cast<std::size_t>(this);
  options_.arguments({"--ros-args", "-r", "__node:=" + std::string(sstream.str())});
  options_.start_parameter_event_publisher(false);
  options_.start_parameter_services(false);
}

ManagedTransformBuffer::~ManagedTransformBuffer()
{
  deactivateListener();
  deactivateLocalListener();
}

template <>
std::optional<TransformStamped> ManagedTransformBuffer::getTransform<TransformStamped>(
  const std::string & target_frame, const std::string & source_frame, const rclcpp::Time & time,
  const rclcpp::Duration & timeout)
{
  return get_transform_(target_frame, source_frame, time, timeout);
}

template <>
std::optional<Eigen::Matrix4f> ManagedTransformBuffer::getTransform<Eigen::Matrix4f>(
  const std::string & target_frame, const std::string & source_frame, const rclcpp::Time & time,
  const rclcpp::Duration & timeout)
{
  auto tf = get_transform_(target_frame, source_frame, time, timeout);
  if (!tf.has_value()) {
    return std::nullopt;
  }
  Eigen::Matrix4f eigen_transform;
  pcl_ros::transformAsMatrix(tf.value(), eigen_transform);
  return std::make_optional<Eigen::Matrix4f>(eigen_transform);
}

template <>
std::optional<tf2::Transform> ManagedTransformBuffer::getTransform<tf2::Transform>(
  const std::string & target_frame, const std::string & source_frame, const rclcpp::Time & time,
  const rclcpp::Duration & timeout)
{
  auto tf = get_transform_(target_frame, source_frame, time, timeout);
  if (!tf.has_value()) {
    return std::nullopt;
  }
  tf2::Transform tf2_transform;
  tf2::fromMsg(tf.value().transform, tf2_transform);
  return std::make_optional<tf2::Transform>(tf2_transform);
}

bool ManagedTransformBuffer::transformPointcloud(
  const std::string & target_frame, const sensor_msgs::msg::PointCloud2 & cloud_in,
  sensor_msgs::msg::PointCloud2 & cloud_out, const rclcpp::Time & time,
  const rclcpp::Duration & timeout)
{
  if (
    pcl::getFieldIndex(cloud_in, "x") == -1 || pcl::getFieldIndex(cloud_in, "y") == -1 ||
    pcl::getFieldIndex(cloud_in, "z") == -1) {
    RCLCPP_ERROR(node_->get_logger(), "Input pointcloud does not have xyz fields");
    return false;
  }
  if (target_frame == cloud_in.header.frame_id) {
    cloud_out = cloud_in;
    return true;
  }
  auto eigen_transform =
    getTransform<Eigen::Matrix4f>(target_frame, cloud_in.header.frame_id, time, timeout);
  if (!eigen_transform.has_value()) {
    return false;
  }
  pcl_ros::transformPointCloud(eigen_transform.value(), cloud_in, cloud_out);
  cloud_out.header.frame_id = target_frame;
  return true;
}

void ManagedTransformBuffer::activateListener()
{
  tf_listener_ = std::make_unique<tf2_ros::TransformListener>(*tf_buffer_);
}

void ManagedTransformBuffer::activateLocalListener()
{
  managed_listener_node_ = rclcpp::Node::make_unique("_", options_);
  callback_group_ = managed_listener_node_->create_callback_group(
    rclcpp::CallbackGroupType::MutuallyExclusive, false);
  tf_options_.callback_group = callback_group_;
  tf_static_options_.callback_group = callback_group_;
  tf_sub_ = rclcpp::create_subscription<tf2_msgs::msg::TFMessage>(
    managed_listener_node_, "/tf", tf2_ros::DynamicListenerQoS(), cb_, tf_options_);
  tf_static_sub_ = rclcpp::create_subscription<tf2_msgs::msg::TFMessage>(
    managed_listener_node_, "/tf_static", tf2_ros::StaticListenerQoS(), cb_static_,
    tf_static_options_);
  executor_ = std::make_unique<rclcpp::executors::SingleThreadedExecutor>();
  executor_->add_callback_group(callback_group_, managed_listener_node_->get_node_base_interface());
  dedicated_listener_thread_ = std::make_unique<std::thread>([&]() { executor_->spin(); });
}

void ManagedTransformBuffer::deactivateListener()
{
  tf_listener_.reset();
}

void ManagedTransformBuffer::deactivateLocalListener()
{
  if (executor_) {
    executor_->cancel();
  }
  if (dedicated_listener_thread_ && dedicated_listener_thread_->joinable()) {
    dedicated_listener_thread_->join();
  }
  tf_static_sub_.reset();
  tf_sub_.reset();
  managed_listener_node_.reset();
  executor_.reset();
  dedicated_listener_thread_.reset();
}

void ManagedTransformBuffer::tfCallback(
  const tf2_msgs::msg::TFMessage::SharedPtr msg, const bool is_static)
{
  for (const auto & transform : msg->transforms) {
    tf_tree_->emplace(transform.child_frame_id, TreeNode{transform.header.frame_id, is_static});
  }
}

std::optional<TransformStamped> ManagedTransformBuffer::lookupTransform(
  const std::string & target_frame, const std::string & source_frame, const rclcpp::Time & time,
  const rclcpp::Duration & timeout) const
{
  try {
    auto tf = tf_buffer_->lookupTransform(target_frame, source_frame, time, timeout);
    return std::make_optional<TransformStamped>(tf);
  } catch (const tf2::TransformException & ex) {
    RCLCPP_ERROR(
      node_->get_logger(), "Failure to get transform from %s to %s with error: %s",
      target_frame.c_str(), source_frame.c_str(), ex.what());
    return std::nullopt;
  }
}

TraverseResult ManagedTransformBuffer::traverseTree(
  const std::string & target_frame, const std::string & source_frame,
  const rclcpp::Duration & timeout)
{
  std::atomic<bool> timeout_reached{false};

  auto traverse = [this, &timeout_reached](std::string t1, std::string t2) -> TraverseResult {
    bool only_static_requested{true};
    std::size_t depth = 0;
    auto current_frame = t1;
    while (!timeout_reached) {
      auto current_tf_tree = *tf_tree_;  // Avoid race condition (mutex would lock callbacks)
      auto frame_it = current_tf_tree.find(current_frame);
      if (frame_it == current_tf_tree.end()) {  // Not found, reset states and reverse the search
        std::swap(t1, t2);
        current_frame = t1;
        only_static_requested = true;
        depth = 0;
        continue;
      }
      only_static_requested = only_static_requested && frame_it->second.is_static;
      current_frame = frame_it->second.parent;
      if (current_frame == t2) {  // Found
        return {true, only_static_requested};
      }
      depth++;
      if (depth > tf2::BufferCore::MAX_GRAPH_DEPTH) {  // Possibly TF tree loop occurred
        RCLCPP_ERROR(
          node_->get_logger(), "Traverse depth exceeded for %s -> %s", t1.c_str(), t2.c_str());
        return {false, false};
      }
    }
    return {false, false};
  };

  std::future<TraverseResult> future =
    std::async(std::launch::async, traverse, target_frame, source_frame);
  if (
    future.wait_for(timeout.to_chrono<std::chrono::milliseconds>()) == std::future_status::ready) {
    return future.get();
  }
  timeout_reached = true;
  return {false, false};
}

std::optional<TransformStamped> ManagedTransformBuffer::getDynamicTransform(
  const std::string & target_frame, const std::string & source_frame, const rclcpp::Time & time,
  const rclcpp::Duration & timeout)
{
  if (!tf_listener_) {
    activateListener();
  }
  return lookupTransform(target_frame, source_frame, time, timeout);
}

std::optional<TransformStamped> ManagedTransformBuffer::getStaticTransform(
  const std::string & target_frame, const std::string & source_frame)
{
  auto key = std::make_pair(target_frame, source_frame);
  auto key_inv = std::make_pair(source_frame, target_frame);

  // Check if the transform is already in the buffer
  auto it = static_tf_buffer_->find(key);
  if (it != static_tf_buffer_->end()) {
    auto tf_msg = it->second;
    tf_msg.header.stamp = node_->now();
    return std::make_optional<TransformStamped>(tf_msg);
  }

  // Check if the inverse transform is already in the buffer
  auto it_inv = static_tf_buffer_->find(key_inv);
  if (it_inv != static_tf_buffer_->end()) {
    auto tf_msg = it_inv->second;
    tf2::Transform tf;
    tf2::fromMsg(tf_msg.transform, tf);
    tf2::Transform inv_tf = tf.inverse();
    TransformStamped inv_tf_msg;
    inv_tf_msg.transform = tf2::toMsg(inv_tf);
    inv_tf_msg.header.frame_id = tf_msg.child_frame_id;
    inv_tf_msg.child_frame_id = tf_msg.header.frame_id;
    inv_tf_msg.header.stamp = node_->now();
    static_tf_buffer_->emplace(key, inv_tf_msg);
    return std::make_optional<TransformStamped>(inv_tf_msg);
  }

  // Check if transform is needed
  if (target_frame == source_frame) {
    auto tf_identity = tf2::Transform::getIdentity();
    TransformStamped tf_msg;
    tf_msg.transform = tf2::toMsg(tf_identity);
    tf_msg.header.frame_id = target_frame;
    tf_msg.child_frame_id = source_frame;
    tf_msg.header.stamp = node_->now();
    static_tf_buffer_->emplace(key, tf_msg);
    return std::make_optional<TransformStamped>(tf_msg);
  }

  return std::nullopt;
}

std::optional<TransformStamped> ManagedTransformBuffer::getUnknownTransform(
  const std::string & target_frame, const std::string & source_frame, const rclcpp::Time & time,
  const rclcpp::Duration & timeout)
{
  // Try to get transform from local static buffer
  auto static_tf = getStaticTransform(target_frame, source_frame);
  if (static_tf.has_value()) {
    return static_tf;
  }

  // Initialize local TF listener and base TF listener
  activateLocalListener();
  activateListener();

  // Check local TF tree and TF buffer asynchronously
  std::future<TraverseResult> traverse_future = std::async(
    std::launch::async, &ManagedTransformBuffer::traverseTree, this, target_frame, source_frame,
    timeout);
  std::future<std::optional<TransformStamped>> tf_future = std::async(
    std::launch::async, &ManagedTransformBuffer::lookupTransform, this, target_frame, source_frame,
    time, timeout);
  auto traverse_result = traverse_future.get();
  auto tf = tf_future.get();
  deactivateLocalListener();

  // Mimic lookup transform error if TF not exists in tree or buffer
  if (!traverse_result.success || !tf.has_value()) {
    return std::nullopt;
  }

  // If TF is static, add it to the static buffer. Otherwise, switch to dynamic listener
  if (traverse_result.is_static) {
    auto key = std::make_pair(target_frame, source_frame);
    static_tf_buffer_->emplace(key, tf.value());
    deactivateListener();
  } else {
    get_transform_ = [this](
                       const std::string & target_frame, const std::string & source_frame,
                       const rclcpp::Time & time,
                       const rclcpp::Duration & timeout) -> std::optional<TransformStamped> {
      return getDynamicTransform(target_frame, source_frame, time, timeout);
    };
    RCLCPP_DEBUG(
      node_->get_logger(), "Transform %s -> %s is dynamic. Switching to dynamic listener.",
      target_frame.c_str(), source_frame.c_str());
  }

  return tf;
}

}  // namespace autoware::universe_utils

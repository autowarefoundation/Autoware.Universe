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

#include "topic_publisher.hpp"

#include <algorithm>
#include <memory>

namespace reaction_analyzer::topic_publisher
{

TopicPublisher::TopicPublisher(
  rclcpp::Node * node, std::atomic<bool> & spawn_object_cmd,
  std::optional<rclcpp::Time> & spawn_cmd_time)
: node_(node), spawn_object_cmd_(spawn_object_cmd), spawn_cmd_time_(spawn_cmd_time)

{
  // get perception_planning mode parameters
  topic_publisher_params_.path_bag_with_object =
    node_->get_parameter("topic_publisher.path_bag_with_object").as_string();
  topic_publisher_params_.path_bag_without_object =
    node_->get_parameter("topic_publisher.path_bag_without_object").as_string();
  topic_publisher_params_.pointcloud_publisher_type =
    node_->get_parameter("topic_publisher.pointcloud_publisher.pointcloud_publisher_type")
      .as_string();
  topic_publisher_params_.pointcloud_publisher_period =
    node_->get_parameter("topic_publisher.pointcloud_publisher.pointcloud_publisher_period")
      .as_double();
  topic_publisher_params_.publish_only_pointcloud_with_object =
    node_->get_parameter("topic_publisher.pointcloud_publisher.publish_only_pointcloud_with_object")
      .as_bool();

  // set pointcloud publisher type
  if (topic_publisher_params_.pointcloud_publisher_type == "sync_header_sync_publish") {
    pointcloud_publisher_type_ = PointcloudPublisherType::SYNC_HEADER_SYNC_PUBLISHER;
  } else if (topic_publisher_params_.pointcloud_publisher_type == "async_header_sync_publish") {
    pointcloud_publisher_type_ = PointcloudPublisherType::ASYNC_HEADER_SYNC_PUBLISHER;
  } else if (topic_publisher_params_.pointcloud_publisher_type == "async_publish") {
    pointcloud_publisher_type_ = PointcloudPublisherType::ASYNC_PUBLISHER;
  } else {
    RCLCPP_ERROR(node_->get_logger(), "Invalid pointcloud_publisher_type");
    rclcpp::shutdown();
    return;
  }

  // Init the publishers which will read the messages from the rosbag
  init_rosbag_publishers();
}

void TopicPublisher::pointcloud_messages_sync_publisher(const PointcloudPublisherType type)
{
  const auto current_time = node_->now();
  const bool is_object_spawned = spawn_object_cmd_;

  switch (type) {
    case PointcloudPublisherType::SYNC_HEADER_SYNC_PUBLISHER: {
      PublisherVarAccessor accessor;
      for (const auto & publisher_var_pair : lidar_pub_variable_pair_map_) {
        accessor.publish_with_current_time(
          *publisher_var_pair.second.first, current_time,
          topic_publisher_params_.publish_only_pointcloud_with_object || is_object_spawned);
        accessor.publish_with_current_time(
          *publisher_var_pair.second.second, current_time,
          topic_publisher_params_.publish_only_pointcloud_with_object || is_object_spawned);
      }
      if (is_object_spawned && !is_object_spawned_message_published) {
        is_object_spawned_message_published = true;
        mutex_.lock();
        spawn_cmd_time_ = node_->now();
        mutex_.unlock();
      }
      break;
    }
    case PointcloudPublisherType::ASYNC_HEADER_SYNC_PUBLISHER: {
      PublisherVarAccessor accessor;
      const auto period_pointcloud_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(topic_publisher_params_.pointcloud_publisher_period));
      const auto phase_dif = period_pointcloud_ns / lidar_pub_variable_pair_map_.size();

      size_t counter = 0;
      for (const auto & publisher_var_pair : lidar_pub_variable_pair_map_) {
        const auto header_time =
          current_time - std::chrono::nanoseconds(counter * phase_dif.count());
        accessor.publish_with_current_time(
          *publisher_var_pair.second.first, header_time,
          topic_publisher_params_.publish_only_pointcloud_with_object || is_object_spawned);
        accessor.publish_with_current_time(
          *publisher_var_pair.second.second, header_time,
          topic_publisher_params_.publish_only_pointcloud_with_object || is_object_spawned);
        counter++;
      }
      if (is_object_spawned && !is_object_spawned_message_published) {
        is_object_spawned_message_published = true;
        mutex_.lock();
        spawn_cmd_time_ = node_->now();
        mutex_.unlock();
      }
      break;
    }
    default:
      break;
  }
}

void TopicPublisher::pointcloud_messages_async_publisher(
  const std::pair<
    std::shared_ptr<PublisherVariables<PointCloud2>>,
    std::shared_ptr<PublisherVariables<PointCloud2>>> & lidar_output_pair_)
{
  PublisherVarAccessor accessor;
  const auto current_time = node_->now();
  const bool is_object_spawned = spawn_object_cmd_;
  accessor.publish_with_current_time(
    *lidar_output_pair_.first, current_time,
    topic_publisher_params_.publish_only_pointcloud_with_object || is_object_spawned);
  accessor.publish_with_current_time(
    *lidar_output_pair_.second, current_time,
    topic_publisher_params_.publish_only_pointcloud_with_object || is_object_spawned);

  if (is_object_spawned && !is_object_spawned_message_published) {
    is_object_spawned_message_published = true;
    mutex_.lock();
    spawn_cmd_time_ = node_->now();
    mutex_.unlock();
  }
}

void TopicPublisher::generic_message_publisher(const std::string & topic_name)
{
  PublisherVarAccessor accessor;
  const bool is_object_spawned = spawn_object_cmd_;
  const auto current_time = node_->now();
  const auto & publisher_variant = topic_publisher_map_[topic_name];

  std::visit(
    [&](const auto & var) {
      accessor.publish_with_current_time(var, current_time, is_object_spawned);
    },
    publisher_variant);
}

void TopicPublisher::reset()
{
  is_object_spawned_message_published = false;
}

void TopicPublisher::init_rosbag_publishers()
{
  auto string_to_publisher_message_type = [](const std::string & input) {
    if (input == "sensor_msgs/msg/PointCloud2") {
      return PublisherMessageType::POINTCLOUD2;
    } else if (input == "sensor_msgs/msg/CameraInfo") {
      return PublisherMessageType::CAMERA_INFO;
    } else if (input == "sensor_msgs/msg/Image") {
      return PublisherMessageType::IMAGE;
    } else if (input == "geometry_msgs/msg/PoseWithCovarianceStamped") {
      return PublisherMessageType::POSE_WITH_COVARIANCE_STAMPED;
    } else if (input == "geometry_msgs/msg/PoseStamped") {
      return PublisherMessageType::POSE_STAMPED;
    } else if (input == "nav_msgs/msg/Odometry") {
      return PublisherMessageType::ODOMETRY;
    } else if (input == "sensor_msgs/msg/Imu") {
      return PublisherMessageType::IMU;
    } else if (input == "autoware_auto_vehicle_msgs/msg/ControlModeReport") {
      return PublisherMessageType::CONTROL_MODE_REPORT;
    } else if (input == "autoware_auto_vehicle_msgs/msg/GearReport") {
      return PublisherMessageType::GEAR_REPORT;
    } else if (input == "autoware_auto_vehicle_msgs/msg/HazardLightsReport") {
      return PublisherMessageType::HAZARD_LIGHTS_REPORT;
    } else if (input == "autoware_auto_vehicle_msgs/msg/SteeringReport") {
      return PublisherMessageType::STEERING_REPORT;
    } else if (input == "autoware_auto_vehicle_msgs/msg/TurnIndicatorsReport") {
      return PublisherMessageType::TURN_INDICATORS_REPORT;
    } else if (input == "autoware_auto_vehicle_msgs/msg/VelocityReport") {
      return PublisherMessageType::VELOCITY_REPORT;
    } else {
      return PublisherMessageType::UNKNOWN;
    }
  };

  rosbag2_cpp::Reader reader;

  // read the messages without object
  {
    try {
      reader.open(topic_publisher_params_.path_bag_without_object);
    } catch (const std::exception & e) {
      RCLCPP_ERROR_STREAM(node_->get_logger(), "Error opening bag file: " << e.what());
      rclcpp::shutdown();
      return;
    }

    const auto & topics = reader.get_metadata().topics_with_message_count;

    auto getMessageTypeForTopic = [&topics, &string_to_publisher_message_type](
                                    const std::string & topicName) -> PublisherMessageType {
      auto it = std::find_if(topics.begin(), topics.end(), [&topicName](const auto & topic) {
        return topic.topic_metadata.name == topicName;
      });
      if (it != topics.end()) {
        return string_to_publisher_message_type(
          it->topic_metadata.type);  // Return the message type if found
      } else {
        return PublisherMessageType::UNKNOWN;  //
      }
    };

    // Collect timestamps for each topic to set the frequency of the publishers
    std::unordered_map<std::string, std::vector<rclcpp::Time>> timestamps_per_topic;

    while (reader.has_next()) {
      const auto bag_message = reader.read_next();

      const auto current_topic = bag_message->topic_name;

      const auto message_type = getMessageTypeForTopic(current_topic);
      if (message_type == PublisherMessageType::UNKNOWN) {
        continue;
      }

      // Record timestamp
      timestamps_per_topic[current_topic].emplace_back(bag_message->time_stamp);

      // Deserialize and store the first message as a sample
      if (timestamps_per_topic[current_topic].size() == 1) {
        set_message_to_variable_map(message_type, current_topic, *bag_message, true);
      }
    }

    // After collecting all timestamps for each topic, set frequencies of the publishers
    set_period_to_variable_map(timestamps_per_topic);

    reader.close();
  }

  // read messages with object
  {
    try {
      reader.open(topic_publisher_params_.path_bag_with_object);
    } catch (const std::exception & e) {
      RCLCPP_ERROR_STREAM(node_->get_logger(), "Error opening bag file: " << e.what());
      rclcpp::shutdown();
      return;
    }

    const auto & topics = reader.get_metadata().topics_with_message_count;

    auto getMessageTypeForTopic = [&topics, &string_to_publisher_message_type](
                                    const std::string & topicName) -> PublisherMessageType {
      auto it = std::find_if(topics.begin(), topics.end(), [&topicName](const auto & topic) {
        return topic.topic_metadata.name == topicName;
      });

      if (it != topics.end()) {
        return string_to_publisher_message_type(
          it->topic_metadata.type);  // Return the message type if found
      } else {
        return PublisherMessageType::UNKNOWN;
      }
    };

    while (reader.has_next()) {
      auto bag_message = reader.read_next();
      const auto current_topic = bag_message->topic_name;

      const auto message_type = getMessageTypeForTopic(current_topic);

      if (message_type == PublisherMessageType::UNKNOWN) {
        continue;
      }
      set_message_to_variable_map(message_type, current_topic, *bag_message, false);
    }
    reader.close();
  }

  if (set_publishers_and_timers_to_variable_map()) {
    RCLCPP_INFO(node_->get_logger(), "Publishers and timers are set correctly");
  } else {
    RCLCPP_ERROR(node_->get_logger(), "Publishers and timers are not set correctly");
    rclcpp::shutdown();
  }
}

void TopicPublisher::set_message_to_variable_map(
  const PublisherMessageType & message_type, const std::string & topic_name,
  rosbag2_storage::SerializedBagMessage & bag_message, const bool is_empty_area_message)
{
  // is_empty_area_message true for empty area messages, false for object spawned messages
  switch (message_type) {
    case PublisherMessageType::POINTCLOUD2: {
      rclcpp::Serialization<PointCloud2> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<PublisherVariables<PointCloud2>>(publisher_var)) {
          publisher_var = PublisherVariables<PointCloud2>();
        }
        std::get<PublisherVariables<PointCloud2>>(publisher_var).empty_area_message =
          std::make_shared<PointCloud2>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<PointCloud2>>(publisher_var).empty_area_message));
      } else {
        if (!std::holds_alternative<PublisherVariables<PointCloud2>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<PointCloud2>>(publisher_var).object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<PointCloud2>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::CAMERA_INFO: {
      rclcpp::Serialization<sensor_msgs::msg::CameraInfo> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<PublisherVariables<sensor_msgs::msg::CameraInfo>>(
              publisher_var)) {
          publisher_var = PublisherVariables<sensor_msgs::msg::CameraInfo>();
        }
        std::get<PublisherVariables<sensor_msgs::msg::CameraInfo>>(publisher_var)
          .empty_area_message = std::make_shared<sensor_msgs::msg::CameraInfo>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<sensor_msgs::msg::CameraInfo>>(publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<PublisherVariables<sensor_msgs::msg::CameraInfo>>(
              publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<sensor_msgs::msg::CameraInfo>>(publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<sensor_msgs::msg::CameraInfo>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::IMAGE: {
      rclcpp::Serialization<sensor_msgs::msg::Image> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<PublisherVariables<sensor_msgs::msg::Image>>(publisher_var)) {
          publisher_var = PublisherVariables<sensor_msgs::msg::Image>();
        }
        std::get<PublisherVariables<sensor_msgs::msg::Image>>(publisher_var).empty_area_message =
          std::make_shared<sensor_msgs::msg::Image>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<sensor_msgs::msg::Image>>(publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<PublisherVariables<sensor_msgs::msg::Image>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<sensor_msgs::msg::Image>>(publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<sensor_msgs::msg::Image>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::POSE_WITH_COVARIANCE_STAMPED: {
      rclcpp::Serialization<geometry_msgs::msg::PoseWithCovarianceStamped> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<geometry_msgs::msg::PoseWithCovarianceStamped>>(publisher_var)) {
          publisher_var = PublisherVariables<geometry_msgs::msg::PoseWithCovarianceStamped>();
        }
        std::get<PublisherVariables<geometry_msgs::msg::PoseWithCovarianceStamped>>(publisher_var)
          .empty_area_message = std::make_shared<geometry_msgs::msg::PoseWithCovarianceStamped>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<geometry_msgs::msg::PoseWithCovarianceStamped>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<geometry_msgs::msg::PoseWithCovarianceStamped>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<geometry_msgs::msg::PoseWithCovarianceStamped>>(publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message =
            std::make_shared<geometry_msgs::msg::PoseWithCovarianceStamped>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::POSE_STAMPED: {
      rclcpp::Serialization<geometry_msgs::msg::PoseStamped> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<PublisherVariables<geometry_msgs::msg::PoseStamped>>(
              publisher_var)) {
          publisher_var = PublisherVariables<geometry_msgs::msg::PoseStamped>();
        }
        std::get<PublisherVariables<geometry_msgs::msg::PoseStamped>>(publisher_var)
          .empty_area_message = std::make_shared<geometry_msgs::msg::PoseStamped>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<geometry_msgs::msg::PoseStamped>>(publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<PublisherVariables<geometry_msgs::msg::PoseStamped>>(
              publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<geometry_msgs::msg::PoseStamped>>(publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<geometry_msgs::msg::PoseStamped>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::ODOMETRY: {
      rclcpp::Serialization<nav_msgs::msg::Odometry> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<PublisherVariables<nav_msgs::msg::Odometry>>(publisher_var)) {
          publisher_var = PublisherVariables<nav_msgs::msg::Odometry>();
        }
        std::get<PublisherVariables<nav_msgs::msg::Odometry>>(publisher_var).empty_area_message =
          std::make_shared<nav_msgs::msg::Odometry>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<nav_msgs::msg::Odometry>>(publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<PublisherVariables<nav_msgs::msg::Odometry>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<nav_msgs::msg::Odometry>>(publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<nav_msgs::msg::Odometry>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::IMU: {
      rclcpp::Serialization<sensor_msgs::msg::Imu> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<PublisherVariables<sensor_msgs::msg::Imu>>(publisher_var)) {
          publisher_var = PublisherVariables<sensor_msgs::msg::Imu>();
        }
        std::get<PublisherVariables<sensor_msgs::msg::Imu>>(publisher_var).empty_area_message =
          std::make_shared<sensor_msgs::msg::Imu>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<sensor_msgs::msg::Imu>>(publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<PublisherVariables<sensor_msgs::msg::Imu>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<sensor_msgs::msg::Imu>>(publisher_var).object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<sensor_msgs::msg::Imu>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::CONTROL_MODE_REPORT: {
      rclcpp::Serialization<autoware_auto_vehicle_msgs::msg::ControlModeReport> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::ControlModeReport>>(
              publisher_var)) {
          publisher_var = PublisherVariables<autoware_auto_vehicle_msgs::msg::ControlModeReport>();
        }
        std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::ControlModeReport>>(
          publisher_var)
          .empty_area_message =
          std::make_shared<autoware_auto_vehicle_msgs::msg::ControlModeReport>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::ControlModeReport>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::ControlModeReport>>(
              publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::ControlModeReport>>(
            publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message =
            std::make_shared<autoware_auto_vehicle_msgs::msg::ControlModeReport>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::GEAR_REPORT: {
      rclcpp::Serialization<autoware_auto_vehicle_msgs::msg::GearReport> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::GearReport>>(publisher_var)) {
          publisher_var = PublisherVariables<autoware_auto_vehicle_msgs::msg::GearReport>();
        }
        std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::GearReport>>(publisher_var)
          .empty_area_message = std::make_shared<autoware_auto_vehicle_msgs::msg::GearReport>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::GearReport>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::GearReport>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::GearReport>>(publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message = std::make_shared<autoware_auto_vehicle_msgs::msg::GearReport>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::HAZARD_LIGHTS_REPORT: {
      rclcpp::Serialization<autoware_auto_vehicle_msgs::msg::HazardLightsReport> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::HazardLightsReport>>(
              publisher_var)) {
          publisher_var = PublisherVariables<autoware_auto_vehicle_msgs::msg::HazardLightsReport>();
        }
        std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::HazardLightsReport>>(
          publisher_var)
          .empty_area_message =
          std::make_shared<autoware_auto_vehicle_msgs::msg::HazardLightsReport>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::HazardLightsReport>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::HazardLightsReport>>(
              publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::HazardLightsReport>>(
            publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message =
            std::make_shared<autoware_auto_vehicle_msgs::msg::HazardLightsReport>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::STEERING_REPORT: {
      rclcpp::Serialization<autoware_auto_vehicle_msgs::msg::SteeringReport> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::SteeringReport>>(publisher_var)) {
          publisher_var = PublisherVariables<autoware_auto_vehicle_msgs::msg::SteeringReport>();
        }
        std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::SteeringReport>>(publisher_var)
          .empty_area_message = std::make_shared<autoware_auto_vehicle_msgs::msg::SteeringReport>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::SteeringReport>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::SteeringReport>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::SteeringReport>>(
            publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message =
            std::make_shared<autoware_auto_vehicle_msgs::msg::SteeringReport>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::TURN_INDICATORS_REPORT: {
      rclcpp::Serialization<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>>(
              publisher_var)) {
          publisher_var =
            PublisherVariables<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>();
        }
        std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>>(
          publisher_var)
          .empty_area_message =
          std::make_shared<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>>(
              publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>>(
            publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message =
            std::make_shared<autoware_auto_vehicle_msgs::msg::TurnIndicatorsReport>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    case PublisherMessageType::VELOCITY_REPORT: {
      rclcpp::Serialization<autoware_auto_vehicle_msgs::msg::VelocityReport> serialization;
      rclcpp::SerializedMessage extracted_serialized_msg(*bag_message.serialized_data);

      auto & publisher_var = topic_publisher_map_[topic_name];
      if (is_empty_area_message) {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::VelocityReport>>(publisher_var)) {
          publisher_var = PublisherVariables<autoware_auto_vehicle_msgs::msg::VelocityReport>();
        }
        std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::VelocityReport>>(publisher_var)
          .empty_area_message = std::make_shared<autoware_auto_vehicle_msgs::msg::VelocityReport>();
        serialization.deserialize_message(
          &extracted_serialized_msg,
          &(*std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::VelocityReport>>(
               publisher_var)
               .empty_area_message));
      } else {
        if (!std::holds_alternative<
              PublisherVariables<autoware_auto_vehicle_msgs::msg::VelocityReport>>(publisher_var)) {
          RCLCPP_ERROR_STREAM(
            node_->get_logger(), "Variant couldn't found in the topic named: " << topic_name);
          rclcpp::shutdown();
        }
        auto & object_spawned_message =
          std::get<PublisherVariables<autoware_auto_vehicle_msgs::msg::VelocityReport>>(
            publisher_var)
            .object_spawned_message;
        if (!object_spawned_message) {
          object_spawned_message =
            std::make_shared<autoware_auto_vehicle_msgs::msg::VelocityReport>();
          serialization.deserialize_message(&extracted_serialized_msg, &(*object_spawned_message));
        }
      }
      break;
    }
    default:
      break;
  }
}

void TopicPublisher::set_period_to_variable_map(
  const std::unordered_map<std::string, std::vector<rclcpp::Time>> & time_map)
{
  for (auto & topic_pair : time_map) {
    auto timestamps_tmp = topic_pair.second;

    // Sort the timestamps
    std::sort(timestamps_tmp.begin(), timestamps_tmp.end());

    // Then proceed with the frequency calculation
    std::string topic_name = topic_pair.first;
    if (timestamps_tmp.size() > 1) {
      int64_t total_time_diff_ns = 0;

      // Accumulate the differences in nanoseconds
      for (size_t i = 1; i < timestamps_tmp.size(); ++i) {
        total_time_diff_ns += (timestamps_tmp[i] - timestamps_tmp[i - 1]).nanoseconds();
      }

      // Conversion to std::chrono::milliseconds
      auto total_duration_ns = std::chrono::nanoseconds(total_time_diff_ns);
      auto period_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_duration_ns) /
                       (timestamps_tmp.size() - 1);

      PublisherVariablesVariant & publisherVar = topic_publisher_map_[topic_name];
      PublisherVarAccessor accessor;

      std::visit([&](auto & var) { accessor.set_period(var, period_ms); }, publisherVar);
    }
  }
}

bool TopicPublisher::set_publishers_and_timers_to_variable_map()
{
  // before create publishers and timers, check all the messages are correctly initialized with
  // their conjugate messages.
  if (check_publishers_initialized_correctly()) {
    RCLCPP_INFO(node_->get_logger(), "Messages are initialized correctly");
  } else {
    RCLCPP_ERROR(node_->get_logger(), "Messages are not initialized correctly");
    return false;
  }

  std::unordered_map<std::string, PublisherVariables<PointCloud2>>
    pointcloud_variables_map;  // temp map for pointcloud publishers

  // initialize timers and message publishers except pointcloud messages
  for (auto & [topic_name, variant] : topic_publisher_map_) {
    PublisherVarAccessor accessor;
    const auto & topic_ref = topic_name;
    const auto period_ns = std::chrono::duration<double, std::nano>(
      std::visit([&](const auto & var) { return accessor.get_period(var); }, variant));

    // Dynamically create the correct publisher type based on the topic
    std::visit(
      [&](auto & var) {
        using MessageType = typename decltype(var.empty_area_message)::element_type;

        // Check if the MessageType is PointCloud2
        if constexpr (
          std::is_same_v<MessageType, sensor_msgs::msg::PointCloud2> ||
          std::is_same_v<MessageType, sensor_msgs::msg::Image>) {
          // For PointCloud2, use rclcpp::SensorDataQoS
          var.publisher = node_->create_publisher<MessageType>(topic_ref, rclcpp::SensorDataQoS());
        } else {
          // For other message types, use the QoS setting depth of 1
          var.publisher = node_->create_publisher<MessageType>(topic_ref, rclcpp::QoS(1));
        }
      },
      variant);

    // Conditionally create the timer based on the message type, if message type is not PointCloud2
    std::visit(
      [&](auto & var) {
        using MessageType = typename decltype(var.empty_area_message)::element_type;

        if constexpr (!std::is_same_v<MessageType, sensor_msgs::msg::PointCloud2>) {
          var.timer = node_->create_wall_timer(
            period_ns, [this, topic_ref]() { this->generic_message_publisher(topic_ref); });
        } else {
          // For PointCloud2, Store the variables in a temporary map
          pointcloud_variables_map[topic_ref] = var;
        }
      },
      variant);
  }

  // Set the point cloud publisher timers
  if (pointcloud_variables_map.empty()) {
    RCLCPP_ERROR(node_->get_logger(), "No pointcloud publishers found!");
    return false;
  }

  // Arrange the PointCloud2 variables w.r.t. the lidars' name
  for (auto & [topic_name, pointcloud_variant] : pointcloud_variables_map) {
    const auto lidar_name = split(topic_name, '/').at(3);

    if (lidar_pub_variable_pair_map_.find(lidar_name) == lidar_pub_variable_pair_map_.end()) {
      lidar_pub_variable_pair_map_[lidar_name] = std::make_pair(
        std::make_shared<PublisherVariables<PointCloud2>>(pointcloud_variant), nullptr);
    } else {
      if (lidar_pub_variable_pair_map_[lidar_name].second) {
        RCLCPP_ERROR_STREAM(
          node_->get_logger(),
          "Lidar name: " << lidar_name << " is already used by another pointcloud publisher");
        return false;
      }
      lidar_pub_variable_pair_map_[lidar_name].second =
        std::make_shared<PublisherVariables<PointCloud2>>(pointcloud_variant);
    }
  }

  // Create the timer(s) to publish PointCloud2 Messages
  const auto period_pointcloud_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(topic_publisher_params_.pointcloud_publisher_period));

  if (pointcloud_publisher_type_ != PointcloudPublisherType::ASYNC_PUBLISHER) {
    // Create 1 timer to publish all PointCloud2 messages
    pointcloud_sync_publish_timer_ = node_->create_wall_timer(period_pointcloud_ns, [this]() {
      this->pointcloud_messages_sync_publisher(this->pointcloud_publisher_type_);
    });
  } else {
    // Create multiple timers which will run with a phase difference
    const auto phase_dif = period_pointcloud_ns / lidar_pub_variable_pair_map_.size();

    // Create a timer to create phase difference bw timers which will be created for each lidar
    // topics
    auto one_shot_timer = node_->create_wall_timer(phase_dif, [this, period_pointcloud_ns]() {
      for (const auto & [lidar_name, publisher_var_pair] : lidar_pub_variable_pair_map_) {
        if (
          pointcloud_publish_timers_map_.find(lidar_name) == pointcloud_publish_timers_map_.end()) {
          // Create the periodic timer
          auto periodic_timer =
            node_->create_wall_timer(period_pointcloud_ns, [this, publisher_var_pair]() {
              this->pointcloud_messages_async_publisher(publisher_var_pair);
            });
          // Store the periodic timer to keep it alive
          pointcloud_publish_timers_map_[lidar_name] = periodic_timer;
          return;
        }
      }
      // close the timer
      one_shot_timer_shared_ptr_->cancel();
    });

    one_shot_timer_shared_ptr_ = one_shot_timer;  // Store a weak pointer to the timer
  }
  return true;
}

bool TopicPublisher::check_publishers_initialized_correctly()
{
  // check messages are correctly initialized or not from rosbags
  for (const auto & [topic_name, variant] : topic_publisher_map_) {
    PublisherVarAccessor accessor;
    auto empty_area_message =
      std::visit([&](const auto & var) { return accessor.get_empty_area_message(var); }, variant);
    auto object_spawned_message = std::visit(
      [&](const auto & var) { return accessor.get_object_spawned_message(var); }, variant);

    if (!empty_area_message) {
      RCLCPP_ERROR_STREAM(
        node_->get_logger(),
        "Empty area message couldn't found in the topic named: " << topic_name);
      return false;
    } else if (!object_spawned_message) {
      RCLCPP_ERROR_STREAM(
        node_->get_logger(),
        "Object spawned message couldn't found in the topic named: " << topic_name);
      return false;
    }
  }
  return true;
}
}  // namespace reaction_analyzer::topic_publisher

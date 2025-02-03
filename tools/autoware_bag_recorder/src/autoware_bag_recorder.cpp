// Copyright 2023 TIER IV, Inc., Leo Drive Teknoloji A.Ş.
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

#include "autoware_bag_recorder.hpp"

namespace autoware_bag_recorder
{

AutowareBagRecorderNode::AutowareBagRecorderNode(
  const std::string & node_name, const rclcpp::NodeOptions & node_options)
: rclcpp::Node(node_name, node_options), node_(this)
{
  // common params declarations
  database_storage_ = declare_parameter<std::string>("common.database_storage");
  bag_path_ = declare_parameter<std::string>("common.path");
  prefix_ = declare_parameter<std::string>("common.prefix");
  minimum_acceptable_disk_space_ =
    static_cast<int>(declare_parameter<int>("common.minimum_acceptable_disk_space"));
  maximum_record_time_ = static_cast<int>(declare_parameter<int>("common.maximum_record_time"));
  maximum_bag_file_size_ =
    static_cast<double>(declare_parameter<double>("common.maximum_bag_file_size"));
  maximum_allowed_bag_storage_size_ =
    static_cast<double>(declare_parameter<double>("common.maximum_allowed_bag_storage_size"));
  number_of_maximum_bags_ =
    static_cast<int>(declare_parameter<int>("common.number_of_maximum_bags"));
  enable_only_auto_mode_recording_ =
    declare_parameter<bool>("common.enable_only_auto_mode_recording");
  disk_space_action_mode_ = declare_parameter<std::string>("common.disk_space_threshold_action");

  is_writing_ = false;
  remaining_topic_num_ = 0;

  // create gate mode subscription
  gate_mode_sub_ = create_subscription<tier4_control_msgs::msg::GateMode>(
    "/control/current_gate_mode", 1,
    std::bind(&AutowareBagRecorderNode::gate_mode_cmd_callback, this, std::placeholders::_1));

  // initialize module sections
  setup_module_sections();

  // check recording all topics in a single bag file is enabled
  setup_all_module_topics();

  // Check the files at initialization
  check_and_remove_files_at_init();

  run();
}

void AutowareBagRecorderNode::setup_module_sections()
{
  setup_single_module("raw_input_topics", raw_input_topics_, "raw_input");
  setup_single_module("other_topics", other_topics_, "other");
}

void AutowareBagRecorderNode::setup_single_module(
  const std::string & module_param, std::vector<std::string> & topics,
  const std::string & section_name)
{
  const auto topics_parameter_name = module_param + "." + section_name + "_topics";
  bool record_module_topics = declare_parameter<bool>(module_param + ".record_" + section_name);
  if (record_module_topics) {
    topics = declare_parameter<std::vector<std::string>>(topics_parameter_name);
    all_topics_.insert(all_topics_.end(), topics.begin(), topics.end());
  }
}

void AutowareBagRecorderNode::setup_all_module_topics()
{
  section_factory(all_topics_, bag_path_);
}

void AutowareBagRecorderNode::check_and_remove_files_at_init()
{
  for (auto & section : module_sections_) {
    remove_remainder_bags_in_folder(section);
  }
}

std::string AutowareBagRecorderNode::get_timestamp()
{
  char timestamp_str[100];
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::strftime(
    timestamp_str, sizeof(timestamp_str), "%Y_%m_%d-%H_%M_%S", std::localtime(&now_time_t));
  return timestamp_str;
}

void AutowareBagRecorderNode::create_bag_file(
  std::unique_ptr<rosbag2_cpp::Writer> & writer, const std::string & bag_path)
{
  if (std::filesystem::exists(bag_path)) {
    return;
  }

  writer = std::make_unique<rosbag2_cpp::Writer>();

  rosbag2_storage::StorageOptions storage_options_new;
  storage_options_new.uri = bag_path;
  storage_options_new.storage_id = database_storage_;
  writer->open(storage_options_new);
}

void AutowareBagRecorderNode::bag_file_handler(ModuleSection & section)
{
  remove_remainder_bags_in_folder(section);
  std::lock_guard<std::mutex> lock(writer_mutex_);
  const auto bag_file_path = section.folder_path + prefix_ + "_" + get_timestamp();
  create_bag_file(section.bag_writer, bag_file_path);
  section.current_bag_name = bag_file_path;
  // section.bag_names.push_back(bag_file_path);
  for (auto & topic_info : section.topic_info) {
    add_topics_to_writer(section.bag_writer, topic_info.topic_name, topic_info.topic_type);
  }
}

void AutowareBagRecorderNode::section_factory(
  const std::vector<std::string> & topics, const std::string & path)
{
  ModuleSection section;
  for (const auto & topic : topics) {
    TopicInfo topic_info = {topic, ""};
    section.topic_info.push_back(topic_info);
    section.topic_names.push_back(topic);
  }

  if (!section.topic_names.empty()) {
    section.folder_path = path;
    section.bag_writer = std::make_unique<rosbag2_cpp::Writer>();

    if (!std::filesystem::exists(path)) {
      std::filesystem::create_directories(path);
    }
    module_sections_.emplace_back(std::move(section));
  }
}

void AutowareBagRecorderNode::add_topics_to_writer(
  std::unique_ptr<rosbag2_cpp::Writer> & writer_, std::string topic_name, std::string topic_type)
{
  rosbag2_storage::TopicMetadata topic_metadata;
  topic_metadata.name = std::move(topic_name);

  topic_metadata.type = std::move(topic_type);
  topic_metadata.serialization_format = "cdr";

  writer_->create_topic(topic_metadata);
}

rclcpp::QoS AutowareBagRecorderNode::get_qos_profile_of_topic(const std::string & topic_name)
{
  auto publisher_info = this->get_publishers_info_by_topic(topic_name);
  auto subscriber_info = this->get_subscriptions_info_by_topic(topic_name);
  if (!publisher_info.empty()) {
    return publisher_info[0].qos_profile();
  }
  return subscriber_info[0].qos_profile();
}

void AutowareBagRecorderNode::generic_subscription_callback(
  const std::shared_ptr<rclcpp::SerializedMessage const> & msg, const std::string & topic_name,
  autoware_bag_recorder::ModuleSection & section)
{
  // check autoware mode is arrived or not
  if (!gate_mode_msg_ptr) {
    return;
  }

  const bool is_auto_mode = gate_mode_msg_ptr->data == tier4_control_msgs::msg::GateMode::AUTO;
  const bool should_record = !enable_only_auto_mode_recording_ || (is_auto_mode && is_writing_);

  if (should_record) {
    auto serialized_bag_msg = std::make_shared<rosbag2_storage::SerializedBagMessage>();
    serialized_bag_msg->serialized_data = std::make_shared<rcutils_uint8_array_t>();
    serialized_bag_msg->topic_name = topic_name;
    serialized_bag_msg->time_stamp = node_->now().nanoseconds();
    serialized_bag_msg->serialized_data->buffer = msg->get_rcl_serialized_message().buffer;
    serialized_bag_msg->serialized_data->buffer_length =
      msg->get_rcl_serialized_message().buffer_length;
    serialized_bag_msg->serialized_data->buffer_capacity =
      msg->get_rcl_serialized_message().buffer_capacity;
    serialized_bag_msg->serialized_data->allocator = msg->get_rcl_serialized_message().allocator;

    std::lock_guard<std::mutex> lock(writer_mutex_);
    section.bag_writer->write(serialized_bag_msg);
  }
}

void AutowareBagRecorderNode::rotate_topic_names(autoware_bag_recorder::ModuleSection & section)
{
  if (!section.topic_names.empty()) {
    std::rotate(
      section.topic_names.rbegin(), section.topic_names.rbegin() + 1, section.topic_names.rend());
  }
}

void AutowareBagRecorderNode::update_topic_info(
  autoware_bag_recorder::ModuleSection & section, const std::string & topic_name,
  const std::string & topic_type)
{
  for (auto & topic_info : section.topic_info) {
    if (topic_info.topic_name == topic_name) {
      topic_info.topic_type = topic_type;
    }
  }
}

void AutowareBagRecorderNode::handle_valid_topic(
  autoware_bag_recorder::ModuleSection & section, const std::string & topic_name,
  const std::string & topic_type)
{
  add_topics_to_writer(section.bag_writer, topic_name, topic_type);
  auto topics_interface = node_->get_node_topics_interface();
  RCLCPP_INFO(
    get_logger(), "Subscribed topic %s of type %s", topic_name.c_str(), topic_type.c_str());

  auto subscription = rclcpp::create_generic_subscription(
    topics_interface, topic_name, topic_type, get_qos_profile_of_topic(topic_name),
    [this, topic_name, &section](const std::shared_ptr<rclcpp::SerializedMessage const> & msg) {
      generic_subscription_callback(msg, topic_name, section);
    });

  subscriptions_.push_back(subscription);
  remaining_topic_num_ = remaining_topic_num_ - 1;

  update_topic_info(section, topic_name, topic_type);
  section.topic_names.erase(section.topic_names.begin());
}

void AutowareBagRecorderNode::search_topic(autoware_bag_recorder::ModuleSection & section)
{
  if (section.topic_names.empty()) {
    return;
  }

  std::string topic_name = section.topic_names.front();
  auto topic_info_map = node_->get_topic_names_and_types();
  std::string topic_type;

  for (const auto & topic_info : topic_info_map) {
    if (topic_info.first == topic_name) {
      topic_type = topic_info.second[0];
      break;
    }
  }

  if (!topic_type.empty()) {
    handle_valid_topic(section, topic_name, topic_type);
  } else {
    rotate_topic_names(section);
  }
}

double AutowareBagRecorderNode::get_root_disk_space() const
{
  std::filesystem::space_info root = std::filesystem::space(bag_path_);

  return static_cast<double>(root.available) / pow(1024.0, 3.0);  // Convert to GB
}

double AutowareBagRecorderNode::get_bag_path_directory_size(const std::filesystem::path & directory)
{
  std::uintmax_t size{0};
  if (std::filesystem::exists(directory)) {
    for (const auto & entry : std::filesystem::recursive_directory_iterator(directory)) {
      if (entry.is_regular_file() && !entry.is_symlink()) {
        size += entry.file_size();
      }
    }
    size = size / static_cast<std::uintmax_t>(pow(1024.0, 2.0));
  }
  return static_cast<double>(size);  // returns MB
}

void AutowareBagRecorderNode::check_files_in_folder(
  autoware_bag_recorder::ModuleSection & section, std::vector<std::string> & directories)
{
  for (const auto & path : std::filesystem::recursive_directory_iterator(section.folder_path)) {
    if (path.is_directory()) {
      directories.push_back(path.path().string());
    }
  }

  std::sort(
    directories.begin(), directories.end(),
    [](const std::string & a, const std::string & b) -> bool { return a < b; });
}

void AutowareBagRecorderNode::remove_remainder_bags_in_folder(
  autoware_bag_recorder::ModuleSection & section) const
{
  std::vector<std::string> directories;
  check_files_in_folder(section, directories);

  while (directories.size() >= static_cast<std::vector<int>::size_type>(number_of_maximum_bags_)) {
    std::filesystem::remove_all(directories[0]);
    directories.erase(directories.begin());
  }
}

void AutowareBagRecorderNode::free_disk_space_for_continue(
  autoware_bag_recorder::ModuleSection & section) const
{
  std::vector<std::string> directories;
  check_files_in_folder(section, directories);

  std::filesystem::remove_all(directories[0]);
  directories.erase(directories.begin());
}

void AutowareBagRecorderNode::gate_mode_cmd_callback(
  const tier4_control_msgs::msg::GateMode::ConstSharedPtr msg)
{
  gate_mode_msg_ptr = msg;  // AUTO = 1, EXTERNAL = 0
  if (gate_mode_msg_ptr->data != tier4_control_msgs::msg::GateMode::AUTO) {
    is_writing_ = false;
  }
}

void AutowareBagRecorderNode::initialize_bag_files_for_topics()
{
  for (auto & section : module_sections_) {
    const auto bag_file_path = section.folder_path + prefix_ + "_" + get_timestamp();
    create_bag_file(section.bag_writer, bag_file_path);
    section.current_bag_name = bag_file_path;
    remaining_topic_num_ = remaining_topic_num_ + static_cast<int>(section.topic_names.size());
  }
}

void AutowareBagRecorderNode::start_topic_search()
{
  std::thread([this]() {
    rclcpp::Rate rate(100);
    // if all topics are not subscribed, then continue checking
    while (remaining_topic_num_ > 0) {
      for (auto & section : module_sections_) {
        search_topic(section);
      }
      rate.sleep();
    }
  }).detach();
}

bool AutowareBagRecorderNode::is_acceptable_disk_limit_reached()
{
  return get_root_disk_space() < minimum_acceptable_disk_space_;
}

bool AutowareBagRecorderNode::is_bag_folder_limit_reached()
{
  return (
    get_bag_path_directory_size(std::filesystem::u8path(bag_path_)) >
    maximum_allowed_bag_storage_size_ * 1024);
}

void AutowareBagRecorderNode::disk_space_handler()
{
  if (disk_space_action_mode_ == "remove") {
    while (is_acceptable_disk_limit_reached() || is_bag_folder_limit_reached()) {
      for (auto & section : module_sections_) {
        free_disk_space_for_continue(section);
      }
    }
  } else {
    rclcpp::shutdown();
  }
}

void AutowareBagRecorderNode::check_disk_space()
{
  // check available disk space, if current disk space is smaller than threshold,
  // then shutdown node or free disk space
  if (static_cast<int>(get_root_disk_space()) < minimum_acceptable_disk_space_) {
    RCLCPP_WARN(
      this->get_logger(), "Available Disk Space is: %lf under the threshold.",
      get_root_disk_space());
    disk_space_handler();
  }

  if (
    get_bag_path_directory_size(std::filesystem::u8path(bag_path_)) >
    maximum_allowed_bag_storage_size_ * 1024) {
    disk_space_handler();
  }
}

void AutowareBagRecorderNode::check_record_time(
  const std::chrono::time_point<std::chrono::system_clock> & start_record_time)
{
  // check record time, if record time is exceeded then shutdown node
  if (
    (std::chrono::system_clock::now() - start_record_time) >
    std::chrono::seconds(maximum_record_time_)) {
    RCLCPP_WARN(this->get_logger(), "The maximum record time is reached.");
    rclcpp::shutdown();
  }
}

void AutowareBagRecorderNode::check_bag_size()
{
  for (auto & section : module_sections_) {
    if (
      get_bag_path_directory_size(std::filesystem::u8path(section.current_bag_name)) >
      (maximum_bag_file_size_ * 1024)) {
      bag_file_handler(section);
    }
  }
}

void AutowareBagRecorderNode::check_auto_mode()
{
  if (!gate_mode_msg_ptr) {
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000, "The current gate mode not received!");
    return;
  }

  const bool is_auto_mode = gate_mode_msg_ptr->data == tier4_control_msgs::msg::GateMode::AUTO;
  const bool should_write = enable_only_auto_mode_recording_ && !is_writing_;

  if (is_auto_mode && should_write) {
    is_writing_ = true;
    for (auto & section : module_sections_) {
      bag_file_handler(section);
    }
  }
}

void AutowareBagRecorderNode::start_status_control()
{
  std::thread([this]() {
    rclcpp::Rate rate(10);

    auto start_record_time = std::chrono::system_clock::now();

    while (rclcpp::ok()) {
      // check available disk space
      check_disk_space();

      // check record time limit
      check_record_time(start_record_time);

      // check bag size limit
      check_bag_size();

      // check autoware mode
      check_auto_mode();

      rate.sleep();
    }
  }).detach();
}

void AutowareBagRecorderNode::run()
{
  // initialize bag files and topics according to the parameters
  initialize_bag_files_for_topics();

  // starting topic searching thread
  start_topic_search();

  // starting condition checking thread
  start_status_control();
}

}  // namespace autoware_bag_recorder

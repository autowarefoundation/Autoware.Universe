// Copyright 2020 Autoware Foundation
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

/**
 * @file net_monitor.cpp
 * @brief Net monitor class
 */

#include "system_monitor/net_monitor/net_monitor.hpp"

#include "system_monitor/system_monitor_utility.hpp"
#include "system_monitor/traffic_reader/traffic_reader_common.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <algorithm>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <linux/if_link.h>
#include <linux/sockios.h>

NetMonitor::NetMonitor(const rclcpp::NodeOptions & options)
: Node("net_monitor", options),
  updater_(this),
  hostname_(),
  last_update_time_{0, 0, this->get_clock()->get_clock_type()},
  device_params_(
    declare_parameter<std::vector<std::string>>("devices", std::vector<std::string>())),
  getifaddrs_error_code_(0),
  enable_traffic_monitor_(declare_parameter<bool>("enable_traffic_monitor", true)),
  monitor_program_(declare_parameter<std::string>("monitor_program", "greengrass")),
  socket_path_(declare_parameter("socket_path", traffic_reader_service::socket_path)),
  crc_error_check_duration_(declare_parameter<int>("crc_error_check_duration", 1)),
  crc_error_count_threshold_(declare_parameter<int>("crc_error_count_threshold", 1)),
  reassembles_failed_info_(this),
  udp_rcvbuf_errors_info_(this),
  udp_sndbuf_errors_info_(this)
{
  if (monitor_program_.empty()) {
    monitor_program_ = "*";
  }

  gethostname(hostname_, sizeof(hostname_));
  updater_.setHardwareID(hostname_);
  updater_.add("Network Connection", this, &NetMonitor::check_connection);
  updater_.add("Network Usage", this, &NetMonitor::check_usage);
  updater_.add("Network Traffic", this, &NetMonitor::monitor_traffic);
  updater_.add("Network CRC Error", this, &NetMonitor::check_crc_error);
  updater_.add("IP Packet Reassembles Failed", this, &NetMonitor::check_reassembles_failed);
  updater_.add("UDP Buf Errors", this, &NetMonitor::check_udp_buf_errors);

  nl80211_.init();

  // Run I/O service processing loop
  boost::system::error_code error_code;
  io_service_.run(error_code);
  if (error_code) {
    RCLCPP_WARN(get_logger(), "Failed to run I/O service. %s", error_code.message().c_str());
  }

  // Update list of network information
  update_network_list();
  using namespace std::literals::chrono_literals;
  timer_ = rclcpp::create_timer(this, get_clock(), 1s, std::bind(&NetMonitor::on_timer, this));

  // Initialize information for `/proc/net/snmp`
  int reassembles_failed_check_duration =
    declare_parameter<int>("reassembles_failed_check_duration", 1);
  int reassembles_failed_check_count = declare_parameter<int>("reassembles_failed_check_count", 1);
  int udp_buf_errors_check_duration = declare_parameter<int>("udp_buf_errors_check_duration", 1);
  int udp_buf_errors_check_count = declare_parameter<int>("udp_buf_errors_check_count", 1);
  reassembles_failed_info_.set_check_parameters(
    reassembles_failed_check_duration, reassembles_failed_check_count);
  udp_rcvbuf_errors_info_.set_check_parameters(
    udp_buf_errors_check_duration, udp_buf_errors_check_count);
  udp_sndbuf_errors_info_.set_check_parameters(
    udp_buf_errors_check_duration, udp_buf_errors_check_count);
  reassembles_failed_info_.find_index("Ip:", "ReasmFails");
  udp_rcvbuf_errors_info_.find_index("Udp:", "RcvbufErrors");
  udp_sndbuf_errors_info_.find_index("Udp:", "SndbufErrors");

  // Send request to start nethogs
  if (enable_traffic_monitor_) {
    send_start_nethogs_request();
  } else {
    send_skip_nethogs_request();
  }
}

NetMonitor::~NetMonitor()
{
  shutdown_nl80211();
}

void NetMonitor::check_connection(diagnostic_updater::DiagnosticStatusWrapper & status)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  if (device_params_.empty()) {
    status.summary(DiagStatus::ERROR, "invalid device parameter");
    return;
  }
  if (getifaddrs_error_code_ != 0) {
    status.summary(DiagStatus::ERROR, "getifaddrs error");
    status.add("getifaddrs", strerror(getifaddrs_error_code_));
    return;
  }

  int index = 0;
  int whole_level = DiagStatus::OK;
  std::string error_message;

  for (const auto & network : network_list_) {
    if (network.is_invalid) {
      make_invalid_diagnostic_status(network, index, status, error_message);
    } else {
      status.add(fmt::format("Network {}: status", index), connection_messages_.at(DiagStatus::OK));
      status.add(fmt::format("Network {}: name", index), network.interface_name);
    }
    ++index;
  }

  // Check if specified device exists
  for (const auto & device : device_params_) {
    // Skip if device not specified
    if (device == "*") continue;

    // Check if device exists in detected networks
    const auto object = std::find_if(
      network_list_.begin(), network_list_.end(),
      [&device](const auto & network) { return network.interface_name == device; });

    if (object == network_list_.end()) {
      whole_level = std::max(whole_level, static_cast<int>(DiagStatus::WARN));
      error_message = "no such device";
      status.add(
        fmt::format("Network {}: status", index), connection_messages_.at(DiagStatus::WARN));
      status.add(fmt::format("Network {}: name", index), device);
    }

    ++index;
  }

  if (!error_message.empty()) {
    status.summary(whole_level, error_message);
  } else {
    status.summary(whole_level, connection_messages_.at(whole_level));
  }

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, status);
}

void NetMonitor::check_usage(diagnostic_updater::DiagnosticStatusWrapper & status)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  int level = DiagStatus::OK;
  int index = 0;

  for (const auto & network : network_list_) {
    // Skip if network is not supported
    if (network.is_invalid) continue;

    level = network.is_running ? DiagStatus::OK : DiagStatus::ERROR;

    status.add(fmt::format("Network {}: status", index), usage_messages_.at(level));
    status.add(fmt::format("Network {}: interface name", index), network.interface_name);
    status.addf(fmt::format("Network {}: rx_usage", index), "%.2f%%", network.rx_usage * 1e+2);
    status.addf(fmt::format("Network {}: tx_usage", index), "%.2f%%", network.tx_usage * 1e+2);
    status.addf(fmt::format("Network {}: rx_traffic", index), "%.2f MBit/s", network.rx_traffic);
    status.addf(fmt::format("Network {}: tx_traffic", index), "%.2f MBit/s", network.tx_traffic);
    status.addf(fmt::format("Network {}: capacity", index), "%.1f MBit/s", network.speed);
    status.add(fmt::format("Network {}: mtu", index), network.mtu);
    status.add(fmt::format("Network {}: rx_bytes", index), network.rx_bytes);
    status.add(fmt::format("Network {}: rx_errors", index), network.rx_errors);
    status.add(fmt::format("Network {}: tx_bytes", index), network.tx_bytes);
    status.add(fmt::format("Network {}: tx_errors", index), network.tx_errors);
    status.add(fmt::format("Network {}: collisions", index), network.collisions);

    ++index;
  }

  status.summary(DiagStatus::OK, "OK");

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, status);
}

void NetMonitor::check_crc_error(diagnostic_updater::DiagnosticStatusWrapper & status)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  int whole_level = DiagStatus::OK;
  int index = 0;
  std::string error_message;

  for (const auto & network : network_list_) {
    // Skip if network is not supported
    if (network.is_invalid) continue;

    CrcErrors & crc_errors = crc_errors_[network.interface_name];
    unsigned int unit_rx_crc_errors = 0;

    for (auto errors : crc_errors.errors_queue) {
      unit_rx_crc_errors += errors;
    }

    status.add(fmt::format("Network {}: interface name", index), network.interface_name);
    status.add(
      fmt::format("Network {}: total rx_crc_errors", index), crc_errors.last_rx_crc_errors);
    status.add(fmt::format("Network {}: rx_crc_errors per unit time", index), unit_rx_crc_errors);

    if (unit_rx_crc_errors >= crc_error_count_threshold_) {
      whole_level = std::max(whole_level, static_cast<int>(DiagStatus::WARN));
      error_message = "CRC error";
    }

    ++index;
  }

  if (!error_message.empty()) {
    status.summary(whole_level, error_message);
  } else {
    status.summary(whole_level, "OK");
  }

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, status);
}

void NetMonitor::monitor_traffic(diagnostic_updater::DiagnosticStatusWrapper & status)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  if (enable_traffic_monitor_) {
    // Get result of nethogs
    traffic_reader_service::Result result;
    get_nethogs_result(result);

    // traffic_reader result to output
    if (result.error_code != EXIT_SUCCESS) {
      status.summary(DiagStatus::ERROR, "traffic_reader error");
      status.add("error", result.output);
    } else {
      status.summary(DiagStatus::OK, "OK");

      if (result.output.empty()) {
        status.add("nethogs: result", fmt::format("No data monitored: {}", monitor_program_));
      } else {
        std::stringstream lines{result.output};
        std::string line;
        std::vector<std::string> list;
        int index = 0;
        while (std::getline(lines, line)) {
          if (line.empty()) continue;

          boost::split(list, line, boost::is_any_of("\t"), boost::token_compress_on);
          if (list.size() > 3) {
            status.add(fmt::format("nethogs {}: program", index), list[3].c_str());
            status.add(fmt::format("nethogs {}: sent (KB/s)", index), list[1].c_str());
            status.add(fmt::format("nethogs {}: received (KB/sec)", index), list[2].c_str());
          } else {
            status.add(fmt::format("nethogs {}: result", index), line);
          }
          ++index;
        }
      }
    }
  } else {
    status.summary(DiagStatus::OK, "OK");
    status.add("nethogs: result", "traffic monitor is NOT activated");
  }

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, status);
}

void NetMonitor::check_reassembles_failed(diagnostic_updater::DiagnosticStatusWrapper & status)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  uint64_t total_reassembles_failed = 0;
  uint64_t unit_reassembles_failed = 0;
  NetSnmp::Result ret =
    reassembles_failed_info_.check_metrics(total_reassembles_failed, unit_reassembles_failed);
  status.add("total packet reassembles failed", total_reassembles_failed);
  status.add("packet reassembles failed per unit time", unit_reassembles_failed);

  int whole_level = DiagStatus::OK;
  std::string error_message = "OK";
  switch (ret) {
    case NetSnmp::Result::OK:
    default:
      break;
    case NetSnmp::Result::CHECK_WARNING:
      whole_level = DiagStatus::WARN;
      error_message = "reassembles failed";
      break;
    case NetSnmp::Result::READ_ERROR:
      whole_level = DiagStatus::ERROR;
      error_message = "failed to read /proc/net/snmp";
      break;
  }

  status.summary(whole_level, error_message);

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, status);
}

void NetMonitor::check_udp_buf_errors(diagnostic_updater::DiagnosticStatusWrapper & status)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  uint64_t total_udp_rcvbuf_errors = 0;
  uint64_t unit_udp_rcvbuf_errors = 0;
  NetSnmp::Result ret_rcv =
    udp_rcvbuf_errors_info_.check_metrics(total_udp_rcvbuf_errors, unit_udp_rcvbuf_errors);
  status.add("total UDP rcv buf errors", total_udp_rcvbuf_errors);
  status.add("UDP rcv buf errors per unit time", unit_udp_rcvbuf_errors);

  uint64_t total_udp_sndbuf_errors = 0;
  uint64_t unit_udp_sndbuf_errors = 0;
  NetSnmp::Result ret_snd =
    udp_sndbuf_errors_info_.check_metrics(total_udp_sndbuf_errors, unit_udp_sndbuf_errors);
  status.add("total UDP snd buf errors", total_udp_sndbuf_errors);
  status.add("UDP snd buf errors per unit time", unit_udp_sndbuf_errors);

  int whole_level = DiagStatus::OK;
  std::string error_message = "OK";
  if (ret_rcv == NetSnmp::Result::READ_ERROR || ret_snd == NetSnmp::Result::READ_ERROR) {
    whole_level = DiagStatus::ERROR;
    error_message = "failed to read /proc/net/snmp";
  } else if (
    ret_rcv == NetSnmp::Result::CHECK_WARNING || ret_snd == NetSnmp::Result::CHECK_WARNING) {
    whole_level = DiagStatus::WARN;
    error_message = "UDP buf errors";
  }

  status.summary(whole_level, error_message);

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, status);
}

void NetMonitor::on_timer()
{
  // Update list of network information
  update_network_list();
}

void NetMonitor::shutdown_nl80211()
{
  nl80211_.shutdown();
}

void NetMonitor::make_invalid_diagnostic_status(
  const NetworkInfomation & network, int index,
  diagnostic_updater::DiagnosticStatusWrapper & status, std::string & error_message)
{
  // MTU information
  if (network.mtu_error_code != 0) {
    if (network.mtu_error_code == ENOTSUP) {
      status.add(fmt::format("Network {}: status", index), "Not Supported");
      // Assume no error, no error message
    } else {
      status.add(fmt::format("Network {}: status", index), "Error");
      error_message = "ioctl error";
    }

    status.add(fmt::format("Network {}: interface name", index), network.interface_name);
    status.add("ioctl(SIOCGIFMTU)", strerror(network.mtu_error_code));
    return;
  }

  // network capacity
  if (network.speed <= 0) {
    if (network.ethtool_error_code == ENOTSUP) {
      status.add(fmt::format("Network {}: status", index), "Not Supported");
      // Assume no error, no error message
    } else {
      status.add(fmt::format("Network {}: status", index), "Error");
      error_message = "ioctl error";
    }

    status.add(fmt::format("Network {}: interface name", index), network.interface_name);
    status.add("ioctl(SIOCETHTOOL)", strerror(network.ethtool_error_code));
  }
}

void NetMonitor::update_network_list()
{
  rclcpp::Duration duration = this->now() - last_update_time_;

  struct ifaddrs * interfaces = {};
  network_list_.clear();

  // Get network interfaces
  if (getifaddrs(&interfaces) < 0) {
    getifaddrs_error_code_ = errno;
    return;
  }

  const bool use_loopback =
    std::any_of(device_params_.begin(), device_params_.end(), [this](const std::string & device) {
      return device == loopback_interface_name_ || device == "*";
    });

  for (const auto * interface = interfaces; interface; interface = interface->ifa_next) {
    // Skip no addr
    if (!interface->ifa_addr) {
      continue;
    }
    if (!use_loopback) {
      // Skip loopback
      if (interface->ifa_flags & IFF_LOOPBACK) {
        continue;
      }
    }
    // Skip non AF_PACKET
    if (interface->ifa_addr->sa_family != AF_PACKET) {
      continue;
    }
    // Skip device not specified
    const auto object = std::find_if(
      device_params_.begin(), device_params_.end(),
      [&interface](const auto & device) { return device == "*" || device == interface->ifa_name; });
    if (object == device_params_.end()) {
      continue;
    }

    NetworkInfomation network{};
    network.interface_name = interface->ifa_name;
    network.is_running = (interface->ifa_flags & IFF_RUNNING);

    // Update network information using socket
    update_network_information_by_socket(network);

    // Update network information using routing netlink stats
    update_network_information_by_routing_netlink(network, interface->ifa_data, duration);

    network_list_.emplace_back(network);
  }

  freeifaddrs(interfaces);

  last_update_time_ = this->now();
}

void NetMonitor::update_network_information_by_socket(NetworkInfomation & network)
{
  // Get MTU information
  int fd = socket(AF_INET, SOCK_DGRAM, 0);

  // Update MTU information
  update_mtu(network, fd);

  // Update network capacity
  update_network_capacity(network, fd);

  close(fd);
}

void NetMonitor::update_mtu(NetworkInfomation & network, int socket)
{
  struct ifreq request = {};

  // NOLINTNEXTLINE [cppcoreguidelines-pro-type-union-access]
  strncpy(request.ifr_name, network.interface_name.c_str(), IFNAMSIZ - 1);
  if (ioctl(socket, SIOCGIFMTU, &request) < 0) {
    network.is_invalid = true;
    network.mtu_error_code = errno;
    return;
  }

  network.mtu = request.ifr_mtu;  // NOLINT [cppcoreguidelines-pro-type-union-access]
}

void NetMonitor::update_network_capacity(NetworkInfomation & network, int socket)
{
  struct ifreq request = {};
  struct ethtool_cmd ether_request = {};

  // NOLINTNEXTLINE [cppcoreguidelines-pro-type-union-access]
  strncpy(request.ifr_name, network.interface_name.c_str(), IFNAMSIZ - 1);
  ether_request.cmd = ETHTOOL_GSET;
  request.ifr_data = (caddr_t)&ether_request;  // NOLINT [cppcoreguidelines-pro-type-cstyle-cast]

  if (ioctl(socket, SIOCETHTOOL, &request) >= 0) {
    network.speed = ether_request.speed;
    return;
  }

  // capacity is not available for loopback
  if (network.interface_name == loopback_interface_name_) {
    network.speed = -1;
    return;
  }

  // Possibly wireless connection, get bitrate(MBit/s)
  float ret = nl80211_.getBitrate(network.interface_name.c_str());
  if (ret <= 0) {
    network.is_invalid = true;
    network.ethtool_error_code = errno;
  } else {
    network.speed = ret;
  }
}

void NetMonitor::update_network_information_by_routing_netlink(
  NetworkInfomation & network, void * data, const rclcpp::Duration & duration)
{
  auto * stats = static_cast<struct rtnl_link_stats *>(data);

  update_traffic(network, stats, duration);

  update_crc_error(network, stats);
}

void NetMonitor::update_traffic(
  NetworkInfomation & network, const struct rtnl_link_stats * stats,
  const rclcpp::Duration & duration)
{
  network.rx_bytes = stats->rx_bytes;
  network.rx_errors = stats->rx_errors;
  network.tx_bytes = stats->tx_bytes;
  network.tx_errors = stats->tx_errors;
  network.collisions = stats->collisions;

  // Calculate traffic and usage if interface is entried in bytes
  const auto bytes_entry = bytes_.find(network.interface_name);
  if (bytes_entry != bytes_.end()) {
    network.rx_traffic =
      to_mbit(stats->rx_bytes - bytes_entry->second.rx_bytes) / duration.seconds();
    network.tx_traffic =
      to_mbit(stats->tx_bytes - bytes_entry->second.tx_bytes) / duration.seconds();
    if (network.speed > 0) {
      network.rx_usage = network.rx_traffic / network.speed;
      network.tx_usage = network.tx_traffic / network.speed;
    }
  }

  bytes_[network.interface_name].rx_bytes = stats->rx_bytes;
  bytes_[network.interface_name].tx_bytes = stats->tx_bytes;
}

void NetMonitor::update_crc_error(NetworkInfomation & network, const struct rtnl_link_stats * stats)
{
  // Get the count of CRC errors
  CrcErrors & crc_errors = crc_errors_[network.interface_name];
  crc_errors.errors_queue.push_back(stats->rx_crc_errors - crc_errors.last_rx_crc_errors);
  while (crc_errors.errors_queue.size() > crc_error_check_duration_) {
    crc_errors.errors_queue.pop_front();
  }
  crc_errors.last_rx_crc_errors = stats->rx_crc_errors;
}

void NetMonitor::send_start_nethogs_request()
{
  // Connect to boot/shutdown service
  if (!connect_service()) {
    close_connection();
    return;
  }

  diagnostic_updater::DiagnosticStatusWrapper stat;
  std::vector<std::string> interface_names;

  for (const auto & network : network_list_) {
    // Skip if network is not supported
    if (network.is_invalid) continue;
    if (network.interface_name == loopback_interface_name_) continue;

    interface_names.push_back(network.interface_name);
  }

  // Send data to traffic-reader service
  send_data_with_parameters(
    traffic_reader_service::START_NETHOGS, interface_names, monitor_program_);

  // Close connection with traffic-reader service
  close_connection();
}

void NetMonitor::send_skip_nethogs_request()
{
  // Connect to boot/shutdown service
  if (!connect_service()) {
    close_connection();
    return;
  }

  // Send data to traffic-reader service
  send_data(traffic_reader_service::SKIP_NETHOGS);

  // Close connection with traffic-reader service
  close_connection();
}

void NetMonitor::get_nethogs_result(traffic_reader_service::Result & result)
{
  // Connect to traffic-reader service
  if (!connect_service()) {
    close_connection();
    return;
  }

  // Send data to traffic-reader service
  if (!send_data(traffic_reader_service::Request::GET_RESULT)) {
    close_connection();
    return;
  }

  // Receive data from traffic-reader service
  receive_data(result);

  // Close connection with traffic-reader service
  close_connection();
}

bool NetMonitor::connect_service()
{
  local::stream_protocol::endpoint endpoint(socket_path_);
  socket_ = std::make_unique<local::stream_protocol::socket>(io_service_);

  // Connect socket
  boost::system::error_code error_code;
  socket_->connect(endpoint, error_code);

  if (error_code) {
    RCLCPP_ERROR_ONCE(get_logger(), "Failed to connect socket. %s", error_code.message().c_str());
    return false;
  }

  return true;
}

bool NetMonitor::send_data(traffic_reader_service::Request request)
{
  std::ostringstream out_stream;
  boost::archive::text_oarchive archive(out_stream);
  archive & request;

  // Write data to socket
  boost::system::error_code error_code;
  socket_->write_some(
    boost::asio::buffer(out_stream.str().c_str(), out_stream.str().length()), error_code);

  if (error_code) {
    RCLCPP_ERROR(get_logger(), "Failed to write data to socket. %s", error_code.message().c_str());
    return false;
  }

  return true;
}

bool NetMonitor::send_data_with_parameters(
  traffic_reader_service::Request request, std::vector<std::string> & parameters,
  std::string & program_name)
{
  std::ostringstream out_stream;
  boost::archive::text_oarchive archive(out_stream);
  archive & request;
  archive & parameters;
  archive & program_name;

  // Write data to socket
  boost::system::error_code error_code;
  socket_->write_some(
    boost::asio::buffer(out_stream.str().c_str(), out_stream.str().length()), error_code);

  if (error_code) {
    RCLCPP_ERROR(get_logger(), "Failed to write data to socket. %s", error_code.message().c_str());
    return false;
  }

  return true;
}

void NetMonitor::receive_data(traffic_reader_service::Result & result)
{
  uint8_t request_id = traffic_reader_service::Request::NONE;

  // Read data from socket
  char buffer[10240]{};
  boost::system::error_code error_code;
  socket_->read_some(boost::asio::buffer(buffer, sizeof(buffer)), error_code);

  if (error_code) {
    RCLCPP_ERROR(
      get_logger(), "Failed to read data from socket. %s\n", error_code.message().c_str());
    return;
  }

  // Restore device status list
  try {
    std::istringstream in_stream(buffer);
    boost::archive::text_iarchive archive(in_stream);
    archive >> request_id;
    archive >> result;
  } catch (const std::exception & e) {
    RCLCPP_ERROR(get_logger(), "Failed to restore message. %s\n", e.what());
  }
}

void NetMonitor::close_connection()
{
  // Close socket
  socket_->close();
}

NetSnmp::NetSnmp(rclcpp::Node * node)
: logger_(node->get_logger().get_child("net_snmp")),
  check_duration_(1),
  check_count_(1),
  index_row_(0),
  index_col_(0),
  current_value_(0),
  last_value_(0),
  value_per_unit_time_(0),
  queue_()
{
}

void NetSnmp::set_check_parameters(unsigned int check_duration, unsigned int check_count)
{
  check_duration_ = check_duration;
  check_count_ = check_count;
}

void NetSnmp::find_index(const std::string & protocol, const std::string & metrics)
{
  // /proc/net/snmp
  // Ip: Forwarding DefaultTTL InReceives ... ReasmTimeout ReasmReqds ReasmOKs ReasmFails ...
  // Ip: 2          64         5636471397 ... 135          2303339    216166   270        ..
  std::ifstream ifs("/proc/net/snmp");
  if (!ifs) {
    RCLCPP_WARN(logger_, "Failed to open /proc/net/snmp.");
    index_row_ = index_col_ = 0;
    return;
  }

  std::vector<std::string> target_header_list;
  std::string line;
  while (std::getline(ifs, line)) {
    std::vector<std::string> header_list;
    boost::split(header_list, line, boost::is_space());
    if (header_list.empty()) continue;
    if (header_list[0] == protocol) {
      target_header_list = header_list;
      break;
    }
    ++index_row_;
  }

  ++index_row_;  // The values are placed in the row following the header

  for (const auto & header : target_header_list) {
    if (header == metrics) {
      return;
    }
    ++index_col_;
  }

  RCLCPP_WARN(logger_, "Failed to get header of /proc/net/snmp.");
  index_row_ = index_col_ = 0;
  return;
}

NetSnmp::Result NetSnmp::check_metrics(uint64_t & current_value, uint64_t & value_per_unit_time)
{
  if (!read_value_from_proc(index_row_, index_col_, current_value_)) {
    queue_.push_back(0);
    current_value = value_per_unit_time = 0;
    return Result::READ_ERROR;
  }

  if (queue_.empty()) {
    last_value_ = current_value_;
  }
  queue_.push_back(current_value_ - last_value_);
  last_value_ = current_value_;
  while (queue_.size() > check_duration_) {
    queue_.pop_front();
  }

  value_per_unit_time_ = std::accumulate(queue_.begin(), queue_.end(), static_cast<uint64_t>(0));

  current_value = current_value_;
  value_per_unit_time = value_per_unit_time_;

  if (value_per_unit_time_ >= check_count_) {
    return Result::CHECK_WARNING;
  } else {
    return Result::OK;
  }
}

bool NetSnmp::read_value_from_proc(
  unsigned int index_row, unsigned int index_col, uint64_t & output_value)
{
  if (index_row == 0 && index_col == 0) {
    RCLCPP_WARN_ONCE(logger_, "index is invalid. : %u, %u", index_row, index_col);
    return false;
  }

  std::ifstream ifs("/proc/net/snmp");
  if (!ifs) {
    RCLCPP_WARN_ONCE(logger_, "Failed to open /proc/net/snmp.");
    return false;
  }

  std::string target_line;
  std::string line;
  for (unsigned int row_index = 0; std::getline(ifs, line); ++row_index) {
    if (row_index == index_row) {
      target_line = line;
      break;
    }
  }

  if (target_line.empty()) {
    RCLCPP_WARN_ONCE(logger_, "Failed to get a line of /proc/net/snmp.");
    return false;
  }

  std::vector<std::string> value_list;
  boost::split(value_list, target_line, boost::is_space());
  if (index_col >= value_list.size()) {
    RCLCPP_WARN_ONCE(
      logger_, "There are not enough columns for the column index. : column size=%lu index=%u, %u",
      value_list.size(), index_row, index_col);
    return false;
  }

  std::string value_str = value_list[index_col];
  if (value_str.empty()) {
    RCLCPP_WARN_ONCE(logger_, "The value is empty. : index=%u, %u", index_row, index_col);
    return false;
  }

  if (value_str[0] == '-') {
    RCLCPP_WARN_ONCE(logger_, "The value is minus. : %s", value_str.c_str());
    output_value = 0;
    return false;
  } else {
    output_value = std::stoull(value_str);
    return true;
  }
}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(NetMonitor)

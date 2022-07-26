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

#include <traffic_reader/traffic_reader.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/range/algorithm.hpp>
// #include <boost/algorithm/string.hpp>   // workaround for build errors

#include <fmt/format.h>
#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <linux/if_link.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

NetMonitor::NetMonitor(const rclcpp::NodeOptions & options)
: Node("net_monitor", options),
  updater_(this),
  last_update_time_{0, 0, this->get_clock()->get_clock_type()},
  device_params_(
    declare_parameter<std::vector<std::string>>("devices", std::vector<std::string>())),
  last_reasm_fails_(0),
  monitor_program_(declare_parameter<std::string>("monitor_program", "greengrass")),
  traffic_reader_port_(declare_parameter<int>("traffic_reader_port", TRAFFIC_READER_PORT)),
  reasm_fails_check_duration_(declare_parameter<int>("ip_reasm_fails_check_duration", 1)),
  reasm_fails_count_threshold_(declare_parameter<int>("ip_reasm_fails_count_threshold", 1))
{
  if (monitor_program_.empty()) {
    monitor_program_ = GET_ALL_STR;
    nethogs_all_ = true;
  } else {
    nethogs_all_ = false;
  }

  gethostname(hostname_, sizeof(hostname_));
  updater_.setHardwareID(hostname_);
  updater_.add("Network Usage", this, &NetMonitor::checkUsage);
  updater_.add("Network Traffic", this, &NetMonitor::monitorTraffic);
  updater_.add("IP Packet Reassembles Failed", this, &NetMonitor::checkIpReasmFails);

  nl80211_.init();
}

NetMonitor::~NetMonitor() { shutdown_nl80211(); }

void NetMonitor::update() { updater_.force_update(); }

void NetMonitor::shutdown_nl80211() { nl80211_.shutdown(); }

void NetMonitor::checkUsage(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  if (device_params_.empty()) {
    stat.summary(DiagStatus::ERROR, "invalid device parameter");
    return;
  }

  const struct ifaddrs * ifa;
  struct ifaddrs * ifas = nullptr;

  rclcpp::Duration duration = this->now() - last_update_time_;

  // Get network interfaces
  if (getifaddrs(&ifas) < 0) {
    stat.summary(DiagStatus::ERROR, "getifaddrs error");
    stat.add("getifaddrs", strerror(errno));
    return;
  }

  int level = DiagStatus::OK;
  int whole_level = DiagStatus::OK;
  int index = 0;
  std::string error_str;
  float rx_traffic{0.0};
  float tx_traffic{0.0};
  float rx_usage{0.0};
  float tx_usage{0.0};
  std::vector<std::string> interface_names;

  for (ifa = ifas; ifa; ifa = ifa->ifa_next) {
    // Skip no addr
    if (!ifa->ifa_addr) {
      continue;
    }
    // Skip loopback
    if (ifa->ifa_flags & IFF_LOOPBACK) {
      continue;
    }
    // Skip non AF_PACKET
    if (ifa->ifa_addr->sa_family != AF_PACKET) {
      continue;
    }
    // Skip device not specified
    if (
      boost::find(device_params_, ifa->ifa_name) == device_params_.end() &&
      boost::find(device_params_, "*") == device_params_.end()) {
      continue;
    }

    int fd;
    struct ifreq ifrm;
    struct ifreq ifrc;
    struct ethtool_cmd edata;

    // Get MTU information
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    strncpy(ifrm.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
    if (ioctl(fd, SIOCGIFMTU, &ifrm) < 0) {
      if (errno == ENOTSUP) {
        stat.add(fmt::format("Network {}: status", index), "Not Supported");
      } else {
        stat.add(fmt::format("Network {}: status", index), "Error");
        error_str = "ioctl error";
      }

      stat.add(fmt::format("Network {}: interface name", index), ifa->ifa_name);
      stat.add("ioctl(SIOCGIFMTU)", strerror(errno));

      ++index;
      close(fd);
      interface_names.push_back(ifa->ifa_name);
      continue;
    }

    // Get network capacity
    float speed = 0.0;
    strncpy(ifrc.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
    ifrc.ifr_data = (caddr_t)&edata;
    edata.cmd = ETHTOOL_GSET;
    if (ioctl(fd, SIOCETHTOOL, &ifrc) < 0) {
      // possibly wireless connection, get bitrate(MBit/s)
      speed = nl80211_.getBitrate(ifa->ifa_name);
      if (speed <= 0) {
        if (errno == ENOTSUP) {
          stat.add(fmt::format("Network {}: status", index), "Not Supported");
        } else {
          stat.add(fmt::format("Network {}: status", index), "Error");
          error_str = "ioctl error";
        }

        stat.add(fmt::format("Network {}: interface name", index), ifa->ifa_name);
        stat.add("ioctl(SIOCETHTOOL)", strerror(errno));

        ++index;
        close(fd);
        interface_names.push_back(ifa->ifa_name);
        continue;
      }
    } else {
      speed = edata.speed;
    }

    level = (ifa->ifa_flags & IFF_RUNNING) ? DiagStatus::OK : DiagStatus::ERROR;

    auto * stats = (struct rtnl_link_stats *)ifa->ifa_data;
    if (bytes_.find(ifa->ifa_name) != bytes_.end()) {
      rx_traffic = toMbit(stats->rx_bytes - bytes_[ifa->ifa_name].rx_bytes) / duration.seconds();
      tx_traffic = toMbit(stats->tx_bytes - bytes_[ifa->ifa_name].tx_bytes) / duration.seconds();
      rx_usage = rx_traffic / speed;
      tx_usage = tx_traffic / speed;
    }

    stat.add(fmt::format("Network {}: status", index), usage_dict_.at(level));
    stat.add(fmt::format("Network {}: interface name", index), ifa->ifa_name);
    stat.addf(fmt::format("Network {}: rx_usage", index), "%.2f%%", rx_usage * 1e+2);
    stat.addf(fmt::format("Network {}: tx_usage", index), "%.2f%%", tx_usage * 1e+2);
    stat.addf(fmt::format("Network {}: rx_traffic", index), "%.2f MBit/s", rx_traffic);
    stat.addf(fmt::format("Network {}: tx_traffic", index), "%.2f MBit/s", tx_traffic);
    stat.addf(fmt::format("Network {}: capacity", index), "%.1f MBit/s", speed);
    stat.add(fmt::format("Network {}: mtu", index), ifrm.ifr_mtu);
    stat.add(fmt::format("Network {}: rx_bytes", index), stats->rx_bytes);
    stat.add(fmt::format("Network {}: rx_errors", index), stats->rx_errors);
    stat.add(fmt::format("Network {}: tx_bytes", index), stats->tx_bytes);
    stat.add(fmt::format("Network {}: tx_errors", index), stats->tx_errors);
    stat.add(fmt::format("Network {}: collisions", index), stats->collisions);

    close(fd);

    bytes_[ifa->ifa_name].rx_bytes = stats->rx_bytes;
    bytes_[ifa->ifa_name].tx_bytes = stats->tx_bytes;

    ++index;

    interface_names.push_back(ifa->ifa_name);
  }

  freeifaddrs(ifas);

  // Check if specified device exists
  for (const auto & device : device_params_) {
    // Skip if all devices specified
    if (device == "*") {
      continue;
    }
    // Skip if device already appended
    if (boost::find(interface_names, device) != interface_names.end()) {
      continue;
    }

    stat.add(fmt::format("Network {}: status", index), "No Such Device");
    stat.add(fmt::format("Network {}: interface name", index), device);
    error_str = "no such device";
    ++index;
  }

  if (!error_str.empty()) {
    stat.summary(DiagStatus::ERROR, error_str);
  } else {
    stat.summary(whole_level, usage_dict_.at(whole_level));
  }

  last_update_time_ = this->now();

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, stat);
}

#include <boost/algorithm/string.hpp>  // workaround for build errors

void NetMonitor::monitorTraffic(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  // Create a new socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    stat.summary(DiagStatus::ERROR, "socket error");
    stat.add("socket", strerror(errno));
    return;
  }

  // Specify the receiving timeouts until reporting an error
  struct timeval tv;
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  if (ret < 0) {
    stat.summary(DiagStatus::ERROR, "setsockopt error");
    stat.add("setsockopt", strerror(errno));
    close(sock);
    return;
  }

  // Connect the socket referred to by the file descriptor
  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(traffic_reader_port_);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    stat.summary(DiagStatus::ERROR, "connect error");
    stat.add("connect", strerror(errno));
    close(sock);
    return;
  }

  // Write list of devices to FD
  ret = write(sock, monitor_program_.c_str(), monitor_program_.length());
  if (ret < 0) {
    stat.summary(DiagStatus::ERROR, "write error");
    stat.add("write", strerror(errno));
    RCLCPP_ERROR(get_logger(), "write error");
    close(sock);
    return;
  }

  // Receive messages from a socket
  std::string rcv_str;
  char buf[16 * 1024 + 1];
  do {
    ret = recv(sock, buf, sizeof(buf) - 1, 0);
    if (ret < 0) {
      stat.summary(DiagStatus::ERROR, "recv error");
      stat.add("recv", strerror(errno));
      close(sock);
      return;
    }
    if (ret > 0) {
      buf[ret] = '\0';
      rcv_str += std::string(buf);
    }
  } while (ret > 0);

  // Close the file descriptor FD
  ret = close(sock);
  if (ret < 0) {
    stat.summary(DiagStatus::ERROR, "close error");
    stat.add("close", strerror(errno));
    return;
  }

  // No data received
  if (rcv_str.length() == 0) {
    stat.summary(DiagStatus::ERROR, "recv error");
    stat.add("recv", "No data received");
    return;
  }

  // Restore  information list
  TrafficReaderResult result;
  try {
    std::istringstream iss(rcv_str);
    boost::archive::text_iarchive oa(iss);
    oa >> result;
  } catch (const std::exception & e) {
    stat.summary(DiagStatus::ERROR, "recv error");
    stat.add("recv", e.what());
    return;
  }

  // traffic_reader result to output
  if (result.error_code_ != 0) {
    stat.summary(DiagStatus::ERROR, "traffic_reader error");
    stat.add("error", result.str_);
  } else {
    stat.summary(DiagStatus::OK, "OK");

    if (result.str_.length() == 0) {
      stat.add("nethogs: result", "nothing");
    } else if (nethogs_all_) {
      stat.add("nethogs: all (KB/sec):", result.str_);
    } else {
      std::stringstream lines{result.str_};
      std::string line;
      std::vector<std::string> list;
      int idx = 0;
      while (std::getline(lines, line)) {
        if (line.empty()) {
          continue;
        }
        boost::split(list, line, boost::is_any_of("\t"), boost::token_compress_on);
        if (list.size() >= 3) {
          stat.add(fmt::format("nethogs {}: PROGRAM", idx), list[0].c_str());
          stat.add(fmt::format("nethogs {}: SENT (KB/sec)", idx), list[1].c_str());
          stat.add(fmt::format("nethogs {}: RECEIVED (KB/sec)", idx), list[2].c_str());
        } else {
          stat.add(fmt::format("nethogs {}: result", idx), line);
        }
        idx++;
      }  // while
    }
  }

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, stat);
}

void NetMonitor::checkIpReasmFails(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  // Remember start time to measure elapsed time
  const auto t_start = SystemMonitorUtility::startMeasurement();

  int whole_level = DiagStatus::OK;
  std::string error_str;
  uint64_t total_reasm_fails = 0;
  uint64_t unit_reasm_fails = 0;

  if (getIpReasmFails(total_reasm_fails)) {
    reasm_fails_queue_.push_back(total_reasm_fails - last_reasm_fails_);
    while (reasm_fails_queue_.size() > reasm_fails_check_duration_) {
      reasm_fails_queue_.pop_front();
    }

    for (auto reasm_fails : reasm_fails_queue_) {
      unit_reasm_fails += reasm_fails;
    }

    stat.add(fmt::format("total packet reassembles failed"), total_reasm_fails);
    stat.add(fmt::format("packet reassembles failed per unit time"), unit_reasm_fails);

    if (unit_reasm_fails >= reasm_fails_count_threshold_) {
      whole_level = std::max(whole_level, static_cast<int>(DiagStatus::WARN));
      error_str = "reassembles failed";
    }

    last_reasm_fails_ = total_reasm_fails;
  } else {
    reasm_fails_queue_.push_back(0);
    whole_level = std::max(whole_level, static_cast<int>(DiagStatus::ERROR));
    error_str = "failed to read /proc/net/snmp";
  }

  if (!error_str.empty()) {
    stat.summary(whole_level, error_str);
  } else {
    stat.summary(whole_level, "OK");
  }

  // Measure elapsed time since start time and report
  SystemMonitorUtility::stopMeasurement(t_start, stat);
}

bool NetMonitor::getIpReasmFails(uint64_t & reasm_fails)
{
  std::ifstream ifs("/proc/net/snmp");
  if (!ifs) {
    return false;
  }

  std::string line;
  while (std::getline(ifs, line)) {
    std::vector<std::string> title_list;
    boost::split(title_list, line, boost::is_space());
    if (title_list.size() > 1 && title_list[0] == "Ip:") {
      for (std::size_t i = 1; i < title_list.size(); i++) {
        if (title_list[i] == "ReasmFails") {
          if (std::getline(ifs, line)) {
            std::vector<std::string> value_list;
            boost::split(value_list, line, boost::is_space());
            if (value_list.size() > i) {
              reasm_fails = std::stoull(value_list[i]);
              return true;
            }
          }
          break;
        }
      }
      break;
    }
  }
  return false;
}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(NetMonitor)

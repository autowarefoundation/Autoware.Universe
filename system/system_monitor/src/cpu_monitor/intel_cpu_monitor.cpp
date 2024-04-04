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
 * @file _cpu_monitor.cpp
 * @brief  CPU monitor class
 */

#include "system_monitor/cpu_monitor/intel_cpu_monitor.hpp"

#include "system_monitor/msr_reader/msr_reader.hpp"
#include "system_monitor/system_monitor_utility.hpp"

#include <tier4_autoware_utils/system/stop_watch.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/filesystem.hpp>

#include <fmt/format.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <algorithm>
#include <regex>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

CPUMonitor::CPUMonitor(const rclcpp::NodeOptions & options) : CPUMonitorBase("cpu_monitor", options)
{
  msr_reader_port_ = declare_parameter<int>("msr_reader_port", 7634);

  this->getTempNames();
  this->getFreqNames();
}

void CPUMonitor::checkThrottling(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  std::string error_str;
  std::map<int, int> map;
  double elapsed_ms;

  {
    std::lock_guard<std::mutex> lock(throt_mutex_);
    error_str = throt_error_str_;
    map = throt_map_;
    elapsed_ms = throt_elapsed_ms_;
  }

  if (!error_str.empty()) {
    stat.summary(DiagStatus::ERROR, "read throttling error");
    stat.add("read throttling", error_str);
    return;
  }

  int whole_level = DiagStatus::OK;

  for (auto itr = map.begin(); itr != map.end(); ++itr) {
    stat.add(fmt::format("CPU {}: Pkg Thermal Status", itr->first), thermal_dict_.at(itr->second));
    whole_level = std::max(whole_level, itr->second);
  }

  bool timeout_expired = false;
  {
    std::lock_guard<std::mutex> lock(throt_timeout_mutex_);
    timeout_expired = throt_timeout_expired_;
  }

  if (timeout_expired) {
    stat.summary(DiagStatus::WARN, "read throttling timeout");
  } else {
    stat.summary(whole_level, thermal_dict_.at(whole_level));
  }

  stat.addf("elapsed_time", "%f ms", elapsed_ms);
}

void CPUMonitor::onThrotTimeout()
{
  RCLCPP_WARN(get_logger(), "Read CPU Throttling Timeout occurred.");
  std::lock_guard<std::mutex> lock(throt_timeout_mutex_);
  throt_timeout_expired_ = true;
}

void CPUMonitor::executeReadThrottling()
{
  // Remember start time to measure elapsed time
  tier4_autoware_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  stop_watch.tic("execution_time");

  // Start timeout timer for reading throttling status
  {
    std::lock_guard<std::mutex> lock(throt_timeout_mutex_);
    throt_timeout_expired_ = false;
  }
  timeout_timer_ = rclcpp::create_timer(
    this, get_clock(), std::chrono::seconds(temp_timeout_),
    std::bind(&CPUMonitor::onThrotTimeout, this));

  std::string error_str;

  // Create a new socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    error_str = "socket error: " + std::string(strerror(errno));
    return;
  }

  // Specify the receiving timeouts until reporting an error
  struct timeval tv;
  tv.tv_sec = 10;
  tv.tv_usec = 0;
  int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  if (ret < 0) {
    error_str = "setsockopt error: " + std::string(strerror(errno));
    close(sock);
    return;
  }

  // Connect the socket referred to by the file descriptor
  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(msr_reader_port_);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    error_str = "connect error: " + std::string(strerror(errno));
    close(sock);
    return;
  }

  // Receive messages from a socket
  char buf[1024] = "";
  ret = recv(sock, buf, sizeof(buf) - 1, 0);
  if (ret < 0) {
    error_str = "recv error: " + std::string(strerror(errno));
    close(sock);
    return;
  }
  // No data received
  if (ret == 0) {
    error_str = "recv error: " + std::string("No data received");
    close(sock);
    return;
  }

  // Close the file descriptor FD
  ret = close(sock);
  if (ret < 0) {
    error_str = "close error: " + std::string(strerror(errno));
    return;
  }

  // Restore MSR information
  MSRInfo info;

  try {
    std::istringstream iss(buf);
    boost::archive::text_iarchive oa(iss);
    oa >> info;
  } catch (const std::exception & e) {
    error_str = "recv error: " + std::string(e.what());
    return;
  }

  // msr_reader returns an error
  if (info.error_code_ != 0) {
    error_str = "msr_reader error: " + std::string(strerror(info.error_code_));
    return;
  }

  int level = DiagStatus::OK;
  int whole_level = DiagStatus::OK;
  std::map<int, int> map;
  int index = 0;

  for (auto itr = info.pkg_thermal_status_.begin(); itr != info.pkg_thermal_status_.end();
       ++itr, ++index) {
    if (*itr) {
      level = DiagStatus::ERROR;
    } else {
      level = DiagStatus::OK;
    }

    throt_map_[index] = level;
  }

  // stop timeout timer
  timeout_timer_->cancel();

  // Measure elapsed time since start time and report
  const double elapsed_ms = stop_watch.toc("execution_time");

  {
    std::lock_guard<std::mutex> lock(throt_mutex_);
    throt_error_str_ = error_str;
    throt_map_ = map;
    throt_elapsed_ms_ = elapsed_ms;
  }
}

void CPUMonitor::getTempNames()
{
  const fs::path root("/sys/devices/platform/coretemp.0");

  if (!fs::exists(root)) {
    return;
  }

  for (const fs::path & path : boost::make_iterator_range(
         fs::recursive_directory_iterator(root), fs::recursive_directory_iterator())) {
    if (fs::is_directory(path)) {
      continue;
    }

    std::cmatch match;
    const std::string temp_input = path.generic_string();

    // /sys/devices/platform/coretemp.0/hwmon/hwmon[0-9]/temp[0-9]_input ?
    if (!std::regex_match(temp_input.c_str(), match, std::regex(".*temp(\\d+)_input"))) {
      continue;
    }

    cpu_temp_info temp;
    temp.path_ = temp_input;
    temp.label_ = path.filename().generic_string();

    std::string label = boost::algorithm::replace_all_copy(temp_input, "input", "label");
    const fs::path label_path(label);
    fs::ifstream ifs(label_path, std::ios::in);
    if (ifs) {
      std::string line;
      if (std::getline(ifs, line)) {
        temp.label_ = line;
      }
    }
    ifs.close();
    temps_.push_back(temp);
  }

  std::sort(temps_.begin(), temps_.end(), [](const cpu_temp_info & c1, const cpu_temp_info & c2) {
    std::smatch match;
    const std::regex filter(".*temp(\\d+)_input");
    int n1 = 0;
    int n2 = 0;
    if (std::regex_match(c1.path_, match, filter)) {
      n1 = std::stoi(match[1].str());
    }
    if (std::regex_match(c2.path_, match, filter)) {
      n2 = std::stoi(match[1].str());
    }
    return n1 < n2;
  });  // NOLINT
}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(CPUMonitor)

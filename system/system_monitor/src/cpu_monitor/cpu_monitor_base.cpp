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
 * @file cpu_monitor_base.cpp
 * @brief CPU monitor base class
 */

#include "system_monitor/cpu_monitor/cpu_monitor_base.hpp"

#include "system_monitor/system_monitor_utility.hpp"

#include <tier4_autoware_utils/system/stop_watch.hpp>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>

#include <fmt/format.h>

#include <algorithm>
#include <regex>
#include <string>

namespace bp = boost::process;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

CPUMonitorBase::CPUMonitorBase(const std::string & node_name, const rclcpp::NodeOptions & options)
: Node(node_name, options),
  updater_(this),
  hostname_(),
  num_cores_(0),
  temps_(),
  freqs_(),
  mpstat_exists_(false),
  usage_warn_(declare_parameter<float>("usage_warn", 0.96)),
  usage_error_(declare_parameter<float>("usage_error", 0.96)),
  usage_warn_count_(declare_parameter<int>("usage_warn_count", 1)),
  usage_error_count_(declare_parameter<int>("usage_error_count", 2)),
  usage_avg_(declare_parameter<bool>("usage_avg", true))
{
  using namespace std::literals::chrono_literals;

  gethostname(hostname_, sizeof(hostname_));
  num_cores_ = boost::thread::hardware_concurrency();
  usage_warn_check_cnt_.resize(num_cores_ + 2);   // 2 = all + dummy
  usage_error_check_cnt_.resize(num_cores_ + 2);  // 2 = all + dummy

  // Check if command exists
  fs::path p = bp::search_path("mpstat");
  mpstat_exists_ = (p.empty()) ? false : true;

  updater_.setHardwareID(hostname_);
  updater_.add("CPU Temperature", this, &CPUMonitorBase::checkTemp);
  updater_.add("CPU Usage", this, &CPUMonitorBase::checkUsage);
  updater_.add("CPU Load Average", this, &CPUMonitorBase::checkLoad);
  updater_.add("CPU Thermal Throttling", this, &CPUMonitorBase::checkThrottling);
  updater_.add("CPU Frequency", this, &CPUMonitorBase::checkFrequency);

  // Publisher
  rclcpp::QoS durable_qos{1};
  durable_qos.transient_local();
  pub_cpu_usage_ =
    this->create_publisher<tier4_external_api_msgs::msg::CpuUsage>("~/cpu_usage", durable_qos);

  // timer
  timer_callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  timer_ = rclcpp::create_timer(
    this, get_clock(), 1s, std::bind(&CPUMonitorBase::onTimer, this), timer_callback_group_);
}

void CPUMonitorBase::update()
{
  updater_.force_update();
}

void CPUMonitorBase::checkTemp(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  std::string error_str;
  std::map<std::string, float> map;
  double elapsed_ms;

  {
    std::lock_guard<std::mutex> lock(temp_mutex_);
    error_str = temp_error_str_;
    map = temp_map_;
    elapsed_ms = temp_elapsed_ms_;
  }

  int level = DiagStatus::OK;

  if (!error_str.empty()) {
    stat.summary(DiagStatus::ERROR, "read Temp error");
    stat.add("executeReadTemp()", error_str);
    return;
  }

  for (auto itr = map.begin(); itr != map.end(); ++itr) {
    stat.addf(itr->first, "%.1f DegC", itr->second);
  }

  bool timeout_expired = false;
  {
    std::lock_guard<std::mutex> lock(temp_timeout_mutex_);
    timeout_expired = temp_timeout_expired_;
  }

  if (timeout_expired) {
    stat.summary(DiagStatus::WARN, "read Temp Timeout");
  } else {
    stat.summary(level, temp_dict_.at(level));
  }

  stat.addf("elapsed_time", "%f ms", elapsed_ms);
}

void CPUMonitorBase::checkUsage(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  using CpuStatus = tier4_external_api_msgs::msg::CpuStatus;

  std::string error_str;
  std::map<std::string, CpuStatus> map;
  double elapsed_ms;

  {
    std::lock_guard<std::mutex> lock(usage_mutex_);
    error_str = usage_error_str_;
    map = usage_map_;
    elapsed_ms = usage_elapsed_ms_;
  }

  int whole_level = DiagStatus::OK;

  if (!error_str.empty()) {
    stat.summary(DiagStatus::ERROR, "read Usage error");
    stat.add("executeReadUsage()", error_str);
    return;
  }

  for (auto itr = map.begin(); itr != map.end(); ++itr) {
    stat.add(fmt::format("{}: status", itr->first), load_dict_.at(itr->second.status));
    stat.addf(fmt::format("{}: total", itr->first), "%.2f%%", itr->second.total);
    stat.addf(fmt::format("{}: usr", itr->first), "%.2f%%", itr->second.usr);
    stat.addf(fmt::format("{}: nice", itr->first), "%.2f%%", itr->second.nice);
    stat.addf(fmt::format("{}: sys", itr->first), "%.2f%%", itr->second.sys);
    stat.addf(fmt::format("{}: idle", itr->first), "%.2f%%", itr->second.idle);

    if (usage_avg_ == true) {
      if (itr->first == "cpu all") {
        whole_level = itr->second.status;
      }
    } else {
      whole_level = std::max(whole_level, static_cast<int>(itr->second.status));
    }
  }

  bool timeout_expired = false;
  {
    std::lock_guard<std::mutex> lock(usage_timeout_mutex_);
    timeout_expired = usage_timeout_expired_;
  }

  if (timeout_expired) {
    stat.summary(DiagStatus::WARN, "read Usage Timeout");
  } else {
    stat.summary(whole_level, load_dict_.at(whole_level));
  }

  stat.addf("elapsed_time", "%f ms", elapsed_ms);
}

int CPUMonitorBase::CpuUsageToLevel(const std::string & cpu_name, float usage)
{
  // cpu name to counter index
  int idx;
  try {
    int num = std::stoi(cpu_name.substr(3));
    if (num > num_cores_ || num < 0) {
      num = num_cores_;
    }
    idx = num + 1;
  } catch (std::exception &) {
    if (cpu_name == std::string("cpu")) {  // mpstat output "all"
      idx = 0;
    } else {
      idx = num_cores_ + 1;
    }
  }

  // convert CPU usage to level
  int level = DiagStatus::OK;
  if (usage >= usage_warn_) {
    if (usage_warn_check_cnt_[idx] < usage_warn_count_) {
      usage_warn_check_cnt_[idx]++;
    }
    if (usage_warn_check_cnt_[idx] >= usage_warn_count_) {
      level = DiagStatus::WARN;
    }
  } else {
    usage_warn_check_cnt_[idx] = 0;
  }
  if (usage >= usage_error_) {
    if (usage_error_check_cnt_[idx] < usage_error_count_) {
      usage_error_check_cnt_[idx]++;
    }
    if (usage_error_check_cnt_[idx] >= usage_error_count_) {
      level = DiagStatus::ERROR;
    }
  } else {
    usage_error_check_cnt_[idx] = 0;
  }

  return level;
}

void CPUMonitorBase::checkLoad(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  std::string error_str;
  double avg[3];
  double elapsed_ms;

  {
    std::lock_guard<std::mutex> lock(load_mutex_);
    error_str = load_error_str_;
    std::copy(std::begin(load_avg_), std::end(load_avg_), std::begin(avg));
    elapsed_ms = load_elapsed_ms_;
  }

  if (!error_str.empty()) {
    stat.summary(DiagStatus::ERROR, "read Load error");
    stat.add("executeReadLoad()", error_str);
    return;
  }

  bool timeout_expired = false;
  {
    std::lock_guard<std::mutex> lock(load_timeout_mutex_);
    timeout_expired = load_timeout_expired_;
  }

  if (timeout_expired) {
    stat.summary(DiagStatus::WARN, "read Load Timeout");
  } else {
    stat.summary(DiagStatus::OK, "OK");
    stat.addf("1min", "%.2f%%", avg[0] * 1e2);
    stat.addf("5min", "%.2f%%", avg[1] * 1e2);
    stat.addf("15min", "%.2f%%", avg[2] * 1e2);
  }

  stat.addf("elapsed_time", "%f ms", elapsed_ms);
}

void CPUMonitorBase::checkThrottling(
  [[maybe_unused]] diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  RCLCPP_INFO(this->get_logger(), "CPUMonitorBase::checkThrottling not implemented.");
}

void CPUMonitorBase::checkFrequency(diagnostic_updater::DiagnosticStatusWrapper & stat)
{
  std::string error_str;
  std::map<int, float> map;
  double elapsed_ms;

  {
    std::lock_guard<std::mutex> lock(freq_mutex_);
    error_str = freq_error_str_;
    map = freq_map_;
    elapsed_ms = freq_elapsed_ms_;
  }

  if (!error_str.empty()) {
    stat.summary(DiagStatus::ERROR, "read Frequency error");
    stat.add("executeReadFrequency()", error_str);
    return;
  }

  for (auto itr = map.begin(); itr != map.end(); ++itr) {
    stat.addf(fmt::format("CPU {}: clock", itr->first), "%d MHz", itr->second);
  }

  bool timeout_expired = false;
  {
    std::lock_guard<std::mutex> lock(freq_timeout_mutex_);
    timeout_expired = freq_timeout_expired_;
  }

  if (timeout_expired) {
    stat.summary(DiagStatus::WARN, "read Frequency Timeout");
  } else {
    stat.summary(DiagStatus::OK, "OK");
  }

  stat.addf("elapsed_time", "%f ms", elapsed_ms);
}

void CPUMonitorBase::onTimer()
{
  executeReadTemp();
  executeReadUsage();
  executeReadLoad();
  executeReadThrottling();
  executeReadFrequency();
}

void CPUMonitorBase::onTempTimeout()
{
  RCLCPP_WARN(get_logger(), "Read CPU Temperature Timeout occurred.");
  std::lock_guard<std::mutex> lock(temp_timeout_mutex_);
  temp_timeout_expired_ = true;
}

void CPUMonitorBase::onUsageTimeout()
{
  RCLCPP_WARN(get_logger(), "Read CPU Usage Timeout occurred.");
  std::lock_guard<std::mutex> lock(usage_timeout_mutex_);
  usage_timeout_expired_ = true;
}

void CPUMonitorBase::onLoadTimeout()
{
  RCLCPP_WARN(get_logger(), "Read CPU Load Timeout occurred.");
  std::lock_guard<std::mutex> lock(load_timeout_mutex_);
  load_timeout_expired_ = true;
}

void CPUMonitorBase::onFreqTimeout()
{
  RCLCPP_WARN(get_logger(), "Read CPU Frequency Timeout occurred.");
  std::lock_guard<std::mutex> lock(freq_timeout_mutex_);
  freq_timeout_expired_ = true;
}

void CPUMonitorBase::executeReadTemp()
{
  // Remember start time to measure elapsed time
  tier4_autoware_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  stop_watch.tic("execution_time");

  // Start timeout timer for reading temperature
  {
    std::lock_guard<std::mutex> lock(temp_timeout_mutex_);
    temp_timeout_expired_ = false;
  }
  timeout_timer_ = rclcpp::create_timer(
    this, get_clock(), std::chrono::seconds(temp_timeout_),
    std::bind(&CPUMonitorBase::onTempTimeout, this));

  std::string error_str = "";
  std::map<std::string, float> map;

  if (temps_.empty()) {
    error_str = "temperature files not found";
    return;
  }

  for (auto itr = temps_.begin(); itr != temps_.end(); ++itr) {
    // Read temperature file
    const fs::path path(itr->path_);
    fs::ifstream ifs(path, std::ios::in);
    if (!ifs) {
      error_str = "file open error";
      continue;
    }

    float temp;
    ifs >> temp;
    ifs.close();
    temp /= 1000;
    map[itr->label_] = temp;
  }
  // stop timeout timer
  timeout_timer_->cancel();

  // Measure elapsed time since start time and report
  const double elapsed_ms = stop_watch.toc("execution_time");

  {
    std::lock_guard<std::mutex> lock(temp_mutex_);
    temp_error_str_ = error_str;
    temp_map_ = map;
    temp_elapsed_ms_ = elapsed_ms;
  }
}

void CPUMonitorBase::executeReadUsage()
{
  // Remember start time to measure elapsed time
  tier4_autoware_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  stop_watch.tic("execution_time");

  // Start timeout timer for reading temperature
  {
    std::lock_guard<std::mutex> lock(temp_timeout_mutex_);
    usage_timeout_expired_ = false;
  }
  timeout_timer_ = rclcpp::create_timer(
    this, get_clock(), std::chrono::seconds(usage_timeout_),
    std::bind(&CPUMonitorBase::onUsageTimeout, this));

  tier4_external_api_msgs::msg::CpuUsage cpu_usage;
  using CpuStatus = tier4_external_api_msgs::msg::CpuStatus;

  std::ifstream ifs("/proc/stat");
  std::vector<cpu_usage_info> curr_usages;
  std::string line;
  std::string error_str = "";
  std::map<std::string, CpuStatus> map;
  int level = DiagStatus::OK;
  int whole_level = DiagStatus::OK;

  if (!ifs) {
    error_str = "ifstream: Error opening /proc/stat";
    cpu_usage.all.status = CpuStatus::STALE;
    publishCpuUsage(cpu_usage);
    return;
  }

  while (std::getline(ifs, line)) {
    if (line.compare(0, 3, "cpu") == 0) {
      std::istringstream ss(line);
      cpu_usage_info tmp_usage;
      if (!(ss >> tmp_usage.cpu_name_ >> tmp_usage.usr_ >> tmp_usage.nice_ >> tmp_usage.sys_ >>
            tmp_usage.idle_ >> tmp_usage.iowait_ >> tmp_usage.irq_ >> tmp_usage.soft_ >>
            tmp_usage.steal_)) {
        error_str = "istringstream: Error parsing line in /proc/stat: " + line;
        cpu_usage.all.status = CpuStatus::STALE;
        publishCpuUsage(cpu_usage);
        return;
      }
      curr_usages.push_back(tmp_usage);
    } else {
      break;
    }
  }

  if (prev_usages_.empty()) {
    prev_usages_ = curr_usages;
    return;
  }

  if (prev_usages_[0].usr_ <= 0) {
    error_str =
      "cpu usages overflow error: A value of /proc/stat is greater than INT_MAX. Restart ECU";
    cpu_usage.all.status = CpuStatus::STALE;
    publishCpuUsage(cpu_usage);
    return;
  }

  if (prev_usages_.size() != curr_usages.size()) {
    error_str = "update error: Error update cpu usage info from /proc/stat";
    cpu_usage.all.status = CpuStatus::STALE;
    publishCpuUsage(cpu_usage);
    return;
  }

  for (int i = 0; i < prev_usages_.size(); i++) {
    CpuStatus cpu_status;
    if (curr_usages[i].cpu_name_ == "cpu") curr_usages[i].cpu_name_ = "cpu all";
    int total_diff = curr_usages[i].totalTime() - prev_usages_[i].totalTime();
    cpu_status.usr = 100.0 * (curr_usages[i].usr_ - prev_usages_[i].usr_) / total_diff;
    cpu_status.nice = 100.0 * (curr_usages[i].nice_ - prev_usages_[i].nice_) / total_diff;
    cpu_status.sys = 100.0 * (curr_usages[i].sys_ - prev_usages_[i].sys_) / total_diff;
    cpu_status.idle = 100.0 * (curr_usages[i].idle_ - prev_usages_[i].idle_) / total_diff;
    cpu_status.total =
      100.0 * (curr_usages[i].totalActiveTime() - prev_usages_[i].totalActiveTime()) / total_diff;
    cpu_status.status = CpuUsageToLevel(curr_usages[i].cpu_name_, cpu_status.total * 1e-2);
    map[curr_usages[i].cpu_name_] = cpu_status;

    if (curr_usages[i].cpu_name_ == "cpu all") {
      cpu_usage.all = cpu_status;
    } else {
      cpu_usage.cpus.push_back(cpu_status);
    }
  }

  // Publish msg
  publishCpuUsage(cpu_usage);

  // stop timeout timer
  timeout_timer_->cancel();

  // Measure elapsed time since start time and report
  const double elapsed_ms = stop_watch.toc("execution_time");

  {
    std::lock_guard<std::mutex> lock(usage_mutex_);
    usage_error_str_ = error_str;
    usage_map_ = map;
    usage_elapsed_ms_ = elapsed_ms;
  }
}

void CPUMonitorBase::executeReadLoad()
{
  // Remember start time to measure elapsed time
  tier4_autoware_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  stop_watch.tic("execution_time");

  // Start timeout timer for reading temperature
  {
    std::lock_guard<std::mutex> lock(load_timeout_mutex_);
    load_timeout_expired_ = false;
  }
  timeout_timer_ = rclcpp::create_timer(
    this, get_clock(), std::chrono::seconds(load_timeout_),
    std::bind(&CPUMonitorBase::onLoadTimeout, this));

  double avg[3];
  std::string error_str = "";

  std::ifstream ifs("/proc/loadavg", std::ios::in);

  if (!ifs) {
    error_str = "ifstream: Error opening /proc/loadavg";
    return;
  }

  std::string line;

  if (!std::getline(ifs, line)) {
    error_str = "getline: Error reading /proc/loadavg";
    return;
  }

  if (sscanf(line.c_str(), "%lf %lf %lf", &avg[0], &avg[1], &avg[2]) != 3) {
    error_str = "sscanf: Error parsing /proc/loadavg";
    return;
  }

  avg[0] /= num_cores_;
  avg[1] /= num_cores_;
  avg[2] /= num_cores_;

  // stop timeout timer
  timeout_timer_->cancel();

  // Measure elapsed time since start time and report
  const double elapsed_ms = stop_watch.toc("execution_time");

  {
    std::lock_guard<std::mutex> lock(load_mutex_);
    load_error_str_ = error_str;
    std::copy(std::begin(avg), std::end(avg), std::begin(load_avg_));
    load_elapsed_ms_ = elapsed_ms;
  }
}

void CPUMonitorBase::executeReadThrottling()
{
  // override this function in derived class
}

void CPUMonitorBase::executeReadFrequency()
{
  // Remember start time to measure elapsed time
  tier4_autoware_utils::StopWatch<std::chrono::milliseconds> stop_watch;
  stop_watch.tic("execution_time");

  // Start timeout timer for reading temperature
  {
    std::lock_guard<std::mutex> lock(freq_timeout_mutex_);
    freq_timeout_expired_ = false;
  }
  timeout_timer_ = rclcpp::create_timer(
    this, get_clock(), std::chrono::seconds(freq_timeout_),
    std::bind(&CPUMonitorBase::onFreqTimeout, this));

  std::string error_str = "";
  std::map<int, float> map;

  if (freqs_.empty()) {
    error_str = "frequency files not found";
    return;
  }

  for (auto itr = freqs_.begin(); itr != freqs_.end(); ++itr) {
    // Read scaling_cur_freq file
    const fs::path path(itr->path_);
    fs::ifstream ifs(path, std::ios::in);
    if (ifs) {
      std::string line;
      if (std::getline(ifs, line)) {
        map[itr->index_] = std::stoi(line) / 1000;
      }
    } else {
      error_str = "file open error";
    }
    ifs.close();
  }

  // stop timeout timer
  timeout_timer_->cancel();

  // Measure elapsed time since start time and report
  const double elapsed_ms = stop_watch.toc("execution_time");

  {
    std::lock_guard<std::mutex> lock(freq_mutex_);
    freq_error_str_ = error_str;
    freq_map_ = map;
    freq_elapsed_ms_ = elapsed_ms;
  }
}

void CPUMonitorBase::getTempNames()
{
  RCLCPP_INFO(this->get_logger(), "CPUMonitorBase::getTempNames not implemented.");
}

void CPUMonitorBase::getFreqNames()
{
  const fs::path root("/sys/devices/system/cpu");

  for (const fs::path & path :
       boost::make_iterator_range(fs::directory_iterator(root), fs::directory_iterator())) {
    if (!fs::is_directory(path)) {
      continue;
    }

    std::cmatch match;
    const char * cpu_dir = path.generic_string().c_str();

    // /sys/devices/system/cpu[0-9] ?
    if (!std::regex_match(cpu_dir, match, std::regex(".*cpu(\\d+)"))) {
      continue;
    }

    // /sys/devices/system/cpu[0-9]/cpufreq/scaling_cur_freq
    cpu_freq_info freq;
    const fs::path freq_path = path / "cpufreq/scaling_cur_freq";
    freq.index_ = std::stoi(match[1].str());
    freq.path_ = freq_path.generic_string();
    freqs_.push_back(freq);
  }

  std::sort(freqs_.begin(), freqs_.end(), [](const cpu_freq_info & c1, const cpu_freq_info & c2) {
    return c1.index_ < c2.index_;
  });  // NOLINT
}

void CPUMonitorBase::publishCpuUsage(tier4_external_api_msgs::msg::CpuUsage usage)
{
  // Create timestamp
  const auto stamp = this->now();

  usage.stamp = stamp;
  pub_cpu_usage_->publish(usage);
}

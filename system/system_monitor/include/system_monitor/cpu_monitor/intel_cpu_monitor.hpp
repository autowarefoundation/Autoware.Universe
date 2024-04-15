// Copyright 2020 Tier IV, Inc.
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
 * @file intel_cpu_monitor.h
 * @brief  CPU monitor class
 */

#ifndef SYSTEM_MONITOR__CPU_MONITOR__INTEL_CPU_MONITOR_HPP_
#define SYSTEM_MONITOR__CPU_MONITOR__INTEL_CPU_MONITOR_HPP_

#include "system_monitor/cpu_monitor/cpu_monitor_base.hpp"

#include <string>
#include <vector>

class CPUMonitor : public CPUMonitorBase
{
public:
  /**
   * @brief constructor
   * @param [in] options Options associated with this node.
   */
  explicit CPUMonitor(const rclcpp::NodeOptions & options);

protected:
  /**
   * @brief check CPU thermal throttling
   * @param [out] stat diagnostic message passed directly to diagnostic publish calls
   * @note NOLINT syntax is needed since diagnostic_updater asks for a non-const reference
   * to pass diagnostic message updated in this function to diagnostic publish calls.
   */
  void checkThrottling(
    diagnostic_updater::DiagnosticStatusWrapper & stat) override;  // NOLINT(runtime/references)

  /**
   * @brief check CPU information
   */
  void onTimer() override;

  /**
   * @brief execute read throttling
   */
  std::string executeReadThrottling(std::vector<int> & vector);

  /**
   * @brief get names for core temperature files
   */
  void getTempNames() override;

  /**
   * @brief Add a loadable kernel module msr
   */
  void modprobeMSR();

  int msr_reader_port_;   //!< @brief port number to connect to msr_reader
  int throttle_timeout_;  //!< @brief Timeout for reading thermal throttling

  std::mutex throttle_mutex_;         //!< @brief Mutex for output from reading thermal throttling
  std::string throttle_error_str_;    //!< @brief Error string
  std::vector<int> throttle_vector_;  //!< @brief Vector of core id and thermal throttling status
  double throttle_elapsed_ms_;        //!< @brief Execution time of reading thermal throttling
};

#endif  // SYSTEM_MONITOR__CPU_MONITOR__INTEL_CPU_MONITOR_HPP_

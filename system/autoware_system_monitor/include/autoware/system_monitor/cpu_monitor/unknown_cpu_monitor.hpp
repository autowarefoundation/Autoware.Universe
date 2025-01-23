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
 * @file unknown_cpu_monitor.h
 * @brief Unknown CPU monitor class
 */

#ifndef AUTOWARE__SYSTEM_MONITOR__CPU_MONITOR__UNKNOWN_CPU_MONITOR_HPP_
#define AUTOWARE__SYSTEM_MONITOR__CPU_MONITOR__UNKNOWN_CPU_MONITOR_HPP_

#include "autoware/system_monitor/cpu_monitor/cpu_monitor_base.hpp"

#include <string>

namespace autoware::system_monitor
{

class CPUMonitor : public CPUMonitorBase
{
public:
  /**
   * @brief constructor
   * @param [in] node_name Name of the node.
   * @param [in] options Options associated with this node.
   */
  explicit CPUMonitor(const rclcpp::NodeOptions & options);
};

}  // namespace autoware::system_monitor

#endif  // AUTOWARE__SYSTEM_MONITOR__CPU_MONITOR__UNKNOWN_CPU_MONITOR_HPP_

// Copyright 2023 The Autoware Contributors
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

#ifndef CORE__MANAGER_HPP_
#define CORE__MANAGER_HPP_

#include "types.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>
#include <string>
#include <vector>

namespace system_diagnostic_graph
{

class GraphManager final
{
public:
  GraphManager();
  ~GraphManager();

  void init(const std::string & file, const std::string & mode);
  void callback(const DiagnosticArray & array, const rclcpp::Time & stamp);
  void update(const rclcpp::Time & stamp);
  DiagnosticGraph create_graph_message() const;

  void debug();

private:
  std::vector<std::unique_ptr<BaseNode>> nodes_;
  rclcpp::Time stamp_;
};

}  // namespace system_diagnostic_graph

#endif  // CORE__MANAGER_HPP_

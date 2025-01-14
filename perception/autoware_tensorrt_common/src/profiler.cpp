// Copyright 2023 TIER IV, Inc.
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

#include <autoware/tensorrt_common/profiler.hpp>

#include <algorithm>
#include <iomanip>
#include <string>
#include <vector>

namespace autoware
{
namespace tensorrt_common
{

Profiler::Profiler(const std::vector<Profiler> & src_profilers)
{
  index_ = 0;
  for (const auto & src_profiler : src_profilers) {
    for (const auto & [name, record] : src_profiler.profile_) {
      auto it = profile_.find(name);
      if (it == profile_.end()) {
        profile_.emplace(name, record);
      } else {
        it->second.time += record.time;
        it->second.count += record.count;
      }
    }
  }
}

void Profiler::reportLayerTime(const char * layerName, float ms) noexcept
{
  if (profile_.find(layerName) == profile_.end()) {
    return;
  }
  profile_[layerName].count++;
  profile_[layerName].time += ms;
  if (profile_[layerName].min_time == -1.0) {
    profile_[layerName].min_time = ms;
    profile_[layerName].index = index_;
    index_++;
  } else if (profile_[layerName].min_time > ms) {
    profile_[layerName].min_time = ms;
  }
}

std::string Profiler::toString() const
{
  std::ostringstream out;
  float total_time = 0.0;
  std::string layer_name = "Operation";

  int max_layer_name_length = static_cast<int>(layer_name.size());
  for (const auto & elem : profile_) {
    total_time += elem.second.time;
    max_layer_name_length = std::max(max_layer_name_length, static_cast<int>(elem.first.size()));
  }

  auto old_settings = out.flags();
  auto old_precision = out.precision();
  // Output header
  {
    out << "index, " << std::setw(12);
    out << std::setw(max_layer_name_length) << layer_name << " ";
    out << std::setw(12) << "Runtime" << "%," << " ";
    out << std::setw(12) << "Invocations" << " , ";
    out << std::setw(12) << "Runtime[ms]" << " , ";
    out << std::setw(12) << "Avg Runtime[ms]" << " ,";
    out << std::setw(12) << "Min Runtime[ms]" << std::endl;
  }
  int index = index_;
  for (int i = 0; i < index; i++) {
    for (const auto & elem : profile_) {
      if (elem.second.index == i) {
        out << i << ",   ";
        out << std::setw(max_layer_name_length) << elem.first << ",";
        out << std::setw(12) << std::fixed << std::setprecision(1)
            << (elem.second.time * 100.0F / total_time) << "%" << ",";
        out << std::setw(12) << elem.second.count << ",";
        out << std::setw(12) << std::fixed << std::setprecision(2) << elem.second.time << ", ";
        out << std::setw(12) << std::fixed << std::setprecision(2)
            << elem.second.time / elem.second.count << ", ";
        out << std::setw(12) << std::fixed << std::setprecision(2) << elem.second.min_time
            << std::endl;
      }
    }
  }
  out.flags(old_settings);
  out.precision(old_precision);
  out << "========== total runtime = " << total_time << " ms ==========" << std::endl;

  return out.str();
}

std::ostream & operator<<(std::ostream & out, const Profiler & value)
{
  out << value.toString();
  return out;
}
}  // namespace tensorrt_common
}  // namespace autoware

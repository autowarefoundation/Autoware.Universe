// Copyright 2024 TIER IV, Inc.
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

#ifndef AUTOWARE__MTR__INTENTION_POINT_HPP_
#define AUTOWARE__MTR__INTENTION_POINT_HPP_

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace autoware::mtr
{
/**
 * @brief A class to load and store intention points.
 */
struct IntentionPoint
{
public:
  /**
   * @brief Construct a new instance from a csv file.
   *
   * @param csv_filepath Path to csv file.
   * @param num_cluster The number of clusters.
   */
  static IntentionPoint from_file(size_t num_cluster, const std::string csv_filepath)
  {
    std::ifstream file(csv_filepath);
    if (!file.is_open()) {
      std::ostringstream err_msg;
      err_msg << "Error opening file: " << csv_filepath << ". Please check if the file exists.";
      throw std::runtime_error(err_msg.str());
    }

    std::vector<std::tuple<float, float, std::string>> buffer;
    std::string line;
    while (std::getline(file, line)) {
      std::istringstream ss(line);
      float x, y;
      std::string label;

      ss >> x;
      ss.ignore();
      ss >> y;
      ss.ignore();
      std::getline(ss, label, ',');

      buffer.emplace_back(x, y, label);
    }
    file.close();

    std::unordered_map<std::string, std::vector<float>> data_map;
    for (const auto & [x, y, label] : buffer) {
      data_map[label].emplace_back(x);
      data_map[label].emplace_back(y);
    }

    return {num_cluster, data_map};
  }

  // Returns the point state dimension, which is 2 (x, y).
  static size_t state_dim() noexcept { return 2; }

  /**
   * @brief Load intention points for specified label names.
   *
   * @param label_names Array of label names for all agents, in shape [N].
   * @return std::vector<float> Array of all points in shape, [N * num_cluster * 2].
   */
  std::vector<float> as_array(const std::vector<std::string> & label_names) const;

  // Return the size of intension point `K*D`.
  size_t size() const noexcept;

  size_t num_cluster;  //!< The number of point clusters, which is K.

private:
  /**
   * @brief Construct a new Intention.
   *
   * @param data_map Map of intention points hashed by label names.
   * @param num_cluster The number of clusters.
   */
  IntentionPoint(
    size_t num_cluster, const std::unordered_map<std::string, std::vector<float>> data_map)
  : num_cluster(num_cluster), data_map_(data_map)
  {
  }

  std::unordered_map<std::string, std::vector<float>> data_map_;  //!< Map of label name and points.
};
}  // namespace autoware::mtr
#endif  // AUTOWARE__MTR__INTENTION_POINT_HPP_

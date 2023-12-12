// Copyright 2023 Autoware Foundation
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

#ifndef POSE_ESTIMATOR_ARBITER__BASE_POSE_ESTIMATOR_SUB_ARBITER_HPP_
#define POSE_ESTIMATOR_ARBITER__BASE_POSE_ESTIMATOR_SUB_ARBITER_HPP_

#include "shared_data.hpp"

#include <rclcpp/rclcpp.hpp>

#include <memory>

namespace pose_estimator_arbiter
{
class BasePoseEstimatorSubArbiter
{
public:
  using SharedPtr = std::shared_ptr<BasePoseEstimatorSubArbiter>;

  explicit BasePoseEstimatorSubArbiter(
    rclcpp::Node * node, const std::shared_ptr<const SharedData> shared_data)
  : logger_(node->get_logger()), shared_data_(shared_data)
  {
  }

  void enable() { set_enable(true); }
  void disable() { set_enable(false); }

  virtual void set_enable(bool enabled) = 0;

protected:
  rclcpp::Logger logger_;
  std::shared_ptr<const SharedData> shared_data_{nullptr};
};
}  // namespace pose_estimator_arbiter

#endif  // POSE_ESTIMATOR_ARBITER__BASE_POSE_ESTIMATOR_SUB_ARBITER_HPP_

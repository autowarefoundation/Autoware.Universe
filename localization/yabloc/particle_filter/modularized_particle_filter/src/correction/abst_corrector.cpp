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

#include "modularized_particle_filter/correction/abst_corrector.hpp"

namespace yabloc::modularized_particle_filter
{
AbstCorrector::AbstCorrector(const std::string & node_name)
: Node(node_name),
  acceptable_max_delay_(declare_parameter<float>("acceptable_max_delay", 1.0f)),
  visualize_(declare_parameter<bool>("visualize", false)),
  logger_(rclcpp::get_logger("abst_corrector"))
{
  using std::placeholders::_1;
  particle_pub_ = create_publisher<ParticleArray>("weighted_particles", 10);
  particle_sub_ = create_subscription<ParticleArray>(
    "predicted_particles", 10, std::bind(&AbstCorrector::on_particle_array, this, _1));

  if (visualize_) visualizer_ = std::make_shared<ParticleVisualizer>(*this);
}

void AbstCorrector::on_particle_array(const ParticleArray & particle_array)
{
  particle_array_buffer_.push_back(particle_array);
}

std::optional<AbstCorrector::ParticleArray> AbstCorrector::get_synchronized_particle_array(
  const rclcpp::Time & stamp)
{
  auto itr = particle_array_buffer_.begin();
  while (itr != particle_array_buffer_.end()) {
    rclcpp::Duration dt = rclcpp::Time(itr->header.stamp) - stamp;
    if (dt.seconds() < -acceptable_max_delay_)
      particle_array_buffer_.erase(itr++);
    else
      break;
  }

  if (particle_array_buffer_.empty()) {
    RCLCPP_WARN_STREAM_THROTTLE(
      logger_, *get_clock(), 2000, "sychronized particles are requested but buffer is empty");
  }

  if (particle_array_buffer_.empty()) return std::nullopt;

  auto comp = [stamp](ParticleArray & x1, ParticleArray & x2) -> bool {
    auto dt1 = rclcpp::Time(x1.header.stamp) - stamp;
    auto dt2 = rclcpp::Time(x2.header.stamp) - stamp;
    return std::abs(dt1.seconds()) < std::abs(dt2.seconds());
  };
  return *std::min_element(particle_array_buffer_.begin(), particle_array_buffer_.end(), comp);
}

void AbstCorrector::set_weighted_particle_array(const ParticleArray & particle_array)
{
  particle_pub_->publish(particle_array);
  if (visualize_) visualizer_->publish(particle_array);
}

}  // namespace yabloc::modularized_particle_filter
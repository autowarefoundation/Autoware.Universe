#ifndef MODULARIZED_PARTICLE_FILTER__COMMON__VISUALIZE_HPP_
#define MODULARIZED_PARTICLE_FILTER__COMMON__VISUALIZE_HPP_

#include <rclcpp/rclcpp.hpp>

#include <modularized_particle_filter_msgs/msg/particle_array.hpp>
#include <std_msgs/msg/color_rgba.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

class ParticleVisualizer
{
public:
  using MarkerArray = visualization_msgs::msg::MarkerArray;
  using Particle = modularized_particle_filter_msgs::msg::Particle;
  using ParticleArray = modularized_particle_filter_msgs::msg::ParticleArray;

  ParticleVisualizer(rclcpp::Node & node);
  void publish(const ParticleArray & msg);

private:
  rclcpp::Publisher<MarkerArray>::SharedPtr pub_marker_array_;
  std_msgs::msg::ColorRGBA computeColor(float value);
};

#endif  // MODULARIZED_PARTICLE_FILTER__CORRECTION__CORRECTION_UTIL_HPP_
#include "trajectory/twist_estimator.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<trajectory::TwistEstimator>());
  rclcpp::shutdown();
  return 0;
}
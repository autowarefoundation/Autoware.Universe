cmake_minimum_required(VERSION 3.14)
project(autoware_goal_distance_calculator)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/goal_distance_calculator_node.cpp
  src/goal_distance_calculator.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "autoware::goal_distance_calculator::GoalDistanceCalculatorNode"
  EXECUTABLE autoware_goal_distance_calculator_node
)

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
)

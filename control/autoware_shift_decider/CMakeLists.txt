cmake_minimum_required(VERSION 3.14)
project(autoware_shift_decider)

find_package(autoware_cmake REQUIRED)
autoware_package()

generate_parameter_library(shift_decider_parameters
  param/shift_decider_parameters.yaml
)

ament_auto_add_library(${PROJECT_NAME}_node SHARED
  src/${PROJECT_NAME}.cpp
)

target_link_libraries(${PROJECT_NAME}_node
  shift_decider_parameters
)

rclcpp_components_register_node(${PROJECT_NAME}_node
  PLUGIN "autoware::shift_decider::ShiftDecider"
  EXECUTABLE ${PROJECT_NAME}
)

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
)

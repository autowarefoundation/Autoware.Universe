cmake_minimum_required(VERSION 3.14)
project(autoware_glog_component)

find_package(autoware_cmake REQUIRED)
autoware_package()


ament_auto_add_library(${PROJECT_NAME} SHARED
  src/glog_component.cpp
)
target_link_libraries(${PROJECT_NAME} glog::glog)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "autoware::glog_component::GlogComponent"
  EXECUTABLE ${PROJECT_NAME}_node
)

ament_auto_package()

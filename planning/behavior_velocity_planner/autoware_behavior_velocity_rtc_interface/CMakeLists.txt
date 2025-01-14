cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_velocity_rtc_interface)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/scene_module_interface_with_rtc.cpp
)

ament_auto_package(INSTALL_TO_SHARE)

cmake_minimum_required(VERSION 3.5)
project(autoware_bag_recorder)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_executable(autoware_bag_recorder
  src/autoware_bag_recorder_node.cpp
  src/autoware_bag_recorder.cpp)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config)

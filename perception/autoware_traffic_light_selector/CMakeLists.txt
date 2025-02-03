cmake_minimum_required(VERSION 3.8)
project(autoware_traffic_light_selector)

# find dependencies
find_package(autoware_cmake REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Sophus REQUIRED)
find_package(Boost REQUIRED)
find_package(PCL REQUIRED)
find_package(CGAL REQUIRED COMPONENTS Core)

include_directories(
  include
  SYSTEM
  ${Boost_INCLUDE_DIRS}
  ${PCL_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${Sophus_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)

autoware_package()

# Targets
ament_auto_add_library(${PROJECT_NAME} SHARED
  src/traffic_light_selector_node.cpp
  src/utils.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "autoware::traffic_light::TrafficLightSelectorNode"
  EXECUTABLE traffic_light_selector_node)


if(BUILD_TESTING)
  list(APPEND AMENT_LINT_AUTO_EXCLUDE ament_cmake_uncrustify)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

endif()

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
)

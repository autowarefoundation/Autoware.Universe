cmake_minimum_required(VERSION 3.14)
project(autoware_lane_departure_checker)

find_package(autoware_cmake REQUIRED)
autoware_package()

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
)

ament_auto_add_library(autoware_lane_departure_checker SHARED
  src/lane_departure_checker_node/lane_departure_checker.cpp
  src/lane_departure_checker_node/lane_departure_checker_node.cpp
  src/lane_departure_checker_node/utils.cpp
  src/lane_departure_checker_node/parameters.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "autoware::lane_departure_checker::LaneDepartureCheckerNode"
  EXECUTABLE lane_departure_checker_node
)

if(BUILD_TESTING)
  file(GLOB_RECURSE TEST_SOURCES test/*.cpp)
  ament_add_gtest(test_autoware_lane_departure_checker
    ${TEST_SOURCES}
  )
  target_link_libraries(test_autoware_lane_departure_checker autoware_lane_departure_checker)
endif()

ament_auto_package(
  INSTALL_TO_SHARE
    launch
    config
)

cmake_minimum_required(VERSION 3.14)
project(autoware_traffic_light_utils)

find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(Boost REQUIRED)
find_package(autoware_cmake REQUIRED)

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/traffic_light_utils.cpp
)

if(BUILD_TESTING)
  find_package(ament_cmake_ros REQUIRED)
  file(GLOB_RECURSE TEST_SOURCES test/*.cpp)
  ament_add_ros_isolated_gtest(test_traffic_light_utils ${TEST_SOURCES})
  target_include_directories(test_traffic_light_utils PRIVATE src/include)
  target_link_libraries(test_traffic_light_utils ${PROJECT_NAME})
endif()

ament_auto_package()

cmake_minimum_required(VERSION 3.14)
project(autoware_fake_test_node)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED src/fake_test_node.cpp)

if(BUILD_TESTING)
  ament_add_ros_isolated_gtest(test_fake_test_node
    test/test_fake_test_node.cpp
  )
  add_dependencies(test_fake_test_node ${PROJECT_NAME})
  target_link_libraries(test_fake_test_node ${PROJECT_NAME})
endif()

ament_auto_package()

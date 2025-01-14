cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_velocity_walkway_module)

find_package(autoware_cmake REQUIRED)
autoware_package()
pluginlib_export_plugin_description_file(autoware_behavior_velocity_planner plugins.xml)

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/debug.cpp
  src/manager.cpp
  src/scene_walkway.cpp
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  file(GLOB_RECURSE TEST_SOURCES test/*.cpp)
  ament_add_ros_isolated_gtest(test_${PROJECT_NAME}
    ${TEST_SOURCES}
  )
  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME})
endif()

ament_auto_package(INSTALL_TO_SHARE config)

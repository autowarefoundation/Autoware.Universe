cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_path_start_planner_module)

find_package(autoware_cmake REQUIRED)
autoware_package()
pluginlib_export_plugin_description_file(autoware_behavior_path_planner plugins.xml)

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/start_planner_module.cpp
  src/manager.cpp
  src/pull_out_planner_base.cpp
  src/freespace_pull_out.cpp
  src/geometric_pull_out.cpp
  src/shift_pull_out.cpp
  src/data_structs.cpp
  src/util.cpp
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  # Add test helper library
  ament_auto_add_library(start_planner_test_helper SHARED
    test/src/start_planner_test_helper.cpp
  )
  target_include_directories(start_planner_test_helper PUBLIC
    test/include
  )

  file(GLOB_RECURSE TEST_SOURCES test/*.cpp)
  ament_add_ros_isolated_gtest(test_${PROJECT_NAME}
    ${TEST_SOURCES}
  )
  target_link_libraries(test_${PROJECT_NAME} ${PROJECT_NAME}
    start_planner_test_helper
  )
endif()

ament_auto_package(INSTALL_TO_SHARE config)

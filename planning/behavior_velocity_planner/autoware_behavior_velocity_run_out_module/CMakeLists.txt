cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_velocity_run_out_module)

find_package(autoware_cmake REQUIRED)
autoware_package()
pluginlib_export_plugin_description_file(autoware_behavior_velocity_planner plugins.xml)

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/debug.cpp
  src/dynamic_obstacle.cpp
  src/manager.cpp
  src/scene.cpp
  src/state_machine.cpp
  src/utils.cpp
  src/path_utils.cpp
)

if(BUILD_TESTING)
  ament_add_ros_isolated_gtest(test_${PROJECT_NAME}
    test/test_dynamic_obstacle.cpp
    test/test_path_utils.cpp
    test/test_utils.cpp
    test/test_state_machine.cpp
    test/test_node_interface.cpp
  )
  target_link_libraries(test_${PROJECT_NAME}
    autoware_behavior_velocity_run_out_module
  )
  target_include_directories(test_${PROJECT_NAME} PRIVATE src)
endif()



ament_auto_package(INSTALL_TO_SHARE config)

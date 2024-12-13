cmake_minimum_required(VERSION 3.14)
project(autoware_component_interface_tools)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/service_log_checker.cpp
)

if(BUILD_TESTING)
  ament_add_ros_isolated_gtest(test_service_log_checker
    test/test_service_log_checker.cpp
  )

  target_link_libraries(test_service_log_checker
  component_interface_tools
  )
  target_include_directories(test_service_log_checker PRIVATE src)
endif()

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "autoware::component_interface_tools::ServiceLogChecker"
  EXECUTABLE service_log_checker_node
)

ament_auto_package(INSTALL_TO_SHARE launch)

cmake_minimum_required(VERSION 3.14)
project(autoware_component_interface_tools)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/service_log_checker.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "autoware::component_interface_tools::ServiceLogChecker"
  EXECUTABLE service_log_checker_node
)

if(BUILD_TESTING)
  ament_add_ros_isolated_gtest(test_${PROJECT_NAME}
    test/test_autoware_component_interface_tools.cpp
  )
  target_link_libraries(test_${PROJECT_NAME}
    ${PROJECT_NAME}
  )
  target_include_directories(test_${PROJECT_NAME} PRIVATE src)
endif()

ament_auto_package(INSTALL_TO_SHARE launch)

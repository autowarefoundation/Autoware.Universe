cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_velocity_planner)

find_package(autoware_cmake REQUIRED)

autoware_package()

ament_auto_add_library(${PROJECT_NAME}_lib SHARED
  src/node.cpp
  src/planner_manager.cpp
  src/test_utils.cpp
)

rclcpp_components_register_node(${PROJECT_NAME}_lib
  PLUGIN "autoware::behavior_velocity_planner::BehaviorVelocityPlannerNode"
  EXECUTABLE ${PROJECT_NAME}_node
)

if(BUILD_TESTING)
  ament_add_ros_isolated_gtest(test_${PROJECT_NAME}
    test/test_node_interface.cpp
  )
  target_link_libraries(test_${PROJECT_NAME}
    gtest_main
    ${PROJECT_NAME}_lib
  )
  target_include_directories(test_${PROJECT_NAME} PRIVATE src)
endif()

ament_auto_package(INSTALL_TO_SHARE
  launch
  config
)

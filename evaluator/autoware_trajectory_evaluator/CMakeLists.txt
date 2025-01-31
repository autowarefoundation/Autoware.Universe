cmake_minimum_required(VERSION 3.22)
project(autoware_trajectory_evaluator)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(pluginlib REQUIRED)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_library(trajectory_evaluator_node SHARED
  src/trajectory_evaluator_node.cpp
  src/trajectory_evaluator.cpp
)

rclcpp_components_register_node(trajectory_evaluator_node
  PLUGIN "trajectory_evaluator::TrajectoryEvaluatorNode"
  EXECUTABLE trajectory_evaluator
)

ament_auto_package(
  INSTALL_TO_SHARE
    config
    launch
)

if(BUILD_TESTING)
  ament_add_gtest(test_trajectory_evaluator
    test/test_trajectory_evaluator_node.cpp
  )
  target_link_libraries(test_trajectory_evaluator
    trajectory_evaluator_node
  )
endif()

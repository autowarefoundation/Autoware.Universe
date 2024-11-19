cmake_minimum_required(VERSION 3.14)
project(autoware_lanelet2_map_visualizer)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(lanelet2_map_visualization_node SHARED
  src/lanelet2_map_visualization_node.cpp
)

rclcpp_components_register_node(lanelet2_map_visualization_node
  PLUGIN "autoware::lanelet2_map_visualizer::Lanelet2MapVisualizationNode"
  EXECUTABLE lanelet2_map_visualization
)

ament_auto_package(INSTALL_TO_SHARE
  launch
)

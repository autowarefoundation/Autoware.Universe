cmake_minimum_required(VERSION 3.14)
project(autoware_dynamic_lanelet_provider)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(dynamic_lanelet_provider_node SHARED
  src/dynamic_lanelet_provider.cpp
)

rclcpp_components_register_node(dynamic_lanelet_provider_node
  PLUGIN "autoware::dynamic_lanelet_provider::DynamicLaneletProviderNode"
  EXECUTABLE dynamic_lanelet_provider
)

ament_auto_package(INSTALL_TO_SHARE
        launch
        config
)

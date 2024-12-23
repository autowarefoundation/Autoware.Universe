cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_path_bidirectional_traffic_module)

find_package(autoware_cmake REQUIRED)
autoware_package()
pluginlib_export_plugin_description_file(autoware_behavior_path_planner plugins.xml)

# glob cpp
file(GLOB_RECURSE CPP_FILES src/*.cpp)
ament_auto_add_library(${PROJECT_NAME} SHARED ${CPP_FILES})

if(BUILD_TESTING)
endif()

ament_auto_package(INSTALL_TO_SHARE config)

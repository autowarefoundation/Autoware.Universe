cmake_minimum_required(VERSION 3.17)
project(autoware_tensorrt_bevdet)

add_compile_options(-std=c++17)

find_package(bevdet_vendor)
if(NOT ${bevdet_vendor_FOUND})
  message(WARNING "The bevdet_vendor package is not found. Please check its dependencies.")
  return()
endif()

find_package(autoware_cmake REQUIRED)
autoware_package()
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(autoware_perception_msgs REQUIRED)

ament_auto_add_library(${PROJECT_NAME}_component SHARED
    src/bevdet_node.cpp
    src/ros_utils.cpp
)

include_directories(
  include
  ${EIGEN3_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${cv_bridge_INCLUDE_DIRS}
  ${tf2_geometry_msgs_INCLUDE_DIRS}
  ${autoware_perception_msgs_INCLUDE_DIRS}
  ${bevdet_vendor_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}_component
    rclcpp::rclcpp
    ${bevdet_vendor_TARGETS}
    ${OpenCV_LIBS}
)

rclcpp_components_register_node(${PROJECT_NAME}_component
    PLUGIN "autoware::tensorrt_bevdet::TRTBEVDetNode"
    EXECUTABLE ${PROJECT_NAME}_node
)

ament_auto_package(
  INSTALL_TO_SHARE
    launch
    config
)

cmake_minimum_required(VERSION 3.5)
project(autoware_bytetrack3d)

find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
find_package(Boost REQUIRED)

#
# Core library
#
file(GLOB bytetrack3D_lib_src
  "lib/src/*.cpp"
)

ament_auto_add_library(bytetrack3D_lib SHARED
  ${bytetrack3D_lib_src}
)

target_include_directories(bytetrack3D_lib
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(bytetrack3D_lib
  Eigen3::Eigen
  yaml-cpp
)

#
# ROS node
#
ament_auto_add_library(${PROJECT_NAME} SHARED
  src/bytetrack3D.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  OpenCV
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
  $<INSTALL_INTERFACE:include>
)

ament_auto_add_library(${PROJECT_NAME}_node SHARED
  src/bytetrack3D_node.cpp
)

ament_target_dependencies(${PROJECT_NAME}_node
  OpenCV
)

target_link_libraries(${PROJECT_NAME}_node
  ${PROJECT_NAME}
)

rclcpp_components_register_node(${PROJECT_NAME}_node
  PLUGIN "bytetrack3D::ByteTrack3DNode"
  EXECUTABLE ${PROJECT_NAME}_node_exe
)

ament_auto_package(INSTALL_TO_SHARE
  launch
  config
)

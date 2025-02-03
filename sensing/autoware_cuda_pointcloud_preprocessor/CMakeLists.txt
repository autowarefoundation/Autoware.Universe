cmake_minimum_required(VERSION 3.14)
project(autoware_cuda_pointcloud_preprocessor)

find_package(ament_cmake_auto REQUIRED)
find_package(CUDA)
find_package(PCL REQUIRED)

if(NOT ${CUDA_FOUND})
  message(WARNING "cuda was not found, so the autoware_cuda_pointcloud_preprocessor package will not be built.")
  return()
endif()

ament_auto_find_build_dependencies()

# Default to C++17
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wunused-function)
endif ()

if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
endif()

include_directories(
    include
    SYSTEM
    ${CUDA_INCLUDE_DIRS}
)

list(APPEND CUDA_NVCC_FLAGS "--expt-relaxed-constexpr -diag-suppress 20012") # cSpell: ignore expt

cuda_add_library(cuda_pointcloud_preprocessor_lib SHARED
  src/cuda_pointcloud_preprocessor/cuda_pointcloud_preprocessor.cu
)

target_link_libraries(cuda_pointcloud_preprocessor_lib
  ${PCL_LIBRARIES}
)

target_include_directories(cuda_pointcloud_preprocessor_lib SYSTEM PRIVATE
  ${autoware_point_types_INCLUDE_DIRS}
  ${cuda_blackboard_INCLUDE_DIRS}
  ${geometry_msgs_INCLUDE_DIRS}
  ${nebula_common_INCLUDE_DIRS}
  ${rclcpp_INCLUDE_DIRS}
  ${rcl_interfaces_INCLUDE_DIRS}
  ${sensor_msgs_INCLUDE_DIRS}
  ${tf2_INCLUDE_DIRS}
)

# Targets
ament_auto_add_library(cuda_organized_pointcloud_adapter SHARED
  src/cuda_organized_pointcloud_adapter/cuda_organized_pointcloud_adapter_node.cpp
)

target_link_libraries(cuda_organized_pointcloud_adapter
  ${CUDA_LIBRARIES}
)

ament_auto_add_library(cuda_pointcloud_preprocessor SHARED
  src/cuda_pointcloud_preprocessor/cuda_pointcloud_preprocessor_node.cpp
)

target_link_libraries(cuda_pointcloud_preprocessor
  ${CUDA_LIBRARIES}
  cuda_pointcloud_preprocessor_lib
)

rclcpp_components_register_node(cuda_organized_pointcloud_adapter
  PLUGIN "autoware::cuda_organized_pointcloud_adapter::CudaOrganizedPointcloudAdapterNode"
  EXECUTABLE cuda_organized_pointcloud_adapter_node
)

rclcpp_components_register_node(cuda_pointcloud_preprocessor
  PLUGIN "autoware::cuda_pointcloud_preprocessor::CudaPointcloudPreprocessorNode"
  EXECUTABLE cuda_pointcloud_preprocessor_node
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_auto_package()

# Set ROS_DISTRO macros
set(ROS_DISTRO $ENV{ROS_DISTRO})
if(${ROS_DISTRO} STREQUAL "rolling")
    add_compile_definitions(ROS_DISTRO_ROLLING)
elseif(${ROS_DISTRO} STREQUAL "foxy")
    add_compile_definitions(ROS_DISTRO_FOXY)
elseif(${ROS_DISTRO} STREQUAL "galactic")
    add_compile_definitions(ROS_DISTRO_GALACTIC)
elseif(${ROS_DISTRO} STREQUAL "humble")
    add_compile_definitions(ROS_DISTRO_HUMBLE)
endif()

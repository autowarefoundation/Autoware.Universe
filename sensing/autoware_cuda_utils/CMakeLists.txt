cmake_minimum_required(VERSION 3.5)
project(autoware_cuda_utils)

find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(CUDA)

if(NOT ${CUDA_FOUND})
  message(WARNING "cuda is not found, so the autoware_cuda_utils package won't be built.")
  return()
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(CUDA)

ament_auto_package()

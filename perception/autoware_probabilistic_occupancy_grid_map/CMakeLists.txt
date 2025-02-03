cmake_minimum_required(VERSION 3.14)
project(autoware_probabilistic_occupancy_grid_map)

find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(CUDA)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL REQUIRED)

if(NOT ${CUDA_FOUND})
  message(WARNING "cuda was not found, so only autoware::occupancy_grid_map::LaserscanBasedOccupancyGridMapNode will be built.")
else()
  add_definitions(-DUSE_CUDA)
endif()

include_directories(
  include
  SYSTEM
    ${EIGEN3_INCLUDE_DIR}
    ${grid_map_ros_INCLUDE_DIRS}
)

if(${CUDA_FOUND})
  include_directories(
    SYSTEM
      ${CUDA_INCLUDE_DIRS}
  )

  # cSpell: ignore expt
  list(APPEND CUDA_NVCC_FLAGS --expt-relaxed-constexpr -diag-suppress 20012 --compiler-options -fPIC)
  set(CUDA_SEPARABLE_COMPILATION ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  cuda_add_library(${PROJECT_NAME}_cuda SHARED
    lib/costmap_2d/occupancy_grid_map_fixed_kernel.cu
    lib/costmap_2d/occupancy_grid_map_projective_kernel.cu
    lib/updater/binary_bayes_filter_updater_kernel.cu
    lib/updater/log_odds_bayes_filter_updater_kernel.cu
    lib/utils/utils_kernel.cu
  )

  target_link_libraries(${PROJECT_NAME}_cuda
    ${CUDA_LIBRARIES}
  )

  ament_auto_add_library(${PROJECT_NAME}_common SHARED
    lib/costmap_2d/occupancy_grid_map_base.cpp
    lib/updater/binary_bayes_filter_updater.cpp
    lib/utils/utils.cpp
  )
  target_link_libraries(${PROJECT_NAME}_common
    ${PCL_LIBRARIES}
    ${PROJECT_NAME}_cuda
  )

  # PointcloudBasedOccupancyGridMap
  ament_auto_add_library(pointcloud_based_occupancy_grid_map SHARED
    src/pointcloud_based_occupancy_grid_map/pointcloud_based_occupancy_grid_map_node.cpp
    lib/costmap_2d/occupancy_grid_map_fixed.cpp
    lib/costmap_2d/occupancy_grid_map_projective.cpp
  )

  target_link_libraries(pointcloud_based_occupancy_grid_map
    ${PCL_LIBRARIES}
    ${PROJECT_NAME}_common
    ${PROJECT_NAME}_cuda
  )

  rclcpp_components_register_node(pointcloud_based_occupancy_grid_map
    PLUGIN "autoware::occupancy_grid_map::PointcloudBasedOccupancyGridMapNode"
    EXECUTABLE pointcloud_based_occupancy_grid_map_node
  )

  # GridMapFusionNode
  ament_auto_add_library(synchronized_grid_map_fusion SHARED
    src/fusion/synchronized_grid_map_fusion_node.cpp
    lib/fusion_policy/fusion_policy.cpp
    lib/costmap_2d/occupancy_grid_map_fixed.cpp
    lib/updater/log_odds_bayes_filter_updater.cpp
    lib/utils/utils.cpp
  )

  target_link_libraries(synchronized_grid_map_fusion
    ${PCL_LIBRARIES}
  )

  rclcpp_components_register_node(synchronized_grid_map_fusion
    PLUGIN "autoware::occupancy_grid_map::GridMapFusionNode"
    EXECUTABLE synchronized_grid_map_fusion_node
  )
else()
  ament_auto_add_library(${PROJECT_NAME}_common SHARED
    lib/costmap_2d/occupancy_grid_map_base.cpp
    lib/updater/binary_bayes_filter_updater.cpp
    lib/utils/utils.cpp
  )

  target_link_libraries(${PROJECT_NAME}_common
    ${PCL_LIBRARIES}
  )
endif()


# LaserscanBasedOccupancyGridMap
ament_auto_add_library(laserscan_based_occupancy_grid_map SHARED
  src/laserscan_based_occupancy_grid_map/laserscan_based_occupancy_grid_map_node.cpp
  lib/costmap_2d/occupancy_grid_map.cpp
)

target_link_libraries(laserscan_based_occupancy_grid_map
  ${PCL_LIBRARIES}
  ${PROJECT_NAME}_common
)

rclcpp_components_register_node(laserscan_based_occupancy_grid_map
  PLUGIN "autoware::occupancy_grid_map::LaserscanBasedOccupancyGridMapNode"
  EXECUTABLE laserscan_based_occupancy_grid_map_node
)


if(${CUDA_FOUND})
  install(
    TARGETS ${PROJECT_NAME}_cuda
    DESTINATION lib
  )
endif()

ament_auto_package(
  INSTALL_TO_SHARE
    launch
    config
)

# test
# Temporary disabled, tracked by:
# https://github.com/autowarefoundation/autoware.universe/issues/7724
#if(BUILD_TESTING)
#  # launch_testing
#  find_package(launch_testing_ament_cmake REQUIRED)
#  add_launch_test(test/test_pointcloud_based_method.py)
#  add_launch_test(test/test_synchronized_grid_map_fusion_node.py)
#
#  # gtest
#  ament_add_gtest(test_utils
#    test/test_utils.cpp
#  )
#  ament_add_gtest(costmap_unit_tests
#    test/cost_value_test.cpp
#  )
#  ament_add_gtest(fusion_policy_unit_tests
#    test/fusion_policy_test.cpp
#    lib/fusion_policy/fusion_policy.cpp
#  )
#  target_link_libraries(test_utils
#    ${CUDA_LIBRARIES}
#    ${PCL_LIBRARIES}
#    ${PROJECT_NAME}_common
#    ${PROJECT_NAME}_cuda
#  )
#  target_include_directories(costmap_unit_tests PRIVATE "include")
#  target_include_directories(fusion_policy_unit_tests PRIVATE "include")
#endif()

cmake_minimum_required(VERSION 3.8)
project(autoware_tensorrt_rtmdet)

find_package(autoware_tensorrt_common)
if(NOT ${autoware_tensorrt_common_FOUND})
  message(WARNING "The autoware_tensorrt_common package is not found. Please check its dependencies.")
  return()
endif()

find_package(autoware_cmake REQUIRED)
autoware_package()

# TODO(amadeuszsz): Remove -Wno-deprecated-declarations once removing implicit quantization
add_compile_options(-Wno-deprecated-declarations)

find_package(OpenCV REQUIRED)

include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
  enable_language(CUDA)
else()
  message(WARNING "CUDA is not found. preprocess acceleration using CUDA will not be available.")
endif()

find_package(OpenMP)
if(OpenMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# tensorrt_rtmdet
ament_auto_add_library(${PROJECT_NAME} SHARED
        src/tensorrt_rtmdet.cpp
)

ament_target_dependencies(${PROJECT_NAME}
        OpenCV
)

if(CMAKE_CUDA_COMPILER)
  # preprocess
  cuda_add_library(${PROJECT_NAME}_gpu_preprocess
          SHARED
          src/preprocess.cu
  )

  target_include_directories(${PROJECT_NAME}_gpu_preprocess PUBLIC
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
          $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  )

  target_link_libraries(${PROJECT_NAME}
          ${autoware_tensorrt_common_LIBRARIES}
          ${PROJECT_NAME}_gpu_preprocess
  )

else()
  target_link_libraries(${PROJECT_NAME}
          ${autoware_tensorrt_common_LIBRARIES}
  )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        TENSORRT_VERSION_MAJOR=${TENSORRT_VERSION_MAJOR}
)

# tensorrt_rtmdet_node
ament_auto_add_library(${PROJECT_NAME}_node SHARED
        src/tensorrt_rtmdet_node.cpp
)

ament_target_dependencies(${PROJECT_NAME}_node
        OpenCV
)

target_link_libraries(${PROJECT_NAME}_node
        ${PROJECT_NAME}
)

target_compile_definitions(${PROJECT_NAME}_node PRIVATE
        TENSORRT_VERSION_MAJOR=${TENSORRT_VERSION_MAJOR}
)

rclcpp_components_register_node(${PROJECT_NAME}_node
        PLUGIN "autoware::tensorrt_rtmdet::TrtRTMDetNode"
        EXECUTABLE ${PROJECT_NAME}_node_exe
)

if(BUILD_TESTING)
  find_package(ament_cmake_ros REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_ros_isolated_gtest(test_rtmdet test/test_rtmdet.cpp)
  set_tests_properties(test_rtmdet PROPERTIES TIMEOUT 60) # It could take a long time on the first run to create the engine
  target_link_libraries(test_rtmdet ${PROJECT_NAME} ${PROJECT_NAME}_node)
  target_include_directories(test_rtmdet PRIVATE include)
endif()

ament_auto_package(INSTALL_TO_SHARE
        launch
        config
        test/data
)

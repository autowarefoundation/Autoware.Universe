cmake_minimum_required(VERSION 3.8)
project(autoware_mtr)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(CUDA REQUIRED)
if (CUDA_FOUND)
  include_directories(${CUDA_INCLUDE_DIRS})
  find_library(CUBLAS_LIBRARIES cublas HINTS ${CUDA_TOOLKIT_ROOT_DIR}/lib64
                                            ${CUDA_TOOLKIT_ROOT_DIR}/lib)
  find_library(
    CUDNN_LIBRARIES
    NAMES libcudnn.so${__cudnn_ver_suffix} libcudnn${__cudnn_ver_suffix}.dylib
          ${__cudnn_lib_win_name}
    PATHS $ENV{LD_LIBRARY_PATH} ${__libpath_cudart} ${CUDNN_ROOT_DIR}
          ${PC_CUDNN_LIBRARY_DIRS} ${CMAKE_INSTALL_PREFIX}
    PATH_SUFFIXES lib lib64 bin
    DOC "CUDNN library.")
else()
  message(FATAL_ERROR "Can not find CUDA")
endif()

list(APPEND TRT_PLUGINS "nvinfer")
list(APPEND TRT_PLUGINS "nvonnxparser")
list(APPEND TRT_PLUGINS "nvparsers")
foreach(libName ${TRT_PLUGINS})
  find_library(${libName}_lib NAMES ${libName} "/usr" PATH_SUFFIXES lib)
  list(APPEND TRT_PLUGINS ${${libName}_lib})
endforeach()

# TRT plugins and cuda kernels
cuda_add_library(${PROJECT_NAME}_cuda_lib SHARED
  lib/src/attention/trt_attn_value_computation_kernel.cu
  lib/src/attention/trt_attn_value_computation.cpp
  lib/src/attention/trt_attn_weight_computation_kernel.cu
  lib/src/attention/trt_attn_weight_computation.cpp
  lib/src/knn/trt_knn_batch_kernel.cu
  lib/src/knn/trt_knn_batch.cpp
  lib/src/knn/trt_knn_batch_mlogk_kernel.cu
  lib/src/knn/trt_knn_batch_mlogk.cpp
  lib/src/preprocess/agent_preprocess_kernel.cu
  lib/src/preprocess/polyline_preprocess_kernel.cu
  lib/src/postprocess/postprocess_kernel.cu
)
target_include_directories(${PROJECT_NAME}_cuda_lib PUBLIC
  lib/include
)

ament_auto_add_library(${PROJECT_NAME}_lib SHARED
  src/builder.cpp
  src/trt_mtr.cpp
  src/conversions/lanelet.cpp
  src/intention_point.cpp
  src/conversions/history.cpp
)
target_link_libraries(${PROJECT_NAME}_lib
  ${TRT_PLUGINS}
  ${TRT_PLUGIN_LIBS}
  ${CUDA_LIBRARIES}
  ${CUBLAS_LIBRARIES}
  ${CUDNN_LIBRARIES}
  ${PROJECT_NAME}_cuda_lib
)
target_include_directories(${PROJECT_NAME}_lib PUBLIC
  include
  lib/include
)

target_compile_options(${PROJECT_NAME}_lib PRIVATE
  -Wall -Wextra -Wpedantic -Werror -Wno-deprecated-declarations
)

# ROS 2 node
ament_auto_add_library(${PROJECT_NAME}_node SHARED
  src/node.cpp
)

target_link_libraries(${PROJECT_NAME}_node
  ${PROJECT_NAME}_lib
)

rclcpp_components_register_node(${PROJECT_NAME}_node
  PLUGIN "autoware::mtr::MTRNode"
  EXECUTABLE ${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  ament_auto_add_gtest(test_fixed_queue test/test_fixed_queue.cpp)
endif()

ament_auto_package(
  INSTALL_TO_SHARE
  launch
  config
  data
)

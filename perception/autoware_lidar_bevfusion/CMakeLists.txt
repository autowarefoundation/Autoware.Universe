cmake_minimum_required(VERSION 3.14)
project(autoware_lidar_bevfusion)

find_package(autoware_cmake REQUIRED)
autoware_package()

add_compile_options(-Wno-deprecated-declarations)

option(CUDA_VERBOSE "Verbose output of CUDA modules" OFF)

# set flags for CUDA availability
option(CUDA_AVAIL "CUDA available" OFF)
find_package(CUDA)
if(CUDA_FOUND)
  find_library(CUBLAS_LIBRARIES cublas HINTS
    ${CUDA_TOOLKIT_ROOT_DIR}/lib64
    ${CUDA_TOOLKIT_ROOT_DIR}/lib
  )
  if(CUDA_VERBOSE)
    message("CUDA is available!")
    message("CUDA Libs: ${CUDA_LIBRARIES}")
    message("CUDA Headers: ${CUDA_INCLUDE_DIRS}")
  endif()
  # Note: cublas_device was depreciated in CUDA version 9.2
  #       https://forums.developer.nvidia.com/t/where-can-i-find-libcublas-device-so-or-libcublas-device-a/67251/4
  #       In LibTorch, CUDA_cublas_device_LIBRARY is used.
  unset(CUDA_cublas_device_LIBRARY CACHE)
  set(CUDA_AVAIL ON)
else()
  message("CUDA NOT FOUND")
  set(CUDA_AVAIL OFF)
endif()

# set flags for TensorRT availability
option(TRT_AVAIL "TensorRT available" OFF)
# try to find the tensorRT modules
find_library(NVINFER nvinfer)
find_library(NVONNXPARSER nvonnxparser)
if(NVINFER AND NVONNXPARSER)
  if(CUDA_VERBOSE)
    message("TensorRT is available!")
    message("NVINFER: ${NVINFER}")
    message("NVONNXPARSER: ${NVONNXPARSER}")
  endif()
  set(TRT_AVAIL ON)
else()
  message("TensorRT is NOT Available")
  set(TRT_AVAIL OFF)
endif()

# set flags for CUDNN availability
option(CUDNN_AVAIL "CUDNN available" OFF)
# try to find the CUDNN module
find_library(CUDNN_LIBRARY
NAMES libcudnn.so${__cudnn_ver_suffix} libcudnn${__cudnn_ver_suffix}.dylib ${__cudnn_lib_win_name}
PATHS $ENV{LD_LIBRARY_PATH} ${__libpath_cudart} ${CUDNN_ROOT_DIR} ${PC_CUDNN_LIBRARY_DIRS} ${CMAKE_INSTALL_PREFIX}
PATH_SUFFIXES lib lib64 bin
DOC "CUDNN library."
)
if(CUDNN_LIBRARY)
  if(CUDA_VERBOSE)
    message(STATUS "CUDNN is available!")
    message(STATUS "CUDNN_LIBRARY: ${CUDNN_LIBRARY}")
  endif()
  set(CUDNN_AVAIL ON)
else()
  message("CUDNN is NOT Available")
  set(CUDNN_AVAIL OFF)
endif()

# set flags for spconv availability
option(SPCONV_AVAIL "spconv available" OFF)
# try to find spconv
find_package(cumm)
find_package(spconv)
if(${cumm_FOUND} AND ${spconv_FOUND})
  message("spconv is available!")
  set(SPCONV_AVAIL ON)
else()
  message("spconv is NOT Available")
  set(SPCONV_AVAIL OFF)
endif()

if(TRT_AVAIL AND CUDA_AVAIL AND CUDNN_AVAIL AND SPCONV_AVAIL)
  find_package(ament_cmake_auto REQUIRED)
  ament_auto_find_build_dependencies()

  include_directories(
    include
    ${CUDA_INCLUDE_DIRS}
  )

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CUDA_FLAGS ${CMAKE_CUDA_FLAGS} "-g -G")
  endif()

  # Find Eigen3
  find_package(Eigen3 3.3 REQUIRED NO_MODULE)

  add_definitions("-DTV_CUDA")

  list(APPEND CUDA_NVCC_FLAGS "--expt-relaxed-constexpr -diag-suppress 1675 --extended-lambda")

  cuda_add_library(bev_ops SHARED
    src/bev_ops/bev_pool_cuda.cu
  )

  add_library(autoware_tensorrt_plugins SHARED
    src/tensorrt_plugins/plugin_utils.cpp
    src/tensorrt_plugins/quick_cumsum_cuda_plugin.cpp
    src/tensorrt_plugins/quick_cumsum_cuda_plugin_creator.cpp
    src/tensorrt_plugins/get_indice_pairs_implicit_gemm_plugin_creator.cpp
    src/tensorrt_plugins/get_indice_pairs_implicit_gemm_plugin.cpp
    src/tensorrt_plugins/implicit_gemm_plugin_creator.cpp
    src/tensorrt_plugins/implicit_gemm_plugin.cpp

    src/tensorrt_plugins/plugin_registration.cpp
  )

  target_compile_definitions(autoware_tensorrt_plugins PRIVATE _GLIBCXX_USE_CXX11_ABI=1)

  target_link_libraries(autoware_tensorrt_plugins PRIVATE
      ${NVINFER}
      CUDA::cudart
      bev_ops
      spconv::spconv
  )

  cuda_add_library(${PROJECT_NAME}_cuda_lib SHARED
    lib/postprocess/circle_nms_kernel.cu
    lib/postprocess/postprocess_kernel.cu
    lib/preprocess/preprocess_kernel.cu
  )

  target_link_libraries(${PROJECT_NAME}_cuda_lib
    spconv::spconv
  )

  target_include_directories(${PROJECT_NAME}_cuda_lib
    SYSTEM PUBLIC
    ${autoware_cuda_utils_INCLUDE_DIRS}
  )

  ament_auto_add_library(${PROJECT_NAME}_lib SHARED
    lib/detection_class_remapper.cpp
    lib/postprocess/non_maximum_suppression.cpp
    lib/preprocess/voxel_generator.cpp
    lib/preprocess/pointcloud_densification.cpp
    lib/preprocess/precomputed_features.cpp
    lib/ros_utils.cpp
    lib/bevfusion_trt.cpp
  )

  target_compile_definitions(${PROJECT_NAME}_lib PRIVATE
    TENSORRT_VERSION_MAJOR=${TENSORRT_VERSION_MAJOR}
  )

  target_link_libraries(${PROJECT_NAME}_lib
    ${NVINFER}
    ${NVONNXPARSER}
    ${CUDA_LIBRARIES}
    ${CUBLAS_LIBRARIES}
    ${CUDA_curand_LIBRARY}
    ${CUDNN_LIBRARY}
    ${PROJECT_NAME}_cuda_lib
  )

  # To suppress unknown-pragmas error. The root-cause is CUB library in CUDA 11.6.
  # This issue was fixed by https://github.com/NVIDIA/cub/commit/7d608bf1dc14553e2fb219eabeed80b76621b6fe
  target_include_directories(${PROJECT_NAME}_lib
    SYSTEM PUBLIC
    ${CUDA_INCLUDE_DIRS}
    $(autoware_point_types_INCLUDE_DIRS)
  )

  ament_auto_add_library(${PROJECT_NAME}_component SHARED
    src/lidar_bevfusion_node.cpp
  )

  target_link_libraries(${PROJECT_NAME}_component
    ${PROJECT_NAME}_lib
  )

  rclcpp_components_register_node(${PROJECT_NAME}_component
    PLUGIN "autoware::lidar_bevfusion::LidarBEVFusionNode"
    EXECUTABLE ${PROJECT_NAME}_node
  )

  install(
    TARGETS ${PROJECT_NAME}_cuda_lib
    DESTINATION lib
  )

  install(
    TARGETS autoware_tensorrt_plugins
    DESTINATION share/${PROJECT_NAME}/plugins
  )

  ament_auto_package(
    INSTALL_TO_SHARE
      launch
      config
  )

else()
  find_package(ament_cmake_auto REQUIRED)
  ament_auto_find_build_dependencies()

  ament_auto_package(
    INSTALL_TO_SHARE
      launch
  )
endif()

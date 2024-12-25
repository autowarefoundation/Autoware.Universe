cmake_minimum_required(VERSION 3.14)

project(autoware_trajectory)

option(BUILD_EXAMPLES "Build examples" OFF)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(autoware_trajectory SHARED
  DIRECTORY src
)

if(BUILD_TESTING)
  find_package(ament_cmake_ros REQUIRED)

  file(GLOB_RECURSE test_files test/*.cpp)

  ament_add_ros_isolated_gtest(test_autoware_trajectory ${test_files})

  target_link_libraries(test_autoware_trajectory
    autoware_trajectory
  )
endif()

if(BUILD_EXAMPLES)
  message(STATUS "Building examples")

  include(FetchContent)
  fetchcontent_declare(
    matplotlibcpp17
    GIT_REPOSITORY https://github.com/soblin/matplotlibcpp17.git
    GIT_TAG master
  )
  fetchcontent_makeavailable(matplotlibcpp17)

  file(GLOB_RECURSE example_files examples/*.cpp)

  foreach(example_file ${example_files})
    get_filename_component(example_name ${example_file} NAME_WE)
    ament_auto_add_executable(${example_name} ${example_file})
    set_source_files_properties(${example_file} PROPERTIES COMPILE_FLAGS -Wno-error -Wno-attributes -Wno-unused-parameter)
    target_link_libraries(${example_name}
      autoware_trajectory
      matplotlibcpp17::matplotlibcpp17
    )
  endforeach()
endif()

ament_auto_package()

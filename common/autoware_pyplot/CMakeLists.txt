cmake_minimum_required(VERSION 3.14)

project(autoware_pyplot)

find_package(autoware_cmake REQUIRED)

find_package(
  Python3
  COMPONENTS Interpreter Development
  REQUIRED)
find_package(pybind11 REQUIRED)

autoware_package()

ament_auto_add_library(${PROJECT_NAME} STATIC
  DIRECTORY src
)
target_link_libraries(${PROJECT_NAME} ${Python3_LIBRARIES} pybind11::embed)

if(BUILD_TESTING)
  find_package(ament_cmake_ros REQUIRED)

  file(GLOB_RECURSE test_files test/*.cpp)

  ament_add_ros_isolated_gtest(test_${PROJECT_NAME} ${test_files})

  target_link_libraries(test_${PROJECT_NAME}
    ${PROJECT_NAME}
  )
endif()

ament_auto_package()

cmake_minimum_required(VERSION 3.14)
project(perception_utils)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/run_length_encoder.cpp
)

find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME}
  ${OpenCV_LIBS}
)

install(
  DIRECTORY include/
  DESTINATION include
)

if(BUILD_TESTING)
  ament_auto_add_gtest(test_utils
    test/test_utils.cpp
  )
endif()

ament_auto_package()

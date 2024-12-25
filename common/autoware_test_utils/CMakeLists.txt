cmake_minimum_required(VERSION 3.14)
project(autoware_test_utils)

find_package(autoware_cmake REQUIRED)
autoware_package()

ament_auto_add_library(autoware_test_utils SHARED
  src/autoware_test_utils.cpp
  src/mock_data_parser.cpp
)

ament_auto_add_executable(topic_snapshot_saver src/topic_snapshot_saver.cpp)

target_link_libraries(topic_snapshot_saver autoware_test_utils)

if(BUILD_TESTING)
  ament_auto_add_gtest(test_autoware_test_utils
    test/test_mock_data_parser.cpp
    test/test_autoware_test_manager.cpp
  )
endif()

ament_auto_package(INSTALL_TO_SHARE
  config
  test_map
  test_data
  launch
  rviz
)

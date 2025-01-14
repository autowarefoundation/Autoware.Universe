cmake_minimum_required(VERSION 3.14)
project(autoware_behavior_velocity_blind_spot_module)

option(EXPORT_TEST_PLOT_FIGURE "Export plot figures in test" OFF)

find_package(autoware_cmake REQUIRED)
autoware_package()
pluginlib_export_plugin_description_file(autoware_behavior_velocity_planner plugins.xml)

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/debug.cpp
  src/manager.cpp
  src/scene.cpp
  src/decisions.cpp
  src/util.cpp
  src/parameter.cpp
)

if(BUILD_TESTING)
  if(EXPORT_TEST_PLOT_FIGURE)
    add_definitions(-DEXPORT_TEST_PLOT_FIGURE "-Wno-attributes") # // cspell: ignore DEXPORT
  endif()
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  # NOTE(soblin): pybind11::scoped_interpreter needs to be initialized globally, not in the FixtureClass instantiated for each test suite
  ament_add_gtest(test_${PROJECT_NAME}_util
    test/test_util.cpp
  )
  target_link_libraries(test_${PROJECT_NAME}_util ${PROJECT_NAME})
endif()

ament_auto_package(INSTALL_TO_SHARE config test_data)

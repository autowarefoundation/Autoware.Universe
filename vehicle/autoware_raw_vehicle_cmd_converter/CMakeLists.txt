cmake_minimum_required(VERSION 3.14)
project(autoware_raw_vehicle_cmd_converter)

find_package(autoware_cmake REQUIRED)
find_package(pybind11 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Python REQUIRED COMPONENTS Development)
autoware_package()
add_compile_options(-fPIC)

ament_auto_add_library(actuation_map_converter SHARED
  src/accel_map.cpp
  src/brake_map.cpp
  src/steer_map.cpp
  src/csv_loader.cpp
  src/pid.cpp
  src/vgr.cpp
)

ament_auto_add_library(vehicle_adaptor SHARED
  src/vehicle_adaptor/vehicle_adaptor.cpp
  src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/src/vehicle_adaptor_compensator.cpp
  src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/src/inputs_prediction.cpp
  src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/src/nominal_dynamics.cpp
  src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/src/transform_vehicle_adaptor_model.cpp
  src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/src/inputs_ref_smoother.cpp
  src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/src/vehicle_adaptor_utils.cpp
)
target_include_directories(vehicle_adaptor PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/include
)
ament_auto_add_library(autoware_raw_vehicle_cmd_converter_node_component SHARED
  src/node.cpp
)
include_directories(
    src/vehicle_adaptor/autoware_vehicle_adaptor/autoware_vehicle_adaptor/include
    ${EIGEN3_INCLUDE_DIR}
    ${Python_INCLUDE_DIRS}
)
target_link_libraries(autoware_raw_vehicle_cmd_converter_node_component
  actuation_map_converter
  vehicle_adaptor yaml-cpp
  ${Python_LIBRARIES}
  pybind11::embed
)

rclcpp_components_register_node(autoware_raw_vehicle_cmd_converter_node_component
  PLUGIN "autoware::raw_vehicle_cmd_converter::RawVehicleCommandConverterNode"
  EXECUTABLE autoware_raw_vehicle_cmd_converter_node
)

if(BUILD_TESTING)
  set(TEST_SOURCES
    test/test_autoware_raw_vehicle_cmd_converter.cpp
  )
  set(TEST_RAW_VEHICLE_CMD_CONVERTER_EXE test_autoware_raw_vehicle_cmd_converter)
  ament_add_ros_isolated_gtest(${TEST_RAW_VEHICLE_CMD_CONVERTER_EXE} ${TEST_SOURCES})
  target_link_libraries(${TEST_RAW_VEHICLE_CMD_CONVERTER_EXE}
    actuation_map_converter
    vehicle_adaptor
    autoware_raw_vehicle_cmd_converter_node_component
  )
endif()

ament_auto_package(INSTALL_TO_SHARE
  config
  data
  launch
  test
)
target_compile_definitions(vehicle_adaptor PRIVATE BUILD_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/src/vehicle_adaptor/autoware_vehicle_adaptor\")
install(PROGRAMS scripts/plot_accel_brake_map.py
  DESTINATION lib/${PROJECT_NAME}
)

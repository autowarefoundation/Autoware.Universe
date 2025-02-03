cmake_minimum_required(VERSION 3.8)
project(autoware_minimap_overlay_rviz_plugin)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};/usr/share/cmake/geographiclib")

find_package(autoware_cmake REQUIRED)
autoware_package()

find_package(GeographicLib REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(OGRE REQUIRED)

set(
    headers_to_moc
)

set(
  headers_to_not_moc
  src/include/minimap.hpp
  src/include/goal_pose.hpp
  src/include/tile.hpp
  src/include/tile_field.hpp
  src/include/tile_provider.hpp
  src/include/overlay_utils.hpp
  src/include/path_overlay.hpp
)

foreach(header "${headers_to_moc}")
    qt5_wrap_cpp(display_moc_files "${header}")
endforeach()

set(
    display_source_files
    src/overlay_utils.cpp
    src/minimap.cpp
    src/tile.cpp
    src/tile_field.cpp
    src/tile_provider.cpp
    src/goal_pose.cpp
    src/path_overlay.cpp
)

ament_auto_add_library(
    ${PROJECT_NAME} SHARED
    ${display_moc_files}
    ${headers_to_not_moc}
    ${display_source_files}
)

target_compile_options(
  ${PROJECT_NAME}
  PUBLIC
)

include_directories(
  src
  /usr/include/x86_64-linux-gnu/qt5
  ${OGRE_INCLUDE_DIRS}
  ${rviz_common_INCLUDE_DIRS}
  ${rviz_default_plugins_INCLUDE_DIRS}
  ${GeographicLib_INCLUDE_DIRS}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE src
  PRIVATE /usr/include/x86_64-linux-gnu/qt5
  PRIVATE ${OGRE_INCLUDE_DIRS}
  PRIVATE ${rviz_common_INCLUDE_DIRS}
  PRIVATE ${rviz_default_plugins_INCLUDE_DIRS}
  PRIVATE ${GeographicLib_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Network
  Qt5::Widgets
  ${OGRE_LIBRARIES}
  ${rviz_common_LIBRARIES}
  ${rviz_default_plugins_LIBRARIES}
  ${GeographicLib_LIBRARIES}
)

# prevent pluginlib from using boost
target_compile_definitions(${PROJECT_NAME} PUBLIC "PLUGINLIB__DISABLE_BOOST_FUNCTIONS")
set_property(TARGET ${PROJECT_NAME} PROPERTY AUTOMOC ON)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icons")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons"
    DESTINATION "share/${PROJECT_NAME}"
  )
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    DESTINATION "share/${PROJECT_NAME}"
  )
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Install demo
install(FILES demo/demo.launch.xml
  DESTINATION share/${PROJECT_NAME}/launch)
install(PROGRAMS demo/publish_demo_data
  DESTINATION lib/${PROJECT_NAME})

pluginlib_export_plugin_description_file(rviz_common plugin_description.xml)
ament_package()

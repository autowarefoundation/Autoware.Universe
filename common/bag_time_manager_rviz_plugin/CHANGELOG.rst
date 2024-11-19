^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package bag_time_manager_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* Contributors: Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* build(iron): remove rmw_qos_profile_t (`#3809 <https://github.com/autowarefoundation/autoware.universe/issues/3809>`_)
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(bag_time_manager_rviz_plugin): update create_client API to use rc… (`#2783 <https://github.com/autowarefoundation/autoware.universe/issues/2783>`_)
  * fix(bag_time_manager_rviz_plugin): update create_client API to use rclcpp::QoS
* feat(bag_time_manager_rviz_plugin): add bag time manager rviz plugin (`#1776 <https://github.com/autowarefoundation/autoware.universe/issues/1776>`_)
  * feat(bag_timemanager): add bag time manager rviz plugin
  * fix: fix change
* Contributors: Daisuke Nishimatsu, Esteve Fernandez, Vincent Richard, taikitanaka3

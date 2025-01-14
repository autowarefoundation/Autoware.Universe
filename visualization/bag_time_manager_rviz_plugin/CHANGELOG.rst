^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package bag_time_manager_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore: move rviz plugins from common to visualization/ folder (`#9417 <https://github.com/autowarefoundation/autoware.universe/issues/9417>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe

0.39.0 (2024-11-25)
-------------------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Yutaka Kondo

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

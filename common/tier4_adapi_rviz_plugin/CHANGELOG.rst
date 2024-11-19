^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_adapi_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* style: update rviz plugin icons to match the theme (`#8868 <https://github.com/autowarefoundation/autoware.universe/issues/8868>`_)
* fix(tier4_adapi_rviz_plugin): fix unusedFunction (`#8840 <https://github.com/autowarefoundation/autoware.universe/issues/8840>`_)
  fix:unusedFunction
* feat(tier4_adapi_rviz_plugin, tier4_state_rviz_plugin): set timestamp to velocity_limit msg from rviz panels (`#8548 <https://github.com/autowarefoundation/autoware.universe/issues/8548>`_)
  set timestamp to velocity_limit msg
* feat(tier4_adapi_rviz_plugin): add legacy state panel (`#7494 <https://github.com/autowarefoundation/autoware.universe/issues/7494>`_)
* Contributors: Autumn60, Khalil Selyan, Takagi, Isamu, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* feat(default_ad_api): add door api (`#5737 <https://github.com/autowarefoundation/autoware.universe/issues/5737>`_)
* feat(tier4_adapi_rviz_plugin): add change button to the route panel (`#6326 <https://github.com/autowarefoundation/autoware.universe/issues/6326>`_)
  feat(tier4_adapi_rviz_plugin): add route change button to the route panel
* fix(readme): add acknowledgement for material icons in tool plugins (`#6354 <https://github.com/autowarefoundation/autoware.universe/issues/6354>`_)
* style(update): autoware tools icons (`#6351 <https://github.com/autowarefoundation/autoware.universe/issues/6351>`_)
* feat(tier4_adapi_rviz_plugin): add route panel (`#3840 <https://github.com/autowarefoundation/autoware.universe/issues/3840>`_)
  * feat: add panel
  * feat: set route
  * feat: set waypoints
  * feat: add readme
  * fix: copyright
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(tier4_adapi_rviz_plugin): add adapi rviz plugin (`#3380 <https://github.com/autowarefoundation/autoware.universe/issues/3380>`_)
  * feat(tier4_adapi_rviz_plugin): add adapi rviz plugin
  * feat: fix copyright and name
  ---------
* Contributors: Khalil Selyan, Takagi, Isamu, Vincent Richard

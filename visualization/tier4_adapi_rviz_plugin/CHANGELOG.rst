^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_adapi_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore: move rviz plugins from common to visualization/ folder (`#9417 <https://github.com/autowarefoundation/autoware.universe/issues/9417>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(autoware_ad_api_specs): prefix package and namespace with autoware (`#9250 <https://github.com/autowarefoundation/autoware.universe/issues/9250>`_)
  * refactor(autoware_ad_api_specs): prefix package and namespace with autoware
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api_specs to adapi_specs
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

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

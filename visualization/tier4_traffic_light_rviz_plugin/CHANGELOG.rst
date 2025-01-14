^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_traffic_light_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Ryohsuke Mitsudome, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* feat(behavior_velocity): support new traffic signal interface (`#4133 <https://github.com/autowarefoundation/autoware.universe/issues/4133>`_)
  * feat(behavior_velocity): support new traffic signal interface
  * style(pre-commit): autofix
  * add missing dependency
  * style(pre-commit): autofix
  * remove the external signal input source in behavior_planning_launch.py
  * replace TrafficLightElement with TrafficSignalElement
  * style(pre-commit): autofix
  * use the regulatory element id instead of traffic light id
  * change the input of traffic signal to traffic light arbiter
  * style(pre-commit): autofix
  * do not return until the all regulatory elements are checked
  * change input topic of the traffic signals
  * fix the traffic signal type in perception reproducer
  * add debug log when the signal data is outdated
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(rviz_plugin): fx traffic light and velocity factor rviz plugin (`#3598 <https://github.com/autowarefoundation/autoware.universe/issues/3598>`_)
  fix(rviz_plugin); fx traffic light and velocity factor rviz plugin
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* refactor(tier4_traffic_light_rviz_plugin): apply clang-tidy (`#1650 <https://github.com/autowarefoundation/autoware.universe/issues/1650>`_)
* fix: remove unused check of rviz plugin version (`#1474 <https://github.com/autowarefoundation/autoware.universe/issues/1474>`_)
* feat(traffic-light-rviz-panel): select traffic light ID from Combobox (`#1010 <https://github.com/autowarefoundation/autoware.universe/issues/1010>`_)
  * feat(traffic-light-rviz-panel): (1) select traffic light ID instead of inputting its value (2) added a workaround for tinyxml2::tinyxml2 for humble build (3) updated README and fixed mkdocs.yaml to upload .gif file to unvierse documentation (4)sort traffic light IDs, use scrollable box
* docs(tier4_traffic_light_rviz_plugin): update documentation (`#905 <https://github.com/autowarefoundation/autoware.universe/issues/905>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* feat(tier4_traffic_light_rviz_plugin): selectable light shape, status and confidence (`#663 <https://github.com/autowarefoundation/autoware.universe/issues/663>`_)
* feat(tier4_traffic_light_rviz_plugin): add traffic light publish panel (`#640 <https://github.com/autowarefoundation/autoware.universe/issues/640>`_)
  * feat(tier4_traffic_light_rviz_plugin): add traffic light publish panel
  * fix(tier4_traffic_light_rviz_plugin): fix behavior
  * fix: lisense description
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * fix: lisence description
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* Contributors: Hiroki OTA, Kenji Miyake, Mamoru Sobue, Satoshi OTA, Takagi, Isamu, Tomohito ANDO, Vincent Richard, taikitanaka3

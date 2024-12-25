^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package component_state_monitor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* fix(cpplint): include what you use - system (`#9573 <https://github.com/autowarefoundation/autoware.universe/issues/9573>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

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
* feat!: replace autoware_auto_msgs with autoware_msgs for system modules (`#7249 <https://github.com/autowarefoundation/autoware.universe/issues/7249>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* fix(componet_state_monitor): remove ndt node alive monitoring (`#6957 <https://github.com/autowarefoundation/autoware.universe/issues/6957>`_)
  remove ndt node alive monitoring
* chore(component_state_monitor): relax pose_estimator_pose timeout (`#6916 <https://github.com/autowarefoundation/autoware.universe/issues/6916>`_)
* Contributors: Ryohsuke Mitsudome, Shumpei Wakabayashi, Yamato Ando, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* fix(component_state_monitor): change pose_estimator_pose rate (`#6563 <https://github.com/autowarefoundation/autoware.universe/issues/6563>`_)
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* feat(component_state_monitor): monitor traffic light recognition output (`#5778 <https://github.com/autowarefoundation/autoware.universe/issues/5778>`_)
* feat(component_state_monitor): monitor pose_estimator output (`#5617 <https://github.com/autowarefoundation/autoware.universe/issues/5617>`_)
* docs: add readme for interface packages (`#4235 <https://github.com/autowarefoundation/autoware.universe/issues/4235>`_)
  add readme for interface packages
* chore: update maintainer (`#4140 <https://github.com/autowarefoundation/autoware.universe/issues/4140>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(pose_initializer): enable pose initialization while running (only for sim) (`#3038 <https://github.com/autowarefoundation/autoware.universe/issues/3038>`_)
  * feat(pose_initializer): enable pose initialization while running (only for sim)
  * both logsim and psim params
  * only one pose_initializer_param_path arg
  * use two param files for pose_initializer
  ---------
* fix(component_state_monitor): add dependency on topic_state_monitor (`#3030 <https://github.com/autowarefoundation/autoware.universe/issues/3030>`_)
* fix(component_state_monitor): fix lanelet route package (`#2552 <https://github.com/autowarefoundation/autoware.universe/issues/2552>`_)
* feat!: replace HADMap with Lanelet (`#2356 <https://github.com/autowarefoundation/autoware.universe/issues/2356>`_)
  * feat!: replace HADMap with Lanelet
  * update topic.yaml
  * Update perception/traffic_light_map_based_detector/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update planning/behavior_path_planner/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update planning/mission_planner/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update planning/scenario_selector/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * format readme
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* feat(component_state_monitor): add component state monitor (`#2120 <https://github.com/autowarefoundation/autoware.universe/issues/2120>`_)
  * feat(component_state_monitor): add component state monitor
  * feat: change module
* Contributors: Kenji Miyake, Kosuke Takeuchi, Takagi, Isamu, Tomohito ANDO, Vincent Richard, Yamato Ando, kminoda

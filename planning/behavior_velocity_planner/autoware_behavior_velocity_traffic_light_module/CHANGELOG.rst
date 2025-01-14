^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_velocity_traffic_light_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* ci(pre-commit): autoupdate (`#8949 <https://github.com/autowarefoundation/autoware.universe/issues/8949>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@autoware.org>
* feat(behavior_velocity_planner)!: remove stop_reason (`#9452 <https://github.com/autowarefoundation/autoware.universe/issues/9452>`_)
* refactor(autoware_behavior_velocity_planner_common,autoware_behavior_velocity_planner): separate param files (`#9470 <https://github.com/autowarefoundation/autoware.universe/issues/9470>`_)
  * refactor(autoware_behavior_velocity_planner_common,autoware_behavior_velocity_planner): separate param files
  * Update planning/autoware_static_centerline_generator/test/test_static_centerline_generator.test.py
  Co-authored-by: Kyoichi Sugahara <32741405+kyoichi-sugahara@users.noreply.github.com>
  * fix
  ---------
  Co-authored-by: Kyoichi Sugahara <32741405+kyoichi-sugahara@users.noreply.github.com>
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(traffic_light_utils): prefix package and namespace with autoware (`#9251 <https://github.com/autowarefoundation/autoware.universe/issues/9251>`_)
* feat(behavior_velocity_planner): replace first_stop_path_point_index (`#9296 <https://github.com/autowarefoundation/autoware.universe/issues/9296>`_)
  * feat(behavior_velocity_planner): replace first_stop_path_point_index
  * add const
  * Update planning/behavior_velocity_planner/autoware_behavior_velocity_traffic_light_module/src/scene.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update planning/behavior_velocity_planner/autoware_behavior_velocity_planner/src/planner_manager.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Mamoru Sobue, Ryohsuke Mitsudome, Yukinari Hisaki, Yutaka Kondo, awf-autoware-bot[bot]

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
* test(traffic_light): write test for utils (`#9046 <https://github.com/autowarefoundation/autoware.universe/issues/9046>`_)
  * refactor: separate utils file
  * refactor: utils function output
  * test: write test for utils
  * chore: add doxygen
  ---------
* fix(behavior_velocity_traffic_light): make dilemma_zone_plotter.py executable (`#8684 <https://github.com/autowarefoundation/autoware.universe/issues/8684>`_)
* feat(traffic_light): add dilemma_zone_plotter.py (`#8638 <https://github.com/autowarefoundation/autoware.universe/issues/8638>`_)
  * feat(traffic_light): add dilemma_zone_plotter.py
  * fix typo
  * fix typo
  ---------
* fix(behavior_velocity_planner): fix cppcheck warnings of virtualCallInConstructor (`#8376 <https://github.com/autowarefoundation/autoware.universe/issues/8376>`_)
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
* fix(autoware_behavior_velocity_traffic_light_module): fix uninitMemberVar (`#8317 <https://github.com/autowarefoundation/autoware.universe/issues/8317>`_)
  fix:funinitMemberVar
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* chore(behavior_velocity_planner): move packages (`#7526 <https://github.com/autowarefoundation/autoware.universe/issues/7526>`_)
* Contributors: Fumiya Watanabe, Kosuke Takeuchi, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------

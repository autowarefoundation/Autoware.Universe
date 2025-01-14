^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_path_planner
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(bpp)!: remove stop reason (`#9449 <https://github.com/autowarefoundation/autoware.universe/issues/9449>`_)
  fix(bpp): remove stop reason
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(bpp): add velocity interface (`#9344 <https://github.com/autowarefoundation/autoware.universe/issues/9344>`_)
  * feat(bpp): add velocity interface
  * fix(adapi): subscribe additional velocity factors
  ---------
* refactor(factor): move steering factor interface to motion utils (`#9337 <https://github.com/autowarefoundation/autoware.universe/issues/9337>`_)
* refactor(bpp): rework steering factor interface (`#9325 <https://github.com/autowarefoundation/autoware.universe/issues/9325>`_)
  * refactor(bpp): rework steering factor interface
  * refactor(soa): rework steering factor interface
  * refactor(AbLC): rework steering factor interface
  * refactor(doa): rework steering factor interface
  * refactor(lc): rework steering factor interface
  * refactor(gp): rework steering factor interface
  * refactor(sp): rework steering factor interface
  * refactor(sbp): rework steering factor interface
  * refactor(ss): rework steering factor interface
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Satoshi OTA, Yutaka Kondo

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
* fix(autoware_behavior_path_planner): fix cppcheck unusedVariable (`#9193 <https://github.com/autowarefoundation/autoware.universe/issues/9193>`_)
* fix(behavior_path_planner): suppress reseting root lanelet (`#9129 <https://github.com/autowarefoundation/autoware.universe/issues/9129>`_)
  fix(behavior_path_planner): suppress resseting root lanelet
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* test(bpp_common): add test for object related functions (`#9062 <https://github.com/autowarefoundation/autoware.universe/issues/9062>`_)
  * add test for object related functions
  * use EXPECT_DOUBLE_EQ instead of EXPECT_NEAR
  * fix build error
  ---------
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(signal_processing): prefix package and namespace with autoware (`#8541 <https://github.com/autowarefoundation/autoware.universe/issues/8541>`_)
* chore(planning): consistent parameters with autoware_launch (`#8915 <https://github.com/autowarefoundation/autoware.universe/issues/8915>`_)
  * chore(planning): consistent parameters with autoware_launch
  * update
  * fix json schema
  ---------
* fix(autoware_behavior_path_planner): fix syntaxError (`#8834 <https://github.com/autowarefoundation/autoware.universe/issues/8834>`_)
  fix:syntaxError
* fix(autoware_behavior_path_planner): align the parameters with launcher (`#8790 <https://github.com/autowarefoundation/autoware.universe/issues/8790>`_)
  parameters in behavior_path_planner aligned
* refactor(behavior_path_planner): planner data parameter initializer function (`#8767 <https://github.com/autowarefoundation/autoware.universe/issues/8767>`_)
* chore(autoware_default_adapi)!: prefix autoware to package name (`#8533 <https://github.com/autowarefoundation/autoware.universe/issues/8533>`_)
* fix(docs): fix dead links in behavior path planner manager (`#8309 <https://github.com/autowarefoundation/autoware.universe/issues/8309>`_)
  * fix dead links
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(behavior_path_planner, spellchecks): spell checks in behavior path planner (`#8307 <https://github.com/autowarefoundation/autoware.universe/issues/8307>`_)
  * fix spell checks in behavior path planner
  * try re-routable
  ---------
* feat(behavior_path _planner): divide planner manager modules into dependent slots (`#8117 <https://github.com/autowarefoundation/autoware.universe/issues/8117>`_)
* fix(behavior_path_planner_common): fix dynamic drivable area expansion with few input bound points (`#8136 <https://github.com/autowarefoundation/autoware.universe/issues/8136>`_)
* refactor(autoware_universe_utils): changed the API to be more intuitive and added documentation (`#7443 <https://github.com/autowarefoundation/autoware.universe/issues/7443>`_)
  * refactor(tier4_autoware_utils): Changed the API to be more intuitive and added documentation.
  * use raw shared ptr in PollingPolicy::NEWEST
  * update
  * fix
  * Update evaluator/autoware_control_evaluator/include/autoware/control_evaluator/control_evaluator_node.hpp
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
  ---------
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
* feat(autoware_behavior_path_planner): prevent infinite loop in approving scene module process (`#7881 <https://github.com/autowarefoundation/autoware.universe/issues/7881>`_)
  * prevent infinite loop
  * calculate max_iteration_num from number of scene modules
  * add doxygen explanation for calculateMaxIterationNum
  ---------
* feat(autoware_behavior_path_planner_common,autoware_behavior_path_lane_change_module): add time_keeper to bpp (`#8004 <https://github.com/autowarefoundation/autoware.universe/issues/8004>`_)
  * feat(autoware_behavior_path_planner_common,autoware_behavior_path_lane_change_module): add time_keeper to bpp
  * update
  ---------
* feat(autoware_behavior_path_planner): remove max_module_size param (`#7764 <https://github.com/autowarefoundation/autoware.universe/issues/7764>`_)
  * feat(behavior_path_planner): remove max_module_size param
  The max_module_size param has been removed from the behavior_path_planner scene_module_manager.param.yaml file. This param was unnecessary and has been removed to simplify the configuration.
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(behaivor_path_planner)!: rename to include/autoware/{package_name} (`#7522 <https://github.com/autowarefoundation/autoware.universe/issues/7522>`_)
  * refactor(behavior_path_planner)!: make autoware dir in include
  * refactor(start_planner): make autoware include dir
  * refactor(goal_planner): make autoware include dir
  * sampling planner module
  * fix sampling planner build
  * dynamic_avoidance
  * lc
  * side shift
  * autoware_behavior_path_static_obstacle_avoidance_module
  * autoware_behavior_path_planner_common
  * make behavior_path dir
  * pre-commit
  * fix pre-commit
  * fix build
  ---------
* Contributors: Esteve Fernandez, Go Sakayori, Kosuke Takeuchi, Kyoichi Sugahara, Mamoru Sobue, Maxime CLEMENT, Ryuta Kambe, Takagi, Isamu, Takayuki Murooka, Yukinari Hisaki, Yutaka Kondo, Yuxuan Liu, Zhe Shen, kobayu858

0.26.0 (2024-04-03)
-------------------

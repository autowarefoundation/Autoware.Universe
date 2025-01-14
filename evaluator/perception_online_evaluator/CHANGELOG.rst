^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package perception_online_evaluator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - evaluator (`#9566 <https://github.com/autowarefoundation/autoware.universe/issues/9566>`_)
* refactor(perception_online_evaluator): use tier4_metric_msgs instead of diagnostic_msgs (`#9485 <https://github.com/autowarefoundation/autoware.universe/issues/9485>`_)
* refactor(evaluators, autoware_universe_utils): rename Stat class to Accumulator and move it to autoware_universe_utils (`#9459 <https://github.com/autowarefoundation/autoware.universe/issues/9459>`_)
  * add Accumulator class to autoware_universe_utils
  * use Accumulator on all evaluators.
  * pre-commit
  * found and fixed a bug. add more tests.
  * pre-commit
  * Update common/autoware_universe_utils/include/autoware/universe_utils/math/accumulator.hpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(evaluator): missing dependency in evaluator components (`#9074 <https://github.com/autowarefoundation/autoware.universe/issues/9074>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kem (TiankuiXian), Kotaro Uetake, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, ぐるぐる

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
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* fix(perception_online_evaluator): fix unusedFunction (`#8559 <https://github.com/autowarefoundation/autoware.universe/issues/8559>`_)
  fix:unusedFunction
* feat(evalautor): rename evaluator diag topics (`#8152 <https://github.com/autowarefoundation/autoware.universe/issues/8152>`_)
  * feat(evalautor): rename evaluator diag topics
  * perception
  ---------
* fix(perception_online_evaluator): passedByValue (`#8201 <https://github.com/autowarefoundation/autoware.universe/issues/8201>`_)
  fix: passedByValue
* fix(perception_online_evaluator): fix shadowVariable (`#7933 <https://github.com/autowarefoundation/autoware.universe/issues/7933>`_)
  * fix:shadowVariable
  * fix:clang-format
  * fix:shadowVariable
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(vehicle_info_utils)!: prefix package and namespace with autoware (`#7353 <https://github.com/autowarefoundation/autoware.universe/issues/7353>`_)
  * chore(autoware_vehicle_info_utils): rename header
  * chore(bpp-common): vehicle info
  * chore(path_optimizer): vehicle info
  * chore(velocity_smoother): vehicle info
  * chore(bvp-common): vehicle info
  * chore(static_centerline_generator): vehicle info
  * chore(obstacle_cruise_planner): vehicle info
  * chore(obstacle_velocity_limiter): vehicle info
  * chore(mission_planner): vehicle info
  * chore(obstacle_stop_planner): vehicle info
  * chore(planning_validator): vehicle info
  * chore(surround_obstacle_checker): vehicle info
  * chore(goal_planner): vehicle info
  * chore(start_planner): vehicle info
  * chore(control_performance_analysis): vehicle info
  * chore(lane_departure_checker): vehicle info
  * chore(predicted_path_checker): vehicle info
  * chore(vehicle_cmd_gate): vehicle info
  * chore(obstacle_collision_checker): vehicle info
  * chore(operation_mode_transition_manager): vehicle info
  * chore(mpc): vehicle info
  * chore(control): vehicle info
  * chore(common): vehicle info
  * chore(perception): vehicle info
  * chore(evaluator): vehicle info
  * chore(freespace): vehicle info
  * chore(planning): vehicle info
  * chore(vehicle): vehicle info
  * chore(simulator): vehicle info
  * chore(launch): vehicle info
  * chore(system): vehicle info
  * chore(sensing): vehicle info
  * fix(autoware_joy_controller): remove unused deps
  ---------
* fix(perception_online_evaluator):  add metric_value not only stat (`#7100 <https://github.com/autowarefoundation/autoware.universe/issues/7100>`_)(`#7118 <https://github.com/autowarefoundation/autoware.universe/issues/7118>`_) (revert of revert) (`#7167 <https://github.com/autowarefoundation/autoware.universe/issues/7167>`_)
  * Revert "fix(perception_online_evaluator): revert "add metric_value not only s…"
  This reverts commit d827b1bd1f4bbacf0333eb14a62ef42e56caef25.
  * Update evaluator/perception_online_evaluator/include/perception_online_evaluator/perception_online_evaluator_node.hpp
  * Update evaluator/perception_online_evaluator/src/perception_online_evaluator_node.cpp
  * use emplace back
  ---------
  Co-authored-by: Kotaro Uetake <60615504+ktro2828@users.noreply.github.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for evaluator modules (`#7241 <https://github.com/autowarefoundation/autoware.universe/issues/7241>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* fix(perception_online_evaluator): revert "add metric_value not only stat (`#7100 <https://github.com/autowarefoundation/autoware.universe/issues/7100>`_)" (`#7118 <https://github.com/autowarefoundation/autoware.universe/issues/7118>`_)
* feat(perception_online_evaluator): add metric_value not only stat (`#7100 <https://github.com/autowarefoundation/autoware.universe/issues/7100>`_)
* fix(perception_online_evaluator): fix range resolution (`#7115 <https://github.com/autowarefoundation/autoware.universe/issues/7115>`_)
* chore(glog): add initialization check (`#6792 <https://github.com/autowarefoundation/autoware.universe/issues/6792>`_)
* fix(perception_online_evaluator): fix bug of constStatement (`#6922 <https://github.com/autowarefoundation/autoware.universe/issues/6922>`_)
* feat(perception_online_evaluator): imporve yaw rate metrics considering flip (`#6881 <https://github.com/autowarefoundation/autoware.universe/issues/6881>`_)
  * feat(perception_online_evaluator): imporve yaw rate metrics considering flip
  * fix test
  ---------
* feat(perception_evaluator): counts objects within detection range  (`#6848 <https://github.com/autowarefoundation/autoware.universe/issues/6848>`_)
  * feat(perception_evaluator): counts objects within detection range
  detection counter
  add enable option and refactoring
  fix
  update document
  readme
  clean up
  * fix from review
  * use $
  fix
  * fix include
  ---------
* docs(perception_online_evaluator): update metrics explanation (`#6819 <https://github.com/autowarefoundation/autoware.universe/issues/6819>`_)
* feat(perception_online_evaluator): better waitForDummyNode (`#6827 <https://github.com/autowarefoundation/autoware.universe/issues/6827>`_)
* feat(perception_online_evaluator): add predicted path variance (`#6793 <https://github.com/autowarefoundation/autoware.universe/issues/6793>`_)
  * feat(perception_online_evaluator): add predicted path variance
  * add unit test
  * update readme
  * pre commit
  ---------
* feat(perception_online_evaluator): ignore reversal of orientation from yaw_rate calculation (`#6748 <https://github.com/autowarefoundation/autoware.universe/issues/6748>`_)
* docs(perception_online_evaluator): add description about yaw rate evaluation (`#6737 <https://github.com/autowarefoundation/autoware.universe/issues/6737>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, Kyoichi Sugahara, Nagi70, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Takamasa Horibe, Takayuki Murooka, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* feat(perception_online_evaluator): extract moving object for deviation check (`#6682 <https://github.com/autowarefoundation/autoware.universe/issues/6682>`_)
  fix test
* feat(perception_online_evaluator): unify debug markers instead of separating for each object (`#6681 <https://github.com/autowarefoundation/autoware.universe/issues/6681>`_)
  * feat(perception_online_evaluator): unify debug markers instead of separating for each object
  * fix for
  ---------
* feat(perception_online_evaluator): add yaw rate metrics for stopped object (`#6667 <https://github.com/autowarefoundation/autoware.universe/issues/6667>`_)
  * feat(perception_online_evaluator): add yaw rate metrics for stopped object
  add
  add test
  * feat: add stopped vel parameter
  ---------
* fix(perception_online_evaluator): fix build error (`#6595 <https://github.com/autowarefoundation/autoware.universe/issues/6595>`_)
* build(perception_online_evaluator): add lanelet_extension dependency (`#6592 <https://github.com/autowarefoundation/autoware.universe/issues/6592>`_)
* feat(perception_online_evaluator): publish metrics of each object class (`#6556 <https://github.com/autowarefoundation/autoware.universe/issues/6556>`_)
* feat(perception_online_evaluator): add perception_online_evaluator (`#6493 <https://github.com/autowarefoundation/autoware.universe/issues/6493>`_)
  * feat(perception_evaluator): add perception_evaluator
  tmp
  update
  add
  add
  add
  update
  clean up
  change time horizon
  * fix build werror
  * fix topic name
  * clean up
  * rename to perception_online_evaluator
  * refactor: remove timer
  * feat: add test
  * fix: ci check
  ---------
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Satoshi OTA

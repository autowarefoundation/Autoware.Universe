^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_path_dynamic_obstacle_avoidance_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* refactor(signal_processing): prefix package and namespace with autoware (`#8541 <https://github.com/autowarefoundation/autoware.universe/issues/8541>`_)
* fix(autoware_behavior_path_planner): align the parameters with launcher (`#8790 <https://github.com/autowarefoundation/autoware.universe/issues/8790>`_)
  parameters in behavior_path_planner aligned
* feat(autoware_behavior_path_dynamic_obstacle_avoidance): expand drivable area (`#8295 <https://github.com/autowarefoundation/autoware.universe/issues/8295>`_)
  * add expansion drivable area feature
  * make expansion optional
  * style(pre-commit): autofix
  * clean the code
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(bpp): use common steering factor interface for same scene modules (`#8675 <https://github.com/autowarefoundation/autoware.universe/issues/8675>`_)
* fix(autoware_behavior_path_dynamic_obstacle_avoidance_module): fix unusedFunction (`#8652 <https://github.com/autowarefoundation/autoware.universe/issues/8652>`_)
  fix:unusedFunction
* feat(behavior_path _planner): divide planner manager modules into dependent slots (`#8117 <https://github.com/autowarefoundation/autoware.universe/issues/8117>`_)
* perf(dynamic_obstacle_avoidance): decrease the computation time with time_keeper (`#7986 <https://github.com/autowarefoundation/autoware.universe/issues/7986>`_)
  * decrease computation cost
  * remove TODO
  * fix
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* fix(autoware_behavior_path_dynamic_obstacle_avoidance_module): fix bitwiseOnBoolean warning (`#7636 <https://github.com/autowarefoundation/autoware.universe/issues/7636>`_)
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
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Mamoru Sobue, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, Zhe Shen, beyzanurkaya, kobayu858

0.26.0 (2024-04-03)
-------------------

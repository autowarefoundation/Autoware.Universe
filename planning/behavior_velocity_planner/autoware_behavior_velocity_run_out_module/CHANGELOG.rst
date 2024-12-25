^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_velocity_run_out_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* feat(behavior_velocity_planner)!: remove stop_reason (`#9452 <https://github.com/autowarefoundation/autoware.universe/issues/9452>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(behavior_velocity_planner): update velocity factor initialization for run out module (`#9352 <https://github.com/autowarefoundation/autoware.universe/issues/9352>`_)
  feat(behavior_velocity_planner): update velocity factor initialization
  Update the initialization of the velocity factor in the RunOutModule of the behavior_velocity_planner. The velocity factor is now initialized for the RUN_OUT behavior instead of the ROUTE_OBSTACLE behavior.
* fix(autoware_behavior_velocity_run_out_module): fix clang-diagnostic-unused-lambda-capture (`#9416 <https://github.com/autowarefoundation/autoware.universe/issues/9416>`_)
  fix: clang-diagnostic-unused-lambda-capture
* feat(run_out_module): add tests to run out (`#9222 <https://github.com/autowarefoundation/autoware.universe/issues/9222>`_)
  * WIP add tests for utils and path_utils
  * add tests for utils and fix test path utils
  * dynamic obstacles
  * new tests and add function declarations
  * add points for test of extractObstaclePointsWithinPolygon
  * add state machine tests and other tests for dynamic obstacle
  * remove unused test checks
  * remove unused tests
  * remove unwanted semicolons
  * test
  * add comments
  * solve cpp-check limitation issue by removing namespaces
  ---------
* fix(run_out): output velocity factor (`#9319 <https://github.com/autowarefoundation/autoware.universe/issues/9319>`_)
  * fix(run_out): output velocity factor
  * fix(adapi): subscribe run out velocity factor
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kyoichi Sugahara, M. Fatih Cırıt, Mamoru Sobue, Ryohsuke Mitsudome, Satoshi OTA, Yutaka Kondo, danielsanchezaran, kobayu858

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
* fix(behavior_velocity_planner): align the parameters with launcher (`#8791 <https://github.com/autowarefoundation/autoware.universe/issues/8791>`_)
  parameters in behavior_velocity_planner aligned
* fix(autoware_behavior_velocity_run_out_module): fix unusedFunction (`#8779 <https://github.com/autowarefoundation/autoware.universe/issues/8779>`_)
  fix:unusedFunction
* fix(autoware_behavior_velocity_run_out_module): fix unusedFunction (`#8669 <https://github.com/autowarefoundation/autoware.universe/issues/8669>`_)
  fix:unusedFunction
* fix(behavior_velocity_planner): fix cppcheck warnings of virtualCallInConstructor (`#8376 <https://github.com/autowarefoundation/autoware.universe/issues/8376>`_)
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
* fix(behavior_velocity_planner): fix cppcheck warnings of functionStatic (`#8262 <https://github.com/autowarefoundation/autoware.universe/issues/8262>`_)
  fix: deal with functionStatic warnings
* fix(autoware_behavior_velocity_run_out_module): fix functionConst (`#8284 <https://github.com/autowarefoundation/autoware.universe/issues/8284>`_)
  fix:functionConst
* fix(autoware_behavior_velocity_run_out_module): fix passedByValue (`#8215 <https://github.com/autowarefoundation/autoware.universe/issues/8215>`_)
  * fix:passedByValue
  * fix:passedByValue
  * fix:passedByValue
  ---------
* fix(autoware_behavior_velocity_run_out_module): fix constParameterReference (`#8050 <https://github.com/autowarefoundation/autoware.universe/issues/8050>`_)
  fix:constParameterReference
* fix(behavior_path_planner, behavior_velocity_planner): fix redefinition errors (`#7688 <https://github.com/autowarefoundation/autoware.universe/issues/7688>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* chore(behavior_velocity_planner): fix CODEOWNERS and page links (`#7534 <https://github.com/autowarefoundation/autoware.universe/issues/7534>`_)
  * chore(behavior_velocity_planner): fix CODEOWNERS and page links
  * fix: fix page link
  ---------
* chore(behavior_velocity_planner): move packages (`#7526 <https://github.com/autowarefoundation/autoware.universe/issues/7526>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, Zhe Shen, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------

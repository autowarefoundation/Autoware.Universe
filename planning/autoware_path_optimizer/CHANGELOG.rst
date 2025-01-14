^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_path_optimizer
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
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* refactor(global_parameter_loader): prefix package and namespace with autoware (`#9303 <https://github.com/autowarefoundation/autoware.universe/issues/9303>`_)
* docs: update the list styles (`#9555 <https://github.com/autowarefoundation/autoware.universe/issues/9555>`_)
* fix(path_optimizer): solve issue with time keeper (`#9483 <https://github.com/autowarefoundation/autoware.universe/issues/9483>`_)
  solve issue with time keeper
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(fake_test_node): prefix package and namespace with autoware (`#9249 <https://github.com/autowarefoundation/autoware.universe/issues/9249>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, danielsanchezaran

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
* refactor(osqp_interface): added autoware prefix to osqp_interface (`#8958 <https://github.com/autowarefoundation/autoware.universe/issues/8958>`_)
* chore(path_optimizer): add warn msg for exceptional behavior (`#9033 <https://github.com/autowarefoundation/autoware.universe/issues/9033>`_)
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(motion_planning): align the parameters with launcher (`#8792 <https://github.com/autowarefoundation/autoware.universe/issues/8792>`_)
  parameters in motion_planning aligned
* fix(autoware_path_optimizer): fix unusedFunction (`#8644 <https://github.com/autowarefoundation/autoware.universe/issues/8644>`_)
  fix:unusedFunction
* fix(autoware_path_optimizer): fix unreadVariable (`#8361 <https://github.com/autowarefoundation/autoware.universe/issues/8361>`_)
  * fix:unreadVariable
  * fix:unreadVariable
  ---------
* fix(autoware_path_optimizer): fix passedByValue (`#8190 <https://github.com/autowarefoundation/autoware.universe/issues/8190>`_)
  fix:passedByValue
* fix(path_optimizer): revert the feature of publishing processing time (`#8160 <https://github.com/autowarefoundation/autoware.universe/issues/8160>`_)
* feat(autoware_universe_utils): add TimeKeeper to track function's processing time (`#7754 <https://github.com/autowarefoundation/autoware.universe/issues/7754>`_)
* fix(autoware_path_optimizer): fix redundantContinue warnings (`#7577 <https://github.com/autowarefoundation/autoware.universe/issues/7577>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(path_optimizer): rename to include/autoware/{package_name} (`#7529 <https://github.com/autowarefoundation/autoware.universe/issues/7529>`_)
* refactor(test_utils): move to common folder (`#7158 <https://github.com/autowarefoundation/autoware.universe/issues/7158>`_)
  * Move autoware planning test manager to autoware namespace
  * fix package share directory for behavior path planner
  * renaming files and directory
  * rename variables that has planning_test_utils in its name.
  * use autoware namespace for test utils
  * move folder to common
  * update .pages file
  * fix test error
  * removed obstacle velocity limiter test artifact
  * remove namespace from planning validator, it has using keyword
  ---------
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
* refactor(path_optimizer, velocity_smoother)!: prefix package and namespace with autoware (`#7354 <https://github.com/autowarefoundation/autoware.universe/issues/7354>`_)
  * chore(autoware_velocity_smoother): update namespace
  * chore(autoware_path_optimizer): update namespace
  ---------
* feat!: replace autoware_auto_msgs with autoware_msgs for planning modules (`#7246 <https://github.com/autowarefoundation/autoware.universe/issues/7246>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* chore(autoware_velocity_smoother, autoware_path_optimizer): rename packages (`#7202 <https://github.com/autowarefoundation/autoware.universe/issues/7202>`_)
  * chore(autoware_path_optimizer): rename package and namespace
  * chore(autoware_static_centerline_generator): rename package and namespace
  * chore: update module name
  * chore(autoware_velocity_smoother): rename package and namespace
  * chore(tier4_planning_launch): update module name
  * chore: update module name
  * fix: test
  * fix: test
  * fix: test
  ---------
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Zhe Shen, Zulfaqar Azmi, kobayu858

0.26.0 (2024-04-03)
-------------------

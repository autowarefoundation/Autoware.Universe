^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_surround_obstacle_checker
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(surround_obstacle_checker)!: remove stop reason (`#9450 <https://github.com/autowarefoundation/autoware.universe/issues/9450>`_)
  fix(surround_obstacle_checker): remove stop reason
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_surround_obstacle_checker): fix clang-diagnostic-unused-private-field (`#9399 <https://github.com/autowarefoundation/autoware.universe/issues/9399>`_)
  * fix: clang-diagnostic-unused-private-field
  * refactor: fmt
  * refactor: fmt
  * refactor: fmt
  * fix: hpp
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Satoshi OTA, Yutaka Kondo, awf-autoware-bot[bot], kobayu858

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
* feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): add processing time pub. (`#9065 <https://github.com/autowarefoundation/autoware.universe/issues/9065>`_)
  * feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): Add: processing_time_pub
  * fix: pre-commit
  * feat(costmap_generator): fix: No output when not Active.
  * fix: clang-format
  * Re: fix: clang-format
  ---------
* test(surround_obstacle_checker): add unit tests (`#9039 <https://github.com/autowarefoundation/autoware.universe/issues/9039>`_)
  * refactor: isStopRequired
  * test: write test for isStopRequired
  * refactor: use universe utils
  * fix: shutdown
  ---------
* fix(other_planning_packages): align the parameters with launcher (`#8793 <https://github.com/autowarefoundation/autoware.universe/issues/8793>`_)
  * parameters in planning/others aligned
  * update json
  ---------
* fix(autoware_surround_obstacle_checker): fix unusedFunction (`#8774 <https://github.com/autowarefoundation/autoware.universe/issues/8774>`_)
  fix:unusedFunction
* feat(surround_obstacle_checker): integrate generate_parameter_library (`#8719 <https://github.com/autowarefoundation/autoware.universe/issues/8719>`_)
  * add generate_parameter_library to package
  * add parameter file generator script
  * use mapped parameters
  * integrate generate_parameter_library
  * style(pre-commit): autofix
  * check to use dynamic object
  * remove default values
  * fix variable shadowing
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_surround_obstacle_checker): fix passedByValue (`#8206 <https://github.com/autowarefoundation/autoware.universe/issues/8206>`_)
  fix:passedByValue
* fix(autoware_surround_obstacle_checker): fix constVariableReference (`#8059 <https://github.com/autowarefoundation/autoware.universe/issues/8059>`_)
  fix:constVariableReference
* fix(autoware_surround_obstacle_checker): fix funcArgNamesDifferent (`#8020 <https://github.com/autowarefoundation/autoware.universe/issues/8020>`_)
  fix:funcArgNamesDifferent
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(surround_obstacle_checker): remove include directory (`#7507 <https://github.com/autowarefoundation/autoware.universe/issues/7507>`_)
  * feat(surround_obstacle_checker): remove include directory
  * fix
  * fix
  ---------
* fix(planning): set single depth sensor data qos for pointlcoud polling subscribers (`#7490 <https://github.com/autowarefoundation/autoware.universe/issues/7490>`_)
  set single depth sensor data qos for pointlcoud polling subscribers
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
* refactor(surround_obstacle_checker)!: prefix package and namespace with autoware (`#7298 <https://github.com/autowarefoundation/autoware.universe/issues/7298>`_)
  * fix(autoware_surround_obstacle_checker): rename
  * fix(autoware_surround_obstacle_checker): rename header
  * fix(launch): update package name
  ---------
* Contributors: Kazunori-Nakajima, Kosuke Takeuchi, Mitsuhiro Sakamoto, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, Zhe Shen, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

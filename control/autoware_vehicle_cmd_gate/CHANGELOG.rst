^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_vehicle_cmd_gate
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - control (`#9565 <https://github.com/autowarefoundation/autoware.universe/issues/9565>`_)
* fix(autoware_vehicle_cmd_gate): remove unused variable (`#9377 <https://github.com/autowarefoundation/autoware.universe/issues/9377>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(control): missing dependency in control components (`#9073 <https://github.com/autowarefoundation/autoware.universe/issues/9073>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(vehicle_cmd_gate): fix processing time measurement (`#9260 <https://github.com/autowarefoundation/autoware.universe/issues/9260>`_)
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Maxime CLEMENT, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo, ぐるぐる

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(control): missing dependency in control components (`#9073 <https://github.com/autowarefoundation/autoware.universe/issues/9073>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(vehicle_cmd_gate): fix processing time measurement (`#9260 <https://github.com/autowarefoundation/autoware.universe/issues/9260>`_)
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Maxime CLEMENT, Yutaka Kondo, ぐるぐる

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(component_interface_specs): prefix package and namespace with autoware (`#9094 <https://github.com/autowarefoundation/autoware.universe/issues/9094>`_)
* feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): add processing time pub. (`#9065 <https://github.com/autowarefoundation/autoware.universe/issues/9065>`_)
  * feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): Add: processing_time_pub
  * fix: pre-commit
  * feat(costmap_generator): fix: No output when not Active.
  * fix: clang-format
  * Re: fix: clang-format
  ---------
* fix(control): align the parameters with launcher (`#8789 <https://github.com/autowarefoundation/autoware.universe/issues/8789>`_)
  align the control parameters
* chore(vehicle_cmd_gate): delete deprecated parameters (`#8537 <https://github.com/autowarefoundation/autoware.universe/issues/8537>`_)
  delete deprecated params in vehicle_cmd_gate.param.yaml
* fix(autoware_vehicle_cmd_gate): fix unusedFunction (`#8556 <https://github.com/autowarefoundation/autoware.universe/issues/8556>`_)
  fix:unusedFunction
* refactor(vehicle_cmd_gate): use smaller class for filter command (`#8518 <https://github.com/autowarefoundation/autoware.universe/issues/8518>`_)
* fix(autoware_vehicle_cmd_gate): fix unreadVariable (`#8351 <https://github.com/autowarefoundation/autoware.universe/issues/8351>`_)
* feat(autoware_vehicle_cmd_gate):  accept same topic unless mode change occurs (`#8479 <https://github.com/autowarefoundation/autoware.universe/issues/8479>`_)
  * add prev_commands\_ and check cmd's time stamp
  * add timestamp when is_engaged is false
  * style(pre-commit): autofix
  * add initialization for hazard_light timestamp in Commands
  * style(pre-commit): autofix
  * update README.md
  * style(pre-commit): autofix
  * fix typo
  * fix(autoware_vehicle_cmd_gate): rename the function that checks the continuity of topics
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * style(pre-commit): autofix
  * feat(autoware_vehicle_cmd_gate): check continuity using shared_ptr
  * feat(autoware_vehicle_cmd_gate): add INFO message for topics  that are not receiving
  * fix template function to pass build-and-test-differential
  * fix(autoware_vehicle_cmd_gate): add #include <string>  according to pre-commit.ci
  * fix(vehicle_cmd_gate) add underscores to member variable names for consistency
  * style(pre-commit): autofix
  * feat(vehicle_cmd_gate): accept same topic unless mode change occurs
  * feat(vehicle_cmd_gate): add default topic_name to getContinuousTopic function
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* feat(autoware_vehicle_cmd_gate): check the timestamp of input topics to avoid using old topics (`#8084 <https://github.com/autowarefoundation/autoware.universe/issues/8084>`_)
  * add prev_commands\_ and check cmd's time stamp
  * add timestamp when is_engaged is false
  * style(pre-commit): autofix
  * add initialization for hazard_light timestamp in Commands
  * style(pre-commit): autofix
  * update README.md
  * style(pre-commit): autofix
  * fix typo
  * fix(autoware_vehicle_cmd_gate): rename the function that checks the continuity of topics
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * style(pre-commit): autofix
  * feat(autoware_vehicle_cmd_gate): check continuity using shared_ptr
  * feat(autoware_vehicle_cmd_gate): add INFO message for topics  that are not receiving
  * fix template function to pass build-and-test-differential
  * fix(autoware_vehicle_cmd_gate): add #include <string>  according to pre-commit.ci
  * fix(vehicle_cmd_gate) add underscores to member variable names for consistency
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* fix(autoware_vehicle_cmd_gate): fix functionConst (`#8253 <https://github.com/autowarefoundation/autoware.universe/issues/8253>`_)
  fix: functionConst
* fix(autoware_vehicle_cmd_gate): fix cppcheck warning of functionStatic (`#8260 <https://github.com/autowarefoundation/autoware.universe/issues/8260>`_)
  * fix: deal with functionStatic warnings
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_vehicle_cmd_gate): fix uninitMemberVar (`#8339 <https://github.com/autowarefoundation/autoware.universe/issues/8339>`_)
  fix:uninitMemberVar
* fix(autoware_vehicle_cmd_gate): fix passedByValue (`#8243 <https://github.com/autowarefoundation/autoware.universe/issues/8243>`_)
  fix: passedByValue
* fix(autoware_vehicle_cmd_gate): fix funcArgNamesDifferent (`#8006 <https://github.com/autowarefoundation/autoware.universe/issues/8006>`_)
  fix:funcArgNamesDifferent
* refactor(vehicle_cmd_gate)!: delete rate limit skipping function for vehicle departure (`#7720 <https://github.com/autowarefoundation/autoware.universe/issues/7720>`_)
  * delete a fucntion block. More appropriate function can be achieved by the parameter settings.
  * add notation to readme
  ---------
* fix(vehicle_cmd_gate): colcon test failure due to heavy process (`#7678 <https://github.com/autowarefoundation/autoware.universe/issues/7678>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(vehicle_cmd_gate): put back subscriber rather than using polling subsriber (`#7500 <https://github.com/autowarefoundation/autoware.universe/issues/7500>`_)
  put back polling subscribers to subscribers in neccesary cases
* fix(vehicle_cmd_gate): fix unnecessary modification (`#7488 <https://github.com/autowarefoundation/autoware.universe/issues/7488>`_)
  fix onGateMode function
* feat(vehicle_cmd_gate): use polling subscriber (`#7418 <https://github.com/autowarefoundation/autoware.universe/issues/7418>`_)
  * change to polling subscriber
  * fix
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
* chore(vehicle_cmd_gate): add prefix autoware\_ to vehicle_cmd_gate (`#7327 <https://github.com/autowarefoundation/autoware.universe/issues/7327>`_)
  * add prefix autoware\_ to vehicle_cmd_gate package
  * fix
  * fix include guard
  * fix pre-commit
  ---------
* Contributors: Autumn60, Esteve Fernandez, Go Sakayori, Hayate TOBA, Kazunori-Nakajima, Kosuke Takeuchi, SHtokuda, Satoshi OTA, Takamasa Horibe, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Zhe Shen, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------

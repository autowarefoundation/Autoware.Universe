^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_mpc_lateral_controller
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_mpc_lateral_controller): fix clang-tidy errors (`#9436 <https://github.com/autowarefoundation/autoware.universe/issues/9436>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(mpc_lateral_controller): suppress rclcpp_warning/error (`#9382 <https://github.com/autowarefoundation/autoware.universe/issues/9382>`_)
  * feat(mpc_lateral_controller): suppress rclcpp_warning/error
  * fix
  * fix test
  ---------
* fix(autoware_mpc_lateral_controller): fix variableScope (`#9390 <https://github.com/autowarefoundation/autoware.universe/issues/9390>`_)
* feat: suppress warning/error of the empty predicted trajectory by MPC (`#9373 <https://github.com/autowarefoundation/autoware.universe/issues/9373>`_)
* chore(autoware_mpc_lateral_controller): add maintainer (`#9374 <https://github.com/autowarefoundation/autoware.universe/issues/9374>`_)
* feat(trajectory_follower): publsih control horzion (`#8977 <https://github.com/autowarefoundation/autoware.universe/issues/8977>`_)
  * feat(trajectory_follower): publsih control horzion
  * fix typo
  * rename functions and minor refactor
  * add option to enable horizon pub
  * add tests for horizon
  * update docs
  * rename to ~/debug/control_cmd_horizon
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_mpc_lateral_controller): fix bugprone-misplaced-widening-cast (`#9224 <https://github.com/autowarefoundation/autoware.universe/issues/9224>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: consider negative values
  ---------
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(mpc_lateral_controller): correctly resample the MPC trajectory yaws (`#9199 <https://github.com/autowarefoundation/autoware.universe/issues/9199>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, Kyoichi Sugahara, M. Fatih Cırıt, Maxime CLEMENT, Ryohsuke Mitsudome, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, kobayu858

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_mpc_lateral_controller): fix bugprone-misplaced-widening-cast (`#9224 <https://github.com/autowarefoundation/autoware.universe/issues/9224>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: consider negative values
  ---------
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(mpc_lateral_controller): correctly resample the MPC trajectory yaws (`#9199 <https://github.com/autowarefoundation/autoware.universe/issues/9199>`_)
* Contributors: Esteve Fernandez, Maxime CLEMENT, Yutaka Kondo, kobayu858

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(osqp_interface): added autoware prefix to osqp_interface (`#8958 <https://github.com/autowarefoundation/autoware.universe/issues/8958>`_)
* fix(autoware_mpc_lateral_controller): fix calculation method of predicted trajectory (`#9048 <https://github.com/autowarefoundation/autoware.universe/issues/9048>`_)
  * fix(vehicle_model): fix calculation method of predicted trajectory
  ---------
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* chore(mpc_lateral_controller): consistent parameters with autoware_launch (`#8914 <https://github.com/autowarefoundation/autoware.universe/issues/8914>`_)
* chore: remove duplicate line in mpc_lateral_controller.cpp (`#8916 <https://github.com/autowarefoundation/autoware.universe/issues/8916>`_)
  remove duplicate line in mpc_lateral_controller.cpp
* feat(autoware_mpc_lateral_controller): add predicted trajectory acconts for input delay (`#8436 <https://github.com/autowarefoundation/autoware.universe/issues/8436>`_)
  * feat: enable delayed initial state for predicted trajectory
  * feat: enable debug publishing of predicted and resampled reference trajectories
  ---------
* fix(autoware_mpc_lateral_controller): fix cppcheck warnings (`#8149 <https://github.com/autowarefoundation/autoware.universe/issues/8149>`_)
  * fix(autoware_mpc_lateral_controller): fix cppcheck warnings
  * Update control/autoware_mpc_lateral_controller/src/lowpass_filter.cpp
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  ---------
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* fix(autoware_mpc_lateral_controller): add timestamp and frame ID to published trajectory (`#8164 <https://github.com/autowarefoundation/autoware.universe/issues/8164>`_)
  add timestamp and frame ID to published trajectory
* fix(controller): revival of dry steering (`#7903 <https://github.com/autowarefoundation/autoware.universe/issues/7903>`_)
  * Revert "fix(autoware_mpc_lateral_controller): delete the zero speed constraint (`#7673 <https://github.com/autowarefoundation/autoware.universe/issues/7673>`_)"
  This reverts commit 69258bd92cb8a0ff8320df9b2302db72975e027f.
  * dry steering
  * add comments
  * add minor fix and modify unit test for dry steering
  ---------
* fix(autoware_mpc_lateral_controller): delete the zero speed constraint (`#7673 <https://github.com/autowarefoundation/autoware.universe/issues/7673>`_)
  * delete steer rate limit when vel = 0
  * delete unnecessary variable
  * pre-commit
  ---------
* fix(autoware_mpc_lateral_controller): relax the steering rate constraint at zero speed (`#7581 <https://github.com/autowarefoundation/autoware.universe/issues/7581>`_)
  * constraint for zero velocity updated
  * correct the comment
  ---------
* fix(autoware_mpc_lateral_controller): fix duplicateExpression warning (`#7542 <https://github.com/autowarefoundation/autoware.universe/issues/7542>`_)
  * fix(autoware_mpc_lateral_controller): fix duplicateExpression warning
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_mpc_lateral_controller): fix duplicateAssignExpression warning (`#7572 <https://github.com/autowarefoundation/autoware.universe/issues/7572>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(mpc_lateral_controller): align the MPC steering angle when the car is controlled manually. (`#7109 <https://github.com/autowarefoundation/autoware.universe/issues/7109>`_)
  * align the MPC steering angle when the car is controlled manually.
  * update the condition for is_driving_manually
  * STOP mode included
  * comment the is_driving_manually
  * align the steering outside (after) the solver.
  * use the flag input_data.current_operation_mode.is_autoware_control_enabled
  * correct a typo
  * correct the under control condition check
  * undo the space delete
  * unchange the unrelevant line
  * pre-commit
  ---------
* feat(mpc_lateral_controller): signal a MRM when MPC fails. (`#7016 <https://github.com/autowarefoundation/autoware.universe/issues/7016>`_)
  * mpc fail checker diagnostic added
  * fix some scope issues
  * member attribute added.
  * shared pointer added.
  * member attribute (diag_updater\_) added
  * dependency added.
  * implementation of the MpcLateralController corrected!
  * typo in comment corrected!
  * member method argument corrected
  * delete unnecessary reference mark
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * rebase
  * correct the include
  * pre-commit
  ---------
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(control)!: refactor directory structures of the trajectory followers (`#7521 <https://github.com/autowarefoundation/autoware.universe/issues/7521>`_)
  * control_traj
  * add follower_node
  * fix
  ---------
* refactor(trajectory_follower_node): trajectory follower node add autoware prefix (`#7344 <https://github.com/autowarefoundation/autoware.universe/issues/7344>`_)
  * rename trajectory follower node package
  * update dependencies, launch files, and README files
  * fix formats
  * remove autoware\_ prefix from launch arg option
  ---------
* refactor(trajectory_follower_base): trajectory follower base add autoware prefix (`#7343 <https://github.com/autowarefoundation/autoware.universe/issues/7343>`_)
  * rename trajectory follower base package
  * update dependencies and includes
  * fix formats
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
* refactor(mpc_lateral_controller, trajectory_follower_node)!: prefix package and namespace with autoware (`#7306 <https://github.com/autowarefoundation/autoware.universe/issues/7306>`_)
  * add the prefix to the folder
  * named to autoware_mpc_lateral_controller
  * rename the folder in the include
  * correct the package name in xml and CMakeLists
  * correct the namespace and include
  * change namespace and include in src/
  * change namespace and include in test/
  * fix the trajectory_follower_node
  * undo rename to the namespace
  * change the trajectory_follower_node, Controller.drawio.svg, and README.md
  * fixed by pre-commit
  * suppress the unnecessary line length detect
  ---------
* Contributors: Autumn60, Esteve Fernandez, Kosuke Takeuchi, Kyoichi Sugahara, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Zhe Shen, mkquda

0.26.0 (2024-04-03)
-------------------

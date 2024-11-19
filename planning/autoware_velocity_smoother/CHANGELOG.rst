^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_velocity_smoother
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(osqp_interface): added autoware prefix to osqp_interface (`#8958 <https://github.com/autowarefoundation/autoware.universe/issues/8958>`_)
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* perf(motion_velocity_smoother): remove some heavy debug logging (`#8798 <https://github.com/autowarefoundation/autoware.universe/issues/8798>`_)
  remove some heavy logging in the velocity smoother
* fix(autoware_velocity_smoother): fix unusedFunction (`#8649 <https://github.com/autowarefoundation/autoware.universe/issues/8649>`_)
  fix:unusedFunction
* fix(autoware_velocity_smoother): fix variableScope (`#8442 <https://github.com/autowarefoundation/autoware.universe/issues/8442>`_)
  fix:variableScope
* fix(autoware_velocity_smoother): fix unreadVariable (`#8364 <https://github.com/autowarefoundation/autoware.universe/issues/8364>`_)
  fix:unreadVariable
* fix(autoware_velocity_smoother): fix passedByValue (`#8207 <https://github.com/autowarefoundation/autoware.universe/issues/8207>`_)
  * fix:passedByValue
  * fix:clang format
  ---------
* perf(velocity_smoother): not resample debug_trajectories is not used (`#8030 <https://github.com/autowarefoundation/autoware.universe/issues/8030>`_)
  * not resample debug_trajectories if not published
  * update dependant packages
  ---------
* perf(velocity_smoother): use ProxQP for faster optimization (`#8028 <https://github.com/autowarefoundation/autoware.universe/issues/8028>`_)
  * perf(velocity_smoother): use ProxQP for faster optimization
  * consider max_iteration
  * disable warm start
  * fix test
  ---------
* fix(velocity_smoother, obstacle_cruise_planner ): float type of processing time was wrong (`#8161 <https://github.com/autowarefoundation/autoware.universe/issues/8161>`_)
  fix(velocity_smoother): float type of processing time was wrong
* fix(autoware_velocity_smoother): fix constVariableReference (`#8060 <https://github.com/autowarefoundation/autoware.universe/issues/8060>`_)
  fix:constVariableReference
* fix(autoware_velocity_smoother): fix constParameterReference (`#8043 <https://github.com/autowarefoundation/autoware.universe/issues/8043>`_)
  fix:constParameterReference
* chore(velocity_smoother): add maintainer  (`#8121 <https://github.com/autowarefoundation/autoware.universe/issues/8121>`_)
  add maintainer
* refactor(autoware_universe_utils): changed the API to be more intuitive and added documentation (`#7443 <https://github.com/autowarefoundation/autoware.universe/issues/7443>`_)
  * refactor(tier4_autoware_utils): Changed the API to be more intuitive and added documentation.
  * use raw shared ptr in PollingPolicy::NEWEST
  * update
  * fix
  * Update evaluator/autoware_control_evaluator/include/autoware/control_evaluator/control_evaluator_node.hpp
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
  ---------
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
* fix(autoware_velocity_smoother): fix shadowVariablefix:shadowVariable (`#7950 <https://github.com/autowarefoundation/autoware.universe/issues/7950>`_)
  fix:shadowVariable
* feat(velocity_smoother): add time_keeper (`#8026 <https://github.com/autowarefoundation/autoware.universe/issues/8026>`_)
* refactor(velocity_smoother): change method to take data for external velocity (`#7810 <https://github.com/autowarefoundation/autoware.universe/issues/7810>`_)
  refactor external velocity
* fix(autoware_velocity_smoother): fix duplicateBreak warning (`#7699 <https://github.com/autowarefoundation/autoware.universe/issues/7699>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(autoware_velocity_smoother): fix unusedVariable warning (`#7585 <https://github.com/autowarefoundation/autoware.universe/issues/7585>`_)
  fix unusedVariable warning
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(velocity_smoother): rename to include/autoware/{package_name} (`#7533 <https://github.com/autowarefoundation/autoware.universe/issues/7533>`_)
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
* feat(autoware_velocity_smoother): use polling subscriber (`#7216 <https://github.com/autowarefoundation/autoware.universe/issues/7216>`_)
  feat(motion_velocity_smoother): use polling subscriber
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, Go Sakayori, Kosuke Takeuchi, Maxime CLEMENT, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yukinari Hisaki, Yutaka Kondo, Zulfaqar Azmi, kobayu858

0.26.0 (2024-04-03)
-------------------

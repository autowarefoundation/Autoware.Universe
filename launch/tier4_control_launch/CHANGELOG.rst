^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_control_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix: autoware_glog_compontnt (`#9586 <https://github.com/autowarefoundation/autoware.universe/issues/9586>`_)
  Fixed autoware_glog_compontnt
* refactor(glog_component): prefix package and namespace with autoware (`#9302 <https://github.com/autowarefoundation/autoware.universe/issues/9302>`_)
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* feat(control_evaluator, tier4_control_launch): add a trigger to choice whether to output metrics to log folder (`#9478 <https://github.com/autowarefoundation/autoware.universe/issues/9478>`_)
  * refactor and add output_metrics. a bug existing when psim.
  * refactored launch file.
  * output description
  * add parm to launch file.
  * move output_metrics from param config to launch file.
  * move output_metrics from config to launch.xml
  * fix unit test bug.
  * fix test bug again.
  * Update evaluator/autoware_control_evaluator/include/autoware/control_evaluator/control_evaluator_node.hpp
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(tier4_control_launch): use preset.yaml to control which modules to launch for control modules (`#9351 <https://github.com/autowarefoundation/autoware.universe/issues/9351>`_)
  * update contro.launch for preset.xml
  * update options.
  * fix bug.
  * rename to enable\_*
  * check group.
  * space.
  * reduce num of load_composable_node.
  * tmp save.
  * tmp save.
  * splite control modules' launch.
  * final version
  * remove on/off option for shift decider, vehicle cmd gate, and operation mode transition manager
  * pre-commit
  ---------
* fix(collision_detector): skip process when odometry is not published (`#9308 <https://github.com/autowarefoundation/autoware.universe/issues/9308>`_)
  * subscribe odometry
  * fix precommit
  * remove unnecessary log info
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(aeb): set global param to override autoware state check (`#9263 <https://github.com/autowarefoundation/autoware.universe/issues/9263>`_)
  * set global param to override autoware state check
  * change variable to be more general
  * add comment
  * move param to control component launch
  * change param name to be more straightforward
  ---------
* feat(control_launch): add collision detector in launch (`#9214 <https://github.com/autowarefoundation/autoware.universe/issues/9214>`_)
  add collision detector in launch
* Contributors: Esteve Fernandez, Fumiya Watanabe, Go Sakayori, Kem (TiankuiXian), Ryohsuke Mitsudome, SakodaShintaro, Yutaka Kondo, danielsanchezaran

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(collision_detector): skip process when odometry is not published (`#9308 <https://github.com/autowarefoundation/autoware.universe/issues/9308>`_)
  * subscribe odometry
  * fix precommit
  * remove unnecessary log info
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(aeb): set global param to override autoware state check (`#9263 <https://github.com/autowarefoundation/autoware.universe/issues/9263>`_)
  * set global param to override autoware state check
  * change variable to be more general
  * add comment
  * move param to control component launch
  * change param name to be more straightforward
  ---------
* feat(control_launch): add collision detector in launch (`#9214 <https://github.com/autowarefoundation/autoware.universe/issues/9214>`_)
  add collision detector in launch
* Contributors: Esteve Fernandez, Go Sakayori, Yutaka Kondo, danielsanchezaran

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(obstacle_collision_checker): move to autoware namespace (`#9047 <https://github.com/autowarefoundation/autoware.universe/issues/9047>`_)
* fix(tier4_control_launch): fix aeb input predicted object topic name (`#8874 <https://github.com/autowarefoundation/autoware.universe/issues/8874>`_)
  fix aeb input predicted object topic
* feat(autonomous_emergency_braking): add some tests to aeb (`#8126 <https://github.com/autowarefoundation/autoware.universe/issues/8126>`_)
  * add initial tests
  * add more tests
  * more tests
  * WIP add publishing and test subscription
  * add more tests
  * fix lint cmake
  * WIP tf topic
  * Revert "WIP tf topic"
  This reverts commit b5ef11b499e719b2cdbe0464bd7de7778de54e76.
  * add path crop test
  * add test for transform object
  * add briefs
  * delete repeated test
  ---------
* feat(evalautor): rename evaluator diag topics (`#8152 <https://github.com/autowarefoundation/autoware.universe/issues/8152>`_)
  * feat(evalautor): rename evaluator diag topics
  * perception
  ---------
* fix(control_launch): fix control launch control_evaluator plugin name (`#7846 <https://github.com/autowarefoundation/autoware.universe/issues/7846>`_)
  fix control evaluator plugin name
* refactor(tier4_control_launch): replace python launch with xml (`#7682 <https://github.com/autowarefoundation/autoware.universe/issues/7682>`_)
  * refactor: add xml version of control launch
  * refactor: remove python version of control launch
  * set default value of trajectory_follower_mode
  * fix file extension to .py
  * fix node name
  * add lanelet info to the metrics
  * style(pre-commit): autofix
  * Update launch/tier4_control_launch/launch/control.launch.xml
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: Kotaro Yoshimoto <pythagora.yoshimoto@gmail.com>
* refactor(control_evaluator): use class naming standard and use remapped param name (`#7782 <https://github.com/autowarefoundation/autoware.universe/issues/7782>`_)
  use class naming standard and use remapped param name
* feat(control_evaluator): add lanelet info to the metrics (`#7765 <https://github.com/autowarefoundation/autoware.universe/issues/7765>`_)
  * add route handler
  * add lanelet info to diagnostic
  * add const
  * add kinematic state info
  * clean
  * remove unusde subscriptions
  * clean
  * add shoulder lanelets
  * fix includes
  ---------
* fix(smart_mpc_trajectory_folower): fix running by adding control_state and changing msg/package_name (`#7666 <https://github.com/autowarefoundation/autoware.universe/issues/7666>`_)
* feat(autonomous_emergency_braking): add predicted object support for aeb (`#7548 <https://github.com/autowarefoundation/autoware.universe/issues/7548>`_)
  * add polling sub to predicted objects
  * WIP requires changing path frame to map
  * add parameters and reuse predicted obj speed
  * introduce early break to reduce computation time
  * resolve merge conflicts
  * fix guard
  * remove unused declaration
  * fix include
  * fix include issues
  * remove inline
  * delete unused dependencies
  * add utils.cpp
  * remove _ for non member variable
  ---------
* feat(control_evaluator): rename to include/autoware/{package_name} (`#7520 <https://github.com/autowarefoundation/autoware.universe/issues/7520>`_)
  * feat(control_evaluator): rename to include/autoware/{package_name}
  * fix
  ---------
* feat(diagnostic_converter): fix output metrics topic name and add to converter (`#7495 <https://github.com/autowarefoundation/autoware.universe/issues/7495>`_)
* feat(control_evaluator): add deviation metrics and queue for diagnostics (`#7484 <https://github.com/autowarefoundation/autoware.universe/issues/7484>`_)
* refactor(operation_mode_transition_manager): prefix package and namespace with autoware\_ (`#7291 <https://github.com/autowarefoundation/autoware.universe/issues/7291>`_)
  * RT1-6682 add prefix package and namespace with autoware\_
  * RT1-6682 fix package's description
  ---------
* refactor(trajectory_follower_node): trajectory follower node add autoware prefix (`#7344 <https://github.com/autowarefoundation/autoware.universe/issues/7344>`_)
  * rename trajectory follower node package
  * update dependencies, launch files, and README files
  * fix formats
  * remove autoware\_ prefix from launch arg option
  ---------
* refactor(shift_decider): prefix package and namespace with autoware\_ (`#7310 <https://github.com/autowarefoundation/autoware.universe/issues/7310>`_)
  * RT1-6684 add autoware prefix and namespace
  * RT1-6684 Revert svg
  This reverts commit 4e0569e4796ab432c734905fb7f2106779575e29.
  ---------
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* fix(tier4_control_launch, crosswalk_traffic_light_estimator): fix a mistake when adding prefixes (`#7423 <https://github.com/autowarefoundation/autoware.universe/issues/7423>`_)
  Fixed a mistake when adding prefixes
* refactor(external cmd converter)!: add autoware\_ prefix (`#7361 <https://github.com/autowarefoundation/autoware.universe/issues/7361>`_)
  * add prefix to the code
  * rename
  * fix
  * fix
  * fix
  * Update .github/CODEOWNERS
  ---------
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* refactor(control_validator)!: prefix package and namespace with autoware (`#7304 <https://github.com/autowarefoundation/autoware.universe/issues/7304>`_)
  * rename folders
  * rename add prefix
  * change param path
  * fix pluggin problem
  * fix extra prefixes
  * change back launchers
  * add namespace to address conflict
  * delete stubborn file
  ---------
* refactor(external_cmd_selector): prefix package and namespace with au… (`#7384 <https://github.com/autowarefoundation/autoware.universe/issues/7384>`_)
  refactor(external_cmd_selector): prefix package and namespace with autoware\_
* chore(vehicle_cmd_gate): add prefix autoware\_ to vehicle_cmd_gate (`#7327 <https://github.com/autowarefoundation/autoware.universe/issues/7327>`_)
  * add prefix autoware\_ to vehicle_cmd_gate package
  * fix
  * fix include guard
  * fix pre-commit
  ---------
* feat(autonomous_emergency_braking): prefix package and namespace with autoware\_ (`#7294 <https://github.com/autowarefoundation/autoware.universe/issues/7294>`_)
  * change package name
  * add the prefix
  * change option
  * change back node name
  * eliminate some prefixes that are not required
  * fix node name
  ---------
* chore(smart_mpc_trajectory_follower): add prefix autoware\_ to smart_mpc_trajectory_follower (`#7367 <https://github.com/autowarefoundation/autoware.universe/issues/7367>`_)
  * add prefix
  * fix pre-commit
  ---------
* refactor(lane_departure_checker)!: prefix package and namespace with autoware (`#7325 <https://github.com/autowarefoundation/autoware.universe/issues/7325>`_)
  * add prefix autoware\_ to lane_departure_checker package
  ---------
* feat(smart_mpc_trajectory_follower): add smart_mpc_trajectory_follower (`#6805 <https://github.com/autowarefoundation/autoware.universe/issues/6805>`_)
  * feat(smart_mpc_trajectory_follower): add smart_mpc_trajectory_follower
  * style(pre-commit): autofix
  * modified control.launch.py
  * update README.md
  * Minor changes
  * style(pre-commit): autofix
  * bug fixed
  * update README and add a comment to mpc_param.yaml
  * minor changes
  * add copyright
  * mpc_param.yaml changed
  * add note to README
  * update according to spell check
  * update python_simulator according to spell check
  * update scripts according to spell check
  * update according to spell-check-partial
  * fixed ignored words in spell check
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* feat(control_evaluator): implement a control evaluator (`#6959 <https://github.com/autowarefoundation/autoware.universe/issues/6959>`_)
  * add control evaluator module
  * make the evaluator depend on messages from AEB
  * update output msg
  * delete extra new line
  * update/fix details
  * add a package mantainer
  * Add a timer to maintain a constant rate of msg publishing
  ---------
* revert: "feat(logger_level_configure): make it possible to change level of container logger (`#6823 <https://github.com/autowarefoundation/autoware.universe/issues/6823>`_)" (`#6842 <https://github.com/autowarefoundation/autoware.universe/issues/6842>`_)
  This reverts commit 51b5f830780eb69bd1a7dfe60e295773f394fd8e.
* feat(logger_level_configure): make it possible to change level of container logger (`#6823 <https://github.com/autowarefoundation/autoware.universe/issues/6823>`_)
  * feat(launch): add logging_demo::LoggerConfig into container
  * fix(logger_level_reconfigure_plugin): fix yaml
  * feat(logging_level_configure): add composable node
  ---------
* Contributors: Go Sakayori, Ismet Atabay, Kosuke Takeuchi, Kyoichi Sugahara, Maxime CLEMENT, Mitsuhiro Sakamoto, SakodaShintaro, Satoshi OTA, Takayuki Murooka, Yuki TAKAGI, Yukinari Hisaki, Yutaka Kondo, Zulfaqar Azmi, danielsanchezaran, masayukiaino, mkquda

0.26.0 (2024-04-03)
-------------------
* feat: enable multithreading for the control container (`#6666 <https://github.com/autowarefoundation/autoware.universe/issues/6666>`_)
* feat(pid_longitudinal_controller): add maker for stop reason (`#6579 <https://github.com/autowarefoundation/autoware.universe/issues/6579>`_)
  * feat(pid_longitudinal_controller): add maker for stop reason
  * minor fix
  ---------
* chore(tier4_control_launch): fix control validator name duplication (`#6446 <https://github.com/autowarefoundation/autoware.universe/issues/6446>`_)
* feat(tier4_control_launch): run control_validator out of main control container (`#6435 <https://github.com/autowarefoundation/autoware.universe/issues/6435>`_)
* feat(tier4_control_launch): add launch argument for predicted path checker (`#5186 <https://github.com/autowarefoundation/autoware.universe/issues/5186>`_)
* feat(predicted_path_checker): check predicted trajectory to avoid collisions planning can not handle (`#2528 <https://github.com/autowarefoundation/autoware.universe/issues/2528>`_)
  * feat(predicted_path_checker): check predicted trajectory to avoid collisions planning can not handle (`#2528 <https://github.com/autowarefoundation/autoware.universe/issues/2528>`_)
  * Added pkg to control.launch.py
  ---------
* fix(operation_mode_transition_manager): check trajectory_follower_cmd for engage condition (`#5038 <https://github.com/autowarefoundation/autoware.universe/issues/5038>`_)
* feat(glog): add glog in planning and control modules (`#4714 <https://github.com/autowarefoundation/autoware.universe/issues/4714>`_)
  * feat(glog): add glog component
  * formatting
  * remove namespace
  * remove license
  * Update launch/tier4_planning_launch/launch/scenario_planning/lane_driving/motion_planning/motion_planning.launch.py
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update launch/tier4_planning_launch/launch/scenario_planning/lane_driving/behavior_planning/behavior_planning.launch.py
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update common/glog_component/CMakeLists.txt
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update launch/tier4_control_launch/launch/control.launch.py
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * add copyright
  ---------
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* feat(control_validator): measure predicted path deviation from trajectory (`#4549 <https://github.com/autowarefoundation/autoware.universe/issues/4549>`_)
  * add feature for getting predicted path deviation from trajectory
  * fix for build success
  * fix topic name
  * temp
  * temp
  * cut off extra length on predicted path
  * cut off extra length on predicted path
  * style(pre-commit): autofix
  * minor refactor
  * change function name
  * add control validator
  * style(pre-commit): autofix
  * add max_deviation calculation
  * refactor
  * style(pre-commit): autofix
  * update launch
  * style(pre-commit): autofix
  * change maintainer
  * refactor
  * style(pre-commit): autofix
  * feat(dynamic_avoidance): object polygon based drivable area generation (`#4598 <https://github.com/autowarefoundation/autoware.universe/issues/4598>`_)
  * update
  * update README
  * fix typo
  * apply clang-tidy check
  * Update control/control_validator/include/control_validator/utils.hpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * remove debug code
  * add maintainer
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* feat(shift_decider): send current gear if the autoware state is not driving (`#3684 <https://github.com/autowarefoundation/autoware.universe/issues/3684>`_)
* feat(vehicle_cmd_gate):  do not send current gear if autoware is not engaged (`#3683 <https://github.com/autowarefoundation/autoware.universe/issues/3683>`_)
  This reverts commit be3138545d6814a684a314a7dbf1ffb450f90970.
* style: fix typos (`#3617 <https://github.com/autowarefoundation/autoware.universe/issues/3617>`_)
  * style: fix typos in documents
  * style: fix typos in package.xml
  * style: fix typos in launch files
  * style: fix typos in comments
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(control_launch): add necessary parameter (`#3235 <https://github.com/autowarefoundation/autoware.universe/issues/3235>`_)
* feat(tier4_control_launch): add check_external_emergency_heartbeat option (`#3079 <https://github.com/autowarefoundation/autoware.universe/issues/3079>`_)
* feat(control): add autonomous emergency braking module (`#2793 <https://github.com/autowarefoundation/autoware.universe/issues/2793>`_)
* feat(vehicle_cmd_gate): enable filter with actual steer in manual mode (`#2717 <https://github.com/autowarefoundation/autoware.universe/issues/2717>`_)
  * feature(vehicle_cmd_gate): enable filter with actual steer in manual mode
  * update parameters based on experiment
  * update launch
  * update param
  ---------
* feat(longitudinal_controller): skip integral in manual mode (`#2619 <https://github.com/autowarefoundation/autoware.universe/issues/2619>`_)
  * feat(longitudinal_controller): skip integral in manual mode
  * change control_mode to operation_mode
  * fix test
* chore(control_launch): add maintainer (`#2758 <https://github.com/autowarefoundation/autoware.universe/issues/2758>`_)
* feat(vehicle_cmd_gate): send current gear if autoware is not engaged (`#2555 <https://github.com/autowarefoundation/autoware.universe/issues/2555>`_)
  * feat(vehicle_cmd_gate): send current gear if autoware is not engaged
  * ci(pre-commit): autofix
  * add topic map to launch file
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(tier4_control_launch): remove parameter definition in control.launch.py (`#2585 <https://github.com/autowarefoundation/autoware.universe/issues/2585>`_)
  * refactor trajectory_follower_node's param
  * organize parameter definition in control_launch
  * fix typo
  * fix failed test
* feat(trajectory_follower): seperate lat lon controller packages (`#2580 <https://github.com/autowarefoundation/autoware.universe/issues/2580>`_)
  * feat(trajectory_follower): seperate controller implementation packages
  * update doc
  * fix doc
  * fix test
  * rename: mpc_follower -> mpc
  * rename to trajectory_follower_base, trajectory_follower_node
  * fix doc
  * remove unnecessary change
* feat(tier4_control_launch): remove configs and move to autoware_launch  (`#2544 <https://github.com/autowarefoundation/autoware.universe/issues/2544>`_)
  * feat(tier4_control_launch): remove configs and move to autoware_launch
  * remove config
  * Update launch/tier4_control_launch/README.md
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  Co-authored-by: kminoda <koji.minoda@tier4.jp>
* fix(tier4_control_launch): add parameter about nearest search (`#2542 <https://github.com/autowarefoundation/autoware.universe/issues/2542>`_)
* feat(trajectory_follower): extend mpc trajectory for terminal yaw (`#2447 <https://github.com/autowarefoundation/autoware.universe/issues/2447>`_)
  * feat(trajectory_follower): extend mpc trajectory for terminal yaw
  * make mpc min vel param
  * add mpc extended point after smoothing
  * Revert "make mpc min vel param"
  This reverts commit 02157b6ae0c2ff1564840f6d15e3c55025327baf.
  * add comment and hypot
  * remove min vel
  * add flag for extending traj
  * add extend param to default param
  * fix typo
  * fix from TakaHoribe review
  * fix typo
  * refactor
* refactor(vehicle_cmd_gate): remove old emergency topics (`#2403 <https://github.com/autowarefoundation/autoware.universe/issues/2403>`_)
* fix: rename `use_external_emergency_stop` to  `check_external_emergency_heartbeat` (`#2455 <https://github.com/autowarefoundation/autoware.universe/issues/2455>`_)
  * fix: rename use_external_emergency_stop to check_external_emergency_heartbeat
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(transition_manager): add param to ignore autonomous transition condition (`#2453 <https://github.com/autowarefoundation/autoware.universe/issues/2453>`_)
  * feat(transition_manager): add param to ignore autonomous transition condition
  * same for modeChangeCompleted
  * remove debug print
* feat(operation_mode_transition_manager): modify transition timeout (`#2318 <https://github.com/autowarefoundation/autoware.universe/issues/2318>`_)
  feat(operation_mode_transition_manager): modify mode change in transition
* feat(emergency_handler): add a selector for multiple MRM behaviors (`#2070 <https://github.com/autowarefoundation/autoware.universe/issues/2070>`_)
  * feat(emergency_handler): add mrm command and status publishers
  * feat(autoware_ad_api_msgs): define mrm operation srv and mrm status msg
  * feat(emergency_handler): add mrm clients and subscribers
  * feat(mrm_comfortable_stop_operator): ready ros2 node template
  * feat(mrm_comfortable_stop_operator): implemented
  * feat(mrm_comfortable_stop_operator): implement as component
  * chore(mrm_comfortable_stop_operator): add a launch script
  * refactor(mrm_comfortable_stop_operator): remove a xml launch file
  * feat(autoware_ad_api_msgs): change mrm status msg
  * feat(emergency_handler): add mrm operator and mrm behavior updater
  * feat(emergency_handler): add mrm behavior state machine
  * feat(emergency_handler): remap io names
  * fix(emergency_handler): fix request generation
  * fix(emergency_handler): add multi thread execution for service
  * feat(vehicle_cmd_gate): add mrm operation service and status publisher
  * refactor(mrm_comfortable_stop_operator): use MRMBehaviorStatus struct
  * fix(mrm_comfortable_stop_operator): add time stamp for status
  * feat(vehicle_cmd_gate): change system emergency state by mrm operation
  * chore(autoware_ad_api_msgs): remove rti_operating state from mrm status
  * feat(mrm_sudden_stop_operator): add mrm_sudden_stop_operator
  * refactor(autoware_ad_api_msgs): rename from mrm status to mrm state
  * fix(mrm_comfortable_stop_operator): set qos for velocity limit publisher
  * feat(emergency_handler): add mrm state publisher
  * feat(vehicle_cmd_gate): add subscription for mrm_state
  * fix(mrm_sudden_stop_operator): fix control command topic name
  * feat(vehicle_cmd_gate): pub emergency control_cmd according to mrm state
  * feat(emergency_handler): remove emergency control_cmd publisher
  * chore(tier4_control_launch): remap mrm state topic
  * feat(tier4_system_launch): launch mrm operators
  * fix(emergency_handler): fix autoware_ad_api_msgs to autoware_adapi_v1_msgs
  * fix(vehicle_cmd_gate): remove subscribers for emergency_state and mrm operation
  * fix(vehicle_cmd_gate): fix system emergency condition
  * fix(emergency_handler): add stamp for mrm_state
  * fix(mrm_emergency_stop_operator): rename sudden stop to emergency stop
  * fix(vehicle_cmd_gate): remove emergency stop status publisher
  * fix(emergency_handler): replace emergency state to mrm state
  * feat(mrm_emergency_stop_operator): add is_available logic
  * feat(emergency_handler): add use_comfortable_stop param
  * refactor(emergency_handler): rename getCurrentMRMBehavior
  * feat(emergency_handler): add mrm available status for ready conditions
  * feat(emergency_handler): add readme
  * fix(mrm_comfortable_stop_operator): fix update rate
  * refactor(emergency_handler): move MRMBehaviorStatus msg to tier4_system_msgs
  * feat(emergency_handler): describe new io for emergency_handler
  * fix(emergency_handler): remove extra settings
  * fix(mrm_emergency_stop_operator): fix is_available condition
  * fix(mrm_emergency_stop_operator): fix typo
  * ci(pre-commit): autofix
  * fix(mrm_emergency_stop_operator): remove extra descriptions on config files
  * fix(mrm_comfortable_stop_operator): fix typo
  * chore(mrm_comfortable_stop_operator): change words
  * chore(mrm_comfortable_stop_operator): change maintainer infomation
  * fix(emergency_handler): fix acronyms case
  * chore(emergency_handler): add a maintainer
  * fix(emergency_handler): fix to match msg changes
  * fix(vehicle_cmd_gate): remove an extra include
  * ci(pre-commit): autofix
  * fix(emergency_handler): fix topic name spaces
  * fix(emergency_handler): fix acronyms case
  * chore(tier4_system_launch): add a mrm comfortable stop parameter
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(operation_mode_transition_manager): support ad api (`#1535 <https://github.com/autowarefoundation/autoware.universe/issues/1535>`_)
  * feat(operation_mode_transition_manager): support ad api
  * fix: merge operation mode state message
  * feat(autoware_ad_api_msgs): define operation mode interface
  * fix: add message
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat: apply field name change
  * feat: move adapi message
  * feat: change message type
  * fix: fix build error
  * fix: fix error message
  * WIP
  * feat: add compatibility
  * fix: fix operation mode change when disable autoware control
  * fix: fix operation mode change when autoware control is disabled
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat(tier4_control_launch): add obstacle_collision_checker in control.launch.py (`#2193 <https://github.com/autowarefoundation/autoware.universe/issues/2193>`_)
  Co-authored-by: Berkay Karaman <berkay@leodrive.ai>
* feat(tier4_planning/control_launch): add missing dependency (`#2201 <https://github.com/autowarefoundation/autoware.universe/issues/2201>`_)
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(control_launch): add longitudinal controller mode (`#2062 <https://github.com/autowarefoundation/autoware.universe/issues/2062>`_)
  feature(control_launch): add longitudinal controller mode
* fix: modified to reflect the argument "initial_selector_mode" in control_launch (`#1961 <https://github.com/autowarefoundation/autoware.universe/issues/1961>`_)
* refactor: replace acc calculation in planning control modules (`#1213 <https://github.com/autowarefoundation/autoware.universe/issues/1213>`_)
  * [obstacle_cruise_planner] replace acceleration calculation
  * [obstacle_stop_planner] replace acceleration calculation
  * [trajectory_follower] replace acceleration calculation
  * remap topic name in lanuch
  * fix nullptr check
  * fix controller test
  * fix
* feat(shift_decider): add config file (`#1857 <https://github.com/autowarefoundation/autoware.universe/issues/1857>`_)
  * feat(shift_decider): add config file
  * feat(tier4_control_launch): add shift_decider.param.yaml
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(shift_decider): put the gear in park when vehicle reaches the goal (`#1818 <https://github.com/autowarefoundation/autoware.universe/issues/1818>`_)
  * feat(shift_decider): put the gear in park when vehicle reaches the goal
  * ci(pre-commit): autofix
  * feat(shift_decider): check /autoware/state subscriber in timer function
  * refactor(shif_decider): change state topic name for remapping
  * feat(tier4_control_launch): add state topic remap for shift_decider
  * feat(shift_decider): add state topic remap to launch file
  * feat(shift_decider): add park_on_goal flag
  * feat(tier4_control_launch): add park_on_goal param for shift_decider
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(motion_velocity_smoother): add steering rate limit while planning velocity (`#1071 <https://github.com/autowarefoundation/autoware.universe/issues/1071>`_)
  * feat(motion_velocity_smoother): add steering rate limit while planning velocity (`#1071 <https://github.com/autowarefoundation/autoware.universe/issues/1071>`_)
  function added,
  not turning
  fix the always positive curvature problem
  added lower velocity limit
  added vehicle parameters
  functions created
  * Update readme
  update svg
  readme updated
  with test params
  change sample rate
  calculate accurate dt
  test
  fix trajectory size
  update readme
  change map loader params
  clear unnecessary comment
  change the min and max index
  ci(pre-commit): autofix
  removed unnecessary params and comments
  ci(pre-commit): autofix
  all velocities in lookup distance is changed
  ci(pre-commit): autofix
  works
  ci(pre-commit): autofix
  changed calculations
  with const lookupdistance
  ci(pre-commit): autofix
  not work peak points
  written with constant distances
  added param
  ci(pre-commit): autofix
  update
  ci(pre-commit): autofix
  update steering angle calculation method
  ci(pre-commit): autofix
  changed curvature calculation of steeringAngleLimit func
  changed default parameter values
  update readme
  update engage velocity parameter
  * ci(pre-commit): autofix
  Co-authored-by: Berkay <berkay@leodrive.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(planning/control packages): organized authors and maintainers (`#1610 <https://github.com/autowarefoundation/autoware.universe/issues/1610>`_)
  * organized planning authors and maintainers
  * organized control authors and maintainers
  * fix typo
  * fix colcon test
  * fix
  Update control/external_cmd_selector/package.xml
  Update control/vehicle_cmd_gate/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/motion_velocity_smoother/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/freespace_planner/package.xml
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix
  * fix
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* chore(trajectory_follower_nodes): remove the remaining latlon_muxer (`#1592 <https://github.com/autowarefoundation/autoware.universe/issues/1592>`_)
* feat(trajectory_follower): keep stop until the steering control is done (`#1672 <https://github.com/autowarefoundation/autoware.universe/issues/1672>`_)
* fix(velocity_controller, mpc_follower): use common ego nearest search (`#1590 <https://github.com/autowarefoundation/autoware.universe/issues/1590>`_)
  * fix(trajectory_follower): use common ego nearest search
  * removed calcNearestIndex
  * add nearest search param
  * fix
  * fix
  * Update launch/tier4_control_launch/launch/control.launch.py
  * update test fil
* fix(tier4_control_launch): pass vehicle_info_param to trajectory_follower (`#1450 <https://github.com/autowarefoundation/autoware.universe/issues/1450>`_)
* refactor(trajectory_follower_nodes): use max_steer_angle in common (`#1422 <https://github.com/autowarefoundation/autoware.universe/issues/1422>`_)
  * refactor(trajectory_follower_nodes): use max_steer_angle in common
  * remove parameters from tier4_control_launch
  * fix
* feat(tier4_control_launch): declare param path argument (`#1432 <https://github.com/autowarefoundation/autoware.universe/issues/1432>`_)
* fix(operation_mode_transition_manager): add required param (`#1342 <https://github.com/autowarefoundation/autoware.universe/issues/1342>`_)
* feat(operation_mode_transition_manager): add package to manage vehicle autonomous mode change (`#1246 <https://github.com/autowarefoundation/autoware.universe/issues/1246>`_)
  * add engage_transition_manager
  * rename to operation mode transition manager
  * fix precommit
  * fix cpplint
  * fix topic name & vehicle_info
  * update launch
  * update default param
  * add allow_autonomous_in_stopped
  * fix typo
  * fix precommit
* feat(trajectory_follower): add min_prediction_length to mpc (`#1171 <https://github.com/autowarefoundation/autoware.universe/issues/1171>`_)
  * feat(trajectory_follower): Add min_prediction_length to mpc
  * refactor
* feat(vehicle cmd gate): add transition filter (`#1244 <https://github.com/autowarefoundation/autoware.universe/issues/1244>`_)
  * feat(vehicle_cmd_gate): add transition filter
  * fix precommit
  * remove debug code
  * update param yaml
  * update readme
  * fix default topic name
* feat(trajectory_follower): integrate latlon controller (`#901 <https://github.com/autowarefoundation/autoware.universe/issues/901>`_)
  * feat(trajectory_follower): integrate latlon controller
  * Remove unnecessary throw error
  * update from review comment
  * Set steer converged params false
  * Update params of design.md
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* feat(longitudinal_controller): add disable emergency option (`#1201 <https://github.com/autowarefoundation/autoware.universe/issues/1201>`_)
  * feat(longitudinal_controller): add disable emergency option
  * update readme
  * add param
* refactor(vehicle_cmd_gate): change namespace in launch file (`#927 <https://github.com/autowarefoundation/autoware.universe/issues/927>`_)
  Co-authored-by: Berkay <berkay@leodrive.ai>
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* perf(trajectory_follower_nodes): change longitudinal control to use period parameter (`#763 <https://github.com/autowarefoundation/autoware.universe/issues/763>`_)
  * perf(trajectory_follower_nodes): change longitudinal control to use period parameter
  * perf(trajectory_follower_nodes): remove duplicate ros parameters in 'control.launch.py'
  * doc(trajectory_follower_nodes): update design doc according to code update
  * ci(pre-commit): autofix
  Co-authored-by: Shark Liu <shark.liu@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: add pure_pursuit as lateral controller into launch files (`#750 <https://github.com/autowarefoundation/autoware.universe/issues/750>`_)
* fix(longitudinal_controller_node, vehicle_cmd_gate): update stopped condition and behavior (`#700 <https://github.com/autowarefoundation/autoware.universe/issues/700>`_)
  * fix(longitudinal_controller_node): parameterize stopped state entry condition
  * fix(longitudinal_controller_node): simply set stopped velocity in STOPPED STATE
  * fix(vehicle_cmd_gate): check time duration since the vehicle stopped
* fix(control_launch): change default mpc param to improve performance (`#667 <https://github.com/autowarefoundation/autoware.universe/issues/667>`_)
* fix(trajectory_follower): change stop check speed threshold to almost 0 (`#473 <https://github.com/autowarefoundation/autoware.universe/issues/473>`_)
  * fix(trajectory_follower): change stop check speed threshold to 0
  * change default parameter
* fix(trajectory_follower): change default param for curvature smoothing (`#498 <https://github.com/autowarefoundation/autoware.universe/issues/498>`_)
* feat: change launch package name (`#186 <https://github.com/autowarefoundation/autoware.universe/issues/186>`_)
  * rename launch folder
  * autoware_launch -> tier4_autoware_launch
  * integration_launch -> tier4_integration_launch
  * map_launch -> tier4_map_launch
  * fix
  * planning_launch -> tier4_planning_launch
  * simulator_launch -> tier4_simulator_launch
  * control_launch -> tier4_control_launch
  * localization_launch -> tier4_localization_launch
  * perception_launch -> tier4_perception_launch
  * sensing_launch -> tier4_sensing_launch
  * system_launch -> tier4_system_launch
  * ci(pre-commit): autofix
  * vehicle_launch -> tier4_vehicle_launch
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: tanaka3 <ttatcoder@outlook.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* Contributors: Azumi Suzuki, Berkay, Berkay Karaman, Kenji Miyake, Kosuke Takeuchi, Kyoichi Sugahara, Makoto Kurihara, Mamoru Sobue, Ryohsuke Mitsudome, Satoshi OTA, Shark, Shumpei Wakabayashi, Takagi, Isamu, Takamasa Horibe, Takayuki Murooka, Tomoya Kimura, Vincent Richard, Yutaka Shimizu, lilyildiz, taikitanaka3

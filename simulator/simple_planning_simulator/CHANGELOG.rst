^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package simple_planning_simulator
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
* fix(cpplint): include what you use - simulator (`#9572 <https://github.com/autowarefoundation/autoware.universe/issues/9572>`_)
* fix(simple_planning_simulator): fix clang-diagnostic-delete-non-abstract-non-virtual-dtor (`#9448 <https://github.com/autowarefoundation/autoware.universe/issues/9448>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(simple_planning_simulator): add mechanical actuaion sim model (`#9300 <https://github.com/autowarefoundation/autoware.universe/issues/9300>`_)
  * feat(simple_planning_simulator): add mechanical actuaion sim model
  update docs
  * update from suggestions
  * calc internal state using RK4 results
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo

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
* fix(simple_planning_simulator): change orger of IDX in SimModelDelayS… (`#9128 <https://github.com/autowarefoundation/autoware.universe/issues/9128>`_)
* fix(simple_planning_simulator, raw_vehicle_cmd_converter): swap row index and column index for csv loader  (`#8963 <https://github.com/autowarefoundation/autoware.universe/issues/8963>`_)
  swap row and column
* chore(simple_planning_simulator): remove unnecessary lines (`#8932 <https://github.com/autowarefoundation/autoware.universe/issues/8932>`_)
  remove unnecessary semicolons
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(simple_planning_simulator): delete velocity dead band for brake (`#8685 <https://github.com/autowarefoundation/autoware.universe/issues/8685>`_)
  * delete dead band
* fix(simple_planning_simulator): increase test_steer_map values (`#8631 <https://github.com/autowarefoundation/autoware.universe/issues/8631>`_)
* feat(simple_planning_simulator): print actual and expected value in test (`#8630 <https://github.com/autowarefoundation/autoware.universe/issues/8630>`_)
* fix(simple_planning_simulator): fix dimension (`#8629 <https://github.com/autowarefoundation/autoware.universe/issues/8629>`_)
* fix(simple_planning_simulator): fix acc output for the model sim_model_delay_steer_acc_geared_wo_fall_guard (`#8607 <https://github.com/autowarefoundation/autoware.universe/issues/8607>`_)
  fix acceleration output
* feat(simple_planning_simulator): add VGR sim model (`#8415 <https://github.com/autowarefoundation/autoware.universe/issues/8415>`_)
  * feat(simple_planning_simulator): add VGR sim model
  * Update simulator/simple_planning_simulator/test/test_simple_planning_simulator.cpp
  * move to interface
  * add const
  ---------
* feat(psim)!: preapre settings to launch localization modules on psim (`#8212 <https://github.com/autowarefoundation/autoware.universe/issues/8212>`_)
* fix(simple_planning_simulator): fix publised acc of actuation simulator (`#8169 <https://github.com/autowarefoundation/autoware.universe/issues/8169>`_)
* feat(simple_planning_simulator): add actuation command simulator (`#8065 <https://github.com/autowarefoundation/autoware.universe/issues/8065>`_)
  * feat(simple_planning_simulator): add actuation command simulator
  tmp
  add
  * remove unused functions
  * common map
  * pre-commit
  * update readme
  * add test
  install test dir
  fix test
  * pre-commit
  * clean up test for for scalability parameter
  * fix typo
  ---------
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat(simple_planning_simulator): add new vehicle model with falling down (`#7651 <https://github.com/autowarefoundation/autoware.universe/issues/7651>`_)
  * add new vehicle model
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(simple_planning_simulator): fix duplicateBranch warnings (`#7574 <https://github.com/autowarefoundation/autoware.universe/issues/7574>`_)
  * fix(simple_planning_simulator): fix duplicateBranch warnings
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* refactor(simple_planning_simulator): remove static odom tf publisher (`#7265 <https://github.com/autowarefoundation/autoware.universe/issues/7265>`_)
* feat!: replace autoware_auto_msgs with autoware_msgs for simulator modules (`#7248 <https://github.com/autowarefoundation/autoware.universe/issues/7248>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat!: remove autoware_auto_tf2 package (`#7218 <https://github.com/autowarefoundation/autoware.universe/issues/7218>`_)
  * feat!: remove autoware_auto_geometry package
  * docs: remove autoware_auto_geometry package from docs
  * feat!: remove autoware_auto_tf2 package
  * fix: remove from autoware_auto_tf2 packages from docs page
  ---------
* chore(simple_planning_simulator): add maintainer (`#7026 <https://github.com/autowarefoundation/autoware.universe/issues/7026>`_)
* chore(simple_planning_simulator): publish control mode before the self-position is given (`#7008 <https://github.com/autowarefoundation/autoware.universe/issues/7008>`_)
* feat(learned_model): create package (`#6395 <https://github.com/autowarefoundation/autoware.universe/issues/6395>`_)
  Co-authored-by: Tomas Nagy <tomas@pmc.sk>
* Contributors: Autumn60, Dawid Moszyński, Esteve Fernandez, Go Sakayori, Kosuke Takeuchi, Maxime CLEMENT, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Tomas Nagy, Tomoya Kimura, Yuki TAKAGI, Yutaka Kondo, Zulfaqar Azmi

0.26.0 (2024-04-03)
-------------------
* feat(simple_planning_simulator): add enable_road_slope_simulation param (`#5933 <https://github.com/autowarefoundation/autoware.universe/issues/5933>`_)
* fix(log-messages): reduce excessive log messages (`#5971 <https://github.com/autowarefoundation/autoware.universe/issues/5971>`_)
* fix(simple_planning_simulator): fix steering bias model (`#6240 <https://github.com/autowarefoundation/autoware.universe/issues/6240>`_)
  * fix(simple_planning_simulator): fix steering bias model
  * remove old implementation
  * fix initialize order
  * fix yawrate measurement
  * remove unused code
  * add bias to steer rate
  * add comments
  * fix getWz()
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/vehicle_model/sim_model_delay_steer_acc.cpp
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/vehicle_model/sim_model_delay_steer_map_acc_geared.cpp
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/vehicle_model/sim_model_delay_steer_vel.cpp
  ---------
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(simple_planning_simulator): add option to use initialpose for z position (`#4256 <https://github.com/autowarefoundation/autoware.universe/issues/4256>`_)
  * feat(simple_planning_simulator): add option to use initialpose for z position
  * Revert "feat(simple_planning_simulator): add option to use initialpose for z position"
  This reverts commit a3e2779cd38841ba49e063c42fc3a2366c176ad6.
  * update initial z logic
  ---------
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* fix(autoware_auto_common): move headers to a separate directory (`#5919 <https://github.com/autowarefoundation/autoware.universe/issues/5919>`_)
  * fix(autoware_auto_common): move headers to a separate directory
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(simple_planning_simulator): add mesurent_steer_bias (`#5868 <https://github.com/autowarefoundation/autoware.universe/issues/5868>`_)
  * feat(simple_planning_simulator): add mesurent_steer_bias
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(simple_plannign_simulator): add map acc model (`#5688 <https://github.com/autowarefoundation/autoware.universe/issues/5688>`_)
  * (simple_planning_simulator):add delay converter model
  * refactoring
  rename and format
  read acc map path from config
  * update docs
  * remove noisy print
  * update map
  * fix pre-commit
  * update acc map
  * fix pre-commit and typo
  typo
  typo
  * Update simulator/simple_planning_simulator/README.md
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Update simulator/simple_planning_simulator/README.md
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Update simulator/simple_planning_simulator/README.md
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Update simulator/simple_planning_simulator/include/simple_planning_simulator/vehicle_model/sim_model_delay_steer_map_acc_geared.hpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * update error message
  * simplify map exmaple
  * use double
  * style(pre-commit): autofix
  * Update simulator/simple_planning_simulator/README.md
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * add csv loader im sim pacakges
  * revert raw vehicle cmd converter
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/vehicle_model/sim_model_delay_steer_map_acc_geared.cpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Update simulator/simple_planning_simulator/include/simple_planning_simulator/utils/csv_loader.hpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/utils/csv_loader.cpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  ---------
  Co-authored-by: Takumi Ito <takumi.ito@tier4.jp>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(simple_planning_simulator): fix ego sign pitch problem (`#5616 <https://github.com/autowarefoundation/autoware.universe/issues/5616>`_)
  * fix ego sign pitch problem
  * change variable name for clarity
  * update documentation to clarify that driving against the lane is not supported
  ---------
* fix(simple_planning_simulator): change default value of manual gear, DRIVE -> PARK (`#5563 <https://github.com/autowarefoundation/autoware.universe/issues/5563>`_)
* feat(simple_planning_simulator): add acceleration and steer command scaling factor for debug (`#5534 <https://github.com/autowarefoundation/autoware.universe/issues/5534>`_)
  * feat(simple_planning_simulator): add acceleration and steer command scaling factor
  * update params as debug
  ---------
* fix(simple_planning_simulator): set ego pitch to 0 if road slope is not simulated (`#5501 <https://github.com/autowarefoundation/autoware.universe/issues/5501>`_)
  set ego pitch to 0 if road slope is not simulated
* feat(simple_planning_simulator): add steer dead band (`#5477 <https://github.com/autowarefoundation/autoware.universe/issues/5477>`_)
  * feat(simple_planning_simulator): add steer dead band
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/simple_planning_simulator_core.cpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Update simulator/simple_planning_simulator/README.md
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * update params
  ---------
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* fix(simple_planning_simulator): initialize variables (`#5460 <https://github.com/autowarefoundation/autoware.universe/issues/5460>`_)
* feat(simple_planning_sim): publish lateral acceleration (`#5307 <https://github.com/autowarefoundation/autoware.universe/issues/5307>`_)
* fix(simulator, controller): fix inverse pitch calculation (`#5199 <https://github.com/autowarefoundation/autoware.universe/issues/5199>`_)
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* fix(simple_planning_simulator): fix build error (`#5062 <https://github.com/autowarefoundation/autoware.universe/issues/5062>`_)
* feat(simple_planning_simulator): consider ego pitch angle for simulation (`#4941 <https://github.com/autowarefoundation/autoware.universe/issues/4941>`_)
  * feat(simple_planning_simulator): consider ego pitch angle for simulation
  * update
  * fix spell
  * update
  ---------
* chore(build): remove tier4_autoware_utils.hpp evaluator/ simulator/ (`#4839 <https://github.com/autowarefoundation/autoware.universe/issues/4839>`_)
* docs(simple_planning_simulator): rename docs to readme (`#4221 <https://github.com/autowarefoundation/autoware.universe/issues/4221>`_)
* fix(simple_planning_simulator): old style arg for static_tf_publisher (`#3736 <https://github.com/autowarefoundation/autoware.universe/issues/3736>`_)
  * fix(simple_planning_simulator): old style arg for static_tf_publisher
  * Update simulator/simple_planning_simulator/launch/simple_planning_simulator.launch.py
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  ---------
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* build: proper eigen deps and include (`#3615 <https://github.com/autowarefoundation/autoware.universe/issues/3615>`_)
  * build: proper eigen deps and include
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* chore: sync files (`#3227 <https://github.com/autowarefoundation/autoware.universe/issues/3227>`_)
  * chore: sync files
  * style(pre-commit): autofix
  ---------
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(simple_planning_sim): publish sensing interface imu data (`#2843 <https://github.com/autowarefoundation/autoware.universe/issues/2843>`_)
  * feat(simple_planning_sim): publish sensing interface imu data
  * fix covariance index
  ---------
* chore(planning-sim): change debug topic name (`#2610 <https://github.com/autowarefoundation/autoware.universe/issues/2610>`_)
* fix(simple_planning_simulator): fix ideal steer acc calc (`#2595 <https://github.com/autowarefoundation/autoware.universe/issues/2595>`_)
* refactor(simple_planning_simulator): make function for duplicated code (`#2502 <https://github.com/autowarefoundation/autoware.universe/issues/2502>`_)
* feat(simple_planning_simulator): add initial twist for debug purpose (`#2268 <https://github.com/autowarefoundation/autoware.universe/issues/2268>`_)
* chore(simple_planning_simulator): add maintainer  (`#2444 <https://github.com/autowarefoundation/autoware.universe/issues/2444>`_)
  chore(simple_planning_simulator): add maintainer
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* fix(simple_planning_simulator): sim model with gear acc (`#2437 <https://github.com/autowarefoundation/autoware.universe/issues/2437>`_)
* chore: remove autoware_auto_common dependency from simple_planning_simulator and osqp_interface (`#2233 <https://github.com/autowarefoundation/autoware.universe/issues/2233>`_)
  remove autoware_auto_common dependency from simple_planning_simulator, osqp_interface
* chore: remove motion_common dependency (`#2231 <https://github.com/autowarefoundation/autoware.universe/issues/2231>`_)
  * remove motion_common from smoother
  * remove motion_common from control_performance_analysis and simple_planning_simualtor
  * fix include
  * add include
* refactor!: remove tier4 control mode msg (`#1533 <https://github.com/autowarefoundation/autoware.universe/issues/1533>`_)
  * [simple_planning_simulator] replace T4 ControlMode msg too auto_msg
  * [operation_mode_transition_manager] replace T4 ControlMode msg too auto_msg
* refactor(simple_planning_simulator): refactor covariance index (`#1972 <https://github.com/autowarefoundation/autoware.universe/issues/1972>`_)
* feat(pose_initializer)!: support ad api (`#1500 <https://github.com/autowarefoundation/autoware.universe/issues/1500>`_)
  * feat(pose_initializer): support ad api
  * docs: update readme
  * fix: build error
  * fix: test
  * fix: auto format
  * fix: auto format
  * feat(autoware_ad_api_msgs): define localization interface
  * feat: update readme
  * fix: copyright
  * fix: main function
  * Add readme of localization message
  * feat: modify stop check time
  * fix: fix build error
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(simple_planning_simulator): fix param file levels (`#1612 <https://github.com/autowarefoundation/autoware.universe/issues/1612>`_)
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
* fix(simple_planning_simulator): fix timer type (`#1538 <https://github.com/autowarefoundation/autoware.universe/issues/1538>`_)
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
* feat(simple_planning_simulator): add acceleration publisher (`#1214 <https://github.com/autowarefoundation/autoware.universe/issues/1214>`_)
  * feat(simple_planning_simulator): add acceleration publisher
  * add cov
* feat(simple_planning_simulator): add control_mode server (`#1061 <https://github.com/autowarefoundation/autoware.universe/issues/1061>`_)
  * add control-mode in simulator
  * precommit
  * update
  * update readme
  * update simulator
  * fix typo
* fix(simple_planning_simlator): keep alive tf (`#1175 <https://github.com/autowarefoundation/autoware.universe/issues/1175>`_)
  * fix(simple_planning_simlator): keep alive tf
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* docs(simulator): fixed simple_planning_simulator table (`#1025 <https://github.com/autowarefoundation/autoware.universe/issues/1025>`_)
* docs: update link style and fix typos (`#950 <https://github.com/autowarefoundation/autoware.universe/issues/950>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * docs: update link style
  * chore: fix link
  * feat(map_tf_generator): accelerate the 'viewer' coordinate calculation (`#890 <https://github.com/autowarefoundation/autoware.universe/issues/890>`_)
  * add random point sampling function to quickly calculate the 'viewer' coordinate
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * docs(obstacle_stop_planner): update documentation (`#880 <https://github.com/autowarefoundation/autoware.universe/issues/880>`_)
  * docs(tier4_traffic_light_rviz_plugin): update documentation (`#905 <https://github.com/autowarefoundation/autoware.universe/issues/905>`_)
  * fix(accel_brake_map_calibrator): rviz panel type (`#895 <https://github.com/autowarefoundation/autoware.universe/issues/895>`_)
  * fixed panel type
  * modified instruction for rosbag replay case
  * modified update_map_dir service name
  * fix(behavior velocity planner): skipping emplace back stop reason if it is empty (`#898 <https://github.com/autowarefoundation/autoware.universe/issues/898>`_)
  * skipping emplace back stop reason if it is empty
  * add braces
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * feat(behavior_path_planner): weakened noise filtering of drivable area (`#838 <https://github.com/autowarefoundation/autoware.universe/issues/838>`_)
  * feat(behavior_path_planner): Weakened noise filtering of drivable area
  * fix lanelet's longitudinal disconnection
  * add comments of erode/dilate process
  * refactor(vehicle-cmd-gate): using namespace for msgs (`#913 <https://github.com/autowarefoundation/autoware.universe/issues/913>`_)
  * refactor(vehicle-cmd-gate): using namespace for msgs
  * for clang
  * feat(pose_initializer): introduce an array copy function (`#900 <https://github.com/autowarefoundation/autoware.universe/issues/900>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat: add lidar point filter when debug (`#865 <https://github.com/autowarefoundation/autoware.universe/issues/865>`_)
  * feat: add lidar point filter when debug
  * ci(pre-commit): autofix
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(component_interface_utils): add interface classes  (`#899 <https://github.com/autowarefoundation/autoware.universe/issues/899>`_)
  * feat(component_interface_utils): add interface classes
  * feat(default_ad_api): apply the changes of interface utils
  * fix(component_interface_utils): remove old comment
  * fix(component_interface_utils): add client log
  * fix(component_interface_utils): remove unimplemented message
  * docs(component_interface_utils): add design policy
  * docs(component_interface_utils): add comment
  * refactor(vehicle_cmd_gate): change namespace in launch file (`#927 <https://github.com/autowarefoundation/autoware.universe/issues/927>`_)
  Co-authored-by: Berkay <berkay@leodrive.ai>
  * feat: visualize lane boundaries (`#923 <https://github.com/autowarefoundation/autoware.universe/issues/923>`_)
  * feat: visualize lane boundaries
  * fix: start_bound
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(system_monitor): fix truncation warning in strncpy (`#872 <https://github.com/autowarefoundation/autoware.universe/issues/872>`_)
  * fix(system_monitor): fix truncation warning in strncpy
  * Use std::string constructor to copy char array
  * Fixed typo
  * fix(behavior_velocity_planner.stopline): extend following and previous search range to avoid no collision (`#917 <https://github.com/autowarefoundation/autoware.universe/issues/917>`_)
  * fix: extend following and previous search range to avoid no collision
  * chore: add debug marker
  * fix: simplify logic
  * chore: update debug code
  * fix: delete space
  * fix: some fix
  * ci(pre-commit): autofix
  * fix: delete debug code
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * docs(surround obstacle checker): update documentation (`#878 <https://github.com/autowarefoundation/autoware.universe/issues/878>`_)
  * docs(surround_obstacle_checker): update pub/sub topics & params
  * docs(surround_obstacle_checker): remove unused files
  * docs(surround_obstacke_checker): update purpose
  * feat(tier4_autoware_utils): add vehicle state checker (`#896 <https://github.com/autowarefoundation/autoware.universe/issues/896>`_)
  * feat(tier4_autoware_utils): add vehicle state checker
  * fix(tier4_autoware_utils): use absolute value
  * feat(tier4_autoware_utils): divide into two classies
  * test(tier4_autoware_utils): add unit test for vehicle_state checker
  * fix(tier4_autoware_utils): impl class inheritance
  * docs(tier4_autoware_utils): add vehicle_state_checker document
  * fix(tier4_autoware_utils): into same loop
  * fix(tier4_autoware_utils): fix variables name
  * fix(tier4_autoware_utils): remove redundant codes
  * fix(motion_velocity_smoother): fix overwriteStopPoint using backward point (`#816 <https://github.com/autowarefoundation/autoware.universe/issues/816>`_)
  * fix(motion_velocity_smoother): fix overwriteStopPoint using backward point
  * Modify overwriteStopPoint input and output
  * feat(obstacle_avoidance_planner): explicitly insert zero velocity (`#906 <https://github.com/autowarefoundation/autoware.universe/issues/906>`_)
  * feat(obstacle_avoidance_planner) fix bug of stop line unalignment
  * fix bug of unsorted output points
  * move calcVelocity in node.cpp
  * fix build error
  * feat(behavior_velocity): find occlusion more efficiently (`#829 <https://github.com/autowarefoundation/autoware.universe/issues/829>`_)
  * fix(system_monitor): add some smart information to diagnostics (`#708 <https://github.com/autowarefoundation/autoware.universe/issues/708>`_)
  * feat(obstacle_avoidance_planner): dealt with close lane change (`#921 <https://github.com/autowarefoundation/autoware.universe/issues/921>`_)
  * feat(obstacle_avoidance_planner): dealt with close lane change
  * fix bug of right lane change
  * feat(obstacle_avoidance_planner): some fix for narrow driving (`#916 <https://github.com/autowarefoundation/autoware.universe/issues/916>`_)
  * use car like constraints in mpt
  * use not widest bounds for the first bounds
  * organized params
  * fix format
  * prepare rear_drive and uniform_circle constraints
  * fix param callback
  * update config
  * remove unnecessary files
  * update tier4_planning_launch params
  * chore(obstacle_avoidance_planner): removed obsolete obstacle_avoidance_planner doc in Japanese (`#919 <https://github.com/autowarefoundation/autoware.universe/issues/919>`_)
  * chore(behavior_velocity_planner.stopline): add debug marker for stopline collision check (`#932 <https://github.com/autowarefoundation/autoware.universe/issues/932>`_)
  * chore(behavior_velocity_planner.stopline): add debug marker for stopline collision check
  * feat: use marker helper
  * feat(map_loader): visualize center line by points (`#931 <https://github.com/autowarefoundation/autoware.universe/issues/931>`_)
  * feat: visualize center line points
  * fix: delete space
  * feat: visualize center line by arrow
  * revert insertMarkerArray
  * fix: delete space
  * feat: add RTC interface (`#765 <https://github.com/autowarefoundation/autoware.universe/issues/765>`_)
  * feature(rtc_interface): add files
  * feature(rtc_interface): implement functions
  * feature(rtc_interface): reimprement functions to use CooperateCommands and write README.md
  * feature(rtc_interface): fix README
  * feature(rtc_interface): add getModuleType()
  * feature(rtc_interface): fix definition of constructor
  * feature(rtc_interface): fix time stamp
  * feature(rtc_interface): fix README
  * feature(rtc_interface): add isRegistered and clearCooperateStatus
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore: sync files (`#911 <https://github.com/autowarefoundation/autoware.universe/issues/911>`_)
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  * fix: replace boost::mutex::scoped_lock to std::scoped_lock (`#907 <https://github.com/autowarefoundation/autoware.universe/issues/907>`_)
  * fix: replace boost::mutex::scoped_lock to std::scoped_lock
  * fix: replace boost::mutex to std::mutex
  * feat(tensorrt_yolo): add multi gpu support to tensorrt_yolo node (`#885 <https://github.com/autowarefoundation/autoware.universe/issues/885>`_)
  * feat(tensorrt_yolo): add multi gpu support to tensorrt_yolo node
  * feat(tensorrt_yolo): update arg
  Co-authored-by: Kaan Colak <kcolak@leodrive.ai>
  * feat(tier4_planning_launch): create parameter yaml for behavior_velocity_planner (`#887 <https://github.com/autowarefoundation/autoware.universe/issues/887>`_)
  * feat(tier4_planning_launch): create parameter yaml for behavior_velocity_planner
  * Update launch/tier4_planning_launch/config/scenario_planning/lane_driving/behavior_planning/behavior_velocity_planner/behavior_velocity_planner.param.yaml
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * feat: add param.yaml in behavior_velocity_planner package
  * some fix
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * fix(map_loader): use std::filesystem to load pcd files in pointcloud_map_loader (`#942 <https://github.com/autowarefoundation/autoware.universe/issues/942>`_)
  * fix(map_loader): use std::filesystem to load pcd files in pointcloud_map_loader
  * fix(map_loader): remove c_str
  * fix(map_loader): replace c_str to string
  * fix: relative link
  * fix: relative links
  * fix: relative links
  * fix: relative links
  * fix: typo
  * fix relative links
  * docs: ignore rare unknown words
  * ci(pre-commit): autofix
  * docs: ignore unknown words one by one
  * ci(pre-commit): autofix
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Ishita <ishitah.takeshi@gmail.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
  Co-authored-by: TakumiKozaka-T4 <70260442+TakumiKozaka-T4@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: storrrrrrrrm <103425473+storrrrrrrrm@users.noreply.github.com>
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: Berkay <brkay54@gmail.com>
  Co-authored-by: Berkay <berkay@leodrive.ai>
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: kk-inoue-esol <76925382+kk-inoue-esol@users.noreply.github.com>
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: awf-autoware-bot[bot] <94889083+awf-autoware-bot[bot]@users.noreply.github.com>
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: RyuYamamoto <ryu.yamamoto@tier4.jp>
  Co-authored-by: Kaan Çolak <kaancolak95@gmail.com>
  Co-authored-by: Kaan Colak <kcolak@leodrive.ai>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(vehicle_info_util): add max_steer_angle (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * feat(vehicle_info_util): add max_steer_angle
  * applied pre-commit
  * Added max_steer_angle in test config
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: simplify Rolling support (`#854 <https://github.com/autowarefoundation/autoware.universe/issues/854>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* chore: remove bad chars (`#845 <https://github.com/autowarefoundation/autoware.universe/issues/845>`_)
* fix: suppress compiler warnings (`#852 <https://github.com/autowarefoundation/autoware.universe/issues/852>`_)
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* fix(autoware_auto_tf2): modify build error in rolling (`#718 <https://github.com/autowarefoundation/autoware.universe/issues/718>`_)
  * fix(autoware_auto_common): modify build error in rolling
  * fix(autoware_auto_tf2): modify build error in rolling
  * fix(autoware_auto_geometry): modify build error in rolling
  * fix(simple_planning_simulator): add compile definition for geometry2
  * fix(motion_common): add compile definition for geometry2
  * fix(motion_testing): add compile definition for geometry2
  * fix(simple_planning_simulator): modify build error in rolling
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): clear the exclude option (`#426 <https://github.com/autowarefoundation/autoware.universe/issues/426>`_)
  * ci(pre-commit): remove unnecessary excludes
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * address pre-commit for Markdown files
  * fix Python imports
  * address cpplint errors
  * fix broken package.xml
  * rename ROS parameter files
  * fix build
  * use autoware_lint_common
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(simple_planning_simulator): fix bug in function to apply noise (`#665 <https://github.com/autowarefoundation/autoware.universe/issues/665>`_)
* test(simple_planning_simulator): add node test (`#422 <https://github.com/autowarefoundation/autoware.universe/issues/422>`_)
  * test(simple_planning_simulator): add node test
  * use TEST_P
* fix(simple psim): gear bug to update state in simple psim (`#370 <https://github.com/autowarefoundation/autoware.universe/issues/370>`_)
  * fix(simple psim): gear bug to update state in simple psim
  * upadte ideal acc geared model as well
* fix: simple psim with vehicle engage (`#301 <https://github.com/autowarefoundation/autoware.universe/issues/301>`_)
  * feat: add initial_engage_state for /vehicle/engage sub result
  * feat: simulating only when vehicle engage is true
* feat(simple_planning_simulator): add delay model of velocity and steering (`#235 <https://github.com/autowarefoundation/autoware.universe/issues/235>`_)
  * add delay steer vel in psim
  * change wz to steer
  * fix param description
  * modify readme
  * modify cmake
  * ci: change file URL
  * fix: order to create callback (`#220 <https://github.com/autowarefoundation/autoware.universe/issues/220>`_)
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  * chore: remove unnecessary depends (`#227 <https://github.com/autowarefoundation/autoware.universe/issues/227>`_)
  * ci: add check-build-depends.yaml
  * chore: simplify build_depends.repos
  * chore: remove exec_depend
  * chore: use register-autonomoustuff-repository
  * chore: add setup tasks to other workflows
  * ci: update .yamllint.yaml (`#229 <https://github.com/autowarefoundation/autoware.universe/issues/229>`_)
  * ci: update .yamllint.yaml
  * chore: fix for yamllint
  * fix: remove warning for compile error (`#198 <https://github.com/autowarefoundation/autoware.universe/issues/198>`_)
  * fix: fix compile error of pointcloud preprocessor
  * fix: fix compiler warning for had map utils
  * fix: fix compiler warning for behavior velocity planner
  * fix: fix compiler warning for compare map segmentation
  * fix: fix compiler warning for occupancy grid map outlier filter
  * fix: fix compiler warning for detection by tracker
  * fix: restore comment
  * fix: set control_mode false before autoware engage (`#232 <https://github.com/autowarefoundation/autoware.universe/issues/232>`_)
  * fix: set control_mode false before autoware engage
  * add input/engage remap in launch
  * fix: library path (`#225 <https://github.com/autowarefoundation/autoware.universe/issues/225>`_)
  Co-authored-by: taikitanaka3 <taiki.tanaka@tier4.jp>
  * fix: interpolation (`#791 <https://github.com/autowarefoundation/autoware.universe/issues/791>`_) (`#218 <https://github.com/autowarefoundation/autoware.universe/issues/218>`_)
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * add missing function definition in .cpp
  * set input and state for DELAY_STEER_VEL model
  * fix: fix typo
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: taikitanaka3 <taiki.tanaka@tier4.jp>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
* fix: set control_mode false before autoware engage (`#232 <https://github.com/autowarefoundation/autoware.universe/issues/232>`_)
  * fix: set control_mode false before autoware engage
  * add input/engage remap in launch
* feat: replace VehicleStateCommand with GearCommand (`#217 <https://github.com/autowarefoundation/autoware.universe/issues/217>`_)
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
* fix: fix typo and url (`#201 <https://github.com/autowarefoundation/autoware.universe/issues/201>`_)
  * fix typo
  * fix url (jp -> en)
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: rename existing packages name starting with autoware to different names (`#180 <https://github.com/autowarefoundation/autoware.universe/issues/180>`_)
  * autoware_api_utils -> tier4_api_utils
  * autoware_debug_tools -> tier4_debug_tools
  * autoware_error_monitor -> system_error_monitor
  * autoware_utils -> tier4_autoware_utils
  * autoware_global_parameter_loader -> global_parameter_loader
  * autoware_iv_auto_msgs_converter -> tier4_auto_msgs_converter
  * autoware_joy_controller -> joy_controller
  * autoware_error_monitor -> system_error_monitor(launch)
  * autoware_state_monitor -> ad_service_state_monitor
  * autoware_web_controller -> web_controller
  * remove autoware_version
  * remove autoware_rosbag_recorder
  * autoware\_*_rviz_plugin -> tier4\_*_rviz_plugin
  * fix ad_service_state_monitor
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: update simple planning simulator param file (`#179 <https://github.com/autowarefoundation/autoware.universe/issues/179>`_)
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* feat: add simulator_launch package (`#166 <https://github.com/autowarefoundation/autoware.universe/issues/166>`_)
  * Add simulator_launch package (`#459 <https://github.com/autowarefoundation/autoware.universe/issues/459>`_)
  * Add simulator_launch package
  * add argument
  * fix depend order
  * add argument
  * move dummy_perception_publisher
  * add arg for dummy_perception_publisher
  * Update simulator_launch/launch/simulator.launch.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Move simple_planning_simulator to simulator_launch (`#462 <https://github.com/autowarefoundation/autoware.universe/issues/462>`_)
  * move simple_planning_simulator
  * add simulation arg to logging_simulator.launch
  * delete unused argument
  * add arguments for logging simulation
  * change default value
  * update README
  * add default value to simulator arg
  * restore vehicle_simulation arg
  * Fix/revert initial engage state (`#484 <https://github.com/autowarefoundation/autoware.universe/issues/484>`_)
  * Fix args
  * Add initial_engage_state to vehicle.launch.xml
  * Update vehicle.launch.xml
  * Change formatter to black (`#488 <https://github.com/autowarefoundation/autoware.universe/issues/488>`_)
  * Update pre-commit settings
  * Apply Black
  * Replace ament_lint_common with autoware_lint_common
  * Update build_depends.repos
  * Fix build_depends
  * Auto/fix launch (`#110 <https://github.com/autowarefoundation/autoware.universe/issues/110>`_)
  * fix namespace
  * remove dynamic_object_visualization
  * fix rviz
  * add default vehicle param file
  * ci(pre-commit): autofix
  * fix typo
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* feat: load vehicle info default param (`#148 <https://github.com/autowarefoundation/autoware.universe/issues/148>`_)
  * update global_parameter loader readme
  * remove unused dependency
  * add default vehicle_info_param to launch files
  * fix: import os
  * Update simulator/simple_planning_simulator/launch/simple_planning_simulator.launch.py
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  * Update perception/ground_segmentation/launch/scan_ground_filter.launch.py
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  * fix dependency
  * fix scan_ground_filter.launch
  * ci(pre-commit): autofix
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: add simple planning simulator package (`#5 <https://github.com/autowarefoundation/autoware.universe/issues/5>`_)
  * release v0.4.0
  * remove ROS1 packages temporarily
  * add sample ros2 packages
  * add COLCON_IGNORE to ros1 packages
  * Fix simple planning simulator (`#26 <https://github.com/autowarefoundation/autoware.universe/issues/26>`_)
  * simple planning simulator: fix params & launch file
  * remove unused file
  * fix timercallback
  * [simple_planning_simulator] add rostopic relay in launch file (`#117 <https://github.com/autowarefoundation/autoware.universe/issues/117>`_)
  * [simple_planning_simulator] add rostopic relay in launch file
  * add topic_tools as exec_depend
  * Adjust copyright notice on 532 out of 699 source files (`#143 <https://github.com/autowarefoundation/autoware.universe/issues/143>`_)
  * Use quotes for includes where appropriate (`#144 <https://github.com/autowarefoundation/autoware.universe/issues/144>`_)
  * Use quotes for includes where appropriate
  * Fix lint tests
  * Make tests pass hopefully
  * Run uncrustify on the entire Pilot.Auto codebase (`#151 <https://github.com/autowarefoundation/autoware.universe/issues/151>`_)
  * Run uncrustify on the entire Pilot.Auto codebase
  * Exclude open PRs
  * reduce terminal ouput for better error message visibility (`#200 <https://github.com/autowarefoundation/autoware.universe/issues/200>`_)
  * reduce terminal ouput for better error message visibility
  * [costmap_generator] fix waiting for first transform
  * fix tests
  * fix test
  * Use trajectory for z position source (`#243 <https://github.com/autowarefoundation/autoware.universe/issues/243>`_)
  * Ros2 v0.8.0 engage (`#342 <https://github.com/autowarefoundation/autoware.universe/issues/342>`_)
  * [autoware_vehicle_msgs]: Add engage message
  * [as]: Update message
  * [awapi_awiv_adapter]: Update message
  * [web_controller]: Update message
  * [vehicle_cmd_gate]: Update message
  * [autoware_state_monitor]: Update message
  * [autoware_control_msgs]: Remove EngageMode message
  * [simple_planning_simulator]: Update message
  * Ros2 v0.8.0 fix packages (`#351 <https://github.com/autowarefoundation/autoware.universe/issues/351>`_)
  * add subscription to QoS
  * add vihicle_param _file to simple_planning_sim
  * update cmake/packages.xml
  * comment out unused parameter
  * apply lint
  * add vehicle_info_util to lane_change_planner
  * add vehicle_info_util to vehicle_cmd_gate
  * fix cmake of simple planning simulator
  * update cmake/packages.xml of vehicle cmd gate
  * apply lint
  * apply lint
  * add latch option to autoware_state_monitor
  * delete unused comment
  * Rename ROS-related .yaml to .param.yaml (`#352 <https://github.com/autowarefoundation/autoware.universe/issues/352>`_)
  * Rename ROS-related .yaml to .param.yaml
  * Remove prefix 'default\_' of yaml files
  * Rename vehicle_info.yaml to vehicle_info.param.yaml
  * Rename diagnostic_aggregator's param files
  * Fix overlooked parameters
  * Fix typo in simulator module (`#439 <https://github.com/autowarefoundation/autoware.universe/issues/439>`_)
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Format launch files (`#1219 <https://github.com/autowarefoundation/autoware.universe/issues/1219>`_)
  * Fix rolling build errors (`#1225 <https://github.com/autowarefoundation/autoware.universe/issues/1225>`_)
  * Add missing include files
  * Replace rclcpp::Duration
  * Use reference for exceptions
  * Use from_seconds
  * Sync public repo (`#1228 <https://github.com/autowarefoundation/autoware.universe/issues/1228>`_)
  * [simple_planning_simulator] add readme (`#424 <https://github.com/autowarefoundation/autoware.universe/issues/424>`_)
  * add readme of simple_planning_simulator
  * Update simulator/simple_planning_simulator/README.md
  * set transit_margin_time to intersect. planner (`#460 <https://github.com/autowarefoundation/autoware.universe/issues/460>`_)
  * Fix pose2twist (`#462 <https://github.com/autowarefoundation/autoware.universe/issues/462>`_)
  * Ros2 vehicle info param server (`#447 <https://github.com/autowarefoundation/autoware.universe/issues/447>`_)
  * add vehicle_info_param_server
  * update vehicle info
  * apply format
  * fix bug
  * skip unnecessary search
  * delete vehicle param file
  * fix bug
  * Ros2 fix topic name part2 (`#425 <https://github.com/autowarefoundation/autoware.universe/issues/425>`_)
  * Fix topic name of traffic_light_classifier
  * Fix topic name of traffic_light_visualization
  * Fix topic name of traffic_light_ssd_fine_detector
  * Fix topic name of traffic_light_map_based_detector
  * Fix lint traffic_light_recognition
  * Fix lint traffic_light_ssd_fine_detector
  * Fix lint traffic_light_classifier
  * Fix lint traffic_light_classifier
  * Fix lint traffic_light_ssd_fine_detector
  * Fix issues in hdd_reader (`#466 <https://github.com/autowarefoundation/autoware.universe/issues/466>`_)
  * Fix some issues detected by Coverity Scan and Clang-Tidy
  * Update launch command
  * Add more `close(new_sock)`
  * Simplify the definitions of struct
  * fix: re-construct laneletMapLayer for reindex RTree (`#463 <https://github.com/autowarefoundation/autoware.universe/issues/463>`_)
  * Rviz overlay render fix (`#461 <https://github.com/autowarefoundation/autoware.universe/issues/461>`_)
  * Moved painiting in SteeringAngle plugin to update()
  * super class now back to MFD
  * uncrustified
  * acquire data in mutex
  * back to RTD as superclass
  * Rviz overlay render in update (`#465 <https://github.com/autowarefoundation/autoware.universe/issues/465>`_)
  * Moved painiting in SteeringAngle plugin to update()
  * super class now back to MFD
  * uncrustified
  * acquire data in mutex
  * removed unnecessary includes and some dead code
  * Adepted remaining vehicle plugin classes to render-in-update concept. Returned to MFD superclass
  * restored RTD superclass
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Refactor vehicle info util (`#1305 <https://github.com/autowarefoundation/autoware.universe/issues/1305>`_)
  * Update license
  * Refactor vehicle_info_util
  * Rename and split files
  * Fix interfaces
  * Fix bug and add error handling
  * Add "// namespace"
  * Add missing include
  * Fix lint errors (`#1378 <https://github.com/autowarefoundation/autoware.universe/issues/1378>`_)
  * Fix lint errors
  * Fix variable names
  * Add pre-commit (`#1560 <https://github.com/autowarefoundation/autoware.universe/issues/1560>`_)
  * add pre-commit
  * add pre-commit-config
  * add additional settings for private repository
  * use default pre-commit-config
  * update pre-commit setting
  * Ignore whitespace for line breaks in markdown
  * Update .github/workflows/pre-commit.yml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * exclude svg
  * remove pretty-format-json
  * add double-quote-string-fixer
  * consider COLCON_IGNORE file when seaching modified package
  * format file
  * pre-commit fixes
  * Update pre-commit.yml
  * Update .pre-commit-config.yaml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Add markdownlint and prettier (`#1661 <https://github.com/autowarefoundation/autoware.universe/issues/1661>`_)
  * Add markdownlint and prettier
  * Ignore .param.yaml
  * Apply format
  * add cov pub in psim (`#1732 <https://github.com/autowarefoundation/autoware.universe/issues/1732>`_)
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * fix some typos (`#1941 <https://github.com/autowarefoundation/autoware.universe/issues/1941>`_)
  * fix some typos
  * fix typo
  * Fix typo
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * Add autoware api (`#1979 <https://github.com/autowarefoundation/autoware.universe/issues/1979>`_)
  * add sort-package-xml hook in pre-commit (`#1881 <https://github.com/autowarefoundation/autoware.universe/issues/1881>`_)
  * add sort xml hook in pre-commit
  * change retval to exit_status
  * rename
  * add prettier plugin-xml
  * use early return
  * add license note
  * add tier4 license
  * restore prettier
  * change license order
  * move local hooks to public repo
  * move prettier-xml to pre-commit-hooks-ros
  * update version for bug-fix
  * apply pre-commit
  * Feature/add ideal accel model interface (`#1894 <https://github.com/autowarefoundation/autoware.universe/issues/1894>`_)
  * Add IDEAL_ACCEL model interface for simple planning simulator
  * Add IDEAL_ACCEL model descriptions
  * Fix format
  * Change vehicle model type description at config file
  * Change formatter to clang-format and black (`#2332 <https://github.com/autowarefoundation/autoware.universe/issues/2332>`_)
  * Revert "Temporarily comment out pre-commit hooks"
  This reverts commit 748e9cdb145ce12f8b520bcbd97f5ff899fc28a3.
  * Replace ament_lint_common with autoware_lint_common
  * Remove ament_cmake_uncrustify and ament_clang_format
  * Apply Black
  * Apply clang-format
  * Fix build errors
  * Fix for cpplint
  * Fix include double quotes to angle brackets
  * Apply clang-format
  * Fix build errors
  * Add COLCON_IGNORE (`#500 <https://github.com/autowarefoundation/autoware.universe/issues/500>`_)
  * Back port .auto control packages (`#571 <https://github.com/autowarefoundation/autoware.universe/issues/571>`_)
  * Implement Lateral and Longitudinal Control Muxer
  * [`#570 <https://github.com/autowarefoundation/autoware.universe/issues/570>`_] Porting wf_simulator
  * [`#1189 <https://github.com/autowarefoundation/autoware.universe/issues/1189>`_] Deactivate flaky test in 'trajectory_follower_nodes'
  * [`#1189 <https://github.com/autowarefoundation/autoware.universe/issues/1189>`_] Fix flacky test in 'trajectory_follower_nodes/latlon_muxer'
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Add osqp_interface package
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Add library code for MPC-based lateral control
  * [`#1271 <https://github.com/autowarefoundation/autoware.universe/issues/1271>`_] Use std::abs instead of abs
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Implement Lateral Controller for Cargo ODD
  * [`#1246 <https://github.com/autowarefoundation/autoware.universe/issues/1246>`_] Resolve "Test case names currently use snake_case but should be CamelCase"
  * [`#1325 <https://github.com/autowarefoundation/autoware.universe/issues/1325>`_] Deactivate flaky smoke test in 'trajectory_follower_nodes'
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add library code of longitudinal controller
  * Fix build error for trajectory follower
  * Fix build error for trajectory follower nodes
  * [`#1272 <https://github.com/autowarefoundation/autoware.universe/issues/1272>`_] Add AckermannControlCommand support to simple_planning_simulator
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add Longitudinal Controller node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Rename velocity_controller -> longitudinal_controller
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Update CMakeLists.txt for the longitudinal_controller_node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add smoke test python launch file
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use LowPassFilter1d from trajectory_follower
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use autoware_auto_msgs
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Changes for .auto (debug msg tmp fix, common func, tf listener)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Remove unused parameters
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix ros test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Rm default params from declare_parameters + use autoware types
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use default param file to setup NodeOptions in the ros test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix docstring
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Replace receiving a Twist with a VehicleKinematicState
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Change class variables format to m\_ prefix
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix plugin name of LongitudinalController in CMakeLists.txt
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix copyright dates
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Reorder includes
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add some tests (~89% coverage without disabling flaky tests)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add more tests (90+% coverage without disabling flaky tests)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use Float32MultiArrayDiagnostic message for debug and slope
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Calculate wheel_base value from vehicle parameters
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Cleanup redundant logger setting in tests
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Set ROS_DOMAIN_ID when running tests to prevent CI failures
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Remove TF listener and use published vehicle state instead
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Change smoke tests to use autoware_testing
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add plotjuggler cfg for both lateral and longitudinal control
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Improve design documents
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Disable flaky test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Properly transform vehicle state in longitudinal node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix TF buffer of lateral controller
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Tuning of lateral controller for LGSVL
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix formating
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix /tf_static sub to be transient_local
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix yaw recalculation of reverse trajs in the lateral controller
  * modify trajectory_follower for galactic build
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update trajectory_follower
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update simple_planning_simulator
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update trajectory_follower_nodes
  * apply trajectory msg modification in control
  * move directory
  * remote control/trajectory_follower level dorectpry
  * remove .iv trajectory follower
  * use .auto trajectory_follower
  * remove .iv simple_planning_simulator & osqp_interface
  * use .iv simple_planning_simulator & osqp_interface
  * add tmp_autoware_auto_dependencies
  * tmporally add autoware_auto_msgs
  * apply .auto message split
  * fix build depend
  * fix packages using osqp
  * fix autoware_auto_geometry
  * ignore lint of some packages
  * ignore ament_lint of some packages
  * ignore lint/pre-commit of trajectory_follower_nodes
  * disable unit tests of some packages
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  * [simple planning simulator]change type of msg (`#590 <https://github.com/autowarefoundation/autoware.universe/issues/590>`_)
  * remove kinematic_state
  * remove vehicle_state_command/report
  * get z-position from trajectory
  * set topic name of trajectory
  * twist -> velocity report
  * change default param
  * Update simulator/simple_planning_simulator/test/test_simple_planning_simulator.cpp
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * Update simulator/simple_planning_simulator/include/simple_planning_simulator/simple_planning_simulator_core.hpp
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * fix typo
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * [autoware_vehicle_rviz_plugin/route_handler/simple_planning_simulator]fix some packages (`#606 <https://github.com/autowarefoundation/autoware.universe/issues/606>`_)
  * fix console meter
  * fix velocity_history
  * fix route handler
  * change topic name
  * update to support velocity report header (`#655 <https://github.com/autowarefoundation/autoware.universe/issues/655>`_)
  * update to support velocity report header
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/simple_planning_simulator_core.cpp
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * use maybe_unused
  * fix precommit
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * adapt to actuation cmd/status as control msg (`#646 <https://github.com/autowarefoundation/autoware.universe/issues/646>`_)
  * adapt to actuation cmd/status as control msg
  * fix readme
  * fix topics
  * fix remaing topics
  * as to pacmod interface
  * fix vehicle status
  * add header to twist
  * revert gyro_odometer_change
  * revert twist topic change
  * revert unchanged package
  * FIx vehicle status topic name/type (`#658 <https://github.com/autowarefoundation/autoware.universe/issues/658>`_)
  * shift -> gear_status
  * twist -> velocity_status
  * fix topic name (`#674 <https://github.com/autowarefoundation/autoware.universe/issues/674>`_)
  * fix topic name
  * fix gear message name
  * Fix psim param path (`#696 <https://github.com/autowarefoundation/autoware.universe/issues/696>`_)
  * Fix/psim topics emergency handler awapi (`#702 <https://github.com/autowarefoundation/autoware.universe/issues/702>`_)
  * fix emergency handler
  * fix awapi
  * remove unused topic
  * remove duplecated vehicle cmd
  * Auto/add turn indicators and hazards (`#717 <https://github.com/autowarefoundation/autoware.universe/issues/717>`_)
  * add turn indicators
  * add hazard light
  * omit name space
  * remap topic name
  * delete unnecessary blank line
  * [simple_planning_simulator]fix bug (`#727 <https://github.com/autowarefoundation/autoware.universe/issues/727>`_)
  * input z-axis of trajectory to pose(tf/odometry)
  * output 0 velocity when invalid gear is input
  * fix gear process in sim (`#728 <https://github.com/autowarefoundation/autoware.universe/issues/728>`_)
  * Fix for integration test (`#732 <https://github.com/autowarefoundation/autoware.universe/issues/732>`_)
  * Add backward compatibility of autoware state
  * Add simulator initial pose service
  * Fix pre-commit
  * Fix pre-commit
  * Simple planning simulator update for latest develop (`#735 <https://github.com/autowarefoundation/autoware.universe/issues/735>`_)
  * Refactor vehicle info util (`#1305 <https://github.com/autowarefoundation/autoware.universe/issues/1305>`_)
  * add cov pub in psim (`#1732 <https://github.com/autowarefoundation/autoware.universe/issues/1732>`_)
  * remove pose_with_covariance publisher and add covariance information in Odometry
  * Fix acceleration for reverse (`#737 <https://github.com/autowarefoundation/autoware.universe/issues/737>`_)
  * Fix acceleration for reverse
  * Fix acceleration in set_input
  * remove unused using
  * Fix code
  * ci(pre-commit): autofix
  * remove tests
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Kosuke Murakami <kosuke.murakami@tier4.jp>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Makoto Kurihara <mkuri8m@gmail.com>
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Sugatyon <32741405+Sugatyon@users.noreply.github.com>
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Ahmed Ebrahim, Daisuke Nishimatsu, Esteve Fernandez, Haoru Xue, Hiroki OTA, Kenji Miyake, Kosuke Takeuchi, Mamoru Sobue, Maxime CLEMENT, Satoshi OTA, Satoshi Tanaka, Shumpei Wakabayashi, Takagi, Isamu, Takamasa Horibe, Takayuki Murooka, Tomoya Kimura, Vincent Richard, Yukihiro Saito, awf-autoware-bot[bot], danielsanchezaran, kyoichi-sugahara, taikitanaka3

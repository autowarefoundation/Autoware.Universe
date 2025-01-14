^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_simulator_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(vehicle_velocity_converter)!: prefix package and namespace with autoware (`#8967 <https://github.com/autowarefoundation/autoware.universe/issues/8967>`_)
  * add autoware prefix
  * fix conflict
  ---------
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(tier4_metric_msgs): apply tier4_metric_msgs for scenario_simulator_v2_adapter, control_evaluator, planning_evaluator, autonomous_emergency_braking, obstacle_cruise_planner, motion_velocity_planner, processing_time_checker (`#9180 <https://github.com/autowarefoundation/autoware.universe/issues/9180>`_)
  * first commit
  * fix building errs.
  * change diagnostic messages to metric messages for publishing decision.
  * fix bug about motion_velocity_planner
  * change the diagnostic msg to metric msg in autoware_obstacle_cruise_planner.
  * tmp save for planning_evaluator
  * change the topic to which metrics published to.
  * fix typo.
  * remove unnesessary publishing of metrics.
  * mke planning_evaluator publish msg of MetricArray instead of Diags.
  * update aeb with metric type for decision.
  * fix some bug
  * remove autoware_evaluator_utils package.
  * remove diagnostic_msgs dependency of planning_evaluator
  * use metric_msgs for autoware_processing_time_checker.
  * rewrite diagnostic_convertor to scenario_simulator_v2_adapter, supporting metric_msgs.
  * pre-commit and fix typo
  * publish metrics even if there is no metric in the MetricArray.
  * modify the metric name of processing_time.
  * update unit test for test_planning/control_evaluator
  * manual pre-commit
  ---------
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kem (TiankuiXian), Masaki Baba, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(tier4_metric_msgs): apply tier4_metric_msgs for scenario_simulator_v2_adapter, control_evaluator, planning_evaluator, autonomous_emergency_braking, obstacle_cruise_planner, motion_velocity_planner, processing_time_checker (`#9180 <https://github.com/autowarefoundation/autoware.universe/issues/9180>`_)
  * first commit
  * fix building errs.
  * change diagnostic messages to metric messages for publishing decision.
  * fix bug about motion_velocity_planner
  * change the diagnostic msg to metric msg in autoware_obstacle_cruise_planner.
  * tmp save for planning_evaluator
  * change the topic to which metrics published to.
  * fix typo.
  * remove unnesessary publishing of metrics.
  * mke planning_evaluator publish msg of MetricArray instead of Diags.
  * update aeb with metric type for decision.
  * fix some bug
  * remove autoware_evaluator_utils package.
  * remove diagnostic_msgs dependency of planning_evaluator
  * use metric_msgs for autoware_processing_time_checker.
  * rewrite diagnostic_convertor to scenario_simulator_v2_adapter, supporting metric_msgs.
  * pre-commit and fix typo
  * publish metrics even if there is no metric in the MetricArray.
  * modify the metric name of processing_time.
  * update unit test for test_planning/control_evaluator
  * manual pre-commit
  ---------
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Kem (TiankuiXian), Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* chore(simple_planning_simulator): add stop_filter_param_path (`#9127 <https://github.com/autowarefoundation/autoware.universe/issues/9127>`_)
* refactor(pose_initializer)!: prefix package and namespace with autoware (`#8701 <https://github.com/autowarefoundation/autoware.universe/issues/8701>`_)
  * add autoware\_ prefix
  * fix link
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* feat(psim)!: preapre settings to launch localization modules on psim (`#8212 <https://github.com/autowarefoundation/autoware.universe/issues/8212>`_)
* feat(psim)!: change a setting parameter type from bool to string (`#8331 <https://github.com/autowarefoundation/autoware.universe/issues/8331>`_)
  * change a param type, bool to string
  * add param description, add null tag group for the null option
  ---------
* feat(evalautor): rename evaluator diag topics (`#8152 <https://github.com/autowarefoundation/autoware.universe/issues/8152>`_)
  * feat(evalautor): rename evaluator diag topics
  * perception
  ---------
* refactor(elevation_map_loader): add package name prefix `autoware\_`, fix namespace and directory structure (`#7988 <https://github.com/autowarefoundation/autoware.universe/issues/7988>`_)
  * refactor: add namespace, remove unused dependencies, file structure
  chore: remove unused dependencies
  style(pre-commit): autofix
  * refactor: rename elevation_map_loader to autoware_elevation_map_loader
  Rename the `elevation_map_loader` package to `autoware_elevation_map_loader` to align with the Autoware naming convention.
  style(pre-commit): autofix
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
* feat(processing_time_checker): add a new package (`#7957 <https://github.com/autowarefoundation/autoware.universe/issues/7957>`_)
  * feat(processing_time_checker): add a new package
  * fix
  * fix
  * update README and schema.json
  * fix
  * fix
  * fix
  ---------
* feat(tier4_perception_launch): add missing arg use_multi_channel_tracker_merger (`#7705 <https://github.com/autowarefoundation/autoware.universe/issues/7705>`_)
  * feat(tier4_perception_launch): add missing arg use_multi_channel_tracker_merger
  * feat: add use_multi_channel_tracker_merger argument to simulator launch
  This commit adds the `use_multi_channel_tracker_merger` argument to the simulator launch file. The argument is set to `false` by default. This change enables the use of the multi-channel tracker merger in the simulator.
  ---------
* feat(diagnostic_converter): fix output metrics topic name and add to converter (`#7495 <https://github.com/autowarefoundation/autoware.universe/issues/7495>`_)
* feat(perception_online_evaluator): add use_perception_online_evaluator option and disable it by default (`#6861 <https://github.com/autowarefoundation/autoware.universe/issues/6861>`_)
* Contributors: Kosuke Takeuchi, Masaki Baba, Taekjin LEE, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* fix(pose_initializer): added "user_defined_initial_pose" to dummy localization (`#6723 <https://github.com/autowarefoundation/autoware.universe/issues/6723>`_)
  Added "used_defined_initial_pose" to dummy localization
* feat(default_ad_api): add door api (`#5737 <https://github.com/autowarefoundation/autoware.universe/issues/5737>`_)
* feat(tier4_simulator_launch): add option to disable all perception related modules (`#6382 <https://github.com/autowarefoundation/autoware.universe/issues/6382>`_)
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
* fix(tier4_simulator_launch): add lacked param path (`#5326 <https://github.com/autowarefoundation/autoware.universe/issues/5326>`_)
* chore(tier4_simulator_launch): launch camera and V2X fusion module in simple planning simulator (`#4522 <https://github.com/autowarefoundation/autoware.universe/issues/4522>`_)
* feat: use `pose_source` and `twist_source` for selecting localization methods (`#4257 <https://github.com/autowarefoundation/autoware.universe/issues/4257>`_)
  * feat(tier4_localization_launch): add pose_twist_estimator.launch.py
  * update format
  * update launcher
  * update pose_initailizer config
  * Move pose_initializer to pose_twist_estimator.launch.py, move yabloc namespace
  * use launch.xml instead of launch.py
  * Validated that all the configuration launches correctly (not performance eval yet)
  * Remove arg
  * style(pre-commit): autofix
  * Update eagleye param path
  * minor update
  * fix minor bugs
  * fix minor bugs
  * Introduce use_eagleye_twist args in eagleye_rt.launch.xml to control pose/twist relay nodes
  * Update pose_initializer input topic when using eagleye
  * Add eagleye dependency in tier4_localization_launch
  * Update tier4_localization_launch readme
  * style(pre-commit): autofix
  * Update svg
  * Update svg again (transparent background)
  * style(pre-commit): autofix
  * Update yabloc document
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(occpuancy grid map): move param to yaml (`#4038 <https://github.com/autowarefoundation/autoware.universe/issues/4038>`_)
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(dummy_perception_publisher): add parameter to configure z pose of dummy object (`#3457 <https://github.com/autowarefoundation/autoware.universe/issues/3457>`_)
* refactor(occupancy_grid_map): add occupancy_grid_map method/param var to launcher (`#3393 <https://github.com/autowarefoundation/autoware.universe/issues/3393>`_)
  * add occcupancy_grid_map method/param var to launcher
  * added CODEOWNER
  * Revert "added CODEOWNER"
  This reverts commit 2213c2956af19580d0a7788680aab321675aab3b.
  * add maintainer
  ---------
* fix(tier4_simulator_launch): fix launch package name (`#3340 <https://github.com/autowarefoundation/autoware.universe/issues/3340>`_)
* feat(tier4_simulator_launch): convert /diagnostics_err (`#3152 <https://github.com/autowarefoundation/autoware.universe/issues/3152>`_)
* bugfix(tier4_simulator_launch): fix occupancy grid map not appearing problem in psim  (`#3081 <https://github.com/autowarefoundation/autoware.universe/issues/3081>`_)
  * fixed psim occupancy grid map problem
  * fix parameter designation
  ---------
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* chore(tier4_simulator_launch): add code owner (`#3080 <https://github.com/autowarefoundation/autoware.universe/issues/3080>`_)
  chore(tier4_simulator_launch): add code owners
* fix(tier4_perception_launch): fix config path (`#3078 <https://github.com/autowarefoundation/autoware.universe/issues/3078>`_)
  * fix(tier4_perception_launch): fix config path
  * use pointcloud_based_occupancy_grid_map.launch.py in tier4_simulator_launch
  ---------
* feat(pose_initializer): enable pose initialization while running (only for sim) (`#3038 <https://github.com/autowarefoundation/autoware.universe/issues/3038>`_)
  * feat(pose_initializer): enable pose initialization while running (only for sim)
  * both logsim and psim params
  * only one pose_initializer_param_path arg
  * use two param files for pose_initializer
  ---------
* feat(diagnostic_converter): add converter to use planning_evaluator's output for scenario's condition (`#2514 <https://github.com/autowarefoundation/autoware.universe/issues/2514>`_)
  * add original diagnostic_convertor
  * add test
  * fix typo
  * delete file
  * change include
  * temp
  * delete comments
  * made launch for converter
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * add diagnostic convertor in launch
  * ci(pre-commit): autofix
  * change debug from info
  * change arg name to launch diagnostic convertor
  * add planning_evaluator launcher in simulator.launch.xml
  * fix arg wrong setting
  * style(pre-commit): autofix
  * use simulation msg in tier4_autoware_msgs
  * style(pre-commit): autofix
  * fix README
  * style(pre-commit): autofix
  * refactoring
  * style(pre-commit): autofix
  * remove unnecessary dependency
  * remove unnecessary dependency
  * move folder
  * reformat
  * style(pre-commit): autofix
  * Update evaluator/diagnostic_converter/include/converter_node.hpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Update evaluator/diagnostic_converter/README.md
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Update evaluator/diagnostic_converter/src/converter_node.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Update evaluator/diagnostic_converter/test/test_converter_node.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * define diagnostic_topics as parameter
  * fix include way
  * fix include way
  * delete ament_cmake_clang_format from package.xml
  * fix test_depend
  * Update evaluator/diagnostic_converter/test/test_converter_node.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * style(pre-commit): autofix
  * Update launch/tier4_simulator_launch/launch/simulator.launch.xml
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* feat(pose_initilizer): support gnss/imu pose estimator (`#2904 <https://github.com/autowarefoundation/autoware.universe/issues/2904>`_)
  * Support GNSS/IMU pose estimator
  * style(pre-commit): autofix
  * Revert gnss/imu support
  * Support GNSS/IMU pose estimator
  * style(pre-commit): autofix
  * Separate EKF and NDT trigger modules
  * Integrate activate and deactivate into sendRequest
  * style(pre-commit): autofix
  * Change sendRequest function arguments
  * style(pre-commit): autofix
  * Remove unused conditional branches
  * Fix command name
  * Change to snake_case
  * Fix typos
  * Update localization/pose_initializer/src/pose_initializer/ekf_localization_trigger_module.cpp
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * Update localization/pose_initializer/src/pose_initializer/ndt_localization_trigger_module.cpp
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * Update copyright year
  * Set the copyright year of ekf_localization_module to 2022
  * Delete unnecessary conditional branches
  * Add ekf_enabled parameter
  * Add #include <string>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Ryohei Sasaki <ryohei.sasaki@map4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* feat(tier4_simulator_launch): remove configs and move to autoware_launch (`#2541 <https://github.com/autowarefoundation/autoware.universe/issues/2541>`_)
  * feat(tier4_perception_launch): remove configs and move to autoware_launch
  * update readme
  * first commit
  * remove config
* fix(tier4_simulator_launch): fix path (`#2281 <https://github.com/autowarefoundation/autoware.universe/issues/2281>`_)
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* feat(tier4_simulator_launch): manual sync with tier4/autoware_launch.*/simulator_launch (`#1820 <https://github.com/autowarefoundation/autoware.universe/issues/1820>`_)
  * feat(tier4_simulator_launch): manual sync with tier4/autoware_launch.*/simulator_launch
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat(tier4_simulator_launch): declare param path argument (`#1443 <https://github.com/autowarefoundation/autoware.universe/issues/1443>`_)
  feat(tier4_simulator_launch): declare param path
* feat!: replace ogm at scenario simulation (`#1062 <https://github.com/autowarefoundation/autoware.universe/issues/1062>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* feat: pointcloud based probabilistic occupancy grid map (`#624 <https://github.com/autowarefoundation/autoware.universe/issues/624>`_)
  * initial commit
  * ci(pre-commit): autofix
  * change param
  * update readme
  * add launch
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * fix typo
  * update readme
  * ci(pre-commit): autofix
  * cosmetic change
  * add single frame mode
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: move empty_objects_publisher (`#613 <https://github.com/autowarefoundation/autoware.universe/issues/613>`_)
  * feat: move empty_objects_publisher
  * fix group of empty_object_publisher
* feat(tier4_simulator_launch, dummy_perception_publisher): launch perception modules from simulator.launch.xml (`#465 <https://github.com/autowarefoundation/autoware.universe/issues/465>`_)
  * feat(tier4_simulator_launch, dummy_perception_publisher): launch perception modules from simualtor.launch.xml
  * remove perception launching dummy_perception_publisher.launch.xml
  * remove unnecessary comment
* fix(tier4_simulator_launch, tier4_vehicle_launch)!: fix launch args (`#443 <https://github.com/autowarefoundation/autoware.universe/issues/443>`_)
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
* Contributors: Berkay Karaman, Kenji Miyake, Kosuke Takeuchi, Kyoichi Sugahara, Mamoru Sobue, SakodaShintaro, Satoshi OTA, Takagi, Isamu, Takayuki Murooka, Tomohito ANDO, Tomoya Kimura, Vincent Richard, Yoshi Ri, Yukihiro Saito, kminoda, ryohei sasaki, taikitanaka3

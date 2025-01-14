^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_testing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
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
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

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
* Contributors: Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* chore: add maintainer (`#4234 <https://github.com/autowarefoundation/autoware.universe/issues/4234>`_)
  * chore: add maintainer
  * Update evaluator/localization_evaluator/package.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* fix(smoke_test): fix smoke test failing with big nodes (`#3287 <https://github.com/autowarefoundation/autoware.universe/issues/3287>`_)
  * fix(smoke_test): fix smoke test failing with big nodes
  * style(pre-commit): autofix
  * minor style changes and comment removal
  * moved params to right files
  * added smoke test support for multiple tests with different params
  added general support for multiple smoke tests for one node with different params and added tests for trajectory_follower_node with different controllers
  * style(pre-commit): autofix
  * Parameters structure copied from autoware launch
  * Fixed GTests and Readme
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* build(autoware_testing): add missing dependencies (`#3093 <https://github.com/autowarefoundation/autoware.universe/issues/3093>`_)
* ci(pre-commit): autoupdate (`#2819 <https://github.com/autowarefoundation/autoware.universe/issues/2819>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* test(autoware_testing): fix smoke_test (`#479 <https://github.com/autowarefoundation/autoware.universe/issues/479>`_)
  * fix(autoware_testing): fix smoke_test
  * restore smoke_test for trajectory_follower_nodes
  * add support multiple parameter files
  * ci(pre-commit): autofix
  * minor fix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* chore: remove license notations from CMakeLists.txt (`#846 <https://github.com/autowarefoundation/autoware.universe/issues/846>`_)
* chore: remove bad chars (`#845 <https://github.com/autowarefoundation/autoware.universe/issues/845>`_)
* docs(autoware_testing): fix link (`#741 <https://github.com/autowarefoundation/autoware.universe/issues/741>`_)
  * docs(autoware_testing): fix link
  * fix typo
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
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
* feat: add autoware auto dependencies (`#185 <https://github.com/autowarefoundation/autoware.universe/issues/185>`_)
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
  * Port parking planner packages from .Auto (`#600 <https://github.com/autowarefoundation/autoware.universe/issues/600>`_)
  * Copy code of 'vehicle_constants_manager'
  * Fix vehicle_constants_manager for ROS galactic
  * Rm .iv costmap_generator freespace_planner freespace_planning_aglorihtms
  * Add astar_search (from .Auto)
  * Copy freespace_planner from .Auto
  * Update freespace_planner for .IV
  * Copy costmap_generator from .Auto
  * Copy and update had_map_utils from .Auto
  * Update costmap_generator
  * Copy costmap_generator_nodes
  * Update costmap_generator_nodes
  * Comment out all tests
  * Move vehicle_constant_managers to tmp_autoware_auto_dependencies
  * ignore pre-commit for back-ported packages
  * ignore testing
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * fix: fix pre-commit
  * fix: fix markdownlint
  * fix: fix cpplint
  * feat: remove autoware_auto_dependencies
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* Contributors: Esteve Fernandez, Keisuke Shima, Kenji Miyake, Mateusz Palczuk, Satoshi OTA, Shumpei Wakabayashi, Takeshi Miura, Vincent Richard, pre-commit-ci[bot]

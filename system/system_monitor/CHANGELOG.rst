^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package system_monitor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* ci(pre-commit): update cpplint to 2.0.0 (`#9557 <https://github.com/autowarefoundation/autoware.universe/issues/9557>`_)
* fix(cpplint): include what you use - system (`#9573 <https://github.com/autowarefoundation/autoware.universe/issues/9573>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(system_monitor): add on/off config for network traffic monitor (`#9069 <https://github.com/autowarefoundation/autoware.universe/issues/9069>`_)
  * feat(system_monitor): add config for network traffic monitor
  * fix: change function name from stop to skip
  ---------
* feat(system_monitor): support loopback network interface (`#9067 <https://github.com/autowarefoundation/autoware.universe/issues/9067>`_)
  * feat(system_monitor): support loopback network interface
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, awf-autoware-bot[bot], iwatake

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(system_monitor): add on/off config for network traffic monitor (`#9069 <https://github.com/autowarefoundation/autoware.universe/issues/9069>`_)
  * feat(system_monitor): add config for network traffic monitor
  * fix: change function name from stop to skip
  ---------
* feat(system_monitor): support loopback network interface (`#9067 <https://github.com/autowarefoundation/autoware.universe/issues/9067>`_)
  * feat(system_monitor): support loopback network interface
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Yutaka Kondo, iwatake

0.38.0 (2024-11-08)
-------------------
* remove system_monitor/CHANGELOG.rst
* unify package.xml version to 0.37.0
* fix(system_monitor): fix variableScope (`#8448 <https://github.com/autowarefoundation/autoware.universe/issues/8448>`_)
  fix:variableScope
* fix(system_monitor): fix unusedStructMember (`#8401 <https://github.com/autowarefoundation/autoware.universe/issues/8401>`_)
  * fix:unusedStructMember
  * fix:clang format
  * fix:clang format
  ---------
* fix(system_monitor): fix unreadVariable (`#8372 <https://github.com/autowarefoundation/autoware.universe/issues/8372>`_)
  fix:unreadVariable
* fix(system_monitor): fix shadowVariable (`#7981 <https://github.com/autowarefoundation/autoware.universe/issues/7981>`_)
  fix:shadowVariable
* fix(system_monitor): apply cppcheck-suppress for cstyleCast (`#7867 <https://github.com/autowarefoundation/autoware.universe/issues/7867>`_)
  * fix(system_monitor): apply cppcheck-suppress for cstyleCast
  * fix(system_monitor): apply cppcheck-suppress for cstyleCast
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(net_monitor): fix cppcheck warnings (`#7573 <https://github.com/autowarefoundation/autoware.universe/issues/7573>`_)
  * fix unusedVariable warning
  * fix unusedVariable warning
  * fix variableScope warning
  * fix unreadVariable warning
  * fix
  ---------
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(system_monitor): fix unsignedLessThanZero warning (`#7545 <https://github.com/autowarefoundation/autoware.universe/issues/7545>`_)
* ci(pre-commit): autoupdate (`#7499 <https://github.com/autowarefoundation/autoware.universe/issues/7499>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* fix(system_monitor): fix warning of containerOutOfBounds (`#6927 <https://github.com/autowarefoundation/autoware.universe/issues/6927>`_)
* Contributors: Koichi98, Kosuke Takeuchi, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, awf-autoware-bot[bot], kobayu858

0.26.0 (2024-04-03)
-------------------
* fix(system_monitor): move headers to a separate directory (`#5942 <https://github.com/autowarefoundation/autoware.universe/issues/5942>`_)
  * fix(system_monitor): move headers to a separate directory
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(system_monitor): fix uninitialized diag level of process monitor (`#5753 <https://github.com/autowarefoundation/autoware.universe/issues/5753>`_)
* chore: update maintainer (`#5730 <https://github.com/autowarefoundation/autoware.universe/issues/5730>`_)
  update maintainer
* fix(system_monitor): output command line (`#5430 <https://github.com/autowarefoundation/autoware.universe/issues/5430>`_)
  * fix(system_monitor): output command line
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* perf(system_monitor): fix program command line reading (`#5191 <https://github.com/autowarefoundation/autoware.universe/issues/5191>`_)
  * Fix program command line reading
  * style(pre-commit): autofix
  * fix spelling commandline->command_line
  ---------
  Co-authored-by: Owen-Liuyuxuan <uken.ryu@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(ntp_monitor): move chronyc command execution to a timer (`#4634 <https://github.com/autowarefoundation/autoware.universe/issues/4634>`_)
  * fix(ntp_monitor): move chronyc command execution to a timer
  * add newly added parameter timeout to config
  ---------
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
* fix(system_monitor): high-memory process are not provided in MEM order (`#4654 <https://github.com/autowarefoundation/autoware.universe/issues/4654>`_)
  * fix(process_monitor): high-memory process are not being provided in %MEM order
  * changed option from 'g' to 'n'
  ---------
* fix(system_monitor): extend command line to display (`#4553 <https://github.com/autowarefoundation/autoware.universe/issues/4553>`_)
* feat(system_monitor): add detection of ECC memory errors (`#3795 <https://github.com/autowarefoundation/autoware.universe/issues/3795>`_)
  * feat(system_monitor): add detection of ECC memory errors
  * style(pre-commit): autofix
  * fix process crash when edac-utils is not installed
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(perception): remove UB reinterpret_cast (`#3383 <https://github.com/autowarefoundation/autoware.universe/issues/3383>`_)
  * fix(perception): remove UB reinterpret_cast
  see https://github.com/autowarefoundation/autoware.universe/issues/3215
  * fix(pointcloud_preprocessor): remove UB reinterpret_cast
  * refactor
  ---------
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
* chore: sync files (`#3227 <https://github.com/autowarefoundation/autoware.universe/issues/3227>`_)
  * chore: sync files
  * style(pre-commit): autofix
  ---------
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* build(system_monitor): added missing Boost dependencies (`#2881 <https://github.com/autowarefoundation/autoware.universe/issues/2881>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
* build(system_monitor): add build dependency (`#2740 <https://github.com/autowarefoundation/autoware.universe/issues/2740>`_)
* fix(system_monitor): change default param path (`#2560 <https://github.com/autowarefoundation/autoware.universe/issues/2560>`_)
* fix(system_monitor): prevent nethogs from monitoring all networks due to high CPU load (`#2474 <https://github.com/autowarefoundation/autoware.universe/issues/2474>`_)
  * fix(system_monitor): prevent nethogs from monitoring all networks due to high CPU load
  * ci(pre-commit): autofix
  * fix(system_monitor): fix include guards
  * fix(system_monitor): fix build error
  * fix(net_monitor): change lower camel case to snake case
  * fix(net_monitor): fix clang-tidy errors and warnings
  * ci(pre-commit): autofix
  * fix(net_monitor): fix clang-tidy warnings
  * ci(pre-commit): autofix
  * fix(net_monitor: fix clang-tidy warnings)
  * fix(net_monitor): fix clang-tidy warnings
  * fix(net_monitor): change C-style socket to boost::asio
  * fix(net_monitor): fix clang-tidy warnings
  * fix(net_monitor): fix clang-tidy warnings
  * fix(net_monitor): first refactoring
  * fix(net_monitor): refactoring
  * fix(net_monitor): fix clang-tidy errors
  * fix(net_monitor): update README
  * fix(net_monitor): add lock guard to protect variable
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: replace python launch with xml launch for system monitor (`#2430 <https://github.com/autowarefoundation/autoware.universe/issues/2430>`_)
  * feat: replace python launch with xml launch for system monitor
  * ci(pre-commit): autofix
  * update figure
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(system_monitor): add maintainer (`#2420 <https://github.com/autowarefoundation/autoware.universe/issues/2420>`_)
* refactor(system_monitor/hdd_monitor): rename structs and functions (`#2144 <https://github.com/autowarefoundation/autoware.universe/issues/2144>`_)
  * refactor(system_monitor/hdd_monitor): rename structs and functions
  * fix a mistake
* chore(system_monitor): fix typos (`#2142 <https://github.com/autowarefoundation/autoware.universe/issues/2142>`_)
* feat: (system_monitor) adding a node for CMOS battery monitoring (`#1989 <https://github.com/autowarefoundation/autoware.universe/issues/1989>`_)
  * adding document for voltage monitor
  * ci(pre-commit): autofix
  * fixed for the issue of multithread
  * Fixed the lack for  processing of Error case.
  * deleted magic number 200
  * ci(pre-commit): autofix
  * moved voltage_mnitor to tha last
  * minimizing between try-catch.
  * ci(pre-commit): autofix
  * deleted unused files
  * added default vlue of cmos_battery_voltage
  * changed the label name to cmos_battery_label.
  * adding language specified
  * resolved conflict
  * resolved conflict
  * resolved conflict
  * ci(pre-commit): autofix
  * added topics_voltage_monitor.md)
  * ci(pre-commit): autofix
  * chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(dummy_diag_publisher): use anon to make unique node name instead of diag name (`#639 <https://github.com/autowarefoundation/autoware.universe/issues/639>`_)
  * chore: sync files (`#648 <https://github.com/autowarefoundation/autoware.universe/issues/648>`_)
  * chore: sync files
  * Revert "chore: sync files"
  This reverts commit b24f530b48306e16aa285f80a629ce5c5a9ccda7.
  * sync codecov.yaml
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage (`#666 <https://github.com/autowarefoundation/autoware.universe/issues/666>`_)
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(behavior_velocity): avoid insert same point on trajectory utils (`#834 <https://github.com/autowarefoundation/autoware.universe/issues/834>`_)
  * refactor(behavior_velocity_planner): simplify CMakeLists.txt (`#855 <https://github.com/autowarefoundation/autoware.universe/issues/855>`_)
  * docs: fix 404 error caused by typo in url (`#871 <https://github.com/autowarefoundation/autoware.universe/issues/871>`_)
  * docs: fix 404 error caused by typo in url
  * docs: fix typo in url for yolov4
  * fix(image_projection_based_fusion): set imagebuffersize (`#820 <https://github.com/autowarefoundation/autoware.universe/issues/820>`_)
  * fix: set imagebuffersize configured
  * ci(pre-commit): autofix
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore(avoidance_module): fix spell check (`#732 <https://github.com/autowarefoundation/autoware.universe/issues/732>`_)
  * feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
  * docs(virtual traffic light): add documentation (`#245 <https://github.com/autowarefoundation/autoware.universe/issues/245>`_)
  * doc(behavior_velocity): add graph and fix link
  * doc(behavior_velocity): update virtual traffic light doc
  * doc(behavior_velocity): minor fix
  * doc : mediate to coordinate
  * doc: minor update
  * doc: fix pre-commit
  * doc: update docs
  * apply suggestion
  * doc: to intersection-coordination
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat(surround_obstacle_checker): separate surround_obstacle_checker from hierarchical planning flow (`#830 <https://github.com/autowarefoundation/autoware.universe/issues/830>`_)
  * fix(surroud_obstacle_checker): use alias
  * feat(surround_obstacle_checker): use velocity limit
  * chore(surround_obstacle_checker): rename publisher, subscriber and callback functions
  * refactor(surround_obstacle_checker): use parameter struct
  * fix(surround_obstacle_checker): use alias
  * refactor(surround_obstacle_checker): cleanup member functions
  * refactor(surround_obstacle_checker): cleanup polygon handling
  * refactor(surround_obstacle_checker): use marker helper
  * feat(planning_launch): separate surround_obstacle_checker from hierarchical motion planning flow
  * fix(surround_obstacle_checker): fix ego footprint polygon (`#877 <https://github.com/autowarefoundation/autoware.universe/issues/877>`_)
  * fix: update nvinfer api (`#863 <https://github.com/autowarefoundation/autoware.universe/issues/863>`_)
  * fix(lidar_centerpoint): update nvinfer api
  * fix(tensorrt_yolo): update nvinfer api
  * fix(lidar_apollo_instance_segmentation): update nvinfer api
  * fix(traffic_light_classifier): update nvinfer api
  * fix(traffic_light_ssd_fine_detector): update nvinfer api
  * pre-commit run
  * fix(avoidance_module): ignore object instead of creating zero shift (`#731 <https://github.com/autowarefoundation/autoware.universe/issues/731>`_)
  * fix: ignore object instead of creating zero shift
  instead of creating zero shift point, the object will be ignored.
  no behavior changes should be observed.
  * refactor: sync continue with upstream
  * fix: fix debug message for insufficient lateral margin
  * fix(motion_velocity_smoother): curve deceleration not working with a specific parameter set (`#738 <https://github.com/autowarefoundation/autoware.universe/issues/738>`_)
  * test(autoware_testing): fix smoke_test (`#479 <https://github.com/autowarefoundation/autoware.universe/issues/479>`_)
  * fix(autoware_testing): fix smoke_test
  * restore smoke_test for trajectory_follower_nodes
  * add support multiple parameter files
  * ci(pre-commit): autofix
  * minor fix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(rviz_plugins): add velocity limit to autoware state panel (`#879 <https://github.com/autowarefoundation/autoware.universe/issues/879>`_)
  * feat(rviz_plugins): add velocity limit to autoware state panel
  * chore(rviz_plugin): change ms to kmh
  * feat(vehicle_info_util): add max_steer_angle (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * feat(vehicle_info_util): add max_steer_angle
  * applied pre-commit
  * Added max_steer_angle in test config
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  * fix(lidar_centerpoint): fix google drive url to avoid 404 (`#889 <https://github.com/autowarefoundation/autoware.universe/issues/889>`_)
  * fix(lidar_centerpoint): fix google drive url to avoid 404
  * Update CMakeLists.txt
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * chore: fix typos (`#886 <https://github.com/autowarefoundation/autoware.universe/issues/886>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(map_tf_generator): accelerate the 'viewer' coordinate calculation (`#890 <https://github.com/autowarefoundation/autoware.universe/issues/890>`_)
  * add random point sampling function to quickly calculate the 'viewer' coordinate
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * docs(obstacle_stop_planner): update documentation (`#880 <https://github.com/autowarefoundation/autoware.universe/issues/880>`_)
  * ci(pre-commit): autofix
  * fixed conflicts
  * ci(pre-commit): autofix
  * merged fork-origin
  * merged
  * resolve conflict
  * ci(pre-commit): autofix
  * deleted
  * added "Voltage Monitor"
  * merged with main->feature_battery_monitoring
  * merge  main ->feature_battery_monitoring
  * ci(pre-commit): autofix
  * added default vlue of cmos_battery_voltage
  * resolved conflict
  * resolved conflict
  * ci(pre-commit): autofix
  * added topics_voltage_monitor.md)
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(dummy_diag_publisher): use anon to make unique node name instead of diag name (`#639 <https://github.com/autowarefoundation/autoware.universe/issues/639>`_)
  * chore: sync files (`#648 <https://github.com/autowarefoundation/autoware.universe/issues/648>`_)
  * chore: sync files
  * Revert "chore: sync files"
  This reverts commit b24f530b48306e16aa285f80a629ce5c5a9ccda7.
  * sync codecov.yaml
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage (`#666 <https://github.com/autowarefoundation/autoware.universe/issues/666>`_)
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(behavior_velocity): avoid insert same point on trajectory utils (`#834 <https://github.com/autowarefoundation/autoware.universe/issues/834>`_)
  * refactor(behavior_velocity_planner): simplify CMakeLists.txt (`#855 <https://github.com/autowarefoundation/autoware.universe/issues/855>`_)
  * docs: fix 404 error caused by typo in url (`#871 <https://github.com/autowarefoundation/autoware.universe/issues/871>`_)
  * docs: fix 404 error caused by typo in url
  * docs: fix typo in url for yolov4
  * fix(image_projection_based_fusion): set imagebuffersize (`#820 <https://github.com/autowarefoundation/autoware.universe/issues/820>`_)
  * fix: set imagebuffersize configured
  * ci(pre-commit): autofix
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore(avoidance_module): fix spell check (`#732 <https://github.com/autowarefoundation/autoware.universe/issues/732>`_)
  * feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
  * docs(virtual traffic light): add documentation (`#245 <https://github.com/autowarefoundation/autoware.universe/issues/245>`_)
  * doc(behavior_velocity): add graph and fix link
  * doc(behavior_velocity): update virtual traffic light doc
  * doc(behavior_velocity): minor fix
  * doc : mediate to coordinate
  * doc: minor update
  * doc: fix pre-commit
  * doc: update docs
  * apply suggestion
  * doc: to intersection-coordination
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat(surround_obstacle_checker): separate surround_obstacle_checker from hierarchical planning flow (`#830 <https://github.com/autowarefoundation/autoware.universe/issues/830>`_)
  * fix(surroud_obstacle_checker): use alias
  * feat(surround_obstacle_checker): use velocity limit
  * chore(surround_obstacle_checker): rename publisher, subscriber and callback functions
  * refactor(surround_obstacle_checker): use parameter struct
  * fix(surround_obstacle_checker): use alias
  * refactor(surround_obstacle_checker): cleanup member functions
  * refactor(surround_obstacle_checker): cleanup polygon handling
  * refactor(surround_obstacle_checker): use marker helper
  * feat(planning_launch): separate surround_obstacle_checker from hierarchical motion planning flow
  * fix(surround_obstacle_checker): fix ego footprint polygon (`#877 <https://github.com/autowarefoundation/autoware.universe/issues/877>`_)
  * fix: update nvinfer api (`#863 <https://github.com/autowarefoundation/autoware.universe/issues/863>`_)
  * fix(lidar_centerpoint): update nvinfer api
  * fix(tensorrt_yolo): update nvinfer api
  * fix(lidar_apollo_instance_segmentation): update nvinfer api
  * fix(traffic_light_classifier): update nvinfer api
  * fix(traffic_light_ssd_fine_detector): update nvinfer api
  * pre-commit run
  * fix(avoidance_module): ignore object instead of creating zero shift (`#731 <https://github.com/autowarefoundation/autoware.universe/issues/731>`_)
  * fix: ignore object instead of creating zero shift
  instead of creating zero shift point, the object will be ignored.
  no behavior changes should be observed.
  * refactor: sync continue with upstream
  * fix: fix debug message for insufficient lateral margin
  * fix(motion_velocity_smoother): curve deceleration not working with a specific parameter set (`#738 <https://github.com/autowarefoundation/autoware.universe/issues/738>`_)
  * test(autoware_testing): fix smoke_test (`#479 <https://github.com/autowarefoundation/autoware.universe/issues/479>`_)
  * fix(autoware_testing): fix smoke_test
  * restore smoke_test for trajectory_follower_nodes
  * add support multiple parameter files
  * ci(pre-commit): autofix
  * minor fix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(rviz_plugins): add velocity limit to autoware state panel (`#879 <https://github.com/autowarefoundation/autoware.universe/issues/879>`_)
  * feat(rviz_plugins): add velocity limit to autoware state panel
  * chore(rviz_plugin): change ms to kmh
  * feat(vehicle_info_util): add max_steer_angle (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * feat(vehicle_info_util): add max_steer_angle
  * applied pre-commit
  * Added max_steer_angle in test config
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  * fix(lidar_centerpoint): fix google drive url to avoid 404 (`#889 <https://github.com/autowarefoundation/autoware.universe/issues/889>`_)
  * fix(lidar_centerpoint): fix google drive url to avoid 404
  * Update CMakeLists.txt
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * chore: fix typos (`#886 <https://github.com/autowarefoundation/autoware.universe/issues/886>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(map_tf_generator): accelerate the 'viewer' coordinate calculation (`#890 <https://github.com/autowarefoundation/autoware.universe/issues/890>`_)
  * add random point sampling function to quickly calculate the 'viewer' coordinate
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * docs(obstacle_stop_planner): update documentation (`#880 <https://github.com/autowarefoundation/autoware.universe/issues/880>`_)
  * ci(pre-commit): autofix
  * fixed conflicts
  * ci(pre-commit): autofix
  * resolve conflict
  * ci(pre-commit): autofix
  * merged with main->feature_battery_monitoring
  * merge  main ->feature_battery_monitoring
  * Added voltages are provisional values.
  * ci(pre-commit): autofix
  * feat(behavior_path_planner): add turn signal parameters (`#2086 <https://github.com/autowarefoundation/autoware.universe/issues/2086>`_)
  * feat(behavior_path_planner): add and change parameters
  * update
  * update
  * refactor(perception_utils): refactor matching function in perception_utils (`#2045 <https://github.com/autowarefoundation/autoware.universe/issues/2045>`_)
  * refactor(perception_util): refactor matching function in perception_util
  * fix namespace
  * refactor
  * refactor
  * fix bug
  * add const
  * refactor function name
  * refactor(perception_utils): refactor object_classification (`#2042 <https://github.com/autowarefoundation/autoware.universe/issues/2042>`_)
  * refactor(perception_utils): refactor object_classification
  * fix bug
  * fix unittest
  * refactor
  * fix unit test
  * remove redundant else
  * refactor variable name
  * feat(autoware_auto_perception_rviz_plugin): add accel text visualization (`#2046 <https://github.com/autowarefoundation/autoware.universe/issues/2046>`_)
  * refactor(motion_utils, obstacle_cruise_planner): add offset to virtual wall utils func (`#2078 <https://github.com/autowarefoundation/autoware.universe/issues/2078>`_)
  * refactor(osqp_interface, motion_velocity_smoother): unsolved status log (`#2076 <https://github.com/autowarefoundation/autoware.universe/issues/2076>`_)
  * refactor(osqp_interface, motion_velocity_smoother): unsolved status log
  * Update common/osqp_interface/src/osqp_interface.cpp
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * feat(lidar_centerpoint): eliminated the tf dependency for single frame detection (`#1925 <https://github.com/autowarefoundation/autoware.universe/issues/1925>`_)
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * change name hardware_monitor -> voltage_monitor
  * copy right 2020 -> 2022
  * delete duplicated lines
  * fix: catch exception, remove sensors_exists\_
  * adding error message output
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * adding document for voltage monitor
  * fixed for the issue of multithread
  * ci(pre-commit): autofix
  * Fixed the lack for  processing of Error case.
  * deleted magic number 200
  * moved voltage_mnitor to tha last
  * minimizing between try-catch.
  * ci(pre-commit): autofix
  * added default vlue of cmos_battery_voltage
  * changed the label name to cmos_battery_label.
  * adding language specified
  * resolved conflict
  * resolved conflict
  * ci(pre-commit): autofix
  * added topics_voltage_monitor.md)
  * ci(pre-commit): autofix
  * chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(dummy_diag_publisher): use anon to make unique node name instead of diag name (`#639 <https://github.com/autowarefoundation/autoware.universe/issues/639>`_)
  * chore: sync files (`#648 <https://github.com/autowarefoundation/autoware.universe/issues/648>`_)
  * chore: sync files
  * Revert "chore: sync files"
  This reverts commit b24f530b48306e16aa285f80a629ce5c5a9ccda7.
  * sync codecov.yaml
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage (`#666 <https://github.com/autowarefoundation/autoware.universe/issues/666>`_)
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(behavior_velocity): avoid insert same point on trajectory utils (`#834 <https://github.com/autowarefoundation/autoware.universe/issues/834>`_)
  * refactor(behavior_velocity_planner): simplify CMakeLists.txt (`#855 <https://github.com/autowarefoundation/autoware.universe/issues/855>`_)
  * docs: fix 404 error caused by typo in url (`#871 <https://github.com/autowarefoundation/autoware.universe/issues/871>`_)
  * docs: fix 404 error caused by typo in url
  * docs: fix typo in url for yolov4
  * fix(image_projection_based_fusion): set imagebuffersize (`#820 <https://github.com/autowarefoundation/autoware.universe/issues/820>`_)
  * fix: set imagebuffersize configured
  * ci(pre-commit): autofix
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore(avoidance_module): fix spell check (`#732 <https://github.com/autowarefoundation/autoware.universe/issues/732>`_)
  * feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
  * docs(virtual traffic light): add documentation (`#245 <https://github.com/autowarefoundation/autoware.universe/issues/245>`_)
  * doc(behavior_velocity): add graph and fix link
  * doc(behavior_velocity): update virtual traffic light doc
  * doc(behavior_velocity): minor fix
  * doc : mediate to coordinate
  * doc: minor update
  * doc: fix pre-commit
  * doc: update docs
  * apply suggestion
  * doc: to intersection-coordination
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat(surround_obstacle_checker): separate surround_obstacle_checker from hierarchical planning flow (`#830 <https://github.com/autowarefoundation/autoware.universe/issues/830>`_)
  * fix(surroud_obstacle_checker): use alias
  * feat(surround_obstacle_checker): use velocity limit
  * chore(surround_obstacle_checker): rename publisher, subscriber and callback functions
  * refactor(surround_obstacle_checker): use parameter struct
  * fix(surround_obstacle_checker): use alias
  * refactor(surround_obstacle_checker): cleanup member functions
  * refactor(surround_obstacle_checker): cleanup polygon handling
  * refactor(surround_obstacle_checker): use marker helper
  * feat(planning_launch): separate surround_obstacle_checker from hierarchical motion planning flow
  * fix(surround_obstacle_checker): fix ego footprint polygon (`#877 <https://github.com/autowarefoundation/autoware.universe/issues/877>`_)
  * fix: update nvinfer api (`#863 <https://github.com/autowarefoundation/autoware.universe/issues/863>`_)
  * fix(lidar_centerpoint): update nvinfer api
  * fix(tensorrt_yolo): update nvinfer api
  * fix(lidar_apollo_instance_segmentation): update nvinfer api
  * fix(traffic_light_classifier): update nvinfer api
  * fix(traffic_light_ssd_fine_detector): update nvinfer api
  * pre-commit run
  * fix(avoidance_module): ignore object instead of creating zero shift (`#731 <https://github.com/autowarefoundation/autoware.universe/issues/731>`_)
  * fix: ignore object instead of creating zero shift
  instead of creating zero shift point, the object will be ignored.
  no behavior changes should be observed.
  * refactor: sync continue with upstream
  * fix: fix debug message for insufficient lateral margin
  * fix(motion_velocity_smoother): curve deceleration not working with a specific parameter set (`#738 <https://github.com/autowarefoundation/autoware.universe/issues/738>`_)
  * test(autoware_testing): fix smoke_test (`#479 <https://github.com/autowarefoundation/autoware.universe/issues/479>`_)
  * fix(autoware_testing): fix smoke_test
  * restore smoke_test for trajectory_follower_nodes
  * add support multiple parameter files
  * ci(pre-commit): autofix
  * minor fix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(rviz_plugins): add velocity limit to autoware state panel (`#879 <https://github.com/autowarefoundation/autoware.universe/issues/879>`_)
  * feat(rviz_plugins): add velocity limit to autoware state panel
  * chore(rviz_plugin): change ms to kmh
  * feat(vehicle_info_util): add max_steer_angle (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * feat(vehicle_info_util): add max_steer_angle
  * applied pre-commit
  * Added max_steer_angle in test config
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  * fix(lidar_centerpoint): fix google drive url to avoid 404 (`#889 <https://github.com/autowarefoundation/autoware.universe/issues/889>`_)
  * fix(lidar_centerpoint): fix google drive url to avoid 404
  * Update CMakeLists.txt
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * chore: fix typos (`#886 <https://github.com/autowarefoundation/autoware.universe/issues/886>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(map_tf_generator): accelerate the 'viewer' coordinate calculation (`#890 <https://github.com/autowarefoundation/autoware.universe/issues/890>`_)
  * add random point sampling function to quickly calculate the 'viewer' coordinate
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * docs(obstacle_stop_planner): update documentation (`#880 <https://github.com/autowarefoundation/autoware.universe/issues/880>`_)
  * ci(pre-commit): autofix
  * fixed conflicts
  * ci(pre-commit): autofix
  * resolve conflict
  * deleted
  * added "Voltage Monitor"
  * ci(pre-commit): autofix
  * merged with main->feature_battery_monitoring
  * merge  main ->feature_battery_monitoring
  * ci(pre-commit): autofix
  * added default vlue of cmos_battery_voltage
  * resolved conflict
  * resolved conflict
  * added topics_voltage_monitor.md)
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(dummy_diag_publisher): use anon to make unique node name instead of diag name (`#639 <https://github.com/autowarefoundation/autoware.universe/issues/639>`_)
  * chore: sync files (`#648 <https://github.com/autowarefoundation/autoware.universe/issues/648>`_)
  * chore: sync files
  * Revert "chore: sync files"
  This reverts commit b24f530b48306e16aa285f80a629ce5c5a9ccda7.
  * sync codecov.yaml
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage (`#666 <https://github.com/autowarefoundation/autoware.universe/issues/666>`_)
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(behavior_velocity): avoid insert same point on trajectory utils (`#834 <https://github.com/autowarefoundation/autoware.universe/issues/834>`_)
  * refactor(behavior_velocity_planner): simplify CMakeLists.txt (`#855 <https://github.com/autowarefoundation/autoware.universe/issues/855>`_)
  * docs: fix 404 error caused by typo in url (`#871 <https://github.com/autowarefoundation/autoware.universe/issues/871>`_)
  * docs: fix 404 error caused by typo in url
  * docs: fix typo in url for yolov4
  * fix(image_projection_based_fusion): set imagebuffersize (`#820 <https://github.com/autowarefoundation/autoware.universe/issues/820>`_)
  * fix: set imagebuffersize configured
  * ci(pre-commit): autofix
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore(avoidance_module): fix spell check (`#732 <https://github.com/autowarefoundation/autoware.universe/issues/732>`_)
  * feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
  * docs(virtual traffic light): add documentation (`#245 <https://github.com/autowarefoundation/autoware.universe/issues/245>`_)
  * doc(behavior_velocity): add graph and fix link
  * doc(behavior_velocity): update virtual traffic light doc
  * doc(behavior_velocity): minor fix
  * doc : mediate to coordinate
  * doc: minor update
  * doc: fix pre-commit
  * doc: update docs
  * apply suggestion
  * doc: to intersection-coordination
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat(surround_obstacle_checker): separate surround_obstacle_checker from hierarchical planning flow (`#830 <https://github.com/autowarefoundation/autoware.universe/issues/830>`_)
  * fix(surroud_obstacle_checker): use alias
  * feat(surround_obstacle_checker): use velocity limit
  * chore(surround_obstacle_checker): rename publisher, subscriber and callback functions
  * refactor(surround_obstacle_checker): use parameter struct
  * fix(surround_obstacle_checker): use alias
  * refactor(surround_obstacle_checker): cleanup member functions
  * refactor(surround_obstacle_checker): cleanup polygon handling
  * refactor(surround_obstacle_checker): use marker helper
  * feat(planning_launch): separate surround_obstacle_checker from hierarchical motion planning flow
  * fix(surround_obstacle_checker): fix ego footprint polygon (`#877 <https://github.com/autowarefoundation/autoware.universe/issues/877>`_)
  * fix: update nvinfer api (`#863 <https://github.com/autowarefoundation/autoware.universe/issues/863>`_)
  * fix(lidar_centerpoint): update nvinfer api
  * fix(tensorrt_yolo): update nvinfer api
  * fix(lidar_apollo_instance_segmentation): update nvinfer api
  * fix(traffic_light_classifier): update nvinfer api
  * fix(traffic_light_ssd_fine_detector): update nvinfer api
  * pre-commit run
  * fix(avoidance_module): ignore object instead of creating zero shift (`#731 <https://github.com/autowarefoundation/autoware.universe/issues/731>`_)
  * fix: ignore object instead of creating zero shift
  instead of creating zero shift point, the object will be ignored.
  no behavior changes should be observed.
  * refactor: sync continue with upstream
  * fix: fix debug message for insufficient lateral margin
  * fix(motion_velocity_smoother): curve deceleration not working with a specific parameter set (`#738 <https://github.com/autowarefoundation/autoware.universe/issues/738>`_)
  * test(autoware_testing): fix smoke_test (`#479 <https://github.com/autowarefoundation/autoware.universe/issues/479>`_)
  * fix(autoware_testing): fix smoke_test
  * restore smoke_test for trajectory_follower_nodes
  * add support multiple parameter files
  * ci(pre-commit): autofix
  * minor fix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(rviz_plugins): add velocity limit to autoware state panel (`#879 <https://github.com/autowarefoundation/autoware.universe/issues/879>`_)
  * feat(rviz_plugins): add velocity limit to autoware state panel
  * chore(rviz_plugin): change ms to kmh
  * feat(vehicle_info_util): add max_steer_angle (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * feat(vehicle_info_util): add max_steer_angle
  * applied pre-commit
  * Added max_steer_angle in test config
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  * fix(lidar_centerpoint): fix google drive url to avoid 404 (`#889 <https://github.com/autowarefoundation/autoware.universe/issues/889>`_)
  * fix(lidar_centerpoint): fix google drive url to avoid 404
  * Update CMakeLists.txt
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * chore: fix typos (`#886 <https://github.com/autowarefoundation/autoware.universe/issues/886>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(map_tf_generator): accelerate the 'viewer' coordinate calculation (`#890 <https://github.com/autowarefoundation/autoware.universe/issues/890>`_)
  * add random point sampling function to quickly calculate the 'viewer' coordinate
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * docs(obstacle_stop_planner): update documentation (`#880 <https://github.com/autowarefoundation/autoware.universe/issues/880>`_)
  * ci(pre-commit): autofix
  * fixed conflicts
  * ci(pre-commit): autofix
  * resolve conflict
  * ci(pre-commit): autofix
  * merged with main->feature_battery_monitoring
  * merge  main ->feature_battery_monitoring
  * Added voltages are provisional values.
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * fixed conflict manually
  * fixed conflict manually
  * ci(pre-commit): autofix
  * fixed conflict
  * fixed conflict
  * ci(pre-commit): autofix
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: awf-autoware-bot[bot] <94889083+awf-autoware-bot[bot]@users.noreply.github.com>
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Shintaro Tomie <58775300+Shin-kyoto@users.noreply.github.com>
  Co-authored-by: storrrrrrrrm <103425473+storrrrrrrrm@users.noreply.github.com>
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  Co-authored-by: badai nguyen <94814556+badai-nguyen@users.noreply.github.com>
  Co-authored-by: Takeshi Ishita <ishitah.takeshi@gmail.com>
  Co-authored-by: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Co-authored-by: Satoshi Tanaka <16330533+scepter914@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
* feat: add HDD monitoring items to hdd_monitor (`#721 <https://github.com/autowarefoundation/autoware.universe/issues/721>`_)
  * feat: add HDD monitoring items to hdd_monitor
  * fix pre-commit C long type error
  * fixed the monitoring method of RecoveredError
  * additional support for storage health check
  * resolve conflicts
  * fix bug when setting mount point of HDD Monitor
  * fix(system_monitor): level change when not connected and unmount function added in HDD connection monitoring
  * fix(system_monitor): level change when not connected in HDD connection monitoring
  * fix(system_monitor): unmount function added in hdd_reader
  * fix(system_monitor): separate S.M.A.R.T. request and lazy unmount request for hdd_reader
* feat(system_monitor): add IP packet reassembles failed monitoring to net_monitor (`#1427 <https://github.com/autowarefoundation/autoware.universe/issues/1427>`_)
  * feat(system_monitor): add IP packet reassembles failed monitoring to net_monitor
  * fix build errors caused by merge mistakes
  * fix(system_monitor): chang word Reasm and fix deep nesting
  * fix(system_monitor): fix deep nesting
  * fix(system_monitor): lightweight /proc/net/snmp reading
  * fix(system_monitor): fix index variable type to unsigned, add log output, and make index evaluation expression easier to understand
  * fix(system_monitor): remove unnecessary static_cast
  * fix(system_monitor): typo fix
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
* feat: add GPU clock monitoring to gpu_monitor (`#687 <https://github.com/autowarefoundation/autoware.universe/issues/687>`_)
* fix(system_monitor): fix parameter threshold of CPU Usage monitoring (`#1805 <https://github.com/autowarefoundation/autoware.universe/issues/1805>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
* fix(system_monitor): incorrect counter increment in CPU Usage monitor (`#1783 <https://github.com/autowarefoundation/autoware.universe/issues/1783>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
* feat: add CRC error monitoring to net_monitor (`#638 <https://github.com/autowarefoundation/autoware.universe/issues/638>`_)
  * feat: add CRC error monitoring to net_monitor
  * add CRC error monitoring information to README.md
  * ci(pre-commit): autofix
  Co-authored-by: noriyuki.h <n-hamaike@esol.co.jp>
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(system_monitor): multithreading support for boost::process (`#1714 <https://github.com/autowarefoundation/autoware.universe/issues/1714>`_)
* fix(system_monitor): move top command execution to a timer (`#948 <https://github.com/autowarefoundation/autoware.universe/issues/948>`_)
  * fix(system_monitor): move top command execution to  a timer
  * removed unnecessary update method
  * use tier4_autoware_utils::StopWatch
  * Ensure thread-safe
* fix(system_monitor): add some smart information to diagnostics (`#708 <https://github.com/autowarefoundation/autoware.universe/issues/708>`_)
* fix(system_monitor): fix truncation warning in strncpy (`#872 <https://github.com/autowarefoundation/autoware.universe/issues/872>`_)
  * fix(system_monitor): fix truncation warning in strncpy
  * Use std::string constructor to copy char array
  * Fixed typo
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* fix(system_monitor): fix build error on tegra platform (`#869 <https://github.com/autowarefoundation/autoware.universe/issues/869>`_)
  * fix(system_monitor): fix build error on tegra platform
  * ci(pre-commit): autofix
  * Update system/system_monitor/src/gpu_monitor/tegra_gpu_monitor.cpp
  Co-authored-by: Shark Liu <shark.liu@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* chore: remove bad chars (`#845 <https://github.com/autowarefoundation/autoware.universe/issues/845>`_)
* fix: suppress compiler warnings (`#852 <https://github.com/autowarefoundation/autoware.universe/issues/852>`_)
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* fix(system_monitor): modify build error in rolling (`#788 <https://github.com/autowarefoundation/autoware.universe/issues/788>`_)
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(system_monitor): add some smart information to diagnostics (`#560 <https://github.com/autowarefoundation/autoware.universe/issues/560>`_)
  * feat(system_monitor): add some smart information to diagnostics
  * ci(pre-commit): autofix
  * modify regex for nvme device name
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(system_monitor): change method of CPU usage monitoring (`#557 <https://github.com/autowarefoundation/autoware.universe/issues/557>`_)
  * feat(lidar_detection): changing default input topic name of lidar detection nodes (`#433 <https://github.com/autowarefoundation/autoware.universe/issues/433>`_)
  * feat(system_monitor): change method of CPU usage monitoring
  Co-authored-by: Taichi Higashide <azumade.30@gmail.com>
* feat(hdd_monitor): add unit to value side as well as other metrics (`#325 <https://github.com/autowarefoundation/autoware.universe/issues/325>`_)
* feat: add cpu usage topic (`#353 <https://github.com/autowarefoundation/autoware.universe/issues/353>`_)
  * modified for publishing cpu_usage_api
  * modified for calib error output and cpu usage output
  * modified push_back condition
  * modified topic name
  * Delete unnecessary comments
  * Delete unnecessary comments
  * modified for publishing cpu_usage_api
  * Delete unnecessary comments
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * run pre-commit
  * remove unnecessary comments
  * modify unnecessary change for pull request
  * run pre-commit
  * modify unnecessary change
  * modified along the comments on PR `#353 <https://github.com/autowarefoundation/autoware.universe/issues/353>`_
  * modified along the comments on PR `#353 <https://github.com/autowarefoundation/autoware.universe/issues/353>`_
  * remove unnecessary process
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(system_monitor): handle parameter as mount point (`#259 <https://github.com/autowarefoundation/autoware.universe/issues/259>`_)
* fix(system_monitor): fix build error on aarch64 (`#263 <https://github.com/autowarefoundation/autoware.universe/issues/263>`_)
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
* chore(sync): merged autoware.iv/pull/2362 (`#761 <https://github.com/autowarefoundation/autoware.universe/issues/761>`_) (`#134 <https://github.com/autowarefoundation/autoware.universe/issues/134>`_)
  Co-authored-by: h-mitsui-esol <57085544+h-mitsui-esol@users.noreply.github.com>
* feat: add autoware_system_monitor package (`#14 <https://github.com/autowarefoundation/autoware.universe/issues/14>`_)
  * release v0.4.0
  * Fixed uninitialized variable. (`#763 <https://github.com/autowarefoundation/autoware.universe/issues/763>`_)
  * Fixed various bugs. (`#768 <https://github.com/autowarefoundation/autoware.universe/issues/768>`_)
  * Fixed various bugs.
  * Fixed wrong status report of NIC.
  * Added the mode of CPU Usage to check statistics calculated as averages among all processors by default. (`#788 <https://github.com/autowarefoundation/autoware.universe/issues/788>`_)
  * fix uninitialized variables (`#816 <https://github.com/autowarefoundation/autoware.universe/issues/816>`_)
  * remove ROS1 packages temporarily
  * Revert "remove ROS1 packages temporarily"
  This reverts commit a9436882d50dc09fa5b8d6c0a151a10def76b242.
  * add COLCON_IGNORE to ros1 packages
  * Rename launch files to launch.xml (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
  * Port system monitor to ros2 (`#71 <https://github.com/autowarefoundation/autoware.universe/issues/71>`_)
  * Implement a utility function that spins and updates a monitor node.
  * Port cpu monitor
  * Port hdd monitor.
  * Port mem_monitor to ROS2
  * Port  net_monitor to ROS2
  * Port  ntp_monitor to ROS2
  * Port  process_monitor to ROS2
  * Port GPU_monitor to ROS2
  * Port msr_reader and hdd_reader to ROS2
  * Clean up the build and launch files:
  * Clean up and comment on CMake and package files.
  * Port the launch file to ROS2
  * Rename h files to hpp (`#142 <https://github.com/autowarefoundation/autoware.universe/issues/142>`_)
  * Change includes
  * Rename files
  * Adjustments to make things compile
  * Other packages
  * Adjust copyright notice on 532 out of 699 source files (`#143 <https://github.com/autowarefoundation/autoware.universe/issues/143>`_)
  * Use quotes for includes where appropriate (`#144 <https://github.com/autowarefoundation/autoware.universe/issues/144>`_)
  * Use quotes for includes where appropriate
  * Fix lint tests
  * Make tests pass hopefully
  * Run uncrustify on the entire Pilot.Auto codebase (`#151 <https://github.com/autowarefoundation/autoware.universe/issues/151>`_)
  * Run uncrustify on the entire Pilot.Auto codebase
  * Exclude open PRs
  * ROS2 Linting: system_monitor (`#207 <https://github.com/autowarefoundation/autoware.universe/issues/207>`_)
  * Add linters
  * Fix clang-tidy error in util.hpp
  * Ros2 v0.8.0 system monitor (`#276 <https://github.com/autowarefoundation/autoware.universe/issues/276>`_)
  * fix dependency of system_monitor
  * Rename ROS-related .yaml to .param.yaml (`#352 <https://github.com/autowarefoundation/autoware.universe/issues/352>`_)
  * Rename ROS-related .yaml to .param.yaml
  * Remove prefix 'default\_' of yaml files
  * Rename vehicle_info.yaml to vehicle_info.param.yaml
  * Rename diagnostic_aggregator's param files
  * Fix overlooked parameters
  * Exclude SwPowerCap as an error. (`#1146 <https://github.com/autowarefoundation/autoware.universe/issues/1146>`_) (`#364 <https://github.com/autowarefoundation/autoware.universe/issues/364>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  * [Update v0.9.0] system monitor (`#365 <https://github.com/autowarefoundation/autoware.universe/issues/365>`_)
  * Disable CPU Load Average warning. (`#1147 <https://github.com/autowarefoundation/autoware.universe/issues/1147>`_)
  * Fix cpu_monitor respawning forever. (`#1150 <https://github.com/autowarefoundation/autoware.universe/issues/1150>`_)
  * Disable cpu_temperature in planning simulation (`#1151 <https://github.com/autowarefoundation/autoware.universe/issues/1151>`_)
  * Net Monitor: Handle as an error if specified device not exist. (`#1152 <https://github.com/autowarefoundation/autoware.universe/issues/1152>`_)
  * Handled as an error if specified device not exist.
  * Disable network diags in simulation
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * apply ament_uncrustify
  * Disable resource monitoring in planning_simulator (`#1172 <https://github.com/autowarefoundation/autoware.universe/issues/1172>`_)
  * Treat logging errors as safe faults (`#1164 <https://github.com/autowarefoundation/autoware.universe/issues/1164>`_)
  * Fix test code of system_monitor (`#1178 <https://github.com/autowarefoundation/autoware.universe/issues/1178>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Use thread for ntpdate. (`#1160 <https://github.com/autowarefoundation/autoware.universe/issues/1160>`_) (`#375 <https://github.com/autowarefoundation/autoware.universe/issues/375>`_)
  * Use thread for ntpdate. (`#1160 <https://github.com/autowarefoundation/autoware.universe/issues/1160>`_)
  * removed unused variable
  * Import v0.9.1 (`#431 <https://github.com/autowarefoundation/autoware.universe/issues/431>`_)
  * add local optimal solution ocillation check to ndt_scan_matcher (`#1182 <https://github.com/autowarefoundation/autoware.universe/issues/1182>`_)
  * Add obstacle_crush diagnostic (`#1186 <https://github.com/autowarefoundation/autoware.universe/issues/1186>`_)
  * Fix diagnostics api (`#1185 <https://github.com/autowarefoundation/autoware.universe/issues/1185>`_)
  * Fix diagnostics api
  * Don't overwrite level
  * Overwrite level of No Fault diagnostics
  * Add missing diag in autoware_error_monitor.yaml (`#1187 <https://github.com/autowarefoundation/autoware.universe/issues/1187>`_)
  * Filter hazard_status (`#1191 <https://github.com/autowarefoundation/autoware.universe/issues/1191>`_)
  * Filter hazard_status
  * Filter leaf diagnostics
  * Fix wrong calculation of available memory. (`#1168 <https://github.com/autowarefoundation/autoware.universe/issues/1168>`_)
  * Fixed wrong calculation of available memory.
  * Added comments about output example of free -tb command.
  * Change monitoring method to get HDD temperature and usage per specified device. (`#1195 <https://github.com/autowarefoundation/autoware.universe/issues/1195>`_)
  * Changed monitoring method to get temperature and usage per specified device.
  * Fixed test codes.
  * Removed unnecessary (void) parameter.
  * return input pointcloud when ground plane not found (`#1190 <https://github.com/autowarefoundation/autoware.universe/issues/1190>`_)
  * fix yaw-smoothing bug (`#1198 <https://github.com/autowarefoundation/autoware.universe/issues/1198>`_)
  * Fix lint
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * Fix typo in system module (`#434 <https://github.com/autowarefoundation/autoware.universe/issues/434>`_)
  * Fix typo in system module
  * Change variable name
  * Move comments
  * Apply uncrustify
  * Split system_monitor config (`#452 <https://github.com/autowarefoundation/autoware.universe/issues/452>`_)
  * Remove unnecessary diagnostic update. (`#455 <https://github.com/autowarefoundation/autoware.universe/issues/455>`_)
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
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
  * Fix issues in gpu_monitor (`#1248 <https://github.com/autowarefoundation/autoware.universe/issues/1248>`_)
  * Fix issues in gpu_monitor
  * Fix uninitialized variables
  * Use range-based for loop
  * Fix compile errors of tegra_gpu_monitor
  * Replace C-style to C++-style
  * Make iterators const
  * Fix fmt::format() usage error
  * Unify Apache-2.0 license name (`#1242 <https://github.com/autowarefoundation/autoware.universe/issues/1242>`_)
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * [system_monitor] change some nodes into components (`#1234 <https://github.com/autowarefoundation/autoware.universe/issues/1234>`_)
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <takeshi.miura@tier4.jp>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  * add system_monitor.launch.py (`#1238 <https://github.com/autowarefoundation/autoware.universe/issues/1238>`_)
  * add system_monitor.launch.py
  * refactor system_monitor.launch.py
  * fix launch bug
  * fix typo
  * fix launch py
  * fix param loading
  * format code
  * fix system monitor executor to publish diagnostics asynclonously (`#1283 <https://github.com/autowarefoundation/autoware.universe/issues/1283>`_)
  * Fix lint errors (`#1378 <https://github.com/autowarefoundation/autoware.universe/issues/1378>`_)
  * Fix lint errors
  * Fix variable names
  * Add kernel CPU usage. (`#1465 <https://github.com/autowarefoundation/autoware.universe/issues/1465>`_)
  * Add kernel CPU usage.
  * Change CPU x: usage to CPU x: total.
  * Changed variable name.
  * Add markdownlint and prettier (`#1661 <https://github.com/autowarefoundation/autoware.universe/issues/1661>`_)
  * Add markdownlint and prettier
  * Ignore .param.yaml
  * Apply format
  * suppress warnings for system monitor (`#1723 <https://github.com/autowarefoundation/autoware.universe/issues/1723>`_)
  * fix for hdd_monitor
  * fix no initialization and warning
  * change command for ntp_monitor (`#1705 <https://github.com/autowarefoundation/autoware.universe/issues/1705>`_)
  * [EVT4-403] change command for ntp_monitor
  * [EVT4-403] fixed CI build error
  * [EVT4-403] fixed cpplint error
  * delete executeChronyc thread, fix error topic and log output code
  * fix cpplint error and code style divergence
  * fix cpplint error(missing correction)
  * Fix MD029 (`#1813 <https://github.com/autowarefoundation/autoware.universe/issues/1813>`_)
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * add gpu usage per process (`#1798 <https://github.com/autowarefoundation/autoware.universe/issues/1798>`_)
  * add gpu usage per process
  * change illegal usage(4294967295%) to 0%, and fix CI running errors
  * Replace gettimeofday with rclcpp::Node::now().
  * Fix uncrustify error.
  * Replace rclcpp::Node::now() with rclcpp::Clock(RCL_SYSTEM_TIME).
  Co-authored-by: ito-san <fumihito.ito@tier4.jp>
  * fix some typos (`#1941 <https://github.com/autowarefoundation/autoware.universe/issues/1941>`_)
  * fix some typos
  * fix typo
  * Fix typo
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * suppress warnings for system directory `#2046 <https://github.com/autowarefoundation/autoware.universe/issues/2046>`_
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
  * Add execution time logging. (`#2066 <https://github.com/autowarefoundation/autoware.universe/issues/2066>`_)
  * Add markdown-link-check pre-commit (`#2215 <https://github.com/autowarefoundation/autoware.universe/issues/2215>`_)
  * add markdown-lint-check pre-commit
  * delete files argument
  * add optional hook
  * modify comment
  * add comment
  * delete hook
  * add retry option
  * add option
  * add files arg
  * Fix links in hdd_reader.md
  * Ignore 403
  * Ignore tier4 github url
  * Update link
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
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
  * remove COLCON_IGNORE in system_packages and map_tf_generator (`#532 <https://github.com/autowarefoundation/autoware.universe/issues/532>`_)
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: Yunus Emre Çalışkan <yunus.ec@gmail.com>
  Co-authored-by: Jilada Eccleston <jilada.eccleston@gmail.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <isamu.takagi@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <takeshi.miura@tier4.jp>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: v-kitahara8153 <86092199+v-kitahara8153@users.noreply.github.com>
  Co-authored-by: ito-san <fumihito.ito@tier4.jp>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* Contributors: Akihiro Sakurai, Daisuke Nishimatsu, Esteve Fernandez, Keisuke Shima, Kenji Miyake, Maxime CLEMENT, Shark, Takagi, Isamu, Takayuki AKAMINE, TakumiKozaka-T4, Tomoya Kimura, Vincent Richard, Yuxuan Liu, awf-autoware-bot[bot], ito-san, kk-inoue-esol, kminoda, nobuotakamasa, takeshi-iwanari, v-nakayama7440-esol

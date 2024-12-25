^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_system_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(system): fixed to use autoware_component_interface_tools (`#9133 <https://github.com/autowarefoundation/autoware.universe/issues/9133>`_)
  Fixed component_interface_tools
* feat(system_error_monitor): remove system error monitor (`#8929 <https://github.com/autowarefoundation/autoware.universe/issues/8929>`_)
  * feat: delete-system-error-monitor-from-autoware
  * feat: remove unnecessary params
  ---------
* feat(emergency_handler): delete package (`#8917 <https://github.com/autowarefoundation/autoware.universe/issues/8917>`_)
  * feat(emergency_handler): delete package
* feat(processing_time_checker): add a new package (`#7957 <https://github.com/autowarefoundation/autoware.universe/issues/7957>`_)
  * feat(processing_time_checker): add a new package
  * fix
  * fix
  * update README and schema.json
  * fix
  * fix
  * fix
  ---------
* feat(tier4_system_launch): use mrm handler by default (`#7728 <https://github.com/autowarefoundation/autoware.universe/issues/7728>`_)
* feat(tier4_system_launch): modify diagnostic_graph_aggregator_graph argument (`#7133 <https://github.com/autowarefoundation/autoware.universe/issues/7133>`_)
* feat(default_ad_api): use diagnostic graph (`#7043 <https://github.com/autowarefoundation/autoware.universe/issues/7043>`_)
* Contributors: Ryuta Kambe, SakodaShintaro, Takagi, Isamu, Takayuki Murooka, TetsuKawa, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* chore(tier4_system_launch): add option to select graph path depending on running mode (`#6700 <https://github.com/autowarefoundation/autoware.universe/issues/6700>`_)
  chore(tier4_system_launch): add option of using graph path for simulation
* feat(tier4_system_launch): add option to launch mrm handler (`#6660 <https://github.com/autowarefoundation/autoware.universe/issues/6660>`_)
* chore: update maintainer (`#5730 <https://github.com/autowarefoundation/autoware.universe/issues/5730>`_)
  update maintainer
* feat(duplicated_node_checker): add packages to check duplication of node names in ros2 (`#5286 <https://github.com/autowarefoundation/autoware.universe/issues/5286>`_)
  * add implementation for duplicated node checking
  * update the default parameters of system_error_monitor to include results from duplication check
  * style(pre-commit): autofix
  * fix typo in readme
  * update license
  * change module to the system module
  * follow json schema: 1. update code to start without default 2. add schema/config/readme/launch accordingly
  * add duplicated node checker to launch
  * style(pre-commit): autofix
  * fix var name to config for uniform launch
  * Update system/duplicated_node_checker/README.md
  * Update system/duplicated_node_checker/README.md
  ---------
  Co-authored-by: Owen-Liuyuxuan <uken.ryu@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* chore: update maintainer (`#4140 <https://github.com/autowarefoundation/autoware.universe/issues/4140>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(pose_initializer): enable pose initialization while running (only for sim) (`#3038 <https://github.com/autowarefoundation/autoware.universe/issues/3038>`_)
  * feat(pose_initializer): enable pose initialization while running (only for sim)
  * both logsim and psim params
  * only one pose_initializer_param_path arg
  * use two param files for pose_initializer
  ---------
* feat(dummy diag publisher): change diag name specification method to YAML (`#2840 <https://github.com/autowarefoundation/autoware.universe/issues/2840>`_)
  * Signed-off-by: asana17 <akihiro.sakurai@tier4.jp>
  modified dummy_diag_publisher to use YAML for param
  * Signed-off-by: asana17 <akihiro.sakurai@tier4.jp>
  use YAML param for dummy_diag_publisher
  * fix empty param
  * fixed empty param
  * fix spelling
  * add pkg maintainer
  * launch dummy_diag_publisher by launch_dummy_diag_publisher param
  ---------
* feat(tier4_system_launch): remove configs and move to autoware_launch (`#2540 <https://github.com/autowarefoundation/autoware.universe/issues/2540>`_)
  * feat(tier4_system_launch): remove configs and move to autoware_launch
  * update readme
  * fix readme
  * remove config
  * minor fix
  * fix readme
  * fix mistake
  * fix typo
* feat(component_interface_tools): add service log checker  (`#2503 <https://github.com/autowarefoundation/autoware.universe/issues/2503>`_)
  * feat(component_interface_utils): add service log checker
  * feat(component_interface_tools): add service log checker
  * feat(component_interface_tools): add diagnostics
  * feat: update system error monitor config
* feat: replace python launch with xml launch for system monitor (`#2430 <https://github.com/autowarefoundation/autoware.universe/issues/2430>`_)
  * feat: replace python launch with xml launch for system monitor
  * ci(pre-commit): autofix
  * update figure
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(system_monitor): add maintainer (`#2420 <https://github.com/autowarefoundation/autoware.universe/issues/2420>`_)
* feat!: replace HADMap with Lanelet (`#2356 <https://github.com/autowarefoundation/autoware.universe/issues/2356>`_)
  * feat!: replace HADMap with Lanelet
  * update topic.yaml
  * Update perception/traffic_light_map_based_detector/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update planning/behavior_path_planner/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update planning/mission_planner/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update planning/scenario_selector/README.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * format readme
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* fix(mrm_emergency_stop_operator): fix parameter loading in mrm operators (`#2378 <https://github.com/autowarefoundation/autoware.universe/issues/2378>`_)
  * fix(mrm_emergency_stop_operator): fix parameter loading in mrm operators
  * ci(pre-commit): autofix
  * fix(mrm_emergency_stop_operator): remove os import
  * fix(mrm_emergency_stop_operator): remove unused packages
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ad_service_state_monitor)!: remove ad_service_state_monitor (`#2311 <https://github.com/autowarefoundation/autoware.universe/issues/2311>`_)
  * feat(autoware_ad_api_msgs): define operation mode interface
  * feat(default_ad_api): add operation mode api
  * fix: add message
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix: add message callback
  * feat: add topic monitoring
  * feat: use topic monitoring
  * feat: modify topic monitoring config
  * fix: config name
  * feat: modify diag name
  * feat: move adapi message
  * feat: change message type
  * fix: merge
  * WIP
  * fix: fix build error
  * feat: move diagnostics
  * feat: remove diagnostics
  * feat: modify error message
  * feat: remove unused code
  * feat(default_ad_api): add autoware state
  * feat: reproduce old state
  * feat: add shutdown service
  * feat: change operation mode to stop
  * feat: change operation mode to stop
  * feat: remove ad_service_state_monitor
  * feat: apply removing of ad_service_state_monitor
  * ci(pre-commit): autofix
  * fix: remove comment for sync-file
  * feat: discard sensing topic rate status
  * Revert "feat: discard sensing topic rate status"
  This reverts commit 120d4f8d1aee93d7cbb29cc9bfbbbc52fe12cbf6.
  * feat: add dummy topic rate check for alive monitoring
  * Revert "feat: add dummy topic rate check for alive monitoring"
  This reverts commit 46d9d4a495b6bc1ee86dcd2e71b5df346e8f1f6b.
  * feat: remove sensing alive monitoring
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(system_monitor): add parameter to launch system_monitor and fix hdd_monitor (`#2285 <https://github.com/autowarefoundation/autoware.universe/issues/2285>`_)
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
* chore(tier4_system_launch): remove unused system file (`#2263 <https://github.com/autowarefoundation/autoware.universe/issues/2263>`_)
  * chore(tier4_system_launch): remove unused system file
  * remove unnecessary code
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(component_state_monitor): add component state monitor (`#2120 <https://github.com/autowarefoundation/autoware.universe/issues/2120>`_)
  * feat(component_state_monitor): add component state monitor
  * feat: change module
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
* chore(system_error_monitor): add maintainer (`#1922 <https://github.com/autowarefoundation/autoware.universe/issues/1922>`_)
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
* fix(system_monitor): fix parameter threshold of CPU Usage monitoring (`#1805 <https://github.com/autowarefoundation/autoware.universe/issues/1805>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
* feat: add CRC error monitoring to net_monitor (`#638 <https://github.com/autowarefoundation/autoware.universe/issues/638>`_)
  * feat: add CRC error monitoring to net_monitor
  * add CRC error monitoring information to README.md
  * ci(pre-commit): autofix
  Co-authored-by: noriyuki.h <n-hamaike@esol.co.jp>
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_system_launch): declare tier4_system_launch_param_path (`#1411 <https://github.com/autowarefoundation/autoware.universe/issues/1411>`_)
* fix(tier4_system_launch): add group tag (`#1240 <https://github.com/autowarefoundation/autoware.universe/issues/1240>`_)
  * fix(tier4_system_launch): add group tag
  * move arg into group
* fix(system_monitor): add some smart information to diagnostics (`#708 <https://github.com/autowarefoundation/autoware.universe/issues/708>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* Contributors: Akihiro Sakurai, Daisuke Nishimatsu, Kenji Miyake, Kosuke Takeuchi, Makoto Kurihara, Mamoru Sobue, Takagi, Isamu, Takayuki Murooka, Tomohito ANDO, Tomoya Kimura, Vincent Richard, Xinyu Wang, Yuxuan Liu, asana17, ito-san, kk-inoue-esol, kminoda, nobuotakamasa, v-nakayama7440-esol

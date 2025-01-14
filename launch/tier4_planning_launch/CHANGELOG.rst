^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_planning_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(autoware_behavior_velocity_planner_common,autoware_behavior_velocity_planner): separate param files (`#9470 <https://github.com/autowarefoundation/autoware.universe/issues/9470>`_)
  * refactor(autoware_behavior_velocity_planner_common,autoware_behavior_velocity_planner): separate param files
  * Update planning/autoware_static_centerline_generator/test/test_static_centerline_generator.test.py
  Co-authored-by: Kyoichi Sugahara <32741405+kyoichi-sugahara@users.noreply.github.com>
  * fix
  ---------
  Co-authored-by: Kyoichi Sugahara <32741405+kyoichi-sugahara@users.noreply.github.com>
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, SakodaShintaro, Yukinari Hisaki, Yutaka Kondo

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
* fix(autoware_obstacle_stop_planner): register obstacle stop planner node with autoware scoping (`#8512 <https://github.com/autowarefoundation/autoware.universe/issues/8512>`_)
  Register node plugin with autoware scoping
* feat(scenario_selector, freespace_planner): improve freespace planner edge case behavior (`#8348 <https://github.com/autowarefoundation/autoware.universe/issues/8348>`_)
  * refactor free space planner subscribers
  * implement scenario switching for edge cases
  * fix scenario selector test
  * implement confidence for checking if obstacle is on trajectory
  * fix isInLane check to work for case when provided position is on lane bound
  * update parameter schemas
  * fix format
  * improve near target logic
  * use timer based implementation for obstacle check
  ---------
* refactor(compare_map_segmentation): add package name prefix of autoware\_ (`#8005 <https://github.com/autowarefoundation/autoware.universe/issues/8005>`_)
  * refactor(compare_map_segmentation): add package name prefix of autoware\_
  * docs: update Readme
  ---------
* refactor(pointcloud_preprocessor): prefix package and namespace with autoware (`#7983 <https://github.com/autowarefoundation/autoware.universe/issues/7983>`_)
  * refactor(pointcloud_preprocessor)!: prefix package and namespace with autoware
  * style(pre-commit): autofix
  * style(pointcloud_preprocessor): suppress line length check for macros
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * refactor(pointcloud_preprocessor): directory structure (soft)
  * refactor(pointcloud_preprocessor): directory structure (hard)
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(compare_map_segmentation)!: fix namespace and directory structure (`#7910 <https://github.com/autowarefoundation/autoware.universe/issues/7910>`_)
  * feat: update namespace and directory structure for compare_map_segmentation code
  * refactor: update  directory structure
  * fix: add missing include
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(frenet_planner): fix mistake in the curvature calculation (`#7920 <https://github.com/autowarefoundation/autoware.universe/issues/7920>`_)
* feat(obstacle_cruise_planner): support pointcloud-based obstacles (`#6907 <https://github.com/autowarefoundation/autoware.universe/issues/6907>`_)
  * add pointcloud to obstacle properties
  * add tf listener & pointcloud subscriber
  * add parameters for pointcloud obstacle
  * add type aliases
  * convert pointcloud to obstacle
  * add type alias
  * add polygon conversion for pointcloud obstacle
  * initialize twist & pose of pointcloud obstacle
  * overload to handle both obstacle & predicted path
  * implement ego behavior determination against pointcloud obstacles
  * generate obstacle from point
  * revert getCollisionIndex()
  * generate obstacle from each point in cloud
  * set pointcloud obstacle velocity to 0
  * use tf buffer & listener with pointers
  * update latest pointcloud data
  * add topic remap
  * remove unnecessary includes
  * set slow down obstacle velocity to 0
  * add flag to consider pointcloud obstacle for stopping & slowing down
  * style(pre-commit): autofix
  * downsample pointcloud using voxel grid
  * change  shape type of pointcloud obstacle to polygon
  * convert pointcloud to obstacle by clustering
  * add parameters for clustering
  * add max_num_points parameter to dummy object
  * downsample pointcloud when the number of points is larger than max_num_points
  * add max_num_points property to dummy bus
  * add parameters for pointcloud based obstacles
  * store pointcloud in obstacle struct
  * change obstacle conversion method
  * migrate previous changes to new package
  * store necessary points only
  * move use_pointcloud to common parameter
  * extract necessary points from pointcloud
  * add use_pointcloud parameter to planner interface
  * fix obstacle conversion
  * fix collision point determination
  * simplify pointcloud transformation
  * style(pre-commit): autofix
  * fix collision point determination
  * pick nearest stop collision point
  * check collision for every point in cluster
  * migrate previous changes to new files
  * reduce diff
  * remove use_pointcloud parameter
  * add parameters for pointcloud filtering
  * add autoware namespace
  * Revert "add max_num_points parameter to dummy object"
  This reverts commit 98bcd0856f861d23c9f7989d8128939ec0b3e27c.
  * Revert "downsample pointcloud when the number of points is larger than max_num_points"
  This reverts commit fb00b59d8f14cec6810e7fab12bc34d8a0c617c7.
  * Revert "add max_num_points property to dummy bus"
  This reverts commit 5f9e4ab5ae7d8d46521c736b1d259040121f3bc5.
  * feat(diagnostic_graph_utils): add logging tool
  * fix all OK
  * feat(default_ad_api): add log when operation mode change fails
  * get only the necessary one of object or pointcloud data
  * addfield for obstacle source type
  * enable simultaneous use of PredictedObjects and PointCloud
  * separate convertToObstacles() by source type
  * avoid using pointer
  * reduce diff
  * make nest shallower
  * define vector concatenate function
  * shorten variable names
  * fix redundant condition
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <isamu.takagi@tier4.jp>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* refactor(autoware_obstacle_stop_planner): prefix package and namespace with autoware (`#7565 <https://github.com/autowarefoundation/autoware.universe/issues/7565>`_)
  * refactor(autoware_obstacle_stop_planner): prefix package and namespace with autoware
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(planning_evaluator): rename to include/autoware/{package_name} (`#7518 <https://github.com/autowarefoundation/autoware.universe/issues/7518>`_)
  * fix
  * fix
  ---------
* refactor(dynamic_obstacle_stop): move to motion_velocity_planner (`#7460 <https://github.com/autowarefoundation/autoware.universe/issues/7460>`_)
* feat(obstacle_velocity_limiter): move to motion_velocity_planner (`#7439 <https://github.com/autowarefoundation/autoware.universe/issues/7439>`_)
* refactor(bpp): add namespace `autoware::` (`#7437 <https://github.com/autowarefoundation/autoware.universe/issues/7437>`_)
  * refactor: add namespace autoware::
  * refactor(bpp-common): add namespace autoware::
  * refactor(ablc): add namespace autoware::
  * refactor(doa): add namespace autoware::
  * refactor(soa): add namespace autoware::
  * refactor(erlc): add namespace autoware::
  * refactor(lc): add namespace autoware::
  * refactor(ss): add namespace autoware::
  * refactor(sp): add namespace autoware::
  * refactor(gp): add namespace autoware::
  * refactor(tier4_planning_launch): add namespace autoware::
  * refactor(sbp): add namespace autoware::
  ---------
* refactor(behavior_path_planner): prefix autoware\_ to behavior_path_planner package (`#7433 <https://github.com/autowarefoundation/autoware.universe/issues/7433>`_)
  * move dir
  * fix pluginlib
  ---------
* refactor(obstacle_cruise_planner)!: add autoware\_ prefix (`#7419 <https://github.com/autowarefoundation/autoware.universe/issues/7419>`_)
* refactor(behavior_path_sampling_planner_module): add autoware prefix (`#7392 <https://github.com/autowarefoundation/autoware.universe/issues/7392>`_)
* refactor(mission_planner)!: add autoware prefix and namespace (`#7414 <https://github.com/autowarefoundation/autoware.universe/issues/7414>`_)
  * refactor(mission_planner)!: add autoware prefix and namespace
  * fix svg
  ---------
* refactor(freespace_planner)!: add autoware prefix (`#7376 <https://github.com/autowarefoundation/autoware.universe/issues/7376>`_)
  refactor(freespace_planner)!: add autoawre prefix
* refactor(external_cmd_selector): prefix package and namespace with au… (`#7384 <https://github.com/autowarefoundation/autoware.universe/issues/7384>`_)
  refactor(external_cmd_selector): prefix package and namespace with autoware\_
* refactor(scenario_selector): prefix package and namespace with autoware\_ (`#7379 <https://github.com/autowarefoundation/autoware.universe/issues/7379>`_)
* fix(motion_planning.launch): fix input traj of obstacle_velocity_limiter (`#7386 <https://github.com/autowarefoundation/autoware.universe/issues/7386>`_)
* refactor(out_of_lane): remove from behavior_velocity (`#7359 <https://github.com/autowarefoundation/autoware.universe/issues/7359>`_)
* refactor(path_smoother)!: prefix package and namespace with autoware (`#7381 <https://github.com/autowarefoundation/autoware.universe/issues/7381>`_)
  * git mv
  * fix
  * fix launch
  * rever a part of prefix
  * fix test
  * fix
  * fix static_centerline_optimizer
  * fix
  ---------
* fix(tier4_planning_launch): unexpected modules were registered (`#7377 <https://github.com/autowarefoundation/autoware.universe/issues/7377>`_)
* refactor(costmap_generator)!: add autoware prefix (`#7329 <https://github.com/autowarefoundation/autoware.universe/issues/7329>`_)
  refactor(costmap_generator): add autoware prefix
* refactor(path_optimizer, velocity_smoother)!: prefix package and namespace with autoware (`#7354 <https://github.com/autowarefoundation/autoware.universe/issues/7354>`_)
  * chore(autoware_velocity_smoother): update namespace
  * chore(autoware_path_optimizer): update namespace
  ---------
* refactor(planning_validator)!: prefix package and namespace with autoware (`#7320 <https://github.com/autowarefoundation/autoware.universe/issues/7320>`_)
  * add autoware\_ prefix to planning_validator
  * add prefix to package name in .pages
  * fix link of the image
  ---------
* refactor(behavior_velocity_planner_common)!: prefix package and namespace with autoware (`#7314 <https://github.com/autowarefoundation/autoware.universe/issues/7314>`_)
  * refactor(behavior_velocity_planner_common): add autoware prefix
  * refactor(behavior_velocity_planner_common): fix run_out module
  * refactor(behavior_velocity_planner_common): fix for autoware_behavior_velocity_walkway_module
  * refactor(behavior_velocity_planner_common): remove unnecessary using
  ---------
* refactor(sampling_based_planner): add autoware prefix (`#7348 <https://github.com/autowarefoundation/autoware.universe/issues/7348>`_)
* refactor(surround_obstacle_checker)!: prefix package and namespace with autoware (`#7298 <https://github.com/autowarefoundation/autoware.universe/issues/7298>`_)
  * fix(autoware_surround_obstacle_checker): rename
  * fix(autoware_surround_obstacle_checker): rename header
  * fix(launch): update package name
  ---------
* refactor(autoware_velocity_walkway_module): prefix package with autoware\_ and move code to the autoware namespace (`#7153 <https://github.com/autowarefoundation/autoware.universe/issues/7153>`_)
  * refactor(autoware_velocity_walkway_module): prefix package with autoware\_ and move code to the autoware namespace
  * style(pre-commit): autofix
  * fix: fix issue loading packages that have been prefixed
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(autoware_planning_topic_converter): add prefix `autoware\_` (`#7296 <https://github.com/autowarefoundation/autoware.universe/issues/7296>`_)
  chore(autoware_planning_topic_converter): rename
* chore(autoware_external_velocity_limit_selector): add prefix `autoware\_` (`#7295 <https://github.com/autowarefoundation/autoware.universe/issues/7295>`_)
  chore(autoware_external_velocity_limit_selector): rename
* refactor(autoware_velocity_run_out_module): prefix package with autoware\_ and move code to the autoware namespace (`#7154 <https://github.com/autowarefoundation/autoware.universe/issues/7154>`_)
  * refactor(autoware_velocity_run_out_module): prefix package with autoware\_ and move code to the autoware namespace
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(autoware_velocity_virtual_traffic_light_module): prefix package with autoware\_ and move code to the autoware namespace (`#7155 <https://github.com/autowarefoundation/autoware.universe/issues/7155>`_)
* feat!: replace autoware_auto_msgs with autoware_msgs for launch files (`#7242 <https://github.com/autowarefoundation/autoware.universe/issues/7242>`_)
  * feat!: replace autoware_auto_msgs with autoware_msgs for launch files
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  ---------
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
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
* chore(static_obstacle_avoidance, dynamic_obstacle_avoidance): rename avoidance package (`#7168 <https://github.com/autowarefoundation/autoware.universe/issues/7168>`_)
  * chore(autoware_behavior_path_static_obstacle_avoidance_module): rename package and namespace
  * chore(autoware_behavior_path_dynamic_obstacle_avoidance_module): rename package and namespace
  * chore(tier4_planning_launch): update module name
  * chore(rtc_interface): update module name
  * chore(avoidance): update module param file name
  * chore(avoidance): update schema file name
  * fix(AbLC): fix file name
  * docs: update module name
  ---------
* feat(motion_velocity_planner): add new motion velocity planning (`#7064 <https://github.com/autowarefoundation/autoware.universe/issues/7064>`_)
* refactor(behavior_velocity_planner)!: prefix package and namespace with autoware\_ (`#6693 <https://github.com/autowarefoundation/autoware.universe/issues/6693>`_)
* build(behavior_path_external_request_lane_change_module): prefix package and namespace with autoware\_ (`#6636 <https://github.com/autowarefoundation/autoware.universe/issues/6636>`_)
* feat: add autoware_remaining_distance_time_calculator and overlay (`#6855 <https://github.com/autowarefoundation/autoware.universe/issues/6855>`_)
* revert: "feat(logger_level_configure): make it possible to change level of container logger (`#6823 <https://github.com/autowarefoundation/autoware.universe/issues/6823>`_)" (`#6842 <https://github.com/autowarefoundation/autoware.universe/issues/6842>`_)
  This reverts commit 51b5f830780eb69bd1a7dfe60e295773f394fd8e.
* feat(logger_level_configure): make it possible to change level of container logger (`#6823 <https://github.com/autowarefoundation/autoware.universe/issues/6823>`_)
  * feat(launch): add logging_demo::LoggerConfig into container
  * fix(logger_level_reconfigure_plugin): fix yaml
  * feat(logging_level_configure): add composable node
  ---------
* Contributors: Ahmed Ebrahim, Amadeusz Szymko, Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, Kyoichi Sugahara, Mamoru Sobue, Maxime CLEMENT, Mitsuhiro Sakamoto, Mukunda Bharatheesha, Ryohsuke Mitsudome, Satoshi OTA, Taekjin LEE, Takayuki Murooka, Yuki TAKAGI, Yukinari Hisaki, Yutaka Kondo, badai nguyen, mkquda

0.26.0 (2024-04-03)
-------------------
* chore(tier4_planning_launch): set log output both (`#6685 <https://github.com/autowarefoundation/autoware.universe/issues/6685>`_)
* feat(traffic_light): depend on is_simulation for scenario simulator (`#6498 <https://github.com/autowarefoundation/autoware.universe/issues/6498>`_)
  * feat(traffic_light): depend on is_simulation for scenario simulator
  * fix comments
  * fix
  ---------
* feat(mission_planner)!: introduce route_selector node (`#6363 <https://github.com/autowarefoundation/autoware.universe/issues/6363>`_)
  * feat(mission_planner): introduce route_selector node
  * remove unused file
  * fix use goal pose only when resuming
  * fix: change mrm mode if route set is successful
  * add interrupted state
  * fix mrm set route uuid
  * remove unused reference
  * add resume route function
  * try to resume planned route
  * remove debug code
  * use full license text instead of spdx
  ---------
* feat: remove use_pointcloud_container (`#6115 <https://github.com/autowarefoundation/autoware.universe/issues/6115>`_)
  * feat!: remove use_pointcloud_container
  * fix pre-commit
  * fix: completely remove use_pointcloud_container after merge main
  * fix: set use_pointcloud_container = true
  * revert: revert change in probabilistic_occupancy_grid_map
  * revert change in launcher of ogm
  ---------
* feat(behavior_path_sampling_module): add sampling based planner  (`#6131 <https://github.com/autowarefoundation/autoware.universe/issues/6131>`_)
  * first commit: add only necessary bpp code for template
  * change name of file
  * delete more unrelated code
  * refactor
  * fix manager
  * rebase
  * Copy sampling-based planner to behavior path planner
  * fix include paths
  * rebase
  * eliminate unused code
  * delete repeated code
  * add dependencies for bezier and frenet planners
  * [WIP] Made a simple implementation of behavior planning
  * [WIP] added comments on making drivable area
  * Just adding functions to test
  * [WIP] Implement Frenet Planner
  * eliminate unused code
  * WIP add debug marker generation
  * Comment out for debugging
  * return prev drivable area (temp)
  * fixes to compile after rebase
  * WIP update sampling planner param structure to equal behav planner
  * Updated param handling
  * changed names of internal_variable to match changes
  * partially solve markers not clearing
  * add param update functionality
  * WIP transform frenet to pathwithlaneid
  * set frenet path as output
  * Added pruning to select the best frenet  path
  * Initialize vehicle info
  * calculate properly right and left bound for drivable area check
  * remove debug prints and increase vehicle margin, should be param
  * param changes for driving in small lanes
  * WIP add drivable area expansion from LC
  * add drivable area expansion
  * add driveable area
  * Make the points on the path have the same z as goal
  * remove print, changes
  * WIP add prev sampling path to calculation
  * WIP constraints handler
  * Add modifiable hard constraints checking function
  * Add modifiable soft constraints checking function
  * Add costs for distance to goal and curvature
  * take out todo-> solved
  * Added normalized constraints with ref path speed
  * (WIP)isExecution requested update to not execute
  * refactor: move getInitialState to utils
  * refactor: move some functions to utils, get rid of velocity req in generate pathwithlaneid
  * made curvature soft constraint depend on distance to goal
  * Add prev path extension
  * add calculation of initial lateral velocity and acceleration
  * add calculation of initial lateral velocity and acceleration to path extension
  * WIP Add poses to path to get centerline distance and other stuff
  * clear info_marker\_ to prevent performance issues
  * solve dependency issues
  * Add cost to avg. distance to centerline
  * added arc lenght based extension limit
  * Add running and success conditions, add dist to soft const
  * update success transition
  * Solve bug with goal not being in any current lanelet
  * Add todo comment
  * Adjust to centerline cost
  * update soft costs
  * tuning
  * add reference path change after sampling planner Success (which might cause a LC
  * Added soft constraints weights as parameter to easily tune w/ rqt
  * improve performance by computing arc coordinates before soft constraints check
  * temp
  * temp
  * deleted unusused
  * delete unused
  * add plugin export macro
  * fix launch file
  * WIP still not launching sampling planner plugin
  * solve problem of plugin insertion (duplicated files)
  * partly fix issue with PathwithLaneID not having laneids at the first points
  * Modify PreviousOutput path since it is no longer a shared ptr
  * Added new change root lanelet request override
  * WIP update collision detection to use rtree
  * fix bug with index
  * Add rtree for collision checking
  * refine soft constraints use remaining length of path max curv and normalize lateral error
  * Add sanity check and delete unused code
  * change success transit function to be more accurate
  * refactor
  * solve bug for path.extend with 0 poses
  * add hard check for empty paths
  * fix private current_state usage
  * Add path reuse at different lenghts
  * delete old comments use param for path reuse
  * light refactoring
  * pre-commit changes
  * pre-commit add dependency
  * delete unused dependencies
  * change constraints evaluation to return vectors
  * use tier4 autoware utils function to calc quaternion
  * refactor, use autoware utils
  * update comment
  * Add documentation
  * pre-commit changes
  * delete unused dependencies and repeated args
  * update copyright and fix magic numbers
  * delete unused header
  * refactoring
  * remove unused dependency
  * update copyright and dependency
  * update calcBound to work properly
  * solve problem with drivable area
  * remove forced false
  * solve calc bound problem
  * fix compatibility with updates to bpp
  * remove cerr print
  * solve bugs when merging with lane
  * solve issue of sbp not activating
  * remove unused commented code
  ---------
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
* feat(behavior_velocity_planner): add enable_all_modules_auto_mode argument to launch files for behavior velocity planner modules (`#6094 <https://github.com/autowarefoundation/autoware.universe/issues/6094>`_)
  * set default value for enable_all_modules_auto_mode
  * fix enable_rtc configuration in scene_module_manager_interface.hpp
  * Refactor scene module managers to use getEnableRTC function
  ---------
* feat(behavior_path_planner): add enable_all_modules_auto_mode argument to launch files for behavior path planner modules (`#6093 <https://github.com/autowarefoundation/autoware.universe/issues/6093>`_)
  * Add enable_all_modules_auto_mode argument to launch files
  * set default value for enable_all_modules_auto_mode
  * fix enable_rtc configuration in scene_module_manager_interface.hpp
  ---------
* refactor(tier4_planning_launch): remove duplicate arguments in launchfile (`#6040 <https://github.com/autowarefoundation/autoware.universe/issues/6040>`_)
* feat(behavior_velocity_planner): add new 'dynamic_obstacle_stop' module (`#5835 <https://github.com/autowarefoundation/autoware.universe/issues/5835>`_)
* refactor(behavior_path_planner): remove use_experimental_lane_change_function (`#5889 <https://github.com/autowarefoundation/autoware.universe/issues/5889>`_)
* fix(behavior, launch): fix launch error (`#5847 <https://github.com/autowarefoundation/autoware.universe/issues/5847>`_)
  * fix(launch): set null to avoid launch error
  * fix(behavior): check null
  * chore(behavior): add comment
  * fix(launch): set  at the end of list
  * fix(launch): fill empty string at the end of module list
  ---------
* refactor(bpp): use pluginlib to load scene module (`#5771 <https://github.com/autowarefoundation/autoware.universe/issues/5771>`_)
  * refactor(bpp): use pluginlib
  * refactor(tier4_planning_launch): update launcher
  * refactor(avoidance): support pluginlib
  * refactor(lane_change): support pluginlib
  * refactor(dynamic_avoidance): support pluginlib
  * refactor(goal_planner): support pluginlib
  * refactor(side_shift): support pluginlib
  * refactor(start_planner): support pluginlib
  * refactor(bpp): move interface
  * fix(bpp): add const
  ---------
* fix(tier4_planning_launch): obstacle_cruise_planner pipeline is not connected (`#5542 <https://github.com/autowarefoundation/autoware.universe/issues/5542>`_)
* refactor(tier4_planning_launch): align argument name (`#5505 <https://github.com/autowarefoundation/autoware.universe/issues/5505>`_)
  * chore(tier4_planning_launch): align arument name
  * refactor(tier4_planning_launch): pass params directly
  ---------
* refactor(tier4_planning_launch): use xml style launch (`#5502 <https://github.com/autowarefoundation/autoware.universe/issues/5502>`_)
  * refactor(tier4_planning_launch): use xml style launch
  * refactor(tier4_planning_launch): remove python style launch
  * fix(tier4_planning_launch): enable console output
  ---------
* chore(planning modules): remove maintainer... (`#5458 <https://github.com/autowarefoundation/autoware.universe/issues/5458>`_)
  remove shimizu-san from maintainer and add maintainer for stop line and turn signal decider
* refactor(tier4_planning_launch): use xml style launch (`#5470 <https://github.com/autowarefoundation/autoware.universe/issues/5470>`_)
  * refactor(tier4_planning_launch): use xml style launch
  * refactor(tier4_planning_launch): remove python style launch
  * fix(tier4_plannning_launch): fix namespace
  ---------
* refactor(tier4_planning_launch): use xml style launch (`#5448 <https://github.com/autowarefoundation/autoware.universe/issues/5448>`_)
  * refactor(tier4_planning_launch): use xml style launch
  * refactor(tier4_planning_launch): remove python style launch
  ---------
* feat(behavior_path_planner): subscribe traffic light recognition result (`#5436 <https://github.com/autowarefoundation/autoware.universe/issues/5436>`_)
  feat(avoidance): use traffic light signal info
* feat(rtc_auto_mode_manager): eliminate rtc auto mode manager (`#5235 <https://github.com/autowarefoundation/autoware.universe/issues/5235>`_)
  * change namespace of auto_mode
  * delete RTC auto mode manager package
  * delete rtc_replayer.param
  * style(pre-commit): autofix
  * fix typo
  * fix typo
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(behavior_velocity): support new traffic signal interface (`#4133 <https://github.com/autowarefoundation/autoware.universe/issues/4133>`_)
  * feat(behavior_velocity): support new traffic signal interface
  * style(pre-commit): autofix
  * add missing dependency
  * style(pre-commit): autofix
  * remove the external signal input source in behavior_planning_launch.py
  * replace TrafficLightElement with TrafficSignalElement
  * style(pre-commit): autofix
  * use the regulatory element id instead of traffic light id
  * change the input of traffic signal to traffic light arbiter
  * style(pre-commit): autofix
  * do not return until the all regulatory elements are checked
  * change input topic of the traffic signals
  * fix the traffic signal type in perception reproducer
  * add debug log when the signal data is outdated
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(mission_planning.launch): add glog in mission planner (`#4745 <https://github.com/autowarefoundation/autoware.universe/issues/4745>`_)
* feat(motion_velocity_smoother.launch): add glog component (`#4746 <https://github.com/autowarefoundation/autoware.universe/issues/4746>`_)
  * use node instead of include
  * use container & add glog component
  ---------
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
* chore(tier4_planning_launch): enable to abort lane change from a parameter file (`#4469 <https://github.com/autowarefoundation/autoware.universe/issues/4469>`_)
* refactor(behavior_path_planner): remove unused config files (`#4241 <https://github.com/autowarefoundation/autoware.universe/issues/4241>`_)
* refactor(obstacle_avoidance_planner): move the elastic band smoothing to a new package (`#4114 <https://github.com/autowarefoundation/autoware.universe/issues/4114>`_)
  * Add path_smoothing package
  * Add elastic band smoother node
  * Add Debug section to elastic band documentation
  * Remove elastic band from the obstacle_avoidance_planner
  * Move elastic band debug images to the path_smoothing package
  * Update launch files to run the elastic_band_smoother
  * Set path topic names based on the path_smoother_type argument
  * Publish path with backward paths
  * Rename path_smoothing -> path_smoother
  ---------
* fix(obstacle_velocity_limiter): remove hardcoded parameter (`#4098 <https://github.com/autowarefoundation/autoware.universe/issues/4098>`_)
* refactor(lane_change): add namespace for lane-change-cancel (`#4090 <https://github.com/autowarefoundation/autoware.universe/issues/4090>`_)
  * refactor(lane_change): add namespace for lane-change-cancel
  * fix indent
  * lane_change_cancel -> cancel
  ---------
* refactor(behavior_velocity_planner): update launch and parameter files for plugin (`#3811 <https://github.com/autowarefoundation/autoware.universe/issues/3811>`_)
  * feat: move param files
  * WIP
  * feat: use behavior velocity module param file list
  * feat: update comment
  * feat: change param load
  * feat: update launch run out flag
  * feat: add disabled module as comment
  * feat: remove unused argument
  * fix test
  * remove unused params
  * move param
  * add test depend
  ---------
* refactor(start_planner): rename pull out to start planner (`#3908 <https://github.com/autowarefoundation/autoware.universe/issues/3908>`_)
* feat: handle invalid areas / lanelets (`#3000 <https://github.com/autowarefoundation/autoware.universe/issues/3000>`_)
* feat(behavior_path_planner): output stop reasons (`#3807 <https://github.com/autowarefoundation/autoware.universe/issues/3807>`_)
  * feat(launch): remap stop reasons
  * feat(behavior_path_planner): add interface to output stop reasons
  * feat(behavior_path_planner): add interface to output stop reasons
  * feat(avoidance): output stop reason
  ---------
* feat(path_sampler): add a sampling based path planner (`#3532 <https://github.com/autowarefoundation/autoware.universe/issues/3532>`_)
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
* feat(behavior_path_planner): add dynamic obstacle avoidance module (`#3415 <https://github.com/autowarefoundation/autoware.universe/issues/3415>`_)
  * implement dynamic avoidance module
  * update
  * update
  * fix spell
  * update
  * Update planning/behavior_path_planner/src/scene_module/dynamic_avoidance/dynamic_avoidance_module.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * Update planning/behavior_path_planner/include/behavior_path_planner/scene_module/dynamic_avoidance/dynamic_avoidance_module.hpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * Update planning/behavior_path_planner/docs/behavior_path_planner_dynamic_avoidance_design.md
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * dealt with review
  * update test
  * disable dynamic avoidance with old architecture, and pass build CI
  * fix
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* refactor(behavior_path_planner): rename pull_over to goal_planner (`#3501 <https://github.com/autowarefoundation/autoware.universe/issues/3501>`_)
* refactor(behavior_path_planeer): use common.params for lane change (`#3520 <https://github.com/autowarefoundation/autoware.universe/issues/3520>`_)
  * refactor(behavior_path_planeer): use common.params for lane change
  * update
  ---------
* feat(behavior_path_planner): move lane_following_params to behavior path params (`#3445 <https://github.com/autowarefoundation/autoware.universe/issues/3445>`_)
  * feat(behavior_path_planner): move lane_following_params to behavior path params
  * fix missing pakage include
  * fix test
  ---------
* chore(planning_evaluator): add dependency (`#3388 <https://github.com/autowarefoundation/autoware.universe/issues/3388>`_)
* feat(behavior_velocity_planner): add out of lane module (`#3191 <https://github.com/autowarefoundation/autoware.universe/issues/3191>`_)
  * Add OutOfLane module to the behavior_velocity_planner
  * Add functions for calculating path footprint and overlaps (WIP)
  * Update behavior_planning launch file to add out_of_lane param file
  TODO: remove launch-prefix from this commit. only needed for development
  * Add param to extend the ego footprint+fixed overlaps+started intervals
  * Implemented basic stop points insertion. "working" with simplified logic
  * Combine overlap and interval calculation, 1st rough working version
  * Add more parameters to replace magic numbers
  * [WIP] cleanup bugs and add a few more params
  * Proper stop point insertion (such that there are no overlaps)
  * Add interval visualization, fix bugs
  * Major refactoring and preparing params for 3rd method (TTC)
  * Implement TTC + more refactoring (not tested)
  * Fix issue with calculating enter/exit time of object after it entered
  * Fix bug in calculating ego distance along path
  * Add option to skip the new module if ego already overlaps another lane
  * Implement decel limit and add some (unimplemented) parameters
  * Implement the "strict" parameter (dont stop while overlapping)
  * Implement "use_predicted_paths" param (not yet tested)
  * Filter lanelets used for checking overlaps
  * Fix calculation of enter/exit times using predicted paths of objects
  * Improve "skip_if_already_overlapping" logic and add debug markers
  * Use dist(left, right) for inside distance when both bounds are overlaped
  * Add fallback when a point with no overlap cannot be found
  Fallback: use the path index previous to the range's entering path index
  * Increase max lateral deviation of predicted paths + add debug prints
  * Fix logic for select path_lanelets and other_lanelets + debug markers
  * Improve object filtering by their lateral distance from overlap range
  * Rename length -> dist in object_time_to_range function
  * Cleanup code and improve use of planner_data\_
  * Add overlapping_range.cpp + code cleanup
  * Add decisions.hpp + code cleanup
  * Add footprint.cpp
  * Cleanup and factorize more code
  * Add docstring + final factorization
  * Remove debug changes in behavior_planning.launch.py
  * Add out of lane design document (WIP)
  * Extend design doc and lint it
  * Finalize 1st draft of design doc (figures are missing)
  * Add figures
  * Fix some clang-tidy errors
  * Factorize the calculate_decisions function
  * Fix spelling relevent -> relevant
  * Add debug.hpp and debug.cpp to simplify createDebugMarkerArray()
  * Factorize calculate_slowdown_points
  * Factorize decisions.cpp a little more
  * Fix for clang tidy
  * Factorize decisions.cpp a little bit more
  * Update copyright
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * Update copyright
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * Update copyright
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * Fix copyrights Tier IV -> TIER IV
  * Populate StopReason
  * Set VelocityFactor
  * Fix design doc title
  * Populate StopReason only when stopping (not when slowing down)
  * Remove default value for declare_parameter of 'launch_run_out'
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* feat(avoidance_by_lc): add new module to avoid obstacle by lane change (`#3125 <https://github.com/autowarefoundation/autoware.universe/issues/3125>`_)
  * feat(rtc_interface): add new module avoidance by lc
  * feat(launch): add new param files
  * feat(avoidance_by_lc): add avoidance by lane change module
  * feat(behavior_path_planner): integrate avoidance by lc
  * fix(avoidance_by_lc): apply refactor
  * fix(avoidance_by_lc): use found_safe_path for ready check
  * fix request condition
  * fix build error
  ---------
* feat(behavior_path_planner): update behavior param file (`#3220 <https://github.com/autowarefoundation/autoware.universe/issues/3220>`_)
  * feat(behavior_path_planner): add new config file for manger
  * feat(launch): add config path
  * fix(behavior_path_planner): add missing param file
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
* feat(mission_planner): refine goal pose with parameter and add config file (`#2603 <https://github.com/autowarefoundation/autoware.universe/issues/2603>`_)
* feat(behavior_path_planner): pull over freespace parking (`#2879 <https://github.com/autowarefoundation/autoware.universe/issues/2879>`_)
  * feat(behavior_path_planner): pull over freespace parking
  * Update planning/behavior_path_planner/include/behavior_path_planner/scene_module/pull_over/pull_over_module.hpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * fix from review
  * add require_increment\_ explanation make the function
  * Update planning/behavior_path_planner/README.md
  * fix mutex
  * fix typo
  * fix build
  * pre-commit
  ---------
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* refactor(obstacle_avoidance_planner): clean up the code (`#2796 <https://github.com/autowarefoundation/autoware.universe/issues/2796>`_)
  * update obstacle avoidance planner, static centerline optimizer, tier4_planning_launch
  * update velocity on joint and correct trajectory z
  * update
  * minor change
  * pre-commit
  ---------
* refactor(planning_error_monitor): remove pkg (`#2604 <https://github.com/autowarefoundation/autoware.universe/issues/2604>`_)
  * remove planning_error_monitor
  * remove launch
  ---------
* fix(tier4_planning_launch): remove unnecessary config (`#2910 <https://github.com/autowarefoundation/autoware.universe/issues/2910>`_)
* feat(behavior_velocity): add mandatory detection area for run out module (`#2864 <https://github.com/autowarefoundation/autoware.universe/issues/2864>`_)
  * feat: add mandatory detection area
  * change the topic order to subscribe compare map filtered points
  * define function for transform pointcloud
  * add missing mutex lock
  * fix subscribing topic for points
  * remove unnecessary comments
  * add debug publisher for pointcloud
  * fix warning for empty frame id
  * add comments
  * add parameter whether to use mandatory detection area
  * use the same stop margin for the two kind of detection area
  * remove unused parameter
  * change max queue size
  * change the marker color of mandatory detection area
  * fix publishing debug pointcloud
  * create function to concatenate clouds
  * use current_odometory instead of current_pose
  * add param for mandatory area
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(behavior_path_planner): expand the drivable area based on the vehicle footprint (`#2609 <https://github.com/autowarefoundation/autoware.universe/issues/2609>`_)
* ci(pre-commit): autoupdate (`#2819 <https://github.com/autowarefoundation/autoware.universe/issues/2819>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(tier4_planning_launch): add missing params and sort params of costmap generator (`#2764 <https://github.com/autowarefoundation/autoware.universe/issues/2764>`_)
* refactor(behavior_path_planner): set occupancy grid map topic name from launch (`#2725 <https://github.com/autowarefoundation/autoware.universe/issues/2725>`_)
* feat(behavior_path_planner): external request lane change (`#2442 <https://github.com/autowarefoundation/autoware.universe/issues/2442>`_)
  * feature(behavior_path_planner): add external request lane change module
  feature(behavior_path_planner): fix for RTC
  feature(behavior_path_planner): fix decision logic
  feat(behavior_path_planner): fix behavior_path_planner_tree.xml
  feat(behavior_path_planner): fix for rebase
  feat(behavior_path_planner): output multiple candidate paths
  feat(behavior_path_planner): get path candidate in behavior tree manager
  feat(behavior_path_planner): fix for multiple candidate path
  feat(behavior_path_planner): separate external request lane change module
  feature(behavior_path_planner): add create publisher method
  feature(behavior_path_planner): move publishers to node
  feature(behavior_path_planner): remove unnecessary publisher
  feat(behavior_path_planner): move reset path candidate function to behavior tree manager
  feat(behavior_path_planner): add external request lane change path candidate publisher
  feat(behavior_path_planner): apply abort lane change
  * fix(behavior_path_planner): remove unnecessary change
  * feat(behavior_path_planner): fix getLaneChangePaths()
  * feat(behavior_path_planner): disable external request lane change in default tree
  * Update rtc_auto_mode_manager.param.yaml
  * fix(route_handler): remove redundant code
  * fix(behavior_path_planner): fix for turn signal
* feat(planning_validator): add planning validator package (`#1947 <https://github.com/autowarefoundation/autoware.universe/issues/1947>`_)
  * feat(planning_validator): add planning validator package
  * remove planning_error_monitor
  * pre-commit
  * change launch for planning_validator
  * Revert "remove planning_error_monitor"
  This reverts commit 90aed51a415c06d9c6e06fc437993602ff765b73.
  * restore error_monitor file
  * add readme
  * update for debug marker
  * add debug marker
  * fix invalid index error
  * update readme
  * update
  * add code to calc computation time
  * use reference arg
  * Revert "use reference arg"
  This reverts commit e81c91bafc0e61eaa9b6fa63feabba96205470ff.
  * remove return-vector code
  * Revert "add code to calc computation time"
  This reverts commit f36c7820ba47ccd3fbcd614e0aca0c414750b9cf.
  * update debug plot config
  * update readme
  * fix precommit
  * update readme
  * add invalid trajectory handling option
  * fix typo
  * Update README.md
  * update comments
  * pre-commit
  * fix typo
  * update
  * use util for marker create
  * fix tests
  * update doc!
  * fix readme
  * update
* feat(behavior_path_planner): modified goal with uuid (`#2602 <https://github.com/autowarefoundation/autoware.universe/issues/2602>`_)
  * feat(behavior_path_planner): modified goal with uuid
  * fix typo
  * fix for top header
  * change to PoseWithUuidStamped
* fix(tier4_planning_launch): make use_experimental_lane_change_function available (`#2676 <https://github.com/autowarefoundation/autoware.universe/issues/2676>`_)
* refactor(tier4_planning_launch): organize arguments (`#2666 <https://github.com/autowarefoundation/autoware.universe/issues/2666>`_)
  * refactor(tier4_planning_launch): organize arguments
  * update
* feat(behavior_path_planner): param to skip some linestring types when expanding the drivable area (`#2288 <https://github.com/autowarefoundation/autoware.universe/issues/2288>`_)
* feat(behavior_velocity_planner): add speed bump module (`#647 <https://github.com/autowarefoundation/autoware.universe/issues/647>`_)
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* fix(tier4_planning_launch): remove unintended config file (`#2554 <https://github.com/autowarefoundation/autoware.universe/issues/2554>`_)
* feat(tier4_planning_launch): remove configs and move to autoware_launch (`#2543 <https://github.com/autowarefoundation/autoware.universe/issues/2543>`_)
  * feat(tier4_planning_launch): remove configs and move to autoware_launch
  * fix
  * remove config
  * add rtc
  * Update launch/tier4_planning_launch/README.md
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* fix(intersection): fixed stuck vehicle detection area (`#2463 <https://github.com/autowarefoundation/autoware.universe/issues/2463>`_)
* feat(behavior_path_planner): remove unnecessary parameters (`#2516 <https://github.com/autowarefoundation/autoware.universe/issues/2516>`_)
  * feat(behavior_path_planner): remove unnecessary parameters
  * remove from static_centerline_optimizer
* feat(obstacle_cruies_planner): improve pid_based cruise planner (`#2507 <https://github.com/autowarefoundation/autoware.universe/issues/2507>`_)
  * feat(obstacle_cruies_planner): improve pid_based cruise planner
  * fix
  * update param in tier4_planning_launch
* feat(behavior_path_planner, obstacle_avoidance_planner): add new drivable area (`#2472 <https://github.com/autowarefoundation/autoware.universe/issues/2472>`_)
  * update
  * update
  * update
  * update obstacle avoidance planner
  * update
  * clean code
  * uddate
  * clean code
  * remove resample
  * update
  * add orientation
  * change color
  * update
  * remove drivable area
  * add flag
  * update
  * update color
  * fix some codes
  * change to makerker array
  * change avoidance utils
* refactor(behavior_path_planner): move turn_signal_on_swerving param to bpp.param.yaml (`#2406 <https://github.com/autowarefoundation/autoware.universe/issues/2406>`_)
  * move turn_signal_on_swerving param to bpp.param.yaml
  * change default value to true
  * add description
  * ci(pre-commit): autofix
  Co-authored-by: beyza <bnk@leodrive.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(avoidance): improve avoidance target filter (`#2329 <https://github.com/autowarefoundation/autoware.universe/issues/2329>`_)
  * feat(route_handler): add getMostLeftLanelet()
  * feat(avoidance): calc shiftable ratio in avoidance target filtering process
  * feat(avoidance): output object's debug info for rviz
  * fix(avoidance): use avoidance debug factor
  * feat(tier4_planning_launch): add new params for avoidance
  * fix(avoidance): reorder params for readability
  * fix(tier4_planning_launch): reorder params for readability
* feat(behavior_path_planner): update path when object is gone (`#2314 <https://github.com/autowarefoundation/autoware.universe/issues/2314>`_)
  * feat(behavior_path_planner): update state with obstacles.
  feat(behavior_path_planner): update path when obstacle is gone
  * ci(pre-commit): autofix
  * update check mechanism
  update check mechanism
  update check mechanism
  * readme.md is updated
  * ci(pre-commit): autofix
  * avoidance maneuver checker is added.
  ci(pre-commit): autofix
  avoidance maneuver checker is added.
  * fix check algorithm
  fix check algorithm
  * documentation is updated.
  * ci(pre-commit): autofix
  * fix typos
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(behavior_path_planner): add option to turn signal while obstacle swerving (`#2333 <https://github.com/autowarefoundation/autoware.universe/issues/2333>`_)
  * add turn_signal_on_swerving param
  * add option for signals
  * get turn_signal_on_swerving param from config file
  * ad turn_signal_on_swerving param
  * ci(pre-commit): autofix
  Co-authored-by: beyza <bnk@leodrive.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(obstacle_avoidance_planner): apply dynamic path length to fixed trajectory in eb (`#2357 <https://github.com/autowarefoundation/autoware.universe/issues/2357>`_)
  * fix(obstacle_avoidance_planner): apply dynamic path length to fixed trajectory in eb
  * add flag to enable clipping fixed trajectory
  * add maintainer
* fix(slow_down_planner): improper parameter used in slow down (`#2276 <https://github.com/autowarefoundation/autoware.universe/issues/2276>`_)
  * fix(slow_down_planner): improper parameter used in slow down
  * fix(tier4_planning_launch): remove hardcoded param enable_slow_down from launch.py
* feat(obstacle_avoidance_planner): parameterize non_fixed_trajectory_length (`#2349 <https://github.com/autowarefoundation/autoware.universe/issues/2349>`_)
* fix(behavior_path_planner): replace object_hold_max_count with object_last_seen_threshold (`#2345 <https://github.com/autowarefoundation/autoware.universe/issues/2345>`_)
  fix: replace object_hold_max_count with object_last_seen_threshold
* feat(behavior_velocity_planner): parameterize ego_yield_query_stop_duration for crosswalk module (`#2346 <https://github.com/autowarefoundation/autoware.universe/issues/2346>`_)
  feat: parameterize ego_yield_query_stop_duration for crosswalk module
* feat(avoidance): improve avoidance target filter (`#2282 <https://github.com/autowarefoundation/autoware.universe/issues/2282>`_)
  * feat(avoidance): use envelope polygon for measure against perception noise
  * feat(avoidance): use moving time for measure against perception noise
  * feat(tier4_planning_launch): add new params for avoidance
  * fix(avoidance): reserve marker array size
* feat(motion_velocity_smoother): tunable deceleration limit for curve … (`#2278 <https://github.com/autowarefoundation/autoware.universe/issues/2278>`_)
  feat(motion_velocity_smoother): tunable deceleration limit for curve deceleration
* feat(tier4_planning/control_launch): add missing dependency (`#2201 <https://github.com/autowarefoundation/autoware.universe/issues/2201>`_)
* feat: add 'obstacle_velocity_limiter' package (`#1579 <https://github.com/autowarefoundation/autoware.universe/issues/1579>`_)
  * Initial commit with barebone SafeVelocityAdjustorNode
  * Add debug topics, launch file, and config file
  * Fix debug markers
  * Fix dynamic parameters
  * Add proper collision detection and debug footprint
  Implements Proposal 1.
  Calculation of the adjusted velocity still needs to be improved
  * Add script to compare the original and adjusted velocity profiles
  * Fix calculation of distance to obstacle
  * Add test for calculation collision distance
  * Add launch file to test the safe_velocity_adjustor with a bag
  * Cleanup code and add tests for forwardSimulatedVector
  * Simplify collision detection by not using a footprint polygon
  * Add filtering of the dynamic objects from the pointcloud
  * [DEBUG] Print runtimes of expensive functions
  * Add trajectory downsampling to boost performance + improve debug markers
  * Modify velocity only from ego pose + distance parameter
  * Add 1st Eigen version of distanceToClosestCollision + benchmark
  * Switch to using contours from occupancy grid for collision checking
  Filtering of dynamic objects is not great
  * Add buffer around dynamic obstacles to avoid false obstacle detection
  * Add parameter to limit the adjusted velocity
  * Use vehicle_info_util to get vehicle footprint
  * Calculate accurate distance to collision + add tests
  * Add parameter for the min velocity where a dynamic obstacle is ignored
  * Add README and some pictures to explain the node inner workings
  * Update scenario_planning.launch.xml to run the new node
  * Fix format of launch files
  * Update launcher and rviz config used for debuging with bag
  * Cleanup debug publishing
  * Complete tests of collision_distance.hpp
  * Add docstring + Small code cleanup
  * Improve test of occupancy_grid_utils
  * Fix bug when setting parameter callback before getting vehicle parameters
  * Rename safe_velocity_adjustor to apparent_safe_velocity_limiter
  * Move declarations to cpp file (apparent_safe_velocity_limiter_node)
  * Move declarations to cpp file (occupancy_grid_utils)
  * Move declarations to cpp file (collision_distance)
  * Add exec of trajectory_visualizer.py in launch files
  * Mask trajectory footprint from the occupancy grid (might be expensive)
  * Filter out the occupancy grid that is outside the envelope polygon
  * Add improved PolygonIterator using scan line algorithm
  * Use autoware_cmake for dependencies
  * Improve performances of PolygonIterator
  * Minor cleanup of PolygonIterator
  * Use improved iterator + add benchmark (max/avg/med) to node
  * Minor code cleanup
  * Switch from set to vector/list in PolygonIterator
  * Remove PolygonIterator and use implementation from grid_map_utils
  * Add parameter to limit deceleration when adjusting the velocity
  * Code cleanup, move type decl and debug functions to separate files
  * Add support for collision detection using pointcloud
  * Code cleanup
  * Speedup pointcloud filtering (still ~100ms on bags)
  * Improve envelope calculation and use separate node for pcd downsampling
  * Add ProjectionParameters to prepare for the bicycle model projection
  * Add bicycle projection with various steering offsets
  * Update docstring
  * Major refactoring, calculate envelope from footprints
  * Add extraction of static obstacles from lanelet map
  * Remove stopwatch
  * Add arc distance calculation when using bicycle projection
  * Fix multi geometry definitions in tier4_autoware_utils/boost_geometry
  * Improve geometry operations to take advantage of Eigen
  * Switch to min/max offset and simplify footprint calculations
  * Fix unit tests (unset params.heading)
  * Add option to filter obstacles using the safety envelope
  * Fix bug with distance calculation and improve debug markers
  * Update README
  * Add parameter to set map obstacles by linestring id (for debug)
  * Move param structures to dedicated file and add PreprocessingParameters
  * Add parameter to calculate steering angle of trajectory points
  * Cleanup footprint generation
  * Fix bug with debug marker ids
  * Fix bug where the VelocityParameters were not constructed
  * Update obstacles extraction
  * Minor code cleanup
  * Switch to collision detection using rtree
  * Add publishing of the runtime (in microseconds)
  * Add option to ignore obstacles on the trajectory
  * Add max length and max duration parameters
  * Restructure Obstacles structure to separate lines and points for speedup
  * Convert obstacle linestrings to segments when used in the rtree
  * Add parameter for extra distance when filtering the ego path
  * Fix issues caused by rebase
  * Minor code cleanup
  * Update to run with looping bag replay
  * Add debug markers for obstacle masks and only publish when subscribed
  * Update README
  * Fix humble build issue with PCL library
  * Update obstacle extraction from lanelet map (no longer based on route)
  * Optimize use of rtree +  use naive collision checking with few obstacles
  * Remove debug code and update default parameters
  * Do not wait for self pose
  * Rename to obstacle_velocity_limiter
  * More minor cleanup
  * Update READEME.md
  * Update README to have the purpose written before the illustration
  * Update copyright notice: Tier IV -> TIER IV
  * Remove use_sim_time param from node launch file
  * Update launch files to run in the motion_planner + add launch config
* feat(motion_velocity_smoother): change osqp parameter (`#2157 <https://github.com/autowarefoundation/autoware.universe/issues/2157>`_)
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(motion_velocity_smoother): change max_lateral_accel from 0.8 to 1.0 (`#2057 <https://github.com/autowarefoundation/autoware.universe/issues/2057>`_)
* feat(behavior_path_planner): params to expand drivable area in each module (`#1973 <https://github.com/autowarefoundation/autoware.universe/issues/1973>`_)
* feat(behavior_path_planner): add turn signal parameters (`#2086 <https://github.com/autowarefoundation/autoware.universe/issues/2086>`_)
  * feat(behavior_path_planner): add and change parameters
  * update
  * update
* feat(behavior_path_planner): pull_over lateral goal search (`#2036 <https://github.com/autowarefoundation/autoware.universe/issues/2036>`_)
  * feat(behavior_path_planner): pull_over lateral goal search
  * fix werror of humble
* feat(obstacle_cruise_planner): add an explanation (`#2034 <https://github.com/autowarefoundation/autoware.universe/issues/2034>`_)
  * feat(obstacle_cruise_planner): add an explanation
  * update readme
* feat(run_out): avoid chattering of state transition (`#1975 <https://github.com/autowarefoundation/autoware.universe/issues/1975>`_)
  * feat: keep approach state to avoid chattering of detection
  * add parameter
  * update parameter
  * update documents
  * revert changed parameter
* feat(obstacle_cruise_planner): add goal safe distance (`#2031 <https://github.com/autowarefoundation/autoware.universe/issues/2031>`_)
* chore(behavior_velocity): add maintainer for run out module (`#1967 <https://github.com/autowarefoundation/autoware.universe/issues/1967>`_)
* refactor(run_out): add state machine class for state transition  (`#1884 <https://github.com/autowarefoundation/autoware.universe/issues/1884>`_)
  * refactor(run_out): add state machine class for state transition
  * remove debug print
  * move parameters
  * add missing parameter
  * add documents
  * fix conflict
  * remove unused argument
  * fix parameter value
* feat(behavior_path_planner): add pull_over base class (`#1911 <https://github.com/autowarefoundation/autoware.universe/issues/1911>`_)
  * feat(behavior_path_planner): add pull_over base class
  * modify calculation of velocity abs
  * modify from review
  * add const
  * refactor shift pull over
  * not use shared_ptr for lane_departure_checker
  * fix deceleration
  * Update planning/behavior_path_planner/src/scene_module/pull_over/shift_pull_over.cpp
  Co-authored-by: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
  * fix werror
  * fix build for main
  Co-authored-by: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
* chore(tier4_planning_launch): add maintainers (`#1955 <https://github.com/autowarefoundation/autoware.universe/issues/1955>`_)
* feat(intersection): use intersection_area if available (`#1733 <https://github.com/autowarefoundation/autoware.universe/issues/1733>`_)
* refactor: replace acc calculation in planning control modules (`#1213 <https://github.com/autowarefoundation/autoware.universe/issues/1213>`_)
  * [obstacle_cruise_planner] replace acceleration calculation
  * [obstacle_stop_planner] replace acceleration calculation
  * [trajectory_follower] replace acceleration calculation
  * remap topic name in lanuch
  * fix nullptr check
  * fix controller test
  * fix
* fix: fix missing dependency (`#1891 <https://github.com/autowarefoundation/autoware.universe/issues/1891>`_)
  * fix: fix missing dependency
  * fix
* feat(obstacle_avoidance_planner): fix can be applied to the first trajectory point (`#1775 <https://github.com/autowarefoundation/autoware.universe/issues/1775>`_)
  * add bicycle model collision avoidance and single fixed point
  * refactor manual warm start
  * add calculation cost plotter
  * fix
  * fix
  * update params
* feat(rtc_auto_mode_manager): add rtc_auto_mode_manager and fix auto mode behavior (`#1541 <https://github.com/autowarefoundation/autoware.universe/issues/1541>`_)
  * feat(rtc_auto_mode_manager): add rtc_auto_mode_manager and fix auto mode behavior
  * ci(pre-commit): autofix
  * fix(rtc_auto_mode_manager): fix typo
  * fix(rtc_interface): revert namespace
  * fix(rtc_interface): reset auto mode just only related uuid
  * fix(rtc_auto_mode_manager): fix
  * fix(tier4_planning_launch): launch rtc_auto_mode_manager
  * Update launch/tier4_planning_launch/launch/scenario_planning/lane_driving.launch.xml
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * fix(rtc_interface): fix
  * fix(behavior_velocity_planner): fix initialization in crosswalk module
  * feat(rtc_auto_mode_manager): fix initialization
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* feat(behavior_planning): use acceleration from localization module (`#1859 <https://github.com/autowarefoundation/autoware.universe/issues/1859>`_)
  * feat(behavior_path_planner): subscribe acceleration from localization module
  * feat(behavior_velocity_planner): subscribe acceleration from localization module
* refactor(run_out): remove unused parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
* feat(obstacle_cruise_planner): add terminal collision checker (`#1807 <https://github.com/autowarefoundation/autoware.universe/issues/1807>`_)
  * feat(motion_utils): add new search zero velocity
  * change arguments
  * feat(obstacle_cruise_planner): add terminal collision checker
  * add parameters
  * change parameters
* feat(behavior_path_planner): change pull over params (`#1815 <https://github.com/autowarefoundation/autoware.universe/issues/1815>`_)
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
* feat(behavior_path_planner): check goal to objects logitudinal distance for pull_over (`#1796 <https://github.com/autowarefoundation/autoware.universe/issues/1796>`_)
  * feat(behavior_path_planner): check goal to objects logitudinal distance for pull_over
  * Update planning/behavior_path_planner/src/utilities.cpp
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  * rename to goal_to_obstacle_margin
  * fix rear check
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* refactor(obstacle_stop_planner): update params name for readability (`#1720 <https://github.com/autowarefoundation/autoware.universe/issues/1720>`_)
  * refactor(obstacle_stop_planner): update parameter name for readability
  * docs(obstacle_stop_planner): update module documentation
  * docs(obstacle_stop_planner): update figure
  * refactor(obstacle_stop_planner): separate params by namespace
  * fix(tier4_planning_launch): separate params by namespace
  * refactor(obstacle_stop_planner): remove default value from declare_parameter
  * refactor(obstacle_stop_planner): add params to config
* fix(behavior_path_planner): fix pull_over request_length and maximum_deceleration (`#1789 <https://github.com/autowarefoundation/autoware.universe/issues/1789>`_)
* feat(behavior_path_planner): use object recognition for pull_over (`#1777 <https://github.com/autowarefoundation/autoware.universe/issues/1777>`_)
  * feat(behavior_path_planner): use object recognition for pull_over
  * Update planning/behavior_path_planner/src/scene_module/pull_over/pull_over_module.cpp
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  * rename checkCollision
  * update docs
  * remove unnecessary lines
  * update warn message
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* feat(behavior_path_planner): update pull out (`#1438 <https://github.com/autowarefoundation/autoware.universe/issues/1438>`_)
  * feat(behavior_path_planner): update pull out
  * refactor(behavior_path_planner): rename pull_out params
  * update from review
  * use debug_data
  * enable back
  * move PlannerType
  * fix debug marker
  * add seach priority
  * change before_pull_out_straight_distance to 0.0
* refactor(behavior_path_planner): rename pull_over params (`#1747 <https://github.com/autowarefoundation/autoware.universe/issues/1747>`_)
* feat(intersection): continue detection after pass judge (`#1719 <https://github.com/autowarefoundation/autoware.universe/issues/1719>`_)
* feat(behavior_path_palnner): update geometric parallel parking for pull_out module (`#1534 <https://github.com/autowarefoundation/autoware.universe/issues/1534>`_)
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
* feat(behavior_path_planner): enable pull_over backward_parking by default (`#1653 <https://github.com/autowarefoundation/autoware.universe/issues/1653>`_)
* feat(obstacle_avoidance_planne): enable plan_from_ego by default (`#1673 <https://github.com/autowarefoundation/autoware.universe/issues/1673>`_)
* feat: add vector map inside area filter (`#1530 <https://github.com/autowarefoundation/autoware.universe/issues/1530>`_)
  * feat: add no detection area filter
  * ci(pre-commit): autofix
  * chore: add documents
  * pre-commit fix
  * remove comments
  * fix comments
  * refactor condition to launch points filter
  * fix container name
  * ci(pre-commit): autofix
  * chore: add visualization for no obstacle segmentation area
  * feat: allow any tags to be given by launch arguments
  * chore: remove unnecessary includes
  * feat: move the polygon removing function to util and use it
  * chore: move the place and change the name of node
  * chore: pre-commit fix
  * chore: remove unnecessary using
  * chore: modify container name
  * chore: fix comments
  * chore: fix comments
  * chore: use output arguments for a large data
  * chore: using namespace of PolygonCgal for readability
  * feat: add functions for multiple polygons
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(mission_planner): prepare to support ad api (`#1561 <https://github.com/autowarefoundation/autoware.universe/issues/1561>`_)
  * refactor(mission_planner): prepare to support ad api
  * fix node name
* feat(surround_obstacle_checker): add vehicle footprint with offset (`#1577 <https://github.com/autowarefoundation/autoware.universe/issues/1577>`_)
  * fix: right and left overhang fix in SelfPolygon func
  * feat: init base polygon
  * ci(pre-commit): autofix
  * fix: change publishers scope
  * feat: pub footprint
  * feat: pub footprint with offset
  * feat: pub footprint with recover offset
  * feat: add footprint publish boolean param to config
  * docs: update readme
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(behavior_path_planner): resample output path (`#1604 <https://github.com/autowarefoundation/autoware.universe/issues/1604>`_)
  * feat(behavior_path_planner): resample output path
  * update param
* fix(behavior_velocity_planner): disable debug path publisher by default (`#1680 <https://github.com/autowarefoundation/autoware.universe/issues/1680>`_)
* fix(behavior_path_planner): pull_over shift parking (`#1652 <https://github.com/autowarefoundation/autoware.universe/issues/1652>`_)
  * fix(behavior_path_planner): pull_over shift parking
  * check lane_depature for each shift path
  * change pull_over_velocity to 3.0
* feat(obstacle_cruise_planner): add velocity_threshold to outside obstacle (`#1646 <https://github.com/autowarefoundation/autoware.universe/issues/1646>`_)
  * feat(obstacle_cruise_planner): add velocity_threshold to outside obstacle
  * add parameter to config
  * update readme
* feat(behavior_velocity): publish internal debug path (`#1635 <https://github.com/autowarefoundation/autoware.universe/issues/1635>`_)
  * feat(behavior_velocity): publish internal path as debug path
  * feat(behavior_velocity): add debug internal scene module path
  * feat(behavior_velcoity, planning_debug_tools): add params for debug path
* feat(run_out): add lateral nearest points filter  (`#1527 <https://github.com/autowarefoundation/autoware.universe/issues/1527>`_)
  * feat(run_out): add lateral nearest points filter
  * chore: update documents
  * chore: pre-commit fix
  * chore: fix typo
* fix(tier4_planning_launch): change parameter to enable abort lane change (`#1602 <https://github.com/autowarefoundation/autoware.universe/issues/1602>`_)
* feat(tier4_planning_launch): add nearest search param (`#1582 <https://github.com/autowarefoundation/autoware.universe/issues/1582>`_)
  * feat(tier4_planning_launch): add nearest search param
  * fix
* feat(obstacle_cruise_planner): delete shape from target obstacle (`#1558 <https://github.com/autowarefoundation/autoware.universe/issues/1558>`_)
  * delete is on ego traj
  * update
  * feat(obstacle_cruise_planner): delete shape
  * update
  * remove unnecessary parameters
  * add new calc distance
  * add threshold
  * fix a bug
  * fix terminal point
  * update
  * update parameters
* fix(behavior_velocity_planner, tier4_planning_launch): modify delay_resopnse_time (`#1557 <https://github.com/autowarefoundation/autoware.universe/issues/1557>`_)
  * fix(behavior_velocity_planner): modify delay_resopnse_time
  * fix(tier4_planning_launch): modify delay_resopnse_time
* fix(costmap_generator): restrict costmap within parking lot (`#996 <https://github.com/autowarefoundation/autoware.universe/issues/996>`_)
  * fix(costmap_generator): restrict costmap within parking lot
  * add parameters for free space planning area selection
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(obstacle_cruise_planner): add missing param (`#1515 <https://github.com/autowarefoundation/autoware.universe/issues/1515>`_)
* fix(behavior_path_planner): fix turn singal output in a avoidance sequence (`#1511 <https://github.com/autowarefoundation/autoware.universe/issues/1511>`_)
  * remove search distance for turn signal
  * set distance to max when a lane_attriute is straight
* refactor(obstacle_avoidance_planner): use max_steer_angle in common (`#1423 <https://github.com/autowarefoundation/autoware.universe/issues/1423>`_)
  * refactor(obstacle_avoidance_planner): use max_steer_angle in common
  * fix runtime error
  * fix
  * fix yaml file
* feat(behavior_velocitiy_planner): predict front vehicle deceleration in intersection and temporarily stop (`#1194 <https://github.com/autowarefoundation/autoware.universe/issues/1194>`_)
  * calculating stopping distance for frontcar from estimated velocity
  * calc stopping_point_projected and stopping_point along centerline
  * create stuck_vehicle_detect_area in modifyVelocity(TODO: use pose of frontcar at stopping_position
  * use centerline on ego_lane_with_next_lane
  * properly checking if stopping_point is in stuck_vehicle_detect_area
  * also check if the point behind stopping_point is in detection_area(aka attention area)
  * refactored
  * will return collision_deteced if is_in_sturck_area && is_behind_point_in_detection_area
  * look working
  * refactored, rename parameter
  * added flag
  * fixed the order of isAheadOf, working in scenario test as well
  * added description in stuck vehicle detection section
  * reflected comments: (1) use vector of ids (2) changed intersection.param.yaml
* feat(tier4_planning_launch): declare param path argument (`#1337 <https://github.com/autowarefoundation/autoware.universe/issues/1337>`_)
  * feat(tier4_planning_launch): declare param path argument
  * Update launch/tier4_planning_launch/launch/planning.launch.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update launch/tier4_planning_launch/launch/planning.launch.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* fix(behavior_path_planner): remove unnecessary publisher and subscriber  (`#1371 <https://github.com/autowarefoundation/autoware.universe/issues/1371>`_)
  * fix(behavior_path_planner): remove unnecessary publisher and subscriber
  * fix(tier4_planning_launch): fix launch file
  * fix(tier4_planning_launch): fix xml file
* fix: fix parameter names of motion_velocity_smoother (`#1376 <https://github.com/autowarefoundation/autoware.universe/issues/1376>`_)
  * fix: fix parameter names of motion_velocity_smoother
  * fix indent
* feat(intersection_module): add option to change the stopline position (`#1364 <https://github.com/autowarefoundation/autoware.universe/issues/1364>`_)
  * use constexpr
  * add stopline before intersection
  * feat(intersection_module): add update stopline index before intersection
  * modify to generate stop line to consider for the vehicle length
  * add generate stop line before intersection func
  * generate pass judge line when use_stuck_stopline
  * update param at launch
  * reduce nest
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_planning_launch): add group tag (`#1239 <https://github.com/autowarefoundation/autoware.universe/issues/1239>`_)
  * fix(tier4_planning_launch): add group tag
  * move arg
  * move arg inside group
* fix(behavior_velocity_planner): fix rtc behavior in crosswalk module (`#1296 <https://github.com/autowarefoundation/autoware.universe/issues/1296>`_)
* feat(behavior_path_planner): update pull_over module (`#873 <https://github.com/autowarefoundation/autoware.universe/issues/873>`_)
  * feat(behavior_path_planner): update pull_over module
  * use tf2_geometry_msgs/tf2_geometry_msgs.hpp for humble
  * fix werror of humble
  * fix test
  * fix goal change bug when starting drive
* feat(tier4_planning_launch): update crosswalk param (`#1265 <https://github.com/autowarefoundation/autoware.universe/issues/1265>`_)
* feat(behavior_velocity): filter points with detection area (`#1073 <https://github.com/autowarefoundation/autoware.universe/issues/1073>`_)
  * feat(behavior_velocity): filter points with detection area
  * remove unnecessary functions
  * update documents
  * ci(pre-commit): autofix
  * remove unnecessary comments
  * use parameter for max deceleration jerk
  * update configs
  * return empty points when the detection area polygon is empty
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(obstacle_avoidance_planner): add description of max_plan_from_ego_length (`#1223 <https://github.com/autowarefoundation/autoware.universe/issues/1223>`_)
  * chore(obstacle_avoidance_planner): add description of max_plan_from_ego_length
  * fix typo
* feat(obstacle_cruise_planner): implemented common obstacle stop (`#1185 <https://github.com/autowarefoundation/autoware.universe/issues/1185>`_)
  * feat(obstacle_cruise_planner): implemented common obstacle stop
  * fix some implementation
  * minor changes
  * use min_ego_accel_for_rss
  * remove unnecessary code
  * fix CI error
  * fix typo
* refactor(freespace_planner): parameterize margin. (`#1190 <https://github.com/autowarefoundation/autoware.universe/issues/1190>`_)
* fix(intersection_module): remove decel parameter (`#1188 <https://github.com/autowarefoundation/autoware.universe/issues/1188>`_)
  * fix(intersection_module): remove decel parameter
  * remove unuse parameter
* feat(obstacle_cruise_planner): some minor updates (`#1136 <https://github.com/autowarefoundation/autoware.universe/issues/1136>`_)
  * checkout latest obstacle cruise changes
  * fix cruise/stop chattering
  * add lpf for cruise wall
  * fix debug print
  * fix cruise wall
  * fix min_behavior_stop_margin bug
  * not use predicted path for obstacle pose for now
  * update tier4_planning_launch param
  * fix typo
  * fix CI error
* feat(obstacle_cruise_planner): clean parameters for optimization based cruise planner (`#1059 <https://github.com/autowarefoundation/autoware.universe/issues/1059>`_)
  * remove unnecessary parameter
  * add new parameter and delete unnecessary constructor
  * remove unnecessary parameter
  * clean parameter
  * delete filtering parameter in optimization based algorithm
  * update
  * delete yaw threshold parameter and update license
  * update
  * remove unnecessary checker
* feat(intersection): add conflicting area with margin debug (`#1021 <https://github.com/autowarefoundation/autoware.universe/issues/1021>`_)
  * add detection area margin debug
  * extention lanelet in intersection function
  * feat: add conflicting area with margin
  * fix(intersection_module): remove unnecessary comment
  * fix check collision
  * fix(intersection_module): remove unnecessary diff
  * ci(pre-commit): autofix
  * fix(intersection_module): fix expand lane only right bound
  * fix(intersection_module): remove calc of detection area to object distance
  * ci(pre-commit): autofix
  * fix(intersection_module): split lane extentions
  * ci(pre-commit): autofix
  * refactor: lanelet::utils::resamplePoints -> resamplePoints
  * feat: add right and left margin parameter
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_planning_launch): launch rtc_auto_approver (`#1046 <https://github.com/autowarefoundation/autoware.universe/issues/1046>`_)
  * feature(tier4_planning_launch): launch rtc_auto_approver
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(obstacle_cruise_planner): add new package (`#570 <https://github.com/autowarefoundation/autoware.universe/issues/570>`_)
  * feat(obstacle_velocity_planner): add obstacle_velocity_planner
  * udpate yaml
  * update dependency
  * fix maybe-unused false positive error
  * Tier IV -> TIER IV
  * fix some reviews
  * fix some reviews
  * minor change
  * minor changes
  * use obstacle_stop by default
  * fix compile error
  * obstacle_velocity -> adaptive_cruise
  * fix for autoware meta repository
  * fix compile error on CI
  * add min_ego_accel_for_rss
  * fix CI error
  * rename to obstacle_cruise_planner
  * fix tier4_planning_launch
  * fix humble CI
* feat(behavior_velocity): add run out module (`#752 <https://github.com/autowarefoundation/autoware.universe/issues/752>`_)
  * fix(behavior_velocity): calculate detection area from the nearest point from ego (`#730 <https://github.com/autowarefoundation/autoware.universe/issues/730>`_)
  * fix(behavior_velocity): calculate lateral distance from the beginning of the path
  * add argument of min_velocity
  * use veloicty from the nearest point from ego
  * pass struct by reference
  * fix to interpolate point in util
  * fix(longitudinal_controller_node, vehicle_cmd_gate): update stopped condition and behavior (`#700 <https://github.com/autowarefoundation/autoware.universe/issues/700>`_)
  * fix(longitudinal_controller_node): parameterize stopped state entry condition
  * fix(longitudinal_controller_node): simply set stopped velocity in STOPPED STATE
  * fix(vehicle_cmd_gate): check time duration since the vehicle stopped
  * docs(autoware_testing): fix link (`#741 <https://github.com/autowarefoundation/autoware.universe/issues/741>`_)
  * docs(autoware_testing): fix link
  * fix typo
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix: trajectory visualizer (`#745 <https://github.com/autowarefoundation/autoware.universe/issues/745>`_)
  * fix(tier4_autoware_utils): modify build error in rolling (`#720 <https://github.com/autowarefoundation/autoware.universe/issues/720>`_)
  * fix(tier4_autoware_utils): modify build error in rolling
  * fix(lanelet2_extension): add target compile definition for geometry2
  * fix(ekf_localizer): add target compile definition for geometry2
  * fix(freespace_planning_algorithms): add target compile definition for geometry2
  * fix(interpolation): add target compile definition for geometry2
  * fix(freespace_planner): add target compile definition for geometry2
  * fix(lane_departure_checker): add target compile definition for geometry2
  * fix(map_based_prediction): add target compile definition for geometry2
  * fix(ground_segmentation): add target compile definition for geometry2
  * fix(motion_velocity_smoother): add target compile definition for geometry2
  * fix(multi_object_tracker): add target compile definition for geometry2
  * fix(trajectory_follower): add target compile definition for geometry2
  * fix(control_performance_analysis): add target compile definition for geometry2
  * fix(detected_object_validation): add target compile definition for geometry2
  * fix(goal_distance_calculator): add target compile definition for geometry2
  * fix(ndt_scan_matcher): add target compile definition for geometry2
  * fix(route_handler): add target compile definition for geometry2
  * fix(behavior_path_planner): add target compile definition for geometry2
  * fix(mission_planner): add target compile definition for geometry2
  * fix(obstacle_avoidance_planner): add target compile definition for geometry2
  * fix(obstacle_stop_planner): add target compile definition for geometry2
  * fix(obstacle_collision_checker): add target compile definition for geometry2
  * fix(shape_estimation): add target compile definition for geometry2
  * fix(behavior_velocity_planner): add target compile definition for geometry2
  * fix(path_distance_calculator): add target compile definition for geometry2
  * fix(detection_by_tracker): add target compile definition for geometry2
  * fix(surround_obstacle_checker): add target compile definition for geometry2
  * fix(probabilistic_occupancy_grid_map): add target compile definition for geometry2
  * fix(tier4_debug_tools): add target compile definition for geometry2
  * fix(tier4_vehicle_rviz_plugin): add target compile definition for geometry2
  * fix(pure_pursuit): add target compile definition for geometry2
  * fix(trajectory_follower_nodes): add target compile definition for geometry2
  * fix(occupancy_grid_map_outlier_filter): add target compile definition for geometry2
  * fix(traffic_light_map_based_detector): add target compile definition for geometry2
  * fix(planning_error_monitor): add target compile definition for geometry2
  * fix(planning_evaluator): add target compile definition for geometry2
  * fix(lidar_centerpoint): add target compile definition for geometry2
  * fix(behavior_velocity): handle the case when finding index failed (`#746 <https://github.com/autowarefoundation/autoware.universe/issues/746>`_)
  * feat: add scene module of dynamic obstacle stop
  * fix warnings
  * add temporary debug value
  * add feature to go after stopping
  * fix parameter namespace
  * use planner util
  * fix calculation when multiple obstacles are detected in one step polygon
  * remove unnecessary debug
  * add option to apply limit jerk
  * Modify parameter name
  * Add param file
  * remove unnecessary comments
  * add feature to exclude obstacles outside of partition
  * modify search distance for partitions
  * apply voxel grid filter to input points
  * set smoother param by passing node instance
  * add parameter for velocity limit (temporary)
  * add dynamic reconfigure
  * add debug value
  * avoid acceleration when stopping for obstacles
  * fix lateral distance to publish distance from vehicle side
  * modify the location to publish longitudinal distance
  * fix calculation of stop index
  * use current velocity for stop dicision
  * add dynamic parameter for slow down limit
  * add debug value to display passing dist
  * modify stopping velocity
  * update param
  * use smoother in planner data
  * use path with lane id instead of trajectory
  * remove unnecessary data check
  * extend path to consider obstacles after the end of the path
  * rename public member variables
  * remove unused paramter
  * create detection area using util
  * fix visualization of stop distance marker
  * make option for detection method easier to understand
  * remove parameter about whether to enable this feature
  * calculate and publish debug data in function
  * use compare map filtered points
  * add comment
  * fix visualization of detection area when calculation of stop distance failed
  * add option whether to specify the jerk
  * fix format
  * change parameter name
  * remove dynamic reconfigure
  * delete unused file
  * remove unnecessary comments
  * remove unnecessary includes
  * add launcher for compare map
  * add launch and config for dynamic obstacle stop planner
  * fix finding package name
  * handle the change of util
  * relay points for simulation
  * update parameter
  * fix position and color of stop line marker
  * pre-commit fixes
  * remove unnecessary comments
  * fix Copyright
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * fix Copyright
  * fix typo
  * add documents for dynamic obstacle stop module
  * ci(pre-commit): autofix
  * update documents
  * docs: begin a sentence with a capital letter
  * docs: replace predicted with smoothed for path velocity
  * create interface class to switch method
  * run compare map filter only when points method is used
  * delete unused functions
  * rename functions for inserting velocity
  * rename parameter of path_size to max_prediction_time
  * fix(behavior_velocity_planner): dynamic obstacle stop planner docs
  * fix(behavior_velocity_planner): add ego vehicle description
  * fix(behavior_velocity_planner): change space to hyphen
  * change smoothed to expected target velocity
  * Start a sentence in figure with a capital letter
  * fix typo
  * use voxel distance based compare map
  * select detection method from param file
  * do not launch as default for now
  * rename dynamic_obstacle_stop to run_out
  * remove unnecessary change
  * remove unnecessary changes
  * remove unnecessary changes
  * fix typo
  * change default to false
  * update cmake to build run_out module
  * add launch_run_out parameter
  * Add note for compare map filtered points
  * handle the change for virtual wall marker
  * rename the parameters for smoother
  * fix build error in humble
  * fix build error in humble
  * launch compare map only when run out module is enabled
  * update a document
  * add calculation time for modify path
  * update a document
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Esteve Fernandez <esteve.fernandez@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Makoto Kurihara <mkuri8m@gmail.com>
* feat(tier4_planning_launch): create parameter yaml for behavior_velocity_planner (`#887 <https://github.com/autowarefoundation/autoware.universe/issues/887>`_)
  * feat(tier4_planning_launch): create parameter yaml for behavior_velocity_planner
  * Update launch/tier4_planning_launch/config/scenario_planning/lane_driving/behavior_planning/behavior_velocity_planner/behavior_velocity_planner.param.yaml
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * feat: add param.yaml in behavior_velocity_planner package
  * some fix
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
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
* feat(behavior_velocity): find occlusion more efficiently (`#829 <https://github.com/autowarefoundation/autoware.universe/issues/829>`_)
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
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* feat(obstacle_avoidance_planner): parameterize bounds search widths (`#807 <https://github.com/autowarefoundation/autoware.universe/issues/807>`_)
  * feat(obstacle_avoidance_planner): parameterize bounds search widths
  * update bounds search widths
  * update tier4_planning_launch
  * Added parameter description of README.md
* chore(behavior_velocity): add system delay parameter and minor update (`#764 <https://github.com/autowarefoundation/autoware.universe/issues/764>`_)
  * chore(behavior_velocity): add system delay parameter and minor update
  * doc(behavior_velocity): add system delay discription
* fix(motion_velocity_smoother): add stop decel parameter (`#739 <https://github.com/autowarefoundation/autoware.universe/issues/739>`_)
  * add stop decel parameter
  * add stop decel parameter
* feat(behavior_velocity): occlusion spot generate not detection area occupancy grid (`#620 <https://github.com/autowarefoundation/autoware.universe/issues/620>`_)
  * feat(behavior_velocity): filter dynamic object by default
  * feat(behavior_velocity): raycast object shadow
  * chore(behavior_velocity): replace target vehicle to filtered vehicle in detection area
  * chore(behavior_velocity): update docs and settings
  * chore(behavior_velocity): cosmetic change
  * chore(behavior_velocity): consider delay for detection
  * fix(behavior_velocity): fix launch and stuck vehicle
  * chore(behavior_velocity): use experiment value
  * chore(behavior_velocity): add comment
* fix(tier4_planning_launch): fix tier4_planning_launch package (`#660 <https://github.com/autowarefoundation/autoware.universe/issues/660>`_)
* feat(behavior_path_planner): stop lane_driving planners in non-lane-driving scenario (`#668 <https://github.com/autowarefoundation/autoware.universe/issues/668>`_)
  * stop lanedriving in parking scenario
  * use skip_first
  * add scenario remap in launch
  * replace warn to info
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(behavior_path_planner): parametrize avoidance target type (`#574 <https://github.com/autowarefoundation/autoware.universe/issues/574>`_)
  * parametrize avoidance target type
  * add target type parameter in yaml
  * mototbike -> motorcycle
  * apply clang
* feat(obstacle avoidance planner): fix out curve, make calculation cost low, make optimization stable, refactor, etc. (`#233 <https://github.com/autowarefoundation/autoware.universe/issues/233>`_)
  * feat (obstacle avoidance planner) fix out curve, make optimization stable, make computation cost low, etc
  * fix typos
  * remove unnecessary codes
  * minor refactoring
  * add path shape change detection for replan
  * add feature explanation in README
  * move some parameters to advanced
  * calcualte radius num automatically
  * update config in tier4_planning_launch
  * truncate path to detect path change, and tune path change detection
  * disable yaw slerp
  * fix ci error
* fix(behavior_path_planner): parametrize avoidance lateral distance threshold (`#404 <https://github.com/autowarefoundation/autoware.universe/issues/404>`_)
  * parametrize lateral threshold
  * format readme
  * apply clang format
* refactor(scenario_planning.launch.xml): add parameter description (`#464 <https://github.com/autowarefoundation/autoware.universe/issues/464>`_)
* feat(behavior_path_planner): make drivable area coordinate fixed to the map coordinate and make its size dynamic (`#360 <https://github.com/autowarefoundation/autoware.universe/issues/360>`_)
  * make drivable area not to oscillate and its size dynamic
  * update README for new parameters
  * remove getLaneletScope from route_handler
  * resolve temporary fix resolution multiplied by 10.0
  * dealt with edge case where length of a lane is too long
  * rename function and put it in proper namespace
  * update param for tier4_planning_launch
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* feat(behavior_path_planner): better avoidance drivable areas extension in behavior path planning (`#287 <https://github.com/autowarefoundation/autoware.universe/issues/287>`_)
  * feat: Increases the flexibility of the function in dealing with several scenarios
  The implementation updates generateExtendedDrivableArea
  this is a part of .iv PR (`tier4/autoware.iv#2383 <https://github.com/tier4/autoware.iv/issues/2383>`_) port
  * feat: change shift_length computation method
  The new method considers the availability of the lane on the left side or right
  side of ego vehicle, depending on the position of the obstacles.
  Will not generate avoidance path if the adjacent lane is not a lanelet.
  this is a part of .iv PR (`tier4/autoware.iv#2383 <https://github.com/tier4/autoware.iv/issues/2383>`_) port
  * feat: Adds documentation and debug marker visualization.
  The visualization will show the remaining distance between the overhang_pose and
  referenced linestring.
  this is a part of .iv PR (`tier4/autoware.iv#2383 <https://github.com/tier4/autoware.iv/issues/2383>`_) port
  * fix: change debug naming to show clearer intent
  The fix also removes a space character from one of the debug printing function call.
  * fix: replace equation from latex to math jax
  Also remove the equation's image
  * fix: equation spacing
  * Fix: slight improvement in debug marker
  1. Increase line ID text size and
  2. Remove unnecessary visualization of connection between last point and the
  front point linestring.
  * fix: drivable area not extend to third lane during extension
  * fix: slightly increase lateral_collision_safety_buffer to 0.7
  The decision to increase is based on discussion with the planning control team
  and also from input by FI team.
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* fix: use autoware_auto_msgs (`#197 <https://github.com/autowarefoundation/autoware.universe/issues/197>`_)
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
* Contributors: Ahmed Ebrahim, Berkay Karaman, Fumiya Watanabe, Hiroki OTA, Ismet Atabay, Karmen Lu, Kenji Miyake, Kosuke Takeuchi, Kyoichi Sugahara, Mamoru Sobue, Maxime CLEMENT, Mehmet Dogru, NorahXiong, Satoshi OTA, Shumpei Wakabayashi, Takagi, Isamu, Takamasa Horibe, Takayuki Murooka, Takeshi Miura, Tomohito ANDO, Tomoya Kimura, Vincent Richard, Xinyu Wang, Yutaka Shimizu, Zulfaqar Azmi, beyzanurkaya, danielsanchezaran, k-obitsu, kminoda, pre-commit-ci[bot], taikitanaka3

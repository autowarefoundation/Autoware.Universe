^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_autonomous_emergency_braking
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(autonomous_emergency_braking): fix no backward imu path and wrong back distance usage (`#9141 <https://github.com/autowarefoundation/autoware.universe/issues/9141>`_)
  * fix no backward imu path and wrong back distance usage
  * use the motion utils isDrivingForward function
  ---------
* refactor(autoware_autonomous_emergency_braking): rename info_marker_publisher to virtual_wall_publisher (`#9078 <https://github.com/autowarefoundation/autoware.universe/issues/9078>`_)
* feat(autonomous_emergency_braking): set max imu path length (`#9004 <https://github.com/autowarefoundation/autoware.universe/issues/9004>`_)
  * set a limit to the imu path length
  * fix test and add a new one
  * update readme
  * pre-commit
  * use velocity and time directly to get arc length
  * refactor to reduce repeated code
  * cleaning code
  ---------
* feat(autonomous_emergency_braking): add sanity chackes (`#8998 <https://github.com/autowarefoundation/autoware.universe/issues/8998>`_)
  add sanity chackes
* feat(autonomous_emergency_braking): calculate the object's velocity in the search area (`#8591 <https://github.com/autowarefoundation/autoware.universe/issues/8591>`_)
  * refactor PR
  * WIP
  * change using polygon to lateral offset
  * improve code
  * remove redundant code
  * skip close points in MPC path generation
  * fix empty path points in short parking scenario
  * fix readme conflicts
  ---------
* docs(autonomous_emergency_braking): add missing params to README (`#8950 <https://github.com/autowarefoundation/autoware.universe/issues/8950>`_)
  add missing params
* feat(autonomous_emergency_braking): make hull markers 3d (`#8930 <https://github.com/autowarefoundation/autoware.universe/issues/8930>`_)
  make hull markers 3d
* docs(autonomous_emergency_braking): make a clearer image for aeb when localization is faulty (`#8873 <https://github.com/autowarefoundation/autoware.universe/issues/8873>`_)
  make a clearer image for aeb when localization is faulty
* feat(autonomous_emergency_braking): add markers showing aeb convex hull polygons for debugging purposes (`#8865 <https://github.com/autowarefoundation/autoware.universe/issues/8865>`_)
  * add markers showing aeb convex hull polygons for debugging purposes
  * fix briefs
  * fix typo
  ---------
* fix(control): align the parameters with launcher (`#8789 <https://github.com/autowarefoundation/autoware.universe/issues/8789>`_)
  align the control parameters
* feat(autonomous_emergency_braking): speed up aeb (`#8778 <https://github.com/autowarefoundation/autoware.universe/issues/8778>`_)
  * add missing rclcpp::Time(0)
  * refactor to reduce cropping to once per iteration
  * add LookUpTransform to utils
  * separate object creation and clustering
  * error handling of empty pointcloud
  ---------
* feat(autonomous_emergency_braking): increase aeb speed by getting last transform (`#8734 <https://github.com/autowarefoundation/autoware.universe/issues/8734>`_)
  set stamp to 0 to get the latest stamp instead of waiting for the stamp
* feat(autonomous_emergency_braking): add timekeeper to AEB (`#8706 <https://github.com/autowarefoundation/autoware.universe/issues/8706>`_)
  * add timekeeper to AEB
  * add more info to output
  ---------
* docs(autoware_autonomous_emergency_braking): improve AEB module's README (`#8612 <https://github.com/autowarefoundation/autoware.universe/issues/8612>`_)
  * docs: improve AEB module's README
  * update rss distance length
  ---------
* fix(autonomous_emergency_braking): fix debug marker visual bug (`#8611 <https://github.com/autowarefoundation/autoware.universe/issues/8611>`_)
  fix bug by using the collision data keeper
* feat(autonomous_emergency_braking): enable aeb with only one req path (`#8569 <https://github.com/autowarefoundation/autoware.universe/issues/8569>`_)
  * make it so AEB works with only one req path type (imu or MPC)
  * fix missing mpc path return
  * add check
  * modify no path msg
  ---------
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
* docs(autonomous_emergency_braking): update readme for new param (`#8330 <https://github.com/autowarefoundation/autoware.universe/issues/8330>`_)
  update readme for new param
* feat(autonomous_emergency_braking): add info marker and override for state (`#8312 <https://github.com/autowarefoundation/autoware.universe/issues/8312>`_)
  add info marker and override for state
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
* feat(autonomous_emergency_braking): add virtual stop wall to aeb (`#7894 <https://github.com/autowarefoundation/autoware.universe/issues/7894>`_)
  * add virtual stop wall to aeb
  * add maintainer
  * add uppercase
  * use motion utils function instead of shiftPose
  ---------
* chore(autonomous_emergency_braking): apply clangd suggestions to aeb (`#7703 <https://github.com/autowarefoundation/autoware.universe/issues/7703>`_)
  * apply clangd suggestions
  * add maintainer
  ---------
* feat(autonomous_emergency_braking): aeb add support negative speeds (`#7707 <https://github.com/autowarefoundation/autoware.universe/issues/7707>`_)
  * add support for negative speeds
  * remove negative speed check for predicted obj
  ---------
* fix(autonomous_emergency_braking): aeb strange mpc polygon (`#7740 <https://github.com/autowarefoundation/autoware.universe/issues/7740>`_)
  change resize to reserve
* feat(autonomous_emergency_braking): add cluster min height for aeb (`#7605 <https://github.com/autowarefoundation/autoware.universe/issues/7605>`_)
  * add minimum cluster height threshold
  * add update param option
  * use param
  * avoid the float check if cluster_surpasses_threshold_height is already true
  * update README
  * add cluster height description
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
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
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(control)!: refactor directory structures of the control checkers (`#7524 <https://github.com/autowarefoundation/autoware.universe/issues/7524>`_)
  * aeb
  * control_validator
  * lane_departure_checker
  * shift_decider
  * fix
  ---------
* feat(autonomous_emergency_braking): aeb disable obj velocity calc w param (`#7493 <https://github.com/autowarefoundation/autoware.universe/issues/7493>`_)
  * feat(autonomous_emergenct_braking): update README and imgs of aeb (`#7482 <https://github.com/autowarefoundation/autoware.universe/issues/7482>`_)
  update README
  * add param to toggle on or off object speed calc for aeb
  * pre-commit readme
  ---------
* fix(planning): set single depth sensor data qos for pointlcoud polling subscribers (`#7490 <https://github.com/autowarefoundation/autoware.universe/issues/7490>`_)
  set single depth sensor data qos for pointlcoud polling subscribers
* feat(autonomous_emergenct_braking): update README and imgs of aeb (`#7482 <https://github.com/autowarefoundation/autoware.universe/issues/7482>`_)
  update README
* feat(autonomous_emergency_braking): aeb for backwards driving (`#7279 <https://github.com/autowarefoundation/autoware.universe/issues/7279>`_)
  * add support for backward path AEB
  * fix sign)
  * add abs and protect against nan
  * solve sign problem with relative speed
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
* feat(autonomous_emergency_braking): prefix package and namespace with autoware\_ (`#7294 <https://github.com/autowarefoundation/autoware.universe/issues/7294>`_)
  * change package name
  * add the prefix
  * change option
  * change back node name
  * eliminate some prefixes that are not required
  * fix node name
  ---------
* Contributors: Amadeusz Szymko, Ismet Atabay, Kosuke Takeuchi, Kyoichi Sugahara, Satoshi OTA, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Zhe Shen, danielsanchezaran, mkquda

0.26.0 (2024-04-03)
-------------------

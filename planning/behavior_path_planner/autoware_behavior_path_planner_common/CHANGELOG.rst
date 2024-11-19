^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_path_planner_common
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(traffic_light_utils): prevent accessing nullopt (`#9163 <https://github.com/autowarefoundation/autoware.universe/issues/9163>`_)
* fix(behavior_path_planner, behavior_velocity_planner): fix to not read invalid ID (`#9103 <https://github.com/autowarefoundation/autoware.universe/issues/9103>`_)
  * fix(behavior_path_planner, behavior_velocity_planner): fix to not read invalid ID
  * style(pre-commit): autofix
  * fix typo
  * fix(behavior_path_planner, behavior_velocity_planner): fix typo and indentation
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(autoware_test_utils): move test_map, add launcher for test_map (`#9045 <https://github.com/autowarefoundation/autoware.universe/issues/9045>`_)
* test(bpp_common): add test for path utils (`#9122 <https://github.com/autowarefoundation/autoware.universe/issues/9122>`_)
  * add test file for path utils
  * fix
  * add tests for map irrelevant function
  * add test for getUnshiftedEgoPose
  * add docstring and remove unneccesary function
  ---------
* feat(test_utils): add simple path with lane id generator (`#9113 <https://github.com/autowarefoundation/autoware.universe/issues/9113>`_)
  * add simple path with lane id generator
  * chnage to explicit template
  * fix
  * add static cast
  * remove header file
  ---------
* feat(lane_change): add unit test for normal lane change class (RT1-7970) (`#9090 <https://github.com/autowarefoundation/autoware.universe/issues/9090>`_)
  * RT1-7970 testing base class
  * additional test
  * Added update lanes
  * check path generation
  * check is lane change required
  * fix PRs comment
  ---------
* test(bpp_common): add test for objects filtering except for lanelet using functions (`#9049 <https://github.com/autowarefoundation/autoware.universe/issues/9049>`_)
  * add test for objects filtering except for lanelet using functions
  * remove unnecessary include file
  * add doxygen
  ---------
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* refactor(bpp_common, motion_utils): move path shifter util functions to autoware::motion_utils (`#9081 <https://github.com/autowarefoundation/autoware.universe/issues/9081>`_)
  * remove unused function
  * mover path shifter utils function to autoware motion utils
  * minor change in license header
  * fix warning message
  * remove header file
  ---------
* test(bpp_common): add test for occupancy grid based collision detector (`#9066 <https://github.com/autowarefoundation/autoware.universe/issues/9066>`_)
  * add test for occupancy grid based collision detector
  * remove unnnecessary header
  * fix
  * change map resolution and corresponding index
  ---------
* test(bpp_common): add test for parking departure utils (`#9055 <https://github.com/autowarefoundation/autoware.universe/issues/9055>`_)
  * add test for parking departure utils
  * fix
  * fix typo
  * use EXPECT_DOUBLE_EQ instead of EXPECT_NEAR
  ---------
* test(bpp_common): add test for object related functions (`#9062 <https://github.com/autowarefoundation/autoware.universe/issues/9062>`_)
  * add test for object related functions
  * use EXPECT_DOUBLE_EQ instead of EXPECT_NEAR
  * fix build error
  ---------
* test(bpp_common): add test for footprint (`#9056 <https://github.com/autowarefoundation/autoware.universe/issues/9056>`_)
  add test for footprint
* refactor(lane_change): refactor get_lane_change_lanes function (`#9044 <https://github.com/autowarefoundation/autoware.universe/issues/9044>`_)
  * refactor(lane_change): refactor get_lane_change_lanes function
  * Add doxygen comment for to_geom_msg_pose
  ---------
* fix(behavior_path_planner_common): swap boolean for filterObjectsByVelocity (`#9036 <https://github.com/autowarefoundation/autoware.universe/issues/9036>`_)
  fix filter object by velocity
* feat(autoware_behavior_path_planner_common): disable feature of turning off blinker at low velocity (`#9005 <https://github.com/autowarefoundation/autoware.universe/issues/9005>`_)
  * feat(turn_signal_decider): disable feature of turning off blinker at low velocity
  ---------
* refactor(bpp): simplify ExtendedPredictedObject and add new member variables (`#8889 <https://github.com/autowarefoundation/autoware.universe/issues/8889>`_)
  * simplify ExtendedPredictedObject and add new member variables
  * replace self polygon to initial polygon
  * comment
  * add comments to dist of ego
  ---------
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(behavior_path_planner): fix rtc state update logic (`#8899 <https://github.com/autowarefoundation/autoware.universe/issues/8899>`_)
  * fix function updateRTCStatus
  * fix pre-commit
  ---------
* test(autoware_behavior_path_planner_common): add tests for calcInterpolatedPoseWithVelocity (`#8270 <https://github.com/autowarefoundation/autoware.universe/issues/8270>`_)
  * test: add interpolated pose calculation function's test
  * disabled SpecialCases test
  ---------
* refactor(behavior_path_planner): planner data parameter initializer function (`#8767 <https://github.com/autowarefoundation/autoware.universe/issues/8767>`_)
* feat(behavior_planning): update test map for BusStopArea and bicycle_lanes (`#8694 <https://github.com/autowarefoundation/autoware.universe/issues/8694>`_)
* fix(autoware_behavior_path_planner_common): fix unusedFunction (`#8736 <https://github.com/autowarefoundation/autoware.universe/issues/8736>`_)
  fix:unusedFunction
* fix(autoware_behavior_path_planner_common): fix unusedFunction (`#8707 <https://github.com/autowarefoundation/autoware.universe/issues/8707>`_)
  * fix:createDrivableLanesMarkerArray and setDecelerationVelocity
  * fix:convertToSnakeCase
  * fix:clang format
  ---------
* fix(bpp): use common steering factor interface for same scene modules (`#8675 <https://github.com/autowarefoundation/autoware.universe/issues/8675>`_)
* fix(autoware_behavior_path_planner_common): fix unusedFunction (`#8654 <https://github.com/autowarefoundation/autoware.universe/issues/8654>`_)
  * fix:unusedFunction 0-2
  * fix:unusedFunction 3-5
  * fix:unusedFunction
  ---------
* chore(behavior_path_planner_common): update road_shoulder test_map (`#8550 <https://github.com/autowarefoundation/autoware.universe/issues/8550>`_)
* perf(goal_planner): faster path sorting and selection  (`#8457 <https://github.com/autowarefoundation/autoware.universe/issues/8457>`_)
  * perf(goal_planner): faster path sorting and selection
  * path_id_to_rough_margin_map
  ---------
* feat(behavior_path_planner_common): add calculateRoughDistanceToObjects (`#8464 <https://github.com/autowarefoundation/autoware.universe/issues/8464>`_)
* fix(autoware_behavior_path_planner_common): fix variableScope (`#8443 <https://github.com/autowarefoundation/autoware.universe/issues/8443>`_)
  fix:variableScope
* refactor(safety_checker): remove redundant polygon creation (`#8502 <https://github.com/autowarefoundation/autoware.universe/issues/8502>`_)
* feat(lane_change): ensure LC merging lane stop point is safe (`#8369 <https://github.com/autowarefoundation/autoware.universe/issues/8369>`_)
  * function to check for merging lane
  * function to compute distance from last fit width center line point to lane end
  * ensure lane width at LC stop point is larger than ego width
  * refactor function isMergingLane
  * improve implementation
  * apply logic only when current ego foot print is within lane
  * change implementation to use intersection points of buffered centerline and lane polygon
  * minor refactoring
  * overload function isEgoWithinOriginalLane to pass lane polygon directly
  ---------
* feat(behavior_path_planner_common): add road_shoulder test map (`#8454 <https://github.com/autowarefoundation/autoware.universe/issues/8454>`_)
* fix(turn_signal, lane_change, goal_planner): add optional to tackle lane change turn signal and pull over turn signal (`#8463 <https://github.com/autowarefoundation/autoware.universe/issues/8463>`_)
  * add optional to tackle LC turn signal and pull over turn signal
  * CPP file should not re-define default value; typo in copying from internal repos
  ---------
* perf(static_obstacle_avoidance): improve logic to reduce computational cost (`#8432 <https://github.com/autowarefoundation/autoware.universe/issues/8432>`_)
  * perf(safety_check): check within first
  * perf(static_obstacle_avoidance): remove duplicated process
  * perf(static_obstacle_avoidance): remove heavy process
  ---------
* fix(start/goal_planner): align geometric parall parking start pose with center line (`#8326 <https://github.com/autowarefoundation/autoware.universe/issues/8326>`_)
* feat(behavior_path _planner): divide planner manager modules into dependent slots (`#8117 <https://github.com/autowarefoundation/autoware.universe/issues/8117>`_)
* feat(path_safety_checker): add rough collision check (`#8193 <https://github.com/autowarefoundation/autoware.universe/issues/8193>`_)
  * feat(path_safety_checker): add rough collision check
  * Update planning/behavior_path_planner/autoware_behavior_path_planner_common/src/utils/path_safety_checker/safety_check.cpp
  ---------
* fix(autoware_behavior_path_planner_common): fix passedByValue (`#8209 <https://github.com/autowarefoundation/autoware.universe/issues/8209>`_)
  * fix:clang format
  * fix:passedByValue
  * fix:passedByValue
  ---------
* fix(behavior_path_planner_common): fix dynamic drivable area expansion with few input bound points (`#8136 <https://github.com/autowarefoundation/autoware.universe/issues/8136>`_)
* fix(bpp): fix approved request search  (`#8119 <https://github.com/autowarefoundation/autoware.universe/issues/8119>`_)
  fix existApprovedRequest condition
* fix(bpp, rtc_interface): fix state transition (`#7743 <https://github.com/autowarefoundation/autoware.universe/issues/7743>`_)
  * fix(rtc_interface): check rtc state
  * fix(bpp_interface): check rtc state
  * feat(rtc_interface): print
  ---------
* fix(autoware_behavior_path_planner_common): fix constParameterReference (`#8045 <https://github.com/autowarefoundation/autoware.universe/issues/8045>`_)
  fix:constParameterReference
* feat(autoware_behavior_path_planner_common,autoware_behavior_path_lane_change_module): add time_keeper to bpp (`#8004 <https://github.com/autowarefoundation/autoware.universe/issues/8004>`_)
  * feat(autoware_behavior_path_planner_common,autoware_behavior_path_lane_change_module): add time_keeper to bpp
  * update
  ---------
* fix(autoware_behavior_path_planner_common): fix shadowVariable (`#7965 <https://github.com/autowarefoundation/autoware.universe/issues/7965>`_)
  fix:shadowVariable
* feat(safety_check): filter safety check targe objects by yaw deviation between pose and lane (`#7828 <https://github.com/autowarefoundation/autoware.universe/issues/7828>`_)
  * fix(safety_check): filter by yaw deviation to check object belongs to lane
  * fix(static_obstacle_avoidance): check yaw only when the object is moving
  ---------
* fix(autoware_behavior_path_planner_common): fix knownConditionTrueFalse (`#7816 <https://github.com/autowarefoundation/autoware.universe/issues/7816>`_)
* feat(autoware_behavior_path_planner): remove max_module_size param (`#7764 <https://github.com/autowarefoundation/autoware.universe/issues/7764>`_)
  * feat(behavior_path_planner): remove max_module_size param
  The max_module_size param has been removed from the behavior_path_planner scene_module_manager.param.yaml file. This param was unnecessary and has been removed to simplify the configuration.
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat(start_planner): yaw threshold for rss check (`#7657 <https://github.com/autowarefoundation/autoware.universe/issues/7657>`_)
  * add param to customize yaw th
  * add param to other modules
  * docs
  * update READMEs with params
  * fix LC README
  * use normalized yaw diff
  ---------
* fix(autoware_behavior_path_planner_common): fix containerOutOfBounds warning (`#7675 <https://github.com/autowarefoundation/autoware.universe/issues/7675>`_)
  * fix(autoware_behavior_path_planner_common): fix containerOutOfBounds warning
  * fix type
  ---------
* fix(autoware_behavior_path_planner_common): fix shadowArgument warning in getDistanceToCrosswalk (`#7665 <https://github.com/autowarefoundation/autoware.universe/issues/7665>`_)
* fix(autoware_behavior_path_planner_common): fix shadowArgument warning (`#7623 <https://github.com/autowarefoundation/autoware.universe/issues/7623>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(autoware_behavior_path_planner_common): fix redundantContinue warning (`#7578 <https://github.com/autowarefoundation/autoware.universe/issues/7578>`_)
* fix(behavior_path_planner): fix redundantAssignment warning (`#7560 <https://github.com/autowarefoundation/autoware.universe/issues/7560>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(behavior_path_planner): fix redundantIfRemove warning (`#7544 <https://github.com/autowarefoundation/autoware.universe/issues/7544>`_)
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
* refactor(rtc_interface)!: rename to include/autoware/{package_name} (`#7531 <https://github.com/autowarefoundation/autoware.universe/issues/7531>`_)
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
* refactor(freespace_planner)!: rename to include/autoware/{package_name}  (`#7525 <https://github.com/autowarefoundation/autoware.universe/issues/7525>`_)
  refactor(freespace_planner)!: rename to include/autoware/{package_name}
  refactor(start_planner): make autoware include dir
  refactor(goal_planner): make autoware include dir
  sampling planner module
  fix sampling planner build
  dynamic_avoidance
  lc
  side shift
  autoware_behavior_path_static_obstacle_avoidance_module
  autoware_behavior_path_planner_common
  make behavior_path dir
  pre-commit
  fix pre-commit
  fix build
  autoware_freespace_planner
  freespace_planning_algorithms
* refactor(control)!: refactor directory structures of the control checkers (`#7524 <https://github.com/autowarefoundation/autoware.universe/issues/7524>`_)
  * aeb
  * control_validator
  * lane_departure_checker
  * shift_decider
  * fix
  ---------
* refactor(objects_of_interest_marker_interface): rename to include/autoware/{package_name} (`#7535 <https://github.com/autowarefoundation/autoware.universe/issues/7535>`_)
* refactor(behaivor_path_planner)!: rename to include/autoware/{package_name} (`#7522 <https://github.com/autowarefoundation/autoware.universe/issues/7522>`_)
  * refactor(behavior_path_planner)!: make autoware dir in include
  * refactor(start_planner): make autoware include dir
  * refactor(goal_planner): make autoware include dir
  * sampling planner module
  * fix sampling planner build
  * dynamic_avoidance
  * lc
  * side shift
  * autoware_behavior_path_static_obstacle_avoidance_module
  * autoware_behavior_path_planner_common
  * make behavior_path dir
  * pre-commit
  * fix pre-commit
  * fix build
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Go Sakayori, Koichi98, Kosuke Takeuchi, Kyoichi Sugahara, Mamoru Sobue, Maxime CLEMENT, Ryuta Kambe, Satoshi OTA, T-Kimura-MM, Takayuki Murooka, Yuki TAKAGI, Yukinari Hisaki, Yutaka Kondo, Yuxuan Liu, Zulfaqar Azmi, danielsanchezaran, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

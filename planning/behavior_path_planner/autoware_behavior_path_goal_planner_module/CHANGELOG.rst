^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_path_goal_planner_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* feat(behavior_path_planner): add detail text to virutal wall (`#9600 <https://github.com/autowarefoundation/autoware.universe/issues/9600>`_)
  * feat(behavior_path_planner): add detail text to virutal wall
  * goal is far
  * pull over start pose is far
  * fix lc build
  * fix build
  * Update planning/behavior_path_planner/autoware_behavior_path_goal_planner_module/src/goal_planner_module.cpp
  ---------
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* fix(goal_planner): fix isStopped judgement (`#9585 <https://github.com/autowarefoundation/autoware.universe/issues/9585>`_)
  * fix(goal_planner): fix isStopped judgement
  * fix typo
  ---------
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* feat(goal_planner): check opposite lane for lane departure_check (`#9460 <https://github.com/autowarefoundation/autoware.universe/issues/9460>`_)
  * feat(goal_planner): check opposite lane for lane departure_check
  * refactor getMostInnerLane
  ---------
* refactor(goal_planner): improve log message and change level (`#9562 <https://github.com/autowarefoundation/autoware.universe/issues/9562>`_)
  Co-authored-by: Yukinari Hisaki <42021302+yhisaki@users.noreply.github.com>
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* refactor(goal_planner): move PathDecisionController implementation to a different file (`#9523 <https://github.com/autowarefoundation/autoware.universe/issues/9523>`_)
  refactor(goal_planner): move decision_state implementation
* refactor(goal_planner): move unnecessary member functions (`#9522 <https://github.com/autowarefoundation/autoware.universe/issues/9522>`_)
* fix(autoware_freespace_planner, autoware_freespace_planning_algorithms): modify freespace planner to use node clock instead of system clock (`#9152 <https://github.com/autowarefoundation/autoware.universe/issues/9152>`_)
  * Modified the autoware_freespace_planner and autoware_freespace_planning_algorithms packages to use the node clock instead of rclcpp detached clock. This allows the module to make use of sim time. Previously during simulation the parking trajectory would have system time in trajectory header messages causing downstream issues like non-clearance of trajectory buffers in motion planning based on elapsed time.
  * style(pre-commit): autofix
  * Updated the freespace planner instantiation call in the path planning modules
  * style(pre-commit): autofix
  * Updated tests for the utility functions
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Steven Brills <sbrills@oshkoshcorp.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(goal_planner): fix multiple lane ids of shift pull over (`#9360 <https://github.com/autowarefoundation/autoware.universe/issues/9360>`_)
  fix vel
* fix(goal_planner): remove stop reason (`#9365 <https://github.com/autowarefoundation/autoware.universe/issues/9365>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(goal_planner): use departure_check_lane for path (`#9423 <https://github.com/autowarefoundation/autoware.universe/issues/9423>`_)
* refactor(goal_planner): rename shoulder_lane to pull_over_lane (`#9422 <https://github.com/autowarefoundation/autoware.universe/issues/9422>`_)
* feat(goal_planner): do not insert shift end pose on pull over lane to path (`#9361 <https://github.com/autowarefoundation/autoware.universe/issues/9361>`_)
* feat(goal_planner): remove unnecessary member from ThreadSafeData (`#9393 <https://github.com/autowarefoundation/autoware.universe/issues/9393>`_)
* feat(goal_planner): move goal_candidates from ThreadSafeData to GoalPlannerData (`#9292 <https://github.com/autowarefoundation/autoware.universe/issues/9292>`_)
* feat(goal_planner): output velocity factor (`#9348 <https://github.com/autowarefoundation/autoware.universe/issues/9348>`_)
* refactor(bpp): rework steering factor interface (`#9325 <https://github.com/autowarefoundation/autoware.universe/issues/9325>`_)
  * refactor(bpp): rework steering factor interface
  * refactor(soa): rework steering factor interface
  * refactor(AbLC): rework steering factor interface
  * refactor(doa): rework steering factor interface
  * refactor(lc): rework steering factor interface
  * refactor(gp): rework steering factor interface
  * refactor(sp): rework steering factor interface
  * refactor(sbp): rework steering factor interface
  * refactor(ss): rework steering factor interface
  ---------
* refactor(goal_planner): remove reference_goal_pose getter/setter (`#9270 <https://github.com/autowarefoundation/autoware.universe/issues/9270>`_)
* feat(goal_planner): safety check with only parking path (`#9293 <https://github.com/autowarefoundation/autoware.universe/issues/9293>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(goal_planner): sort candidate path only when num to avoid is different (`#9271 <https://github.com/autowarefoundation/autoware.universe/issues/9271>`_)
* fix(autoware_behavior_path_goal_planner_module): fix cppcheck unreadVariable (`#9192 <https://github.com/autowarefoundation/autoware.universe/issues/9192>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, M. Fatih Cırıt, Mamoru Sobue, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Yutaka Kondo, stevenbrills

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(goal_planner): remove reference_goal_pose getter/setter (`#9270 <https://github.com/autowarefoundation/autoware.universe/issues/9270>`_)
* feat(goal_planner): safety check with only parking path (`#9293 <https://github.com/autowarefoundation/autoware.universe/issues/9293>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(goal_planner): sort candidate path only when num to avoid is different (`#9271 <https://github.com/autowarefoundation/autoware.universe/issues/9271>`_)
* fix(autoware_behavior_path_goal_planner_module): fix cppcheck unreadVariable (`#9192 <https://github.com/autowarefoundation/autoware.universe/issues/9192>`_)
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Mamoru Sobue, Ryuta Kambe, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(goal_planner): align vehicle footprint heading parallel to parking side lane boundary (`#9159 <https://github.com/autowarefoundation/autoware.universe/issues/9159>`_)
* chore(goal_planner): compare sampled/filtered candidate paths on plot (`#9140 <https://github.com/autowarefoundation/autoware.universe/issues/9140>`_)
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* feat(goal_planner): use vehicle side edge to check isCrossingPossible for pull over execution (`#9102 <https://github.com/autowarefoundation/autoware.universe/issues/9102>`_)
* feat(autoware_test_utils): move test_map, add launcher for test_map (`#9045 <https://github.com/autowarefoundation/autoware.universe/issues/9045>`_)
* refactor(goal_planner): move last_previous_module_output_path out of ThreadSafeData (`#9075 <https://github.com/autowarefoundation/autoware.universe/issues/9075>`_)
* refactor(bpp_common, motion_utils): move path shifter util functions to autoware::motion_utils (`#9081 <https://github.com/autowarefoundation/autoware.universe/issues/9081>`_)
  * remove unused function
  * mover path shifter utils function to autoware motion utils
  * minor change in license header
  * fix warning message
  * remove header file
  ---------
* refactor(goal_planner): remove prev_data / last_path_idx_time from ThreadSafeData (`#9064 <https://github.com/autowarefoundation/autoware.universe/issues/9064>`_)
  refactor(goal_planner): remove prev_data and last_path_idx_update_time
* refactor(goal_planner): remove lane parking pull over path (`#9063 <https://github.com/autowarefoundation/autoware.universe/issues/9063>`_)
* refactor(goal_planner): remove modified_goal in ThreadDafeData (`#9010 <https://github.com/autowarefoundation/autoware.universe/issues/9010>`_)
* refactor(goal planner): hold modified_goal in PullOverPath ,copy modified goal once from background thread (`#9006 <https://github.com/autowarefoundation/autoware.universe/issues/9006>`_)
  refactor(goal_planner): save modified_goal_pose in PullOverPlannerBase
* fix(behavior_path_planner_common): swap boolean for filterObjectsByVelocity (`#9036 <https://github.com/autowarefoundation/autoware.universe/issues/9036>`_)
  fix filter object by velocity
* fix(goal_planner): fix parking_path curvature and DecidingState transition (`#9022 <https://github.com/autowarefoundation/autoware.universe/issues/9022>`_)
* refactor(goal_planner): use the PullOverPath, PullOverPathCandidates copied from ThreadData to reduce access (`#8994 <https://github.com/autowarefoundation/autoware.universe/issues/8994>`_)
* refactor(goal_planner): remove unused header and divide ThreadSafeData to another file (`#8990 <https://github.com/autowarefoundation/autoware.universe/issues/8990>`_)
* refactor(goal_planner): refactor PullOverPlannseBase to instantiate only valid path (`#8983 <https://github.com/autowarefoundation/autoware.universe/issues/8983>`_)
* fix(goal_planner): fix freespace planning chattering (`#8981 <https://github.com/autowarefoundation/autoware.universe/issues/8981>`_)
* feat(goal_planner): use neighboring lane of pull over lane to check goal footprint (`#8716 <https://github.com/autowarefoundation/autoware.universe/issues/8716>`_)
  move to utils and add tests
* refactor(goal_planner): remove unnecessary GoalPlannerData member (`#8920 <https://github.com/autowarefoundation/autoware.universe/issues/8920>`_)
* feat(goal_planner): move PathDecidingStatus to other controller class (`#8872 <https://github.com/autowarefoundation/autoware.universe/issues/8872>`_)
* chore(planning): consistent parameters with autoware_launch (`#8915 <https://github.com/autowarefoundation/autoware.universe/issues/8915>`_)
  * chore(planning): consistent parameters with autoware_launch
  * update
  * fix json schema
  ---------
* fix(goal_planner): fix typo (`#8910 <https://github.com/autowarefoundation/autoware.universe/issues/8910>`_)
* fix(autoware_behavior_path_goal_planner_module): fix unusedFunction (`#8786 <https://github.com/autowarefoundation/autoware.universe/issues/8786>`_)
  fix:unusedFunction
* refactor(goal_planner): reduce call to isSafePath (`#8812 <https://github.com/autowarefoundation/autoware.universe/issues/8812>`_)
* feat(goal_planner): execute goal planner if previous module path terminal is pull over neighboring lane (`#8715 <https://github.com/autowarefoundation/autoware.universe/issues/8715>`_)
* feat(goal_planner):  dense goal candidate sampling in BusStopArea (`#8795 <https://github.com/autowarefoundation/autoware.universe/issues/8795>`_)
* fix(autoware_behavior_path_planner): align the parameters with launcher (`#8790 <https://github.com/autowarefoundation/autoware.universe/issues/8790>`_)
  parameters in behavior_path_planner aligned
* feat(goal_planner): add getBusStopAreaPolygons (`#8794 <https://github.com/autowarefoundation/autoware.universe/issues/8794>`_)
* fix(autoware_behavior_path_goal_planner_module): fix unusedFunction (`#8775 <https://github.com/autowarefoundation/autoware.universe/issues/8775>`_)
  fix:unusedFunction
* feat(behavior_path_goal planner): add example plot for development (`#8772 <https://github.com/autowarefoundation/autoware.universe/issues/8772>`_)
* fix(goal_planner): fix time_keeper race (`#8780 <https://github.com/autowarefoundation/autoware.universe/issues/8780>`_)
* fix(goal_planner): fix object extraction area (`#8764 <https://github.com/autowarefoundation/autoware.universe/issues/8764>`_)
* fix(goal_planner): fix typo (`#8763 <https://github.com/autowarefoundation/autoware.universe/issues/8763>`_)
* feat(goal_planner): extend pull over lanes inward to extract objects (`#8714 <https://github.com/autowarefoundation/autoware.universe/issues/8714>`_)
  * feat(goal_planner): extend pull over lanes inward to extract objects
  * update from review
  * use optionale
  * rename lamda
  * return nullopt
  * Update planning/behavior_path_planner/autoware_behavior_path_goal_planner_module/src/util.cpp
  Co-authored-by: Mamoru Sobue <mamoru.sobue@tier4.jp>
  * pre-commit
  ---------
  Co-authored-by: Mamoru Sobue <mamoru.sobue@tier4.jp>
* refactor(goal_planner): initialize parameter with free function (`#8712 <https://github.com/autowarefoundation/autoware.universe/issues/8712>`_)
* fix(bpp): use common steering factor interface for same scene modules (`#8675 <https://github.com/autowarefoundation/autoware.universe/issues/8675>`_)
* refactor(goal_planner): remove unnecessary member from PreviousPullOverData (`#8698 <https://github.com/autowarefoundation/autoware.universe/issues/8698>`_)
* refactor(goal_planner): remove unnecessary member from pull_over_planner (`#8697 <https://github.com/autowarefoundation/autoware.universe/issues/8697>`_)
* refactor(goal_planner): move pull_over_planner directory (`#8696 <https://github.com/autowarefoundation/autoware.universe/issues/8696>`_)
* fix(goal_planner): fix zero velocity in middle of path (`#8563 <https://github.com/autowarefoundation/autoware.universe/issues/8563>`_)
  * fix(goal_planner): fix zero velocity in middle of path
  * add comment
  ---------
* fix(goal_planner): remove time keeper in non main thread (`#8610 <https://github.com/autowarefoundation/autoware.universe/issues/8610>`_)
* feat(freespace_planning_algorithms): implement option for backward search from goal to start (`#8091 <https://github.com/autowarefoundation/autoware.universe/issues/8091>`_)
  * refactor freespace planning algorithms
  * fix error
  * use vector instead of map for a-star node graph
  * remove unnecessary parameters
  * precompute average turning radius
  * add threshold for minimum distance between direction changes
  * apply curvature weight and change in curvature weight
  * store total cost instead of heuristic cost
  * fix reverse weight application
  * fix parameter description in README
  * implement edt map to store distance to nearest obstacle for each grid cell
  * use obstacle edt in collision check
  * add cost for distance to obstacle
  * fix formats
  * add missing include
  * refactor functions
  * add missing include
  * implement backward search option
  * precompute number of margin cells to reduce out of range vertices check necessity
  * add reset data function
  * remove unnecessary code
  * add member function set() to AstarNode struct
  * implement adaptive expansion distance
  * remove unnecessary code
  * interpolate nodes with large expansion distance
  * minor refactor
  * fix interpolation for backward search
  * ensure expansion distance is larger than grid cell diagonal
  * compute collision free distance to goal map
  * use obstacle edt when computing collision free distance map
  * minor refactor
  * fix expansion cost function
  * set distance map before setting start node
  * refactor detect collision function
  * use flag instead of enum
  * add missing variable initialization
  * remove declared but undefined function
  * refactor makePlan() function
  * remove bool return statement for void function
  * remove unnecessary checks
  * minor fix
  * refactor computeEDTMap function
  * remove unnecessary code
  * set min and max expansion distance after setting costmap
  * refactor detectCollision function
  * remove unused function
  * change default parameter values
  * add missing last waypoint
  * fix computeEDTMap function
  * rename parameter
  * use linear function for obstacle distance cost
  * fix rrtstar obstacle check
  * add public access function to get distance to nearest obstacle
  * remove redundant return statements
  * check goal pose validity before setting collision free distance map
  * declare variables as const where necessary
  * compare front and back lengths when setting min and max dimension
  * add docstring and citation for computeEDTMap function
  * transform pose to local frame in getDistanceToObstacle funcion
  * update freespace planner parameter schema
  * refactor setPath function
  * fix function setPath
  * minor refactor
  ---------
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* perf(goal_planner): faster path sorting and selection  (`#8457 <https://github.com/autowarefoundation/autoware.universe/issues/8457>`_)
  * perf(goal_planner): faster path sorting and selection
  * path_id_to_rough_margin_map
  ---------
* refactor(behavior_path_planner): apply clang-tidy check (`#7549 <https://github.com/autowarefoundation/autoware.universe/issues/7549>`_)
  * goal_planner
  * lane_change
  ---------
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* perf(goal_planner): reduce unnecessary recursive lock guard (`#8465 <https://github.com/autowarefoundation/autoware.universe/issues/8465>`_)
  * perf(goal_planner): reduce unnecessary recursive lock guard
  * make set_no_lock private
  ---------
* fix(turn_signal, lane_change, goal_planner): add optional to tackle lane change turn signal and pull over turn signal (`#8463 <https://github.com/autowarefoundation/autoware.universe/issues/8463>`_)
  * add optional to tackle LC turn signal and pull over turn signal
  * CPP file should not re-define default value; typo in copying from internal repos
  ---------
* fix(goal_planner): fix lane departure check not working correctly due to uninitialized variable (`#8449 <https://github.com/autowarefoundation/autoware.universe/issues/8449>`_)
* fix(autoware_behavior_path_goal_planner_module): fix unreadVariable (`#8365 <https://github.com/autowarefoundation/autoware.universe/issues/8365>`_)
  fix:unreadVariable
* feat(behavior_path _planner): divide planner manager modules into dependent slots (`#8117 <https://github.com/autowarefoundation/autoware.universe/issues/8117>`_)
* perf(goal_planner): reduce processing time  (`#8195 <https://github.com/autowarefoundation/autoware.universe/issues/8195>`_)
  * perf(goal_palnner): reduce processing time
  * add const& return
  * use copy getter
  * pre commit
  ---------
* fix(start/goal_planner): fix freespace planning error handling (`#8246 <https://github.com/autowarefoundation/autoware.universe/issues/8246>`_)
* feat(goal_planner): add time keeper (`#8194 <https://github.com/autowarefoundation/autoware.universe/issues/8194>`_)
  time keeper
* refactor(freespace_planning_algorithm): refactor and improve astar search (`#8068 <https://github.com/autowarefoundation/autoware.universe/issues/8068>`_)
  * refactor freespace planning algorithms
  * fix error
  * use vector instead of map for a-star node graph
  * remove unnecessary parameters
  * precompute average turning radius
  * add threshold for minimum distance between direction changes
  * apply curvature weight and change in curvature weight
  * store total cost instead of heuristic cost
  * fix reverse weight application
  * fix parameter description in README
  * fix formats
  * add missing include
  * refactor functions
  * precompute number of margin cells to reduce out of range vertices check necessity
  * add reset data function
  * add member function set() to AstarNode struct
  * remove unnecessary code
  * minor refactor
  * ensure expansion distance is larger than grid cell diagonal
  * compute collision free distance to goal map
  * minor refactor
  * fix expansion cost function
  * set distance map before setting start node
  * minor fix
  * remove unnecessary code
  * change default parameter values
  * rename parameter
  * fix rrtstar obstacle check
  * remove redundant return statements
  * check goal pose validity before setting collision free distance map
  * declare variables as const where necessary
  ---------
* fix(autoware_behavior_path_goal_planner_module): fix shadowVariable (`#7962 <https://github.com/autowarefoundation/autoware.universe/issues/7962>`_)
  fix:shadowVariable
* fix(start/goal_planner): fix addition of duplicate segments in calcBeforeShiftedArcLength (`#7902 <https://github.com/autowarefoundation/autoware.universe/issues/7902>`_)
  * fix(start/goal_planner): fix addition of duplicate segments in calcBeforeShiftedArcLength
  * Update trajectory.hpp
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
  * Update trajectory.hpp
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
  ---------
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
* docs(goal_planner): update parameter description (`#7889 <https://github.com/autowarefoundation/autoware.universe/issues/7889>`_)
  * docs(goal_planner): update parameter description
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(goal_planner): prioritize pull over path by curvature (`#7791 <https://github.com/autowarefoundation/autoware.universe/issues/7791>`_)
  * feat(goal_planner): prioritize pull over path by curvature
  fix
  * add comment
  * pre commit
  ---------
  Co-authored-by: Mamoru Sobue <mamoru.sobue@tier4.jp>
* feat(safety_check): filter safety check targe objects by yaw deviation between pose and lane (`#7828 <https://github.com/autowarefoundation/autoware.universe/issues/7828>`_)
  * fix(safety_check): filter by yaw deviation to check object belongs to lane
  * fix(static_obstacle_avoidance): check yaw only when the object is moving
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
* fix(autoware_behavior_path_goal_planner_module): fix lateral_offset related warnings (`#7624 <https://github.com/autowarefoundation/autoware.universe/issues/7624>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
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
* Contributors: Fumiya Watanabe, Go Sakayori, Keisuke Shima, Kosuke Takeuchi, Mamoru Sobue, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Yuxuan Liu, Zhe Shen, danielsanchezaran, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

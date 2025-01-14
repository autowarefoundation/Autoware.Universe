^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_freespace_planner
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
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
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(freespace_planner): add processing time pub (`#9332 <https://github.com/autowarefoundation/autoware.universe/issues/9332>`_)
* fix(freespace_planner): fix is near target check (`#9327 <https://github.com/autowarefoundation/autoware.universe/issues/9327>`_)
  * fix is_near_target_check
  * update unit test
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kazunori-Nakajima, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, mkquda, stevenbrills

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
* test(freespace_planner): make unit tests for member functions (`#9060 <https://github.com/autowarefoundation/autoware.universe/issues/9060>`_)
  * refactor freespace planner
  * implement unit tests for freespace planner util functions
  * add freespace planner utils namespace
  * add function is_near_target to freespace planner utils
  * unit test for is_near_target function
  * implement unit tests for FreespacePlanner member functions
  * add docstring for functions
  ---------
* test(freespace_planner): add unit tests for util functions (`#9059 <https://github.com/autowarefoundation/autoware.universe/issues/9059>`_)
  * refactor freespace planner
  * add function is_near_target to freespace planner utils
  * add freespace planner utils namespace
  * fix function call
  * implement unit tests for freespace planner util functions
  * add freespace planner utils namespace
  ---------
* refactor(freespace_planner): move functions to utils (`#9058 <https://github.com/autowarefoundation/autoware.universe/issues/9058>`_)
  * refactor freespace planner
  * add function is_near_target to freespace planner utils
  * add freespace planner utils namespace
  * fix function call
  ---------
* fix(other_planning_packages): align the parameters with launcher (`#8793 <https://github.com/autowarefoundation/autoware.universe/issues/8793>`_)
  * parameters in planning/others aligned
  * update json
  ---------
* fix(freespace_planner): fix free space planner spamming message (`#8614 <https://github.com/autowarefoundation/autoware.universe/issues/8614>`_)
  check data availability only when scenario is active
* feat(freespace_planning_algorithms): implement support for multiple goal candidates in A star planner (`#8092 <https://github.com/autowarefoundation/autoware.universe/issues/8092>`_)
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
  * make A-star search work with multiple goal candidates as input
  * fix is_back flag logic
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
  * enable both forward and backward search options for multiple goal candidates
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
  * compare node index with goal index in isGoal check
  * append shifted goal pose to waypoints for more accurate arrival
  * remove redundant return statements
  * check goal pose validity before setting collision free distance map
  * declare variables as const where necessary
  * initialize vectors using assign function
  * compare front and back lengths when setting min and max dimension
  * add docstring and citation for computeEDTMap function
  * fix shifted goal pose for backward search
  * transform pose to local frame in getDistanceToObstacle funcion
  * add cost for lateral distance near goal
  * compute distance to obstacle from ego frame instead of base
  * update freespace planner parameter schema
  * update freespace planner parameter schema
  * refactor setPath function
  * fix function setPath
  * declare bool var as constant
  * remove unnecessary includes
  * minor refactor
  ---------
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
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
* refactor(autoware_freespace_planner): rework parameters (`#8296 <https://github.com/autowarefoundation/autoware.universe/issues/8296>`_)
* feat(freespace_planning_algorithms): use distance to nearest obstacle to improve path planning (`#8089 <https://github.com/autowarefoundation/autoware.universe/issues/8089>`_)
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
  * precompute number of margin cells to reduce out of range vertices check necessity
  * add reset data function
  * add member function set() to AstarNode struct
  * implement adaptive expansion distance
  * remove unnecessary code
  * interpolate nodes with large expansion distance
  * minor refactor
  * ensure expansion distance is larger than grid cell diagonal
  * compute collision free distance to goal map
  * use obstacle edt when computing collision free distance map
  * minor refactor
  * fix expansion cost function
  * set distance map before setting start node
  * refactor detect collision function
  * add missing variable initialization
  * remove declared but undefined function
  * remove unnecessary checks
  * minor fix
  * refactor computeEDTMap function
  * remove unnecessary code
  * set min and max expansion distance after setting costmap
  * refactor detectCollision function
  * remove unused function
  * change default parameter values
  * fix computeEDTMap function
  * rename parameter
  * use linear function for obstacle distance cost
  * fix rrtstar obstacle check
  * remove redundant return statements
  * check goal pose validity before setting collision free distance map
  * declare variables as const where necessary
  * compare front and back lengths when setting min and max dimension
  * add docstring and citation for computeEDTMap function
  * suppress spell check
  ---------
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* fix(freespace_planner): disable randomly failing tests (`#8337 <https://github.com/autowarefoundation/autoware.universe/issues/8337>`_)
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
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
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
* refactor(test_utils): move to common folder (`#7158 <https://github.com/autowarefoundation/autoware.universe/issues/7158>`_)
  * Move autoware planning test manager to autoware namespace
  * fix package share directory for behavior path planner
  * renaming files and directory
  * rename variables that has planning_test_utils in its name.
  * use autoware namespace for test utils
  * move folder to common
  * update .pages file
  * fix test error
  * removed obstacle velocity limiter test artifact
  * remove namespace from planning validator, it has using keyword
  ---------
* refactor(route_handler): route handler add autoware prefix (`#7341 <https://github.com/autowarefoundation/autoware.universe/issues/7341>`_)
  * rename route handler package
  * update packages dependencies
  * update include guards
  * update includes
  * put in autoware namespace
  * fix formats
  * keep header and source file name as before
  ---------
* refactor(freespace_planner)!: add autoware prefix (`#7376 <https://github.com/autowarefoundation/autoware.universe/issues/7376>`_)
  refactor(freespace_planner)!: add autoawre prefix
* Contributors: Batuhan Beytekin, Kosuke Takeuchi, Maxime CLEMENT, Takayuki Murooka, Yutaka Kondo, Zhe Shen, Zulfaqar Azmi, mkquda

0.26.0 (2024-04-03)
-------------------

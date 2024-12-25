^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_freespace_planning_algorithms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor: correct spelling (`#9528 <https://github.com/autowarefoundation/autoware.universe/issues/9528>`_)
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
* fix(autoware_freespace_planning_algorithms): fix clang-diagnostic-unused-const-variable (`#9433 <https://github.com/autowarefoundation/autoware.universe/issues/9433>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_freespace_planning_algorithms): fix clang-diagnostic-ignored-optimization-argument (`#9418 <https://github.com/autowarefoundation/autoware.universe/issues/9418>`_)
  fix: clang-diagnostic-ignored-optimization-argument
* fix(autoware_freespace_planning_algorithms): fix clang-diagnostic-unused-private-field (`#9396 <https://github.com/autowarefoundation/autoware.universe/issues/9396>`_)
  * fix: clang-diagnostic-unused-private-field
  * fix: build error
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(/autoware_freespace_planning_algorithms): fix cppcheck unusedFunction (`#9274 <https://github.com/autowarefoundation/autoware.universe/issues/9274>`_)
* fix(autoware_freespace_planning_algorithms): fix bugprone-unused-raii (`#9230 <https://github.com/autowarefoundation/autoware.universe/issues/9230>`_)
  fix: bugprone-unused-raii
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo, kobayu858, stevenbrills

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
* fix(/autoware_freespace_planning_algorithms): fix cppcheck unusedFunction (`#9274 <https://github.com/autowarefoundation/autoware.universe/issues/9274>`_)
* fix(autoware_freespace_planning_algorithms): fix bugprone-unused-raii (`#9230 <https://github.com/autowarefoundation/autoware.universe/issues/9230>`_)
  fix: bugprone-unused-raii
* Contributors: Esteve Fernandez, Ryuta Kambe, Yutaka Kondo, kobayu858

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
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
* fix(autoware_freespace_planning_algorithms): fix variableScope (`#8431 <https://github.com/autowarefoundation/autoware.universe/issues/8431>`_)
  fix: variableScope
  Co-authored-by: kobayu858 <129580202+kobayu858@users.noreply.github.com>
* chore(autoware_freespace_planning_algorithms): add missing dependency (`#8494 <https://github.com/autowarefoundation/autoware.universe/issues/8494>`_)
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
* fix(autoware_freespace_planning_algorithms): fix unreadVariable (`#8360 <https://github.com/autowarefoundation/autoware.universe/issues/8360>`_)
  * fix:unreadVariable
  * fix:clang format
  ---------
* fix(autoware_freespace_planning_algorithms): fix functionConst (`#8281 <https://github.com/autowarefoundation/autoware.universe/issues/8281>`_)
  fix:functionConst
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
* fix(autoware_freespace_planning_algorithms): fix shadowVariable (`#7949 <https://github.com/autowarefoundation/autoware.universe/issues/7949>`_)
  * fix:shadowVariable
  * fix:shadowVariable
  * fix:shadowVariable
  ---------
* chore(freespace_planning_algorithm): modify A* script for standalone running (`#7070 <https://github.com/autowarefoundation/autoware.universe/issues/7070>`_)
  * modify astar for standalone running
  move clearNoe() from setMap to makePlan().
  * small modification
  * run pre-commit
  ---------
  Co-authored-by: Takumi Ito <takumi.ito@tier4.jp>
* feat(freespace_planning_algorithms): add is_back flag into the return of A* python wrapper (`#7831 <https://github.com/autowarefoundation/autoware.universe/issues/7831>`_)
  add is_back flag to the return of getWaypoints
  Co-authored-by: Takumi Ito <takumi.ito@tier4.jp>
* fix(autoware_freespace_planning_algorithms): fix syntaxError (`#7812 <https://github.com/autowarefoundation/autoware.universe/issues/7812>`_)
* fix(autoware_freespace_planning_algorithms): fix constStatement warning (`#7580 <https://github.com/autowarefoundation/autoware.universe/issues/7580>`_)
* fix(autoware_freespace_planning_algorithms): fix unusedScopedObject bug (`#7562 <https://github.com/autowarefoundation/autoware.universe/issues/7562>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
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
* refactor(freespace_planning_algorithms)!: add autoware prefix (`#7375 <https://github.com/autowarefoundation/autoware.universe/issues/7375>`_)
* Contributors: Kosuke Takeuchi, M. Fatih Cırıt, Nagi70, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, TakumIto, Yutaka Kondo, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

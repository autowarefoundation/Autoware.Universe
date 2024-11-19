^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_scenario_selector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): add processing time pub. (`#9065 <https://github.com/autowarefoundation/autoware.universe/issues/9065>`_)
  * feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): Add: processing_time_pub
  * fix: pre-commit
  * feat(costmap_generator): fix: No output when not Active.
  * fix: clang-format
  * Re: fix: clang-format
  ---------
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
* perf(costmap_generator, scenario_selector): faster getLinkedParkingLot (`#7930 <https://github.com/autowarefoundation/autoware.universe/issues/7930>`_)
* refactor(autoware_universe_utils): changed the API to be more intuitive and added documentation (`#7443 <https://github.com/autowarefoundation/autoware.universe/issues/7443>`_)
  * refactor(tier4_autoware_utils): Changed the API to be more intuitive and added documentation.
  * use raw shared ptr in PollingPolicy::NEWEST
  * update
  * fix
  * Update evaluator/autoware_control_evaluator/include/autoware/control_evaluator/control_evaluator_node.hpp
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
  ---------
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
* feat(scenario_selector): rename to include/autoware/{package_name} (`#7512 <https://github.com/autowarefoundation/autoware.universe/issues/7512>`_)
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
* feat(scenario_selector): use polling subscribers (`#7412 <https://github.com/autowarefoundation/autoware.universe/issues/7412>`_)
* refactor(route_handler): route handler add autoware prefix (`#7341 <https://github.com/autowarefoundation/autoware.universe/issues/7341>`_)
  * rename route handler package
  * update packages dependencies
  * update include guards
  * update includes
  * put in autoware namespace
  * fix formats
  * keep header and source file name as before
  ---------
* refactor(scenario_selector): prefix package and namespace with autoware\_ (`#7379 <https://github.com/autowarefoundation/autoware.universe/issues/7379>`_)
* Contributors: Kazunori-Nakajima, Kosuke Takeuchi, Maxime CLEMENT, Takayuki Murooka, Yukinari Hisaki, Yutaka Kondo, Zulfaqar Azmi, mkquda

0.26.0 (2024-04-03)
-------------------

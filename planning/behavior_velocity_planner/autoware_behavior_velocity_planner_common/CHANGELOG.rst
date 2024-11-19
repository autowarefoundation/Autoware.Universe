^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_velocity_planner_common
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(behavior_velocity_planner_common): fix findOffsetSegment (`#9130 <https://github.com/autowarefoundation/autoware.universe/issues/9130>`_)
* feat(autoware_test_utils): move test_map, add launcher for test_map (`#9045 <https://github.com/autowarefoundation/autoware.universe/issues/9045>`_)
* test(no_stopping_area): refactor and add tests (`#9009 <https://github.com/autowarefoundation/autoware.universe/issues/9009>`_)
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* fix(autoware_behavior_velocity_planner_common): add node clock, fix use sim time (`#8876 <https://github.com/autowarefoundation/autoware.universe/issues/8876>`_)
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(bvp): fix rtc state update logic (`#8884 <https://github.com/autowarefoundation/autoware.universe/issues/8884>`_)
  * fix(bvp): fix rtc state update logic
  * fix(intersection): fix unexpected rtc state initialization
  ---------
* feat(behavior_planning): update test map for BusStopArea and bicycle_lanes (`#8694 <https://github.com/autowarefoundation/autoware.universe/issues/8694>`_)
* feat(intersection): fix topological sort for complicated intersection (`#8520 <https://github.com/autowarefoundation/autoware.universe/issues/8520>`_)
  * for enclave occlusion detection lanelet
  * some refactorings and modify doxygen
  * fix ci
  ---------
  Co-authored-by: Y.Hisaki <yhisaki31@gmail.com>
* fix(autoware_behavior_velocity_planner_common): fix variableScope (`#8446 <https://github.com/autowarefoundation/autoware.universe/issues/8446>`_)
  fix:variableScope
* feat(intersection): add test map for intersection (`#8455 <https://github.com/autowarefoundation/autoware.universe/issues/8455>`_)
* perf(velocity_smoother): not resample debug_trajectories is not used (`#8030 <https://github.com/autowarefoundation/autoware.universe/issues/8030>`_)
  * not resample debug_trajectories if not published
  * update dependant packages
  ---------
* feat(autoware_behavior_velocity_planner_common,autoware_behavior_velocity_stop_line_module): add time_keeper to bvp (`#8070 <https://github.com/autowarefoundation/autoware.universe/issues/8070>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* fix(autoware_behavior_velocity_planner_common): remove lane_id check from arc_lane_util (`#7710 <https://github.com/autowarefoundation/autoware.universe/issues/7710>`_)
  * fix(arc_lane_util): remove lane_id check from arc_lane_util
  * modify test_arc_lane_util.cpp
  ---------
* refactor(behavior_velocity_intersection): apply clang-tidy check (`#7552 <https://github.com/autowarefoundation/autoware.universe/issues/7552>`_)
  intersection
* fix(autoware_behavior_velocity_planner_common): fix unusedScopedObject bug (`#7570 <https://github.com/autowarefoundation/autoware.universe/issues/7570>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
* refactor(rtc_interface)!: rename to include/autoware/{package_name} (`#7531 <https://github.com/autowarefoundation/autoware.universe/issues/7531>`_)
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
* refactor(objects_of_interest_marker_interface): rename to include/autoware/{package_name} (`#7535 <https://github.com/autowarefoundation/autoware.universe/issues/7535>`_)
* refactor(velocity_smoother): rename to include/autoware/{package_name} (`#7533 <https://github.com/autowarefoundation/autoware.universe/issues/7533>`_)
* chore(behavior_velocity_planner): move packages (`#7526 <https://github.com/autowarefoundation/autoware.universe/issues/7526>`_)
* Contributors: Dawid Moszyński, Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, Mamoru Sobue, Maxime CLEMENT, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yukinari Hisaki, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------

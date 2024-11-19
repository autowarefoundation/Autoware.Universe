^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_velocity_stop_line_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(behavior_velocity_planner): fix cppcheck warnings of virtualCallInConstructor (`#8376 <https://github.com/autowarefoundation/autoware.universe/issues/8376>`_)
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
* fix(autoware_behavior_velocity_stop_line_module): fix uninitMemberVar (`#8318 <https://github.com/autowarefoundation/autoware.universe/issues/8318>`_)
  fix:uninitMemberVar
* feat(autoware_behavior_velocity_planner_common,autoware_behavior_velocity_stop_line_module): add time_keeper to bvp (`#8070 <https://github.com/autowarefoundation/autoware.universe/issues/8070>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* fix(autoware_behavior_velocity_planner_common): remove lane_id check from arc_lane_util (`#7710 <https://github.com/autowarefoundation/autoware.universe/issues/7710>`_)
  * fix(arc_lane_util): remove lane_id check from arc_lane_util
  * modify test_arc_lane_util.cpp
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* chore(behavior_velocity_planner): move packages (`#7526 <https://github.com/autowarefoundation/autoware.universe/issues/7526>`_)
* Contributors: Fumiya Watanabe, Kosuke Takeuchi, Takayuki Murooka, Yukinari Hisaki, Yutaka Kondo, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------

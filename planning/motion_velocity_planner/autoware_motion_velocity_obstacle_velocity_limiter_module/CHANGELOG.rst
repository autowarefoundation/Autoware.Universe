^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_motion_velocity_obstacle_velocity_limiter_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_grid_map_utils): prefix folder structure with autoware/ (`#9170 <https://github.com/autowarefoundation/autoware.universe/issues/9170>`_)
* fix(obstacle_velocity_limiter): more stable virtual wall (`#8499 <https://github.com/autowarefoundation/autoware.universe/issues/8499>`_)
* feat(out_of_lane): redesign to improve accuracy and performance (`#8453 <https://github.com/autowarefoundation/autoware.universe/issues/8453>`_)
* chore(obstacle_velocity_limiter): add Alqudah Mohammad as codeowner (`#8516 <https://github.com/autowarefoundation/autoware.universe/issues/8516>`_)
* fix(autoware_motion_velocity_obstacle_velocity_limiter_module): fix functionStatic (`#8483 <https://github.com/autowarefoundation/autoware.universe/issues/8483>`_)
  fix:functionStatic
* fix(autoware_motion_velocity_obstacle_velocity_limiter_module): fix unreadVariable (`#8366 <https://github.com/autowarefoundation/autoware.universe/issues/8366>`_)
  * fix:unreadVariable
  * fix:unreadVariable
  ---------
* fix(autoware_motion_velocity_obstacle_velocity_limiter_module): fix uninitMemberVar (`#8314 <https://github.com/autowarefoundation/autoware.universe/issues/8314>`_)
  fix:funinitMemberVar
* fix(autoware_motion_velocity_obstacle_velocity_limiter_module): fix funcArgNamesDifferent (`#8025 <https://github.com/autowarefoundation/autoware.universe/issues/8025>`_)
  fix:funcArgNamesDifferent
* fix(autoware_motion_velocity_obstacle_velocity_limiter_module): fix shadowVariable (`#7977 <https://github.com/autowarefoundation/autoware.universe/issues/7977>`_)
  * fix:shadowVariable
  * fix:shadowVariable
  ---------
* perf(motion_velocity_planner): resample trajectory after vel smoothing (`#7732 <https://github.com/autowarefoundation/autoware.universe/issues/7732>`_)
  * perf(dynamic_obstacle_stop): create rtree with packing algorithm
  * Revert "perf(out_of_lane): downsample the trajectory to improve performance (`#7691 <https://github.com/autowarefoundation/autoware.universe/issues/7691>`_)"
  This reverts commit 8444a9eb29b32f500be3724dd5662013b9b81060.
  * perf(motion_velocity_planner): resample trajectory after vel smoothing
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* perf(out_of_lane): downsample the trajectory to improve performance (`#7691 <https://github.com/autowarefoundation/autoware.universe/issues/7691>`_)
* feat(motion_velocity_planner, lane_departure_checker): add processing time Float64 publishers (`#7683 <https://github.com/autowarefoundation/autoware.universe/issues/7683>`_)
* feat(motion_velocity_planner): publish processing times (`#7633 <https://github.com/autowarefoundation/autoware.universe/issues/7633>`_)
* fix(autoware_motion_velocity_obstacle_velocity_limiter_module): fix unreadVariable warning (`#7625 <https://github.com/autowarefoundation/autoware.universe/issues/7625>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(motion_velocity_planner): rename include directories (`#7523 <https://github.com/autowarefoundation/autoware.universe/issues/7523>`_)
* refactor(grid_map_utils): add autoware prefix and namespace (`#7487 <https://github.com/autowarefoundation/autoware.universe/issues/7487>`_)
* feat(obstacle_velocity_limiter): move to motion_velocity_planner (`#7439 <https://github.com/autowarefoundation/autoware.universe/issues/7439>`_)
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Maxime CLEMENT, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------

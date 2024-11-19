^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_motion_velocity_dynamic_obstacle_stop_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* test(dynamic_obstacle_stop): add tests and do some refactoring (`#8250 <https://github.com/autowarefoundation/autoware.universe/issues/8250>`_)
* fix(autoware_motion_velocity_dynamic_obstacle_stop_module): fix funcArgNamesDifferent (`#8024 <https://github.com/autowarefoundation/autoware.universe/issues/8024>`_)
  fix:funcArgNamesDifferent
* perf(dynamic_obstacle_stop): construct rtree nodes in place (`#7753 <https://github.com/autowarefoundation/autoware.universe/issues/7753>`_)
* perf(dynamic_obstacle_stop): create rtree with packing algorithm (`#7730 <https://github.com/autowarefoundation/autoware.universe/issues/7730>`_)
* feat(motion_velocity_planner, lane_departure_checker): add processing time Float64 publishers (`#7683 <https://github.com/autowarefoundation/autoware.universe/issues/7683>`_)
* feat(motion_velocity_planner): publish processing times (`#7633 <https://github.com/autowarefoundation/autoware.universe/issues/7633>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(motion_velocity_planner): rename include directories (`#7523 <https://github.com/autowarefoundation/autoware.universe/issues/7523>`_)
* refactor(dynamic_obstacle_stop): move to motion_velocity_planner (`#7460 <https://github.com/autowarefoundation/autoware.universe/issues/7460>`_)
* Contributors: Kosuke Takeuchi, Maxime CLEMENT, Takayuki Murooka, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------

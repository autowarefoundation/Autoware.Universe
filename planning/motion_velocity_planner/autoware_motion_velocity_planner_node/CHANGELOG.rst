^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_motion_velocity_planner_node
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* chore(motion_velocity_planner): add Alqudah Mohammad as maintainer (`#8877 <https://github.com/autowarefoundation/autoware.universe/issues/8877>`_)
* perf(motion_velocity_planner): fix heavy resampling and transform lookup (`#8839 <https://github.com/autowarefoundation/autoware.universe/issues/8839>`_)
* fix(obstacle_velocity_limiter): more stable virtual wall (`#8499 <https://github.com/autowarefoundation/autoware.universe/issues/8499>`_)
* feat(out_of_lane): redesign to improve accuracy and performance (`#8453 <https://github.com/autowarefoundation/autoware.universe/issues/8453>`_)
* feat(motion_velocity_planner,planning_evaluator): add  stop, slow_down diags (`#8503 <https://github.com/autowarefoundation/autoware.universe/issues/8503>`_)
  * tmp save.
  * publish diagnostics.
  * move clearDiagnostics func to head
  * change to snake_names.
  * remove a change of launch.xml
  * pre-commit run -a
  * publish diagnostics on node side.
  * move empty checking out of 'get_diagnostics'.
  * remove get_diagnostics; change reason str.
  * remove unused condition.
  * Update planning/motion_velocity_planner/autoware_motion_velocity_planner_node/src/planner_manager.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update planning/motion_velocity_planner/autoware_motion_velocity_planner_node/src/planner_manager.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* perf(velocity_smoother): not resample debug_trajectories is not used (`#8030 <https://github.com/autowarefoundation/autoware.universe/issues/8030>`_)
  * not resample debug_trajectories if not published
  * update dependant packages
  ---------
* feat(out_of_lane): ignore objects coming from behind ego (`#7891 <https://github.com/autowarefoundation/autoware.universe/issues/7891>`_)
* fix(motion_planning): fix processing time topic names (`#7885 <https://github.com/autowarefoundation/autoware.universe/issues/7885>`_)
* fix(motion_velocity_planner): use the slowdown velocity (instead of 0) (`#7840 <https://github.com/autowarefoundation/autoware.universe/issues/7840>`_)
* perf(motion_velocity_planner): resample trajectory after vel smoothing (`#7732 <https://github.com/autowarefoundation/autoware.universe/issues/7732>`_)
  * perf(dynamic_obstacle_stop): create rtree with packing algorithm
  * Revert "perf(out_of_lane): downsample the trajectory to improve performance (`#7691 <https://github.com/autowarefoundation/autoware.universe/issues/7691>`_)"
  This reverts commit 8444a9eb29b32f500be3724dd5662013b9b81060.
  * perf(motion_velocity_planner): resample trajectory after vel smoothing
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat(motion_velocity_planner, lane_departure_checker): add processing time Float64 publishers (`#7683 <https://github.com/autowarefoundation/autoware.universe/issues/7683>`_)
* feat(motion_velocity_planner): publish processing times (`#7633 <https://github.com/autowarefoundation/autoware.universe/issues/7633>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(velocity_smoother): rename to include/autoware/{package_name} (`#7533 <https://github.com/autowarefoundation/autoware.universe/issues/7533>`_)
* feat(motion_velocity_planner): rename include directories (`#7523 <https://github.com/autowarefoundation/autoware.universe/issues/7523>`_)
* fix(planning): set single depth sensor data qos for pointlcoud polling subscribers (`#7490 <https://github.com/autowarefoundation/autoware.universe/issues/7490>`_)
  set single depth sensor data qos for pointlcoud polling subscribers
* refactor(dynamic_obstacle_stop): move to motion_velocity_planner (`#7460 <https://github.com/autowarefoundation/autoware.universe/issues/7460>`_)
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
* feat(obstacle_velocity_limiter): move to motion_velocity_planner (`#7439 <https://github.com/autowarefoundation/autoware.universe/issues/7439>`_)
* feat(motion_velocity_planner): use polling subscriber to efficiently get messages (`#7223 <https://github.com/autowarefoundation/autoware.universe/issues/7223>`_)
  * feat(motion_velocity_planner): use polling subscriber for odometry topic
  * use polling subscribers for more topics
  * remove blocking mutex lock when processing traffic lights
  * fix assign after return
  ---------
* refactor(path_optimizer, velocity_smoother)!: prefix package and namespace with autoware (`#7354 <https://github.com/autowarefoundation/autoware.universe/issues/7354>`_)
  * chore(autoware_velocity_smoother): update namespace
  * chore(autoware_path_optimizer): update namespace
  ---------
* feat!: replace autoware_auto_msgs with autoware_msgs for planning modules (`#7246 <https://github.com/autowarefoundation/autoware.universe/issues/7246>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* chore(autoware_velocity_smoother, autoware_path_optimizer): rename packages (`#7202 <https://github.com/autowarefoundation/autoware.universe/issues/7202>`_)
  * chore(autoware_path_optimizer): rename package and namespace
  * chore(autoware_static_centerline_generator): rename package and namespace
  * chore: update module name
  * chore(autoware_velocity_smoother): rename package and namespace
  * chore(tier4_planning_launch): update module name
  * chore: update module name
  * fix: test
  * fix: test
  * fix: test
  ---------
* feat(motion_velocity_planner): add new motion velocity planning (`#7064 <https://github.com/autowarefoundation/autoware.universe/issues/7064>`_)
* Contributors: Fumiya Watanabe, Kosuke Takeuchi, Maxime CLEMENT, Ryohsuke Mitsudome, Satoshi OTA, Takayuki Murooka, Tiankui Xian, Yutaka Kondo, Zulfaqar Azmi, mkquda

0.26.0 (2024-04-03)
-------------------

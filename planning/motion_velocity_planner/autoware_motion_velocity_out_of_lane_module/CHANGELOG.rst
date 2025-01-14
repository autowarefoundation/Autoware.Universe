^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_motion_velocity_out_of_lane_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(traffic_light_utils): prefix package and namespace with autoware (`#9251 <https://github.com/autowarefoundation/autoware.universe/issues/9251>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(out_of_lane): correct calculations of the stop pose (`#9209 <https://github.com/autowarefoundation/autoware.universe/issues/9209>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Maxime CLEMENT, Ryohsuke Mitsudome, Yutaka Kondo

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
* fix(out_of_lane): correct calculations of the stop pose (`#9209 <https://github.com/autowarefoundation/autoware.universe/issues/9209>`_)
* Contributors: Esteve Fernandez, Maxime CLEMENT, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* perf(out_of_lane): use intersection with other lanes instead of difference with ego lane (`#8870 <https://github.com/autowarefoundation/autoware.universe/issues/8870>`_)
* chore(motion_velocity_planner): add Alqudah Mohammad as maintainer (`#8877 <https://github.com/autowarefoundation/autoware.universe/issues/8877>`_)
* chore(planning): consistent parameters with autoware_launch (`#8915 <https://github.com/autowarefoundation/autoware.universe/issues/8915>`_)
  * chore(planning): consistent parameters with autoware_launch
  * update
  * fix json schema
  ---------
* fix(motion_planning): align the parameters with launcher (`#8792 <https://github.com/autowarefoundation/autoware.universe/issues/8792>`_)
  parameters in motion_planning aligned
* docs(out_of_lane): update documentation for the new design (`#8692 <https://github.com/autowarefoundation/autoware.universe/issues/8692>`_)
* fix(out_of_lane): fix a bug with the rtree reference deleted nodes (`#8679 <https://github.com/autowarefoundation/autoware.universe/issues/8679>`_)
* fix(out_of_lane): fix noConstructor cppcheck warning (`#8636 <https://github.com/autowarefoundation/autoware.universe/issues/8636>`_)
* feat(out_of_lane): redesign to improve accuracy and performance (`#8453 <https://github.com/autowarefoundation/autoware.universe/issues/8453>`_)
* perf(out_of_lane): use rtree to get stop lines and trajectory lanelets (`#8439 <https://github.com/autowarefoundation/autoware.universe/issues/8439>`_)
* chore(out_of_lane): add Mamoru SOBUE as maintainer (`#8440 <https://github.com/autowarefoundation/autoware.universe/issues/8440>`_)
* feat(out_of_lane): also apply lat buffer between the lane and stop pose (`#7918 <https://github.com/autowarefoundation/autoware.universe/issues/7918>`_)
* feat(out_of_lane): ignore objects coming from behind ego (`#7891 <https://github.com/autowarefoundation/autoware.universe/issues/7891>`_)
* fix(autoware_motion_velocity_out_of_lane_module): fix constParameterReference (`#8051 <https://github.com/autowarefoundation/autoware.universe/issues/8051>`_)
  fix:constParameterReference
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
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
* feat(motion_velocity_planner): rename include directories (`#7523 <https://github.com/autowarefoundation/autoware.universe/issues/7523>`_)
* refactor(route_handler): route handler add autoware prefix (`#7341 <https://github.com/autowarefoundation/autoware.universe/issues/7341>`_)
  * rename route handler package
  * update packages dependencies
  * update include guards
  * update includes
  * put in autoware namespace
  * fix formats
  * keep header and source file name as before
  ---------
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
* feat(motion_velocity_planner): use polling subscriber to efficiently get messages (`#7223 <https://github.com/autowarefoundation/autoware.universe/issues/7223>`_)
  * feat(motion_velocity_planner): use polling subscriber for odometry topic
  * use polling subscribers for more topics
  * remove blocking mutex lock when processing traffic lights
  * fix assign after return
  ---------
* feat!: replace autoware_auto_msgs with autoware_msgs for planning modules (`#7246 <https://github.com/autowarefoundation/autoware.universe/issues/7246>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat(motion_velocity_planner): add new motion velocity planning (`#7064 <https://github.com/autowarefoundation/autoware.universe/issues/7064>`_)
* Contributors: Kosuke Takeuchi, Maxime CLEMENT, Ryohsuke Mitsudome, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, Zhe Shen, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

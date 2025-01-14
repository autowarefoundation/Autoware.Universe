^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package scenario_simulator_v2_adapter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix(cpplint): include what you use - evaluator (`#9566 <https://github.com/autowarefoundation/autoware.universe/issues/9566>`_)
* fix(scenario_simulator_v2_adapter): remove invalid CHANGELOG.rst file (`#9501 <https://github.com/autowarefoundation/autoware.universe/issues/9501>`_)
* 0.39.0
* fix version
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(tier4_metric_msgs): apply tier4_metric_msgs for scenario_simulator_v2_adapter, control_evaluator, planning_evaluator, autonomous_emergency_braking, obstacle_cruise_planner, motion_velocity_planner, processing_time_checker (`#9180 <https://github.com/autowarefoundation/autoware.universe/issues/9180>`_)
  * first commit
  * fix building errs.
  * change diagnostic messages to metric messages for publishing decision.
  * fix bug about motion_velocity_planner
  * change the diagnostic msg to metric msg in autoware_obstacle_cruise_planner.
  * tmp save for planning_evaluator
  * change the topic to which metrics published to.
  * fix typo.
  * remove unnesessary publishing of metrics.
  * mke planning_evaluator publish msg of MetricArray instead of Diags.
  * update aeb with metric type for decision.
  * fix some bug
  * remove autoware_evaluator_utils package.
  * remove diagnostic_msgs dependency of planning_evaluator
  * use metric_msgs for autoware_processing_time_checker.
  * rewrite diagnostic_convertor to scenario_simulator_v2_adapter, supporting metric_msgs.
  * pre-commit and fix typo
  * publish metrics even if there is no metric in the MetricArray.
  * modify the metric name of processing_time.
  * update unit test for test_planning/control_evaluator
  * manual pre-commit
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kem (TiankuiXian), M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix(cpplint): include what you use - evaluator (`#9566 <https://github.com/autowarefoundation/autoware.universe/issues/9566>`_)
* fix(scenario_simulator_v2_adapter): remove invalid CHANGELOG.rst file (`#9501 <https://github.com/autowarefoundation/autoware.universe/issues/9501>`_)
* 0.39.0
* fix version
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(tier4_metric_msgs): apply tier4_metric_msgs for scenario_simulator_v2_adapter, control_evaluator, planning_evaluator, autonomous_emergency_braking, obstacle_cruise_planner, motion_velocity_planner, processing_time_checker (`#9180 <https://github.com/autowarefoundation/autoware.universe/issues/9180>`_)
  * first commit
  * fix building errs.
  * change diagnostic messages to metric messages for publishing decision.
  * fix bug about motion_velocity_planner
  * change the diagnostic msg to metric msg in autoware_obstacle_cruise_planner.
  * tmp save for planning_evaluator
  * change the topic to which metrics published to.
  * fix typo.
  * remove unnesessary publishing of metrics.
  * mke planning_evaluator publish msg of MetricArray instead of Diags.
  * update aeb with metric type for decision.
  * fix some bug
  * remove autoware_evaluator_utils package.
  * remove diagnostic_msgs dependency of planning_evaluator
  * use metric_msgs for autoware_processing_time_checker.
  * rewrite diagnostic_convertor to scenario_simulator_v2_adapter, supporting metric_msgs.
  * pre-commit and fix typo
  * publish metrics even if there is no metric in the MetricArray.
  * modify the metric name of processing_time.
  * update unit test for test_planning/control_evaluator
  * manual pre-commit
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kem (TiankuiXian), M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* chore(package.xml): bump version to 0.39.0 (`#9435 <https://github.com/autowarefoundation/autoware.universe/issues/9435>`_)
  * chore: update CODEOWNERS (`#9203 <https://github.com/autowarefoundation/autoware.universe/issues/9203>`_)
  Co-authored-by: github-actions <github-actions@github.com>
  * refactor(time_utils): prefix package and namespace with autoware (`#9173 <https://github.com/autowarefoundation/autoware.universe/issues/9173>`_)
  * refactor(time_utils): prefix package and namespace with autoware
  * refactor(time_utils): prefix package and namespace with autoware
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(rtc_interface): add requested field (`#9202 <https://github.com/autowarefoundation/autoware.universe/issues/9202>`_)
  * add requested feature
  * Update planning/autoware_rtc_interface/test/test_rtc_interface.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * fix(mpc_lateral_controller): correctly resample the MPC trajectory yaws (`#9199 <https://github.com/autowarefoundation/autoware.universe/issues/9199>`_)
  * fix(bpp): prevent accessing nullopt (`#9204 <https://github.com/autowarefoundation/autoware.universe/issues/9204>`_)
  fix(bpp): calcDistanceToRedTrafficLight null
  * refactor(autoware_map_based_prediction): split pedestrian and bicycle predictor (`#9201 <https://github.com/autowarefoundation/autoware.universe/issues/9201>`_)
  * refactor: grouping functions
  * refactor: grouping parameters
  * refactor: rename member road_users_history to road_users_history\_
  * refactor: separate util functions
  * refactor: Add predictor_vru.cpp and utils.cpp to map_based_prediction_node
  * refactor: Add explicit template instantiation for removeOldObjectsHistory function
  * refactor: Add tf2_geometry_msgs to data_structure
  * refactor: Remove unused variables and functions in map_based_prediction_node.cpp
  * Update perception/autoware_map_based_prediction/include/map_based_prediction/predictor_vru.hpp
  * Apply suggestions from code review
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * refactor(ndt_scan_matcher, ndt_omp): move ndt_omp into ndt_scan_matcher (`#8912 <https://github.com/autowarefoundation/autoware.universe/issues/8912>`_)
  * Moved ndt_omp into ndt_scan_matcher
  * Added Copyright
  * style(pre-commit): autofix
  * Fixed include
  * Fixed cast style
  * Fixed include
  * Fixed honorific title
  * Fixed honorific title
  * style(pre-commit): autofix
  * Fixed include hierarchy
  * style(pre-commit): autofix
  * Fixed include hierarchy
  * style(pre-commit): autofix
  * Fixed hierarchy
  * Fixed NVTP to NVTL
  * Added cspell:ignore
  * Fixed miss spell
  * style(pre-commit): autofix
  * Fixed include
  * Renamed applyFilter
  * Moved ***_impl.hpp from include/ to src/
  * style(pre-commit): autofix
  * Fixed variable scope
  * Fixed to pass by reference
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(autoware_test_utils): add traffic light msgs parser (`#9177 <https://github.com/autowarefoundation/autoware.universe/issues/9177>`_)
  * fix(rtc_interface): update requested field for every cooperateStatus state (`#9211 <https://github.com/autowarefoundation/autoware.universe/issues/9211>`_)
  * fix rtc_interface
  * fix test condition
  ---------
  * feat(static_obstacle_avoidance): operator request for ambiguous vehicle (`#9205 <https://github.com/autowarefoundation/autoware.universe/issues/9205>`_)
  * add operator request feature
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/src/scene.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * feat(collision_detector): use polling subscriber (`#9213 <https://github.com/autowarefoundation/autoware.universe/issues/9213>`_)
  use polling subscriber
  * fix(diagnostic_graph_utils): reset graph when new one is received (`#9208 <https://github.com/autowarefoundation/autoware.universe/issues/9208>`_)
  fix(diagnostic_graph_utils): reset graph when new one is reveived
  * fix(autoware_ndt_scan_matcher): reduce initial_pose_estimation.particles_num from 200 to 100 on tests (`#9218 <https://github.com/autowarefoundation/autoware.universe/issues/9218>`_)
  Reduced initial_pose_estimation.particles_num from 200 to 100 on tests
  * feat(control_launch): add collision detector in launch (`#9214 <https://github.com/autowarefoundation/autoware.universe/issues/9214>`_)
  add collision detector in launch
  * chore(obstacle_cruise_planner): add function tests for a utils function (`#9206 <https://github.com/autowarefoundation/autoware.universe/issues/9206>`_)
  * add utils test
  ---------
  * fix(bvp): remove expired module safely (`#9212 <https://github.com/autowarefoundation/autoware.universe/issues/9212>`_)
  * fix(bvp): remove expired module safely
  * fix: remove module id set
  * fix: use itr to erase expired module
  * fix: remove unused function
  ---------
  * test(bpp_common): add unit test for safety check (`#9223 <https://github.com/autowarefoundation/autoware.universe/issues/9223>`_)
  * add test for object collision
  * add test for more functions
  * add docstring
  * fix lane change
  ---------
  * fix(autoware_behavior_path_goal_planner_module): fix cppcheck unreadVariable (`#9192 <https://github.com/autowarefoundation/autoware.universe/issues/9192>`_)
  * fix(autoware_image_projection_based_fusion): fix bugprone-misplaced-widening-cast (`#9229 <https://github.com/autowarefoundation/autoware.universe/issues/9229>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: clang-format
  ---------
  * fix(autoware_euclidean_cluster): fix bugprone-misplaced-widening-cast (`#9227 <https://github.com/autowarefoundation/autoware.universe/issues/9227>`_)
  fix: bugprone-misplaced-widening-cast
  * fix(autoware_image_projection_based_fusion): fix bugprone-misplaced-widening-cast (`#9226 <https://github.com/autowarefoundation/autoware.universe/issues/9226>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: clang-format
  ---------
  * fix(autoware_compare_map_segmentation): fix cppcheck constVariableReference (`#9196 <https://github.com/autowarefoundation/autoware.universe/issues/9196>`_)
  * refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
  * fix(autoware_behavior_velocity_no_stopping_area_module): fix cppcheck knownConditionTrueFalse (`#9189 <https://github.com/autowarefoundation/autoware.universe/issues/9189>`_)
  * fix(autoware_freespace_planning_algorithms): fix bugprone-unused-raii (`#9230 <https://github.com/autowarefoundation/autoware.universe/issues/9230>`_)
  fix: bugprone-unused-raii
  * refactor(map_based_prediction): divide objectsCallback (`#9219 <https://github.com/autowarefoundation/autoware.universe/issues/9219>`_)
  * refactor(map_based_prediction): move member functions to utils (`#9225 <https://github.com/autowarefoundation/autoware.universe/issues/9225>`_)
  * test(crosswalk): add unit test (`#9228 <https://github.com/autowarefoundation/autoware.universe/issues/9228>`_)
  * fix(autoware_probabilistic_occupancy_grid_map): fix bugprone-incorrect-roundings (`#9221 <https://github.com/autowarefoundation/autoware.universe/issues/9221>`_)
  fix: bugprone-incorrect-roundings
  * refactor(cuda_utils): prefix package and namespace with autoware (`#9171 <https://github.com/autowarefoundation/autoware.universe/issues/9171>`_)
  * fix(crosswalk): don't use vehicle stop checker to remove unnecessary callback (`#9234 <https://github.com/autowarefoundation/autoware.universe/issues/9234>`_)
  * feat(autoware_motion_utils): add new trajectory class (`#8693 <https://github.com/autowarefoundation/autoware.universe/issues/8693>`_)
  * feat(autoware_motion_utils): add interpolator
  * use int32_t instead of int
  * use int32_t instead of int
  * use int32_t instead of int
  * add const as much as possible and use `at()` in `vector`
  * fix directory name
  * refactor code and add example
  * update
  * remove unused include
  * refactor code
  * add clone function
  * fix stairstep
  * make constructor to public
  * feat(autoware_motion_utils): add trajectory class
  * Update CMakeLists.txt
  * fix
  * fix package.xml
  * update crop
  * revert crtp change
  * update package.xml
  * updating...
  * update
  * solve build problem
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(autoware_image_projection_based_fusion): make optional to consider lens distortion in the point projection (`#9233 <https://github.com/autowarefoundation/autoware.universe/issues/9233>`_)
  chore: add point_project_to_unrectified_image parameter to fusion_common.param.yaml
  * feat(autoware_test_utils): add general topic dumper (`#9207 <https://github.com/autowarefoundation/autoware.universe/issues/9207>`_)
  * fix(autoware_ekf_localizer): remove `timer_tf\_` (`#9244 <https://github.com/autowarefoundation/autoware.universe/issues/9244>`_)
  Removed timer_tf\_
  * fix(autoware_rtc_interface): fix dependency (`#9237 <https://github.com/autowarefoundation/autoware.universe/issues/9237>`_)
  * fix(autonomous_emergency_braking): solve issue with arc length (`#9247 <https://github.com/autowarefoundation/autoware.universe/issues/9247>`_)
  * solve issue with arc length
  * fix problem with points one vehicle apart from path
  ---------
  * fix(autoware_lidar_apollo_instance_segmentation): fix cppcheck suspiciousFloatingPointCast (`#9195 <https://github.com/autowarefoundation/autoware.universe/issues/9195>`_)
  * fix(autoware_behavior_path_sampling_planner_module): fix cppcheck unusedVariable (`#9190 <https://github.com/autowarefoundation/autoware.universe/issues/9190>`_)
  * refactor(qp_interface): prefix package and namespace with autoware (`#9236 <https://github.com/autowarefoundation/autoware.universe/issues/9236>`_)
  * chore(autoware_geography_utils): update maintainers (`#9246 <https://github.com/autowarefoundation/autoware.universe/issues/9246>`_)
  * update maintainers
  * add author
  ---------
  * fix(lane_change): enable cancel when ego in turn direction lane (`#9124 <https://github.com/autowarefoundation/autoware.universe/issues/9124>`_)
  * RT0-33893 add checks from prev intersection
  * fix shadow variable
  * fix logic
  * update readme
  * refactor get_ego_footprint
  ---------
  * fix(out_of_lane): correct calculations of the stop pose (`#9209 <https://github.com/autowarefoundation/autoware.universe/issues/9209>`_)
  * fix(autoware_pointcloud_preprocessor): launch file load parameter from yaml (`#8129 <https://github.com/autowarefoundation/autoware.universe/issues/8129>`_)
  * feat: fix launch file
  * chore: fix spell error
  * chore: fix parameters file name
  * chore: remove filter base
  ---------
  * fix: missing dependency in common components (`#9072 <https://github.com/autowarefoundation/autoware.universe/issues/9072>`_)
  * feat(autoware_trajectory): move trajectory_container from autoware_motion_utils to a new package (`#9253 <https://github.com/autowarefoundation/autoware.universe/issues/9253>`_)
  * create trajectory container package
  * update
  * update
  * style(pre-commit): autofix
  * update codeowner
  * update
  * fix cmake
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(autoware_pointcloud_preprocessor): fix the wrong naming of crop box parameter file  (`#9258 <https://github.com/autowarefoundation/autoware.universe/issues/9258>`_)
  fix: fix the wrong file name
  * fix(dummy_diag_publisher): not use diagnostic_updater and param callback (`#9257 <https://github.com/autowarefoundation/autoware.universe/issues/9257>`_)
  * fix(dummy_diag_publisher): not use diagnostic_updater and param callback for v0.29.0 (`#1414 <https://github.com/autowarefoundation/autoware.universe/issues/1414>`_)
  fix(dummy_diag_publisher): not use diagnostic_updater and param callback
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
  * fix: resolve build error of dummy diag publisher (`#1415 <https://github.com/autowarefoundation/autoware.universe/issues/1415>`_)
  fix merge conflict
  ---------
  Co-authored-by: Shohei Sakai <saka1s.jp@gmail.com>
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
  * test(behavior_path_planner_common): add unit test for path shifter (`#9239 <https://github.com/autowarefoundation/autoware.universe/issues/9239>`_)
  * add unit test for path shifter
  * fix unnecessary modification
  * fix spelling mistake
  * add docstring
  ---------
  * feat(system_monitor): support loopback network interface (`#9067 <https://github.com/autowarefoundation/autoware.universe/issues/9067>`_)
  * feat(system_monitor): support loopback network interface
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(autoware_trajectory): change interface of InterpolatedArray (`#9264 <https://github.com/autowarefoundation/autoware.universe/issues/9264>`_)
  change interface of InterpolateArray
  * feat(system_monitor): add on/off config for network traffic monitor (`#9069 <https://github.com/autowarefoundation/autoware.universe/issues/9069>`_)
  * feat(system_monitor): add config for network traffic monitor
  * fix: change function name from stop to skip
  ---------
  * feat(detection_area)!: add retruction feature (`#9255 <https://github.com/autowarefoundation/autoware.universe/issues/9255>`_)
  * fix(vehicle_cmd_gate): fix processing time measurement (`#9260 <https://github.com/autowarefoundation/autoware.universe/issues/9260>`_)
  * fix(bvp): use polling subscriber (`#9242 <https://github.com/autowarefoundation/autoware.universe/issues/9242>`_)
  * fix(bvp): use polling subscriber
  * fix: use newest policy
  ---------
  * refactor(lane_change): remove std::optional from lanes polygon (`#9267 <https://github.com/autowarefoundation/autoware.universe/issues/9267>`_)
  * fix(bpp): prevent accessing nullopt (`#9269 <https://github.com/autowarefoundation/autoware.universe/issues/9269>`_)
  * refactor(lane_change): revert "remove std::optional from lanes polygon" (`#9272 <https://github.com/autowarefoundation/autoware.universe/issues/9272>`_)
  Revert "refactor(lane_change): remove std::optional from lanes polygon (`#9267 <https://github.com/autowarefoundation/autoware.universe/issues/9267>`_)"
  This reverts commit 0c70ea8793985c6aae90f851eeffdd2561fe04b3.
  * feat(goal_planner): sort candidate path only when num to avoid is different (`#9271 <https://github.com/autowarefoundation/autoware.universe/issues/9271>`_)
  * fix(/autoware_freespace_planning_algorithms): fix cppcheck unusedFunction (`#9274 <https://github.com/autowarefoundation/autoware.universe/issues/9274>`_)
  * fix(autoware_behavior_path_start_planner_module): fix cppcheck unreadVariable (`#9277 <https://github.com/autowarefoundation/autoware.universe/issues/9277>`_)
  * fix(autoware_ndt_scan_matcher): fix cppcheck unusedFunction (`#9275 <https://github.com/autowarefoundation/autoware.universe/issues/9275>`_)
  * fix(autoware_pure_pursuit): fix cppcheck unusedFunction (`#9276 <https://github.com/autowarefoundation/autoware.universe/issues/9276>`_)
  * fix(lane_change): correct computation of maximum lane changing length threshold (`#9279 <https://github.com/autowarefoundation/autoware.universe/issues/9279>`_)
  fix computation of maximum lane changing length threshold
  * feat(aeb): set global param to override autoware state check (`#9263 <https://github.com/autowarefoundation/autoware.universe/issues/9263>`_)
  * set global param to override autoware state check
  * change variable to be more general
  * add comment
  * move param to control component launch
  * change param name to be more straightforward
  ---------
  * fix(autoware_default_adapi): change subscribing steering factor topic name for obstacle avoidance and lane changes (`#9273 <https://github.com/autowarefoundation/autoware.universe/issues/9273>`_)
  feat(planning): add new steering factor topics for obstacle avoidance and lane changes
  * chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
  * fix(lane_change): extending lane change path for multiple lane change (RT1-8427) (`#9268 <https://github.com/autowarefoundation/autoware.universe/issues/9268>`_)
  * RT1-8427 extending lc path for multiple lc
  * Update planning/behavior_path_planner/autoware_behavior_path_lane_change_module/src/scene.cpp
  Co-authored-by: mkquda <168697710+mkquda@users.noreply.github.com>
  ---------
  Co-authored-by: mkquda <168697710+mkquda@users.noreply.github.com>
  * fix(autoware_utils): address self-intersecting polygons in random_concave_generator and handle empty inners() during triangulation (`#8995 <https://github.com/autowarefoundation/autoware.universe/issues/8995>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * fix(behavior_path_planner_common): use boost intersects instead of overlaps (`#9289 <https://github.com/autowarefoundation/autoware.universe/issues/9289>`_)
  * fix(behavior_path_planner_common): use boost intersects instead of overlaps
  * Update planning/behavior_path_planner/autoware_behavior_path_planner_common/src/utils/path_safety_checker/safety_check.cpp
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  ---------
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  * ci(.github): update image tags (`#9286 <https://github.com/autowarefoundation/autoware.universe/issues/9286>`_)
  * refactor(autoware_ad_api_specs): prefix package and namespace with autoware (`#9250 <https://github.com/autowarefoundation/autoware.universe/issues/9250>`_)
  * refactor(autoware_ad_api_specs): prefix package and namespace with autoware
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api_specs to adapi_specs
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore(autoware_traffic_light*): add maintainer (`#9280 <https://github.com/autowarefoundation/autoware.universe/issues/9280>`_)
  * add fundamental commit
  * add forgot package
  ---------
  * fix(autoware_mpc_lateral_controller): fix bugprone-misplaced-widening-cast (`#9224 <https://github.com/autowarefoundation/autoware.universe/issues/9224>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: consider negative values
  ---------
  * fix(autoware_detected_object_validation): fix clang-diagnostic-error (`#9215 <https://github.com/autowarefoundation/autoware.universe/issues/9215>`_)
  fix: clang-c-error
  * fix(autoware_detected_object_validation): fix bugprone-incorrect-roundings (`#9220 <https://github.com/autowarefoundation/autoware.universe/issues/9220>`_)
  fix: bugprone-incorrect-roundings
  * feat(autoware_test_utils): use sample_vehicle/sample_sensor_kit (`#9290 <https://github.com/autowarefoundation/autoware.universe/issues/9290>`_)
  * refactor(lane_change): remove std::optional from lanes polygon (`#9288 <https://github.com/autowarefoundation/autoware.universe/issues/9288>`_)
  * feat(tier4_metric_msgs): apply tier4_metric_msgs for scenario_simulator_v2_adapter, control_evaluator, planning_evaluator, autonomous_emergency_braking, obstacle_cruise_planner, motion_velocity_planner, processing_time_checker (`#9180 <https://github.com/autowarefoundation/autoware.universe/issues/9180>`_)
  * first commit
  * fix building errs.
  * change diagnostic messages to metric messages for publishing decision.
  * fix bug about motion_velocity_planner
  * change the diagnostic msg to metric msg in autoware_obstacle_cruise_planner.
  * tmp save for planning_evaluator
  * change the topic to which metrics published to.
  * fix typo.
  * remove unnesessary publishing of metrics.
  * mke planning_evaluator publish msg of MetricArray instead of Diags.
  * update aeb with metric type for decision.
  * fix some bug
  * remove autoware_evaluator_utils package.
  * remove diagnostic_msgs dependency of planning_evaluator
  * use metric_msgs for autoware_processing_time_checker.
  * rewrite diagnostic_convertor to scenario_simulator_v2_adapter, supporting metric_msgs.
  * pre-commit and fix typo
  * publish metrics even if there is no metric in the MetricArray.
  * modify the metric name of processing_time.
  * update unit test for test_planning/control_evaluator
  * manual pre-commit
  ---------
  * feat(diagnostic_graph_aggregator): implement diagnostic graph dump functionality (`#9261 <https://github.com/autowarefoundation/autoware.universe/issues/9261>`_)
  * chore(tvm_utility): remove tvm_utility package as it is no longer used (`#9291 <https://github.com/autowarefoundation/autoware.universe/issues/9291>`_)
  * fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
  * perf(autoware_ndt_scan_matcher): remove evecs\_, evals\_ of Leaf for memory efficiency (`#9281 <https://github.com/autowarefoundation/autoware.universe/issues/9281>`_)
  * fix(lane_change): correct computation of maximum lane changing length threshold (`#9279 <https://github.com/autowarefoundation/autoware.universe/issues/9279>`_)
  fix computation of maximum lane changing length threshold
  * perf: remove evecs, evals from Leaf
  * perf: remove evecs, evals from Leaf
  ---------
  Co-authored-by: mkquda <168697710+mkquda@users.noreply.github.com>
  * test(costmap_generator): unit test implementation for costmap generator (`#9149 <https://github.com/autowarefoundation/autoware.universe/issues/9149>`_)
  * modify costmap generator directory structure
  * rename class CostmapGenerator to CostmapGeneratorNode
  * unit test for object_map_utils
  * catch error from lookupTransform
  * use polling subscriber in costmap generator node
  * add test for costmap generator node
  * add test for isActive()
  * revert unnecessary changes
  * remove commented out line
  * minor fix
  * Update planning/autoware_costmap_generator/src/costmap_generator.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * fix(control): missing dependency in control components (`#9073 <https://github.com/autowarefoundation/autoware.universe/issues/9073>`_)
  * test(autoware_control_evaluator): add unit test for utils autoware_control_evaluator (`#9307 <https://github.com/autowarefoundation/autoware.universe/issues/9307>`_)
  * update unit test of control_evaluator.
  * manual pre-commit.
  ---------
  * fix(collision_detector): skip process when odometry is not published (`#9308 <https://github.com/autowarefoundation/autoware.universe/issues/9308>`_)
  * subscribe odometry
  * fix precommit
  * remove unnecessary log info
  ---------
  * feat(goal_planner): safety check with only parking path (`#9293 <https://github.com/autowarefoundation/autoware.universe/issues/9293>`_)
  * refactor(goal_planner): remove reference_goal_pose getter/setter (`#9270 <https://github.com/autowarefoundation/autoware.universe/issues/9270>`_)
  * feat(start_planner, lane_departure_checker): speed up by updating polygons (`#9309 <https://github.com/autowarefoundation/autoware.universe/issues/9309>`_)
  speed up by updating polygons
  * fix(autoware_trajectory): fix bug of autoware_trajectory (`#9314 <https://github.com/autowarefoundation/autoware.universe/issues/9314>`_)
  * feat(autoware_trajectory): change default value of min_points (`#9315 <https://github.com/autowarefoundation/autoware.universe/issues/9315>`_)
  * chore(codecov): update maintained packages (`#9316 <https://github.com/autowarefoundation/autoware.universe/issues/9316>`_)
  * doc: fix links to design documents (`#9301 <https://github.com/autowarefoundation/autoware.universe/issues/9301>`_)
  * fix(costmap_generator): use vehicle frame for lidar height thresholds (`#9311 <https://github.com/autowarefoundation/autoware.universe/issues/9311>`_)
  * fix(tier4_dummy_object_rviz_plugin): fix missing dependency (`#9306 <https://github.com/autowarefoundation/autoware.universe/issues/9306>`_)
  * fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
  * add changelog
  * update changelog
  * fix version
  * 0.39.0
  * refactor(map_loader)!: prefix package and namespace with autoware (`#8927 <https://github.com/autowarefoundation/autoware.universe/issues/8927>`_)
  * make lanelet2_map_visualization independent
  * remove unused files
  * remove unused package
  * fix package name
  * add autoware\_ prefix
  * add autoware to exec name
  * add autoware prefix
  * removed unnecessary dependency
  ---------
  * update version
  ---------
  Co-authored-by: awf-autoware-bot[bot] <94889083+awf-autoware-bot[bot]@users.noreply.github.com>
  Co-authored-by: github-actions <github-actions@github.com>
  Co-authored-by: Esteve Fernandez <33620+esteve@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Yuki TAKAGI <141538661+yuki-takagi-66@users.noreply.github.com>
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
  Co-authored-by: kobayu858 <129580202+kobayu858@users.noreply.github.com>
  Co-authored-by: Yukinari Hisaki <42021302+yhisaki@users.noreply.github.com>
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
  Co-authored-by: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
  Co-authored-by: Yi-Hsiang Fang (Vivid) <146902905+vividf@users.noreply.github.com>
  Co-authored-by: ぐるぐる <f0reach@f0reach.me>
  Co-authored-by: Shohei Sakai <saka1s.jp@gmail.com>
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
  Co-authored-by: iwatake <take.iwiw2222@gmail.com>
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: mkquda <168697710+mkquda@users.noreply.github.com>
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
  Co-authored-by: Giovanni Muhammad Raditya <mraditya01@yahoo.com>
  Co-authored-by: Masato Saeki <78376491+MasatoSaeki@users.noreply.github.com>
  Co-authored-by: Kem (TiankuiXian) <1041084556@qq.com>
  Co-authored-by: Kento Osa <38522559+taisa1@users.noreply.github.com>
  Co-authored-by: Masaki Baba <maumaumaumaumaumaumaumaumaumau@gmail.com>
* Contributors: Yutaka Kondo

0.38.0 (2024-11-11)
-------------------

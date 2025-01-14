^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package topic_state_monitor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
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
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

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
* chore(topic_state_monitor): enrich error log message (`#7236 <https://github.com/autowarefoundation/autoware.universe/issues/7236>`_)
* Contributors: Takamasa Horibe, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* refactor(topic_state_monitor): add state log message (`#5378 <https://github.com/autowarefoundation/autoware.universe/issues/5378>`_)
  * refactor(topic_state_monitor): add state log message
  * add debug print
  ---------
* docs(topic_state_monitor): rename readme to README (`#4225 <https://github.com/autowarefoundation/autoware.universe/issues/4225>`_)
* chore: update maintainer (`#4140 <https://github.com/autowarefoundation/autoware.universe/issues/4140>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* chore: sync files (`#3227 <https://github.com/autowarefoundation/autoware.universe/issues/3227>`_)
  * chore: sync files
  * style(pre-commit): autofix
  ---------
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(topic_state_monitor): support transform topic check (`#1586 <https://github.com/autowarefoundation/autoware.universe/issues/1586>`_)
* refactor(topic_state_monitor): move the parameter group to match implementation (`#1909 <https://github.com/autowarefoundation/autoware.universe/issues/1909>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: replace legacy timer (`#329 <https://github.com/autowarefoundation/autoware.universe/issues/329>`_)
  * chore(goal_distance_calculator): replace legacy timer
  * chore(path_distance_calculator): replace legacy timer
  * chore(control_performance_analysis): replace legacy timer
  * chore(external_cmd_selector): replace legacy timer
  * chore(joy_controller): replace legacy timer
  * chore(lane_departure_checker): replace legacy timer
  * chore(obstacle_collision_checker): replace legacy timer
  * chore(pure_pursuit): replace legacy timer
  * chore(shift_decider): replace legacy timer
  * chore(trajectory_follower_nodes): replace legacy timer
  * chore(vehicle_cmd_gate): replace legacy timer
  * chore(ekf_localizer): replace legacy timer
  * chore(localization_error_monitor): replace legacy timer
  * chore(multi_object_tracker): replace legacy timer
  * chore(tensorrt_yolo): replace legacy timer
  * chore(traffic_light_classifier): replace legacy timer
  * chore(traffic_light_ssd_fine_detector): replace legacy timer
  * chore(traffic_light_visualization): replace legacy timer
  * chore(behavior_path_planner): replace legacy timer
  * chore(costmap_generator): replace legacy timer
  * chore(freespace_planner): replace legacy timer
  * chore(planning_error_monitor): replace legacy timer
  * chore(scenario_selector): replace legacy timer
  * chore(pointcloud_preprocessor): replace legacy timer
  * chore(dummy_perception_publisher): replace legacy timer
  * chore(ad_service_state_monitor): replace legacy timer
  * chore(dummy_diag_publisher): replace legacy timer
  * chore(emergency_handler): replace legacy timer
  * chore(system_error_monitor): replace legacy timer
  * chore(topic_state_monitor): replace legacy timer
  * chore(accel_brake_map_calibrator): replace legacy timer
  * chore(external_cmd_converter): replace legacy timer
  * chore(pacmod_interface): replace legacy timer
  * chore(lint): apply pre-commit
* feat: add topic_state_monitor package (`#15 <https://github.com/autowarefoundation/autoware.universe/issues/15>`_)
  * Ros2 v0.8.0 topic state monitor (`#283 <https://github.com/autowarefoundation/autoware.universe/issues/283>`_)
  * Add node_name_suffix to topic_state_monitor.launch (`#1157 <https://github.com/autowarefoundation/autoware.universe/issues/1157>`_) (`#370 <https://github.com/autowarefoundation/autoware.universe/issues/370>`_)
  * fix launch file (`#411 <https://github.com/autowarefoundation/autoware.universe/issues/411>`_)
  * add transient local option to topic state monitor (`#410 <https://github.com/autowarefoundation/autoware.universe/issues/410>`_)
  * add transient local option to topic state monitor
  * sort parameters
  * sort parameter
  * [topic_state_monitor]: Add best effort option (`#430 <https://github.com/autowarefoundation/autoware.universe/issues/430>`_)
  Co-authored-by: autoware <autoware@example.com>
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Fix for rolling (`#1226 <https://github.com/autowarefoundation/autoware.universe/issues/1226>`_)
  * Replace doc by description
  * Replace ns by push-ros-namespace
  * change to composable node (`#1233 <https://github.com/autowarefoundation/autoware.universe/issues/1233>`_)
  * Unify Apache-2.0 license name (`#1242 <https://github.com/autowarefoundation/autoware.universe/issues/1242>`_)
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Fix lint errors (`#1378 <https://github.com/autowarefoundation/autoware.universe/issues/1378>`_)
  * Fix lint errors
  * Fix variable names
  * Use integrated generic subscription (`#1342 <https://github.com/autowarefoundation/autoware.universe/issues/1342>`_)
  * suppress warnings for declare parameters (`#1724 <https://github.com/autowarefoundation/autoware.universe/issues/1724>`_)
  * fix for lanelet2_extension
  * fix for traffic light ssd fine detector
  * fix for topic_state_monitor
  * fix for dummy diag publisher
  * fix for remote cmd converter
  * fix for vehicle_info_util
  * fix for multi object tracker
  * fix for freespace planner
  * fix for autoware_error_monitor
  * add Werror for multi object tracker
  * fix for multi object tracker
  * add Werror for liraffic light ssd fine detector
  * add Werror for topic state monitor
  * add Werror
  * add Werror
  * add Werror
  * add Werror
  * fix style
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * Change formatter to clang-format and black (`#2332 <https://github.com/autowarefoundation/autoware.universe/issues/2332>`_)
  * Revert "Temporarily comment out pre-commit hooks"
  This reverts commit 748e9cdb145ce12f8b520bcbd97f5ff899fc28a3.
  * Replace ament_lint_common with autoware_lint_common
  * Remove ament_cmake_uncrustify and ament_clang_format
  * Apply Black
  * Apply clang-format
  * Fix build errors
  * Fix for cpplint
  * Fix include double quotes to angle brackets
  * Apply clang-format
  * Fix build errors
  * Add COLCON_IGNORE (`#500 <https://github.com/autowarefoundation/autoware.universe/issues/500>`_)
  * remove COLCON_IGNORE in system_packages and map_tf_generator (`#532 <https://github.com/autowarefoundation/autoware.universe/issues/532>`_)
  * [topic_state_monitor]add readme (`#565 <https://github.com/autowarefoundation/autoware.universe/issues/565>`_)
  * add readme
  * Update system/topic_state_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update system/topic_state_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update system/topic_state_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update system/topic_state_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update system/topic_state_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: autoware <autoware@example.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* Contributors: Daisuke Nishimatsu, Kenji Miyake, Takagi, Isamu, Takamasa Horibe, Tomoya Kimura, Vincent Richard, awf-autoware-bot[bot]

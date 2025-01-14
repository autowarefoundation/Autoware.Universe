^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package dummy_diag_publisher
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - system (`#9573 <https://github.com/autowarefoundation/autoware.universe/issues/9573>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(dummy_diag_publisher): not use diagnostic_updater and param callback (`#9257 <https://github.com/autowarefoundation/autoware.universe/issues/9257>`_)
  * fix(dummy_diag_publisher): not use diagnostic_updater and param callback for v0.29.0 (`#1414 <https://github.com/autowarefoundation/autoware.universe/issues/1414>`_)
  fix(dummy_diag_publisher): not use diagnostic_updater and param callback
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
  * fix: resolve build error of dummy diag publisher (`#1415 <https://github.com/autowarefoundation/autoware.universe/issues/1415>`_)
  fix merge conflict
  ---------
  Co-authored-by: Shohei Sakai <saka1s.jp@gmail.com>
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yuki TAKAGI, Yutaka Kondo

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
* fix(dummy_diag_publisher): not use diagnostic_updater and param callback (`#9257 <https://github.com/autowarefoundation/autoware.universe/issues/9257>`_)
  * fix(dummy_diag_publisher): not use diagnostic_updater and param callback for v0.29.0 (`#1414 <https://github.com/autowarefoundation/autoware.universe/issues/1414>`_)
  fix(dummy_diag_publisher): not use diagnostic_updater and param callback
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
  * fix: resolve build error of dummy diag publisher (`#1415 <https://github.com/autowarefoundation/autoware.universe/issues/1415>`_)
  fix merge conflict
  ---------
  Co-authored-by: Shohei Sakai <saka1s.jp@gmail.com>
  Co-authored-by: h-ohta <hiroki.ota@tier4.jp>
* Contributors: Esteve Fernandez, Yuki TAKAGI, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(dummy_diag_publisher): componentize node (`#7190 <https://github.com/autowarefoundation/autoware.universe/issues/7190>`_)
* Contributors: Kosuke Takeuchi, Takagi, Isamu, Takayuki Murooka, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* chore: update maintainer (`#5730 <https://github.com/autowarefoundation/autoware.universe/issues/5730>`_)
  update maintainer
* chore: update maintainer (`#4140 <https://github.com/autowarefoundation/autoware.universe/issues/4140>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* style: fix typos (`#3617 <https://github.com/autowarefoundation/autoware.universe/issues/3617>`_)
  * style: fix typos in documents
  * style: fix typos in package.xml
  * style: fix typos in launch files
  * style: fix typos in comments
  ---------
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
* feat(dummy diag publisher): change diag name specification method to YAML (`#2840 <https://github.com/autowarefoundation/autoware.universe/issues/2840>`_)
  * Signed-off-by: asana17 <akihiro.sakurai@tier4.jp>
  modified dummy_diag_publisher to use YAML for param
  * Signed-off-by: asana17 <akihiro.sakurai@tier4.jp>
  use YAML param for dummy_diag_publisher
  * fix empty param
  * fixed empty param
  * fix spelling
  * add pkg maintainer
  * launch dummy_diag_publisher by launch_dummy_diag_publisher param
  ---------
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* fix(dummy_diag_publisher): modify build error in rolling (`#760 <https://github.com/autowarefoundation/autoware.universe/issues/760>`_)
* feat(dummy_diag_publisher): use as a component (`#652 <https://github.com/autowarefoundation/autoware.universe/issues/652>`_)
  * feat(dummy_diag_publisher): use as components
  * fix: add explicit
  * fix: fix node name
* fix(dummy_diag_publisher): use anon to make unique node name instead of diag name (`#639 <https://github.com/autowarefoundation/autoware.universe/issues/639>`_)
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(dummy_diag_publisher): update README and launch file (`#536 <https://github.com/autowarefoundation/autoware.universe/issues/536>`_)
  * chore: update README
  * feat: add param in launch
  * chore: Update system/dummy_diag_publisher/README.md
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * chore: update readme
  * ci(pre-commit): autofix
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(dummy_diag_publisher): add exec_depend (`#523 <https://github.com/autowarefoundation/autoware.universe/issues/523>`_)
  * chore(dummy_diag_publisher): add exec_depend
  * Update system/dummy_diag_publisher/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
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
* feat: rename existing packages name starting with autoware to different names (`#180 <https://github.com/autowarefoundation/autoware.universe/issues/180>`_)
  * autoware_api_utils -> tier4_api_utils
  * autoware_debug_tools -> tier4_debug_tools
  * autoware_error_monitor -> system_error_monitor
  * autoware_utils -> tier4_autoware_utils
  * autoware_global_parameter_loader -> global_parameter_loader
  * autoware_iv_auto_msgs_converter -> tier4_auto_msgs_converter
  * autoware_joy_controller -> joy_controller
  * autoware_error_monitor -> system_error_monitor(launch)
  * autoware_state_monitor -> ad_service_state_monitor
  * autoware_web_controller -> web_controller
  * remove autoware_version
  * remove autoware_rosbag_recorder
  * autoware\_*_rviz_plugin -> tier4\_*_rviz_plugin
  * fix ad_service_state_monitor
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add dummy_diag_publisher package (`#18 <https://github.com/autowarefoundation/autoware.universe/issues/18>`_)
  * release v0.4.0
  * remove ROS1 packages temporarily
  * Revert "remove ROS1 packages temporarily"
  This reverts commit 6ab6bcca1dea5065fcb06aeec107538dad1f62af.
  * add COLCON_IGNORE to ros1 packages
  * Rename launch files to launch.xml (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
  * ROS2 Porting: dummy_diag_publisher (`#69 <https://github.com/autowarefoundation/autoware.universe/issues/69>`_)
  * Fix CMake, package.xml and remove COLCON_IGNORE
  * First pass
  - Remove ROS references: dynamic_configuration
  - Can compile
  * Fix references to dynamic_reconfigure
  - Clean up comments in cmake and package.xml
  - Add timer callback
  * Modify config yamls and remove dynamic reconfigure file
  - Fix launch files
  * Fix declaration of parameters using get parameters
  - Add rqt_reconfigure to package deps
  * Add comment in launch file
  * Remove fmt dependency
  * Address PR comment:
  - Ensure that status value is initialised properly
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  * Address PR comment:
  - Ensure config are declared to ensure it can be set in the parameter callback
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  * Address PR comment:
  - Remove headers from executable generation in cmake
  * Address PR comment:
  - Remove headers specification
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  * Rename h files to hpp (`#142 <https://github.com/autowarefoundation/autoware.universe/issues/142>`_)
  * Change includes
  * Rename files
  * Adjustments to make things compile
  * Other packages
  * Adjust copyright notice on 532 out of 699 source files (`#143 <https://github.com/autowarefoundation/autoware.universe/issues/143>`_)
  * Use quotes for includes where appropriate (`#144 <https://github.com/autowarefoundation/autoware.universe/issues/144>`_)
  * Use quotes for includes where appropriate
  * Fix lint tests
  * Make tests pass hopefully
  * Run uncrustify on the entire Pilot.Auto codebase (`#151 <https://github.com/autowarefoundation/autoware.universe/issues/151>`_)
  * Run uncrustify on the entire Pilot.Auto codebase
  * Exclude open PRs
  * Add linters (`#208 <https://github.com/autowarefoundation/autoware.universe/issues/208>`_)
  * Rename ROS-related .yaml to .param.yaml (`#352 <https://github.com/autowarefoundation/autoware.universe/issues/352>`_)
  * Rename ROS-related .yaml to .param.yaml
  * Remove prefix 'default\_' of yaml files
  * Rename vehicle_info.yaml to vehicle_info.param.yaml
  * Rename diagnostic_aggregator's param files
  * Fix overlooked parameters
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Fix for rolling (`#1226 <https://github.com/autowarefoundation/autoware.universe/issues/1226>`_)
  * Replace doc by description
  * Replace ns by push-ros-namespace
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Cleanup dummy_diag_publisher (`#1392 <https://github.com/autowarefoundation/autoware.universe/issues/1392>`_)
  * Cleanup dummy_diag_publisher
  * Fix typo
  * Make double and write comment
  * Set hardware_id from diag_name
  * Add const to daig_name and hardware_id
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
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
  * Fix typo `obstacle_crush` to `obstacle_crash` (`#2031 <https://github.com/autowarefoundation/autoware.universe/issues/2031>`_)
  * add sort-package-xml hook in pre-commit (`#1881 <https://github.com/autowarefoundation/autoware.universe/issues/1881>`_)
  * add sort xml hook in pre-commit
  * change retval to exit_status
  * rename
  * add prettier plugin-xml
  * use early return
  * add license note
  * add tier4 license
  * restore prettier
  * change license order
  * move local hooks to public repo
  * move prettier-xml to pre-commit-hooks-ros
  * update version for bug-fix
  * apply pre-commit
  * Refactor dummy_diag_publisher (`#2151 <https://github.com/autowarefoundation/autoware.universe/issues/2151>`_)
  * Refactor dummy_diag_publisher
  * fix depend order
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
  * remove COLCON_IGNORE in dummy_diag_publisher (`#528 <https://github.com/autowarefoundation/autoware.universe/issues/528>`_)
  * add README in dummy diag publisher (`#627 <https://github.com/autowarefoundation/autoware.universe/issues/627>`_)
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: Jilada Eccleston <jilada.eccleston@gmail.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* Contributors: Akihiro Sakurai, Daisuke Nishimatsu, Hiroki OTA, Keisuke Shima, Kenji Miyake, Tomoya Kimura, Vincent Richard, asana17, awf-autoware-bot[bot]

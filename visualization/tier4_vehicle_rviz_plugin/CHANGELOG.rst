^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_vehicle_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore: move rviz plugins from common to visualization/ folder (`#9417 <https://github.com/autowarefoundation/autoware.universe/issues/9417>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe

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
* fix: missing dependency in common components (`#9072 <https://github.com/autowarefoundation/autoware.universe/issues/9072>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo, ぐるぐる

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(tier4_vehicle_rviz_plugin): fix cppcheck warning of virtualCallInConstructor (`#8379 <https://github.com/autowarefoundation/autoware.universe/issues/8379>`_)
  fix: deal with virtualCallInConstructor warning
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
* fix: replace Ogre deprecated header (`#7606 <https://github.com/autowarefoundation/autoware.universe/issues/7606>`_)
  Fix Ogre deprecated header
  Co-authored-by: Kotaro Yoshimoto <pythagora.yoshimoto@gmail.com>
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Kosuke Takeuchi, Ryohsuke Mitsudome, Takayuki Murooka, Yutaka Kondo, taisa1, ぐるぐる

0.26.0 (2024-04-03)
-------------------
* refactor(common): extern template for motion_utils / remove tier4_autoware_utils.hpp / remove motion_utis.hpp (`#5027 <https://github.com/autowarefoundation/autoware.universe/issues/5027>`_)
* chore(build): remove tier4_autoware_utils.hpp in common/ (`#4828 <https://github.com/autowarefoundation/autoware.universe/issues/4828>`_)
  removed tier4_autoware_utils.hpp in common/
* chore: do not display steer and velocity value when message is not subscribed yet (`#4739 <https://github.com/autowarefoundation/autoware.universe/issues/4739>`_)
  * chore: do not display steer and velocity value when message is not subscribed yet
  * chore: change msgs
  ---------
* feat(tier4_vehicle_rviz_plugin): add acceleration visualization plugin to RViz (`#4506 <https://github.com/autowarefoundation/autoware.universe/issues/4506>`_)
  * feat: add acceleration visualization plugin to RVIZ
  * feat: add RVIZ plugin for acceleration; remove limit text; debugging: property_label_scale\_ not responding
  * style(pre-commit): autofix
  * fix typo in acceleration
  * fix a bug in keeping using abs(accel) to compute meter angle; delete text of acceleration meter, and delte the parameter of property_label_value
  * feat: separate the setting of max/min emergency threshold; update max/min acceration; set threshold for reconfiguring emergency speed
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Owen-Liuyuxuan <uken.ryu@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* fix(tier4_planning/vehicle_rviz_plugin): fixed license (`#2059 <https://github.com/autowarefoundation/autoware.universe/issues/2059>`_)
  * fix(tier4_planning/vehicle_rviz_plugin): fixed license
  * fix build error
* fix: remove unused check of rviz plugin version (`#1474 <https://github.com/autowarefoundation/autoware.universe/issues/1474>`_)
* fix(tier4_vehicle_rviz_plugin): initialization vehicle rivz plugin (`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_)
  * fix(tier4_vehicle_rviz_plugin): initialization vehicle rviz plugin
  * initialize signal_type
* feat(rviz_plugin): console meter is too large on the Rviz with FHD display, isn't it? (`#587 <https://github.com/autowarefoundation/autoware.universe/issues/587>`_)
  * feat(tier4_planning/vehicle_plugin): make plugins size scalable
  * remove space
  * scaling
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: simplify Rolling support (`#854 <https://github.com/autowarefoundation/autoware.universe/issues/854>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* fix: suppress compiler warnings (`#852 <https://github.com/autowarefoundation/autoware.universe/issues/852>`_)
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* fix(tier4_autoware_utils): modify build error in rolling (`#720 <https://github.com/autowarefoundation/autoware.universe/issues/720>`_)
  * fix(tier4_autoware_utils): modify build error in rolling
  * fix(lanelet2_extension): add target compile definition for geometry2
  * fix(ekf_localizer): add target compile definition for geometry2
  * fix(freespace_planning_algorithms): add target compile definition for geometry2
  * fix(interpolation): add target compile definition for geometry2
  * fix(freespace_planner): add target compile definition for geometry2
  * fix(lane_departure_checker): add target compile definition for geometry2
  * fix(map_based_prediction): add target compile definition for geometry2
  * fix(ground_segmentation): add target compile definition for geometry2
  * fix(motion_velocity_smoother): add target compile definition for geometry2
  * fix(multi_object_tracker): add target compile definition for geometry2
  * fix(trajectory_follower): add target compile definition for geometry2
  * fix(control_performance_analysis): add target compile definition for geometry2
  * fix(detected_object_validation): add target compile definition for geometry2
  * fix(goal_distance_calculator): add target compile definition for geometry2
  * fix(ndt_scan_matcher): add target compile definition for geometry2
  * fix(route_handler): add target compile definition for geometry2
  * fix(behavior_path_planner): add target compile definition for geometry2
  * fix(mission_planner): add target compile definition for geometry2
  * fix(obstacle_avoidance_planner): add target compile definition for geometry2
  * fix(obstacle_stop_planner): add target compile definition for geometry2
  * fix(obstacle_collision_checker): add target compile definition for geometry2
  * fix(shape_estimation): add target compile definition for geometry2
  * fix(behavior_velocity_planner): add target compile definition for geometry2
  * fix(path_distance_calculator): add target compile definition for geometry2
  * fix(detection_by_tracker): add target compile definition for geometry2
  * fix(surround_obstacle_checker): add target compile definition for geometry2
  * fix(probabilistic_occupancy_grid_map): add target compile definition for geometry2
  * fix(tier4_debug_tools): add target compile definition for geometry2
  * fix(tier4_vehicle_rviz_plugin): add target compile definition for geometry2
  * fix(pure_pursuit): add target compile definition for geometry2
  * fix(trajectory_follower_nodes): add target compile definition for geometry2
  * fix(occupancy_grid_map_outlier_filter): add target compile definition for geometry2
  * fix(traffic_light_map_based_detector): add target compile definition for geometry2
  * fix(planning_error_monitor): add target compile definition for geometry2
  * fix(planning_evaluator): add target compile definition for geometry2
  * fix(lidar_centerpoint): add target compile definition for geometry2
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
* Contributors: Daisuke Nishimatsu, Kenji Miyake, Mamoru Sobue, Takagi, Isamu, Takamasa Horibe, Takayuki Murooka, Takeshi Miura, Tomoya Kimura, Vincent Richard, Yuxuan Liu, awf-autoware-bot[bot]

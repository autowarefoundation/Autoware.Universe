^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_static_centerline_generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* fix(static_centerline_generator): several bug fixes (`#9426 <https://github.com/autowarefoundation/autoware.universe/issues/9426>`_)
  * fix: dependent packages
  * feat: use steer angle, use warn for steer angle failure, calc curvature dicontinuously
  * fix cppcheck
  ---------
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* refactor(global_parameter_loader): prefix package and namespace with autoware (`#9303 <https://github.com/autowarefoundation/autoware.universe/issues/9303>`_)
* feat!: replace tier4_map_msgs with autoware_map_msgs for MapProjectorInfo (`#9392 <https://github.com/autowarefoundation/autoware.universe/issues/9392>`_)
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
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(static_centerline_generator): map_tf_generator package name needs update (`#9383 <https://github.com/autowarefoundation/autoware.universe/issues/9383>`_)
  fix map_tf_generator name in autoware_static_centerline_generator.launch
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
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Masaki Baba, Ryohsuke Mitsudome, Takayuki Murooka, Yutaka Kondo, Zhanhong Yan

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
* refactor(osqp_interface): added autoware prefix to osqp_interface (`#8958 <https://github.com/autowarefoundation/autoware.universe/issues/8958>`_)
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(other_planning_packages): align the parameters with launcher (`#8793 <https://github.com/autowarefoundation/autoware.universe/issues/8793>`_)
  * parameters in planning/others aligned
  * update json
  ---------
* refactor(map_projection_loader)!: prefix package and namespace with autoware (`#8420 <https://github.com/autowarefoundation/autoware.universe/issues/8420>`_)
  * add autoware\_ prefix
  * add autoware\_ prefix
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* fix(autoware_static_centerline_generator): fix unusedFunction (`#8647 <https://github.com/autowarefoundation/autoware.universe/issues/8647>`_)
  * fix:unusedFunction
  * fix:unusedFunction
  * fix:compile error
  ---------
* refactor(geography_utils): prefix package and namespace with autoware (`#7790 <https://github.com/autowarefoundation/autoware.universe/issues/7790>`_)
  * refactor(geography_utils): prefix package and namespace with autoware
  * move headers to include/autoware/
  ---------
* fix(autoware_static_centerline_generator): fix funcArgNamesDifferent (`#8019 <https://github.com/autowarefoundation/autoware.universe/issues/8019>`_)
  fix:funcArgNamesDifferent
* fix(static_centerline_generator): save_map only once (`#7770 <https://github.com/autowarefoundation/autoware.universe/issues/7770>`_)
* refactor(static_centerline_optimizer): clean up the code (`#7756 <https://github.com/autowarefoundation/autoware.universe/issues/7756>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat(static_centerline_generator): organize AUTO/GUI/VMB modes (`#7432 <https://github.com/autowarefoundation/autoware.universe/issues/7432>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
* feat(map_loader): add waypoints flag (`#7480 <https://github.com/autowarefoundation/autoware.universe/issues/7480>`_)
  * feat(map_loader): handle centelrine and waypoints
  * update README
  * fix doc
  * update schema
  * fix
  * fix
  ---------
* feat(path_optimizer): rename to include/autoware/{package_name} (`#7529 <https://github.com/autowarefoundation/autoware.universe/issues/7529>`_)
* feat(path_smoother): rename to include/autoware/{package_name} (`#7527 <https://github.com/autowarefoundation/autoware.universe/issues/7527>`_)
  * feat(path_smoother): rename to include/autoware/{package_name}
  * fix
  ---------
* refactor(behaivor_path_planner)!: rename to include/autoware/{package_name} (`#7522 <https://github.com/autowarefoundation/autoware.universe/issues/7522>`_)
  * refactor(behavior_path_planner)!: make autoware dir in include
  * refactor(start_planner): make autoware include dir
  * refactor(goal_planner): make autoware include dir
  * sampling planner module
  * fix sampling planner build
  * dynamic_avoidance
  * lc
  * side shift
  * autoware_behavior_path_static_obstacle_avoidance_module
  * autoware_behavior_path_planner_common
  * make behavior_path dir
  * pre-commit
  * fix pre-commit
  * fix build
  ---------
* feat(mission_planner): rename to include/autoware/{package_name} (`#7513 <https://github.com/autowarefoundation/autoware.universe/issues/7513>`_)
  * feat(mission_planner): rename to include/autoware/{package_name}
  * feat(mission_planner): rename to include/autoware/{package_name}
  * feat(mission_planner): rename to include/autoware/{package_name}
  ---------
* fix(static_centerline_generator): fix dependency (`#7442 <https://github.com/autowarefoundation/autoware.universe/issues/7442>`_)
  * fix: deps
  * fix: package name
  * fix: package name
  ---------
* refactor(route_handler): route handler add autoware prefix (`#7341 <https://github.com/autowarefoundation/autoware.universe/issues/7341>`_)
  * rename route handler package
  * update packages dependencies
  * update include guards
  * update includes
  * put in autoware namespace
  * fix formats
  * keep header and source file name as before
  ---------
* refactor(mission_planner)!: add autoware prefix and namespace (`#7414 <https://github.com/autowarefoundation/autoware.universe/issues/7414>`_)
  * refactor(mission_planner)!: add autoware prefix and namespace
  * fix svg
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
* refactor(path_smoother)!: prefix package and namespace with autoware (`#7381 <https://github.com/autowarefoundation/autoware.universe/issues/7381>`_)
  * git mv
  * fix
  * fix launch
  * rever a part of prefix
  * fix test
  * fix
  * fix static_centerline_optimizer
  * fix
  ---------
* refactor(path_optimizer, velocity_smoother)!: prefix package and namespace with autoware (`#7354 <https://github.com/autowarefoundation/autoware.universe/issues/7354>`_)
  * chore(autoware_velocity_smoother): update namespace
  * chore(autoware_path_optimizer): update namespace
  ---------
* chore(bpp): add prefix `autoware\_` (`#7288 <https://github.com/autowarefoundation/autoware.universe/issues/7288>`_)
  * chore(common): rename package
  * fix(static_obstacle_avoidance): fix header
  * fix(dynamic_obstacle_avoidance): fix header
  * fix(side_shift): fix header
  * fix(sampling_planner): fix header
  * fix(start_planner): fix header
  * fix(goal_planner): fix header
  * fix(lane_change): fix header
  * fix(external_lane_change): fix header
  * fix(AbLC): fix header
  * fix(bpp-node): fix header
  * fix(static_centerline_generator): fix header
  * fix(.pages): update link
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
* refactor(behavior_velocity_planner)!: prefix package and namespace with autoware\_ (`#6693 <https://github.com/autowarefoundation/autoware.universe/issues/6693>`_)
* fix(autoware_static_centerline_generator): update the centerline correctly with map projector (`#6825 <https://github.com/autowarefoundation/autoware.universe/issues/6825>`_)
  * fix(static_centerline_generator): fixed the bug of offset lat/lon values
  * fix typo
  ---------
* fix(autoware_static_centerline_generator): remove prefix from topics and node names (`#7028 <https://github.com/autowarefoundation/autoware.universe/issues/7028>`_)
* build(static_centerline_generator): prefix package and namespace with autoware\_ (`#6817 <https://github.com/autowarefoundation/autoware.universe/issues/6817>`_)
  * build(static_centerline_generator): prefix package and namespace with autoware\_
  * style(pre-commit): autofix
  * build: fix CMake target
  * build(autoware_static_centerline_generator): more renames
  * style(pre-commit): autofix
  * build(autoware_static_centerline_generator): fix namespace
  * fix(autoware_static_centerline_generator): fix clang-tidy issues
  * style(pre-commit): autofix
  * style(pre-commit): autofix
  * fix(autoware_static_centerline_generator): fix clang-tidy issues
  * fix(autoware_static_centerline_generator): fix build issues
  * fix(autoware_static_centerline_generator): fix build issues
  * style(pre-commit): autofix
  * fix(autoware_static_centerline_optimizer): fix clang-tidy issues
  * style(pre-commit): autofix
  * build: fix build errors
  * fix: remove else statements after return
  * fix(autoware_static_centerline_generator): fix clang-tidy issues
  * style(pre-commit): autofix
  * revert changes for static_centerline_generator
  * fix(autoware_static_centerline_generator): add autoware\_ prefix
  * style(pre-commit): autofix
  * fix(autoware_static_centerline_generator): fix filenames
  * fix(autoware_static_centerline_generator): fix namespaces
  * style(pre-commit): autofix
  * fix: added prefix to missing strings
  * refactor(autoware_static_centerline_generator): move header files to src
  * refactor(autoware_static_centerline_generator): fix include paths
  * style(pre-commit): autofix
  * refactor(autoware_static_centerline_generator): rename base folder
  * Update planning/autoware_static_centerline_generator/launch/static_centerline_generator.launch.xml
  Co-authored-by: M. Fatih Cırıt <xmfcx@users.noreply.github.com>
  * build(autoware_static_centerline_generator): fix include in CMake
  * build(autoware_static_centerline_generator): fix missing includes
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: M. Fatih Cırıt <xmfcx@users.noreply.github.com>
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Masaki Baba, Ryohsuke Mitsudome, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, Zhe Shen, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

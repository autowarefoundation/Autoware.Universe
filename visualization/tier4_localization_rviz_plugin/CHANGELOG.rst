^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_localization_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore: move rviz plugins from common to visualization/ folder (`#9417 <https://github.com/autowarefoundation/autoware.universe/issues/9417>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe

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
* fix(tier4_localization_rviz_plugin): fix unmatchedSuppression (`#8919 <https://github.com/autowarefoundation/autoware.universe/issues/8919>`_)
  fix:unmatchedSuppression
* fix(tier4_localization_rviz_plugin): fix unusedFunction (`#8848 <https://github.com/autowarefoundation/autoware.universe/issues/8848>`_)
  fix:unusedFunction
* fix(tier4_localization_rviz_plugin): fix constVariableReference (`#8838 <https://github.com/autowarefoundation/autoware.universe/issues/8838>`_)
  fix:constVariableReference
* fix(tier4_localization_rviz_plugin): fix knownConditionTrueFalse (`#8824 <https://github.com/autowarefoundation/autoware.universe/issues/8824>`_)
  * fix:knownConditionTrueFalse
  * fix:merge
  ---------
* refactor(tier4_localization_rviz_plugin): apply static analysis (`#8683 <https://github.com/autowarefoundation/autoware.universe/issues/8683>`_)
  * refactor based on linter
  * add comment on no lint
  * mod comment for clarification
  ---------
* feat(tier4_localization_rviz_plugin): add visualization of pose with covariance history (`#8191 <https://github.com/autowarefoundation/autoware.universe/issues/8191>`_)
  * display PoseWithCovariance History
  * Correct spelling errors and year
  * Add arrows clear()
  * Extension to 3D in matrix calculations
  * style(pre-commit): autofix
  * Correcting spelling mistakes and adding includes
  ---------
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_localization_rviz_plugin): fix unusedFunction (`#8637 <https://github.com/autowarefoundation/autoware.universe/issues/8637>`_)
  fix:unusedFunction
* chore(tier4_localization_rviz_plugin): add maintainer and author (`#8184 <https://github.com/autowarefoundation/autoware.universe/issues/8184>`_)
  add maintainer and author
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
* Contributors: Masaki Baba, SaltUhey, Satoshi OTA, Yamato Ando, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* build: proper eigen deps and include (`#3615 <https://github.com/autowarefoundation/autoware.universe/issues/3615>`_)
  * build: proper eigen deps and include
  * style(pre-commit): autofix
  ---------
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
* feat(tier4_localization_rviz_plugin): add pose history footprint (`#2387 <https://github.com/autowarefoundation/autoware.universe/issues/2387>`_)
  * feat(tier4_localization_rviz_plugin): add pose history footprint
  * remove unused variables
  * add maintainer
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* revert: readability-identifier-naming for pose history (`#1641 <https://github.com/autowarefoundation/autoware.universe/issues/1641>`_)
  * revert: readability-identifier-naming for pose history
  This reverts commit 9278e714bdbd29ca344976e9a2b26fdb93b41370.
  * Revert "fix: build error"
  This reverts commit 5e855993250a94494d9a8d05e03097162d4e6e0e.
* refactor(tier4_localization_rviz_plugin): apply clang-tidy (`#1608 <https://github.com/autowarefoundation/autoware.universe/issues/1608>`_)
  * refactor(tier4_localization_rviz_plugin): apply clang-tidy
  * ci(pre-commit): autofix
  * refactor: add NOLINT
  * refactor: fix readability-identifier-naming
  * ci(pre-commit): autofix
  * fix: build error
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(tier4_perception_rviz_plugin): apply clang-tidy (`#1624 <https://github.com/autowarefoundation/autoware.universe/issues/1624>`_)
  * refactor(tier4_perception_rviz_plugin): apply clang-tidy
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: remove unused check of rviz plugin version (`#1474 <https://github.com/autowarefoundation/autoware.universe/issues/1474>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* Contributors: Hiroki OTA, Kenji Miyake, Takagi, Isamu, Takamasa Horibe, Tomoya Kimura, Vincent Richard, awf-autoware-bot[bot]

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_planning_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* feat(tier4_planning_rviz_plugin): set path colors from rviz and add fade_out feature (`#8972 <https://github.com/autowarefoundation/autoware.universe/issues/8972>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* style: update rviz plugin icons to match the theme (`#8868 <https://github.com/autowarefoundation/autoware.universe/issues/8868>`_)
* fix(tier4_planning_rviz_plugin): fix unusedFunction (`#8616 <https://github.com/autowarefoundation/autoware.universe/issues/8616>`_)
  fix:unusedFunction
* fix(tier4_planning_rviz_plugin): fix cppcheck warning of virtualCallInConstructor (`#8377 <https://github.com/autowarefoundation/autoware.universe/issues/8377>`_)
  fix: deal with virtualCallInConstructor warning
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
* fix(tier4_planning_rviz_plugin): fix calculation of drivable area bounds (`#7815 <https://github.com/autowarefoundation/autoware.universe/issues/7815>`_)
  change angle condition
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
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
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* fix(tier4_planning_rviz_plugin): memory leak (`#7164 <https://github.com/autowarefoundation/autoware.universe/issues/7164>`_)
  fix memory leak
* Contributors: Khalil Selyan, Kosuke Takeuchi, Ryohsuke Mitsudome, Satoshi OTA, Takayuki Murooka, Yukihiro Saito, Yutaka Kondo, beyzanurkaya, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
* fix(readme): add acknowledgement for material icons in tool plugins (`#6354 <https://github.com/autowarefoundation/autoware.universe/issues/6354>`_)
* style(update): autoware tools icons (`#6351 <https://github.com/autowarefoundation/autoware.universe/issues/6351>`_)
* fix(tier4_planning_rviz_plugin): move headers to tier4_planning_rviz_plugin directory (`#5927 <https://github.com/autowarefoundation/autoware.universe/issues/5927>`_)
  * fix(tier4_planning_rviz_plugin): move headers to tier4_planning_rviz_plugin directory
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(planning modules): remove maintainer... (`#5458 <https://github.com/autowarefoundation/autoware.universe/issues/5458>`_)
  remove shimizu-san from maintainer and add maintainer for stop line and turn signal decider
* feat(tier4_planning_rviz_plugin): visualize text of slope angle (`#5091 <https://github.com/autowarefoundation/autoware.universe/issues/5091>`_)
* refactor(common): extern template for motion_utils / remove tier4_autoware_utils.hpp / remove motion_utis.hpp (`#5027 <https://github.com/autowarefoundation/autoware.universe/issues/5027>`_)
* fix(tier4_planning_rviz_plugin): update vehicle info parameters in panel received from global parameter (`#4907 <https://github.com/autowarefoundation/autoware.universe/issues/4907>`_)
* fix: max velocity display (`#4203 <https://github.com/autowarefoundation/autoware.universe/issues/4203>`_)
  fix max velocity display
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* fix(tier4_planning_rviz_plugin): fix plugin crash (`#3830 <https://github.com/autowarefoundation/autoware.universe/issues/3830>`_)
  * preVisualizePathFootPrint is the cause
  * update ogre_node and text_ptr in each iteration
  ---------
* fix(tier4_planning_rviz_plugin): fix drivable area width (`#3689 <https://github.com/autowarefoundation/autoware.universe/issues/3689>`_)
  * fix(tier4_planning_rviz_plugin): fix drivable area width
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
* fix(tier4_planning_rviz_plugin): suppress warning (`#3578 <https://github.com/autowarefoundation/autoware.universe/issues/3578>`_)
* feat(tier4_planning_rviz_plugin): remove z offset from the bound (`#3551 <https://github.com/autowarefoundation/autoware.universe/issues/3551>`_)
* feat(tier4_planning_rviz_plugin): update path width by global parameters (`#3504 <https://github.com/autowarefoundation/autoware.universe/issues/3504>`_)
  * fix(tier4_planning_rviz_plugin): update vehicle info by global parameters
  * feat(tier4_planning_rviz_plugin): update path width by global parameters
  ---------
* fix(tier4_planning_rviz_plugin): update vehicle info by global parameters (`#3503 <https://github.com/autowarefoundation/autoware.universe/issues/3503>`_)
  * fix(tier4_planning_rviz_plugin): update vehicle info by global parameters
  * fix
  ---------
* chore: sync files (`#3227 <https://github.com/autowarefoundation/autoware.universe/issues/3227>`_)
  * chore: sync files
  * style(pre-commit): autofix
  ---------
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_planning_rviz_plugin): supress initial warning message (`#2960 <https://github.com/autowarefoundation/autoware.universe/issues/2960>`_)
  fix(tier4_planning_rviz_plugin): remove initial warning message
* fix(tier4_rviz_planning_plugin): clear objects before return (`#2995 <https://github.com/autowarefoundation/autoware.universe/issues/2995>`_)
  * fix(tier4_rviz_planning_plugin): clear objects before return
  * update
  ---------
* feat(tier4_planning_rviz_plugin): add maintainer (`#2996 <https://github.com/autowarefoundation/autoware.universe/issues/2996>`_)
* feat(tier4_planning_rviz_plugin): move footprint plugin to path (`#2971 <https://github.com/autowarefoundation/autoware.universe/issues/2971>`_)
  * feat(tier4_rviz_plugin): simplify tier4_planning_rviz_plugin
  * update
  ---------
* feat(tier4_planning_rviz_plugin): add drivable area plugin (`#2868 <https://github.com/autowarefoundation/autoware.universe/issues/2868>`_)
  * feat(tier4_planning_rviz_plugin): add drivable area plugin
  * change default size and color
  * update
  * add drivable area to path
  * update
  ---------
* feat(tier4_autoware_utils): remove drivable area plugin (`#2876 <https://github.com/autowarefoundation/autoware.universe/issues/2876>`_)
* refactor(tier4_planning_rviz_plugin): clean up the code of path (`#2871 <https://github.com/autowarefoundation/autoware.universe/issues/2871>`_)
  * refactor(tier4_planning_rviz_plugin): clean up the code of path
  * fix
  ---------
* refactor(tier4_planning_rviz_plugin): create abstract class for footprint (`#2870 <https://github.com/autowarefoundation/autoware.universe/issues/2870>`_)
  * refactor(tier4_planning_rviz_plugin): create abstract class for footprint
  * fix
  * fix
  * fix
  * fix
  ---------
* feat(tier4_planning_rviz_plugin): visualize pose_with_uuid_stamped (`#2662 <https://github.com/autowarefoundation/autoware.universe/issues/2662>`_)
  * feat(tier4_planning_rviz_plugin): visualize pose_stamped_with_uuid
  * Update common/tier4_planning_rviz_plugin/include/pose_stamped_with_uuid/display.hpp
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * Update common/tier4_planning_rviz_plugin/src/pose_stamped_with_uuid/display.cpp
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * rename to pose_with_uuid_stamped
  * add icon
  * change default size
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* feat(behavior_path_planner, obstacle_avoidance_planner): add new drivable area (`#2472 <https://github.com/autowarefoundation/autoware.universe/issues/2472>`_)
  * update
  * update
  * update
  * update obstacle avoidance planner
  * update
  * clean code
  * uddate
  * clean code
  * remove resample
  * update
  * add orientation
  * change color
  * update
  * remove drivable area
  * add flag
  * update
  * update color
  * fix some codes
  * change to makerker array
  * change avoidance utils
* feat(tier4_planning_rviz_plugin): add offset from baselink param (`#2384 <https://github.com/autowarefoundation/autoware.universe/issues/2384>`_)
* fix(tier4_planning_rviz_plugin): correct velocity text (`#2179 <https://github.com/autowarefoundation/autoware.universe/issues/2179>`_)
* fix(tier4_planning/vehicle_rviz_plugin): fixed license (`#2059 <https://github.com/autowarefoundation/autoware.universe/issues/2059>`_)
  * fix(tier4_planning/vehicle_rviz_plugin): fixed license
  * fix build error
* feat(tier4_planning_rviz_plugin): add owner (`#1953 <https://github.com/autowarefoundation/autoware.universe/issues/1953>`_)
* refactor(tier4_planning_rviz_plugin): apply clang-tidy for path (`#1637 <https://github.com/autowarefoundation/autoware.universe/issues/1637>`_)
* feat(tier4_planning_rviz_plugin): add velocity_text to path_with_lane_id (`#1735 <https://github.com/autowarefoundation/autoware.universe/issues/1735>`_)
  * feat(tier4_planning_rviz_plugin): add velocity_text to path_with_lane_id
  * fix pre-commit
* refactor(tier4_planning_rviz_plugin): apply clang-tidy for mission_checkpoint (`#1634 <https://github.com/autowarefoundation/autoware.universe/issues/1634>`_)
  refactor(tier4_planning_rviz_plugin): apply clang-tidy for mission_checkpoint
* refactor(tier4_planning_rviz_plugin): apply clang-tidy for drivable_area (`#1625 <https://github.com/autowarefoundation/autoware.universe/issues/1625>`_)
* fix: remove unused check of rviz plugin version (`#1474 <https://github.com/autowarefoundation/autoware.universe/issues/1474>`_)
* fix(tier4_planning_rviz_plugin): fix initialize planning_rviz_plugin (`#1387 <https://github.com/autowarefoundation/autoware.universe/issues/1387>`_)
  * fix(tier4_planning_rviz_plugin): fix initialize planning_rviz_plugin
  * ci(pre-commit): autofix
  * remove comment out
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_planning_rviz_plugin): support backward driving in path/traj plugin (`#1335 <https://github.com/autowarefoundation/autoware.universe/issues/1335>`_)
  * fix(tier4_planning_rviz_plugin): support backward driving in path_with_lane_id/path/trajectory plugin
  * add utils.hpp
* feat: view LaneId on PathWithLaneIdFootprint plugin (`#984 <https://github.com/autowarefoundation/autoware.universe/issues/984>`_)
  * feat: view LaneId on PathWithLaneIdFootprint plugin
  * ci(pre-commit): autofix
  * fix: add utility
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: set Eigen include directory as SYSTEM for Humble arm64 (`#978 <https://github.com/autowarefoundation/autoware.universe/issues/978>`_)
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
* fix(tier4_planning_rviz_plugins): modify build error in rolling (`#808 <https://github.com/autowarefoundation/autoware.universe/issues/808>`_)
* feat(tier4_planning_rviz_plugins): add vehicle_info to *FootprintDisplay (`#712 <https://github.com/autowarefoundation/autoware.universe/issues/712>`_)
  * feat(tier4_planning_rviz_plugins): add vehicle_info to PathFootprintDisplay
  * add vehicle_info to other footprint displays
  * fix the scope of local variables
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_planning_rviz_plugin): add PathWithLaneIdFootprint rviz plugin (`#594 <https://github.com/autowarefoundation/autoware.universe/issues/594>`_)
  * feat(tier4_planning_rviz_plugin): add PathWithLaneIdFootprint rviz plugin
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(tier4_planning_rviz_plugin): add PathWithLaneId icon (`#593 <https://github.com/autowarefoundation/autoware.universe/issues/593>`_)
* feat(tier4_planning_rviz_plugin): add  PathWithLaneId rviz plugin (`#591 <https://github.com/autowarefoundation/autoware.universe/issues/591>`_)
  * sync rc rc/v1.7.1 (`#2345 <https://github.com/autowarefoundation/autoware.universe/issues/2345>`_)
  * add behavior_path_rviz_plugin (`#2343 <https://github.com/autowarefoundation/autoware.universe/issues/2343>`_)
  * add behavior_path_rviz_plugin
  * edit README
  * fix for uncrustify
  * fix include guard
  * use autoware_lint_common
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Revert "use autoware_lint_common"
  This reverts commit 98c264d5f32d88fb19cd7953fc64a2052648af29.
  * fix for cpplint
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Fix format
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
  * feat(tier4_planning_rviz_plugin): add PathWithLaneId rviz plugin
  Co-authored-by: autoware-iv-sync-ci[bot] <87871706+autoware-iv-sync-ci[bot]@users.noreply.github.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat: add drivable area visualizer (`#779 <https://github.com/autowarefoundation/autoware.universe/issues/779>`_) (`#193 <https://github.com/autowarefoundation/autoware.universe/issues/193>`_)
  * add drivable area visualizer
  * add license
  * modify pointed out in pre-commit
  * modify pointed out in pre-commit
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* fix: fix typo plannnig -> planning (`#195 <https://github.com/autowarefoundation/autoware.universe/issues/195>`_)
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
* Contributors: Daisuke Nishimatsu, Esteve Fernandez, Hiroki OTA, Kenji Miyake, Khalil Selyan, Kosuke Takeuchi, Kyoichi Sugahara, Mamoru Sobue, Maxime CLEMENT, Takagi, Isamu, Takamasa Horibe, Takayuki Murooka, Takeshi Miura, Tomoya Kimura, Vincent Richard, Yukihiro Saito, Yutaka Shimizu, awf-autoware-bot[bot]

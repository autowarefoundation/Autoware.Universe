^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_time_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix: fix package names in changelog files (`#9500 <https://github.com/autowarefoundation/autoware.universe/issues/9500>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(time_utils): prefix package and namespace with autoware (`#9173 <https://github.com/autowarefoundation/autoware.universe/issues/9173>`_)
  * refactor(time_utils): prefix package and namespace with autoware
  * refactor(time_utils): prefix package and namespace with autoware
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(time_utils): prefix package and namespace with autoware (`#9173 <https://github.com/autowarefoundation/autoware.universe/issues/9173>`_)
  * refactor(time_utils): prefix package and namespace with autoware
  * refactor(time_utils): prefix package and namespace with autoware
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(time_utils): fix unusedFunction (`#8606 <https://github.com/autowarefoundation/autoware.universe/issues/8606>`_)
  fix:unusedFunction
* Contributors: Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* chore: add maintainer (`#4234 <https://github.com/autowarefoundation/autoware.universe/issues/4234>`_)
  * chore: add maintainer
  * Update evaluator/localization_evaluator/package.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* ci(pre-commit): clear the exclude option (`#426 <https://github.com/autowarefoundation/autoware.universe/issues/426>`_)
  * ci(pre-commit): remove unnecessary excludes
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * address pre-commit for Markdown files
  * fix Python imports
  * address cpplint errors
  * fix broken package.xml
  * rename ROS parameter files
  * fix build
  * use autoware_lint_common
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add autoware auto dependencies (`#185 <https://github.com/autowarefoundation/autoware.universe/issues/185>`_)
  * Back port .auto control packages (`#571 <https://github.com/autowarefoundation/autoware.universe/issues/571>`_)
  * Implement Lateral and Longitudinal Control Muxer
  * [`#570 <https://github.com/autowarefoundation/autoware.universe/issues/570>`_] Porting wf_simulator
  * [`#1189 <https://github.com/autowarefoundation/autoware.universe/issues/1189>`_] Deactivate flaky test in 'trajectory_follower_nodes'
  * [`#1189 <https://github.com/autowarefoundation/autoware.universe/issues/1189>`_] Fix flacky test in 'trajectory_follower_nodes/latlon_muxer'
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Add osqp_interface package
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Add library code for MPC-based lateral control
  * [`#1271 <https://github.com/autowarefoundation/autoware.universe/issues/1271>`_] Use std::abs instead of abs
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Implement Lateral Controller for Cargo ODD
  * [`#1246 <https://github.com/autowarefoundation/autoware.universe/issues/1246>`_] Resolve "Test case names currently use snake_case but should be CamelCase"
  * [`#1325 <https://github.com/autowarefoundation/autoware.universe/issues/1325>`_] Deactivate flaky smoke test in 'trajectory_follower_nodes'
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add library code of longitudinal controller
  * Fix build error for trajectory follower
  * Fix build error for trajectory follower nodes
  * [`#1272 <https://github.com/autowarefoundation/autoware.universe/issues/1272>`_] Add AckermannControlCommand support to simple_planning_simulator
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add Longitudinal Controller node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Rename velocity_controller -> longitudinal_controller
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Update CMakeLists.txt for the longitudinal_controller_node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add smoke test python launch file
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use LowPassFilter1d from trajectory_follower
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use autoware_auto_msgs
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Changes for .auto (debug msg tmp fix, common func, tf listener)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Remove unused parameters
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix ros test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Rm default params from declare_parameters + use autoware types
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use default param file to setup NodeOptions in the ros test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix docstring
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Replace receiving a Twist with a VehicleKinematicState
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Change class variables format to m\_ prefix
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix plugin name of LongitudinalController in CMakeLists.txt
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix copyright dates
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Reorder includes
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add some tests (~89% coverage without disabling flaky tests)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add more tests (90+% coverage without disabling flaky tests)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use Float32MultiArrayDiagnostic message for debug and slope
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Calculate wheel_base value from vehicle parameters
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Cleanup redundant logger setting in tests
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Set ROS_DOMAIN_ID when running tests to prevent CI failures
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Remove TF listener and use published vehicle state instead
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Change smoke tests to use autoware_testing
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add plotjuggler cfg for both lateral and longitudinal control
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Improve design documents
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Disable flaky test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Properly transform vehicle state in longitudinal node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix TF buffer of lateral controller
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Tuning of lateral controller for LGSVL
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix formating
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix /tf_static sub to be transient_local
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix yaw recalculation of reverse trajs in the lateral controller
  * modify trajectory_follower for galactic build
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update trajectory_follower
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update simple_planning_simulator
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update trajectory_follower_nodes
  * apply trajectory msg modification in control
  * move directory
  * remote control/trajectory_follower level dorectpry
  * remove .iv trajectory follower
  * use .auto trajectory_follower
  * remove .iv simple_planning_simulator & osqp_interface
  * use .iv simple_planning_simulator & osqp_interface
  * add tmp_autoware_auto_dependencies
  * tmporally add autoware_auto_msgs
  * apply .auto message split
  * fix build depend
  * fix packages using osqp
  * fix autoware_auto_geometry
  * ignore lint of some packages
  * ignore ament_lint of some packages
  * ignore lint/pre-commit of trajectory_follower_nodes
  * disable unit tests of some packages
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  * Port parking planner packages from .Auto (`#600 <https://github.com/autowarefoundation/autoware.universe/issues/600>`_)
  * Copy code of 'vehicle_constants_manager'
  * Fix vehicle_constants_manager for ROS galactic
  * Rm .iv costmap_generator freespace_planner freespace_planning_aglorihtms
  * Add astar_search (from .Auto)
  * Copy freespace_planner from .Auto
  * Update freespace_planner for .IV
  * Copy costmap_generator from .Auto
  * Copy and update had_map_utils from .Auto
  * Update costmap_generator
  * Copy costmap_generator_nodes
  * Update costmap_generator_nodes
  * Comment out all tests
  * Move vehicle_constant_managers to tmp_autoware_auto_dependencies
  * ignore pre-commit for back-ported packages
  * ignore testing
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * fix: fix pre-commit
  * fix: fix markdownlint
  * fix: fix cpplint
  * feat: remove autoware_auto_dependencies
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* Contributors: Kenji Miyake, Maxime CLEMENT, Satoshi OTA, Takeshi Miura, Vincent Richard

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package fault_injection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - simulator (`#9572 <https://github.com/autowarefoundation/autoware.universe/issues/9572>`_)
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

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
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(system diags): rename diag of ndt scan matcher (`#6889 <https://github.com/autowarefoundation/autoware.universe/issues/6889>`_)
  rename ndt diag
* feat(fault injection): change for diagnostic graph aggregator (`#6750 <https://github.com/autowarefoundation/autoware.universe/issues/6750>`_)
  * feat: change to adapt diagnostic graph aggregator
  * Change the configuration to adapt to both system_error_monitor and diagnostic_graph_aggregator
  * style(pre-commit): autofix
  * pre-commit fix
  * style(pre-commit): autofix
  * spell check fix
  * clang-tidy fix
  * style(pre-commit): autofix
  * pre-commit fix
  * style(pre-commit): autofix
  * fix datatype
  * cleanup code
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Tomohito ANDO <tomohito.ando@tier4.jp>
* Contributors: Keisuke Shima, Takayuki Murooka, Yamato Ando, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* refactor(localization_error_monitor): rename localization_accuracy (`#5178 <https://github.com/autowarefoundation/autoware.universe/issues/5178>`_)
  refactor: Rename localization_accuracy
  to localization_error_ellipse
* fix(fault_injection_node): fix history_depth in subscriber qos (`#4042 <https://github.com/autowarefoundation/autoware.universe/issues/4042>`_)
  * fix(fault_injection_node): fix history_depth in sub qos
  * fix(fault_injection_node): add test cases
  * fix(fault_injection): fix test
  * fix(test_fault_injection): ensure that publication takes place
  * ref(test_fault_injection): remove unused import
  ---------
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(fault injection): add find package to cmake (`#2973 <https://github.com/autowarefoundation/autoware.universe/issues/2973>`_)
  * fix(fault injection) add find package to cmake
  * feat: add pluginlib to dependency
  ---------
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(fault_injection): fix diag name (`#958 <https://github.com/autowarefoundation/autoware.universe/issues/958>`_)
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: simplify Rolling support (`#854 <https://github.com/autowarefoundation/autoware.universe/issues/854>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* fix: apply fixes for rolling (`#821 <https://github.com/autowarefoundation/autoware.universe/issues/821>`_)
  * fix(component_interface_utils): add USE_DEPRECATED_TO_YAML
  * fix(lidar_apollo_instance_segmentation): add USE_TF2_GEOMETRY_MSGS_DEPRECATED_HEADER
  * add rclcpp_components to package.xml
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(fault_injection): fix empty hardware_id (`#814 <https://github.com/autowarefoundation/autoware.universe/issues/814>`_)
* fix(fault_injection): modify build error in rolling (`#762 <https://github.com/autowarefoundation/autoware.universe/issues/762>`_)
  * fix(fault_injection): modify build error in rolling
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(fault_injection): fix launch_testing (`#489 <https://github.com/autowarefoundation/autoware.universe/issues/489>`_)
  * fix(fault_injection): fix launch_testing
  * add label
  * add todo comment
* ci: check include guard (`#438 <https://github.com/autowarefoundation/autoware.universe/issues/438>`_)
  * ci: check include guard
  * apply pre-commit
  * Update .pre-commit-config.yaml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix: pre-commit
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
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
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: add fault_injection packages  (`#101 <https://github.com/autowarefoundation/autoware.universe/issues/101>`_)
  * Add fault injection package (`#1760 <https://github.com/autowarefoundation/autoware.universe/issues/1760>`_)
  * add fault injection package
  * fix copyright url
  * fix for lint
  * pre-commit fixed
  * change license note
  * separate functions for responsivity
  * add tests
  * add compile option
  * Remove unnecessary descriptions
  * Update the readme
  * Replace png with svg
  * Remove the constructor to follow the recommendations
  * Remove amant_clang_format to match the standard
  * Change the using statement to clarify the type
  * Add using to shorten the type
  * lint
  * change variable name
  * Rename input and delete output
  * sort lines
  * use range-based for
  * added const
  * Remove temporary variables
  * Set an empty value to get all parameters
  * change filename
  * Move test files to test directory
  * Remove unused lines
  * fix to add reference symbol
  * add read_parameter.launch.py
  * remove unused option
  * add comment
  * Change input type to autoware_simulation_msgs
  * refactoring: move parameter function to parameter_handler
  * refactoring
  * remove key_storage
  * replace data with simulation_events
  * remove temporary variable
  * reflects the opinions of review
  * change order
  * delete template
  * change event name
  * reflect review request
  * remove &
  * change constructor argument
  * delete unused function
  * change to event_diag_map class
  * changes for review
  * fix build error
  * fix test error
  * refactor launch_test
  * replace logging with launch.logging
  reason: launch.logging supports verbose output option.
  i.e. launch_test -v FILE
  * merge update function
  * rename callback function
  * move using line
  * add node name as hardware id
  * fix comment
  * change return value
  * add menber to DiagConfig
  * sort menber order
  * use to_yaml
  * remove const
  * change function order
  * rename getValue() to getDiag()
  * add isEventRegistered function
  * move test
  * modify script
  * delete else
  * change cond
  * use docstring style
  * move msg_buffer
  * rename
  * rename
  * fix
  * fix
  * fix
  * use emplace_back
  * add isEventRegistered
  * fix build error
  * remove destroy_sub,pub
  * change check statement
  * add comment
  * fix build error
  * use leveltype
  * fair test
  * change spin time
  * restore config
  * add node name
  * shorten name
  * change function name
  * remove read_parameter
  * expand timeout
  * comment out launch_test
  * Fix a broken link of the component diagram on Fault Injection document (`#2202 <https://github.com/autowarefoundation/autoware.universe/issues/2202>`_)
  * [Fault injection] Update component diagram (`#2203 <https://github.com/autowarefoundation/autoware.universe/issues/2203>`_)
  * Update component diagram
  * Rename pSim to scenario_simulator_v2
  * fix upload error
  * Transparent background
  * Fix line widths of the component diagram on Fault Injection document (`#2205 <https://github.com/autowarefoundation/autoware.universe/issues/2205>`_)
  * Feature/add fault injection settings (`#2199 <https://github.com/autowarefoundation/autoware.universe/issues/2199>`_)
  * add parameter file
  * add message
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
  * remove colcon_ignore in fault injection (`#585 <https://github.com/autowarefoundation/autoware.universe/issues/585>`_)
  * update readme in fault injection (`#644 <https://github.com/autowarefoundation/autoware.universe/issues/644>`_)
  * Update readme in fault_injection
  * fix precommit
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Yohei Mishina <66298900+YoheiMishina@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* Contributors: DMoszynski, Daisuke Nishimatsu, Hiroki OTA, Keisuke Shima, Kenji Miyake, Maxime CLEMENT, Motz, Takagi, Isamu, Tomoya Kimura, Vincent Richard

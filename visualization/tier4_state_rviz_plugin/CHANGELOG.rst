^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_state_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(tier4_state_rviz_plugin): fix unmatchedSuppression (`#8921 <https://github.com/autowarefoundation/autoware.universe/issues/8921>`_)
  fix:unmatchedSuppression
* style: update state panel plugin (`#8846 <https://github.com/autowarefoundation/autoware.universe/issues/8846>`_)
* fix(tier4_state_rviz_plugin): fix unusedFunction (`#8841 <https://github.com/autowarefoundation/autoware.universe/issues/8841>`_)
  fix:unusedFunction
* fix(tier4_state_rviz_plugin): fix unusedFunction (`#8845 <https://github.com/autowarefoundation/autoware.universe/issues/8845>`_)
  * fix:unusedFunction
  * fix:unusedFunction
  * fix:revert
  ---------
* fix(tier4_state_rviz_plugin): fix constVariablePointer (`#8832 <https://github.com/autowarefoundation/autoware.universe/issues/8832>`_)
  fix:constVariablePointer
* fix(tier4_state_rviz_plugin): fix shadowVariable (`#8831 <https://github.com/autowarefoundation/autoware.universe/issues/8831>`_)
  * fix:shadowVariable
  * fix:clang-format
  ---------
* refactor(custom_button): improve drop shadow effect (`#8781 <https://github.com/autowarefoundation/autoware.universe/issues/8781>`_)
* fix(tier4_state_rviz_plugin): fix unmatchedSuppression (`#8658 <https://github.com/autowarefoundation/autoware.universe/issues/8658>`_)
  fix:unmatchedSuppression
* fix(tier4_state_rviz_plugin): fix unusedFunction (`#8608 <https://github.com/autowarefoundation/autoware.universe/issues/8608>`_)
  * fix:unusedFunction
  * fix:clang format
  * fix:revert custom button
  * fix:revert custom container
  * fix:revert custom icon label
  * fix:revert custom label
  * fix:revert custom segment button
  * fix:revert custom slider
  * fix:revert custom toggle switch
  * fix:revert custom label
  * fix:add blank line
  * fix:revert custom botton item
  * fix:remove declaration
  ---------
* feat(tier4_adapi_rviz_plugin, tier4_state_rviz_plugin): set timestamp to velocity_limit msg from rviz panels (`#8548 <https://github.com/autowarefoundation/autoware.universe/issues/8548>`_)
  set timestamp to velocity_limit msg
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat: update autoware state panel (`#7036 <https://github.com/autowarefoundation/autoware.universe/issues/7036>`_)
* Contributors: Autumn60, Khalil Selyan, Ryohsuke Mitsudome, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* feat: add pull over to autoware_state_panel of rviz (`#6540 <https://github.com/autowarefoundation/autoware.universe/issues/6540>`_)
* chore: set log level of debug printing in rviz plugin to DEBUG (`#5996 <https://github.com/autowarefoundation/autoware.universe/issues/5996>`_)
* feat!: remove planning factor type (`#5793 <https://github.com/autowarefoundation/autoware.universe/issues/5793>`_)
  remove planning factor type
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
* feat: change planning factor behavior constants (`#5590 <https://github.com/autowarefoundation/autoware.universe/issues/5590>`_)
  * replace module type
  * support compatibility
  ---------
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
* feat(tier4_state_rviz_plugin): add init by gnss button (`#4392 <https://github.com/autowarefoundation/autoware.universe/issues/4392>`_)
* fix(tier4_state_rviz_plugin): add NEUTRAL on GEAR (`#3132 <https://github.com/autowarefoundation/autoware.universe/issues/3132>`_)
  fix(tier4_state_rviz_plugin): fix bug https://github.com/autowarefoundation/autoware.universe/issues/3121
* refactor(start_planner): rename pull out to start planner (`#3908 <https://github.com/autowarefoundation/autoware.universe/issues/3908>`_)
* build(iron): remove rmw_qos_profile_t (`#3809 <https://github.com/autowarefoundation/autoware.universe/issues/3809>`_)
* fix(rviz_plugin): fx traffic light and velocity factor rviz plugin (`#3598 <https://github.com/autowarefoundation/autoware.universe/issues/3598>`_)
  fix(rviz_plugin); fx traffic light and velocity factor rviz plugin
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* refactor(behavior_path_planner): rename pull_over to goal_planner (`#3501 <https://github.com/autowarefoundation/autoware.universe/issues/3501>`_)
* chore: sync files (`#3227 <https://github.com/autowarefoundation/autoware.universe/issues/3227>`_)
  * chore: sync files
  * style(pre-commit): autofix
  ---------
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_state_panel): change variable for fail safe behavior (`#2952 <https://github.com/autowarefoundation/autoware.universe/issues/2952>`_)
  fix fail safe behavior value
* fix(tier4_state_rviz_plugin): fix typo (`#2988 <https://github.com/autowarefoundation/autoware.universe/issues/2988>`_)
* fix(tier4_state_rviz_plugin): split into two panels (`#2914 <https://github.com/autowarefoundation/autoware.universe/issues/2914>`_)
  * fix(tier4_state_rviz_plugin): split into two panels
  * feat: add image
  * style(pre-commit): autofix
  * Update common/tier4_state_rviz_plugin/src/velocity_steering_factors_panel.cpp
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * Update common/tier4_state_rviz_plugin/src/velocity_steering_factors_panel.hpp
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * Update autoware_state_panel.hpp
  * Update autoware_state_panel.cpp
  * Update common/tier4_state_rviz_plugin/src/velocity_steering_factors_panel.cpp
  * Update common/tier4_state_rviz_plugin/src/velocity_steering_factors_panel.hpp
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* feat(tier4_state_rviz_plugin): add planning API visualization (`#2632 <https://github.com/autowarefoundation/autoware.universe/issues/2632>`_)
  feat(tier4_state_rviz_plugin): add Planning Visualization
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* feat(tier4_state_rviz_plugin): add Fail Safe Visualization (`#2626 <https://github.com/autowarefoundation/autoware.universe/issues/2626>`_)
  * feat(tier4_state_rviz_plugin): add information for Fail Safe
  * fix color
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* docs(tier4_state_rviz_plugin): update readme (`#2475 <https://github.com/autowarefoundation/autoware.universe/issues/2475>`_)
* feat(tier4_state_rviz_plugin): add API monitoring for Routing, Localization and Motion (`#2436 <https://github.com/autowarefoundation/autoware.universe/issues/2436>`_)
  * feat: add viz for routing API
  * feat: add motion and localiation
  * some refactoring
  * add comment
  * add vertical align
  * fix: in transition
  * fix: setupLabel -> updateLabel
  * add memory
  * fix pre commit
  * ci(pre-commit): autofix
  Co-authored-by: Takagi, Isamu <isamu.takagi@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_state_rviz_plugin): use ADAPI v1 instead of old API (`#2433 <https://github.com/autowarefoundation/autoware.universe/issues/2433>`_)
  * fix: delete path change approval
  * make operation and control mode layout
  * add nullptr
  * fix pre-commit
  * fix comment
  * fix: rename enable disable
  * feat: add TRANSITION
  * fix comment
  * delete unused
* chore(tier4_state_rviz_plugin): add maintainer (`#2435 <https://github.com/autowarefoundation/autoware.universe/issues/2435>`_)
* revert(tier4_state_rviz_plugin): readability-identifier-naming (`#1595 <https://github.com/autowarefoundation/autoware.universe/issues/1595>`_) (`#1617 <https://github.com/autowarefoundation/autoware.universe/issues/1617>`_)
  revert(tier4_state_rviz_plugin): readability-identifier-naming (`#1595 <https://github.com/autowarefoundation/autoware.universe/issues/1595>`_)"
  This reverts commit 57720204fd401a59b5dffd12d5b8958e5ae2a5af.
* refactor(tier4_state_rviz_plugin): apply clang-tidy for readability-identifier-naming (`#1595 <https://github.com/autowarefoundation/autoware.universe/issues/1595>`_)
  * refactor(tier4_state_rviz_plugin): apply clang-tidy for readability-identifier-naming
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(tier4_state_rviz_plugin): apply clang-tidy (`#1589 <https://github.com/autowarefoundation/autoware.universe/issues/1589>`_)
  * fix: clang-tidy for tier4_state_rviz_plugin
  * ci(pre-commit): autofix
  * Update common/tier4_state_rviz_plugin/src/autoware_state_panel.cpp
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update common/tier4_state_rviz_plugin/src/autoware_state_panel.cpp
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * ci(pre-commit): autofix
  * Update common/tier4_state_rviz_plugin/src/autoware_state_panel.hpp
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix: delete NOLINT
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* fix: remove unused check of rviz plugin version (`#1474 <https://github.com/autowarefoundation/autoware.universe/issues/1474>`_)
* fix(tier4_state_rviz_plugin): qos (`#1085 <https://github.com/autowarefoundation/autoware.universe/issues/1085>`_)
* feat(tier4_state_rviz_plugin): add emergency button (`#1048 <https://github.com/autowarefoundation/autoware.universe/issues/1048>`_)
  * feat(tier4_state_rviz_plugin):add emergency button
  * ci(pre-commit): autofix
  * chore: add default button name
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* revert: engage button action in autoware_state_panel (`#1079 <https://github.com/autowarefoundation/autoware.universe/issues/1079>`_)
  * Revert "fix(autoware_state_panel): fix message type for /api/autoware/get/engage (`#666 <https://github.com/autowarefoundation/autoware.universe/issues/666>`_)"
  This reverts commit 49cc906418b15994b7facb881f3c133a9d8eb3a1.
  * Revert "fix(tier4_state_rviz_plugin): change service and topic name for engage (`#633 <https://github.com/autowarefoundation/autoware.universe/issues/633>`_)"
  This reverts commit 15f43bc7063809d38c369e405a82d9666826c052.
* feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(rviz_plugins): add velocity limit to autoware state panel (`#879 <https://github.com/autowarefoundation/autoware.universe/issues/879>`_)
  * feat(rviz_plugins): add velocity limit to autoware state panel
  * chore(rviz_plugin): change ms to kmh
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* fix(autoware_state_panel): fix message type for /api/autoware/get/engage (`#666 <https://github.com/autowarefoundation/autoware.universe/issues/666>`_)
  * fix(autoware_state_panel): fix message type for /api/autoware/get/engage
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_state_rviz_plugin): change service and topic name for engage (`#633 <https://github.com/autowarefoundation/autoware.universe/issues/633>`_)
* feat: add selector mode and disengage function (`#781 <https://github.com/autowarefoundation/autoware.universe/issues/781>`_) (`#194 <https://github.com/autowarefoundation/autoware.universe/issues/194>`_)
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
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
* Contributors: Daisuke Nishimatsu, Fumiya Watanabe, Hiroki OTA, Kenji Miyake, Kosuke Takeuchi, Mark Jin, Satoshi OTA, Shumpei Wakabayashi, Takagi, Isamu, Takayuki Murooka, TetsuKawa, Tomoya Kimura, Vincent Richard, awf-autoware-bot[bot], taikitanaka3, yabuta

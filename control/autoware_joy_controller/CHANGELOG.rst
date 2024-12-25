^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_joy_controller
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_joy_controller): add virtual destructor to autoware_joy_controller (`#7760 <https://github.com/autowarefoundation/autoware.universe/issues/7760>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(joy_controller): check for nullptr messages (`#7540 <https://github.com/autowarefoundation/autoware.universe/issues/7540>`_)
* refactor(control)!: refactor directory structures of the control interface nodes (`#7528 <https://github.com/autowarefoundation/autoware.universe/issues/7528>`_)
  * external_cmd_selector
  * joy_controller
  ---------
* refactor(joy_controller)!: prefix package and namespace with autoware (`#7382 <https://github.com/autowarefoundation/autoware.universe/issues/7382>`_)
  * add prefix
  * fix codeowner
  * fix
  * fix
  ---------
* Contributors: Kosuke Takeuchi, Maxime CLEMENT, Takayuki Murooka, Yuki TAKAGI, Yukinari Hisaki, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: add autoware joy controller (`#72 <https://github.com/autowarefoundation/autoware.universe/issues/72>`_)
  * release v0.4.0
  * Support G29 controller in autoware_joy_controller (`#699 <https://github.com/autowarefoundation/autoware.universe/issues/699>`_)
  * Add map for G29 controller
  * Add new line at end of file
  * Change structure of JoyConverterBase class
  * Rename PS4 -> DS4
  * Rename controler_type -> joy_type
  * Set joy_type by console input
  * Change doc
  * Remap g29 controller
  * Remap AccelPedal -> accel, BrakePedal -> brake
  * Remove [autoware_joy_controller] from ROS_INFO
  Co-authored-by: Fumiya Watanabe <fumiya.watanabe@tier4.jp>
  * Change key map for G29 controller and set deadzone parameter (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * Add missing dependencies of autoware_joy_controller (`#755 <https://github.com/autowarefoundation/autoware.universe/issues/755>`_)
  * remove ROS1 packages temporarily
  * add sample ros2 packages
  * remove ROS1 packages
  * Revert "remove ROS1 packages temporarily"
  This reverts commit c98294b0b159fb98cd3091d34a626d06f29fdece.
  * add COLCON_IGNORE to ros1 packages
  * Rename launch files to launch.xml (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
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
  * Port autoware joy controller (`#124 <https://github.com/autowarefoundation/autoware.universe/issues/124>`_)
  * Port
  * Fixed package.xml
  * now() to use node clock
  * Fix include
  * Clear compilation warnings
  * Run uncrustify on the entire Pilot.Auto codebase (`#151 <https://github.com/autowarefoundation/autoware.universe/issues/151>`_)
  * Run uncrustify on the entire Pilot.Auto codebase
  * Exclude open PRs
  * [update to v0.8.0] autoware joy controller (`#251 <https://github.com/autowarefoundation/autoware.universe/issues/251>`_)
  * restore filename to original for version update
  * Enable to change sensitivity (`#868 <https://github.com/autowarefoundation/autoware.universe/issues/868>`_)
  * Improve remote emergency stop (`#900 <https://github.com/autowarefoundation/autoware.universe/issues/900>`_)
  * Apply format
  * Rename emergency to system_emergency in vehicle_cmd_gate
  * Add emergency stop feature to vehicle_cmd_gate
  * Fix frame_id of vehicle_cmd_gate output
  * Rename /remote/emergency to /remote/emergency_stop in autoware_joy_controller
  * Rename /remote/emergency to /remote/emergency_stop in remote_cmd_converter
  * Rename /remote/emergency to /remote/emergency_stop in autoware_api
  * Check emergency_stop timeout in remote_cmd_converter
  * Ignore timeout = 0.0
  * Add config_file to arg
  * Rename emergency_stop to external_emergency_stop
  * Remove unnecessary lines
  * Wait for first heartbeat
  * Add clear_emergency_stop service
  * Call clear_external_emegency_stop service from autoware_joy_controller
  * Rename function
  * Revert: Wait for first heartbeat
  * Fix console messages
  * Move emergency_stop diag to vehicle_cmd_gate
  * Add heartbeat to vehicle_cmd_gate
  * Revert: Move emergency_stop diag to vehicle_cmd_gate
  * patch in real-vehicle
  * Apply format
  * Change default parameter
  Co-authored-by: jpntaxi4943-autoware <proj-jpntaxi@tier4.jp>
  * restore file name
  * [tmp] fix build error
  * fix service
  * fix format
  * fix service usage
  * fix launch var
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: jpntaxi4943-autoware <proj-jpntaxi@tier4.jp>
  * Rename ROS-related .yaml to .param.yaml (`#352 <https://github.com/autowarefoundation/autoware.universe/issues/352>`_)
  * Rename ROS-related .yaml to .param.yaml
  * Remove prefix 'default\_' of yaml files
  * Rename vehicle_info.yaml to vehicle_info.param.yaml
  * Rename diagnostic_aggregator's param files
  * Fix overlooked parameters
  * remove using in global namespace (`#379 <https://github.com/autowarefoundation/autoware.universe/issues/379>`_)
  * remove using in global namespace (`#1166 <https://github.com/autowarefoundation/autoware.universe/issues/1166>`_)
  * remove using in global namespace
  * Revert "remove using in global namespace"
  This reverts commit 7f120509c9e3a036a38e84883868f6036bca23ad.
  * Add package namespace
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * [autoware_joy_controller] add lint tests
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * fix namespace (`#414 <https://github.com/autowarefoundation/autoware.universe/issues/414>`_)
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Fix for rolling (`#1226 <https://github.com/autowarefoundation/autoware.universe/issues/1226>`_)
  * Replace doc by description
  * Replace ns by push-ros-namespace
  * Make control modules components (`#1262 <https://github.com/autowarefoundation/autoware.universe/issues/1262>`_)
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Remove autoware_debug_msgs from autoware_joy_controller (`#1303 <https://github.com/autowarefoundation/autoware.universe/issues/1303>`_)
  * Porting remote cmd selector (`#1286 <https://github.com/autowarefoundation/autoware.universe/issues/1286>`_)
  * Feature/add remote cmd selector (`#1179 <https://github.com/autowarefoundation/autoware.universe/issues/1179>`_)
  * Add in/out args of remote_cmd_converter.launch
  * Change remote input topic of vehicle_cmd_gate
  * Add msgs for remote_cmd_selector
  * Add remote_cmd_selector
  * Rename remote_cmd_selector to external_cmd_selector
  * Remove VehicleCommand support in autoware_joy_controller
  * Support external_cmd_source in autoware_joy_controller.launch (`#1194 <https://github.com/autowarefoundation/autoware.universe/issues/1194>`_)
  * Fix porting miss
  * fix missing function
  * modify xml format
  * fix include guard
  * add callback group
  * modify remap name
  * Revert "modify remap name"
  This reverts commit 169cc8d28442825b1d61b0439b9892c913304527.
  * change topic name
  * use rclcpp_component
  * Remove autoware_debug_msgs from autoware_joy_controller
  * Change default mode of autoware_joy_controller
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * Ros2/create/external commands (`#1299 <https://github.com/autowarefoundation/autoware.universe/issues/1299>`_)
  * add remote message
  * add remote commands
  * fix topic
  * remove unnecessary topic
  * remove unused topic
  * add external cmd instead
  * ToExternalComd
  * fix topic in joy con
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * Add autoware api (`#1979 <https://github.com/autowarefoundation/autoware.universe/issues/1979>`_)
  * Use EmergencyState instead of deprecated EmergencyMode (`#2030 <https://github.com/autowarefoundation/autoware.universe/issues/2030>`_)
  * Use EmergencyState instead of deprecated EmergencyMode
  * Use stamped type
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
  * Add selected external command API (`#2053 <https://github.com/autowarefoundation/autoware.universe/issues/2053>`_)
  * submit engage with api service from joy controller (`#2320 <https://github.com/autowarefoundation/autoware.universe/issues/2320>`_)
  * fix engagew with api
  * delete unused
  * fix for uncrustify
  * revive vehicle_engage
  * some fix
  * revive autoware name
  * fix service name
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
  * port autoware joy controller (`#588 <https://github.com/autowarefoundation/autoware.universe/issues/588>`_)
  * port autoware joy controller
  * fix compile error
  * use odometry instead of twist
  * update launch
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  * update README.md in autoware_joy_controller (`#593 <https://github.com/autowarefoundation/autoware.universe/issues/593>`_)
  * update README.md
  * update README.md
  * fix typo
  * Update control/autoware_joy_controller/README.md
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * update README.md
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * fix format
  * ci(pre-commit): autofix
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Fumiya Watanabe <fumiya.watanabe@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: Servando <43142004+sgermanserrano@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: jpntaxi4943-autoware <proj-jpntaxi@tier4.jp>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Keisuke Shima <keisuke.shima@tier4.jp>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* Contributors: Tomoya Kimura, taikitanaka3

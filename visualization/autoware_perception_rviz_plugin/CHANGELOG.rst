^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_perception_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_perception_rviz_plugin): fix unusedFunction (`#8784 <https://github.com/autowarefoundation/autoware.universe/issues/8784>`_)
  fix: unusedFunction
* feat(autoware_perception_rviz_plugin): rviz predicted path mark as triangle (`#8536 <https://github.com/autowarefoundation/autoware.universe/issues/8536>`_)
  * refactor: predicted path mark replace to triangle
  * chore: clean up
  ---------
* fix(autoware_perception_rviz_plugin): fix passedByValue (`#8192 <https://github.com/autowarefoundation/autoware.universe/issues/8192>`_)
  * fix: passedByValue
  * fix:passedByValue
  ---------
  Co-authored-by: kobayu858 <yutaro.kobayashi@tier4.jp>
* chore(autoware_perception_rviz_plugin): delete maintainer (`#7900 <https://github.com/autowarefoundation/autoware.universe/issues/7900>`_)
* fix(autoware_perception_rviz_plugin): fix duplicateBranch warnings (`#7695 <https://github.com/autowarefoundation/autoware.universe/issues/7695>`_)
  * fix(autoware_perception_rviz_plugin): fix duplicateBranch warnings
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat(autoware_auto_perception_rviz_plugin)!: rename package to autoware_perception_rviz_plugin (`#7221 <https://github.com/autowarefoundation/autoware.universe/issues/7221>`_)
  feat(autoware_auto_perception_rviz_plugin): rename package to autoware_perception_rviz_plugin
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* Contributors: Hayate TOBA, Nagi70, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi Tanaka, Taekjin LEE, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* feat: add rviz plugin packages (`#3 <https://github.com/autowarefoundation/autoware.universe/issues/3>`_)
  * release v0.4.0
  * remove ROS1 packages temporarily
  * add sample ros2 packages
  * remove ROS1 packages
  * Revert "remove ROS1 packages temporarily"
  This reverts commit 7eacbcea261a65d6c305c7b0d069591ca3a2ee3a.
  * add COLCON_IGNORE to ros1 packages
  * Port autoware-perception-rviz-plugin (`#100 <https://github.com/autowarefoundation/autoware.universe/issues/100>`_)
  * Port to ROS2
  * Update namespaces
  * Port autoware-planning-rviz-plugin (`#103 <https://github.com/autowarefoundation/autoware.universe/issues/103>`_)
  * Port to ROS2
  * Update deprecated
  * Update namespaces
  * Adjust copyright notice on 532 out of 699 source files (`#143 <https://github.com/autowarefoundation/autoware.universe/issues/143>`_)
  * Use quotes for includes where appropriate (`#144 <https://github.com/autowarefoundation/autoware.universe/issues/144>`_)
  * Use quotes for includes where appropriate
  * Fix lint tests
  * Make tests pass hopefully
  * Run uncrustify on the entire Pilot.Auto codebase (`#151 <https://github.com/autowarefoundation/autoware.universe/issues/151>`_)
  * Run uncrustify on the entire Pilot.Auto codebase
  * Exclude open PRs
  * Fix rviz plugins (`#175 <https://github.com/autowarefoundation/autoware.universe/issues/175>`_)
  * [autoware_perception_rviz_plugin] make library to shared and fix library name in plugin_description.xml
  * [autoware_planning_rviz_plugin] make library to shared and fix library name in plugin_description.xml
  * Port autoware vehicle rviz plugin (`#111 <https://github.com/autowarefoundation/autoware.universe/issues/111>`_)
  * Port to ROS2
  * Amend buildtool
  * Fix license
  * Fix
  * Fixes
  * adding linters to autoware_planning_rviz_plugin (`#224 <https://github.com/autowarefoundation/autoware.universe/issues/224>`_)
  * adding linters to autoware_perception_rviz_plugin (`#225 <https://github.com/autowarefoundation/autoware.universe/issues/225>`_)
  * [autoware_perception_rviz_plugin] make plugin library SHARED (`#236 <https://github.com/autowarefoundation/autoware.universe/issues/236>`_)
  * Fix bugs in autoware vehicle rviz plugin (`#246 <https://github.com/autowarefoundation/autoware.universe/issues/246>`_)
  * Ros2 v0.8.0 autoware vehicle rviz plugin (`#333 <https://github.com/autowarefoundation/autoware.universe/issues/333>`_)
  * add test depend
  * fix console meter size (`#909 <https://github.com/autowarefoundation/autoware.universe/issues/909>`_)
  * update to change font scale (`#910 <https://github.com/autowarefoundation/autoware.universe/issues/910>`_)
  * Fix typos in common modules (`#914 <https://github.com/autowarefoundation/autoware.universe/issues/914>`_)
  * fix typos in common modules
  * minor fix (lowercasing)
  * revert changes in PathPoint.msg
  * Fix memory leaks in turn signal plugin (`#932 <https://github.com/autowarefoundation/autoware.universe/issues/932>`_)
  * fix memory leak (QPointF)
  * convert raw pointers to smart pointers
  * update handle image (`#948 <https://github.com/autowarefoundation/autoware.universe/issues/948>`_)
  * reduce calc cost rviz plugin (`#947 <https://github.com/autowarefoundation/autoware.universe/issues/947>`_)
  * reduce calc cost
  * cosmetic change
  * cosmetic change
  * Use CMAKE_CXX_STANDARD to enable C++14 for Qt
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * change font size independency desplay (`#946 <https://github.com/autowarefoundation/autoware.universe/issues/946>`_)
  * bug fix (wrong unit conversion) (`#956 <https://github.com/autowarefoundation/autoware.universe/issues/956>`_)
  * Refactor autoware_vehicle_rviz_plugin (`#967 <https://github.com/autowarefoundation/autoware.universe/issues/967>`_)
  * Refactor autoware_vehicle_rviz_plugin
  - change smart pointers to raw pointers according to Qt convention
  - remove unused headers
  - remove unused variables
  - cosmetic changes according to Google C++ Style Guide
  - use the range-based for statement
  - replace push_back with emplace_back
  See also: `#932 <https://github.com/autowarefoundation/autoware.universe/issues/932>`_, `#964 <https://github.com/autowarefoundation/autoware.universe/issues/964>`_
  * Apply clang-format
  * Change a variable name to clarify: history -> histories
  * add build testing
  * appply ament_uncrustify
  * apply lint
  * fix bug
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * Ros2 v0.8.0 autoware perception rviz plugin (`#334 <https://github.com/autowarefoundation/autoware.universe/issues/334>`_)
  * Fix typos in common modules (`#914 <https://github.com/autowarefoundation/autoware.universe/issues/914>`_)
  * fix typos in common modules
  * minor fix (lowercasing)
  * revert changes in PathPoint.msg
  * ament_cmake_cppcheck  -> ament_lint_common
  * apply lint
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Ros2 v0.8.0 autoware planning rviz plugin (`#336 <https://github.com/autowarefoundation/autoware.universe/issues/336>`_)
  * add speed limit visualizer (`#908 <https://github.com/autowarefoundation/autoware.universe/issues/908>`_)
  * add speed limit visualizer
  * :put_litter_in_its_place:
  * add max velocity output
  * fix bug
  * update visualizer
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  * change font size independency desplay (`#946 <https://github.com/autowarefoundation/autoware.universe/issues/946>`_)
  * ament_cmake_cppcheck -> ament_lint_common
  * apply lint
  * change topic type
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * Ros2 v0.8.0 remove std msgs awapi (`#348 <https://github.com/autowarefoundation/autoware.universe/issues/348>`_)
  * [autoware_vehicle_msgs] add BatteryStatus msg
  * [autoware_planning_msgs] add ExpandStopRange and StopSpeedExceeded messages
  * [autoware_api_msgs] add DoorControlCommand, StopCommand, and VelocityLimit messages
  * remove std_msgs related to autoware_awaiv_adapter node
  * apply ament_uncrustify
  * fix build failure
  * fix test failures
  * address review commends
  * Ros2 v0.9.0 pose history (`#387 <https://github.com/autowarefoundation/autoware.universe/issues/387>`_)
  * Port pose history to ROS2
  * pose_history (`#1169 <https://github.com/autowarefoundation/autoware.universe/issues/1169>`_)
  * change pkg name
  * add alpha
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * fix max velocity visualization (`#397 <https://github.com/autowarefoundation/autoware.universe/issues/397>`_)
  * fix max velocity vis
  * apply lint-format
  * Ros2 rtd plugin (`#444 <https://github.com/autowarefoundation/autoware.universe/issues/444>`_)
  * Use RTD instead of MFD
  * Sync public repo (`#1228 <https://github.com/autowarefoundation/autoware.universe/issues/1228>`_)
  * [simple_planning_simulator] add readme (`#424 <https://github.com/autowarefoundation/autoware.universe/issues/424>`_)
  * add readme of simple_planning_simulator
  * Update simulator/simple_planning_simulator/README.md
  * set transit_margin_time to intersect. planner (`#460 <https://github.com/autowarefoundation/autoware.universe/issues/460>`_)
  * Fix pose2twist (`#462 <https://github.com/autowarefoundation/autoware.universe/issues/462>`_)
  * Ros2 vehicle info param server (`#447 <https://github.com/autowarefoundation/autoware.universe/issues/447>`_)
  * add vehicle_info_param_server
  * update vehicle info
  * apply format
  * fix bug
  * skip unnecessary search
  * delete vehicle param file
  * fix bug
  * Ros2 fix topic name part2 (`#425 <https://github.com/autowarefoundation/autoware.universe/issues/425>`_)
  * Fix topic name of traffic_light_classifier
  * Fix topic name of traffic_light_visualization
  * Fix topic name of traffic_light_ssd_fine_detector
  * Fix topic name of traffic_light_map_based_detector
  * Fix lint traffic_light_recognition
  * Fix lint traffic_light_ssd_fine_detector
  * Fix lint traffic_light_classifier
  * Fix lint traffic_light_classifier
  * Fix lint traffic_light_ssd_fine_detector
  * Fix issues in hdd_reader (`#466 <https://github.com/autowarefoundation/autoware.universe/issues/466>`_)
  * Fix some issues detected by Coverity Scan and Clang-Tidy
  * Update launch command
  * Add more `close(new_sock)`
  * Simplify the definitions of struct
  * fix: re-construct laneletMapLayer for reindex RTree (`#463 <https://github.com/autowarefoundation/autoware.universe/issues/463>`_)
  * Rviz overlay render fix (`#461 <https://github.com/autowarefoundation/autoware.universe/issues/461>`_)
  * Moved painiting in SteeringAngle plugin to update()
  * super class now back to MFD
  * uncrustified
  * acquire data in mutex
  * back to RTD as superclass
  * Rviz overlay render in update (`#465 <https://github.com/autowarefoundation/autoware.universe/issues/465>`_)
  * Moved painiting in SteeringAngle plugin to update()
  * super class now back to MFD
  * uncrustified
  * acquire data in mutex
  * removed unnecessary includes and some dead code
  * Adepted remaining vehicle plugin classes to render-in-update concept. Returned to MFD superclass
  * restored RTD superclass
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  * Unify Apache-2.0 license name (`#1242 <https://github.com/autowarefoundation/autoware.universe/issues/1242>`_)
  * Porting trajectory rviz plugin (`#1295 <https://github.com/autowarefoundation/autoware.universe/issues/1295>`_)
  * update trajectory rviz plugin to show velocity (`#1257 <https://github.com/autowarefoundation/autoware.universe/issues/1257>`_)
  * update trajectory rviz plugin to show velocity
  * use size_t instead of int to remove warning during compiling
  * not show velocity on rviz unless check button is enabled
  * modify visibility of velocity (`#1258 <https://github.com/autowarefoundation/autoware.universe/issues/1258>`_)
  * fix plugin
  * add dependency
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  * Fix msgs (`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_)
  * Fix msgs
  * [autoware_planning_rviz_plugin]: Fix lint
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  * Fix topic name of autoware_perception_rviz_plugin (`#1277 <https://github.com/autowarefoundation/autoware.universe/issues/1277>`_) (`#1479 <https://github.com/autowarefoundation/autoware.universe/issues/1479>`_)
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Porting polar grid to ros2 (`#1507 <https://github.com/autowarefoundation/autoware.universe/issues/1507>`_)
  * Add dummy unknown publisher (`#1470 <https://github.com/autowarefoundation/autoware.universe/issues/1470>`_)
  * Add dummy unknown publisher
  * Fix lint
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * Add pre-commit (`#1560 <https://github.com/autowarefoundation/autoware.universe/issues/1560>`_)
  * add pre-commit
  * add pre-commit-config
  * add additional settings for private repository
  * use default pre-commit-config
  * update pre-commit setting
  * Ignore whitespace for line breaks in markdown
  * Update .github/workflows/pre-commit.yml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * exclude svg
  * remove pretty-format-json
  * add double-quote-string-fixer
  * consider COLCON_IGNORE file when seaching modified package
  * format file
  * pre-commit fixes
  * Update pre-commit.yml
  * Update .pre-commit-config.yaml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * suppress warnings for common packages (`#1891 <https://github.com/autowarefoundation/autoware.universe/issues/1891>`_)
  * add maybe unused
  * add Werror
  * fix for uncrustify
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
  * add autoware_state_rviz_plugin (`#2160 <https://github.com/autowarefoundation/autoware.universe/issues/2160>`_)
  * initial commit
  * fix
  * use raw pointer
  * fix style
  * fix style
  * fix style
  * fix style
  * fix header arrangement
  * add gear check and prefix label (`#2173 <https://github.com/autowarefoundation/autoware.universe/issues/2173>`_)
  * add gear and prefix label
  * add subscription
  * fix for cpplint
  * add engage button and status (`#2257 <https://github.com/autowarefoundation/autoware.universe/issues/2257>`_)
  * fix style
  * add engage button and engage status
  * use api
  * fix for pre commit
  * fix for cpplint
  * fix
  * fix for cpplint
  * fix for cpplint
  * fix coding style
  * Add datetime panel (`#2275 <https://github.com/autowarefoundation/autoware.universe/issues/2275>`_)
  * Add datetime panel
  * Fix/ros time (`#2276 <https://github.com/autowarefoundation/autoware.universe/issues/2276>`_)
  * Fix ros time
  * Add icon
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * add footprint in trajectory msg of rviz plugin (`#1553 <https://github.com/autowarefoundation/autoware.universe/issues/1553>`_) (`#1684 <https://github.com/autowarefoundation/autoware.universe/issues/1684>`_)
  * add footprint in trajectory msg of rviz plugin (`#1553 <https://github.com/autowarefoundation/autoware.universe/issues/1553>`_)
  * add footprint in trajectory msg of rviz plugin
  * update
  * trajectory -> footprint
  * update
  * add icons
  * rename trajectory footprint from footprint
  * add PathFootprint
  * update
  * Add min value
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  * Feature/trajectory point rviz plugin (`#2123 <https://github.com/autowarefoundation/autoware.universe/issues/2123>`_)
  * add trajectory point
  * set trajectory point view false by default
  * add pull over/out module (`#2147 <https://github.com/autowarefoundation/autoware.universe/issues/2147>`_)
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
  * remove unused depends (`#496 <https://github.com/autowarefoundation/autoware.universe/issues/496>`_)
  * Add COLCON_IGNORE (`#500 <https://github.com/autowarefoundation/autoware.universe/issues/500>`_)
  * port planning rviz plugins (`#492 <https://github.com/autowarefoundation/autoware.universe/issues/492>`_)
  * port planning rviz plugins
  * remove COLCON_IGNORE
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  * port autoware vehicle rviz plugin (`#542 <https://github.com/autowarefoundation/autoware.universe/issues/542>`_)
  * [ polar grid ] add readme polar grid remove colcon ignore (`#559 <https://github.com/autowarefoundation/autoware.universe/issues/559>`_)
  * remove ignore
  * add readme
  * fix invalid link
  * port autoware_state_rviz_plugin (`#563 <https://github.com/autowarefoundation/autoware.universe/issues/563>`_)
  * remove COLCON_IGNORE form rviz plugins (`#544 <https://github.com/autowarefoundation/autoware.universe/issues/544>`_)
  * port autoware_perception_rviz_plugin (`#581 <https://github.com/autowarefoundation/autoware.universe/issues/581>`_)
  * add readme in rviz plugin (`#591 <https://github.com/autowarefoundation/autoware.universe/issues/591>`_)
  * [autoware_vehicle_rviz_plugin/route_handler/simple_planning_simulator]fix some packages (`#606 <https://github.com/autowarefoundation/autoware.universe/issues/606>`_)
  * fix console meter
  * fix velocity_history
  * fix route handler
  * change topic name
  * adding autoware_auto_perception_rviz_plugin (`#574 <https://github.com/autowarefoundation/autoware.universe/issues/574>`_)
  * [152] Implement BoundingBoxArray rviz display plugin.
  * [285] Clear bounding box markers before adding new markers on new message
  * [274] Trajectory visualization plugin
  * raw types to sized type
  * ControllerTestingNode: added publish_state(), publish_trajectory(), no timer hack to start test, all init is init()
  * Squashed 'src/external/mpc/' changes from 8fc7cfdd..eaa5908b
  eaa5908b Merge branch 'input-weight-modify' into 'master'
  b9ee8e4f Update default mpc_controller_node parameters
  8d15f49d Add weights to acceleration and steer controls; loosen simulation test case:
  git-subtree-dir: src/external/mpc
  git-subtree-split: eaa5908bdd987051a9dcd9c505f99bfd7f028547
  * [`#404 <https://github.com/autowarefoundation/autoware.universe/issues/404>`_] apply ament_auto macro to autoware_rviz_plugins
  * Adding missing dependency on rviz2.
  * Squashed 'src/external/autoware_auto_msgs/' changes from 56550efd..f40970ea
  f40970ea Adding velocity_mps to VehicleControlCommand.
  git-subtree-dir: src/external/autoware_auto_msgs
  git-subtree-split: f40970ead34d36a695b432dc37accff9d67c17e2
  * Update copyright headers to transfer ownership to Autoware Foundation
  * Add CHANGELOG and update package versions for release
  Add CHANGELOG and update package versions for release
  * [`#286 <https://github.com/autowarefoundation/autoware.universe/issues/286>`_] Parameterize boundingbox colors from rviz
  - Add visualization colours via Qt
  * [`#813 <https://github.com/autowarefoundation/autoware.universe/issues/813>`_] use autoware_set_compile_options() for nearly all compiled tests
  - fix a few causes of warnings and disable warning flags as needed for
  other tests
  - set CXX_STANDARD strictly and only in a single place
  - add CMake flag `AUTOWARE_OPTIMIZATION_OF_SLOW_TARGETS`. Default: OFF
  - update building instructions and MR template
  - fix nasty initialization error of static constexpr member in `GenericState`
  of Kalman filter
  * [`#910 <https://github.com/autowarefoundation/autoware.universe/issues/910>`_] remove private compilation warning ignore flags
  * [`#900 <https://github.com/autowarefoundation/autoware.universe/issues/900>`_] Implement rviz plugin to visualize TrackedObjects
  * [`#1110 <https://github.com/autowarefoundation/autoware.universe/issues/1110>`_] Implement rviz plugin for DetectedObjects msg
  * Resolve "Clarify meaning of pose in *ObjectKinematics messages"
  * [`#1221 <https://github.com/autowarefoundation/autoware.universe/issues/1221>`_] Add co-developed entry to copyright
  * [`#1282 <https://github.com/autowarefoundation/autoware.universe/issues/1282>`_] Fix double free in ObjectPolygonDisplayBase rviz plugin
  * [`#1355 <https://github.com/autowarefoundation/autoware.universe/issues/1355>`_] Make DetectedObject shape corners be in object-local coordinates
  * porting AAP perception visualization from https://github.com/tier4/AutowareArchitectureProposal.iv/blob/main/perception/util/visualizer/dynamic_object_visualization/include/dynamic_object_visualization/dynamic_object_visualizer.hpp
  * rename to autoware_auto_perception_rviz_plugin
  * fix copyright
  * format code
  * fix typo
  * convert camel to snake case
  * Apply suggestions from code review
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * change color
  * replace boost::optional to std::optional
  * add icons
  * set marker id to value corresponding to the upper 32 bits of uuid
  * format code
  * change motorsycle color
  * add uuid map to set marker id
  * format code
  * remove brief comment
  * fix lint error
  * fix include sort
  * format code
  * fix include
  * add autoware_auto_perception_rviz_plugin to pre commit excluded list
  Co-authored-by: Yunus Emre Caliskan <yunus.ec@gmail.com>
  Co-authored-by: Christopher Ho <christopher.ho@apex.ai>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: Jit Ray Chowdhury <jit.ray.c@gmail.com>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Juan Pablo Samper <jp.samper@apex.ai>
  Co-authored-by: Jilada Eccleston <jilada.eccleston@tier4.jp>
  Co-authored-by: Frederik Beaujean <Frederik.Beaujean@apex.ai>
  Co-authored-by: Vincent Richard <vincent.francois.richard@gmail.com>
  Co-authored-by: Gowtham <gowtham.ranganathan@apex.ai>
  Co-authored-by: Nikolai Morin <nikolai.morin@apex.ai>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * add autoware_auto_perception_rviz_plugin README (`#631 <https://github.com/autowarefoundation/autoware.universe/issues/631>`_)
  * fix readme sentence grammar (`#634 <https://github.com/autowarefoundation/autoware.universe/issues/634>`_)
  * Auto/fix perception viz (`#639 <https://github.com/autowarefoundation/autoware.universe/issues/639>`_)
  * add ns of uuid
  * remove dynamic_object_visualization
  * update to support velocity report header (`#655 <https://github.com/autowarefoundation/autoware.universe/issues/655>`_)
  * update to support velocity report header
  * Update simulator/simple_planning_simulator/src/simple_planning_simulator/simple_planning_simulator_core.cpp
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * use maybe_unused
  * fix precommit
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * adapt to actuation cmd/status as control msg (`#646 <https://github.com/autowarefoundation/autoware.universe/issues/646>`_)
  * adapt to actuation cmd/status as control msg
  * fix readme
  * fix topics
  * fix remaing topics
  * as to pacmod interface
  * fix vehicle status
  * add header to twist
  * revert gyro_odometer_change
  * revert twist topic change
  * revert unchanged package
  * FIx vehicle status topic name/type (`#658 <https://github.com/autowarefoundation/autoware.universe/issues/658>`_)
  * shift -> gear_status
  * twist -> velocity_status
  * Sync .auto branch with the latest branch in internal repository (`#691 <https://github.com/autowarefoundation/autoware.universe/issues/691>`_)
  * add trajectory point offset in rviz plugin (`#2270 <https://github.com/autowarefoundation/autoware.universe/issues/2270>`_)
  * sync rc rc/v0.23.0 (`#2258 <https://github.com/autowarefoundation/autoware.universe/issues/2258>`_)
  * fix interpolation for insert point (`#2228 <https://github.com/autowarefoundation/autoware.universe/issues/2228>`_)
  * fix interpolation for insert point
  * to prev interpolation pkg
  * Revert "to prev interpolation pkg"
  This reverts commit 9eb145b5d36e297186015fb17c267ccd5b3c21ef.
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: taikitanaka <ttatcoder@outlook.jp>
  * fix topic name (`#2266 <https://github.com/autowarefoundation/autoware.universe/issues/2266>`_)
  * Add namespace to diag for dual_return_filter (`#2269 <https://github.com/autowarefoundation/autoware.universe/issues/2269>`_)
  * Add a function to make 'geometry_msgs::msg::TransformStamped' (`#2250 <https://github.com/autowarefoundation/autoware.universe/issues/2250>`_)
  * Add a function to make 'geometry_msgs::msg::TransformStamped'
  * Add 'child_frame_id' as an argument of 'pose2transform'
  * Simplify marker scale initialization (`#2286 <https://github.com/autowarefoundation/autoware.universe/issues/2286>`_)
  * Fix/crosswalk polygon (`#2279 <https://github.com/autowarefoundation/autoware.universe/issues/2279>`_)
  * extend crosswalk polygon
  * improve readability
  * fix polygon shape
  * Add warning when decel distance calculation fails (`#2289 <https://github.com/autowarefoundation/autoware.universe/issues/2289>`_)
  * [motion_velocity_smoother] ignore debug print (`#2292 <https://github.com/autowarefoundation/autoware.universe/issues/2292>`_)
  * cosmetic change
  * cahnge severity from WARN to DEBUG for debug info
  * use util for stop_watch
  * fix map based prediction (`#2200 <https://github.com/autowarefoundation/autoware.universe/issues/2200>`_)
  * fix map based prediction
  * fix format
  * change map based prediction
  * fix spells
  * fix spells in comments
  * fix for cpplint
  * fix some problems
  * fix format and code for clang-tidy
  * fix space for cpplint
  * Update Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  * fix vector access method
  * fix readme format
  * add parameter
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update Readme.md
  * Update perception/object_recognition/prediction/map_based_prediction/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * remove failure condition for 0 velocity trajectory (`#2295 <https://github.com/autowarefoundation/autoware.universe/issues/2295>`_)
  * [mpc_follower] remove stop distance condition from stopState decision (`#1916 <https://github.com/autowarefoundation/autoware.universe/issues/1916>`_)
  * [mpc_follower] remove stop distance condition from stopState decision
  * add invalid index handling
  * Move the debug marker initialization part to another file (`#2288 <https://github.com/autowarefoundation/autoware.universe/issues/2288>`_)
  * Move the debug marker initialization part to 'debug.cpp'
  * Make 'isLocalOptimalSolutionOscillation' independent from 'NDTScanMatcher' (`#2300 <https://github.com/autowarefoundation/autoware.universe/issues/2300>`_)
  * Remove an unused function 'getTransform' (`#2301 <https://github.com/autowarefoundation/autoware.universe/issues/2301>`_)
  * Simplify iteration of initial poses (`#2310 <https://github.com/autowarefoundation/autoware.universe/issues/2310>`_)
  * Make a transform object const (`#2311 <https://github.com/autowarefoundation/autoware.universe/issues/2311>`_)
  * Represent poses in 'std::vector' instead of 'geometry_msgs::msg::PoseArray' (`#2312 <https://github.com/autowarefoundation/autoware.universe/issues/2312>`_)
  * Feature/no stopping area (`#2163 <https://github.com/autowarefoundation/autoware.universe/issues/2163>`_)
  * add no stopping area module to behavior velocity planner
  * apply utils
  * add polygon interpolation module order stopline around area is considered
  * devide jpass udge with stop line polygon
  * update docs
  * rename file name
  * update to latest
  * minor change for marker
  * update license
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * update license
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * update license
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * update license
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * minor fix
  * add parameter tuning at experiment
  * update readme
  * format doc
  * apply comments
  * add exception gurd
  * cosmetic change
  * fix ament
  * fix typo and remove for statement
  * & to " "
  * better ns
  * return pass judge param
  * add missing stoppable condition
  * add clear pass judge and stoppable flag
  * add comment
  * precommit fix
  * cpplint
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * sync rc rc/v0.23.0 (`#2281 <https://github.com/autowarefoundation/autoware.universe/issues/2281>`_)
  * Fix side shift planner (`#2171 <https://github.com/autowarefoundation/autoware.universe/issues/2171>`_) (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * add print debug
  * remove forward shift points when adding new point
  * remove debug print
  * format
  * Fix remove threshold
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Fix/pull out and pull over (`#2175 <https://github.com/autowarefoundation/autoware.universe/issues/2175>`_)
  * delete unnecessary check
  * fix condition of starting pull out
  * Add emergency status API (`#2174 <https://github.com/autowarefoundation/autoware.universe/issues/2174>`_) (`#2182 <https://github.com/autowarefoundation/autoware.universe/issues/2182>`_)
  * Fix/mpc reset prev result (`#2185 <https://github.com/autowarefoundation/autoware.universe/issues/2185>`_) (`#2195 <https://github.com/autowarefoundation/autoware.universe/issues/2195>`_)
  * reset prev result
  * clean code
  * reset only raw_steer_cmd
  * Update control/mpc_follower/src/mpc_follower_core.cpp
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * [hotfix] 1 path point exception after resampling (`#2204 <https://github.com/autowarefoundation/autoware.universe/issues/2204>`_)
  * fix 1 path point exception after resampling
  * Apply suggestions from code review
  * Apply suggestions from code review
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * [hotfix] Fix lane ids (`#2211 <https://github.com/autowarefoundation/autoware.universe/issues/2211>`_)
  * Fix lane ids
  * Prevent acceleration on avoidance (`#2214 <https://github.com/autowarefoundation/autoware.universe/issues/2214>`_)
  * prevent acceleration on avoidance
  * fix param name
  * parametrize avoidance acc
  * change param name
  * fix typo
  * Fix qos in roi cluster fusion (`#2218 <https://github.com/autowarefoundation/autoware.universe/issues/2218>`_)
  * fix confidence (`#2220 <https://github.com/autowarefoundation/autoware.universe/issues/2220>`_)
  * too high confidence (`#2229 <https://github.com/autowarefoundation/autoware.universe/issues/2229>`_)
  * Fix/obstacle stop 0.23.0 (`#2232 <https://github.com/autowarefoundation/autoware.universe/issues/2232>`_)
  * fix unexpected slow down in sharp curves (`#2181 <https://github.com/autowarefoundation/autoware.universe/issues/2181>`_)
  * Fix/insert implementation (`#2186 <https://github.com/autowarefoundation/autoware.universe/issues/2186>`_)
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  * [hotfix] Remove exception in avoidance module (`#2233 <https://github.com/autowarefoundation/autoware.universe/issues/2233>`_)
  * Remove exception
  * Fix clock
  * Remove blank line
  * Update traffic light state if ref stop point is ahead of previous one (`#2197 <https://github.com/autowarefoundation/autoware.universe/issues/2197>`_)
  * fix interpolation for insert point (`#2228 <https://github.com/autowarefoundation/autoware.universe/issues/2228>`_)
  * fix interpolation for insert point
  * to prev interpolation pkg
  * fix index (`#2265 <https://github.com/autowarefoundation/autoware.universe/issues/2265>`_)
  * turn signal calculation (`#2280 <https://github.com/autowarefoundation/autoware.universe/issues/2280>`_)
  * add turn signal funtion in path shifter
  * add ros parameters
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Sugatyon <32741405+Sugatyon@users.noreply.github.com>
  * [behavior_path_planner] fix sudden path change around ego (`#2305 <https://github.com/autowarefoundation/autoware.universe/issues/2305>`_) (`#2318 <https://github.com/autowarefoundation/autoware.universe/issues/2318>`_)
  * fix return-from-ego shift point generation logic
  * change param for trimSimilarGradShiftPoint
  * add comment for issue
  * update comment
  * replace code with function (logic has not changed)
  * move func to cpp
  * add comment for issue
  * fix typo
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/src/scene_module/avoidance/avoidance_module.cpp
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/src/scene_module/avoidance/avoidance_module.cpp
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Add functions to make stamped scalar messages (`#2317 <https://github.com/autowarefoundation/autoware.universe/issues/2317>`_)
  * Fix/object yaw in intersection module (`#2294 <https://github.com/autowarefoundation/autoware.universe/issues/2294>`_)
  * fix object orientation
  * fix function name
  * add guard (`#2321 <https://github.com/autowarefoundation/autoware.universe/issues/2321>`_)
  * reduce cost (double to float) (`#2298 <https://github.com/autowarefoundation/autoware.universe/issues/2298>`_)
  * Add detail collision check (`#2274 <https://github.com/autowarefoundation/autoware.universe/issues/2274>`_)
  * Add detail collision check
  * Remove unused function
  * Fix arc length
  * Seperate time margin
  * Fix parameter name
  * Update Readme
  * Address review: Add comment for TimeDistanceArray
  * Run pre-commit
  * Fix cpplint
  * Add return for empty polygon
  * update CenterPoint  (`#2222 <https://github.com/autowarefoundation/autoware.universe/issues/2222>`_)
  * update to model trained by mmdet3d
  * add vizualizer (debug)
  * for multi-frame inputs
  * chagne config
  * use autoware_utils::pi
  * project specific model and param
  * rename vfe -> encoder
  * rename general to common
  * update download link
  * update
  * fix
  * rename model_name
  * change training toolbox link
  * chage lint package
  * fix test error
  * commit suggestion
  * Feature/lane change detection (`#2331 <https://github.com/autowarefoundation/autoware.universe/issues/2331>`_)
  * add old information deleter
  * fix access bug
  * change to deque
  * update obstacle buffer
  * fix some bugs
  * add lane change detector
  * make a update lanelet function
  * fix code style
  * parameterize essential values
  * Update perception/object_recognition/prediction/map_based_prediction/src/map_based_prediction_ros.cpp
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * fix slash position
  * remove unnecessary lines
  * fix format
  * fix format
  * change to new enum
  * fix format
  * fix typo and add guard
  * change funciton name
  * add lane change description
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Add Planning Evaluator  (`#2293 <https://github.com/autowarefoundation/autoware.universe/issues/2293>`_)
  * Add prototype planning evaluator
  Produced data for dist between points, curvature, and relative angle
  * Cleanup the code to make adding metrics easier
  * Add remaining basic metrics (length, duration, vel, accel, jerk)
  * Add motion_evaluator to evaluate the actual ego motion + code cleanup
  * Add deviation metrics
  * Add naive stability metric
  * Handle invalid stat (TODO: fix the output file formatting)
  * Add parameter file and cleanup
  * Add basic obstacle metric (TTC not yet implemented) and fix output file format
  * Add basic time to collision
  * Add lateral-distance based stability metric
  * Add check (at init time) that metrics' maps are complete
  * Publish metrics as ParamaterDeclaration msg (for openscenario)
  * Use lookahead and start from ego_pose when calculating stability metrics
  * Code cleanup
  * Fix lint
  * Add tests
  * Fix bug with Frechet dist and the last traj point
  * Finish implementing tests
  * Fix lint
  * Code cleanup
  * Update README.md
  * Remove unused metric
  * Change msg type of published metrics to DiagnosticArray
  * fix format to fix pre-commit check
  * fix yaml format to fix pre-commit check
  * fix yaml format
  * apply clang-format
  * apply clang-format
  * Update planning/planning_diagnostics/planning_evaluator/include/planning_evaluator/planning_evaluator_node.hpp
  * Update planning/planning_diagnostics/planning_evaluator/test/test_planning_evaluator_node.cpp
  * Update planning/planning_diagnostics/planning_evaluator/test/test_planning_evaluator_node.cpp
  * change lint format to autoware_lint_common
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Add keep braking function at driving state (`#2346 <https://github.com/autowarefoundation/autoware.universe/issues/2346>`_)
  * Add keep braking function at driving state
  * Remove debug messages
  * Fix format
  * Change diag_updater's pediod from default to 0.1sec (`#2348 <https://github.com/autowarefoundation/autoware.universe/issues/2348>`_)
  * add cross judgement and common signal function (`#2319 <https://github.com/autowarefoundation/autoware.universe/issues/2319>`_)
  * merge branch turn_signal_common
  * add turn signal function in signal decider
  * add cross judge in path_utilities and delete from turn_signal_decider
  * remove original signal calculation in lane change
  * omit substitution
  * replace turn signal decider in pull over function
  * modify cross judge logic
  * replace turn signal decider in avoidance
  * add readme of turn signal
  * update
  * delete print debug
  * update
  * delete lane change decider in path shifter
  * delete blank line
  * fix indent
  * fix typo
  * fix typo
  * decrease nest
  * run pre commit
  * Add 0 limit at forward jerk velocity filter (`#2340 <https://github.com/autowarefoundation/autoware.universe/issues/2340>`_)
  * add time offset param to point cloud concatenation (`#2303 <https://github.com/autowarefoundation/autoware.universe/issues/2303>`_)
  * add offset param
  * clang-format
  Co-authored-by: Akihito OHSATO <aohsato@gmail.com>
  * Feature/add doc for keep braking function at driving state (`#2366 <https://github.com/autowarefoundation/autoware.universe/issues/2366>`_)
  * Add the description of brake keeping
  * Add the english document
  * Improve description
  * Add english description
  * Fix include files (`#2339 <https://github.com/autowarefoundation/autoware.universe/issues/2339>`_)
  * fix behavior intersection module
  * fix behavior no stopping area module
  * fix planning_evaluator
  * fix motion_velocity_smoother
  * rename variable
  * Revert "[mpc_follower] remove stop distance condition from stopState decision (`#1916 <https://github.com/autowarefoundation/autoware.universe/issues/1916>`_)"
  This reverts commit ff4f0b5a844d1f835f1b93bd3b36a76747b0cd02.
  * Revert "Add keep braking function at driving state (`#2346 <https://github.com/autowarefoundation/autoware.universe/issues/2346>`_)"
  This reverts commit f0478187db4c28bf6092c198723dcc5ec11a9c70.
  * Revert "Feature/add doc for keep braking function at driving state (`#2366 <https://github.com/autowarefoundation/autoware.universe/issues/2366>`_)"
  This reverts commit 66de2f3924a479049fce2d5c5c6b579cacbd3e49.
  * Fix orientation availability in centerpoint
  * fix test_trajectory.cpp
  * add target link libraries
  * Use .auto msg in test code for planniing evaluator
  * fix include
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: autoware-iv-sync-ci[bot] <87871706+autoware-iv-sync-ci[bot]@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: taikitanaka <ttatcoder@outlook.jp>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: Takeshi Ishita <ishitah.takeshi@gmail.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Makoto Kurihara <mkuri8m@gmail.com>
  Co-authored-by: purewater0901 <43805014+purewater0901@users.noreply.github.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Sugatyon <32741405+Sugatyon@users.noreply.github.com>
  Co-authored-by: s-murakami-esol <81723883+s-murakami-esol@users.noreply.github.com>
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: Shinnosuke Hirakawa <8327162+0x126@users.noreply.github.com>
  Co-authored-by: Akihito OHSATO <aohsato@gmail.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * [autoware_auto_perception_rviz_plugin]fix bug (`#721 <https://github.com/autowarefoundation/autoware.universe/issues/721>`_)
  * fix perception_marker
  * fix missing commit
  * apply format
  * patch for PR721 (`#722 <https://github.com/autowarefoundation/autoware.universe/issues/722>`_)
  * fix id_map erase operation
  * fix code to use c++11 function
  * update tracked_objects_display
  * fix bug
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  * fix rviz plugin (`#743 <https://github.com/autowarefoundation/autoware.universe/issues/743>`_)
  * move plugin packages
  * add ignore file to apply pre-commit
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: Servando <43142004+sgermanserrano@users.noreply.github.com>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: nik-tier4 <71747268+nik-tier4@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  Co-authored-by: Keisuke Shima <keisuke.shima@tier4.jp>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: kyoichi sugahara <81.s.kyo.19@gmail.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Yunus Emre Caliskan <yunus.ec@gmail.com>
  Co-authored-by: Christopher Ho <christopher.ho@apex.ai>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: Jit Ray Chowdhury <jit.ray.c@gmail.com>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Juan Pablo Samper <jp.samper@apex.ai>
  Co-authored-by: Jilada Eccleston <jilada.eccleston@tier4.jp>
  Co-authored-by: Frederik Beaujean <Frederik.Beaujean@apex.ai>
  Co-authored-by: Vincent Richard <vincent.francois.richard@gmail.com>
  Co-authored-by: Gowtham <gowtham.ranganathan@apex.ai>
  Co-authored-by: Nikolai Morin <nikolai.morin@apex.ai>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: autoware-iv-sync-ci[bot] <87871706+autoware-iv-sync-ci[bot]@users.noreply.github.com>
  Co-authored-by: taikitanaka <ttatcoder@outlook.jp>
  Co-authored-by: Takeshi Ishita <ishitah.takeshi@gmail.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Makoto Kurihara <mkuri8m@gmail.com>
  Co-authored-by: purewater0901 <43805014+purewater0901@users.noreply.github.com>
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: Sugatyon <32741405+Sugatyon@users.noreply.github.com>
  Co-authored-by: s-murakami-esol <81723883+s-murakami-esol@users.noreply.github.com>
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: Shinnosuke Hirakawa <8327162+0x126@users.noreply.github.com>
  Co-authored-by: Akihito OHSATO <aohsato@gmail.com>
* Contributors: Tomoya Kimura

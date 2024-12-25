^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_path_static_obstacle_avoidance_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* feat(behavior_path_planner): add detail text to virutal wall (`#9600 <https://github.com/autowarefoundation/autoware.universe/issues/9600>`_)
  * feat(behavior_path_planner): add detail text to virutal wall
  * goal is far
  * pull over start pose is far
  * fix lc build
  * fix build
  * Update planning/behavior_path_planner/autoware_behavior_path_goal_planner_module/src/goal_planner_module.cpp
  ---------
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(autoware_behavior_path_static_obstacle_avoidance_module): add maintainer (`#9581 <https://github.com/autowarefoundation/autoware.universe/issues/9581>`_)
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* fix(avoidance): remove stop reason (`#9364 <https://github.com/autowarefoundation/autoware.universe/issues/9364>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* perf(static_obstacle_avoidance): use lanelet::utils instead of route handle for closest lanelet (`#9367 <https://github.com/autowarefoundation/autoware.universe/issues/9367>`_)
  use lanelet::utils for performance improvement
* perf(static_obstacle_avoidance): remove redundant calculation related to lanelet functions (`#9355 <https://github.com/autowarefoundation/autoware.universe/issues/9355>`_)
  * add traffic light distance and modified goal allowance to avoidance data
  * add closest lanelet related variable to avoidanceData structure
  * use route handler for checking closest lanelet
  * use std::optional
  ---------
* feat(avoidance): output velocity factor (`#9345 <https://github.com/autowarefoundation/autoware.universe/issues/9345>`_)
* fix(static_obstacle_avoidance): override setInitState (`#9340 <https://github.com/autowarefoundation/autoware.universe/issues/9340>`_)
  override setInitState
* refactor(bpp): rework steering factor interface (`#9325 <https://github.com/autowarefoundation/autoware.universe/issues/9325>`_)
  * refactor(bpp): rework steering factor interface
  * refactor(soa): rework steering factor interface
  * refactor(AbLC): rework steering factor interface
  * refactor(doa): rework steering factor interface
  * refactor(lc): rework steering factor interface
  * refactor(gp): rework steering factor interface
  * refactor(sp): rework steering factor interface
  * refactor(sbp): rework steering factor interface
  * refactor(ss): rework steering factor interface
  ---------
* refactor(static obstacle avoidance): remove redundant calculation (`#9326 <https://github.com/autowarefoundation/autoware.universe/issues/9326>`_)
  * refactor bases on clang tidy
  * refactor extend backward length
  * mover redundant calculation in getRoadShoulderDistance
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* test(bpp_common): add unit test for safety check (`#9223 <https://github.com/autowarefoundation/autoware.universe/issues/9223>`_)
  * add test for object collision
  * add test for more functions
  * add docstring
  * fix lane change
  ---------
* feat(static_obstacle_avoidance): operator request for ambiguous vehicle (`#9205 <https://github.com/autowarefoundation/autoware.universe/issues/9205>`_)
  * add operator request feature
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/src/scene.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* Contributors: Esteve Fernandez, Fumiya Watanabe, Go Sakayori, Kosuke Takeuchi, M. Fatih Cırıt, Ryohsuke Mitsudome, Satoshi OTA, Yukinari Hisaki, Yutaka Kondo

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
* test(bpp_common): add unit test for safety check (`#9223 <https://github.com/autowarefoundation/autoware.universe/issues/9223>`_)
  * add test for object collision
  * add test for more functions
  * add docstring
  * fix lane change
  ---------
* feat(static_obstacle_avoidance): operator request for ambiguous vehicle (`#9205 <https://github.com/autowarefoundation/autoware.universe/issues/9205>`_)
  * add operator request feature
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/src/scene.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* Contributors: Esteve Fernandez, Go Sakayori, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(behavior_path_planner, behavior_velocity_planner): fix to not read invalid ID (`#9103 <https://github.com/autowarefoundation/autoware.universe/issues/9103>`_)
  * fix(behavior_path_planner, behavior_velocity_planner): fix to not read invalid ID
  * style(pre-commit): autofix
  * fix typo
  * fix(behavior_path_planner, behavior_velocity_planner): fix typo and indentation
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(static_obstacle_avoidance): suppress unnecessary warning (`#9142 <https://github.com/autowarefoundation/autoware.universe/issues/9142>`_)
* test(static_obstacle_avoidance): add unit test for utils functions (`#9134 <https://github.com/autowarefoundation/autoware.universe/issues/9134>`_)
  * docs(static_obstacle_avoidance): add doxygen
  * test: add test
  * fix: assert and expect
  * fix: wrong comment
  * refactor: use autoware test utils
  ---------
* fix(utils): fix envelope polygon update logic (`#9123 <https://github.com/autowarefoundation/autoware.universe/issues/9123>`_)
* test(bpp_common): add test for path utils (`#9122 <https://github.com/autowarefoundation/autoware.universe/issues/9122>`_)
  * add test file for path utils
  * fix
  * add tests for map irrelevant function
  * add test for getUnshiftedEgoPose
  * add docstring and remove unneccesary function
  ---------
* fix(avoidance): don't insert stop line if the ego can't avoid or return (`#9089 <https://github.com/autowarefoundation/autoware.universe/issues/9089>`_)
  * fix(avoidance): don't insert stop line if the ego can't avoid or return
  * fix: build error
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/include/autoware/behavior_path_static_obstacle_avoidance_module/helper.hpp
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  ---------
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
* refactor(bpp_common, motion_utils): move path shifter util functions to autoware::motion_utils (`#9081 <https://github.com/autowarefoundation/autoware.universe/issues/9081>`_)
  * remove unused function
  * mover path shifter utils function to autoware motion utils
  * minor change in license header
  * fix warning message
  * remove header file
  ---------
* refactor(bpp): simplify ExtendedPredictedObject and add new member variables (`#8889 <https://github.com/autowarefoundation/autoware.universe/issues/8889>`_)
  * simplify ExtendedPredictedObject and add new member variables
  * replace self polygon to initial polygon
  * comment
  * add comments to dist of ego
  ---------
* refactor(static_obstacle_avoidance): move route handler based calculation outside loop (`#8968 <https://github.com/autowarefoundation/autoware.universe/issues/8968>`_)
  * refactor filterTargetObjects
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/src/utils.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* fix(static_obstacle_avoidance): remove redundant calculation (`#8955 <https://github.com/autowarefoundation/autoware.universe/issues/8955>`_)
  remove redundant calculation
* refactor(signal_processing): prefix package and namespace with autoware (`#8541 <https://github.com/autowarefoundation/autoware.universe/issues/8541>`_)
* fix(static_obstacle_avoidance, avoidance_by_lane_change): remove unused variable (`#8926 <https://github.com/autowarefoundation/autoware.universe/issues/8926>`_)
  remove unused variables
* fix(static_obstacle_avoidance): update UUID when candidate shift is empty (`#8901 <https://github.com/autowarefoundation/autoware.universe/issues/8901>`_)
  fix candidate shift line's rtc cooperate status
* docs(static_obstacle_avoidance): update envelope polygon creation (`#8822 <https://github.com/autowarefoundation/autoware.universe/issues/8822>`_)
  * update envelope polygon creation
  * fix whitespace
  ---------
* fix(autoware_behavior_path_planner): align the parameters with launcher (`#8790 <https://github.com/autowarefoundation/autoware.universe/issues/8790>`_)
  parameters in behavior_path_planner aligned
* fix(static_obstacle_avoidance): improve turn signal output timing when the ego returns original lane (`#8726 <https://github.com/autowarefoundation/autoware.universe/issues/8726>`_)
  fix(static_obstacle_avoidance): fix unexpected turn signal output
* docs(static_obstacle_avoidance): light edits. Typos, grammar fixes (`#8759 <https://github.com/autowarefoundation/autoware.universe/issues/8759>`_)
  * Light edit: Typos, grammar fixes. Additional changes to follow
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/README.md
  Paragraph revised to correct typos
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/README.md
  Paragraph revised to correct typos
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  * fix typo in avoidance.png
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/README.md
  * fix pre-commit
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/README.md
  ---------
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  Co-authored-by: Go Sakayori <gsakayori@gmail.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* fix(autoware_behavior_path_static_obstacle_avoidance_module): fix unusedFunction (`#8776 <https://github.com/autowarefoundation/autoware.universe/issues/8776>`_)
  fix:unusedFunction
* fix(static_obstacle_avoidance): ignore objects which has already been decided to avoid (`#8754 <https://github.com/autowarefoundation/autoware.universe/issues/8754>`_)
* fix(autoware_behavior_path_static_obstacle_avoidance_module): fix unusedFunction (`#8732 <https://github.com/autowarefoundation/autoware.universe/issues/8732>`_)
  fix:unusedFunction
* fix(static_obstacle_avoidance): change implementation the logic to remove invalid small shift lines (`#8721 <https://github.com/autowarefoundation/autoware.universe/issues/8721>`_)
  * Revert "fix(static_obstacle_avoidance): remove invalid small shift lines (`#8344 <https://github.com/autowarefoundation/autoware.universe/issues/8344>`_)"
  This reverts commit 2705a63817f02ecfa705960459438763225ea6cf.
  * fix(static_obstacle_avoidance): remove invalid small shift lines
  ---------
* fix(static_obstacle_avoidance): use wrong parameter (`#8720 <https://github.com/autowarefoundation/autoware.universe/issues/8720>`_)
* fix(bpp): use common steering factor interface for same scene modules (`#8675 <https://github.com/autowarefoundation/autoware.universe/issues/8675>`_)
* fix(autoware_behavior_path_static_obstacle_avoidance_module): fix unusedFunction (`#8664 <https://github.com/autowarefoundation/autoware.universe/issues/8664>`_)
  fix:unusedFunction
* feat(static_obstacle_avoidance): update envelope polygon creation method (`#8551 <https://github.com/autowarefoundation/autoware.universe/issues/8551>`_)
  * update envelope polygon by considering pose covariance
  * change parameter
  * modify schema json
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/src/utils.cpp
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  ---------
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* fix(static_obstacle_avoidance): target object sorting (`#8545 <https://github.com/autowarefoundation/autoware.universe/issues/8545>`_)
  * fix compensateLostTargetObjects function
  * remove empty case
  ---------
* docs(static_obstacle_avoidance): add FAQ section in document (`#8514 <https://github.com/autowarefoundation/autoware.universe/issues/8514>`_)
  * add FAQ section in readme
  * refer to FAQ before detail
  * fix
  ---------
* fix(static_obstacle_avoidance): change avoidance condition (`#8433 <https://github.com/autowarefoundation/autoware.universe/issues/8433>`_)
* perf(static_obstacle_avoidance): improve logic to reduce computational cost (`#8432 <https://github.com/autowarefoundation/autoware.universe/issues/8432>`_)
  * perf(safety_check): check within first
  * perf(static_obstacle_avoidance): remove duplicated process
  * perf(static_obstacle_avoidance): remove heavy process
  ---------
* fix(static_obstacle_avoidance): check opposite lane (`#8345 <https://github.com/autowarefoundation/autoware.universe/issues/8345>`_)
* fix(static_obstacle_avoidance): remove invalid small shift lines (`#8344 <https://github.com/autowarefoundation/autoware.universe/issues/8344>`_)
* feat(static_obstacle_avoidance): force deactivation (`#8288 <https://github.com/autowarefoundation/autoware.universe/issues/8288>`_)
  * add force cancel function
  * fix format
  * fix json schema
  * fix spelling
  * fix
  ---------
* feat(static_obstacle_avoidance): enable force execution under unsafe conditions (`#8094 <https://github.com/autowarefoundation/autoware.universe/issues/8094>`_)
  * add force execution for static obstacle avoidance
  * fix
  * erase unused function in RTC interface
  * refactor with lamda function
  * fix rtc_interface
  * add warn throtthle and move code block
  * fix
  ---------
* fix(autoware_behavior_path_static_obstacle_avoidance_module): fix constParameterReference (`#8046 <https://github.com/autowarefoundation/autoware.universe/issues/8046>`_)
  fix:constParameterReference
* fix(static_obstacle_avoidance): avoid object behind unavoidance object if unavoidable is not on the path (`#8066 <https://github.com/autowarefoundation/autoware.universe/issues/8066>`_)
* feat(static_obstacle_avoidance): integrate time keeper to major functions (`#8044 <https://github.com/autowarefoundation/autoware.universe/issues/8044>`_)
* fix(static_obstacle_avoidance): fix issues in target filtiering logic (`#7954 <https://github.com/autowarefoundation/autoware.universe/issues/7954>`_)
  * fix: unknown filtering flow
  * fix: relax target filtering logic for object which is in freespace
  * docs: update flowchart
  * fix: check stopped time in freespace
  ---------
* feat(static_obstacle_avoidance): show markers when system requests operator support (`#7994 <https://github.com/autowarefoundation/autoware.universe/issues/7994>`_)
* fix(static_obstacle_avoidance): don't automatically avoid ambiguous vehicle (`#7851 <https://github.com/autowarefoundation/autoware.universe/issues/7851>`_)
  * fix(static_obstacle_avoidance): don't automatically avoid ambiguous vehicle
  * chore(schema): update schema
  ---------
* fix(static_obstacle_avoidance): stop position is unstable (`#7880 <https://github.com/autowarefoundation/autoware.universe/issues/7880>`_)
  fix(static_obstacle_avoidance): fix stop position
* fix(static_obstacle_avoidance): ignore pedestrian/cyclist who is not on road edge (`#7850 <https://github.com/autowarefoundation/autoware.universe/issues/7850>`_)
  * fix(static_obstacle_avoidance): ignore pedestrian/cyclist who is not on road edge
  * docs(static_obstacle_avoidance): update flowchart
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/README.md
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
  ---------
  Co-authored-by: Go Sakayori <go-sakayori@users.noreply.github.com>
* refactor(static_avoidance): modify getAdjacentLane function (`#7843 <https://github.com/autowarefoundation/autoware.universe/issues/7843>`_)
  add getLeftOppositeLanelers in getAdjacentLane function
* fix(static_obstacle_avoidance): fix issues in target object filtering logic (`#7830 <https://github.com/autowarefoundation/autoware.universe/issues/7830>`_)
  * fix(static_obstacle_avoidance): check if object is inside/outside by its position point instead of its polygon
  * refactor(static_obstacle_avoidance): add getter functions
  * fix(static_obstacle_avoidance): check next lane without route if the current lane is not preferred
  * fix(static_obstacle_avoidance): fix parked vehicle check
  ---------
* feat(safety_check): filter safety check targe objects by yaw deviation between pose and lane (`#7828 <https://github.com/autowarefoundation/autoware.universe/issues/7828>`_)
  * fix(safety_check): filter by yaw deviation to check object belongs to lane
  * fix(static_obstacle_avoidance): check yaw only when the object is moving
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(static_obstacle_avoidance): organize params for drivable lane (`#7715 <https://github.com/autowarefoundation/autoware.universe/issues/7715>`_)
  * refactor(static_obstacle_avoidance): organize params for drivable lane
  * Update planning/behavior_path_planner/autoware_behavior_path_static_obstacle_avoidance_module/schema/static_obstacle_avoidance.schema.json
  ---------
* feat(start_planner): yaw threshold for rss check (`#7657 <https://github.com/autowarefoundation/autoware.universe/issues/7657>`_)
  * add param to customize yaw th
  * add param to other modules
  * docs
  * update READMEs with params
  * fix LC README
  * use normalized yaw diff
  ---------
* docs(static_obstacle_avoidance): fix wrong flowchart (`#7693 <https://github.com/autowarefoundation/autoware.universe/issues/7693>`_)
* fix(static_obstacle_avoidance): fix json schema (`#7692 <https://github.com/autowarefoundation/autoware.universe/issues/7692>`_)
* refactor(static_obstacle_avoidance): change logger name for utils    (`#7617 <https://github.com/autowarefoundation/autoware.universe/issues/7617>`_)
  change logger name for static avoidance utils
* feat(static_obstacle_avoidance): keep object clipping even after the object becomes non-target (`#7591 <https://github.com/autowarefoundation/autoware.universe/issues/7591>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(autoware_behavior_path_static_obstacle_avoidance_module): fix duplicateCondition warnings (`#7582 <https://github.com/autowarefoundation/autoware.universe/issues/7582>`_)
* docs(bpp_static_obstacle_avoidance): add documentation (`#7554 <https://github.com/autowarefoundation/autoware.universe/issues/7554>`_)
  * fix: package path
  * docs: add explanation of lateral margin
  * fix: typo
  * fix: wrong description
  ---------
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
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
* Contributors: Atto, Esteve Fernandez, Go Sakayori, Kosuke Takeuchi, Ryuta Kambe, Satoshi OTA, T-Kimura-MM, Takayuki Murooka, Yutaka Kondo, Zhe Shen, Zulfaqar Azmi, danielsanchezaran, kobayu858

0.26.0 (2024-04-03)
-------------------

^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_path_avoidance_by_lane_change_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
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
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Satoshi OTA, Yutaka Kondo

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
* chore(avoidance_by_lane_change, external_request_lane_change): add maintainers (`#9027 <https://github.com/autowarefoundation/autoware.universe/issues/9027>`_)
  * add maintainers to avoidance by lane change
  * add maintainers to external request lane change
  ---------
* fix(autoware_behavior_path_avoidance_by_lane_change_module): fix unmatchedSuppression (`#8987 <https://github.com/autowarefoundation/autoware.universe/issues/8987>`_)
  fix:unmatchedSuppression
* refactor(lane_change): add TransientData to store commonly used lane change-related variables. (`#8954 <https://github.com/autowarefoundation/autoware.universe/issues/8954>`_)
  * add transient data
  * reverted max lc dist in  calcCurrentMinMax
  * rename
  * minor refactoring
  * update doxygen comments
  ---------
* feat(lane_change): modify lane change target boundary check to consider velocity (`#8961 <https://github.com/autowarefoundation/autoware.universe/issues/8961>`_)
  * check if candidate path footprint exceeds target lane boundary when lc velocity is above minimum
  * move functions to relevant module
  * suppress unused function cppcheck
  * minor change
  ---------
* fix(static_obstacle_avoidance, avoidance_by_lane_change): remove unused variable (`#8926 <https://github.com/autowarefoundation/autoware.universe/issues/8926>`_)
  remove unused variables
* feat(lane_change): improve execution condition of lane change module (`#8648 <https://github.com/autowarefoundation/autoware.universe/issues/8648>`_)
  * refactor lane change utility funcions
  * LC utility function to get distance to next regulatory element
  * don't activate LC module when close to regulatory element
  * modify threshold distance calculation
  * move regulatory element check to canTransitFailureState() function
  * always run LC module if approaching terminal point
  * use max possible LC length as threshold
  * update LC readme
  * refactor implementation
  * update readme
  * check distance to reg element for candidate path only if not near terminal start
  ---------
* fix(bpp): use common steering factor interface for same scene modules (`#8675 <https://github.com/autowarefoundation/autoware.universe/issues/8675>`_)
* refactor(lane_change): update lanes and its polygons only  when it's updated (`#7989 <https://github.com/autowarefoundation/autoware.universe/issues/7989>`_)
  * refactor(lane_change): compute lanes and polygon only when updated
  * Revert accidental changesd
  This reverts commit cbfd9ae8a88b2d6c3b27b35c9a08bb824ecd5011.
  * fix spell check
  * Make a common getter for current lanes
  * add target lanes getter
  * some minor function refactoring
  ---------
* fix(static_obstacle_avoidance): don't automatically avoid ambiguous vehicle (`#7851 <https://github.com/autowarefoundation/autoware.universe/issues/7851>`_)
  * fix(static_obstacle_avoidance): don't automatically avoid ambiguous vehicle
  * chore(schema): update schema
  ---------
* fix(static_obstacle_avoidance): stop position is unstable (`#7880 <https://github.com/autowarefoundation/autoware.universe/issues/7880>`_)
  fix(static_obstacle_avoidance): fix stop position
* feat(safety_check): filter safety check targe objects by yaw deviation between pose and lane (`#7828 <https://github.com/autowarefoundation/autoware.universe/issues/7828>`_)
  * fix(safety_check): filter by yaw deviation to check object belongs to lane
  * fix(static_obstacle_avoidance): check yaw only when the object is moving
  ---------
* fix(behavior_path_planner, behavior_velocity_planner): fix redefinition errors (`#7688 <https://github.com/autowarefoundation/autoware.universe/issues/7688>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
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
* Contributors: Go Sakayori, Kosuke Takeuchi, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, Zulfaqar Azmi, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------

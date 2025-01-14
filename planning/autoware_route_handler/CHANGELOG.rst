^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_route_handler
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(test_utils): return parser as optional (`#9391 <https://github.com/autowarefoundation/autoware.universe/issues/9391>`_)
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, Zulfaqar Azmi

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
* fix(autoware_route_handler): fix cppcheck unusedVariable (`#9191 <https://github.com/autowarefoundation/autoware.universe/issues/9191>`_)
* feat(mission_planner): reroute with current route start pose when triggered by modifed goal (`#9136 <https://github.com/autowarefoundation/autoware.universe/issues/9136>`_)
  * feat(mission_planner): reroute with current route start pose when triggered by modifed goal
  * check new ego goal is in original preffered lane as much as possible
  * check goal is in goal_lane
  ---------
* feat(autoware_test_utils): add path with lane id parser (`#9098 <https://github.com/autowarefoundation/autoware.universe/issues/9098>`_)
  * add path with lane id parser
  * refactor parse to use template
  ---------
* refactor(lane_change): refactor getLaneChangePaths function (`#8909 <https://github.com/autowarefoundation/autoware.universe/issues/8909>`_)
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
  * refactor checking data validity
  * refactor sampling of prepare phase metrics and lane changing phase metrics
  * add route handler function to get pose from 2d arc length
  * refactor candidate path generation
  * refactor candidate path safety check
  * fix variable name
  * Update planning/autoware_route_handler/src/route_handler.cpp
  Co-authored-by: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
  * correct parameter name
  * set prepare segment velocity after taking max path velocity value
  * update LC README
  * minor changes
  * check phase length difference with previos valid candidate path
  * change logger name
  * change functions names to snake case
  * use snake case for function names
  * add colors to flow chart in README
  ---------
  Co-authored-by: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
* test(autoware_route_handler): add unit test for autoware route handler function (`#8271 <https://github.com/autowarefoundation/autoware.universe/issues/8271>`_)
  * remove unused functions in route handler
  * add docstring for function getShoulderLaneletsAtPose
  * update test map to include shoulder lanelet
  * add unit test for function getShoulderLaneletsAtPose
  * add test case for getCenterLinePath to improve branch coverage
  ---------
* fix(route_handler): make new route continuous with previous route (`#8238 <https://github.com/autowarefoundation/autoware.universe/issues/8238>`_)
* feat(map_loader, route_handler)!: add format_version validation (`#7074 <https://github.com/autowarefoundation/autoware.universe/issues/7074>`_)
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat(route_handler): add unit test for lane change related functions (`#7504 <https://github.com/autowarefoundation/autoware.universe/issues/7504>`_)
  * RT1-6230 feat(route_handler): add unit test for lane change related functions
  * fix spell check
  * fix spellcheck
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(route_handler)!: rename to include/autoware/{package_name}  (`#7530 <https://github.com/autowarefoundation/autoware.universe/issues/7530>`_)
  refactor(route_handler)!: rename to include/autoware/{package_name}
* chore(route_handler, lane_change): add codeowner (`#7475 <https://github.com/autowarefoundation/autoware.universe/issues/7475>`_)
* fix(route_handler): route handler overlap removal is too conservative (`#7156 <https://github.com/autowarefoundation/autoware.universe/issues/7156>`_)
  * add flag to enable/disable loop check in getLaneletSequence functions
  * implement function to get closest route lanelet based on previous closest lanelet
  * refactor DefaultPlanner::plan function
  * modify loop check logic in getLaneletSequenceUpTo function
  * improve logic in isEgoOutOfRoute function
  * fix format
  * check if prev lanelet is a goal lanelet in getLaneletSequenceUpTo function
  * separate function to update current route lanelet in planner manager
  * rename function and add docstring
  * modify functions extendNextLane and extendPrevLane to account for overlap
  * refactor function getClosestRouteLaneletFromLanelet
  * add route handler unit tests for overlapping route case
  * fix function getClosestRouteLaneletFromLanelet
  * format fix
  * move test map to autoware_test_utils
  ---------
* refactor(test_utils): move to common folder (`#7158 <https://github.com/autowarefoundation/autoware.universe/issues/7158>`_)
  * Move autoware planning test manager to autoware namespace
  * fix package share directory for behavior path planner
  * renaming files and directory
  * rename variables that has planning_test_utils in its name.
  * use autoware namespace for test utils
  * move folder to common
  * update .pages file
  * fix test error
  * removed obstacle velocity limiter test artifact
  * remove namespace from planning validator, it has using keyword
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
* Contributors: Fumiya Watanabe, Kosuke Takeuchi, Mamoru Sobue, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, Zulfaqar Azmi, mkquda

0.26.0 (2024-04-03)
-------------------
